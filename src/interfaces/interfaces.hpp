#ifndef INTERFACES_H_INCLUDED
#define INTERFACES_H_INCLUDED

#include "dominios.hpp"
#include "entidades.hpp"

#include <list>
#include <string>

class IServicoAutenticacao;
class IServicoUsuario;
class IServicoInvestimento;

/**
 * @brief Interface para os serviços de autenticação na camada de apresentação.
 * 
 * Esta interface define os contratos para a camada de apresentação relacionada
 * à autenticação de usuários no sistema de investimentos. Responsável por
 * gerenciar a interação com o usuário durante o processo de login.
 * 
 * @see IServicoAutenticacao
 */
class IApresentacaoAutenticacao {
public:
    /**
     * @brief Autentica um usuário no sistema.
     * 
     * Solicita e valida as credenciais do usuário (CPF e senha) através
     * da interface de usuário, delegando a validação para a camada de serviço.
     * 
     * @param[out] cpf Ponteiro para armazenar o CPF do usuário autenticado
     * @return true se a autenticação foi bem-sucedida, false caso contrário
     * 
     * @note O método deve solicitar CPF e senha do usuário via interface
     * @note Em caso de sucesso, o CPF é armazenado no parâmetro de saída
     * @note Em caso de falha, deve exibir mensagem apropriada ao usuário
     */
    virtual bool autenticar(Ncpf* cpf) = 0;
    
    /**
     * @brief Define a controladora de serviço para autenticação.
     * 
     * Estabelece a conexão entre a camada de apresentação e a camada de serviço,
     * permitindo que a apresentação delegue operações de autenticação.
     * 
     * @param[in] cntrServicoAutenticacao Ponteiro para a controladora de serviço
     * 
     * @note Deve ser chamado antes de usar o método autenticar()
     * @note A responsabilidade de gerenciar o ciclo de vida do ponteiro é da classe cliente
     */
    virtual void setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) = 0;
    
    /**
     * @brief Destrutor virtual para permitir herança.
     */
    virtual ~IApresentacaoAutenticacao() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de usuário na camada de apresentação.
 * 
 * Esta interface define os contratos para a camada de apresentação relacionada
 * ao gerenciamento de contas de usuários. Responsável por fornecer interface
 * para cadastro, consulta, edição e exclusão de contas.
 * 
 * @see IServicoUsuario
 */
class IApresentacaoUsuario {
public:
    /**
     * @brief Executa o menu de gerenciamento de usuário.
     * 
     * Apresenta as opções disponíveis para gerenciamento de conta do usuário
     * identificado pelo CPF fornecido, incluindo consulta, edição e exclusão.
     * 
     * @param[in] cpf CPF do usuário cuja conta será gerenciada
     * @return true se a operação foi concluída com sucesso, false caso contrário
     * 
     * @note Deve verificar se a conta existe antes de permitir operações
     * @note Deve solicitar confirmação para operações destrutivas
     * @note Deve exibir mensagens apropriadas para cada operação
     */
    virtual bool executar(const Ncpf& cpf) = 0;
    
    /**
     * @brief Executa o processo de cadastro de nova conta.
     * 
     * Solicita e valida os dados necessários para criação de uma nova conta
     * de usuário, incluindo CPF, nome, senha e tipo de perfil.
     * 
     * @note Deve validar se o CPF já não está cadastrado
     * @note Deve solicitar confirmação da senha
     * @note Deve exibir mensagem de sucesso ou erro apropriada
     * @note Deve limpar a tela após conclusão da operação
     */
    virtual void cadastrar() = 0;
    
    /**
     * @brief Define a controladora de serviço para usuário.
     * 
     * Estabelece a conexão entre a camada de apresentação e a camada de serviço,
     * permitindo que a apresentação delegue operações de gerenciamento de usuário.
     * 
     * @param[in] cntrServicoUsuario Ponteiro para a controladora de serviço
     * 
     * @note Deve ser chamado antes de usar os métodos executar() ou cadastrar()
     * @note A responsabilidade de gerenciar o ciclo de vida do ponteiro é da classe cliente
     */
    virtual void setControladoraServico(IServicoUsuario* cntrServicoUsuario) = 0;
    
