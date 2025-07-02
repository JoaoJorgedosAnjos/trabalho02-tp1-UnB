#include <iostream>
#include <iomanip>
#include <cctype>
#include "controladorasApresentacao.hpp"

// =================================================================================================
// FUNÇÕES AUXILIARES PARA FORMATAÇÃO DE ENTRADA
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

        try {
            // Entrada flexível de CPF com máscara automática
            std::cout << "CPF (apenas numeros ou XXX.XXX.XXX-XX): ";
            std::cin >> valor;
            
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

void ControladoraApresentacaoUsuario::executar(const Ncpf& cpf) {
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
                consultarConta(cpf);
                break;
            case 2:
                editarConta(cpf);
                break;
            case 3:
                excluirConta(cpf);
                break;
            case 0:
                break;
            default:
                std::cout << "Opcao invalida!" << std::endl;
        }
    }
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
    
    std::string valor;
    Conta novaConta;
    Ncpf cpf;
    Nome nome;
    Senha senha;
    
    while (true) {
        try {
            // Entrada de CPF com máscara automática
            std::cout << "CPF (apenas numeros ou XXX.XXX.XXX-XX): ";
            std::cin >> valor;
            
            // Aplica a máscara automática se necessário
            try {
                std::string cpfFormatado = formatarCPF(valor);
                std::cout << "CPF formatado: " << cpfFormatado << std::endl;
                cpf.setValor(cpfFormatado);
            } catch (const std::invalid_argument& e) {
                // Se falhou na formatação, tenta usar o valor original
                cpf.setValor(valor);
            }
            
            std::cin.ignore(); // Limpa o buffer para getline
            std::cout << "Nome (ate 20 caracteres): ";
            std::getline(std::cin, valor);
            nome.setValor(valor);
            
            std::cout << "Senha (6 caracteres): ";
            std::cin >> valor;
            senha.setValor(valor);
            
            novaConta.setNcpf(cpf);
            novaConta.setNome(nome);
            novaConta.setSenha(senha);
            
            break;
            
        } catch (const std::invalid_argument &exp) {
            std::cout << "\nErro: " << exp.what() << std::endl;
            std::cout << "\nDicas para cadastro:" << std::endl;
            std::cout << "- CPF: Digite apenas os 11 numeros (ex: 04348848122)" << std::endl;
            std::cout << "- Nome: Maximo 20 caracteres, sem espacos duplos" << std::endl;
            std::cout << "- Senha: 6 caracteres com 1 maiuscula, 1 minuscula, 1 numero e 1 simbolo (#$%&)" << std::endl;
            std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    }
    
    if (cntrServicoUsuario->cadastrarConta(novaConta)) {
        std::cout << "\nConta cadastrada com sucesso!" << std::endl;
        std::cout << "CPF cadastrado: " << cpf.getValor() << std::endl;
        std::cout << "Agora voce ja pode fazer login!" << std::endl;
    } else {
        std::cout << "\nErro ao cadastrar conta. CPF ja existe no sistema." << std::endl;
    }
}

