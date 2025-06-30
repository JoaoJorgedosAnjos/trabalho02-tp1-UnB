#include <stdexcept>
#include <iostream>
#include <string>

#include "interfaces.hpp"
#include "controladorasApresentacao.hpp"
#include "controladorasServico.hpp"

int main() {
    // Instanciar as controladoras concretas de cada camada.
    ControladoraApresentacaoAutenticacao cntrApresentacaoAutenticacao;
    ControladoraServico cntrServico; // Nossa controladora de serviço já tem um usuário de teste

    // Injetar a dependência: conectar a camada de apresentação com a de serviço.
    // A controladora de apresentação agora "sabe" com quem falar para pedir serviços.
    cntrApresentacaoAutenticacao.setControladoraServico(&cntrServico);

    Ncpf cpfAutenticado;
    
    // Iniciar o caso de uso de autenticação.
    if (cntrApresentacaoAutenticacao.autenticar(&cpfAutenticado)) {
        std::cout << "\n>>> Autenticacao realizada com sucesso <<<" << std::endl;
        std::cout << "Usuario autenticado: " << cpfAutenticado.getValor() << std::endl;

        // Futuramente, aqui chamaremos os menus principais do sistema.

    } else {
        std::cout << "\n>>> Falha na autenticacao. CPF ou senha invalidos. <<<" << std::endl;
    }

    return 0;
}