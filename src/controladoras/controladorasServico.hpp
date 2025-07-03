#ifndef CONTROLADORASSERVICO_HPP_INCLUDED
#define CONTROLADORASSERVICO_HPP_INCLUDED

#include "interfaces.hpp"
#include "database/DatabaseManager.hpp"
#include <memory>

// =================================================================================================
/**
 * @class ControladoraServico
 * @brief Implementação concreta das interfaces de serviço com persistência SQLite.
 * @details Esta classe centraliza a lógica de negócio e utiliza SQLite para persistência,
 * implementando as interfaces IServicoAutenticacao, IServicoUsuario e IServicoInvestimento.
 */
//                                     IMPLEMENTA AS 3 INTERFACES
class ControladoraServico : public IServicoAutenticacao, public IServicoUsuario, public IServicoInvestimento
{
  private:
    // Gerenciador de banco de dados SQLite
    std::unique_ptr<DatabaseManager> dbManager;

  public:
    /**
     * @brief Construtor da controladora.
     * @details Inicializa o sistema com banco de dados SQLite.
     */
    ControladoraServico();

    /**
     * @brief Destrutor da controladora.
     */
    ~ControladoraServico();

    /**
     * @brief Inicializa o banco de dados
     * @return true se inicializou com sucesso, false caso contrário
     */
    bool inicializar();

    // Métodos da interface IServicoAutenticacao
    bool autenticar(const Ncpf &cpf, const Senha &senha) override;

    // Métodos da interface IServicoUsuario
    bool cadastrarConta(const Conta &conta) override;
    bool consultarConta(const Ncpf &cpf, Conta *conta, Dinheiro *saldo) override;
    bool editarConta(const Conta &conta) override;
    bool excluirConta(const Ncpf &cpf) override;

    // Métodos da interface IServicoInvestimento
    bool criarCarteira(const Ncpf &cpf, const Carteira &carteira) override;
    bool listarCarteiras(const Ncpf &cpf, std::list<Carteira> *listaCarteiras) override;
    bool consultarCarteira(const Codigo &codigo, Carteira *carteira, Dinheiro *saldo) override;
    bool editarCarteira(const Carteira &carteira) override;
    bool excluirCarteira(const Codigo &codigo) override;
    bool criarOrdem(const Codigo &codigoCarteira, const Ordem &ordem) override;
    bool listarOrdens(const Codigo &codigoCarteira, std::list<Ordem> *listaOrdens) override;
    bool excluirOrdem(const Codigo &codigo) override;
};

#endif // CONTROLADORASSERVICO_HPP_INCLUDED