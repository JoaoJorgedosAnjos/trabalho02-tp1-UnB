#include <iostream>
#include <iomanip>
#include <cctype>
#include <limits>
#include <fstream>
#include <cstdlib>
#include <set>
#include "controladorasApresentacao.hpp"
#include "telaUtils.hpp"
#include "CarteiraController.hpp"
#include "OrdemController.hpp"

/**
 * @brief Formata uma string de números para o padrão de CPF brasileiro
 * 
 * @details Esta função implementa uma máscara automática que permite ao usuário
 * digitar apenas os 11 dígitos do CPF, sem se preocupar com pontos e hífen.
 * A função automaticamente adiciona a formatação necessária.
 * 
 * Exemplos de transformação:
 * - "12345678901" -> "xxx.xxx.xxx-xx"
 * - "98765432100" -> "xxx.xxx.xxx-xx"
 * 
 * A função também aceita CPFs já formatados, retornando-os inalterados se
 * estiverem no formato correto.
 * 
 * @param entrada String contendo os dígitos do CPF (com ou sem formatação)
 * @return std::string CPF formatado no padrão XXX.XXX.XXX-XX
 * @throws std::invalid_argument se a entrada não contiver exatamente 11 dígitos numéricos
 * 
 * @note Esta função apenas formata, não valida os dígitos verificadores
 * @see Ncpf::setValor() para validação completa do CPF
 * 
 * @example
 * @code
 * std::string cpfFormatado = formatarCPF("xxxxxxxxxxx");
 * // Resultado: "xxx.xxx.xxx-xx"
 * @endcode
 */
std::string formatarCPF(const std::string& entrada) {
    std::string apenasNumeros;
    for (char c : entrada) {
        if (std::isdigit(c)) {
            apenasNumeros += c;
        }
    }
    
    if (apenasNumeros.length() != 11) {
        throw std::invalid_argument("CPF deve conter exatamente 11 dígitos numéricos (você digitou " + std::to_string(apenasNumeros.length()) + " dígitos)");
    }
    
    std::string cpfFormatado;
    cpfFormatado += apenasNumeros.substr(0, 3);
    cpfFormatado += ".";
    cpfFormatado += apenasNumeros.substr(3, 3);
    cpfFormatado += ".";
    cpfFormatado += apenasNumeros.substr(6, 3);
    cpfFormatado += "-";
    cpfFormatado += apenasNumeros.substr(9, 2);
    
    return cpfFormatado;
}


/**
 * @brief Define a controladora de serviço para autenticação
 * 
 * @param cntrServicoAutenticacao Ponteiro para a interface de serviço de autenticação
 * 
 * @details Estabelece a conexão com a camada de serviço que será utilizada
 * para realizar as operações de autenticação no banco de dados.
 */
void ControladoraApresentacaoAutenticacao::setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) {
    this->cntrServicoAutenticacao = cntrServicoAutenticacao;
}

/**
 * @brief Implementação do método que apresenta o formulário de autenticação
 * 
 * @details Este método implementa uma interface amigável para autenticação de usuários,
 * com as seguintes melhorias de usabilidade:
 * - Máscara automática para CPF: usuário pode digitar apenas números
 * - Tratamento de erros claro e informativo
 * - Loop de tentativas até entrada válida
 * 
 * O usuário pode digitar o CPF de duas formas:
 * 1. Apenas números: "12345678901" (será formatado automaticamente)
 * 2. Já formatado: "xxx.xxx.xxx-xx" (será aceito se válido)
 * 
 * @param cpf Ponteiro para objeto Ncpf onde será armazenado o CPF validado
 * @return bool true se autenticação bem-sucedida, false caso contrário
 * 
 * @note O método aplica validação tanto de formato quanto de dígitos verificadores
 */
