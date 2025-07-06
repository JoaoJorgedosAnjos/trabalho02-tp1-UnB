#ifndef CONTROLADORASAPRESENTACAO_HPP_INCLUDED
#define CONTROLADORASAPRESENTACAO_HPP_INCLUDED

#include "interfaces.hpp"

class CarteiraController;
class OrdemController;
/**
 * @class ControladoraApresentacaoAutenticacao
 * @brief Implementação concreta da interface de apresentação para autenticação.
 * @details Responsável por exibir o diálogo de login e capturar os dados do usuário.
 * Implementa funcionalidades de formatação automática de CPF e validação de credenciais.
 */
class ControladoraApresentacaoAutenticacao : public IApresentacaoAutenticacao {
private:
    IServicoAutenticacao* cntrServicoAutenticacao; ///< Ponteiro para a camada de serviço

public:
    /**
     * @brief Realiza a autenticação do usuário
     * 
     * @param cpf Ponteiro para objeto Ncpf onde será armazenado o CPF validado
     * @return bool true se autenticação bem-sucedida, false caso contrário
     * 
     * @details Apresenta interface de login com:
     * - Máscara automática para CPF (aceita apenas números)
     * - Validação de formato e dígitos verificadores
     * - Tratamento de erros com dicas informativas
     * - Opção de cancelamento a qualquer momento
     */
    bool autenticar(Ncpf* cpf) override;
    
    /**
     * @brief Define a controladora de serviço para autenticação
     * 
     * @param cntrServicoAutenticacao Ponteiro para a interface de serviço de autenticação
     * 
     * @details Estabelece a conexão com a camada de serviço que será utilizada
     * para realizar as operações de autenticação no banco de dados.
     */
    void setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) override;
};

// =================================================================================================
/**
 * @class ControladoraApresentacaoUsuario
 * @brief Implementação concreta da interface de apresentação para gerenciamento de usuário.
 * @details Responsável por exibir os menus de conta (consultar, editar, excluir) e cadastro.
 * Implementa todas as funcionalidades relacionadas ao ciclo de vida das contas de usuário.
 */
class ControladoraApresentacaoUsuario : public IApresentacaoUsuario {
private:
    IServicoUsuario* cntrServicoUsuario; ///< Ponteiro para a camada de serviço

public:
    /**
     * @brief Executa o menu de gerenciamento de conta do usuário
     * 
     * @param cpf CPF do usuário autenticado
     * @return bool true se a conta foi excluída (logout forçado), false caso contrário
     * 
     * @details Apresenta menu interativo com opções para:
     * - Consultar dados da conta e saldo total
     * - Editar nome e/ou senha
     * - Excluir conta permanentemente
     * - Voltar ao menu principal
     */
    bool executar(const Ncpf& cpf) override;
    
    /**
     * @brief Implementa o cadastro de uma nova conta no sistema
     * 
     * @details Oferece interface amigável para cadastro com:
     * - Máscara automática para CPF
     * - Validação em tempo real dos dados
     * - Possibilidade de corrigir dados anteriores
     * - Confirmação antes de salvar
     * - Tratamento robusto de erros
     */
    void cadastrar() override;
    
    /**
     * @brief Define a controladora de serviço para gerenciamento de usuários
     * 
     * @param cntrServicoUsuario Ponteiro para a interface de serviço de usuário
     * 
     * @details Estabelece a conexão com a camada de serviço que será utilizada
     * para realizar operações de cadastro, consulta, edição e exclusão de contas.
     */
    void setControladoraServico(IServicoUsuario* cntrServicoUsuario) override;

private:
    /**
     * @brief Consulta e exibe os dados da conta do usuário
     * 
     * @param cpf CPF do usuário a ser consultado
     * @return bool true se a conta foi excluída (logout forçado), false caso contrário
     * 
     * @details Exibe informações completas da conta:
     * - CPF formatado
     * - Nome do usuário
     * - Saldo total consolidado de todas as carteiras
     * - Menu de ações rápidas (editar/excluir)
     */
    bool consultarConta(const Ncpf& cpf);
    
    /**
     * @brief Edita os dados da conta do usuário
     * 
     * @param cpf CPF do usuário cuja conta será editada
     * 
     * @details Permite edição de:
     * - Nome (até 20 caracteres)
     * - Senha (6 caracteres com regras específicas)
     * - Ambos os campos simultaneamente
     * 
     * Inclui validação completa e opção de cancelamento.
     */
    void editarConta(const Ncpf& cpf);
    
