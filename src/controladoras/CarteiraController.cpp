#include "CarteiraController.hpp"
#include <limits>

/**
 * @brief Construtor do controlador de carteiras
 * 
 * @param servico Ponteiro para o serviço de investimentos que será utilizado
 *                para realizar operações no banco de dados
 * 
 * @details Inicializa o controlador com uma referência ao serviço de investimentos.
 * O serviço é responsável por todas as operações de persistência de dados.
 */
CarteiraController::CarteiraController(IServicoInvestimento* servico) 
    : servicoInvestimento(servico) {
}

/**
 * @brief Executa o menu principal de gerenciamento de carteiras
 * 
 * @param cpf CPF do usuário autenticado para filtrar as carteiras
 * 
 * @details Apresenta um menu interativo com as seguintes opções:
 * - 1: Criar nova carteira
 * - 2: Listar e consultar carteiras existentes
 * - 3: Editar carteira existente
 * - 4: Excluir carteira
 * - 0: Voltar ao menu anterior
 * 
 * O menu é executado em loop até que o usuário escolha sair (opção 0).
 * Todas as operações são realizadas no contexto do CPF fornecido.
 */
void CarteiraController::executarMenu(const Ncpf& cpf) {
    int opcao;
    
    while (true) {
        exibirMenu();
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
                std::cout << "Opção inválida!" << std::endl;
        }
    }
}

/**
 * @brief Exibe o menu de opções de carteiras
 * 
 * @details Apresenta uma interface textual com as opções disponíveis
 * para gerenciamento de carteiras. O menu inclui:
 * - Cabeçalho formatado
 * - Lista numerada de opções
 * - Separador visual
 * - Prompt para entrada do usuário
 */
void CarteiraController::exibirMenu() {
    telaUtils::exibirCabecalho("MENU DE CARTEIRAS");
    std::cout << "1. Criar carteira" << std::endl;
    std::cout << "2. Listar e consultar carteiras" << std::endl;
    std::cout << "3. Editar carteira" << std::endl;
    std::cout << "4. Excluir carteira" << std::endl;
    std::cout << "0. Voltar" << std::endl;
    telaUtils::exibirSeparador('-', 40);
    std::cout << "Escolha uma opção: ";
}

/**
 * @brief Cria uma nova carteira de investimento
 * 
 * @param cpf CPF do usuário que está criando a carteira
 * 
 * @details Implementa o fluxo completo de criação de carteira:
 * 
 * 1. Verifica o limite de carteiras (máximo 5 por usuário)
 * 2. Solicita e valida o código da carteira (5 dígitos)
 * 3. Solicita e valida o nome da carteira (até 20 caracteres)
 * 4. Solicita e valida o tipo de perfil (Conservador/Moderado/Agressivo)
 * 5. Exibe resumo dos dados para confirmação
 * 6. Cria a carteira através do serviço
 * 
 * Validações implementadas:
 * - Limite máximo de 5 carteiras por usuário
 * - Código deve ser único no sistema
 * - Formato do código: exatamente 5 dígitos numéricos
 * - Nome: até 20 caracteres (letras, números e espaços)
 * - Perfil: deve ser um dos três tipos válidos
 * 
 * @note O usuário pode cancelar a operação a qualquer momento digitando '0'
 * @note A carteira é criada com saldo inicial zero
 */