    /**
     * @brief Destrutor virtual para permitir herança.
     */
    virtual ~IApresentacaoUsuario() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de investimentos na camada de apresentação.
 * 
 * Esta interface define os contratos para a camada de apresentação relacionada
 * ao gerenciamento de carteiras e ordens de investimento. Responsável por
 * fornecer interface para criação, consulta, edição e exclusão de carteiras
 * e ordens de compra/venda.
 * 
 * @see IServicoInvestimento
 */
class IApresentacaoInvestimento {
public:
    /**
     * @brief Executa o menu de gerenciamento de investimentos.
     * 
     * Apresenta as opções disponíveis para gerenciamento de carteiras e ordens
     * do usuário identificado pelo CPF fornecido, incluindo criação, listagem,
     * consulta, edição e exclusão de carteiras e ordens.
     * 
     * @param[in] cpf CPF do usuário cujos investimentos serão gerenciados
     * 
     * @note Deve verificar se o usuário possui carteiras antes de permitir operações
     * @note Deve solicitar confirmação para operações destrutivas
     * @note Deve exibir mensagens apropriadas para cada operação
     * @note Deve limpar a tela após conclusão das operações
     */
    virtual void executar(const Ncpf& cpf) = 0;
    
    /**
     * @brief Define a controladora de serviço para investimentos.
     * 
     * Estabelece a conexão entre a camada de apresentação e a camada de serviço,
     * permitindo que a apresentação delegue operações de gerenciamento de investimentos.
     * 
     * @param[in] cntrServicoInvestimento Ponteiro para a controladora de serviço
     * 
     * @note Deve ser chamado antes de usar o método executar()
     * @note A responsabilidade de gerenciar o ciclo de vida do ponteiro é da classe cliente
     */
    virtual void setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) = 0;
    
    /**
     * @brief Destrutor virtual para permitir herança.
     */
    virtual ~IApresentacaoInvestimento() {}
};

/**
 * @brief Interface para os serviços de autenticação providos pela camada de serviço.
 * 
 * Esta interface define os contratos para a camada de serviço relacionada
 * à autenticação de usuários. Responsável por validar credenciais e
 * gerenciar o processo de autenticação no sistema.
 * 
 * @see IApresentacaoAutenticacao
 */
class IServicoAutenticacao {
public:
    /**
     * @brief Valida as credenciais de autenticação do usuário.
     * 
     * Verifica se o CPF e senha fornecidos correspondem a uma conta
     * válida no sistema, realizando a validação contra o banco de dados.
     * 
     * @param[in] cpf CPF do usuário a ser autenticado
     * @param[in] senha Senha do usuário a ser validada
     * @return true se as credenciais são válidas, false caso contrário
     * 
     * @note Deve validar o formato do CPF e senha antes da consulta
     * @note Deve realizar consulta segura no banco de dados
     * @note Deve retornar false se a conta não existir ou senha estiver incorreta
     */
    virtual bool autenticar(const Ncpf& cpf, const Senha& senha) = 0;
    
    /**
     * @brief Destrutor virtual para permitir herança.
     */
    virtual ~IServicoAutenticacao() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de usuário providos pela camada de serviço.
 * 
 * Esta interface define os contratos para a camada de serviço relacionada
 * ao gerenciamento de contas de usuários. Responsável por operações de
 * persistência e validação de dados de usuário.
 * 
 * @see IApresentacaoUsuario
 */
class IServicoUsuario {
public:
    /**
     * @brief Cadastra uma nova conta de usuário no sistema.
     * 
     * Valida e persiste os dados da conta fornecida no banco de dados,
     * verificando se o CPF não está duplicado e se todos os dados são válidos.
     * 
     * @param[in] conta Objeto Conta contendo os dados do usuário
     * @return true se o cadastro foi realizado com sucesso, false caso contrário
     * 
     * @note Deve validar todos os campos da conta antes da persistência
     * @note Deve verificar se o CPF já não está cadastrado
     * @note Deve criptografar a senha antes de armazenar
     * @note Deve retornar false se houver erro de validação ou persistência
     */
    virtual bool cadastrarConta(const Conta& conta) = 0;
    
