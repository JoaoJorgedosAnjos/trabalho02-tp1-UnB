#ifndef CONTROLADORASSERVICO_HPP_INCLUDED
#define CONTROLADORASSERVICO_HPP_INCLUDED

#include "database/DatabaseManager.hpp"
#include "interfaces.hpp"
#include <memory>

/**
 * @class ControladoraServico
 * @brief Implementação concreta das interfaces de serviço com persistência SQLite
 * @details Esta classe centraliza a lógica de negócio e utiliza SQLite para persistência,
 *          implementando as interfaces IServicoAutenticacao, IServicoUsuario e IServicoInvestimento.
 *          Atua como camada de serviço no padrão arquitetural MVC, processando regras de negócio
 *          e validações antes de persistir os dados.
 * @see IServicoAutenticacao
 * @see IServicoUsuario
 * @see IServicoInvestimento
 * @see DatabaseManager
 */
class ControladoraServico : public IServicoAutenticacao, public IServicoUsuario, public IServicoInvestimento
{
  private:
    std::unique_ptr<DatabaseManager> dbManager;

  public:
    /**
     * @brief Construtor da controladora de serviço
     * @details Inicializa o sistema com banco de dados SQLite usando smart pointer
     *          para gerenciamento automático de memória.
     */
    ControladoraServico();

    /**
     * @brief Destrutor da controladora de serviço
     * @details Destrutor padrão que garante limpeza automática dos recursos.
     */
    ~ControladoraServico();

    /**
     * @brief Inicializa o banco de dados
     * @return true se inicializou com sucesso, false caso contrário
     * @details Estabelece conexão com o banco de dados SQLite e executa a inicialização
     *          das tabelas necessárias. Deve ser chamado antes de qualquer operação.
     */
    bool inicializar();

    /**
     * @brief Autentica um usuário no sistema
     * @param cpf CPF do usuário para autenticação
     * @param senha Senha do usuário para autenticação
     * @return true se a autenticação foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoAutenticacao. Verifica se as credenciais
     *          fornecidas correspondem a um usuário válido cadastrado no sistema.
     * @see IServicoAutenticacao::autenticar()
     */
    bool autenticar(const Ncpf &cpf, const Senha &senha) override;

    /**
     * @brief Cadastra uma nova conta no sistema
     * @param conta Objeto conta com os dados a serem cadastrados
     * @return true se o cadastro foi bem-sucedido, false caso contrário
     * @details Implementação da interface IServicoUsuario. Valida unicidade do CPF
     *          antes de inserir a nova conta no banco de dados.
     * @see IServicoUsuario::cadastrarConta()
     */
    bool cadastrarConta(const Conta &conta) override;

    /**
     * @brief Consulta dados de uma conta e calcula saldo total
     * @param cpf CPF da conta a ser consultada
     * @param conta Ponteiro para objeto onde serão armazenados os dados da conta
     * @param saldo Ponteiro para objeto onde será armazenado o saldo total calculado
     * @return true se a consulta foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoUsuario. Busca dados da conta e
     *          calcula o saldo total somando os saldos de todas as carteiras associadas.
     * @see IServicoUsuario::consultarConta()
     */
    bool consultarConta(const Ncpf &cpf, Conta *conta, Dinheiro *saldo) override;

    /**
     * @brief Edita os dados de uma conta existente
     * @param conta Objeto conta com os novos dados
     * @return true se a edição foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoUsuario. Atualiza os dados da conta
     *          no banco de dados. O CPF não pode ser alterado.
     * @see IServicoUsuario::editarConta()
     */
    bool editarConta(const Conta &conta) override;

    /**
     * @brief Exclui uma conta do sistema
     * @param cpf CPF da conta a ser excluída
     * @return true se a exclusão foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoUsuario. Remove a conta e todos os
     *          dados relacionados (carteiras e ordens) respeitando integridade referencial.
     * @see IServicoUsuario::excluirConta()
     */
    bool excluirConta(const Ncpf &cpf) override;

    /**
     * @brief Cria uma nova carteira para um usuário
     * @param cpf CPF do usuário proprietário da carteira
     * @param carteira Objeto carteira com os dados a serem cadastrados
     * @return true se a criação foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Valida a existência
     *          da conta e a unicidade do código da carteira antes de inserir.
     * @see IServicoInvestimento::criarCarteira()
     */
    bool criarCarteira(const Ncpf &cpf, const Carteira &carteira) override;

    /**
     * @brief Lista todas as carteiras de um usuário
     * @param cpf CPF do usuário
     * @param listaCarteiras Ponteiro para lista onde serão armazenadas as carteiras
     * @return true se a listagem foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Recupera todas as
     *          carteiras associadas ao CPF fornecido.
     * @see IServicoInvestimento::listarCarteiras()
     */
    bool listarCarteiras(const Ncpf &cpf, std::list<Carteira> *listaCarteiras) override;

    /**
     * @brief Consulta dados de uma carteira e calcula seu saldo
     * @param codigo Código da carteira a ser consultada
     * @param carteira Ponteiro para objeto onde serão armazenados os dados da carteira
     * @param saldo Ponteiro para objeto onde será armazenado o saldo calculado
     * @return true se a consulta foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Busca os dados da
     *          carteira e calcula o saldo baseado nas ordens associadas.
     * @see IServicoInvestimento::consultarCarteira()
     */
    bool consultarCarteira(const Codigo &codigo, Carteira *carteira, Dinheiro *saldo) override;

    /**
     * @brief Edita os dados de uma carteira existente
     * @param carteira Objeto carteira com os novos dados
     * @return true se a edição foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Atualiza os dados da
     *          carteira no banco de dados. O código não pode ser alterado.
     * @see IServicoInvestimento::editarCarteira()
     */
    bool editarCarteira(const Carteira &carteira) override;

    /**
     * @brief Exclui uma carteira do sistema
     * @param codigo Código da carteira a ser excluída
     * @return true se a exclusão foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Remove a carteira e
     *          todas as ordens associadas respeitando integridade referencial.
     * @see IServicoInvestimento::excluirCarteira()
     */
    bool excluirCarteira(const Codigo &codigo) override;

    /**
     * @brief Cria uma nova ordem de investimento
     * @param codigoCarteira Código da carteira onde a ordem será criada
     * @param ordem Objeto ordem com os dados a serem cadastrados
     * @return true se a criação foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Processo complexo que
     *          inclui validação da carteira, consulta a dados históricos, cálculo de
     *          preços e formatação monetária antes da inserção.
     * @see IServicoInvestimento::criarOrdem()
     */
    bool criarOrdem(const Codigo &codigoCarteira, const Ordem &ordem) override;

    /**
     * @brief Lista todas as ordens de uma carteira
     * @param codigoCarteira Código da carteira
     * @param listaOrdens Ponteiro para lista onde serão armazenadas as ordens
     * @return true se a listagem foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Recupera todas as
     *          ordens associadas ao código da carteira fornecido.
     * @see IServicoInvestimento::listarOrdens()
     */
    bool listarOrdens(const Codigo &codigoCarteira, std::list<Ordem> *listaOrdens) override;

    /**
     * @brief Exclui uma ordem do sistema
     * @param codigo Código da ordem a ser excluída
     * @return true se a exclusão foi bem-sucedida, false caso contrário
     * @details Implementação da interface IServicoInvestimento. Remove a ordem do
     *          banco de dados, afetando o cálculo do saldo da carteira.
     * @see IServicoInvestimento::excluirOrdem()
     */
    bool excluirOrdem(const Codigo &codigo) override;
};

#endif // CONTROLADORASSERVICO_HPP_INCLUDED