#include "telaUtils.hpp"
#include <cstdlib>
#include <iostream>
#include <limits>

void telaUtils::limparTela()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void telaUtils::pausar(const std::string &mensagem)
{
    std::cout << "\n" << mensagem << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void telaUtils::exibirCabecalho(const std::string &titulo)
{
    limparTela();
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "          " << titulo << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << std::endl;
}

void telaUtils::exibirSeparador(char caractere, int tamanho)
{
    std::cout << std::string(tamanho, caractere) << std::endl;
}