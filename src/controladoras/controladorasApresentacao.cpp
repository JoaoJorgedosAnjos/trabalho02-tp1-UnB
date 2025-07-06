#include <iostream>
#include <iomanip>
#include <cctype>
#include <limits>
#include <fstream>
#include <cstdlib>
#include "controladorasApresentacao.hpp"

// =================================================================================================
// FUNÇÕES AUXILIARES PARA FORMATAÇÃO DE ENTRADA E INTERFACE
// =================================================================================================



/**
 * @brief Formata uma string de números para o padrão de CPF brasileiro
 * 
 * @details Esta função implementa uma máscara automática que permite ao usuário
 * digitar apenas os 11 dígitos do CPF, sem se preocupar com pontos e hífen.
 * A função automaticamente adiciona a formatação necessária.
 * 
 * Exemplos de transformação:
 * - "04348848122" -> "043.488.481-22"
 * - "12345678901" -> "123.456.789-01"
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
 * std::string cpfFormatado = formatarCPF("04348848122");
 * // Resultado: "043.488.481-22"
 * @endcode
 */
std::string formatarCPF(const std::string& entrada) {
    // Remove todos os caracteres não numéricos da entrada
    std::string apenasNumeros;
    for (char c : entrada) {
        if (std::isdigit(c)) {
            apenasNumeros += c;
        }
    }
    
    // Verifica se temos exatamente 11 dígitos
    if (apenasNumeros.length() != 11) {
        throw std::invalid_argument("CPF deve conter exatamente 11 dígitos numéricos");
    }
    
    // Aplica a máscara XXX.XXX.XXX-XX
    std::string cpfFormatado;
    cpfFormatado += apenasNumeros.substr(0, 3);  // Primeiros 3 dígitos
    cpfFormatado += ".";
    cpfFormatado += apenasNumeros.substr(3, 3);  // Próximos 3 dígitos
    cpfFormatado += ".";
    cpfFormatado += apenasNumeros.substr(6, 3);  // Próximos 3 dígitos
    cpfFormatado += "-";
    cpfFormatado += apenasNumeros.substr(9, 2);  // Últimos 2 dígitos (verificadores)
    
    return cpfFormatado;
}

// =================================================================================================
// CONTROLADORA DE AUTENTICAÇÃO
// =================================================================================================

// Implementação do método set para injeção de dependência.
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
 * 1. Apenas números: "04348848122" (será formatado automaticamente)
 * 2. Já formatado: "043.488.481-22" (será aceito se válido)
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
        std::cout << "\n--- Autenticacao de Usuario ---" << std::endl;
        std::cout << "(Digite '0' a qualquer momento para cancelar)" << std::endl;

        try {
            // Entrada flexível de CPF com máscara automática
            std::cout << "CPF (apenas numeros ou XXX.XXX.XXX-XX): ";
            std::cin >> valor;
            
            // Verificar se o usuário quer cancelar
            if (valor == "0") {
                std::cout << "Login cancelado pelo usuario." << std::endl;
                return false;
            }
            
            // Aplica a máscara automática se necessário
            try {
                std::string cpfFormatado = formatarCPF(valor);
                std::cout << "CPF formatado: " << cpfFormatado << std::endl;
                cpf->setValor(cpfFormatado); // Validação completa ocorre aqui
            } catch (const std::invalid_argument& e) {
                // Se falhou na formatação, tenta usar o valor original
                // (pode ser que já esteja formatado corretamente)
                cpf->setValor(valor);
            }

            std::cout << "Senha (6 caracteres)    : ";
            std::cin >> valor;
            
            // Verificar se o usuário quer cancelar
            if (valor == "0") {
                std::cout << "Login cancelado pelo usuario." << std::endl;
                return false;
            }
            
            senha.setValor(valor); // Validação ocorre aqui, no domínio
            
            break; // Se ambos os dados forem válidos, sai do loop.

        } catch (const std::invalid_argument &exp) {
            // Se a validação no domínio falhar, uma exceção é capturada.
            std::cout << "\nErro: " << exp.what() << std::endl;
            std::cout << "\nDica para CPF: Digite apenas os 11 numeros (ex: 04348848122)" << std::endl;
            std::cout << "Dica para Senha: Use 1 maiuscula, 1 minuscula, 1 numero e 1 simbolo (#$%&)" << std::endl;
            std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    }

    // Chama o método da camada de serviço através da interface.
    // A camada de apresentação não sabe como a autenticação é feita, ela apenas delega.
    return cntrServicoAutenticacao->autenticar(*cpf, senha);
}

// =================================================================================================
// CONTROLADORA DE USUÁRIO
// =================================================================================================

void ControladoraApresentacaoUsuario::setControladoraServico(IServicoUsuario* cntrServicoUsuario) {
    this->cntrServicoUsuario = cntrServicoUsuario;
}

bool ControladoraApresentacaoUsuario::executar(const Ncpf& cpf) {
    int opcao = -1;
    while (opcao != 0) {
        std::cout << "\n--- Gerenciamento de Conta ---" << std::endl;
        std::cout << "1. Consultar dados da conta." << std::endl;
        std::cout << "2. Editar dados da conta." << std::endl;
        std::cout << "3. Excluir minha conta." << std::endl;
        std::cout << "0. Voltar ao menu principal." << std::endl;
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                if (consultarConta(cpf)) {
                    // Conta foi excluída a partir do sub-menu de consulta
                    return true;
                }
                break;
            case 2:
                editarConta(cpf);
                break;
            case 3:
                if (excluirConta(cpf)) {
                    // Conta foi excluída com sucesso, retornar true para fazer logout
                    return true;
                }
                break;
            case 0:
                break;
            default:
                std::cout << "Opcao invalida!" << std::endl;
        }
    }
    return false; // Retorna false quando sai normalmente (não excluiu conta)
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
    std::cout << "\n--- Cadastro de Nova Conta ---" << std::endl;
    std::cout << "(Digite '0' a qualquer momento para cancelar)" << std::endl;
    
    std::string valor;
    Conta novaConta;
    Ncpf cpf;
    Nome nome;
    Senha senha;
    
    // Estados para controlar qual campo está sendo preenchido
    bool cpfValido = false;
    bool nomeValido = false;
    bool senhaValida = false;
    
    while (!cpfValido || !nomeValido || !senhaValida) {
        
        // === CPF ===
        if (!cpfValido) {
            try {
                std::cout << "\n=== 1. CPF ===" << std::endl;
                std::cout << "CPF (apenas numeros ou XXX.XXX.XXX-XX): ";
                std::cin >> valor;

                // Verificar se o usuário quer cancelar
                if (valor == "0") {
                    std::cout << "Cadastro cancelado pelo usuario." << std::endl;
                    return;
                }
                
                // Aplica a máscara automática se necessário
                try {
                    std::string cpfFormatado = formatarCPF(valor);
                    std::cout << "CPF formatado: " << cpfFormatado << std::endl;
                    cpf.setValor(cpfFormatado);
                } catch (const std::invalid_argument& e) {
                    // Se falhou na formatação, tenta usar o valor original
                    cpf.setValor(valor);
                }
                
                cpfValido = true; // CPF válido
                
            } catch (const std::invalid_argument &exp) {
                std::cout << "\nErro no CPF: " << exp.what() << std::endl;
                std::cout << "Dica: Digite apenas os 11 numeros (ex: 04348848122)" << std::endl;
                // Continua direto para o próximo input
            }
        }
        
        // === NOME ===
        else if (!nomeValido) {
            try {
                std::cout << "\n=== 2. NOME ===" << std::endl;
                std::cout << "CPF já cadastrado: " << cpf.getValor() << " ✓" << std::endl;
                std::cout << "(Digite '0' para cancelar ou 'r' para reescrever CPF)" << std::endl;
                
                // Limpa o buffer
                std::cin.ignore();
                std::cout << "Nome (ate 20 caracteres): ";
                std::getline(std::cin, valor);
                
                // Verificar se o usuário quer cancelar
                if (valor == "0") {
                    std::cout << "Cadastro cancelado pelo usuario." << std::endl;
                    return;
                }
                
                // Verificar se o usuário quer reescrever CPF
                if (valor == "r" || valor == "R") {
                    std::cout << "Voltando para reescrever CPF..." << std::endl;
                    cpfValido = false; // Volta para CPF
                    continue;
                }
                
                nome.setValor(valor);
                nomeValido = true; // Nome válido
                
            } catch (const std::invalid_argument &exp) {
                std::cout << "\nErro no Nome: " << exp.what() << std::endl;
                std::cout << "Dica: Maximo 20 caracteres, sem espacos duplos" << std::endl;
                // Continua direto para o próximo input
            }
        }
        
        // === SENHA ===
        else if (!senhaValida) {
            try {
                std::cout << "\n=== 3. SENHA ===" << std::endl;
                std::cout << "CPF já cadastrado: " << cpf.getValor() << " ✓" << std::endl;
                std::cout << "Nome já cadastrado: " << nome.getValor() << " ✓" << std::endl;
                std::cout << "(Digite '0' para cancelar, 'r' para reescrever CPF, 'n' para reescrever nome)" << std::endl;
                
                std::cout << "Senha (6 caracteres): ";
                std::cin >> valor;
                
                // Verificar se o usuário quer cancelar
                if (valor == "0") {
                    std::cout << "Cadastro cancelado pelo usuario." << std::endl;
                    return;
                }
                
                // Verificar se o usuário quer reescrever CPF
                if (valor == "r" || valor == "R") {
                    std::cout << "Voltando para reescrever CPF..." << std::endl;
                    cpfValido = false; // Volta para CPF
                    continue;
                }
                
                // Verificar se o usuário quer reescrever nome
                if (valor == "n" || valor == "N") {
                    std::cout << "Voltando para reescrever nome..." << std::endl;
                    nomeValido = false; // Volta para nome
                    continue;
                }
                
                senha.setValor(valor);
                senhaValida = true; // Senha válida
                
            } catch (const std::invalid_argument &exp) {
                std::cout << "\nErro na Senha: " << exp.what() << std::endl;
                std::cout << "Dica: 6 caracteres com 1 maiuscula, 1 minuscula, 1 numero e 1 simbolo (#$%&)" << std::endl;
                // Continua direto para o próximo input
            }
        }
    }
    
    // === RESUMO E CONFIRMAÇÃO ===
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
    
    // === CRIAR CONTA ===
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