    /**
     * @brief Consulta os dados de uma conta de usuário.
     * 
     * Busca no banco de dados os dados da conta associada ao CPF fornecido,
     * retornando os dados da conta e o saldo atual.
     * 
     * @param[in] cpf CPF do usuário cuja conta será consultada
     * @param[out] conta Ponteiro para armazenar os dados da conta encontrada
     * @param[out] saldo Ponteiro para armazenar o saldo atual da conta
     * @return true se a conta foi encontrada, false caso contrário
     * 
     * @note Deve validar o formato do CPF antes da consulta
     * @note Deve retornar false se a conta não existir
     * @note Os parâmetros de saída só devem ser preenchidos se a conta for encontrada
     */
    virtual bool consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) = 0;
    
    /**
     * @brief Edita os dados de uma conta de usuário existente.
     * 
     * Atualiza os dados da conta no banco de dados, mantendo o CPF inalterado
     * e validando todos os outros campos antes da persistência.
     * 
     * @param[in] conta Objeto Conta contendo os dados atualizados
     * @return true se a edição foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar todos os campos antes da atualização
     * @note Deve verificar se a conta existe antes de tentar editar
     * @note Deve criptografar a senha se ela foi alterada
     * @note Deve retornar false se houver erro de validação ou persistência
     */
    virtual bool editarConta(const Conta& conta) = 0;
    
    /**
     * @brief Exclui uma conta de usuário do sistema.
     * 
     * Remove a conta associada ao CPF fornecido do banco de dados,
     * verificando se não há carteiras associadas antes da exclusão.
     * 
     * @param[in] cpf CPF do usuário cuja conta será excluída
     * @return true se a exclusão foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar o formato do CPF antes da exclusão
     * @note Deve verificar se a conta existe antes de tentar excluir
     * @note Deve verificar se não há carteiras associadas à conta
     * @note Deve retornar false se houver carteiras associadas ou erro de persistência
     */
    virtual bool excluirConta(const Ncpf& cpf) = 0;
    
    /**
     * @brief Destrutor virtual para permitir herança.
     */
    virtual ~IServicoUsuario() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de investimentos providos pela camada de serviço.
 * 
 * Esta interface define os contratos para a camada de serviço relacionada
 * ao gerenciamento de carteiras e ordens de investimento. Responsável por
 * operações de persistência e validação de dados de investimento.
 * 
 * @see IApresentacaoInvestimento
 */
class IServicoInvestimento {
public:
    /**
     * @brief Cria uma nova carteira de investimentos.
     * 
     * Valida e persiste os dados da carteira fornecida no banco de dados,
     * associando-a ao usuário identificado pelo CPF.
     * 
     * @param[in] cpf CPF do usuário proprietário da carteira
     * @param[in] carteira Objeto Carteira contendo os dados da carteira
     * @return true se a carteira foi criada com sucesso, false caso contrário
     * 
     * @note Deve validar todos os campos da carteira antes da persistência
     * @note Deve verificar se o usuário existe antes de criar a carteira
     * @note Deve verificar se o código da carteira não está duplicado
     * @note Deve retornar false se houver erro de validação ou persistência
     */
    virtual bool criarCarteira(const Ncpf& cpf, const Carteira& carteira) = 0;
    
    /**
     * @brief Lista todas as carteiras de um usuário.
     * 
     * Busca no banco de dados todas as carteiras associadas ao CPF fornecido,
     * retornando uma lista com os dados de cada carteira.
     * 
     * @param[in] cpf CPF do usuário cujas carteiras serão listadas
     * @param[out] listaCarteiras Ponteiro para lista que armazenará as carteiras
     * @return true se a consulta foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar o formato do CPF antes da consulta
     * @note Deve retornar lista vazia se o usuário não possuir carteiras
     * @note Deve retornar false apenas em caso de erro de banco de dados
     */
    virtual bool listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) = 0;
    
