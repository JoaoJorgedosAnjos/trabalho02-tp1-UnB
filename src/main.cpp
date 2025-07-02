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
    ControladoraServico cntrServico; // Nossa controladora de serviço já tem um usuário de teste

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
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcaoInicial;
        
        if (opcaoInicial == 0) {
            std::cout << "Obrigado por utilizar nosso sistema!" << std::endl;
            break;
        } else if (opcaoInicial == 2) {
            cntrApresentacaoUsuario.cadastrar();
            continue;
        } else if (opcaoInicial != 1) {
            std::cout << "Opcao invalida!" << std::endl;
            continue;
        }
        
        // Processo de autenticação
        Ncpf cpfAutenticado;
        if (cntrApresentacaoAutenticacao.autenticar(&cpfAutenticado)) {
            std::cout << "\n>>> Autenticacao realizada com sucesso <<<" << std::endl;
            std::cout << "Usuario autenticado: " << cpfAutenticado.getValor() << std::endl;

            // Menu principal do sistema
            int opcao;
            while (true) {
                std::cout << "\n=== MENU PRINCIPAL ===" << std::endl;
                std::cout << "1. Gerenciar Conta" << std::endl;
                std::cout << "2. Gerenciar Investimentos" << std::endl;
                std::cout << "0. Logout" << std::endl;
                std::cout << "Escolha uma opcao: ";
                std::cin >> opcao;
                
                switch (opcao) {
                    case 1:
                        cntrApresentacaoUsuario.executar(cpfAutenticado);
                        break;
                    case 2:
                        cntrApresentacaoInvestimento.executar(cpfAutenticado);
                        break;
                    case 0:
                        std::cout << "Logout realizado com sucesso!" << std::endl;
                        goto logout; // Sai dos dois loops
                    default:
                        std::cout << "Opcao invalida!" << std::endl;
                }
            }
            logout:;
        } else {
            std::cout << "\n>>> Falha na autenticacao. CPF ou senha invalidos. <<<" << std::endl;
        }
    }

    return 0;
}