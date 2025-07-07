#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "controladorasApresentacao.hpp"
#include "controladorasServico.hpp"
#include "interfaces.hpp"

int main()
{
    ControladoraApresentacaoAutenticacao cntrApresentacaoAutenticacao;
    ControladoraApresentacaoUsuario cntrApresentacaoUsuario;
    ControladoraApresentacaoInvestimento cntrApresentacaoInvestimento;
    ControladoraServico cntrServico;

    if (!cntrServico.inicializar())
    {
        std::cerr << "❌ ERRO CRÍTICO: Não foi possível inicializar o banco de dados!" << std::endl;
        std::cerr << "Certifique-se de que o SQLite3 está instalado." << std::endl;
        std::cerr << "Ubuntu/Debian: sudo apt-get install sqlite3 libsqlite3-dev" << std::endl;
        return 1;
    }

    cntrApresentacaoAutenticacao.setControladoraServico(&cntrServico);
    cntrApresentacaoUsuario.setControladoraServico(&cntrServico);
    cntrApresentacaoInvestimento.setControladoraServico(&cntrServico);

    InterfaceManager interfaceManager(&cntrApresentacaoAutenticacao, &cntrApresentacaoUsuario,
                                      &cntrApresentacaoInvestimento);

    interfaceManager.executar();

    std::cout << "Sistema encerrado. Banco de dados desconectado." << std::endl;
    return 0;
}