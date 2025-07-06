#ifndef TELAUTILS_HPP_INCLUDED
#define TELAUTILS_HPP_INCLUDED

#include <string>

class telaUtils {
public:
    static void limparTela();

    static void pausar(const std::string& mensagem = "Pressione qualquer tecla para continuar...");

    static void exibirCabecalho(const std::string& titulo);

    static void exibirSeparador(char caractere = '-', int tamanho = 50);
};

#endif // TELAUTILS_HPP_INCLUDED 