    /**
     * @brief Exclui permanentemente a conta do usuário
     * 
     * @param cpf CPF do usuário cuja conta será excluída
     * @return bool true se a conta foi excluída com sucesso, false caso contrário
     * 
     * @details Implementa exclusão segura com:
     * - Aviso sobre irreversibilidade
     * - Confirmação explícita necessária
     * - Validação de integridade referencial
     * - Orientações em caso de erro
     */
    bool excluirConta(const Ncpf& cpf);
};

/**
 * @class ControladoraApresentacaoInvestimento
 * @brief Implementação concreta da interface de apresentação para gerenciamento de investimentos.
 * @details Responsável por exibir os menus de carteiras e ordens de investimento.
 * Utiliza controladores especializados para cada tipo de operação.
 */
class ControladoraApresentacaoInvestimento : public IApresentacaoInvestimento {
private:
    IServicoInvestimento* cntrServicoInvestimento; ///< Ponteiro para a camada de serviço
    CarteiraController* carteiraController;        ///< Controlador especializado para carteiras
    OrdemController* ordemController;              ///< Controlador especializado para ordens

public:
    /**
     * @brief Construtor da controladora de apresentação de investimentos
     * 
     * @details Inicializa os ponteiros como nullptr. Os controladores especializados
     * são criados quando o serviço é definido através de setControladoraServico().
     */
    ControladoraApresentacaoInvestimento();
    
    /**
     * @brief Destrutor da controladora de apresentação de investimentos
     * 
     * @details Libera a memória dos controladores especializados para evitar vazamentos.
     */
    ~ControladoraApresentacaoInvestimento();
    
    /**
     * @brief Executa o menu principal de investimentos
     * 
     * @param cpf CPF do usuário autenticado
     * 
     * @details Apresenta menu com duas opções principais:
     * - Gerenciar Carteiras: acesso direto ao CarteiraController
     * - Gerenciar Ordens: seleção de carteira + OrdemController
     * 
     * Inclui validações e tratamento de erros para cada fluxo.
     */
    void executar(const Ncpf& cpf) override;
    
    /**
     * @brief Define a controladora de serviço para investimentos
     * 
     * @param cntrServicoInvestimento Ponteiro para a interface de serviço de investimentos
     * 
     * @details Estabelece a conexão com a camada de serviço e inicializa
     * os controladores especializados (CarteiraController e OrdemController).
     */
    void setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) override;
};

/**
 * @class InterfaceManager
 * @brief Gerenciador central de interface do usuário
 * @details Responsável por controlar o fluxo de navegação entre telas,
 * limpeza automática do console e apresentação consistente das interfaces.
 * Implementa uma máquina de estados para controlar o fluxo da aplicação.
 */
class InterfaceManager {
private:
    ControladoraApresentacaoAutenticacao* cntrApresentacaoAutenticacao; ///< Controladora de autenticação
    ControladoraApresentacaoUsuario* cntrApresentacaoUsuario;           ///< Controladora de usuário
    ControladoraApresentacaoInvestimento* cntrApresentacaoInvestimento; ///< Controladora de investimentos
    
    /**
     * @enum TelaAtual
     * @brief Estados possíveis da máquina de estados da interface
     * 
     * @details Define todos os estados possíveis do sistema:
     * - MENU_INICIAL: Tela de entrada (login/cadastro)
     * - LOGIN: Processo de autenticação
     * - CADASTRO: Processo de criação de conta
     * - MENU_PRINCIPAL: Menu principal (usuário autenticado)
     * - GERENCIAR_CONTA: Funcionalidades de conta
     * - GERENCIAR_INVESTIMENTOS: Funcionalidades de investimento
     * - MENU_CARTEIRAS: Gerenciamento de carteiras (não usado atualmente)
     * - MENU_ORDENS: Gerenciamento de ordens (não usado atualmente)
     * - SAIR: Encerramento da aplicação
     */
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
    
    /**
     * @brief Limpa a tela (método mantido para compatibilidade)
     * 
     * @deprecated A limpeza de tela agora é feita pela telaUtils::exibirCabecalho()
     */
    void limparTela();
    
