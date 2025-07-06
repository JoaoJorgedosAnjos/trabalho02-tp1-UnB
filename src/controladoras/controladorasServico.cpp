#include "controladorasServico.hpp"
#include "../database/DatabaseManager.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

// Função utilitária para remover espaços em branco do início e fim de uma string
std::string trim(const std::string& str) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t first = str.find_first_not_of(WHITESPACE);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(WHITESPACE);
    return str.substr(first, (last - first + 1));
}

// =================================================================================================
// IMPLEMENTAÇÃO DA CONTROLADORA COM SQLITE
// =================================================================================================

ControladoraServico::ControladoraServico() {
    dbManager = std::make_unique<DatabaseManager>("../database/sistema_investimentos.db");
}

ControladoraServico::~ControladoraServico() {
    // DatabaseManager se auto-desconecta no destrutor
}

bool ControladoraServico::inicializar() {
    if (!dbManager->conectar()) {
        std::cerr << "Erro: Não foi possível conectar ao banco de dados!" << std::endl;
        return false;
    }
    
    if (!dbManager->inicializarBanco()) {
        std::cerr << "Erro: Não foi possível inicializar o banco de dados!" << std::endl;
        return false;
    }
    

    return true;
}

// =================================================================================================
// MÉTODOS DE AUTENTICAÇÃO
// =================================================================================================

bool ControladoraServico::autenticar(const Ncpf& cpf, const Senha& senha) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->autenticarUsuario(cpf, senha);
}

// =================================================================================================
// MÉTODOS DE USUÁRIO
// =================================================================================================

bool ControladoraServico::cadastrarConta(const Conta& conta) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    

    
    // Verifica se a conta já existe
    Conta contaExistente;
    if (dbManager->buscarConta(conta.getNcpf(), &contaExistente)) {
        std::cout << "Erro: Conta com este CPF já existe!" << std::endl;
        return false;
    }
    
    return dbManager->inserirConta(conta);
}

bool ControladoraServico::consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) {
    if (!dbManager->estaConectado() || !conta || !saldo) {
        return false;
    }
    
    // Busca a conta
    if (!dbManager->buscarConta(cpf, conta)) {
        return false;
    }
    
    // Calcula saldo total das carteiras do usuário
    std::list<Carteira> carteiras;
    if (!dbManager->listarCarteiras(cpf, &carteiras)) {
        // Se não tem carteiras, saldo é zero
        try {
            saldo->setValor("0,01");
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    long long saldoTotalCentavos = 0;
    
    for (const auto& carteira : carteiras) {
        Dinheiro saldoCarteira;
        if (dbManager->calcularSaldoCarteira(carteira.getCodigo(), &saldoCarteira)) {
            try {
                long long saldoCarteiraCentavos = DatabaseManager::dinheiroParaCentavos(saldoCarteira);
                saldoTotalCentavos += saldoCarteiraCentavos;
            } catch (const std::exception& e) {
                // Ignora erros de conversão e continua
            }
        }
    }
    
    try {
        std::string saldoFormatado = DatabaseManager::centavosParaDinheiro(saldoTotalCentavos);
        saldo->setValor(saldoFormatado);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ControladoraServico::editarConta(const Conta& conta) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->atualizarConta(conta);
}

bool ControladoraServico::excluirConta(const Ncpf& cpf) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirConta(cpf);
}

// =================================================================================================
// MÉTODOS DE INVESTIMENTO
// =================================================================================================

bool ControladoraServico::criarCarteira(const Ncpf& cpf, const Carteira& carteira) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    // Verifica se a conta existe
    Conta conta;
    if (!dbManager->buscarConta(cpf, &conta)) {
        std::cout << "Erro: Conta não encontrada!" << std::endl;
        return false;
    }
    
    // Verifica se já existe carteira com este código
    Carteira carteiraExistente;
    if (dbManager->buscarCarteira(carteira.getCodigo(), &carteiraExistente)) {
        std::cout << "Erro: Já existe uma carteira com este código!" << std::endl;
        return false;
    }
    
    return dbManager->inserirCarteira(carteira, cpf);
}

bool ControladoraServico::listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) {
    if (!dbManager->estaConectado() || !listaCarteiras) {
        return false;
    }
    
    return dbManager->listarCarteiras(cpf, listaCarteiras);
}

bool ControladoraServico::consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) {
    if (!dbManager->estaConectado() || !carteira || !saldo) {
        return false;
    }
    
    // Busca a carteira
    if (!dbManager->buscarCarteira(codigo, carteira)) {
        return false;
    }
    
    // Calcula o saldo da carteira
    return dbManager->calcularSaldoCarteira(codigo, saldo);
}

bool ControladoraServico::editarCarteira(const Carteira& carteira) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->atualizarCarteira(carteira);
}

bool ControladoraServico::excluirCarteira(const Codigo& codigo) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirCarteira(codigo);
}