bool ControladoraApresentacaoAutenticacao::autenticar(Ncpf* cpf) {
    std::string valor;
    Senha senha;

    while (true) {
        telaUtils::exibirCabecalho("AUTENTICACAO DE USUARIO");
        std::cout << "(Digite '0' a qualquer momento para cancelar)" << std::endl;

        try {
            std::cout << "CPF (apenas numeros ou XXX.XXX.XXX-XX): ";
            std::cin >> valor;
            
            if (valor == "0") {
                std::cout << "Login cancelado pelo usuario." << std::endl;
                return false;
            }
            
            try {
                std::string cpfFormatado = formatarCPF(valor);
                std::cout << "CPF formatado: " << cpfFormatado << std::endl;
                cpf->setValor(cpfFormatado); 
            } catch (const std::invalid_argument& e) {
                try {
                    cpf->setValor(valor);
                } catch (const std::invalid_argument& e2) {
                    throw e;
                }
            }

            std::cout << "Senha (6 caracteres)    : ";
            std::cin >> valor;
            
            if (valor == "0") {
                std::cout << "Login cancelado pelo usuario." << std::endl;
                return false;
            }
            
            senha.setValor(valor); 
            
            break; 

        } catch (const std::invalid_argument &exp) {
            telaUtils::limparTela();
            telaUtils::exibirCabecalho("ERRO DE AUTENTICACAO");
            std::cout << "\n❌ Erro: " << exp.what() << std::endl;
            std::cout << "\n💡 Dicas:" << std::endl;
            std::cout << "   • CPF: Digite apenas os 11 números (ex: xxx.xxx.xxx-xx)" << std::endl;
            std::cout << "   • Senha: Use 1 maiúscula, 1 minúscula, 1 número e 1 símbolo (#$%&)" << std::endl;
            std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    }

    return cntrServicoAutenticacao->autenticar(*cpf, senha);
}

/**
 * @brief Define a controladora de serviço para gerenciamento de usuários
 * 
 * @param cntrServicoUsuario Ponteiro para a interface de serviço de usuário
 * 
 * @details Estabelece a conexão com a camada de serviço que será utilizada
 * para realizar operações de cadastro, consulta, edição e exclusão de contas.
 */
void ControladoraApresentacaoUsuario::setControladoraServico(IServicoUsuario* cntrServicoUsuario) {
    this->cntrServicoUsuario = cntrServicoUsuario;
}

/**
 * @brief Executa o menu de gerenciamento de conta do usuário
 * 
 * @param cpf CPF do usuário autenticado
 * @return bool true se a conta foi excluída (logout forçado), false caso contrário
 * 
 * @details Apresenta um menu interativo com as seguintes opções:
 * - 1: Consultar dados da conta (CPF, nome, saldo total)
 * - 2: Editar dados da conta (nome e/ou senha)
 * - 3: Excluir conta permanentemente
 * - 0: Voltar ao menu principal
 * 
 * O menu é executado em loop até que o usuário escolha voltar (0) ou
 * até que a conta seja excluída (retorna true para forçar logout).
 * 
 * @note Se a conta for excluída, o método retorna true para sinalizar
 * que o usuário deve ser deslogado do sistema
 */
bool ControladoraApresentacaoUsuario::executar(const Ncpf& cpf) {
    int opcao = -1;
    while (opcao != 0) {
        telaUtils::exibirCabecalho("GERENCIAMENTO DE CONTA");
        std::cout << "1. Consultar dados da conta." << std::endl;
        std::cout << "2. Editar dados da conta." << std::endl;
        std::cout << "3. Excluir minha conta." << std::endl;
        std::cout << "0. Voltar ao menu principal." << std::endl;
        telaUtils::exibirSeparador('-', 40);
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                if (consultarConta(cpf)) {
                    return true;
                }
                break;
            case 2:
                editarConta(cpf);
                break;
            case 3:
                if (excluirConta(cpf)) {
                    return true;
                }
                break;
            case 0:
                break;
            default:
                std::cout << "Opcao invalida!" << std::endl;
        }
    }
    return false; 
}

/**
 * @brief Implementa o cadastro de uma nova conta no sistema
 * 
 * @details Este método oferece uma interface amigável para cadastro de novos usuários,
 * com as mesmas melhorias de usabilidade da autenticação:
 * - Máscara automática para CPF
 * - Dicas claras para formatação de dados
 * - Tratamento robusto de erros
 * 
 * O método valida todos os dados antes de tentar o cadastro e fornece
 * feedback específico sobre cada tipo de erro encontrado.
 */
