#include "DatabaseManager.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <list>

DatabaseManager::DatabaseManager(const std::string& caminhoBanco) 
    : db(nullptr), dbPath(caminhoBanco), connected(false) {
}

DatabaseManager::~DatabaseManager() {
    if (connected) {
        desconectar();
    }
}

bool DatabaseManager::conectar() {
    if (connected) {
        return true;
    }
    
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao conectar ao banco: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    
    connected = true;
    return true;
}

void DatabaseManager::desconectar() {
    if (connected && db) {
        sqlite3_close(db);
        db = nullptr;
        connected = false;
    }
}

bool DatabaseManager::executarSQL(const std::string& sql) {
    if (!connected) {
        return false;
    }
    
    char* errorMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Erro SQL: " << (errorMsg ? errorMsg : "erro desconhecido") << std::endl;
        sqlite3_free(errorMsg);
        return false;
    }
    
    return true;
}

bool DatabaseManager::inicializarBanco() {
    if (!conectar()) {
        std::cerr << "Erro: Falha ao conectar com o banco de dados!" << std::endl;
        return false;
    }
    
    std::string schema = R"(
        CREATE TABLE IF NOT EXISTS contas (
            cpf TEXT PRIMARY KEY,
            nome TEXT NOT NULL,
            senha TEXT NOT NULL
        );
        
        CREATE TABLE IF NOT EXISTS carteiras (
            codigo TEXT PRIMARY KEY,
            nome TEXT NOT NULL,
            tipo_perfil TEXT NOT NULL,
            cpf_conta TEXT NOT NULL,
            FOREIGN KEY (cpf_conta) REFERENCES contas(cpf)
        );
        
        CREATE TABLE IF NOT EXISTS ordens (
            codigo TEXT PRIMARY KEY,
            codigo_neg TEXT NOT NULL,
            data TEXT NOT NULL,
            valor TEXT NOT NULL,
            quantidade TEXT NOT NULL,
            codigo_carteira TEXT NOT NULL,
            FOREIGN KEY (codigo_carteira) REFERENCES carteiras(codigo)
        );
    )";
    
    return executarSQL(schema);
}

