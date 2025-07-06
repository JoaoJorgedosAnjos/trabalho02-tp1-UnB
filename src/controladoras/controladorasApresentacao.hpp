#ifndef CONTROLADORASAPRESENTACAO_HPP_INCLUDED
#define CONTROLADORASAPRESENTACAO_HPP_INCLUDED

#include "interfaces.hpp"

// Forward declarations das classes refatoradas
class CarteiraController;
class OrdemController;

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
    CarteiraController* carteiraController;
    OrdemController* ordemController;

public:
    ControladoraApresentacaoInvestimento();
    ~ControladoraApresentacaoInvestimento();
    void executar(const Ncpf& cpf) override;
    void setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) override;
};



// =================================================================================================
/**
 * @class InterfaceManager
 * @brief Gerenciador central de interface do usuário
 * @details Responsável por controlar o fluxo de navegação entre telas,
 * limpeza automática do console e apresentação consistente das interfaces.
 */
class InterfaceManager {
private:
    ControladoraApresentacaoAutenticacao* cntrApresentacaoAutenticacao;
    ControladoraApresentacaoUsuario* cntrApresentacaoUsuario;
    ControladoraApresentacaoInvestimento* cntrApresentacaoInvestimento;
    
    // Estados de navegação
    enum class TelaAtual {
        MENU_INICIAL,
        LOGIN,
        CADASTRO,
        MENU_PRINCIPAL,
        GERENCIAR_CONTA,
        GERENCIAR_INVESTIMENTOS,
        MENU_CARTEIRAS,
        MENU_ORDENS,
        SAIR
    };
    
    TelaAtual telaAtual;
    Ncpf cpfAutenticado;
    bool usuarioAutenticado;
    
    // Métodos auxiliares
    void limparTela();
    void mostrarMenuInicial();
    void mostrarMenuPrincipal();
    void processarMenuInicial();
    void processarMenuPrincipal();
    void processarGerenciarConta();
    void processarGerenciarInvestimentos();
    void processarMenuCarteiras();
    void processarMenuOrdens();
    void fazerLogout();

public:
    InterfaceManager(
        ControladoraApresentacaoAutenticacao* cntrApresentacaoAutenticacao,
        ControladoraApresentacaoUsuario* cntrApresentacaoUsuario,
        ControladoraApresentacaoInvestimento* cntrApresentacaoInvestimento
    );
    
    void executar();
};

// As outras controladoras de apresentação virão aqui depois...

#endif // CONTROLADORASAPRESENTACAO_HPP_INCLUDED