void ControladoraApresentacaoUsuario::cadastrar() {
    telaUtils::exibirCabecalho("CADASTRO DE NOVA CONTA");
    std::cout << "(Digite '0' a qualquer momento para cancelar)" << std::endl;
    
    std::string valor;
    Conta novaConta;
    Ncpf cpf;
    Nome nome;
    Senha senha;
    
    bool cpfValido = false;
    bool nomeValido = false;
    bool senhaValida = false;
    
    while (!cpfValido || !nomeValido || !senhaValida) {
        
        if (!cpfValido) {
            try {
                std::cout << "\n=== 1. CPF ===" << std::endl;
                std::cout << "CPF (apenas numeros ou XXX.XXX.XXX-XX): ";
                std::cin >> valor;

                if (valor == "0") {
                    std::cout << "Cadastro cancelado pelo usuario." << std::endl;
                    return;
                }
                
                try {
                    std::string cpfFormatado = formatarCPF(valor);
                    std::cout << "CPF formatado: " << cpfFormatado << std::endl;
                    cpf.setValor(cpfFormatado);
                } catch (const std::invalid_argument& e) {
                    try {
                        cpf.setValor(valor);
                    } catch (const std::invalid_argument& e2) {
                        throw e;
                    }
                }
                
                cpfValido = true; 
                
            } catch (const std::invalid_argument &exp) {
                telaUtils::limparTela();
                telaUtils::exibirCabecalho("ERRO NO CPF");
                std::cout << "\n❌ Erro no CPF: " << exp.what() << std::endl;
                std::cout << "\n💡 Dica: Digite apenas os 11 números (ex: xxx.xxx.xxx-xx)" << std::endl;
                std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
                std::cin.ignore();
                std::cin.get();
            }
        }
        
        else if (!nomeValido) {
            try {
                std::cout << "\n=== 2. NOME ===" << std::endl;
                std::cout << "CPF já cadastrado: " << cpf.getValor() << " ✓" << std::endl;
                std::cout << "(Digite '0' para cancelar ou 'r' para reescrever CPF)" << std::endl;
                
                std::cin.ignore();
                std::cout << "Nome (ate 20 caracteres): ";
                std::getline(std::cin, valor);
                
                if (valor == "0") {
                    std::cout << "Cadastro cancelado pelo usuario." << std::endl;
                    return;
                }
                
                if (valor == "r" || valor == "R") {
                    std::cout << "Voltando para reescrever CPF..." << std::endl;
                    cpfValido = false;
                    continue;
                }
                
                nome.setValor(valor);
                nomeValido = true; 
                
            } catch (const std::invalid_argument &exp) {
                std::cout << "\nErro no Nome: " << exp.what() << std::endl;
                std::cout << "Dica: Maximo 20 caracteres, sem espacos duplos" << std::endl;
            }
        }
        
        else if (!senhaValida) {
            try {
                std::cout << "\n=== 3. SENHA ===" << std::endl;
                std::cout << "CPF já cadastrado: " << cpf.getValor() << " ✓" << std::endl;
                std::cout << "Nome já cadastrado: " << nome.getValor() << " ✓" << std::endl;
                std::cout << "(Digite '0' para cancelar, 'r' para reescrever CPF, 'n' para reescrever nome)" << std::endl;
                
                std::cout << "Senha (6 caracteres): ";
                std::cin >> valor;
                
                if (valor == "0") {
                    std::cout << "Cadastro cancelado pelo usuario." << std::endl;
                    return;
                }
                
                if (valor == "r" || valor == "R") {
                    std::cout << "Voltando para reescrever CPF..." << std::endl;
                    cpfValido = false; 
                    continue;
                }
                
                if (valor == "n" || valor == "N") {
                    std::cout << "Voltando para reescrever nome..." << std::endl;
                    nomeValido = false; 
                    continue;
                }
                
                senha.setValor(valor);
                senhaValida = true; 
                
            } catch (const std::invalid_argument &exp) {
                std::cout << "\nErro na Senha: " << exp.what() << std::endl;
                std::cout << "Dica: 6 caracteres com 1 maiuscula, 1 minuscula, 1 numero e 1 simbolo (#$%&)" << std::endl;
            }
        }
    }
    
    std::cout << "\n=== RESUMO DO CADASTRO ===" << std::endl;
    std::cout << "CPF  : " << cpf.getValor() << std::endl;
    std::cout << "Nome : " << nome.getValor() << std::endl;
    std::cout << "Senha: ****** (6 caracteres)" << std::endl;
    std::cout << "==========================" << std::endl;
    
    char confirmacao;
    std::cout << "\nConfirma o cadastro? (s/n): ";
    std::cin >> confirmacao;
    
    if (confirmacao != 's' && confirmacao != 'S') {
        std::cout << "\nCadastro cancelado pelo usuario." << std::endl;
        return;
    }
    
    novaConta.setNcpf(cpf);
    novaConta.setNome(nome);
    novaConta.setSenha(senha);

    if (cntrServicoUsuario->cadastrarConta(novaConta)) {
        std::cout << "\n*** CONTA CADASTRADA COM SUCESSO! ***" << std::endl;
        std::cout << "CPF cadastrado: " << cpf.getValor() << std::endl;
        std::cout << "Agora voce ja pode fazer login!" << std::endl;
    } else {
        std::cout << "\nErro ao cadastrar conta. CPF ja existe no sistema." << std::endl;
    }
}