    /**
     * @brief Exibe o menu inicial do sistema
     * 
     * @details Apresenta as opções:
     * - 1: Login
     * - 2: Cadastrar nova conta
     * - 0: Sair
     */
    void mostrarMenuInicial();
    
    /**
     * @brief Exibe o menu principal do sistema
     * 
     * @details Apresenta as opções para usuários autenticados:
     * - Informação do usuário logado
     * - 1: Gerenciar Conta
     * - 2: Gerenciar Investimentos
     * - 0: Logout
     */
    void mostrarMenuPrincipal();
    
    /**
     * @brief Processa a seleção do menu inicial
     * 
     * @details Captura a entrada do usuário e define o próximo estado:
     * - 1: Vai para LOGIN
     * - 2: Vai para CADASTRO
     * - 0: Vai para SAIR
     * - Outros: Exibe erro e mantém estado atual
     */
    void processarMenuInicial();
    
    /**
     * @brief Processa a seleção do menu principal
     * 
     * @details Captura a entrada do usuário e define o próximo estado:
     * - 1: Vai para GERENCIAR_CONTA
     * - 2: Vai para GERENCIAR_INVESTIMENTOS
     * - 0: Executa logout
     * - Outros: Exibe erro e mantém estado atual
     */
    void processarMenuPrincipal();
    
    /**
     * @brief Processa o gerenciamento de conta
     * 
     * @details Executa a controladora de usuário e:
     * - Se retornar true (conta excluída): força logout
     * - Se retornar false: volta para MENU_PRINCIPAL
     */
    void processarGerenciarConta();
    
    /**
     * @brief Processa o gerenciamento de investimentos
     * 
     * @details Executa a controladora de investimentos e
     * retorna automaticamente para MENU_PRINCIPAL ao finalizar.
     */
    void processarGerenciarInvestimentos();
    
    /**
     * @brief Processa o menu de carteiras (não implementado)
     * 
     * @deprecated Este método não é usado na implementação atual.
     * O gerenciamento de carteiras é feito através do CarteiraController.
     */
    void processarMenuCarteiras();
    
    /**
     * @brief Processa o menu de ordens (não implementado)
     * 
     * @deprecated Este método não é usado na implementação atual.
     * O gerenciamento de ordens é feito através do OrdemController.
     */
    void processarMenuOrdens();
    
    /**
     * @brief Realiza o logout do usuário
     * 
     * @details Executa as seguintes ações:
     * - Exibe mensagem de logout
     * - Limpa o estado de autenticação
     * - Retorna para MENU_INICIAL
     */
    void fazerLogout();

public:
    /**
     * @brief Construtor do gerenciador de interface
     * 
     * @param cntrApresentacaoAutenticacao Controladora de apresentação de autenticação
     * @param cntrApresentacaoUsuario Controladora de apresentação de usuário
     * @param cntrApresentacaoInvestimento Controladora de apresentação de investimentos
     * 
     * @details Inicializa o gerenciador com as controladoras necessárias e
     * define o estado inicial como MENU_INICIAL com usuário não autenticado.
     */
    InterfaceManager(
        ControladoraApresentacaoAutenticacao* cntrApresentacaoAutenticacao,
        ControladoraApresentacaoUsuario* cntrApresentacaoUsuario,
        ControladoraApresentacaoInvestimento* cntrApresentacaoInvestimento
    );
    
    /**
     * @brief Executa o loop principal do sistema
     * 
     * @details Implementa a máquina de estados principal, controlando
     * o fluxo de navegação entre diferentes telas e funcionalidades.
     * 
     * O loop continua até que telaAtual seja definida como SAIR.
     * Cada estado é processado por métodos específicos que podem
     * alterar o estado para controlar o fluxo da aplicação.
     * 
     * Estados suportados:
     * - MENU_INICIAL → LOGIN/CADASTRO/SAIR
     * - LOGIN → MENU_PRINCIPAL/MENU_INICIAL
     * - CADASTRO → MENU_INICIAL
     * - MENU_PRINCIPAL → GERENCIAR_CONTA/GERENCIAR_INVESTIMENTOS/MENU_INICIAL
     * - GERENCIAR_CONTA → MENU_PRINCIPAL/MENU_INICIAL (se conta excluída)
     * - GERENCIAR_INVESTIMENTOS → MENU_PRINCIPAL
     */
    void executar();
};

#endif // CONTROLADORASAPRESENTACAO_HPP_INCLUDED