    /**
     * @brief Consulta os dados de uma carteira específica.
     * 
     * Busca no banco de dados os dados da carteira associada ao código fornecido,
     * retornando os dados da carteira e o saldo atual calculado.
     * 
     * @param[in] codigo Código da carteira a ser consultada
     * @param[out] carteira Ponteiro para armazenar os dados da carteira encontrada
     * @param[out] saldo Ponteiro para armazenar o saldo atual da carteira
     * @return true se a carteira foi encontrada, false caso contrário
     * 
     * @note Deve validar o formato do código antes da consulta
     * @note Deve calcular o saldo baseado nas ordens executadas
     * @note Deve retornar false se a carteira não existir
     * @note Os parâmetros de saída só devem ser preenchidos se a carteira for encontrada
     */
    virtual bool consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) = 0;
    
    /**
     * @brief Edita os dados de uma carteira existente.
     * 
     * Atualiza os dados da carteira no banco de dados, mantendo o código inalterado
     * e validando todos os outros campos antes da persistência.
     * 
     * @param[in] carteira Objeto Carteira contendo os dados atualizados
     * @return true se a edição foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar todos os campos antes da atualização
     * @note Deve verificar se a carteira existe antes de tentar editar
     * @note Deve retornar false se houver erro de validação ou persistência
     */
    virtual bool editarCarteira(const Carteira& carteira) = 0;
    
    /**
     * @brief Exclui uma carteira do sistema.
     * 
     * Remove a carteira associada ao código fornecido do banco de dados,
     * verificando se não há ordens associadas antes da exclusão.
     * 
     * @param[in] codigo Código da carteira a ser excluída
     * @return true se a exclusão foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar o formato do código antes da exclusão
     * @note Deve verificar se a carteira existe antes de tentar excluir
     * @note Deve verificar se não há ordens associadas à carteira
     * @note Deve retornar false se houver ordens associadas ou erro de persistência
     */
    virtual bool excluirCarteira(const Codigo& codigo) = 0;
    
    /**
     * @brief Cria uma nova ordem de investimento.
     * 
     * Valida e persiste os dados da ordem fornecida no banco de dados,
     * associando-a à carteira identificada pelo código.
     * 
     * @param[in] codigoCarteira Código da carteira que receberá a ordem
     * @param[in] ordem Objeto Ordem contendo os dados da ordem
     * @return true se a ordem foi criada com sucesso, false caso contrário
     * 
     * @note Deve validar todos os campos da ordem antes da persistência
     * @note Deve verificar se a carteira existe antes de criar a ordem
     * @note Deve verificar se o código da ordem não está duplicado
     * @note Deve retornar false se houver erro de validação ou persistência
     */
    virtual bool criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) = 0;
    
    /**
     * @brief Lista todas as ordens de uma carteira.
     * 
     * Busca no banco de dados todas as ordens associadas ao código da carteira fornecido,
     * retornando uma lista com os dados de cada ordem.
     * 
     * @param[in] codigoCarteira Código da carteira cujas ordens serão listadas
     * @param[out] listaOrdens Ponteiro para lista que armazenará as ordens
     * @return true se a consulta foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar o formato do código antes da consulta
     * @note Deve retornar lista vazia se a carteira não possuir ordens
     * @note Deve retornar false apenas em caso de erro de banco de dados
     */
    virtual bool listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) = 0;
    
    /**
     * @brief Exclui uma ordem do sistema.
     * 
     * Remove a ordem associada ao código fornecido do banco de dados.
     * 
     * @param[in] codigo Código da ordem a ser excluída
     * @return true se a exclusão foi realizada com sucesso, false caso contrário
     * 
     * @note Deve validar o formato do código antes da exclusão
     * @note Deve verificar se a ordem existe antes de tentar excluir
     * @note Deve retornar false se houver erro de persistência
     */
    virtual bool excluirOrdem(const Codigo& codigo) = 0;
    
    /**
     * @brief Destrutor virtual para permitir herança.
     */
    virtual ~IServicoInvestimento() {}
};

#endif // INTERFACES_H_INCLUDED