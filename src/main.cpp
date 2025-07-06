#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>

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

    // Criar e executar o gerenciador de interface
    InterfaceManager interfaceManager(
        &cntrApresentacaoAutenticacao,
        &cntrApresentacaoUsuario,
        &cntrApresentacaoInvestimento
    );
    
    interfaceManager.executar();

    std::cout << "Sistema encerrado. Banco de dados desconectado." << std::endl;
    return 0;
}