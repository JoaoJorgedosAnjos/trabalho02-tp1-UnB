#include <stdexcept>
#include <iostream>
#include <string>

#include "interfaces.hpp"
#include "controladorasApresentacao.hpp"
#include "controladorasServico.hpp"

int main() {

    
    // Instanciar as controladoras concretas de cada camada.
    ControladoraApresentacaoAutenticacao cntrApresentacaoAutenticacao;
    ControladoraApresentacaoUsuario cntrApresentacaoUsuario;
    ControladoraApresentacaoInvestimento cntrApresentacaoInvestimento;
    ControladoraServico cntrServico;

    // Inicializar banco de dados SQLite
    if (!cntrServico.inicializar()) {
        std::cerr << "❌ ERRO CRÍTICO: Não foi possível inicializar o banco de dados!" << std::endl;
        std::cerr << "Certifique-se de que o SQLite3 está instalado." << std::endl;
        std::cerr << "Ubuntu/Debian: sudo apt-get install sqlite3 libsqlite3-dev" << std::endl;
        return 1;
    }

    // Injetar as dependências: conectar as camadas de apresentação com a de serviço.
    cntrApresentacaoAutenticacao.setControladoraServico(&cntrServico);
    cntrApresentacaoUsuario.setControladoraServico(&cntrServico);
    cntrApresentacaoInvestimento.setControladoraServico(&cntrServico);

    // Menu inicial: Login ou Cadastro
    int opcaoInicial;
    while (true) {
        std::cout << "\n=== GERENCIAMENTO DE CONTA ===" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Cadastrar nova conta" << std::endl;
        std::cout << "0. Sair" << std::endl;
        std::cout << "Escolha uma opção: ";
        std::cin >> opcaoInicial;
        
        if (opcaoInicial == 0) {
            std::cout << "Obrigado por utilizar nosso sistema!" << std::endl;
            break;
        } else if (opcaoInicial == 2) {
            cntrApresentacaoUsuario.cadastrar();
            continue;
        } else if (opcaoInicial != 1) {
            std::cout << "Opção inválida!" << std::endl;
            continue;
        }
        
        // Processo de autenticação
        Ncpf cpfAutenticado;
        if (cntrApresentacaoAutenticacao.autenticar(&cpfAutenticado)) {
            std::cout << "\n>>> Autenticação realizada com sucesso <<<" << std::endl;
            std::cout << "Usuário autenticado: " << cpfAutenticado.getValor() << std::endl;

            // Menu principal do sistema
            int opcao;
            while (true) {
                std::cout << "\n=== MENU PRINCIPAL ===" << std::endl;
                std::cout << "1. Gerenciar Conta" << std::endl;
                std::cout << "2. Gerenciar Investimentos" << std::endl;
                std::cout << "0. Logout" << std::endl;
                std::cout << "Escolha uma opção: ";
                std::cin >> opcao;
                
                switch (opcao) {
                    case 1:
                        if (cntrApresentacaoUsuario.executar(cpfAutenticado)) {
                            // Conta foi excluída, fazer logout automático
                            std::cout << "Logout automático realizado." << std::endl;
                            goto logout;
                        }
                        break;
                    case 2:
                        cntrApresentacaoInvestimento.executar(cpfAutenticado);
                        break;
                    case 0:
                        std::cout << "Logout realizado com sucesso!" << std::endl;
                        goto logout; // Sai dos dois loops
                    default:
                        std::cout << "Opção inválida!" << std::endl;
                }
            }
            logout:;
        } else {
            std::cout << "\n>>> Falha na autenticação. CPF ou senha inválidos. <<<" << std::endl;
        }
    }

    std::cout << "Sistema encerrado. Banco de dados desconectado." << std::endl;
    return 0;
}