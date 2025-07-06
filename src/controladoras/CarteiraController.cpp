#include "CarteiraController.hpp"
#include <limits>

CarteiraController::CarteiraController(IServicoInvestimento* servico) 
    : servicoInvestimento(servico) {
}

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