/**
 * @brief Consulta e exibe os dados da conta do usuário
 * 
 * @param cpf CPF do usuário a ser consultado
 * @return bool true se a conta foi excluída (logout forçado), false caso contrário
 * 
 * @details Implementa a funcionalidade de consulta de conta com:
 * 
 * 1. Recuperação dos dados da conta (CPF, nome)
 * 2. Cálculo do saldo total (soma de todas as carteiras)
 * 3. Exibição formatada dos dados
 * 4. Menu de ações rápidas após consulta
 * 
 * Informações exibidas:
 * - CPF do usuário
 * - Nome do usuário
 * - Saldo total consolidado de todas as carteiras
 * 
 * Ações disponíveis após consulta:
 * - Editar dados da conta
 * - Excluir conta permanentemente
 * - Voltar ao menu anterior
 * 
 * @note O saldo total é calculado dinamicamente a partir de todas as carteiras
 * @note Se a conta for excluída, retorna true para forçar logout
 */
bool ControladoraApresentacaoUsuario::consultarConta(const Ncpf& cpf) {
    Conta conta;
    Dinheiro saldo;
    
    if (cntrServicoUsuario->consultarConta(cpf, &conta, &saldo)) {
        std::cout << "\n=== DADOS DA CONTA ===" << std::endl;
        std::cout << "CPF   : " << conta.getNcpf().getValor() << std::endl;
        std::cout << "Nome  : " << conta.getNome().getValor() << std::endl;
        std::cout << "Saldo Total: R$ " << saldo.getValor() << std::endl;
        std::cout << "======================" << std::endl;
        
        int opcao;
        std::cout << "\nO que deseja fazer agora?" << std::endl;
        std::cout << "1. Editar dados da conta" << std::endl;
        std::cout << "2. Excluir minha conta" << std::endl;
        std::cout << "0. Voltar ao menu anterior" << std::endl;
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                editarConta(cpf);
                break;
            case 2:
                if (excluirConta(cpf)) {
                    return true;
                }
                break;
            case 0:
                break;
            default:
                std::cout << "Opcao invalida!" << std::endl;
                telaUtils::pausar();
        }
    } else {
        std::cout << "\nErro ao consultar conta." << std::endl;
        telaUtils::pausar();
    }
    
    return false; 
}

/**
 * @brief Edita os dados da conta do usuário
 * 
 * @param cpf CPF do usuário cuja conta será editada
 * 
 * @details Implementa a funcionalidade de edição de conta com:
 * 
 * 1. Consulta dos dados atuais da conta
 * 2. Apresentação das opções de edição
 * 3. Validação dos novos dados
 * 4. Atualização no banco de dados
 * 
 * Campos editáveis:
 * - Nome: até 20 caracteres (letras, números e espaços)
 * - Senha: 6 caracteres com regras específicas
 * 
 * Opções de edição:
 * - 1: Editar apenas o nome
 * - 2: Editar apenas a senha
 * - 3: Editar nome e senha
 * - 0: Cancelar operação
 * 
 * Validações aplicadas:
 * - Nome: máximo 20 caracteres, sem espaços duplos
 * - Senha: 6 caracteres com 1 maiúscula, 1 minúscula, 1 número e 1 símbolo
 * 
 * @note O usuário pode cancelar a operação a qualquer momento digitando '0'
 * @note O CPF não pode ser alterado após o cadastro
 */
