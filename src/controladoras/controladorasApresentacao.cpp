#include <iostream>
#include "controladorasApresentacao.hpp"

// Implementação do método set para injeção de dependência.
void ControladoraApresentacaoAutenticacao::setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) {
    this->cntrServicoAutenticacao = cntrServicoAutenticacao;
}

// Implementação do método que apresenta o formulário de autenticação.
bool ControladoraApresentacaoAutenticacao::autenticar(Ncpf* cpf) {
    std::string valor;
    Senha senha;

    while (true) {
        std::cout << "\n--- Autenticacao de Usuario ---" << std::endl;

        try {
            std::cout << "CPF    (XXX.XXX.XXX-XX): ";
            std::cin >> valor;
            cpf->setValor(valor); // Validação ocorre aqui, no domínio

            std::cout << "Senha  (6 caracteres)  : ";
            std::cin >> valor;
            senha.setValor(valor); // Validação ocorre aqui, no domínio
            
            break; // Se ambos os dados forem válidos, sai do loop.

        } catch (const std::invalid_argument &exp) {
            // Se a validação no domínio falhar, uma exceção é capturada.
            std::cout << "\nErro: " << exp.what() << std::endl;
            std::cout << "Pressione qualquer tecla para tentar novamente..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }
    }

    // Chama o método da camada de serviço através da interface.
    // A camada de apresentação não sabe como a autenticação é feita, ela apenas delega.
    return cntrServicoAutenticacao->autenticar(*cpf, senha);
}