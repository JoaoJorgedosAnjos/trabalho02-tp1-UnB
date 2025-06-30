#ifndef CONTROLADORASSERVICO_HPP_INCLUDED
#define CONTROLADORASSERVICO_HPP_INCLUDED

#include "interfaces.hpp"
#include <map>
#include <string>

// =================================================================================================
/**
 * @class ControladoraServico
 * @brief Implementação concreta das interfaces de serviço.
 * @details Esta classe centraliza a lógica de negócio e o armazenamento de dados em memória,
 * implementando as interfaces IServicoAutenticacao, IServicoUsuario e IServicoInvestimento.
 */
//                                     IMPLEMENTA AS 3 INTERFACES
class ControladoraServico : public IServicoAutenticacao, public IServicoUsuario, public IServicoInvestimento
{
  private:
    // Usa std::map para acesso rápido aos dados através de uma chave (string).
    std::map<std::string, Conta> contas;
    std::map<std::string, Carteira> carteiras;
    std::map<std::string, Ordem> ordens;

    // Mapa para associar Conta -> Lista de Carteiras
    std::map<std::string, std::list<Codigo>> mapaContaCarteiras;

    // Mapa para associar Carteira -> Lista de Ordens
    std::map<std::string, std::list<Codigo>> mapaCarteiraOrdens;

  public:
    /**
     * @brief Construtor da controladora.
     * @details Pode ser usado para inicializar o sistema com dados de teste.
     */
    ControladoraServico();

    // Métodos da interface IServicoAutenticacao
    bool autenticar(const Ncpf &cpf, const Senha &senha) override;

    // Métodos da interface IServicoUsuario (serão implementados depois)
    bool cadastrarConta(const Conta &conta) override;
    bool consultarConta(const Ncpf &cpf, Conta *conta, Dinheiro *saldo) override;
    bool editarConta(const Conta &conta) override;
    bool excluirConta(const Ncpf &cpf) override;

    // Métodos da interface IServicoInvestimento (serão implementados depois)
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