bool ControladoraApresentacaoUsuario::consultarConta(const Ncpf& cpf) {
    Conta conta;
    Dinheiro saldo;
    
    if (cntrServicoUsuario->consultarConta(cpf, &conta, &saldo)) {
        std::cout << "\n=== DADOS DA CONTA ===" << std::endl;
        std::cout << "CPF   : " << conta.getNcpf().getValor() << std::endl;
        std::cout << "Nome  : " << conta.getNome().getValor() << std::endl;
        std::cout << "Saldo Total: R$ " << saldo.getValor() << std::endl;
        std::cout << "======================" << std::endl;
        
        // Sub-menu após mostrar os dados
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
                    // Se a conta foi excluída, retorna true para fazer logout
                    return true;
                }
                break;
            case 0:
                break;
            default:
                std::cout << "Opcao invalida!" << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
        }
    } else {
        std::cout << "\nErro ao consultar conta." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
    }
    
    return false; // Retorna false se a conta não foi excluída
}

void ControladoraApresentacaoUsuario::editarConta(const Ncpf& cpf) {
    std::cout << "\n--- Edicao de Conta ---" << std::endl;
    
    Conta conta;
    Dinheiro saldo;
    
    // Primeiro consulta os dados atuais
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
            std::cout << "Novo nome (ate 20 caracteres): ";
            std::getline(std::cin, valor);
            novoNome.setValor(valor);
        }
        
        if (opcao == 2 || opcao == 3) {
            std::cout << "Senha atual: ****** (6 caracteres)" << std::endl;
            std::cout << "Nova senha (6 caracteres): ";
            std::cin >> valor;
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
            
            return true; // Indica que a conta foi excluída
        } else {
            std::cout << "\nErro ao excluir conta. Verifique se nao existem carteiras associadas." << std::endl;
            std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    } else {
        std::cout << "\nOperacao cancelada." << std::endl;
        // Não pede para apertar tecla, volta diretamente ao menu anterior
    }
    
    return false; // Indica que a conta não foi excluída
}

// =================================================================================================
// CONTROLADORA DE INVESTIMENTO
// =================================================================================================

void ControladoraApresentacaoInvestimento::setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) {
    this->cntrServicoInvestimento = cntrServicoInvestimento;
}