void ControladoraApresentacaoUsuario::consultarConta(const Ncpf& cpf) {
    Conta conta;
    Dinheiro saldo;
    
    if (cntrServicoUsuario->consultarConta(cpf, &conta, &saldo)) {
        std::cout << "\n=== DADOS DA CONTA ===" << std::endl;
        std::cout << "CPF   : " << conta.getNcpf().getValor() << std::endl;
        std::cout << "Nome  : " << conta.getNome().getValor() << std::endl;
        std::cout << "Saldo Total: R$ " << saldo.getValor() << std::endl;
    } else {
        std::cout << "\nErro ao consultar conta." << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
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

void ControladoraApresentacaoUsuario::excluirConta(const Ncpf& cpf) {
    std::cout << "\n--- Exclusao de Conta ---" << std::endl;
    std::cout << "ATENCAO: Esta operacao e irreversivel!" << std::endl;
    std::cout << "Tem certeza que deseja excluir sua conta? (s/N): ";
    
    char confirmacao;
    std::cin >> confirmacao;
    
    if (confirmacao == 's' || confirmacao == 'S') {
        if (cntrServicoUsuario->excluirConta(cpf)) {
            std::cout << "\nConta excluida com sucesso!" << std::endl;
            std::cout << "Obrigado por utilizar nosso sistema." << std::endl;
        } else {
            std::cout << "\nErro ao excluir conta. Verifique se nao existem carteiras associadas." << std::endl;
        }
    } else {
        std::cout << "\nOperacao cancelada." << std::endl;
    }
    
    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
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
        std::cout << "Escolha uma opcao: ";
        
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
                        std::cout << "Voce nao possui carteiras. Crie uma primeiro." << std::endl;
                        break;
                    }
                    
                    std::cout << "\nSuas carteiras:" << std::endl;
                    for (const auto& carteira : carteiras) {
                        std::cout << "Codigo: " << carteira.getCodigo().getValor() 
                                  << " - " << carteira.getNome().getValor() << std::endl;
                    }
                    
                    std::string codigoStr;
                    std::cout << "Digite o codigo da carteira: ";
                    std::cin >> codigoStr;
                    
                    try {
                        Codigo codigoCarteira;
                        codigoCarteira.setValor(codigoStr);
                        menuOrdens(codigoCarteira);
                    } catch (const std::invalid_argument &exp) {
                        std::cout << "Codigo invalido: " << exp.what() << std::endl;
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
        std::cout << "Escolha uma opcao: ";
        
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
                std::cout << "Opcao invalida!" << std::endl;
        }
    }
}

void ControladoraApresentacaoInvestimento::criarCarteira(const Ncpf& cpf) {
    std::cout << "\n--- Criacao de Nova Carteira ---" << std::endl;
    
    // Primeiro, verificar quantas carteiras o usuário já possui
    std::list<Carteira> carteirasExistentes;
    bool temCarteiras = cntrServicoInvestimento->listarCarteiras(cpf, &carteirasExistentes);
    
    size_t quantidadeAtual = temCarteiras ? carteirasExistentes.size() : 0;
    
    std::cout << "\nVoce possui atualmente " << quantidadeAtual << " carteira(s)." << std::endl;
    std::cout << "Limite maximo: 5 carteiras por usuario." << std::endl;
    
    if (quantidadeAtual >= 5) {
        std::cout << "\n*** LIMITE ATINGIDO ***" << std::endl;
        std::cout << "Voce ja possui o maximo de 5 carteiras permitidas." << std::endl;
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
            std::cout << "\nDigite o codigo da nova carteira (5 digitos): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
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
            std::cout << "Escolha o perfil (1-3): ";
            
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
                    std::cout << "Opcao invalida! Escolha entre 1 e 3." << std::endl;
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
            std::cout << "Use a opcao 'Criar carteira' para comecar!" << std::endl;
            std::cout << "\nPressione qualquer tecla para voltar..." << std::endl;
            std::cin.ignore();
            std::cin.get();
            return;
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
            std::cout << "\nDigite o codigo da carteira que deseja excluir: ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
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
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima." << std::endl;
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
    std::cout << "\n--- Menu de Ordens (Carteira: " << codigoCarteira.getValor() << ") ---" << std::endl;
    // Implementação será adicionada posteriormente
    std::cout << "EM CONSTRUCAO" << std::endl;
}

void ControladoraApresentacaoInvestimento::criarOrdem(const Codigo& codigoCarteira) {
    std::cout << "\n--- Criacao de Ordem ---" << std::endl;
    // Implementação será adicionada posteriormente
    std::cout << "EM CONSTRUCAO" << std::endl;
}

void ControladoraApresentacaoInvestimento::listarOrdens(const Codigo& codigoCarteira) {
    std::cout << "\n--- Lista de Ordens ---" << std::endl;
    // Implementação será adicionada posteriormente
    std::cout << "EM CONSTRUCAO" << std::endl;
}

void ControladoraApresentacaoInvestimento::excluirOrdem() {
    std::cout << "\n--- Exclusao de Ordem ---" << std::endl;
    // Implementação será adicionada posteriormente
    std::cout << "EM CONSTRUCAO" << std::endl;
}