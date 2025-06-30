#include "controladorasServico.hpp"

// Construtor: Vamos criar um usuário de teste para que a função de autenticar possa ser verificada.
ControladoraServico::ControladoraServico() {
    // Cria um usuário "stub" para testes
    Conta contaTeste;
    Ncpf cpfTeste;
    Nome nomeTeste;
    Senha senhaTeste;

    // Define dados válidos para o usuário de teste
    cpfTeste.setValor("043.488.481-22"); // Use um CPF válido se a sua classe já o valida
    nomeTeste.setValor("Usuario Teste");
    senhaTeste.setValor("Test#1");       // Senha que atende aos critérios: 1 maiúscula, 1 minúscula, 1 número, 1 especial, sem repetidos, 6 chars.
    
    contaTeste.setNcpf(cpfTeste);
    contaTeste.setNome(nomeTeste);
    contaTeste.setSenha(senhaTeste);

    // Adiciona a conta de teste ao nosso "banco de dados" em memória (o map)
    contas[cpfTeste.getValor()] = contaTeste;
}


// Implementação do método de autenticação
bool ControladoraServico::autenticar(const Ncpf& cpf, const Senha& senha) {
    // Procura a conta no map usando o CPF como chave
    auto it = contas.find(cpf.getValor());

    // Se o iterador for igual a 'end', significa que a conta não foi encontrada.
    if (it == contas.end()) {
        return false; // Usuário não existe.
    }

    // Se encontrou, 'it->second' é o objeto Conta.
    // Compara a senha fornecida com a senha armazenada na conta.
    if (it->second.getSenha().getValor() == senha.getValor()) {
        return true; // Senha correta. Autenticação bem-sucedida.
    }

    return false; // Senha incorreta.
}


// --- Implementações Vazias (Stubs) ---
// Vamos deixar os outros métodos vazios por enquanto, apenas para o código compilar.
// Implementaremos cada um deles nos próximos passos.

bool ControladoraServico::cadastrarConta(const Conta& conta) { return false; }
bool ControladoraServico::consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) { return false; }
bool ControladoraServico::editarConta(const Conta& conta) { return false; }
bool ControladoraServico::excluirConta(const Ncpf& cpf) { return false; }
bool ControladoraServico::criarCarteira(const Ncpf& cpf, const Carteira& carteira) { return false; }
bool ControladoraServico::listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) { return false; }
bool ControladoraServico::consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) { return false; }
bool ControladoraServico::editarCarteira(const Carteira& carteira) { return false; }
bool ControladoraServico::excluirCarteira(const Codigo& codigo) { return false; }
bool ControladoraServico::criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) { return false; }
bool ControladoraServico::listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) { return false; }
bool ControladoraServico::excluirOrdem(const Codigo& codigo) { return false; }