void ControladoraApresentacaoInvestimento::executar(const Ncpf& cpf) {
    int opcao;
    
    while (true) {
        std::cout << "\n=== MENU DE INVESTIMENTOS ===" << std::endl;
        std::cout << "1. Gerenciar Carteiras" << std::endl;
        std::cout << "2. Gerenciar Ordens (selecionar carteira)" << std::endl;
        std::cout << "0. Voltar ao menu principal" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                menuCarteiras(cpf);
                break;
            case 2: {
                // Primeiro lista as carteiras para o usuário escolher
                std::list<Carteira> carteiras;
                if (cntrServicoInvestimento->listarCarteiras(cpf, &carteiras)) {
                    if (carteiras.empty()) {
                        std::cout << "Você não possui carteiras. Crie uma primeiro." << std::endl;
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
                        menuOrdens(codigoCarteira);
                    } catch (const std::invalid_argument &exp) {
                        std::cout << "Código inválido: " << exp.what() << std::endl;
                    }
                } else {
                    std::cout << "Erro ao listar carteiras." << std::endl;
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

void ControladoraApresentacaoInvestimento::menuCarteiras(const Ncpf& cpf) {
    int opcao;
    
    while (true) {
        std::cout << "\n=== MENU DE CARTEIRAS ===" << std::endl;
        std::cout << "1. Criar carteira" << std::endl;
        std::cout << "2. Listar e consultar carteiras" << std::endl;
        std::cout << "3. Editar carteira" << std::endl;
        std::cout << "4. Excluir carteira" << std::endl;
        std::cout << "0. Voltar" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                criarCarteira(cpf);
                break;
            case 2:
                listarCarteiras(cpf);
                break;
            case 3:
                editarCarteira(cpf);
                break;
            case 4:
                excluirCarteira(cpf);
                break;
            case 0:
                return;
            default:
                std::cout << "Opçã inválida!" << std::endl;
        }
    }
}

void ControladoraApresentacaoInvestimento::criarCarteira(const Ncpf& cpf) {
    std::cout << "\n--- Criação de Nova Carteira ---" << std::endl;
    std::cout << "(Digite '0' a qualquer momento para cancelar)" << std::endl;
    
    // Primeiro, verificar quantas carteiras o usuário já possui
    std::list<Carteira> carteirasExistentes;
    bool temCarteiras = cntrServicoInvestimento->listarCarteiras(cpf, &carteirasExistentes);
    
    size_t quantidadeAtual = temCarteiras ? carteirasExistentes.size() : 0;
    
    std::cout << "\nVocê possui atualmente " << quantidadeAtual << " carteira(s)." << std::endl;
    std::cout << "Limite máximo: 5 carteiras por usuário." << std::endl;
    
    if (quantidadeAtual >= 5) {
        std::cout << "\n*** LIMITE ATINGIDO ***" << std::endl;
        std::cout << "Você já possui o máximo de 5 carteiras permitidas." << std::endl;
        std::cout << "Para criar uma nova carteira, exclua uma existente primeiro." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibir carteiras existentes se houver
    if (quantidadeAtual > 0) {
        std::cout << "\n=== SUAS CARTEIRAS ATUAIS ===" << std::endl;
        std::cout << std::left << std::setw(8) << "Codigo" 
                  << std::setw(25) << "Nome" 
                  << std::setw(12) << "Perfil" << std::endl;
        std::cout << std::string(45, '-') << std::endl;
        
        for (const Carteira& carteira : carteirasExistentes) {
            std::cout << std::left << std::setw(8) << carteira.getCodigo().getValor()
                      << std::setw(25) << carteira.getNome().getValor()
                      << std::setw(12) << carteira.getTipoPerfil().getValor() << std::endl;
        }
        std::cout << std::string(45, '-') << std::endl;
    }
    
    // Coleta os dados da nova carteira
    Codigo codigo;
    Nome nome;
    TipoPerfil tipoPerfil;
    
    // Solicitar código da carteira
    while (true) {
        try {
            std::cout << "\nDigite o código da nova carteira (5 digitos): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            
            // Verificar se o usuário quer cancelar
            if (valorCodigo == "0") {
                std::cout << "Criacao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            codigo.setValor(valorCodigo);
            break; // Código válido
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use exatamente 5 digitos numericos (ex: 12345)." << std::endl;
        }
    }
    
    // Solicitar nome da carteira
    while (true) {
        try {
            std::cout << "Digite o nome da carteira: ";
            std::string valorNome;
            std::cin.ignore(); // Limpa o buffer do cin anterior
            std::getline(std::cin, valorNome);
            
            // Verificar se o usuário quer cancelar
            if (valorNome == "0") {
                std::cout << "Criacao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            nome.setValor(valorNome);
            break; // Nome válido
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use ate 20 caracteres (letras, numeros e espacos)." << std::endl;
        }
    }
    
    // Solicitar perfil da carteira
    while (true) {
        try {
            std::cout << "\nTipos de perfil disponiveis:" << std::endl;
            std::cout << "1. Conservador" << std::endl;
            std::cout << "2. Moderado" << std::endl;
            std::cout << "3. Agressivo" << std::endl;
            std::cout << "0. Cancelar criacao" << std::endl;
            std::cout << "Escolha o perfil (1-3) ou 0 para cancelar: ";
            
            int opcaoPerfil;
            std::cin >> opcaoPerfil;
            
            // Verificar se o usuário quer cancelar
            if (opcaoPerfil == 0) {
                std::cout << "Criacao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            std::string valorPerfil;
            switch (opcaoPerfil) {
                case 1:
                    valorPerfil = "Conservador";
                    break;
                case 2:
                    valorPerfil = "Moderado";
                    break;
                case 3:
                    valorPerfil = "Agressivo";
                    break;
                default:
                    std::cout << "Opcao invalida! Escolha entre 1 e 3, ou 0 para cancelar." << std::endl;
                    continue;
            }
            
            tipoPerfil.setValor(valorPerfil);
            break; // Perfil válido
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
        }
    }
    
    // Criar objeto Carteira com os dados coletados
    Carteira novaCarteira;
    novaCarteira.setCodigo(codigo);
    novaCarteira.setNome(nome);
    novaCarteira.setTipoPerfil(tipoPerfil);
    
    // Exibir resumo para confirmação
    std::cout << "\n=== RESUMO DA NOVA CARTEIRA ===" << std::endl;
    std::cout << "Codigo: " << codigo.getValor() << std::endl;
    std::cout << "Nome  : " << nome.getValor() << std::endl;
    std::cout << "Perfil: " << tipoPerfil.getValor() << std::endl;
    std::cout << "===============================" << std::endl;
    
    std::cout << "\nConfirma a criacao da carteira? (s/n): ";
    char confirmacao;
    std::cin >> confirmacao;
    
    if (confirmacao != 's' && confirmacao != 'S') {
        std::cout << "\nCriacao da carteira cancelada pelo usuario." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Tentar criar a carteira
    if (cntrServicoInvestimento->criarCarteira(cpf, novaCarteira)) {
        std::cout << "\n*** SUCESSO! ***" << std::endl;
        std::cout << "Carteira '" << nome.getValor() << "' criada com sucesso!" << std::endl;
        std::cout << "Codigo: " << codigo.getValor() << std::endl;
        std::cout << "Voce agora possui " << (quantidadeAtual + 1) << " carteira(s)." << std::endl;
        
    } else {
        std::cout << "\n*** ERRO! ***" << std::endl;
        std::cout << "Nao foi possivel criar a carteira." << std::endl;
        std::cout << "Possiveis causas:" << std::endl;
        std::cout << "- Codigo ja existe no sistema (deve ser unico)" << std::endl;
        std::cout << "- Limite de carteiras atingido" << std::endl;
        std::cout << "- Erro interno do sistema" << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void ControladoraApresentacaoInvestimento::listarCarteiras(const Ncpf& cpf) {
    while (true) {
        std::cout << "\n--- Lista de Carteiras ---" << std::endl;
        
        std::list<Carteira> carteiras;
        
        if (!cntrServicoInvestimento->listarCarteiras(cpf, &carteiras) || carteiras.empty()) {
            std::cout << "\nVoce ainda nao possui carteiras." << std::endl;
            std::cout << "\n=== OPCOES ===" << std::endl;
            std::cout << "1. Criar nova carteira" << std::endl;
            std::cout << "0. Voltar ao menu anterior" << std::endl;
            std::cout << "Escolha uma opcao: ";
            
            int opcao;
            std::cin >> opcao;
            
            switch (opcao) {
                case 1:
                    criarCarteira(cpf);
                    // Após criar, volta ao loop para mostrar a lista atualizada
                    continue;
                case 0:
                    return;
                default:
                    std::cout << "Opcao invalida!" << std::endl;
                    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                    std::cin.ignore();
                    std::cin.get();
                    continue;
            }
        }
        
        // Exibe a lista de carteiras
        std::cout << "\n=== SUAS CARTEIRAS ===" << std::endl;
        std::cout << std::left << std::setw(8) << "Codigo" 
                  << std::setw(25) << "Nome" 
                  << std::setw(12) << "Perfil" << std::endl;
        std::cout << std::string(45, '-') << std::endl;
        
        for (const Carteira& carteira : carteiras) {
        // for (const auto& carteira : carteiras) {  // Versão com auto para comparação
            std::cout << std::left << std::setw(8) << carteira.getCodigo().getValor()
                      << std::setw(25) << carteira.getNome().getValor()
                      << std::setw(12) << carteira.getTipoPerfil().getValor() << std::endl;
        }
        std::cout << std::string(45, '-') << std::endl;
        std::cout << "Total de carteiras: " << carteiras.size() << std::endl;
        
        // Opções interativas
        std::cout << "\n>>> OPCOES <<<" << std::endl;
        std::cout << "- Digite o CODIGO de uma carteira para ver detalhes" << std::endl;
        std::cout << "- Digite 0 para voltar ao menu anterior" << std::endl;
        std::cout << "Escolha: ";
        
        std::string entrada;
        std::cin >> entrada;
        
        // Se digitou 0, sai da função
        if (entrada == "0") {
            return;
        }
        
        // Tenta consultar a carteira
        try {
            Codigo codigo;
            codigo.setValor(entrada);
            
            std::cout << "[DEBUG] Tentando consultar carteira com codigo: '" << codigo.getValor() << "'" << std::endl;
            
            Carteira carteiraDetalhada;
            Dinheiro saldo;
            
            if (cntrServicoInvestimento->consultarCarteira(codigo, &carteiraDetalhada, &saldo)) {
                // Exibe os detalhes da carteira selecionada
                std::cout << "\n==============================" << std::endl;
                std::cout << "    DETALHES DA CARTEIRA" << std::endl;
                std::cout << "==============================" << std::endl;
                std::cout << "Codigo     : " << carteiraDetalhada.getCodigo().getValor() << std::endl;
                std::cout << "Nome       : " << carteiraDetalhada.getNome().getValor() << std::endl;
                std::cout << "Perfil     : " << carteiraDetalhada.getTipoPerfil().getValor() << std::endl;
                std::cout << "SALDO TOTAL: R$ " << saldo.getValor() << std::endl;
                std::cout << "==============================" << std::endl;
                
                // === NOVA SEÇÃO: LISTAGEM DAS ORDENS DESTA CARTEIRA ===
                std::list<Ordem> ordensCarteira;
                if (cntrServicoInvestimento->listarOrdens(codigo, &ordensCarteira) && !ordensCarteira.empty()) {
                    std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
                    std::cout << std::left << std::setw(8) << "Codigo"
                              << std::setw(12) << "Data"
                              << std::setw(15) << "Valor"
                              << std::setw(10) << "Quantidade" << std::endl;
                    std::cout << std::string(45, '-') << std::endl;
                    
                    int contador = 1;
                    for (const Ordem& ordem : ordensCarteira) {
                    // for (const auto& ordem : ordensCarteira) {  // Versão com auto para comparação
                        std::cout << std::left << std::setw(8) << ordem.getCodigo().getValor()
                                  << std::setw(12) << ordem.getData().getValor()
                                  << std::setw(15) << ("R$ " + ordem.getDinheiro().getValor())
                                  << std::setw(10) << ordem.getQuantidade().getValor() << std::endl;
                    }
                    std::cout << std::string(45, '-') << std::endl;
                    std::cout << "Total de ordens: " << ordensCarteira.size() << std::endl;
                } else {
                    std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
                    std::cout << "Esta carteira ainda nao possui ordens de investimento." << std::endl;
                    std::cout << "Use o menu de gerenciamento para criar a primeira ordem!" << std::endl;
                }
                
                std::cout << "\nNOTA: O saldo da carteira e calculado pela soma" << std::endl;
                std::cout << "de todas as ordens de investimento listadas acima." << std::endl;
                
                // Submenu de ações para esta carteira específica
                while (true) {
                    std::cout << "\n>>> ACOES PARA ESTA CARTEIRA <<<" << std::endl;
                    std::cout << "1. Editar carteira" << std::endl;
                    std::cout << "2. Excluir carteira" << std::endl;
                    std::cout << "0. Voltar para a lista" << std::endl;
                    std::cout << "Escolha uma acao: ";
                    
                    int acao;
                    std::cin >> acao;
                    
                    switch (acao) {
                        case 1: {
                            // Editar esta carteira específica
                            editarCarteiraEspecifica(cpf, carteiraDetalhada);
                            // Após editar, volta para a lista principal (break do while interno)
                            break;
                        }
                        case 2: {
                            // Excluir esta carteira específica
                            if (excluirCarteiraEspecifica(cpf, carteiraDetalhada)) {
                                // Se excluiu com sucesso, volta para a lista (que será atualizada)
                                std::cout << "\nVoltando para a lista atualizada..." << std::endl;
                            }
                            break;
                        }
                        case 0:
                            // Volta para a lista
                            break;
                        default:
                            std::cout << "Opcao invalida! Tente novamente." << std::endl;
                            continue; // Continua no submenu
                    }
                    break; // Sai do submenu
                }
                // Loop continua, volta para a lista
                
            } else {
                std::cout << "\nErro: Carteira com codigo '" << entrada << "' nao foi encontrada." << std::endl;
                std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
                std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                // Loop continua, volta para a lista
            }
            
        } catch (const std::invalid_argument &exp) {
            std::cout << "\nErro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima." << std::endl;
            std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
            std::cin.ignore();
            std::cin.get();
            // Loop continua, volta para a lista
        }
    }
}

void ControladoraApresentacaoInvestimento::consultarCarteira(const Ncpf& cpf) {
    std::cout << "\n--- Consulta Detalhada de Carteira ---" << std::endl;
    
    // Primeiro lista as carteiras disponíveis para facilitar a escolha
    std::list<Carteira> carteiras;
    if (!cntrServicoInvestimento->listarCarteiras(cpf, &carteiras)) {
        std::cout << "\nVoce ainda nao possui carteiras cadastradas." << std::endl;
        std::cout << "Use a opcao 'Criar carteira' para comecar!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    if (carteiras.empty()) {
        std::cout << "\nVoce ainda nao possui carteiras cadastradas." << std::endl;
        std::cout << "Use a opcao 'Criar carteira' para comecar!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibe as carteiras disponíveis
    std::cout << "\n=== SUAS CARTEIRAS DISPONIVEIS ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Codigo" 
              << std::setw(18) << "Nome" 
              << std::setw(12) << "Perfil" << std::endl;
    std::cout << std::string(38, '-') << std::endl;
    
    for (const Carteira& carteira : carteiras) {
        std::cout << std::left << std::setw(8) << carteira.getCodigo().getValor()
                  << std::setw(18) << carteira.getNome().getValor()
                  << std::setw(12) << carteira.getTipoPerfil().getValor() << std::endl;
    }
    std::cout << std::string(38, '-') << std::endl;
    
    // Solicita o código da carteira para consulta detalhada
    Codigo codigo;
    std::string valorCodigo;
    
    while (true) {
        try {
            std::cout << "\nDigite o codigo da carteira para consulta detalhada: ";
            std::cin >> valorCodigo;
            codigo.setValor(valorCodigo);
            break; // Sai do loop se o código for válido
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima." << std::endl;
        }
    }
    
    // Consulta os detalhes da carteira selecionada
    Carteira carteiraDetalhada;
    Dinheiro saldo;
    
    if (cntrServicoInvestimento->consultarCarteira(codigo, &carteiraDetalhada, &saldo)) {
        std::cout << "\n==============================" << std::endl;
        std::cout << "    DETALHES DA CARTEIRA" << std::endl;
        std::cout << "==============================" << std::endl;
        std::cout << "Codigo     : " << carteiraDetalhada.getCodigo().getValor() << std::endl;
        std::cout << "Nome       : " << carteiraDetalhada.getNome().getValor() << std::endl;
        std::cout << "Perfil     : " << carteiraDetalhada.getTipoPerfil().getValor() << std::endl;
        std::cout << "SALDO TOTAL: R$ " << saldo.getValor() << std::endl;
        std::cout << "==============================" << std::endl;
        
        // === LISTAGEM DAS ORDENS DESTA CARTEIRA ===
        std::list<Ordem> ordensCarteira;
        if (cntrServicoInvestimento->listarOrdens(codigo, &ordensCarteira) && !ordensCarteira.empty()) {
            std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
            std::cout << std::left << std::setw(8) << "Codigo"
                      << std::setw(12) << "Data"
                      << std::setw(15) << "Valor"
                      << std::setw(10) << "Quantidade" << std::endl;
            std::cout << std::string(45, '-') << std::endl;
            
            for (const Ordem& ordem : ordensCarteira) {
            // for (const auto& ordem : ordensCarteira) {  // Versão com auto para comparação
                std::cout << std::left << std::setw(8) << ordem.getCodigo().getValor()
                          << std::setw(12) << ordem.getData().getValor()
                          << std::setw(15) << ("R$ " + ordem.getDinheiro().getValor())
                          << std::setw(10) << ordem.getQuantidade().getValor() << std::endl;
            }
            std::cout << std::string(45, '-') << std::endl;
            std::cout << "Total de ordens: " << ordensCarteira.size() << std::endl;
        } else {
            std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
            std::cout << "Esta carteira ainda nao possui ordens de investimento." << std::endl;
            std::cout << "Use o menu de gerenciamento para criar a primeira ordem!" << std::endl;
        }
        
        // Informação adicional sobre a composição do saldo
        std::cout << "\nNOTA: O saldo da carteira e calculado pela soma" << std::endl;
        std::cout << "de todas as ordens de investimento listadas acima." << std::endl;
        
    } else {
        std::cout << "\nErro: Carteira com codigo '" << valorCodigo << "' nao foi encontrada." << std::endl;
        std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void ControladoraApresentacaoInvestimento::editarCarteira(const Ncpf& cpf) {
    std::cout << "\n--- Edicao de Carteira ---" << std::endl;
    
    // Primeiro, listar as carteiras disponíveis do usuário
    std::list<Carteira> carteiras;
    if (!cntrServicoInvestimento->listarCarteiras(cpf, &carteiras) || carteiras.empty()) {
        std::cout << "\nVoce ainda nao possui carteiras para editar." << std::endl;
        std::cout << "Use a opcao 'Criar carteira' primeiro!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibe as carteiras disponíveis
    std::cout << "\n=== SUAS CARTEIRAS ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Codigo" 
              << std::setw(25) << "Nome" 
              << std::setw(12) << "Perfil" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    
    for (const Carteira& carteira : carteiras) {
        std::cout << std::left << std::setw(8) << carteira.getCodigo().getValor()
                  << std::setw(25) << carteira.getNome().getValor()
                  << std::setw(12) << carteira.getTipoPerfil().getValor() << std::endl;
    }
    std::cout << std::string(45, '-') << std::endl;
    std::cout << "Total de carteiras: " << carteiras.size() << std::endl;
    
    // Solicitar código da carteira a ser editada
    Codigo codigo;
    Carteira carteiraAtual;
    
    while (true) {
        try {
            std::cout << "\nDigite o codigo da carteira que deseja editar: ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            codigo.setValor(valorCodigo);
            
            // Buscar a carteira atual para mostrar os dados
            Dinheiro saldoTemp; // Não usado, apenas para a interface
            if (cntrServicoInvestimento->consultarCarteira(codigo, &carteiraAtual, &saldoTemp)) {
                break; // Carteira encontrada
            } else {
                std::cout << "Erro: Carteira com codigo '" << valorCodigo << "' nao foi encontrada." << std::endl;
                std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
            }
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima." << std::endl;
        }
    }
    
    // Exibir dados atuais da carteira
    std::cout << "\n=== DADOS ATUAIS DA CARTEIRA ===" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << " (nao pode ser alterado)" << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "================================" << std::endl;
    
    // Criar nova carteira com os dados atualizados
    Carteira carteiraEditada = carteiraAtual; // Copia todos os dados atuais
    
    // Solicitar novo nome
    while (true) {
        try {
            std::cout << "\nNome atual: " << carteiraAtual.getNome().getValor() << std::endl;
            std::cout << "Digite o novo nome (ou ENTER para manter o atual): ";
            std::string novoNome;
            std::cin.ignore(); // Limpa o buffer
            std::getline(std::cin, novoNome);
            
            if (novoNome.empty()) {
                // Mantém o nome atual
                break;
            } else {
                Nome nome;
                nome.setValor(novoNome);
                carteiraEditada.setNome(nome);
                break;
            }
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use ate 20 caracteres (letras, numeros e espacos)." << std::endl;
        }
    }
    
    // Solicitar novo perfil
    while (true) {
        try {
            std::cout << "\nPerfil atual: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
            std::cout << "Deseja alterar o perfil? (s/n): ";
            char alterarPerfil;
            std::cin >> alterarPerfil;
            
            if (alterarPerfil == 's' || alterarPerfil == 'S') {
                std::cout << "\nTipos de perfil disponiveis:" << std::endl;
                std::cout << "1. Conservador" << std::endl;
                std::cout << "2. Moderado" << std::endl;
                std::cout << "3. Agressivo" << std::endl;
                std::cout << "Escolha o novo perfil (1-3): ";
                
                int opcaoPerfil;
                std::cin >> opcaoPerfil;
                
                std::string valorPerfil;
                switch (opcaoPerfil) {
                    case 1:
                        valorPerfil = "Conservador";
                        break;
                    case 2:
                        valorPerfil = "Moderado";
                        break;
                    case 3:
                        valorPerfil = "Agressivo";
                        break;
                    default:
                        std::cout << "Opcao invalida! Mantendo perfil atual." << std::endl;
                        break; // Mantém o perfil atual
                }
                
                if (!valorPerfil.empty()) {
                    TipoPerfil tipoPerfil;
                    tipoPerfil.setValor(valorPerfil);
                    carteiraEditada.setTipoPerfil(tipoPerfil);
                }
            }
            break; // Sai do loop de perfil
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
        }
    }
    
    // Exibir resumo das alterações
    std::cout << "\n=== RESUMO DAS ALTERACOES ===" << std::endl;
    std::cout << "Codigo: " << carteiraEditada.getCodigo().getValor() << " (inalterado)" << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << " -> " << carteiraEditada.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << " -> " << carteiraEditada.getTipoPerfil().getValor() << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Confirmar alterações
    std::cout << "\nConfirma as alteracoes? (s/n): ";
    char confirmacao;
    std::cin >> confirmacao;
    
    if (confirmacao != 's' && confirmacao != 'S') {
        std::cout << "\nEdicao da carteira cancelada pelo usuario." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Tentar salvar as alterações
    if (cntrServicoInvestimento->editarCarteira(carteiraEditada)) {
        std::cout << "\n*** SUCESSO! ***" << std::endl;
        std::cout << "Carteira '" << carteiraEditada.getNome().getValor() << "' editada com sucesso!" << std::endl;
        
    } else {
        std::cout << "\n*** ERRO! ***" << std::endl;
        std::cout << "Nao foi possivel salvar as alteracoes da carteira." << std::endl;
        std::cout << "Tente novamente ou contate o suporte." << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void ControladoraApresentacaoInvestimento::excluirCarteira(const Ncpf& cpf) {
    std::cout << "\n--- Exclusao de Carteira ---" << std::endl;
    
    // Listar as carteiras disponíveis do usuário
    std::list<Carteira> carteiras;
    if (!cntrServicoInvestimento->listarCarteiras(cpf, &carteiras) || carteiras.empty()) {
        std::cout << "\nVoce ainda nao possui carteiras para excluir." << std::endl;
        std::cout << "Use a opcao 'Criar carteira' primeiro!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibe as carteiras disponíveis
    std::cout << "\n=== SUAS CARTEIRAS ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Codigo" 
              << std::setw(25) << "Nome" 
              << std::setw(12) << "Perfil" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    
    for (const Carteira& carteira : carteiras) {
        std::cout << std::left << std::setw(8) << carteira.getCodigo().getValor()
                  << std::setw(25) << carteira.getNome().getValor()
                  << std::setw(12) << carteira.getTipoPerfil().getValor() << std::endl;
    }
    std::cout << std::string(45, '-') << std::endl;
    std::cout << "Total de carteiras: " << carteiras.size() << std::endl;
    
    // Solicitar código da carteira a ser excluída
    Codigo codigo;
    Carteira carteiraParaExcluir;
    
    while (true) {
        try {
            std::cout << "\nDigite o codigo da carteira que deseja excluir (ou '0' para cancelar): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            
            // Verificar se o usuário quer cancelar
            if (valorCodigo == "0") {
                std::cout << "Exclusao cancelada pelo usuario." << std::endl;
                return;
            }
            
            codigo.setValor(valorCodigo);
            
            // Buscar a carteira para mostrar os dados
            Dinheiro saldoTemp;
            if (cntrServicoInvestimento->consultarCarteira(codigo, &carteiraParaExcluir, &saldoTemp)) {
                break; // Carteira encontrada
            } else {
                std::cout << "Erro: Carteira com codigo '" << valorCodigo << "' nao foi encontrada." << std::endl;
                std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
            }
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima, ou '0' para cancelar." << std::endl;
        }
    }
    
    // Executar a exclusão específica
    excluirCarteiraEspecifica(cpf, carteiraParaExcluir);
}

void ControladoraApresentacaoInvestimento::editarCarteiraEspecifica(const Ncpf& cpf, const Carteira& carteiraAtual) {
    std::cout << "\n--- Edicao Rapida da Carteira ---" << std::endl;
    
    // Exibir dados atuais da carteira
    std::cout << "\n=== DADOS ATUAIS ===" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << " (nao pode ser alterado)" << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "====================" << std::endl;
    
    // Criar carteira editada com dados atuais
    Carteira carteiraEditada = carteiraAtual;
    
    // Solicitar novo nome
    while (true) {
        try {
            std::cout << "\nNome atual: " << carteiraAtual.getNome().getValor() << std::endl;
            std::cout << "Digite o novo nome (ou ENTER para manter): ";
            std::string novoNome;
            std::cin.ignore();
            std::getline(std::cin, novoNome);
            
            if (novoNome.empty()) {
                break; // Mantém o nome atual
            } else {
                Nome nome;
                nome.setValor(novoNome);
                carteiraEditada.setNome(nome);
                break;
            }
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use ate 20 caracteres (letras, numeros e espacos)." << std::endl;
        }
    }
    
    // Solicitar novo perfil
    while (true) {
        try {
            std::cout << "\nPerfil atual: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
            std::cout << "Alterar perfil? (s/n): ";
            char alterarPerfil;
            std::cin >> alterarPerfil;
            
            if (alterarPerfil == 's' || alterarPerfil == 'S') {
                std::cout << "\nPerfis disponiveis:" << std::endl;
                std::cout << "1. Conservador  2. Moderado  3. Agressivo" << std::endl;
                std::cout << "Escolha (1-3): ";
                
                int opcao;
                std::cin >> opcao;
                
                std::string valorPerfil;
                switch (opcao) {
                    case 1: valorPerfil = "Conservador"; break;
                    case 2: valorPerfil = "Moderado"; break;
                    case 3: valorPerfil = "Agressivo"; break;
                    default: std::cout << "Opcao invalida! Mantendo perfil atual." << std::endl; break;
                }
                
                if (!valorPerfil.empty()) {
                    TipoPerfil tipoPerfil;
                    tipoPerfil.setValor(valorPerfil);
                    carteiraEditada.setTipoPerfil(tipoPerfil);
                }
            }
            break;
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
        }
    }
    
    // Confirmar alterações
    std::cout << "\n=== RESUMO ===" << std::endl;
    std::cout << "Nome : " << carteiraAtual.getNome().getValor() << " -> " << carteiraEditada.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << " -> " << carteiraEditada.getTipoPerfil().getValor() << std::endl;
    std::cout << "Confirma? (s/n): ";
    
    char confirmacao;
    std::cin >> confirmacao;
    
    if (confirmacao == 's' || confirmacao == 'S') {
        if (cntrServicoInvestimento->editarCarteira(carteiraEditada)) {
            std::cout << "\n*** SUCESSO! *** Carteira editada!" << std::endl;
        } else {
            std::cout << "\n*** ERRO! *** Nao foi possivel editar." << std::endl;
        }
    } else {
        std::cout << "\nEdicao cancelada." << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

bool ControladoraApresentacaoInvestimento::excluirCarteiraEspecifica(const Ncpf& cpf, const Carteira& carteiraAtual) {
    std::cout << "\n--- Exclusao da Carteira ---" << std::endl;
    
    // Exibir dados da carteira
    std::cout << "\n*** ATENCAO - EXCLUSAO PERMANENTE ***" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "***********************************" << std::endl;
    
    std::cout << "\nIMPORTANTE:" << std::endl;
    std::cout << "- A exclusao e PERMANENTE e nao pode ser desfeita" << std::endl;
    std::cout << "- A carteira so pode ser excluida se nao possuir ordens" << std::endl;
    std::cout << "- Todos os dados desta carteira serao perdidos" << std::endl;
    
    std::cout << "\nTem certeza que deseja EXCLUIR esta carteira? (digite 'sim' para confirmar): ";
    std::string confirmacao;
    std::cin >> confirmacao;
    
    // Aceita várias formas de confirmação (case-insensitive)
    if (confirmacao != "SIM" && confirmacao != "sim" && 
        confirmacao != "S" && confirmacao != "s") {
        std::cout << "\nExclusao cancelada pelo usuario." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return false;
    }
    
    // Tentar excluir
    if (cntrServicoInvestimento->excluirCarteira(carteiraAtual.getCodigo())) {
        std::cout << "\n*** CARTEIRA EXCLUIDA COM SUCESSO ***" << std::endl;
        std::cout << "A carteira '" << carteiraAtual.getNome().getValor() << "' foi removida permanentemente." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return true;
        
    } else {
        std::cout << "\n*** ERRO NA EXCLUSAO ***" << std::endl;
        std::cout << "Nao foi possivel excluir a carteira." << std::endl;
        std::cout << "Possiveis causas:" << std::endl;
        std::cout << "- A carteira possui ordens de investimento associadas" << std::endl;
        std::cout << "- Erro interno do sistema" << std::endl;
        std::cout << "\nSOLUCAO: Exclua todas as ordens desta carteira primeiro." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return false;
    }
}

void ControladoraApresentacaoInvestimento::menuOrdens(const Codigo& codigoCarteira) {
    // Primeiro, verificar se a carteira existe
    Carteira carteiraAtual;
    Dinheiro saldoAtual;
    
    if (!cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual)) {
        std::cout << "\nErro: Carteira nao encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    int opcao;
    
    while (true) {
        std::cout << "\n=== MENU DE ORDENS ===" << std::endl;
        std::cout << "Carteira: " << carteiraAtual.getNome().getValor() 
                  << " (Código: " << codigoCarteira.getValor() << ")" << std::endl;
        std::cout << "Saldo Atual: R$ " << saldoAtual.getValor() << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "1. Criar nova ordem" << std::endl;
        std::cout << "2. Listar todas as ordens" << std::endl;
        std::cout << "3. Excluir ordem" << std::endl;
        std::cout << "0. Voltar ao menu anterior" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                criarOrdem(codigoCarteira);
                // Atualiza o saldo após possível criação de ordem
                cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual);
                break;
            case 2:
                listarOrdens(codigoCarteira);
                break;
            case 3:
                excluirOrdem(codigoCarteira);
                // Atualiza o saldo após possível exclusão de ordem
                cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual);
                break;
            case 0:
                return;
            default:
                std::cout << "Opção inválida! Tente novamente." << std::endl;
        }
    }
}

void ControladoraApresentacaoInvestimento::criarOrdem(const Codigo& codigoCarteira) {
    std::cout << "\n--- Criacao de Nova Ordem ---" << std::endl;
    
    // Primeiro, verificar se a carteira existe e obter seus dados
    Carteira carteiraAtual;
    Dinheiro saldoAtual;
    
    if (!cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual)) {
        std::cout << "\nErro: Carteira nao encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibir informações da carteira
    std::cout << "\n=== CARTEIRA SELECIONADA ===" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "Saldo Atual: R$ " << saldoAtual.getValor() << std::endl;
    std::cout << "============================" << std::endl;
    
    // Listar ordens existentes na carteira
    std::cout << "\n=== ORDENS EXISTENTES NA CARTEIRA ===" << std::endl;
    std::list<Ordem> ordensExistentes;
    if (cntrServicoInvestimento->listarOrdens(codigoCarteira, &ordensExistentes)) {
        if (ordensExistentes.empty()) {
            std::cout << "Nenhuma ordem encontrada nesta carteira." << std::endl;
        } else {
            std::cout << "┌─────────┬────────────┬────────────┬──────────────┬────────────┐" << std::endl;
            std::cout << "│ CÓDIGO  │   PAPEL    │    DATA    │    VALOR     │ QUANTIDADE │" << std::endl;
            std::cout << "├─────────┼────────────┼────────────┼──────────────┼────────────┤" << std::endl;
            
            for (const Ordem& ordem : ordensExistentes) {
                std::string codigo = ordem.getCodigo().getValor();
                std::string papel = ordem.getCodigoNeg().getValor();
                std::string data = ordem.getData().getValor();
                std::string valor = ordem.getDinheiro().getValor();
                std::string quantidade = ordem.getQuantidade().getValor();
                
                // Remove espaços extras do papel
                size_t posFim = papel.find_last_not_of(' ');
                if (posFim != std::string::npos) {
                    papel = papel.substr(0, posFim + 1);
                }
                
                // Formata a data para exibição (AAAAMMDD -> DD/MM/AAAA)
                std::string dataFormatada = data.substr(6, 2) + "/" + data.substr(4, 2) + "/" + data.substr(0, 4);
                
                std::cout << "│ " << std::setw(7) << std::left << codigo 
                         << " │ " << std::setw(10) << std::left << papel 
                         << " │ " << std::setw(10) << std::left << dataFormatada 
                         << " │ " << std::setw(12) << std::left << ("R$ " + valor) 
                         << " │ " << std::setw(10) << std::left << quantidade << " │" << std::endl;
            }
            std::cout << "└─────────┴────────────┴────────────┴──────────────┴────────────┘" << std::endl;
        }
    } else {
        std::cout << "Erro ao carregar ordens existentes." << std::endl;
    }
    std::cout << "===============================================" << std::endl;
    
    // Explicar os dados necessários para criar uma ordem
    std::cout << "\n=== COMO CRIAR UMA ORDEM ===" << std::endl;
    std::cout << "Para criar uma ordem, você precisa fornecer:" << std::endl;
    std::cout << "1. CODIGO DA ORDEM    - ID único de 5 dígitos (ex: 30001, 30002)" << std::endl;
    std::cout << "2. PAPEL/ATIVO        - Código do produto (ex: 00001, 00002)" << std::endl;
    std::cout << "3. DATA               - Data da operação (ex: 20240315)" << std::endl;
    std::cout << "4. QUANTIDADE         - Quantos papéis (ex: 100, 1.000)" << std::endl;
    std::cout << "============================" << std::endl;
    
    std::cout << "\n=== PRODUTOS DISPONIVEIS (com dados historicos) ===" << std::endl;
    std::cout << "00001, 00002, 00003, 00004, 00005, 00006, 00007, 00008, 00009" << std::endl;
    std::cout << "Datas disponíveis: 20240315 a 20240320" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    // Coleta dos dados da ordem
    Codigo codigoOrdem;
    CodigoNeg codigoNegociacao;
    Data dataOrdem;
    Quantidade quantidadeOrdem;
    
    // === CÓDIGO DA ORDEM ===
    std::cout << "\n=== 1. CÓDIGO DA ORDEM ===" << std::endl;
    std::cout << "Exemplos de códigos válidos:" << std::endl;
    std::cout << "• 30001, 30002, 30003, 30004, 30005" << std::endl;
    std::cout << "• 40001, 40002, 40003, 40004, 40005" << std::endl;
    std::cout << "• 50001, 50002, 50003, 50004, 50005" << std::endl;
    std::cout << "DICA: Use qualquer código de 5 dígitos que não esteja em uso" << std::endl;
    std::cout << "=========================" << std::endl;
    
    while (true) {
        try {
            std::cout << "\nDigite o CODIGO DA ORDEM (5 dígitos) ou '0' para cancelar: ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            
            if (valorCodigo == "0") {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return;
            }
            
            codigoOrdem.setValor(valorCodigo);
            
            // VALIDAÇÃO EM TEMPO REAL: Verificar se o código já existe
            std::list<Ordem> ordensExistentes;
            if (cntrServicoInvestimento->listarOrdens(codigoCarteira, &ordensExistentes)) {
                for (const Ordem& ordem : ordensExistentes) {
                    if (ordem.getCodigo().getValor() == valorCodigo) {
                        std::cout << "❌ ERRO: Código '" << valorCodigo << "' já está em uso!" << std::endl;
                        std::cout << "   Use um código diferente." << std::endl;
                        continue;
                    }
                }
            }
            
            std::cout << "✅ Código da ordem válido: " << valorCodigo << std::endl;
            break;
        } catch (const std::invalid_argument &exp) {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Exemplo: 30001, 30002, 40001, 50001, etc." << std::endl;
        }
    }

    // === CÓDIGO DE NEGOCIAÇÃO ===
    std::cout << "\n=== 2. PAPEL/ATIVO ===" << std::endl;
    std::cout << "Produtos disponíveis no arquivo histórico:" << std::endl;
    std::cout << "┌─────────┬──────────────────────────────────┐" << std::endl;
    std::cout << "│  CÓDIGO │           PRODUTO                │" << std::endl;
    std::cout << "├─────────┼──────────────────────────────────┤" << std::endl;
    std::cout << "│  00001  │ Caneta Esferográfica             │" << std::endl;
    std::cout << "│  00002  │ Lápis HB                         │" << std::endl;
    std::cout << "│  00003  │ Caderno 96 folhas                │" << std::endl;
    std::cout << "│  00004  │ Borracha Branca                  │" << std::endl;
    std::cout << "│  00005  │ Régua 30cm                       │" << std::endl;
    std::cout << "│  00006  │ Tesoura Escolar                  │" << std::endl;
    std::cout << "│  00007  │ Cola Branca                      │" << std::endl;
    std::cout << "│  00008  │ Papel A4 500 folhas              │" << std::endl;
    std::cout << "│  00009  │ Mochila Escolar                  │" << std::endl;
    std::cout << "└─────────┴──────────────────────────────────┘" << std::endl;
    std::cout << "DICA: Digite apenas o código (ex: 00001)" << std::endl;
    std::cout << "======================" << std::endl;
    
    while (true) {
        try {
            std::cout << "\nDigite o PAPEL/ATIVO (código de negociação) ou '0' para cancelar: ";
            std::string valorCodigoNeg;
            std::cin.ignore(); // Limpa buffer
            std::getline(std::cin, valorCodigoNeg);
            
            if (valorCodigoNeg == "0") {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.get();
                return;
            }
            
            // Completa com espaços se necessário (até 12 caracteres)
            if (valorCodigoNeg.length() < 12) {
                valorCodigoNeg.resize(12, ' ');
            }
            
            codigoNegociacao.setValor(valorCodigoNeg);
            
            // VALIDAÇÃO EM TEMPO REAL: Verificar se o produto existe no arquivo histórico
            std::string codigoLimpo = valorCodigoNeg;
            size_t posFim = codigoLimpo.find_last_not_of(' ');
            if (posFim != std::string::npos) {
                codigoLimpo = codigoLimpo.substr(0, posFim + 1);
            }
            
            // Verificar se existe pelo menos uma linha com esse código no arquivo
            std::ifstream arquivo("../data/DADOS_HISTORICOS.txt");
            bool produtoEncontrado = false;
            std::string linha;
            
            while (std::getline(arquivo, linha)) {
                // Pular linhas de comentário
                if (linha.empty() || linha[0] == '#') {
                    continue;
                }
                
                if (linha.find(codigoLimpo + "|") == 0) {
                    produtoEncontrado = true;
                    break;
                }
            }
            arquivo.close();
            
            if (!produtoEncontrado) {
                std::cout << "❌ ERRO: Produto '" << codigoLimpo << "' não encontrado no arquivo histórico!" << std::endl;
                std::cout << "   Use um dos produtos disponíveis: 00001, 00002, 00003, etc." << std::endl;
                continue;
            }
            
            std::cout << "✅ Produto válido: " << codigoLimpo << std::endl;
            break;
        } catch (const std::invalid_argument &exp) {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
                            std::cout << "   Exemplo: 00001, 00002, 00003, 00004, 00005, 00006, 00007, 00008, 00009" << std::endl;
            std::cout << "   DICA: Use um dos produtos listados acima que têm dados históricos!" << std::endl;
        }
    }

    // === DATA DA ORDEM ===
    std::cout << "\n=== 3. DATA DA ORDEM ===" << std::endl;
    std::cout << "Datas disponíveis no arquivo histórico:" << std::endl;
    std::cout << "┌────────────┬─────────────────┬───────────────┐" << std::endl;
    std::cout << "│   CÓDIGO   │      DATA       │  DIA SEMANA   │" << std::endl;
    std::cout << "├────────────┼─────────────────┼───────────────┤" << std::endl;
    std::cout << "│  20240315  │  15/03/2024     │  Sexta-feira  │" << std::endl;
    std::cout << "│  20240316  │  16/03/2024     │  Sábado       │" << std::endl;
    std::cout << "│  20240317  │  17/03/2024     │  Domingo      │" << std::endl;
    std::cout << "│  20240318  │  18/03/2024     │  Segunda-feira│" << std::endl;
    std::cout << "│  20240319  │  19/03/2024     │  Terça-feira  │" << std::endl;
    std::cout << "│  20240320  │  20/03/2024     │  Quarta-feira │" << std::endl;
    std::cout << "└────────────┴─────────────────┴───────────────┘" << std::endl;
    std::cout << "DICA: Digite o código da data (ex: 20240315)" << std::endl;
    std::cout << "========================" << std::endl;
    
    while (true) {
        try {
            std::cout << "\nDigite a DATA da ordem (AAAAMMDD) ou '0' para cancelar: ";
            std::string valorData;
            std::cin >> valorData;
            
            if (valorData == "0") {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return;
            }
            
            dataOrdem.setValor(valorData);
            
            // VALIDAÇÃO EM TEMPO REAL: Verificar se a data existe no arquivo histórico
            std::ifstream arquivo("../data/DADOS_HISTORICOS.txt");
            bool dataEncontrada = false;
            std::string linha;
            
            while (std::getline(arquivo, linha)) {
                // Pular linhas de comentário
                if (linha.empty() || linha[0] == '#') {
                    continue;
                }
                
                if (linha.find("|" + valorData + "|") != std::string::npos) {
                    dataEncontrada = true;
                    break;
                }
            }
            arquivo.close();
            
            if (!dataEncontrada) {
                std::cout << "❌ ERRO: Data '" << valorData << "' não encontrada no arquivo histórico!" << std::endl;
                std::cout << "   Use uma das datas disponíveis: 20240315, 20240316, 20240317, 20240318, 20240319, 20240320" << std::endl;
                continue;
            }
            
            std::cout << "✅ Data válida: " << valorData << std::endl;
            break;
        } catch (const std::invalid_argument &exp) {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Exemplo: 20240315, 20240316, 20240317, 20240318, 20240319, 20240320" << std::endl;
            std::cout << "   DICA: Use apenas datas entre 20240315 e 20240320 (dados históricos disponíveis)!" << std::endl;
        }
    }
    
    // === QUANTIDADE ===
    std::cout << "\n=== 4. QUANTIDADE ===" << std::endl;
    std::cout << "Exemplos de quantidades válidas:" << std::endl;
    std::cout << "┌─────────────┬────────────────────────────────┐" << std::endl;
    std::cout << "│ QUANTIDADE  │           OBSERVAÇÃO           │" << std::endl;
    std::cout << "├─────────────┼────────────────────────────────┤" << std::endl;
    std::cout << "│     100     │ Lote padrão pequeno            │" << std::endl;
    std::cout << "│     500     │ Lote médio                     │" << std::endl;
    std::cout << "│   1.000     │ Lote padrão                    │" << std::endl;
    std::cout << "│   5.000     │ Lote grande                    │" << std::endl;
    std::cout << "│  10.000     │ Lote muito grande              │" << std::endl;
    std::cout << "│ 100.000     │ Lote institucional             │" << std::endl;
    std::cout << "└─────────────┴────────────────────────────────┘" << std::endl;
    std::cout << "DICA: Digite números inteiros (ex: 1000 ou 1.000, 5000 ou 5.000)" << std::endl;
    std::cout << "=========================" << std::endl;
    
    while (true) {
        try {
            std::cout << "\nDigite a QUANTIDADE de papéis ou '0' para cancelar: ";
            std::string valorQuantidade;
            std::cin >> valorQuantidade;
            
            if (valorQuantidade == "0") {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return;
            }
            
            quantidadeOrdem.setValor(valorQuantidade);
            
            // VALIDAÇÃO EM TEMPO REAL: Verificar se a combinação produto+data+quantidade é válida
            std::string codigoLimpo = codigoNegociacao.getValor();
            size_t posFim = codigoLimpo.find_last_not_of(' ');
            if (posFim != std::string::npos) {
                codigoLimpo = codigoLimpo.substr(0, posFim + 1);
            }
            
            // Verificar se existe a combinação específica no arquivo
            std::ifstream arquivo("../data/DADOS_HISTORICOS.txt");
            bool combinacaoEncontrada = false;
            std::string linha;
            
            while (std::getline(arquivo, linha)) {
                // Pular linhas de comentário
                if (linha.empty() || linha[0] == '#') {
                    continue;
                }
                
                // Verificar se a linha contém a combinação produto+data
                if (linha.find(codigoLimpo + "|" + dataOrdem.getValor() + "|") == 0) {
                    combinacaoEncontrada = true;
                    break;
                }
            }
            arquivo.close();
            
            if (!combinacaoEncontrada) {
                std::cout << "❌ ERRO: Combinação inválida!" << std::endl;
                std::cout << "   Produto '" << codigoLimpo << "' não tem dados para a data '" << dataOrdem.getValor() << "'" << std::endl;
                std::cout << "   Verifique se o produto e a data são compatíveis." << std::endl;
                continue;
            }
            
            std::cout << "✅ Quantidade válida: " << valorQuantidade << std::endl;
            break;
        } catch (const std::invalid_argument &exp) {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Exemplo: 100, 500, 1000 ou 1.000, 5000 ou 5.000, etc." << std::endl;
        }
    }
    
    // === CONFIRMAÇÃO DOS DADOS ===
    std::cout << "\n=== RESUMO DA ORDEM ===" << std::endl;
    std::cout << "Codigo da Ordem     : " << codigoOrdem.getValor() << std::endl;
    std::cout << "Papel (Cod.Negoc.)  : " << codigoNegociacao.getValor() << std::endl;
    std::cout << "Data                : " << dataOrdem.getValor() << std::endl;
    std::cout << "Quantidade          : " << quantidadeOrdem.getValor() << std::endl;
    std::cout << "Carteira            : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "======================" << std::endl;
    
    std::cout << "\nIMPORTANTE:" << std::endl;
    std::cout << "- O PRECO da ordem sera calculado automaticamente" << std::endl;
    std::cout << "- Formula: Preco Medio Historico × Quantidade" << std::endl;
    std::cout << "- Os dados serao buscados no arquivo DADOS_HISTORICOS.txt" << std::endl;
    
    char confirmacao;
    std::cout << "\nConfirma a criacao da ordem? (s/n): ";
    std::cin >> confirmacao;
    
    if (confirmacao != 's' && confirmacao != 'S') {
        std::cout << "\nCriacao da ordem cancelada pelo usuario." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // === CRIAR A ORDEM ===
    Ordem novaOrdem;
    novaOrdem.setCodigo(codigoOrdem);
    novaOrdem.setCodigoNeg(codigoNegociacao);
    novaOrdem.setData(dataOrdem);
    novaOrdem.setQuantidade(quantidadeOrdem);
    
    // O valor (dinheiro) será calculado automaticamente pela controladora de serviço
    Dinheiro valorTemporario;
    valorTemporario.setValor("0,01"); // Valor temporário, será substituído
    novaOrdem.setDinheiro(valorTemporario);
    
    // Tenta criar a ordem
    if (cntrServicoInvestimento->criarOrdem(codigoCarteira, novaOrdem)) {
        std::cout << "\n*** ORDEM CRIADA COM SUCESSO! ***" << std::endl;
        
        // Busca a ordem criada para mostrar o preço calculado
        std::list<Ordem> ordensCarteira;
        if (cntrServicoInvestimento->listarOrdens(codigoCarteira, &ordensCarteira)) {
            for (const Ordem& ordem : ordensCarteira) {
                if (ordem.getCodigo().getValor() == codigoOrdem.getValor()) {
                    std::cout << "\n=== DETALHES DA ORDEM CRIADA ===" << std::endl;
                    std::cout << "Codigo da Ordem: " << ordem.getCodigo().getValor() << std::endl;
                    std::cout << "Papel          : " << ordem.getCodigoNeg().getValor() << std::endl;
                    std::cout << "Data           : " << ordem.getData().getValor() << std::endl;
                    std::cout << "Quantidade     : " << ordem.getQuantidade().getValor() << std::endl;
                    std::cout << "VALOR TOTAL    : R$ " << ordem.getDinheiro().getValor() << std::endl;
                    std::cout << "===============================" << std::endl;
                    break;
                }
            }
        }
        
        std::cout << "\nA ordem foi associada a carteira '" << carteiraAtual.getNome().getValor() << "'." << std::endl;
        std::cout << "O preco foi calculado com base nos dados historicos." << std::endl;
        
    } else {
        std::cout << "\n*** ERRO NA CRIACAO DA ORDEM ***" << std::endl;
        std::cout << "Possiveis causas:" << std::endl;
        std::cout << "- Codigo da ordem ja existe no sistema" << std::endl;
        std::cout << "- Carteira nao encontrada" << std::endl;
        std::cout << "- Papel ou data nao encontrados no arquivo historico" << std::endl;
        std::cout << "- Arquivo DADOS_HISTORICOS.TXT nao encontrado" << std::endl;
        std::cout << "- Erro no calculo do preco" << std::endl;
        
        std::cout << "\nDICAS:" << std::endl;
        std::cout << "- Verifique se o codigo da ordem e unico" << std::endl;
        std::cout << "- Use apenas produtos disponiveis: 00001, 00002, 00003, etc." << std::endl;
        std::cout << "- Use datas entre 20240315 e 20240320" << std::endl;
        std::cout << "- Certifique-se que o arquivo DADOS_HISTORICOS.txt existe" << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void ControladoraApresentacaoInvestimento::listarOrdens(const Codigo& codigoCarteira) {
    std::cout << "\n--- Lista de Ordens da Carteira ---" << std::endl;
    
    // Primeiro, obter informações da carteira
    Carteira carteiraAtual;
    Dinheiro saldoCarteira;
    
    if (!cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoCarteira)) {
        std::cout << "\nErro: Carteira nao encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibir cabeçalho com informações da carteira
    std::cout << "\n=== CARTEIRA ===" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "SALDO TOTAL: R$ " << saldoCarteira.getValor() << std::endl;
    std::cout << "=================" << std::endl;
    
    // Buscar e listar as ordens
    std::list<Ordem> ordensCarteira;
    
    if (!cntrServicoInvestimento->listarOrdens(codigoCarteira, &ordensCarteira)) {
        std::cout << "\n=== ORDENS ===" << std::endl;
        std::cout << "Esta carteira ainda nao possui ordens de investimento." << std::endl;
        std::cout << "Use a opcao 'Criar nova ordem' para comecar a investir!" << std::endl;
        std::cout << "==============" << std::endl;
    } else if (ordensCarteira.empty()) {
        std::cout << "\n=== ORDENS ===" << std::endl;
        std::cout << "Esta carteira ainda nao possui ordens de investimento." << std::endl;
        std::cout << "Use a opcao 'Criar nova ordem' para comecar a investir!" << std::endl;
        std::cout << "==============" << std::endl;
    } else {
        std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
        std::cout << std::left 
                  << std::setw(8) << "Codigo"
                  << std::setw(15) << "Papel"
                  << std::setw(12) << "Data"
                  << std::setw(12) << "Quantidade"
                  << std::setw(15) << "Valor Total" << std::endl;
        std::cout << std::string(62, '-') << std::endl;
        
        int contador = 1;
        for (const Ordem& ordem : ordensCarteira) {
            // Limpa espaços do código de negociação para exibição
            std::string codigoNegLimpo = ordem.getCodigoNeg().getValor();
            size_t posFim = codigoNegLimpo.find_last_not_of(' ');
            if (posFim != std::string::npos) {
                codigoNegLimpo = codigoNegLimpo.substr(0, posFim + 1);
            }
            
            std::cout << std::left 
                      << std::setw(8) << ordem.getCodigo().getValor()
                      << std::setw(15) << codigoNegLimpo
                      << std::setw(12) << ordem.getData().getValor()
                      << std::setw(12) << ordem.getQuantidade().getValor()
                      << std::setw(15) << ("R$ " + ordem.getDinheiro().getValor()) << std::endl;
        }
        
        std::cout << std::string(62, '-') << std::endl;
        std::cout << "Total de ordens: " << ordensCarteira.size() << std::endl;
        std::cout << "SALDO CONSOLIDADO: R$ " << saldoCarteira.getValor() << std::endl;
        std::cout << "==============================" << std::endl;
    }
    
    std::cout << "\nNOTA IMPORTANTE:" << std::endl;
    std::cout << "- O saldo da carteira e a soma de todas as ordens listadas acima" << std::endl;
    std::cout << "- Cada ordem representa um investimento ja realizado" << std::endl;
    std::cout << "- O valor da ordem foi calculado com base nos dados historicos" << std::endl;
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void ControladoraApresentacaoInvestimento::excluirOrdem(const Codigo& codigoCarteira) {
    std::cout << "\n--- Exclusão de Ordem ---" << std::endl;
    
    // Primeiro, listar as ordens da carteira atual para o usuário escolher
    std::list<Ordem> ordensCarteira;
    Carteira carteiraAtual;
    Dinheiro saldoCarteira;
    
    if (!cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoCarteira)) {
        std::cout << "\nErro: Carteira não encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    if (!cntrServicoInvestimento->listarOrdens(codigoCarteira, &ordensCarteira) || ordensCarteira.empty()) {
        std::cout << "\n=== NENHUMA ORDEM DISPONÍVEL ===" << std::endl;
        std::cout << "Esta carteira não possui ordens para excluir." << std::endl;
        std::cout << "===============================\n" << std::endl;
        std::cout << "Pressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Exibir informações da carteira
    std::cout << "\n=== CARTEIRA ATUAL ===" << std::endl;
    std::cout << "Código: " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "Saldo : R$ " << saldoCarteira.getValor() << std::endl;
    std::cout << "======================\n" << std::endl;
    
    // Listar todas as ordens disponíveis para exclusão
    std::cout << "=== ORDENS DISPONÍVEIS PARA EXCLUSÃO ===" << std::endl;
    std::cout << std::left 
              << std::setw(8) << "Código"
              << std::setw(15) << "Papel"
              << std::setw(12) << "Data"
              << std::setw(12) << "Quantidade"
              << std::setw(15) << "Valor Total" << std::endl;
    std::cout << std::string(62, '-') << std::endl;
    
    for (const Ordem& ordem : ordensCarteira) {
        // Limpa espaços do código de negociação para exibição
        std::string codigoNegLimpo = ordem.getCodigoNeg().getValor();
        size_t posFim = codigoNegLimpo.find_last_not_of(' ');
        if (posFim != std::string::npos) {
            codigoNegLimpo = codigoNegLimpo.substr(0, posFim + 1);
        }
        
        std::cout << std::left 
                  << std::setw(8) << ordem.getCodigo().getValor()
                  << std::setw(15) << codigoNegLimpo
                  << std::setw(12) << ordem.getData().getValor()
                  << std::setw(12) << ordem.getQuantidade().getValor()
                  << std::setw(15) << ("R$ " + ordem.getDinheiro().getValor()) << std::endl;
    }
    
    std::cout << std::string(62, '-') << std::endl;
    std::cout << "Total de ordens: " << ordensCarteira.size() << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Solicitar código da ordem a ser excluída
    Codigo codigoOrdem;
    std::string valorCodigo;
    
    while (true) {
        try {
            std::cout << "Digite o código da ordem que deseja excluir (ou '0' para cancelar): ";
            std::cin >> valorCodigo;
            
            if (valorCodigo == "0") {
                std::cout << "\nExclusão cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return;
            }
            
            codigoOrdem.setValor(valorCodigo);
            
            // Verificar se a ordem realmente existe na carteira atual
            bool ordemEncontrada = false;
            Ordem ordemSelecionada;
            for (const Ordem& ordem : ordensCarteira) {
                if (ordem.getCodigo().getValor() == codigoOrdem.getValor()) {
                    ordemEncontrada = true;
                    ordemSelecionada = ordem;
                    break;
                }
            }
            
            if (!ordemEncontrada) {
                std::cout << "Erro: Código '" << valorCodigo << "' não encontrado nas ordens desta carteira." << std::endl;
                std::cout << "Por favor, escolha um código da lista acima." << std::endl;
                continue;
            }
            
            // Exibir detalhes da ordem selecionada para confirmação
            std::string codigoNegLimpo = ordemSelecionada.getCodigoNeg().getValor();
            size_t posFim = codigoNegLimpo.find_last_not_of(' ');
            if (posFim != std::string::npos) {
                codigoNegLimpo = codigoNegLimpo.substr(0, posFim + 1);
            }
            
            std::cout << "\n*** ATENÇÃO - EXCLUSÃO PERMANENTE ***" << std::endl;
            std::cout << "Ordem selecionada:" << std::endl;
            std::cout << "  Código     : " << ordemSelecionada.getCodigo().getValor() << std::endl;
            std::cout << "  Papel      : " << codigoNegLimpo << std::endl;
            std::cout << "  Data       : " << ordemSelecionada.getData().getValor() << std::endl;
            std::cout << "  Quantidade : " << ordemSelecionada.getQuantidade().getValor() << std::endl;
            std::cout << "  Valor Total: R$ " << ordemSelecionada.getDinheiro().getValor() << std::endl;
            std::cout << "***********************************" << std::endl;
            break;
            
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um código de 5 dígitos numéricos (ex: 30001)" << std::endl;
        }
    }
    
    std::cout << "\nIMPORTANTE:" << std::endl;
    std::cout << "- A exclusão é PERMANENTE e não pode ser desfeita" << std::endl;
    std::cout << "- A ordem será removida da carteira associada" << std::endl;
    std::cout << "- O saldo da carteira será recalculado automaticamente" << std::endl;
    
    std::cout << "\nTem certeza que deseja EXCLUIR esta ordem? (digite 'sim' para confirmar): ";
    std::string confirmacao;
    std::cin >> confirmacao;
    
    // Aceita várias formas de confirmação (case-insensitive)
    if (confirmacao != "SIM" && confirmacao != "sim" && 
        confirmacao != "S" && confirmacao != "s") {
        std::cout << "\nExclusão cancelada pelo usuário." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    // Tentar excluir a ordem
    if (cntrServicoInvestimento->excluirOrdem(codigoOrdem)) {
        std::cout << "\n*** ORDEM EXCLUÍDA COM SUCESSO ***" << std::endl;
        std::cout << "A ordem com código '" << codigoOrdem.getValor() << "' foi removida permanentemente." << std::endl;
        std::cout << "O saldo da carteira foi recalculado automaticamente." << std::endl;
        
        // Mostrar novo saldo da carteira
        Dinheiro novoSaldo;
        if (cntrServicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &novoSaldo)) {
            std::cout << "Novo saldo da carteira: R$ " << novoSaldo.getValor() << std::endl;
        }
        
    } else {
        std::cout << "\n*** ERRO NA EXCLUSÃO ***" << std::endl;
        std::cout << "Não foi possível excluir a ordem." << std::endl;
        std::cout << "Possíveis causas:" << std::endl;
        std::cout << "- Ordem com código '" << codigoOrdem.getValor() << "' não foi encontrada" << std::endl;
        std::cout << "- Erro interno do sistema" << std::endl;
        
        std::cout << "\nDICAS:" << std::endl;
        std::cout << "- Verifique se o código da ordem está correto" << std::endl;
        std::cout << "- Use a opção 'Listar ordens' para ver os códigos disponíveis" << std::endl;
        std::cout << "- Certifique-se de que a ordem pertence a alguma carteira" << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

// =================================================================================================
// INTERFACE MANAGER - GERENCIADOR CENTRAL DE INTERFACE
// =================================================================================================

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

void InterfaceManager::limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void InterfaceManager::mostrarMenuInicial() {
    limparTela();
    std::cout << "\n=== GERENCIAMENTO DE CONTA ===" << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Cadastrar nova conta" << std::endl;
    std::cout << "0. Sair" << std::endl;
    std::cout << "Escolha uma opção: ";
}

void InterfaceManager::mostrarMenuPrincipal() {
    limparTela();
    std::cout << "\n=== MENU PRINCIPAL ===" << std::endl;
    std::cout << "Usuário: " << cpfAutenticado.getValor() << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "1. Gerenciar Conta" << std::endl;
    std::cout << "2. Gerenciar Investimentos" << std::endl;
    std::cout << "0. Logout" << std::endl;
    std::cout << "Escolha uma opção: ";
}

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

void InterfaceManager::processarGerenciarConta() {
    if (cntrApresentacaoUsuario->executar(cpfAutenticado)) {
        // Conta foi excluída, fazer logout automático
        fazerLogout();
    } else {
        telaAtual = TelaAtual::MENU_PRINCIPAL;
    }
}

void InterfaceManager::processarGerenciarInvestimentos() {
    cntrApresentacaoInvestimento->executar(cpfAutenticado);
    telaAtual = TelaAtual::MENU_PRINCIPAL;
}

void InterfaceManager::fazerLogout() {
    std::cout << "Logout realizado com sucesso!" << std::endl;
    usuarioAutenticado = false;
    telaAtual = TelaAtual::MENU_INICIAL;
}

void InterfaceManager::executar() {
    while (telaAtual != TelaAtual::SAIR) {
        switch (telaAtual) {
            case TelaAtual::MENU_INICIAL:
                mostrarMenuInicial();
                processarMenuInicial();
                break;
                
            case TelaAtual::LOGIN:
                limparTela();
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