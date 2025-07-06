/**
 * @file controladorasServico.cpp
 * @brief Implementação das controladoras de serviço do sistema de investimentos
 * @details Este arquivo contém a implementação das classes responsáveis pela camada de serviço,
 *          incluindo todas as operações de negócio relacionadas a contas, carteiras e ordens.
 *          A camada de serviço atua como intermediária entre a apresentação e o banco de dados,
 *          implementando validações de negócio e cálculos específicos do domínio.
 * @author Sistema de Investimentos
 * @version 1.0
 * @date 2024
 * @see controladorasServico.hpp
 * @see DatabaseManager.hpp
 */

#include "controladorasServico.hpp"
#include "../database/DatabaseManager.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

/**
 * @brief Remove espaços em branco do início e fim de uma string
 * @param str String a ser processada
 * @return String sem espaços em branco nas extremidades
 * @details Função utilitária para limpeza de strings, removendo caracteres de espaçamento
 *          incluindo espaços, quebras de linha, tabs e outros caracteres de formatação.
 */
std::string trim(const std::string& str) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t first = str.find_first_not_of(WHITESPACE);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(WHITESPACE);
    return str.substr(first, (last - first + 1));
}

/**
 * @brief Construtor da controladora de serviço
 * @details Inicializa o gerenciador de banco de dados com o caminho padrão do arquivo SQLite.
 *          Utiliza smart pointer para gerenciamento automático de memória.
 */
ControladoraServico::ControladoraServico() {
    dbManager = std::make_unique<DatabaseManager>("../database/sistema_investimentos.db");
}

/**
 * @brief Destrutor da controladora de serviço
 * @details Destrutor padrão que garante a limpeza automática dos recursos através do smart pointer.
 */
ControladoraServico::~ControladoraServico() {
}

/**
 * @brief Inicializa o sistema de banco de dados
 * @return true se a inicialização foi bem-sucedida, false caso contrário
 * @details Estabelece conexão com o banco de dados e executa a inicialização das tabelas.
 *          Este método deve ser chamado antes de qualquer operação no sistema.
 * @see DatabaseManager::conectar()
 * @see DatabaseManager::inicializarBanco()
 */
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

/**
 * @brief Autentica um usuário no sistema
 * @param cpf CPF do usuário para autenticação
 * @param senha Senha do usuário para autenticação
 * @return true se a autenticação foi bem-sucedida, false caso contrário
 * @details Verifica se as credenciais fornecidas correspondem a um usuário válido no sistema.
 *          Requer conexão ativa com o banco de dados.
 * @see DatabaseManager::autenticarUsuario()
 */
bool ControladoraServico::autenticar(const Ncpf& cpf, const Senha& senha) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->autenticarUsuario(cpf, senha);
}

/**
 * @brief Cadastra uma nova conta no sistema
 * @param conta Objeto conta com os dados a serem cadastrados
 * @return true se o cadastro foi bem-sucedido, false caso contrário
 * @details Verifica se já existe uma conta com o mesmo CPF antes de inserir.
 *          Implementa validação de unicidade do CPF no sistema.
 * @see DatabaseManager::buscarConta()
 * @see DatabaseManager::inserirConta()
 */
bool ControladoraServico::cadastrarConta(const Conta& conta) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    Conta contaExistente;
    if (dbManager->buscarConta(conta.getNcpf(), &contaExistente)) {
        std::cout << "Erro: Conta com este CPF já existe!" << std::endl;
        return false;
    }
    
    return dbManager->inserirConta(conta);
}

/**
 * @brief Consulta dados de uma conta e calcula saldo total
 * @param cpf CPF da conta a ser consultada
 * @param conta Ponteiro para objeto onde serão armazenados os dados da conta
 * @param saldo Ponteiro para objeto onde será armazenado o saldo total calculado
 * @return true se a consulta foi bem-sucedida, false caso contrário
 * @details Busca os dados da conta e calcula o saldo total somando os saldos de todas as carteiras.
 *          O saldo é calculado em centavos para precisão e convertido para formato monetário.
 * @see DatabaseManager::buscarConta()
 * @see DatabaseManager::listarCarteiras()
 * @see DatabaseManager::calcularSaldoCarteira()
 */
