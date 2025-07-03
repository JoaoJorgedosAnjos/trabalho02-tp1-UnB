#ifndef CONTROLADORASAPRESENTACAO_HPP_INCLUDED
#define CONTROLADORASAPRESENTACAO_HPP_INCLUDED

#include "interfaces.hpp"

// =================================================================================================
/**
 * @class ControladoraApresentacaoAutenticacao
 * @brief Implementação concreta da interface de apresentação para autenticação.
 * @details Responsável por exibir o diálogo de login e capturar os dados do usuário.
 */
class ControladoraApresentacaoAutenticacao : public IApresentacaoAutenticacao {
private:
    IServicoAutenticacao* cntrServicoAutenticacao; // Ponteiro para a camada de serviço

public:
    bool autenticar(Ncpf* cpf) override;
    void setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) override;
};

// =================================================================================================
/**
 * @class ControladoraApresentacaoUsuario
 * @brief Implementação concreta da interface de apresentação para gerenciamento de usuário.
 * @details Responsável por exibir os menus de conta (consultar, editar, excluir) e cadastro.
 */
class ControladoraApresentacaoUsuario : public IApresentacaoUsuario {
private:
    IServicoUsuario* cntrServicoUsuario; // Ponteiro para a camada de serviço

public:
    bool executar(const Ncpf& cpf) override;
    void cadastrar() override;
    void setControladoraServico(IServicoUsuario* cntrServicoUsuario) override;

private:
    // Métodos auxiliares para cada funcionalidade
    bool consultarConta(const Ncpf& cpf);
    void editarConta(const Ncpf& cpf);
    bool excluirConta(const Ncpf& cpf);
};

// =================================================================================================
/**
 * @class ControladoraApresentacaoInvestimento
 * @brief Implementação concreta da interface de apresentação para gerenciamento de investimentos.
 * @details Responsável por exibir os menus de carteiras e ordens de investimento.
 */
class ControladoraApresentacaoInvestimento : public IApresentacaoInvestimento {
private:
    IServicoInvestimento* cntrServicoInvestimento; // Ponteiro para a camada de serviço

public:
    void executar(const Ncpf& cpf) override;
    void setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) override;

private:
    // Métodos auxiliares para gerenciamento de carteiras
    void menuCarteiras(const Ncpf& cpf);
    void criarCarteira(const Ncpf& cpf);
    void listarCarteiras(const Ncpf& cpf);
    void consultarCarteira(const Ncpf& cpf);
    void editarCarteira(const Ncpf& cpf);
    void excluirCarteira(const Ncpf& cpf);
    
    // Métodos auxiliares para ações específicas de carteira (chamados dentro da listagem)
    void editarCarteiraEspecifica(const Ncpf& cpf, const Carteira& carteiraAtual);
    bool excluirCarteiraEspecifica(const Ncpf& cpf, const Carteira& carteiraAtual);
    
    // Métodos auxiliares para gerenciamento de ordens
    void menuOrdens(const Codigo& codigoCarteira);
    void criarOrdem(const Codigo& codigoCarteira);
    void listarOrdens(const Codigo& codigoCarteira);
    void excluirOrdem(const Codigo& codigoCarteira);
};

// As outras controladoras de apresentação virão aqui depois...

#endif // CONTROLADORASAPRESENTACAO_HPP_INCLUDED