bool DatabaseManager::inserirConta(const Conta& conta) {
    if (!connected) {
        return false;
    }
    
    if (sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string sql = "INSERT INTO contas (cpf, nome, senha) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    // Capturar os valores em variáveis locais para evitar problemas de referência temporária
    std::string cpfValor = conta.getNcpf().getValor();
    std::string nomeValor = conta.getNome().getValor();
    std::string senhaValor = conta.getSenha().getValor();
    
    sqlite3_bind_text(stmt, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, nomeValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, senhaValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    sqlite3_finalize(stmt);
    
    if (sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    return true;
}

bool DatabaseManager::buscarConta(const Ncpf& cpf, Conta* conta) {
    if (!connected || !conta) {
        return false;
    }
    
    std::string sql = "SELECT cpf, nome, senha FROM contas WHERE cpf = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string cpfValor = cpf.getValor();
    sqlite3_bind_text(stmt, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
    
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        try {
            Ncpf cpfResult;
            Nome nomeResult;
            Senha senhaResult;
            
            cpfResult.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            nomeResult.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            senhaResult.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            
            conta->setNcpf(cpfResult);
            conta->setNome(nomeResult);
            conta->setSenha(senhaResult);
            
            found = true;
        } catch (const std::exception& e) {
            std::cerr << "Erro ao criar conta: " << e.what() << std::endl;
        }
    }
    
    sqlite3_finalize(stmt);
    return found;
}

bool DatabaseManager::autenticarUsuario(const Ncpf& cpf, const Senha& senha) {
    if (!connected) {
        return false;
    }
    
    std::string sql = "SELECT COUNT(*) FROM contas WHERE cpf = ? AND senha = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    // Capturar os valores em variáveis locais para evitar problemas de referência temporária
    std::string cpfValor = cpf.getValor();
    std::string senhaValor = senha.getValor();
    
    sqlite3_bind_text(stmt, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, senhaValor.c_str(), -1, SQLITE_STATIC);
    
    bool authenticated = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        authenticated = count > 0;
    }
    
    sqlite3_finalize(stmt);
    return authenticated;
}

bool DatabaseManager::inserirCarteira(const Carteira& carteira, const Ncpf& cpfProprietario) {
    if (!connected) {
        return false;
    }
    
    std::string sql = "INSERT INTO carteiras (codigo, nome, tipo_perfil, cpf_conta) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    // Capturar valores em variáveis locais para evitar problemas de memória
    std::string codigoValor = carteira.getCodigo().getValor();
    std::string nomeValor = carteira.getNome().getValor();
    std::string tipoPerfilValor = carteira.getTipoPerfil().getValor();
    std::string cpfValor = cpfProprietario.getValor();
    
    sqlite3_bind_text(stmt, 1, codigoValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, nomeValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, tipoPerfilValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, cpfValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool DatabaseManager::listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) {
    if (!connected || !listaCarteiras) {
        return false;
    }
    
    std::string sql = "SELECT codigo, nome, tipo_perfil FROM carteiras WHERE cpf_conta = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string cpfValor = cpf.getValor();
    sqlite3_bind_text(stmt, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
    
    listaCarteiras->clear();
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        try {
            Carteira carteira;
            Codigo codigo;
            Nome nome;
            TipoPerfil tipoPerfil;
            
            codigo.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            nome.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            tipoPerfil.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            
            carteira.setCodigo(codigo);
            carteira.setNome(nome);
            carteira.setTipoPerfil(tipoPerfil);
            
            listaCarteiras->push_back(carteira);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao criar carteira: " << e.what() << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::buscarCarteira(const Codigo& codigo, Carteira* carteira) {
    if (!connected || !carteira) {
        return false;
    }
    
    std::string sql = "SELECT codigo, nome, tipo_perfil FROM carteiras WHERE codigo = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string codigoValor = codigo.getValor();
    sqlite3_bind_text(stmt, 1, codigoValor.c_str(), -1, SQLITE_STATIC);
    
    bool found = false;
    int stepResult = sqlite3_step(stmt);
    
    if (stepResult == SQLITE_ROW) {
        try {
            Codigo codigoResult;
            Nome nomeResult;
            TipoPerfil tipoPerfilResult;
            
            const char* codigoStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* nomeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* tipoPerfilStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            
            codigoResult.setValor(codigoStr);
            nomeResult.setValor(nomeStr);
            tipoPerfilResult.setValor(tipoPerfilStr);
            
            carteira->setCodigo(codigoResult);
            carteira->setNome(nomeResult);
            carteira->setTipoPerfil(tipoPerfilResult);
            
            found = true;
        } catch (const std::exception& e) {
            std::cerr << "Erro ao criar carteira: " << e.what() << std::endl;
        }
    }
    
    sqlite3_finalize(stmt);
    return found;
}

bool DatabaseManager::inserirOrdem(const Ordem& ordem, const Codigo& codigoCarteira) {
    if (!connected) {
        return false;
    }
    
    std::string sql = "INSERT INTO ordens (codigo, codigo_neg, data, valor, quantidade, codigo_carteira) VALUES (?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    // Capturar valores em variáveis locais para evitar problemas de memória
    std::string codigoValor = ordem.getCodigo().getValor();
    std::string codigoNegValor = ordem.getCodigoNeg().getValor();
    std::string dataValor = ordem.getData().getValor();
    std::string dinheiroValor = ordem.getDinheiro().getValor();
    std::string quantidadeValor = ordem.getQuantidade().getValor();
    std::string codigoCarteiraValor = codigoCarteira.getValor();
    
    sqlite3_bind_text(stmt, 1, codigoValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, codigoNegValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dataValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, dinheiroValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, quantidadeValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, codigoCarteiraValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool DatabaseManager::listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) {
    if (!connected || !listaOrdens) {
        return false;
    }
    
    std::string sql = "SELECT codigo, codigo_neg, data, valor, quantidade FROM ordens WHERE codigo_carteira = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string codigoCarteiraValor = codigoCarteira.getValor();
    sqlite3_bind_text(stmt, 1, codigoCarteiraValor.c_str(), -1, SQLITE_STATIC);
    
    listaOrdens->clear();
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        try {
            Ordem ordem;
            Codigo codigo;
            CodigoNeg codigoNeg;
            Data data;
            Dinheiro valor;
            Quantidade quantidade;
            
            codigo.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            codigoNeg.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            data.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            valor.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            quantidade.setValor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            
            ordem.setCodigo(codigo);
            ordem.setCodigoNeg(codigoNeg);
            ordem.setData(data);
            ordem.setDinheiro(valor);
            ordem.setQuantidade(quantidade);
            
            listaOrdens->push_back(ordem);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao criar ordem: " << e.what() << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::excluirOrdem(const Codigo& codigo) {
    if (!connected) {
        return false;
    }
    
    std::string sql = "DELETE FROM ordens WHERE codigo = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string codigoValor = codigo.getValor();
    sqlite3_bind_text(stmt, 1, codigoValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db) > 0;
}

bool DatabaseManager::excluirCarteira(const Codigo& codigo) {
    if (!connected) {
        return false;
    }
    
    std::string sql = "DELETE FROM carteiras WHERE codigo = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string codigoValor = codigo.getValor();
    sqlite3_bind_text(stmt, 1, codigoValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db) > 0;
}

bool DatabaseManager::calcularSaldoCarteira(const Codigo& codigoCarteira, Dinheiro* saldo) {
    if (!connected || !saldo) {
        return false;
    }
    
    std::list<Ordem> ordens;
    bool resultadoListar = listarOrdens(codigoCarteira, &ordens);
    
    // Se não conseguiu listar por erro do banco, retorna false
    if (!resultadoListar) {
        return false;
    }
    
    // Se não há ordens (lista vazia), isso é normal e saldo deve ser 0,01 (mínimo permitido pela classe Dinheiro)
    if (ordens.empty()) {
        try {
            saldo->setValor("0,01");
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    double valorTotal = 0.0;
    for (const auto& ordem : ordens) {
        std::string valorStr = ordem.getDinheiro().getValor();
        // Remove "R$ " e substitui vírgula por ponto
        if (valorStr.size() > 3) {
            valorStr.erase(0, 3); // Remove "R$ "
        }
        std::replace(valorStr.begin(), valorStr.end(), ',', '.');
        valorTotal += std::stod(valorStr);
    }
    
    // Converter de volta para formato brasileiro
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << valorTotal;
    std::string valorFormatado = oss.str();
    std::replace(valorFormatado.begin(), valorFormatado.end(), '.', ',');
    
    try {
        saldo->setValor(valorFormatado);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Implementações simples para métodos não utilizados ainda
bool DatabaseManager::atualizarConta(const Conta& conta) {
    if (!connected) {
        return false;
    }
    
    if (sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        return false;
    }
    
    std::string sql = "UPDATE contas SET nome = ?, senha = ? WHERE cpf = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    // Capturar os valores em variáveis locais para evitar problemas de referência temporária
    std::string nomeValor = conta.getNome().getValor();
    std::string senhaValor = conta.getSenha().getValor();
    std::string cpfValor = conta.getNcpf().getValor();
    
    sqlite3_bind_text(stmt, 1, nomeValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, senhaValor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, cpfValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    sqlite3_finalize(stmt);
    
    if (sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    return sqlite3_changes(db) > 0;
}
bool DatabaseManager::excluirConta(const Ncpf& cpf) {
    if (!connected) {
        return false;
    }
    
    if (sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        return false;
    }
    
    // Primeiro exclui todas as ordens das carteiras do usuário
    std::string sqlOrdens = "DELETE FROM ordens WHERE codigo_carteira IN (SELECT codigo FROM carteiras WHERE cpf_conta = ?)";
    sqlite3_stmt* stmtOrdens;
    
    if (sqlite3_prepare_v2(db, sqlOrdens.c_str(), -1, &stmtOrdens, nullptr) == SQLITE_OK) {
        std::string cpfValor = cpf.getValor();
        sqlite3_bind_text(stmtOrdens, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmtOrdens);
        sqlite3_finalize(stmtOrdens);
    }
    
    // Depois exclui todas as carteiras do usuário
    std::string sqlCarteiras = "DELETE FROM carteiras WHERE cpf_conta = ?";
    sqlite3_stmt* stmtCarteiras;
    
    if (sqlite3_prepare_v2(db, sqlCarteiras.c_str(), -1, &stmtCarteiras, nullptr) == SQLITE_OK) {
        std::string cpfValor = cpf.getValor();
        sqlite3_bind_text(stmtCarteiras, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmtCarteiras);
        sqlite3_finalize(stmtCarteiras);
    }
    
    // Por fim exclui a conta
    std::string sql = "DELETE FROM contas WHERE cpf = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    std::string cpfValor = cpf.getValor();
    sqlite3_bind_text(stmt, 1, cpfValor.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    if (sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
    
    return sqlite3_changes(db) > 0;
}
bool DatabaseManager::atualizarCarteira(const Carteira& carteira) { return false; }
bool DatabaseManager::buscarOrdem(const Codigo& codigo, Ordem* ordem) { return false; }

bool DatabaseManager::prepararStatement(const std::string& sql, sqlite3_stmt** stmt) {
    return sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, nullptr) == SQLITE_OK;
}

void DatabaseManager::finalizarStatement(sqlite3_stmt* stmt) {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

std::string DatabaseManager::escaparString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '\'') {
            escaped += "''";
        } else {
            escaped += c;
        }
    }
    return escaped;
}

std::string DatabaseManager::obterEstatisticas() {
    if (!connected) {
        return "Não conectado ao banco";
    }
    
    std::ostringstream stats;
    stats << "=== ESTATÍSTICAS DO BANCO ===" << std::endl;
    stats << "Banco SQLite conectado" << std::endl;
    stats << "Arquivo: " << dbPath << std::endl;
    
    return stats.str();
}

bool DatabaseManager::limparTodasTabelas() {
    if (!connected) {
        return false;
    }
    
    std::string sql = "DELETE FROM ordens; DELETE FROM carteiras; DELETE FROM contas;";
    return executarSQL(sql);
}