bool ControladoraServico::consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) {
    if (!dbManager->estaConectado() || !conta || !saldo) {
        return false;
    }
    
    if (!dbManager->buscarConta(cpf, conta)) {
        return false;
    }
    
    std::list<Carteira> carteiras;
    if (!dbManager->listarCarteiras(cpf, &carteiras)) {
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

/**
 * @brief Edita os dados de uma conta existente
 * @param conta Objeto conta com os novos dados
 * @return true se a edição foi bem-sucedida, false caso contrário
 * @details Atualiza os dados da conta no banco de dados. O CPF não pode ser alterado.
 * @see DatabaseManager::atualizarConta()
 */
bool ControladoraServico::editarConta(const Conta& conta) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->atualizarConta(conta);
}

/**
 * @brief Exclui uma conta do sistema
 * @param cpf CPF da conta a ser excluída
 * @return true se a exclusão foi bem-sucedida, false caso contrário
 * @details Remove a conta e todos os dados relacionados (carteiras e ordens).
 *          Implementa integridade referencial através de cascata.
 * @see DatabaseManager::excluirConta()
 */
bool ControladoraServico::excluirConta(const Ncpf& cpf) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirConta(cpf);
}

/**
 * @brief Cria uma nova carteira para um usuário
 * @param cpf CPF do usuário proprietário da carteira
 * @param carteira Objeto carteira com os dados a serem cadastrados
 * @return true se a criação foi bem-sucedida, false caso contrário
 * @details Verifica se a conta existe e se o código da carteira é único antes de inserir.
 *          Implementa validações de integridade referencial e unicidade.
 * @see DatabaseManager::buscarConta()
 * @see DatabaseManager::buscarCarteira()
 * @see DatabaseManager::inserirCarteira()
 */
bool ControladoraServico::criarCarteira(const Ncpf& cpf, const Carteira& carteira) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    Conta conta;
    if (!dbManager->buscarConta(cpf, &conta)) {
        std::cout << "Erro: Conta não encontrada!" << std::endl;
        return false;
    }
    
    Carteira carteiraExistente;
    if (dbManager->buscarCarteira(carteira.getCodigo(), &carteiraExistente)) {
        std::cout << "Erro: Já existe uma carteira com este código!" << std::endl;
        return false;
    }
    
    return dbManager->inserirCarteira(carteira, cpf);
}

/**
 * @brief Lista todas as carteiras de um usuário
 * @param cpf CPF do usuário
 * @param listaCarteiras Ponteiro para lista onde serão armazenadas as carteiras
 * @return true se a listagem foi bem-sucedida, false caso contrário
 * @details Recupera todas as carteiras associadas ao CPF fornecido.
 * @see DatabaseManager::listarCarteiras()
 */
bool ControladoraServico::listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) {
    if (!dbManager->estaConectado() || !listaCarteiras) {
        return false;
    }
    
    return dbManager->listarCarteiras(cpf, listaCarteiras);
}

/**
 * @brief Consulta dados de uma carteira e calcula seu saldo
 * @param codigo Código da carteira a ser consultada
 * @param carteira Ponteiro para objeto onde serão armazenados os dados da carteira
 * @param saldo Ponteiro para objeto onde será armazenado o saldo calculado
 * @return true se a consulta foi bem-sucedida, false caso contrário
 * @details Busca os dados da carteira e calcula o saldo baseado nas ordens associadas.
 * @see DatabaseManager::buscarCarteira()
 * @see DatabaseManager::calcularSaldoCarteira()
 */
bool ControladoraServico::consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) {
    if (!dbManager->estaConectado() || !carteira || !saldo) {
        return false;
    }
    
    if (!dbManager->buscarCarteira(codigo, carteira)) {
        return false;
    }
    
    return dbManager->calcularSaldoCarteira(codigo, saldo);
}

/**
 * @brief Edita os dados de uma carteira existente
 * @param carteira Objeto carteira com os novos dados
 * @return true se a edição foi bem-sucedida, false caso contrário
 * @details Atualiza os dados da carteira no banco de dados. O código não pode ser alterado.
 * @see DatabaseManager::atualizarCarteira()
 */
bool ControladoraServico::editarCarteira(const Carteira& carteira) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->atualizarCarteira(carteira);
}

/**
 * @brief Exclui uma carteira do sistema
 * @param codigo Código da carteira a ser excluída
 * @return true se a exclusão foi bem-sucedida, false caso contrário
 * @details Remove a carteira e todas as ordens associadas.
 *          Implementa integridade referencial através de cascata.
 * @see DatabaseManager::excluirCarteira()
 */
bool ControladoraServico::excluirCarteira(const Codigo& codigo) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirCarteira(codigo);
}