bool ControladoraServico::criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    // Verifica se a carteira existe
    Carteira carteira;
    if (!dbManager->buscarCarteira(codigoCarteira, &carteira)) {
        std::cout << "Erro: Carteira não encontrada!" << std::endl;
        return false;
    }
    
    // Verifica se já existe ordem com este código
    Ordem ordemExistente;
    if (dbManager->buscarOrdem(ordem.getCodigo(), &ordemExistente)) {
        std::cout << "Erro: Já existe uma ordem com este código!" << std::endl;
        return false;
    }

    // --- NOVA LÓGICA DE LEITURA DO ARQUIVO DE DADOS HISTÓRICOS (PADRÃO PROFESSOR) ---
    std::ifstream arquivo("../data/DADOS_HISTORICOS.txt");
    if (!arquivo.is_open()) {
        std::cout << "Erro: Não foi possível abrir o arquivo ../data/DADOS_HISTORICOS.txt!" << std::endl;
        return false;
    }

    std::string linha;
    double precoMedioArquivo = 0.0;
    bool encontrouPapel = false;

    // Preparar strings para comparação (remover espaços em branco)
    std::string codigoNegociacao = trim(ordem.getCodigoNeg().getValor());
    std::string dataNegociacao = trim(ordem.getData().getValor());

    while (std::getline(arquivo, linha)) {
        // Garante que a linha tem o comprimento mínimo para evitar erros de substr
        if (linha.length() < 126) {
            continue;
        }

        // Extrai os dados da linha usando substr e "limpa" eles
        std::string dataArquivo = trim(linha.substr(2, 8));
        std::string codigoPapelArquivo = trim(linha.substr(12, 12));
        
        // Compara os dados limpos
        if (codigoPapelArquivo == codigoNegociacao && dataArquivo == dataNegociacao) {
            try {
                // Pega a string do preço, converte para número e divide por 100
                std::string precoStr = linha.substr(113, 13);
                precoMedioArquivo = std::stod(precoStr) / 100.0;
                encontrouPapel = true;
                break; // Encontrou, pode sair do loop
            } catch (const std::exception& e) {
                std::cerr << "Erro ao converter preco da linha: " << linha << std::endl;
                encontrouPapel = false;
                break;
            }
        }
    }
    arquivo.close();

    if (!encontrouPapel) {
        std::cout << "Erro: Papel ou data não encontrados no arquivo de dados históricos!" << std::endl;
        return false;
    }

    // --- CÁLCULO DO PREÇO E CRIAÇÃO DA ORDEM ---
    try {
        // Converter quantidade para número
        std::string quantidadeStr = ordem.getQuantidade().getValor();
        std::string quantidadeLimpa;
        for (char c : quantidadeStr) {
            if (c != '.') {
                quantidadeLimpa += c;
            }
        }
        long long quantidade = std::stoll(quantidadeLimpa);

        // Calcula preço final da ordem
        double precoFinal = precoMedioArquivo * quantidade;

        // Converte para string no formato brasileiro (ex: 1234.56 -> "1.234,56")
        std::ostringstream oss;
        oss.precision(2);
        oss << std::fixed << precoFinal;
        std::string precoFinalStr = oss.str();
        
        // Troca ponto por vírgula para formato brasileiro
        size_t ponto = precoFinalStr.find('.');
        if (ponto != std::string::npos) {
            precoFinalStr[ponto] = ',';
        }
        
        // Adiciona separadores de milhar (pontos) no formato brasileiro
        // Ex: "40469,00" -> "40.469,00"
        if (precoFinalStr.length() > 6) { // Se tem mais de 6 caracteres (incluindo vírgula e decimais)
            std::string parteInteira = precoFinalStr.substr(0, ponto);
            std::string parteDecimal = precoFinalStr.substr(ponto);
            
            // Adiciona pontos a cada 3 dígitos da direita para a esquerda
            for (int i = parteInteira.length() - 3; i > 0; i -= 3) {
                parteInteira.insert(i, ".");
            }
            
            precoFinalStr = parteInteira + parteDecimal;
        }

        // Criar nova ordem com o preço calculado
        Ordem novaOrdem = ordem; // Copia todos os dados
        Dinheiro precoFinalObj;
        precoFinalObj.setValor(precoFinalStr);
        novaOrdem.setDinheiro(precoFinalObj);

        // Salvar a ordem no banco de dados
        return dbManager->inserirOrdem(novaOrdem, codigoCarteira);
    } catch (const std::exception& e) {
        std::cout << "Erro no cálculo do preço: " << e.what() << std::endl;
        return false;
    }
}

bool ControladoraServico::listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) {
    if (!dbManager->estaConectado() || !listaOrdens) {
        return false;
    }
    
    return dbManager->listarOrdens(codigoCarteira, listaOrdens);
}

bool ControladoraServico::excluirOrdem(const Codigo& codigo) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirOrdem(codigo);
}
