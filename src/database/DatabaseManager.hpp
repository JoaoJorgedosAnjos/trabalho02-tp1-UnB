#ifndef DATABASEMANAGER_HPP_INCLUDED
#define DATABASEMANAGER_HPP_INCLUDED

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <list>
#include "../dominios/dominios.hpp"
#include "../entidades/entidades.hpp"

/**
 * @class DatabaseManager
 * @brief Gerenciador de banco de dados SQLite para o sistema de investimentos
 * @details Responsável por todas as operações de persistência, abstraindo 
 * o acesso ao SQLite e fornecendo métodos específicos para cada entidade.
 */
class DatabaseManager {
private:
    sqlite3* db;
    std::string dbPath;
    bool connected;

    // Métodos auxiliares privados
    bool executarSQL(const std::string& sql);
    bool prepararStatement(const std::string& sql, sqlite3_stmt** stmt);
    void finalizarStatement(sqlite3_stmt* stmt);
    std::string escaparString(const std::string& str);

    // Métodos auxiliares para validação de integridade referencial
    bool carteiraTemOrdens(const Codigo& codigoCarteira);
    bool contaTemCarteiras(const Ncpf& cpf);

public:
    /**
     * @brief Construtor padrão
     * @param caminhoBanco Caminho para o arquivo do banco SQLite
     */
    explicit DatabaseManager(const std::string& caminhoBanco = "investimentos.db");
    
    /**
     * @brief Destrutor - fecha conexão automaticamente
     */
    ~DatabaseManager();

    // ============================================
    // MÉTODOS DE CONEXÃO E INICIALIZAÇÃO
    // ============================================
    
    /**
     * @brief Conecta ao banco de dados SQLite
     * @return true se conectou com sucesso, false caso contrário
     */
    bool conectar();
    
    /**
     * @brief Desconecta do banco de dados
     */
    void desconectar();
    
    /**
     * @brief Inicializa o banco criando as tabelas necessárias
     * @return true se inicializou com sucesso, false caso contrário
     */
    bool inicializarBanco();
    
    /**
     * @brief Verifica se está conectado ao banco
     * @return true se conectado, false caso contrário
     */
    bool estaConectado() const { return connected; }

    // ============================================
    // MÉTODOS PARA CONTAS
    // ============================================
    
    /**
     * @brief Insere uma nova conta no banco
     * @param conta Objeto Conta a ser inserido
     * @return true se inseriu com sucesso, false caso contrário
     */
    bool inserirConta(const Conta& conta);
    
    /**
     * @brief Busca uma conta pelo CPF
     * @param cpf CPF da conta a ser buscada
     * @param conta Ponteiro para objeto onde será armazenada a conta encontrada
     * @return true se encontrou a conta, false caso contrário
     */
    bool buscarConta(const Ncpf& cpf, Conta* conta);
    
    /**
     * @brief Atualiza uma conta existente
     * @param conta Objeto Conta com dados atualizados
     * @return true se atualizou com sucesso, false caso contrário
     */
    bool atualizarConta(const Conta& conta);
    
    /**
     * @brief Exclui uma conta do banco
     * @param cpf CPF da conta a ser excluída
     * @return true se excluiu com sucesso, false caso contrário
     */
    bool excluirConta(const Ncpf& cpf);
    
    /**
     * @brief Autentica um usuário
     * @param cpf CPF do usuário
     * @param senha Senha do usuário
     * @return true se autenticação bem-sucedida, false caso contrário
     */
    bool autenticarUsuario(const Ncpf& cpf, const Senha& senha);

    // ============================================
    // MÉTODOS PARA CARTEIRAS
    // ============================================
    
    /**
     * @brief Insere uma nova carteira no banco
     * @param carteira Objeto Carteira a ser inserido
     * @param cpfProprietario CPF do proprietário da carteira
     * @return true se inseriu com sucesso, false caso contrário
     */
    bool inserirCarteira(const Carteira& carteira, const Ncpf& cpfProprietario);
    
    /**
     * @brief Lista todas as carteiras de um usuário
     * @param cpf CPF do proprietário
     * @param listaCarteiras Ponteiro para lista onde serão armazenadas as carteiras
     * @return true se listou com sucesso, false caso contrário
     */
    bool listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras);
    
    /**
     * @brief Busca uma carteira pelo código
     * @param codigo Código da carteira
     * @param carteira Ponteiro para objeto onde será armazenada a carteira
     * @return true se encontrou a carteira, false caso contrário
     */
    bool buscarCarteira(const Codigo& codigo, Carteira* carteira);
    
    /**
     * @brief Atualiza uma carteira existente
     * @param carteira Objeto Carteira com dados atualizados
     * @return true se atualizou com sucesso, false caso contrário
     */
    bool atualizarCarteira(const Carteira& carteira);
    
    /**
     * @brief Exclui uma carteira do banco
     * @param codigo Código da carteira a ser excluída
     * @return true se excluiu com sucesso, false caso contrário
     */
    bool excluirCarteira(const Codigo& codigo);

    // ============================================
    // MÉTODOS PARA ORDENS
    // ============================================
    
    /**
     * @brief Insere uma nova ordem no banco
     * @param ordem Objeto Ordem a ser inserido
     * @param codigoCarteira Código da carteira proprietária
     * @return true se inseriu com sucesso, false caso contrário
     */
    bool inserirOrdem(const Ordem& ordem, const Codigo& codigoCarteira);
    
    /**
     * @brief Lista todas as ordens de uma carteira
     * @param codigoCarteira Código da carteira
     * @param listaOrdens Ponteiro para lista onde serão armazenadas as ordens
     * @return true se listou com sucesso, false caso contrário
     */
    bool listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens);
    
    /**
     * @brief Busca uma ordem pelo código
     * @param codigo Código da ordem
     * @param ordem Ponteiro para objeto onde será armazenada a ordem
     * @return true se encontrou a ordem, false caso contrário
     */
    bool buscarOrdem(const Codigo& codigo, Ordem* ordem);
    
    /**
     * @brief Exclui uma ordem do banco
     * @param codigo Código da ordem a ser excluída
     * @return true se excluiu com sucesso, false caso contrário
     */
    bool excluirOrdem(const Codigo& codigo);
    
    /**
     * @brief Calcula o saldo total de uma carteira
     * @param codigoCarteira Código da carteira
     * @param saldo Ponteiro para objeto onde será armazenado o saldo
     * @return true se calculou com sucesso, false caso contrário
     */
    bool calcularSaldoCarteira(const Codigo& codigoCarteira, Dinheiro* saldo);

    // ============================================
    // MÉTODOS UTILITÁRIOS
    // ============================================
    
    /**
     * @brief Limpa todas as tabelas (usado para testes)
     * @return true se limpou com sucesso, false caso contrário
     */
    bool limparTodasTabelas();
    
    /**
     * @brief Obtém estatísticas do banco
     * @return string com informações sobre número de registros
     */
    std::string obterEstatisticas();
    
    /**
     * @brief Converte centavos para formato brasileiro de dinheiro
     * @param centavos Valor em centavos
     * @return string formatada no padrão brasileiro (#.###.###,##)
     */
    static std::string centavosParaDinheiro(long long centavos);
    
    /**
     * @brief Converte um objeto Dinheiro para centavos
     * @param dinheiro Objeto Dinheiro no formato brasileiro
     * @return Valor em centavos como long long
     */
    static long long dinheiroParaCentavos(const Dinheiro& dinheiro);
};

#endif // DATABASEMANAGER_HPP_INCLUDED 