/**
 * @brief Cria uma nova ordem de investimento
 * @param codigoCarteira Código da carteira onde a ordem será criada
 * @param ordem Objeto ordem com os dados a serem cadastrados
 * @return true se a criação foi bem-sucedida, false caso contrário
 * @details Processo complexo que inclui:
 *          - Validação da existência da carteira
 *          - Verificação de unicidade do código da ordem
 *          - Consulta ao arquivo de dados históricos para obter preço
 *          - Cálculo do valor total da ordem baseado na quantidade e preço histórico
 *          - Formatação monetária do valor calculado
 *          - Inserção da ordem no banco de dados
 * @see DatabaseManager::buscarCarteira()
 * @see DatabaseManager::buscarOrdem()
 * @see DatabaseManager::inserirOrdem()
 */
bool ControladoraServico::criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    Carteira carteira;
    if (!dbManager->buscarCarteira(codigoCarteira, &carteira)) {
        std::cout << "Erro: Carteira não encontrada!" << std::endl;
        return false;
    }
    
    Ordem ordemExistente;
    if (dbManager->buscarOrdem(ordem.getCodigo(), &ordemExistente)) {
        std::cout << "Erro: Já existe uma ordem com este código!" << std::endl;
        return false;
    }

    std::ifstream arquivo("../data/DADOS_HISTORICOS.txt");
    if (!arquivo.is_open()) {
        std::cout << "Erro: Não foi possível abrir o arquivo ../data/DADOS_HISTORICOS.txt!" << std::endl;
        return false;
    }

    std::string linha;
    double precoMedioArquivo = 0.0;
    bool encontrouPapel = false;

    std::string codigoNegociacao = trim(ordem.getCodigoNeg().getValor());
    std::string dataNegociacao = trim(ordem.getData().getValor());

    while (std::getline(arquivo, linha)) {
        if (linha.length() < 126) {
            continue;
        }

        std::string dataArquivo = trim(linha.substr(2, 8));
        std::string codigoPapelArquivo = trim(linha.substr(12, 12));
        
        if (codigoPapelArquivo == codigoNegociacao && dataArquivo == dataNegociacao) {
            try {
                std::string precoStr = linha.substr(113, 13);
                precoMedioArquivo = std::stod(precoStr) / 100.0;
                encontrouPapel = true;
                break;
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

    try {
        std::string quantidadeStr = ordem.getQuantidade().getValor();
        std::string quantidadeLimpa;
        for (char c : quantidadeStr) {
            if (c != '.') {
                quantidadeLimpa += c;
            }
        }
        long long quantidade = std::stoll(quantidadeLimpa);

        double precoFinal = precoMedioArquivo * quantidade;

        std::ostringstream oss;
        oss.precision(2);
        oss << std::fixed << precoFinal;
        std::string precoFinalStr = oss.str();
        
        size_t ponto = precoFinalStr.find('.');
        if (ponto != std::string::npos) {
            precoFinalStr[ponto] = ',';
            
            std::string parteInteira = precoFinalStr.substr(0, ponto);
            std::string parteDecimal = precoFinalStr.substr(ponto);
            
            for (int i = parteInteira.length() - 3; i > 0; i -= 3) {
                parteInteira.insert(i, ".");
            }
            
            precoFinalStr = parteInteira + parteDecimal;
        }

        Ordem novaOrdem = ordem; 
        Dinheiro precoFinalObj;
        precoFinalObj.setValor(precoFinalStr);
        novaOrdem.setDinheiro(precoFinalObj);

        return dbManager->inserirOrdem(novaOrdem, codigoCarteira);
    } catch (const std::exception& e) {
        std::cout << "Erro no cálculo do preço: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Lista todas as ordens de uma carteira
 * @param codigoCarteira Código da carteira
 * @param listaOrdens Ponteiro para lista onde serão armazenadas as ordens
 * @return true se a listagem foi bem-sucedida, false caso contrário
 * @details Recupera todas as ordens associadas ao código da carteira fornecido.
 * @see DatabaseManager::listarOrdens()
 */
bool ControladoraServico::listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) {
    if (!dbManager->estaConectado() || !listaOrdens) {
        return false;
    }
    
    return dbManager->listarOrdens(codigoCarteira, listaOrdens);
}

/**
 * @brief Exclui uma ordem do sistema
 * @param codigo Código da ordem a ser excluída
 * @return true se a exclusão foi bem-sucedida, false caso contrário
 * @details Remove a ordem do banco de dados. A exclusão afeta o cálculo do saldo da carteira.
 * @see DatabaseManager::excluirOrdem()
 */
bool ControladoraServico::excluirOrdem(const Codigo& codigo) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirOrdem(codigo);
}