void CarteiraController::criarCarteira(const Ncpf& cpf) {
    telaUtils::exibirCabecalho("CRIACAO DE NOVA CARTEIRA");
    std::cout << "(Digite '0' a qualquer momento para cancelar)" << std::endl;
    
    std::list<Carteira> carteirasExistentes;
    bool temCarteiras = servicoInvestimento->listarCarteiras(cpf, &carteirasExistentes);
    
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
    
    if (quantidadeAtual > 0) {
        exibirListaCarteiras(carteirasExistentes);
    }
    
    Codigo codigo;
    Nome nome;
    TipoPerfil tipoPerfil;
    
    while (true) {
        try {
            std::cout << "\nDigite o código da nova carteira (5 digitos): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            
            if (valorCodigo == "0") {
                std::cout << "Criacao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            codigo.setValor(valorCodigo);
            break; 
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use exatamente 5 digitos numericos (ex: 12345)." << std::endl;
        }
    }
    
    while (true) {
        try {
            std::cout << "Digite o nome da carteira: ";
            std::string valorNome;
            std::cin.ignore(); 
            std::getline(std::cin, valorNome);
            
            if (valorNome == "0") {
                std::cout << "Criacao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            nome.setValor(valorNome);
            break;
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use ate 20 caracteres (letras, numeros e espacos)." << std::endl;
        }
    }
    
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
            break; 
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
        }
    }
    
    Carteira novaCarteira;
    novaCarteira.setCodigo(codigo);
    novaCarteira.setNome(nome);
    novaCarteira.setTipoPerfil(tipoPerfil);
    
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
    
    if (servicoInvestimento->criarCarteira(cpf, novaCarteira)) {
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

/**
 * @brief Lista e permite consulta detalhada das carteiras do usuário
 * 
 * @param cpf CPF do usuário para filtrar as carteiras
 * 
 * @details Implementa um sistema de listagem interativo que:
 * 
 * 1. Exibe todas as carteiras do usuário em formato tabular
 * 2. Permite selecionar uma carteira específica para ver detalhes
 * 3. Mostra o saldo atual da carteira (calculado a partir das ordens)
 * 4. Lista todas as ordens associadas à carteira
 * 5. Oferece opções para editar ou excluir a carteira selecionada
 * 
 * Se o usuário não possui carteiras, oferece a opção de criar uma nova.
 * 
 * Funcionalidades específicas:
 * - Exibição do saldo total da carteira
 * - Listagem de todas as ordens associadas
 * - Acesso rápido às operações de edição e exclusão
 * - Validação de códigos de carteira
 * 
 * @note O saldo é calculado dinamicamente a partir das ordens de investimento
 * @note Apenas carteiras do usuário autenticado são exibidas
 */
void CarteiraController::listarCarteiras(const Ncpf& cpf) {
    while (true) {
        telaUtils::exibirCabecalho("LISTA DE CARTEIRAS");
        
        std::list<Carteira> carteiras;
        
        if (!servicoInvestimento->listarCarteiras(cpf, &carteiras) || carteiras.empty()) {
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
        
        exibirListaCarteiras(carteiras);
        
        std::cout << "\n>>> OPCOES <<<" << std::endl;
        std::cout << "- Digite o CODIGO de uma carteira para ver detalhes" << std::endl;
        std::cout << "- Digite 0 para voltar ao menu anterior" << std::endl;
        std::cout << "Escolha: ";
        
        std::string entrada;
        std::cin >> entrada;
        
        if (entrada == "0") {
            return;
        }
        
        try {
            Codigo codigo;
            codigo.setValor(entrada);
            
            Carteira carteiraDetalhada;
            Dinheiro saldo;
            
            if (servicoInvestimento->consultarCarteira(codigo, &carteiraDetalhada, &saldo)) {
                exibirDetalhesCarteira(carteiraDetalhada, saldo);
                
                std::list<Ordem> ordensCarteira;
                if (servicoInvestimento->listarOrdens(codigo, &ordensCarteira) && !ordensCarteira.empty()) {
                    std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
                    std::cout << std::left << std::setw(8) << "Codigo"
                              << std::setw(12) << "Data"
                              << std::setw(15) << "Valor"
                              << std::setw(10) << "Quantidade" << std::endl;
                    std::cout << std::string(45, '-') << std::endl;
                    
                    for (const Ordem& ordem : ordensCarteira) {
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
                            editarCarteiraEspecifica(cpf, carteiraDetalhada);
                            break;
                        }
                        case 2: {
                            if (excluirCarteiraEspecifica(cpf, carteiraDetalhada)) {
                                std::cout << "\nVoltando para a lista atualizada..." << std::endl;
                            }
                            break;
                        }
                        case 0:
                            break;
                        default:
                            std::cout << "Opcao invalida! Tente novamente." << std::endl;
                            continue;
                    }
                    break;
                }
                
            } else {
                std::cout << "\nErro: Carteira com codigo '" << entrada << "' nao foi encontrada." << std::endl;
                std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
                std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
                std::cin.ignore();
                std::cin.get();
            }
            
        } catch (const std::invalid_argument &exp) {
            std::cout << "\nErro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima." << std::endl;
            std::cout << "\nPressione qualquer tecla para tentar novamente..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    }
}

/**
 * @brief Exibe a lista de carteiras em formato tabular
 * 
 * @param carteiras Lista de carteiras a ser exibida
 * 
 * @details Cria uma tabela formatada com as seguintes colunas:
 * - Código: identificador único da carteira
 * - Nome: nome descritivo da carteira
 * - Perfil: tipo de perfil de investimento
 * 
 * A tabela inclui:
 * - Cabeçalho com títulos das colunas
 * - Separadores visuais
 * - Contador total de carteiras
 * 
 * @note A formatação utiliza std::setw para alinhamento das colunas
 */
void CarteiraController::exibirListaCarteiras(const std::list<Carteira>& carteiras) {
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
}

/**
 * @brief Exibe detalhes completos de uma carteira específica
 * 
 * @param carteira Carteira cujos detalhes serão exibidos
 * @param saldo Saldo atual da carteira
 * 
 * @details Apresenta uma visualização detalhada da carteira incluindo:
 * - Código da carteira
 * - Nome da carteira
 * - Tipo de perfil
 * - Saldo total atual
 * 
 * A exibição é formatada com separadores visuais para melhor legibilidade.
 * 
 * @note O saldo é fornecido como parâmetro separado pois é calculado
 * dinamicamente a partir das ordens de investimento
 */
void CarteiraController::exibirDetalhesCarteira(const Carteira& carteira, const Dinheiro& saldo) {
    std::cout << "\n==============================" << std::endl;
    std::cout << "    DETALHES DA CARTEIRA" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Codigo     : " << carteira.getCodigo().getValor() << std::endl;
    std::cout << "Nome       : " << carteira.getNome().getValor() << std::endl;
    std::cout << "Perfil     : " << carteira.getTipoPerfil().getValor() << std::endl;
    std::cout << "SALDO TOTAL: R$ " << saldo.getValor() << std::endl;
    std::cout << "==============================" << std::endl;
}

/**
 * @brief Inicia o processo de edição de carteira
 * 
 * @param cpf CPF do usuário autenticado
 * 
 * @details Implementa o fluxo de seleção e edição de carteira:
 * 
 * 1. Lista todas as carteiras do usuário
 * 2. Solicita o código da carteira a ser editada
 * 3. Valida a existência da carteira
 * 4. Chama o método de edição específica
 * 
 * Validações:
 * - Verifica se o usuário possui carteiras
 * - Valida o formato do código (5 dígitos)
 * - Confirma a existência da carteira no sistema
 * 
 * @note O usuário pode cancelar a operação digitando '0'
 * @note Apenas carteiras do usuário autenticado podem ser editadas
 */
void CarteiraController::editarCarteira(const Ncpf& cpf) {
    telaUtils::exibirCabecalho("EDICAO DE CARTEIRA");
    
    std::list<Carteira> carteiras;
    if (!servicoInvestimento->listarCarteiras(cpf, &carteiras) || carteiras.empty()) {
        std::cout << "\nVoce ainda nao possui carteiras para editar." << std::endl;
        std::cout << "Use a opcao 'Criar carteira' primeiro!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    exibirListaCarteiras(carteiras);
    
    Codigo codigo;
    Carteira carteiraAtual;
    
    while (true) {
        try {
            std::cout << "\nDigite o codigo da carteira que deseja editar (ou 0 para cancelar): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            
            if (valorCodigo == "0") {
                std::cout << "Edicao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            codigo.setValor(valorCodigo);
            
            Dinheiro saldoTemp;
            if (servicoInvestimento->consultarCarteira(codigo, &carteiraAtual, &saldoTemp)) {
                break;
            } else {
                std::cout << "Erro: Carteira com codigo '" << valorCodigo << "' nao foi encontrada." << std::endl;
                std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
            }
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima." << std::endl;
        }
    }
    
    editarCarteiraEspecifica(cpf, carteiraAtual);
}

/**
 * @brief Edita uma carteira específica
 * 
 * @param cpf CPF do usuário autenticado
 * @param carteiraAtual Carteira que será editada
 * 
 * @details Implementa a edição interativa de uma carteira específica:
 * 
 * 1. Exibe os dados atuais da carteira
 * 2. Permite edição do nome (opcional)
 * 3. Permite alteração do tipo de perfil (opcional)
 * 4. Exibe resumo das alterações
 * 5. Confirma e salva as alterações
 * 
 * Campos editáveis:
 * - Nome: pode ser mantido (ENTER) ou alterado
 * - Perfil: pode ser mantido ou alterado para um dos três tipos válidos
 * 
 * Validações:
 * - Nome: até 20 caracteres (letras, números e espaços)
 * - Perfil: deve ser Conservador, Moderado ou Agressivo
 * 
 * @note O código da carteira não pode ser alterado
 * @note O usuário pode cancelar a operação a qualquer momento
 */
void CarteiraController::editarCarteiraEspecifica(const Ncpf& cpf, const Carteira& carteiraAtual) {
    std::cout << "\n--- Edicao Rapida da Carteira ---" << std::endl;
    
    std::cout << "\n=== DADOS ATUAIS ===" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << " (nao pode ser alterado)" << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "====================" << std::endl;
    
    Carteira carteiraEditada = carteiraAtual;
    
    while (true) {
        try {
            std::cout << "\nNome atual: " << carteiraAtual.getNome().getValor() << std::endl;
            std::cout << "Digite o novo nome (ou ENTER para manter, ou 0 para cancelar): ";
            std::string novoNome;
            std::cin.ignore();
            std::getline(std::cin, novoNome);
            
            if (novoNome == "0") {
                std::cout << "Edicao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            if (novoNome.empty()) {
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
    
    while (true) {
        try {
            std::cout << "\nPerfil atual: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
            std::cout << "Alterar perfil? (s/n/0 para cancelar): ";
            char alterarPerfil;
            std::cin >> alterarPerfil;
            
            if (alterarPerfil == '0') {
                std::cout << "Edicao de carteira cancelada pelo usuario." << std::endl;
                return;
            }
            
            if (alterarPerfil == 's' || alterarPerfil == 'S') {
                std::cout << "\nPerfis disponiveis:" << std::endl;
                std::cout << "1. Conservador  2. Moderado  3. Agressivo" << std::endl;
                std::cout << "0. Cancelar edicao" << std::endl;
                std::cout << "Escolha (1-3) ou 0 para cancelar: ";
                
                int opcao;
                std::cin >> opcao;
                
                if (opcao == 0) {
                    std::cout << "Edicao de carteira cancelada pelo usuario." << std::endl;
                    return;
                }
                
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
    
    std::cout << "\n=== RESUMO ===" << std::endl;
    std::cout << "Nome : " << carteiraAtual.getNome().getValor() << " -> " << carteiraEditada.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << " -> " << carteiraEditada.getTipoPerfil().getValor() << std::endl;
    std::cout << "Confirma? (s/n): ";
    
    char confirmacao;
    std::cin >> confirmacao;
    
    if (confirmacao == 's' || confirmacao == 'S') {
        if (servicoInvestimento->editarCarteira(carteiraEditada)) {
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

/**
 * @brief Inicia o processo de exclusão de carteira
 * 
 * @param cpf CPF do usuário autenticado
 * 
 * @details Implementa o fluxo de seleção e exclusão de carteira:
 * 
 * 1. Lista todas as carteiras do usuário
 * 2. Solicita o código da carteira a ser excluída
 * 3. Valida a existência da carteira
 * 4. Chama o método de exclusão específica
 * 
 * Validações:
 * - Verifica se o usuário possui carteiras
 * - Valida o formato do código (5 dígitos)
 * - Confirma a existência da carteira no sistema
 * 
 * @note O usuário pode cancelar a operação digitando '0'
 * @note Apenas carteiras do usuário autenticado podem ser excluídas
 */
void CarteiraController::excluirCarteira(const Ncpf& cpf) {
    telaUtils::exibirCabecalho("EXCLUSAO DE CARTEIRA");
    
    std::list<Carteira> carteiras;
    if (!servicoInvestimento->listarCarteiras(cpf, &carteiras) || carteiras.empty()) {
        std::cout << "\nVoce ainda nao possui carteiras para excluir." << std::endl;
        std::cout << "Use a opcao 'Criar carteira' primeiro!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }
    
    exibirListaCarteiras(carteiras);
    
    Codigo codigo;
    Carteira carteiraParaExcluir;
    
    while (true) {
        try {
            std::cout << "\nDigite o codigo da carteira que deseja excluir (ou '0' para cancelar): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;
            
            if (valorCodigo == "0") {
                std::cout << "Exclusao cancelada pelo usuario." << std::endl;
                return;
            }
            
            codigo.setValor(valorCodigo);
            
            Dinheiro saldoTemp;
            if (servicoInvestimento->consultarCarteira(codigo, &carteiraParaExcluir, &saldoTemp)) {
                break;
            } else {
                std::cout << "Erro: Carteira com codigo '" << valorCodigo << "' nao foi encontrada." << std::endl;
                std::cout << "Verifique se o codigo esta correto na lista acima." << std::endl;
            }
        } catch (const std::invalid_argument &exp) {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um codigo de 5 digitos da lista acima, ou '0' para cancelar." << std::endl;
        }
    }
    
    excluirCarteiraEspecifica(cpf, carteiraParaExcluir);
}

/**
 * @brief Exclui uma carteira específica com validações de integridade
 * 
 * @param cpf CPF do usuário autenticado
 * @param carteiraAtual Carteira que será excluída
 * @return bool true se a exclusão foi bem-sucedida, false caso contrário
 * 
 * @details Implementa a exclusão segura de uma carteira com as seguintes etapas:
 * 
 * 1. Exibe detalhes da carteira a ser excluída
 * 2. Adverte sobre a natureza permanente da operação
 * 3. Solicita confirmação explícita do usuário
 * 4. Executa a exclusão através do serviço
 * 5. Informa o resultado da operação
 * 
 * Validações de integridade referencial:
 * - A carteira só pode ser excluída se não possuir ordens associadas
 * - O sistema verifica automaticamente a existência de ordens
 * - Se houver ordens, a exclusão é bloqueada e orientações são fornecidas
 * 
 * Confirmação de segurança:
 * - O usuário deve digitar 'sim' (ou variações) para confirmar
 * - A operação é cancelada para qualquer outra entrada
 * 
 * @note A exclusão é permanente e não pode ser desfeita
 * @note Todas as ordens da carteira devem ser excluídas primeiro
 * @note O sistema fornece orientações claras em caso de erro
 * 
 * @return true se a carteira foi excluída com sucesso
 * @return false se a exclusão falhou (ordens existentes ou erro do sistema)
 */
bool CarteiraController::excluirCarteiraEspecifica(const Ncpf& cpf, const Carteira& carteiraAtual) {
    std::cout << "\n--- Exclusao da Carteira ---" << std::endl;
    
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
    
    if (confirmacao != "SIM" && confirmacao != "sim" && 
        confirmacao != "S" && confirmacao != "s") {
        std::cout << "\nExclusao cancelada pelo usuario." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return false;
    }
    
    if (servicoInvestimento->excluirCarteira(carteiraAtual.getCodigo())) {
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