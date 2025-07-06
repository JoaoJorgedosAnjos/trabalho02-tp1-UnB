#ifndef TELAUTILS_HPP_INCLUDED
#define TELAUTILS_HPP_INCLUDED

#include <string>

class telaUtils {
public:
    // Limpa a tela do console (funciona em Windows e Linux/macOS)
    static void limparTela();

    // Pausa a execução até o usuário pressionar Enter
    static void pausar(const std::string& mensagem = "Pressione qualquer tecla para continuar...");

    // Exibe um cabeçalho formatado, limpando a tela primeiro
    static void exibirCabecalho(const std::string& titulo);

    // Exibe uma linha separadora
    static void exibirSeparador(char caractere = '-', int tamanho = 50);
};

#endif // TELAUTILS_HPP_INCLUDED 