void ControladoraApresentacaoUsuario::editarConta(const Ncpf& cpf) {
    std::cout << "\n--- Edicao de Conta ---" << std::endl;
    
    Conta conta;
    Dinheiro saldo;
    
    if (!cntrServicoUsuario->consultarConta(cpf, &conta, &saldo)) {
        std::cout << "Erro ao consultar conta." << std::endl;
        return;
    }
    
    std::cout << "Dados atuais:" << std::endl;
    std::cout << "Nome: " << conta.getNome().getValor() << std::endl;
    std::cout << "Senha: ****** (6 caracteres)" << std::endl;
    
    int opcao;
    std::cout << "\nO que deseja editar?" << std::endl;
    std::cout << "1. Nome" << std::endl;
    std::cout << "2. Senha" << std::endl;
    std::cout << "3. Ambos" << std::endl;
    std::cout << "0. Cancelar" << std::endl;
    std::cout << "Escolha: ";
    std::cin >> opcao;
    
    if (opcao == 0) return;
    
    std::string valor;
    Nome novoNome = conta.getNome();
    Senha novaSenha = conta.getSenha();
    
    try {
        if (opcao == 1 || opcao == 3) {
            std::cin.ignore();
            std::cout << "Nome atual: " << conta.getNome().getValor() << std::endl;
            std::cout << "Novo nome (ate 20 caracteres, ou 0 para cancelar): ";
            std::getline(std::cin, valor);
            
            if (valor == "0") {
                std::cout << "Edicao de conta cancelada pelo usuario." << std::endl;
                return;
            }
            
            novoNome.setValor(valor);
        }
        
        if (opcao == 2 || opcao == 3) {
            std::cout << "Senha atual: ****** (6 caracteres)" << std::endl;
            std::cout << "Nova senha (6 caracteres, ou 0 para cancelar): ";
            std::cin >> valor;
            
            if (valor == "0") {
                std::cout << "Edicao de conta cancelada pelo usuario." << std::endl;
                return;
            }
            
            novaSenha.setValor(valor);
        }
        
        conta.setNome(novoNome);
        conta.setSenha(novaSenha);
        
        if (cntrServicoUsuario->editarConta(conta)) {
            std::cout << "\nConta editada com sucesso!" << std::endl;
        } else {
            std::cout << "\nErro ao editar conta." << std::endl;
        }
        
    } catch (const std::invalid_argument &exp) {
        std::cout << "\nErro: " << exp.what() << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

/**
 * @brief Exclui permanentemente a conta do usuário
 * 
 * @param cpf CPF do usuário cuja conta será excluída
 * @return bool true se a conta foi excluída com sucesso, false caso contrário
 * 
 * @details Implementa a exclusão segura de conta com:
 * 
 * 1. Aviso sobre a natureza irreversível da operação
 * 2. Solicitação de confirmação explícita
 * 3. Validação de integridade referencial
 * 4. Exclusão em cascata (se aplicável)
 * 
 * Validações de integridade:
 * - Verifica se existem carteiras associadas à conta
 * - Bloqueia a exclusão se houver dependências
 * - Fornece orientações claras sobre como proceder
 * 
 * Confirmação de segurança:
 * - Usuário deve digitar 's' ou 'S' para confirmar
 * - Qualquer outra entrada cancela a operação
 * 
 * Comportamento após exclusão:
 * - Exibe mensagem de sucesso
 * - Força logout do sistema (retorna true)
 * - Agradece pela utilização do sistema
 * 
 * @note A exclusão é permanente e não pode ser desfeita
 * @note Todas as carteiras e ordens devem ser excluídas primeiro
 * @note O retorno true força o logout do usuário
 * 
 * @return true se a conta foi excluída (força logout)
 * @return false se a exclusão falhou ou foi cancelada
 */
bool ControladoraApresentacaoUsuario::excluirConta(const Ncpf& cpf) {
    std::cout << "\n--- Exclusao de Conta ---" << std::endl;
    std::cout << "ATENCAO: Esta operacao e irreversivel!" << std::endl;
    std::cout << "Tem certeza que deseja excluir sua conta? (s/N): ";
    
    char confirmacao;
    std::cin >> confirmacao;
    
    if (confirmacao == 's' || confirmacao == 'S') {
        if (cntrServicoUsuario->excluirConta(cpf)) {
            std::cout << "\nConta excluida com sucesso!" << std::endl;
            std::cout << "Obrigado por utilizar nosso sistema." << std::endl;
            
            std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
            std::cin.ignore();
            std::cin.get();
            
            return true; 
        } else {
            std::cout << "\nErro ao excluir conta. Verifique se nao existem carteiras associadas." << std::endl;
            std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    } else {
        std::cout << "\nOperacao cancelada." << std::endl;
    }
    
    return false; 
}

/**
 * @brief Construtor da controladora de apresentação de investimentos
 */
ControladoraApresentacaoInvestimento::ControladoraApresentacaoInvestimento() 
    : cntrServicoInvestimento(nullptr), carteiraController(nullptr), ordemController(nullptr) {
}

/**
 * @brief Destrutor da controladora de apresentação de investimentos
 */
ControladoraApresentacaoInvestimento::~ControladoraApresentacaoInvestimento() {
    delete carteiraController;
    delete ordemController;
}

/**
 * @brief Define a controladora de serviço para investimentos
 * 
 * @param cntrServicoInvestimento Ponteiro para a interface de serviço de investimentos
 */
void ControladoraApresentacaoInvestimento::setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) {
    this->cntrServicoInvestimento = cntrServicoInvestimento;
    
    if (carteiraController) {
        delete carteiraController;
    }
    if (ordemController) {
        delete ordemController;
    }
    
    carteiraController = new CarteiraController(cntrServicoInvestimento);
    ordemController = new OrdemController(cntrServicoInvestimento);
}

/**
 * @brief Executa o menu principal de investimentos
 * 
 * @param cpf CPF do usuário autenticado
 */
/**
 * @brief Executa o menu principal de investimentos
 * 
 * @param cpf CPF do usuário autenticado
 * 
 * @details Implementa o menu central de investimentos com duas opções principais:
 * 
 * 1. **Gerenciar Carteiras**: Acesso direto ao CarteiraController
 *    - Criar novas carteiras
 *    - Listar e consultar carteiras existentes
 *    - Editar carteiras
 *    - Excluir carteiras
 * 
 * 2. **Gerenciar Ordens**: Seleção de carteira seguida de OrdemController
 *    - Lista todas as carteiras do usuário
 *    - Permite seleção por código
 *    - Redireciona para o gerenciamento de ordens da carteira selecionada
 * 
 * Fluxo de gerenciamento de ordens:
 * - Verifica se o usuário possui carteiras
 * - Exibe lista de carteiras disponíveis
 * - Solicita código da carteira desejada
 * - Valida o código informado
 * - Executa o menu de ordens para a carteira selecionada
 * 
 * Validações implementadas:
 * - Verifica existência de carteiras antes de permitir gerenciamento de ordens
 * - Valida formato do código da carteira (5 dígitos)
 * - Trata erros de entrada com mensagens informativas
 * 
 * @note O menu é executado em loop até que o usuário escolha sair (opção 0)
 * @note Utiliza controladores especializados para cada tipo de operação
 */
void ControladoraApresentacaoInvestimento::executar(const Ncpf& cpf) {
    int opcao;
    
    while (true) {
        telaUtils::exibirCabecalho("MENU DE INVESTIMENTOS");
        std::cout << "1. Gerenciar Carteiras" << std::endl;
        std::cout << "2. Gerenciar Ordens (selecionar carteira)" << std::endl;
        std::cout << "0. Voltar ao menu principal" << std::endl;
        telaUtils::exibirSeparador('-', 40);
        std::cout << "Escolha uma opção: ";
        
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                carteiraController->executarMenu(cpf);
                break;
            case 2: {
                std::list<Carteira> carteiras;
                if (cntrServicoInvestimento->listarCarteiras(cpf, &carteiras)) {
                    if (carteiras.empty()) {
                        std::cout << "Você não possui carteiras. Crie uma primeiro." << std::endl;
                        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                        std::cin.ignore();
                        std::cin.get();
                        break;
                    }
                    
                    std::cout << "\nSuas carteiras:" << std::endl;
                    for (const auto& carteira : carteiras) {
                        std::cout << "Código: " << carteira.getCodigo().getValor() 
                                  << " - " << carteira.getNome().getValor() << std::endl;
                    }
                    
                    std::string codigoStr;
                    std::cout << "Digite o código da carteira: ";
                    std::cin >> codigoStr;
                    
                    try {
                        Codigo codigoCarteira;
                        codigoCarteira.setValor(codigoStr);
                        ordemController->executarMenu(codigoCarteira);
                    } catch (const std::invalid_argument &exp) {
                        std::cout << "Código inválido: " << exp.what() << std::endl;
                        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                        std::cin.ignore();
                        std::cin.get();
                    }
                } else {
                    std::cout << "Erro ao listar carteiras." << std::endl;
                    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                    std::cin.ignore();
                    std::cin.get();
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "Opcao invalida!" << std::endl;
        }
    }
}


/**
 * @brief Construtor do gerenciador de interface
 * 
 * @param cntrApresentacaoAutenticacao Controladora de apresentação de autenticação
 * @param cntrApresentacaoUsuario Controladora de apresentação de usuário
 * @param cntrApresentacaoInvestimento Controladora de apresentação de investimentos
 */
InterfaceManager::InterfaceManager(
    ControladoraApresentacaoAutenticacao* cntrApresentacaoAutenticacao,
    ControladoraApresentacaoUsuario* cntrApresentacaoUsuario,
    ControladoraApresentacaoInvestimento* cntrApresentacaoInvestimento
) : cntrApresentacaoAutenticacao(cntrApresentacaoAutenticacao),
    cntrApresentacaoUsuario(cntrApresentacaoUsuario),
    cntrApresentacaoInvestimento(cntrApresentacaoInvestimento),
    telaAtual(TelaAtual::MENU_INICIAL),
    usuarioAutenticado(false) {
}

/**
 * @brief Limpa a tela (método mantido para compatibilidade)
 * 
 * @note A limpeza de tela agora é feita pela telaUtils::exibirCabecalho()
 */
void InterfaceManager::limparTela() {
}

/**
 * @brief Exibe o menu inicial do sistema
 */
void InterfaceManager::mostrarMenuInicial() {
    std::cout << "\n=== GERENCIAMENTO DE CONTA ===" << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Cadastrar nova conta" << std::endl;
    std::cout << "0. Sair" << std::endl;
    std::cout << "Escolha uma opção: ";
}

/**
 * @brief Exibe o menu principal do sistema
 */
void InterfaceManager::mostrarMenuPrincipal() {
    telaUtils::exibirCabecalho("MENU PRINCIPAL");
    std::cout << "Usuário: " << cpfAutenticado.getValor() << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "1. Gerenciar Conta" << std::endl;
    std::cout << "2. Gerenciar Investimentos" << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "Escolha uma opção: ";
}

/**
 * @brief Processa a seleção do menu inicial
 */
void InterfaceManager::processarMenuInicial() {
    int opcao;
    std::cin >> opcao;
    
    switch (opcao) {
        case 1:
            telaAtual = TelaAtual::LOGIN;
            break;
        case 2:
            telaAtual = TelaAtual::CADASTRO;
            break;
        case 0:
            telaAtual = TelaAtual::SAIR;
            break;
        default:
            std::cout << "Opção inválida!" << std::endl;
            std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
            std::cin.ignore();
            std::cin.get();
    }
}

/**
 * @brief Processa a seleção do menu principal
 */
void InterfaceManager::processarMenuPrincipal() {
    int opcao;
    std::cin >> opcao;
    
    switch (opcao) {
        case 1:
            telaAtual = TelaAtual::GERENCIAR_CONTA;
            break;
        case 2:
            telaAtual = TelaAtual::GERENCIAR_INVESTIMENTOS;
            break;
        case 0:
            fazerLogout();
            break;
        default:
            std::cout << "Opção inválida!" << std::endl;
            std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
            std::cin.ignore();
            std::cin.get();
    }
}

/**
 * @brief Processa o gerenciamento de conta
 */
void InterfaceManager::processarGerenciarConta() {
    if (cntrApresentacaoUsuario->executar(cpfAutenticado)) {
        fazerLogout();
    } else {
        telaAtual = TelaAtual::MENU_PRINCIPAL;
    }
}

/**
 * @brief Processa o gerenciamento de investimentos
 */
void InterfaceManager::processarGerenciarInvestimentos() {
    cntrApresentacaoInvestimento->executar(cpfAutenticado);
    telaAtual = TelaAtual::MENU_PRINCIPAL;
}

/**
 * @brief Realiza o logout do usuário
 */
void InterfaceManager::fazerLogout() {
    telaUtils::exibirCabecalho("GERENCIAMENTO DE CONTA");
    std::cout << "Logout realizado com sucesso!" << std::endl;
    usuarioAutenticado = false;
    telaAtual = TelaAtual::MENU_INICIAL;
}

/**
 * @brief Executa o loop principal do sistema
 */
/**
 * @brief Executa o loop principal do sistema
 * 
 * @details Implementa a máquina de estados principal do sistema, controlando
 * o fluxo de navegação entre diferentes telas e funcionalidades.
 * 
 * Estados do sistema:
 * - **MENU_INICIAL**: Tela de entrada com opções de login e cadastro
 * - **LOGIN**: Processo de autenticação do usuário
 * - **CADASTRO**: Processo de criação de nova conta
 * - **MENU_PRINCIPAL**: Menu principal para usuários autenticados
 * - **GERENCIAR_CONTA**: Funcionalidades de gerenciamento de conta
 * - **GERENCIAR_INVESTIMENTOS**: Funcionalidades de investimento
 * - **SAIR**: Encerramento do sistema
 * 
 * Fluxo de autenticação:
 * - Sucesso: redireciona para MENU_PRINCIPAL
 * - Falha: retorna para MENU_INICIAL
 * - Feedback visual para o usuário
 * 
 * Controle de sessão:
 * - Mantém estado de autenticação
 * - Armazena CPF do usuário logado
 * - Gerencia transições entre telas
 * 
 * Tratamento de saída:
 * - Logout automático em caso de exclusão de conta
 * - Mensagem de despedida ao encerrar
 * - Limpeza de recursos
 * 
 * @note O loop continua até que telaAtual seja definida como SAIR
 * @note Cada estado é processado por métodos específicos
 * @note O sistema mantém consistência de estado durante toda a execução
 */
void InterfaceManager::executar() {
    while (telaAtual != TelaAtual::SAIR) {
        switch (telaAtual) {
            case TelaAtual::MENU_INICIAL:
                mostrarMenuInicial();
                processarMenuInicial();
                break;
                
            case TelaAtual::LOGIN:
                if (cntrApresentacaoAutenticacao->autenticar(&cpfAutenticado)) {
                    std::cout << "\n>>> Autenticação realizada com sucesso <<<" << std::endl;
                    std::cout << "Usuário autenticado: " << cpfAutenticado.getValor() << std::endl;
                    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                    std::cin.ignore();
                    std::cin.get();
                    usuarioAutenticado = true;
                    telaAtual = TelaAtual::MENU_PRINCIPAL;
                } else {
                    std::cout << "\n>>> Falha na autenticação. CPF ou senha inválidos. <<<" << std::endl;
                    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                    std::cin.ignore();
                    std::cin.get();
                    telaAtual = TelaAtual::MENU_INICIAL;
                }
                break;
                
            case TelaAtual::CADASTRO:
                cntrApresentacaoUsuario->cadastrar();
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                telaAtual = TelaAtual::MENU_INICIAL;
                break;
                
            case TelaAtual::MENU_PRINCIPAL:
                mostrarMenuPrincipal();
                processarMenuPrincipal();
                break;
                
            case TelaAtual::GERENCIAR_CONTA:
                processarGerenciarConta();
                break;
                
            case TelaAtual::GERENCIAR_INVESTIMENTOS:
                processarGerenciarInvestimentos();
                break;
                
            case TelaAtual::SAIR:
                break;
        }
    }
    
    std::cout << "Obrigado por utilizar nosso sistema!" << std::endl;
} 