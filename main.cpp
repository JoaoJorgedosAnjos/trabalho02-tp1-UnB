#include <iostream>
#include <stdexcept>

#include "libTrabalho01/dominios/dominios.hpp"
#include "libTrabalho01/entidades/entidades.hpp"
#include "libTrabalho01/tests/testesDominios.hpp"
#include "libTrabalho01/tests/testesEntidades.hpp"

using namespace std;


int main(){
// Dominios
// Codigo
    TUCodigo testeCodigo;

    switch(testeCodigo.run()) {
        case TUCodigo::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUCodigo::FALHA:cout << "FALHA" << endl;
        break;
    }

// Codigo Negociacao
    TUCodigoNeg testeCodigoNeg;

    switch(testeCodigoNeg.run()) {
        case TUCodigoNeg::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUCodigoNeg::FALHA:cout << "FALHA" << endl;
        break;
    }

// CPF
    TUNcpf testeNcpf;

    switch(testeNcpf.run()) {
        case TUNcpf::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUNcpf::FALHA:cout << "FALHA" << endl;
        break;
    }

// Data
    TUData testeData;

    switch(testeData.run()) {
        case TUData::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUData::FALHA:cout << "FALHA" << endl;
        break;
    }

// Nome
    TUNome testeNome;

    switch(testeNome.run()) {
        case TUNome::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUNome::FALHA:cout << "FALHA" << endl;
        break;
    }

// Perfil
    TUTipoPerfil testeTipoPerfil;

    switch(testeTipoPerfil.run()) {
        case TUTipoPerfil::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUTipoPerfil::FALHA:cout << "FALHA" << endl;
        break;
    }

// Dinheiro
    TUDinheiro testeDinheiro;

    switch(testeDinheiro.run()) {
        case TUDinheiro::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUDinheiro::FALHA:cout << "FALHA" << endl;
        break;
    }

// Quantidade
    TUQuantidade testeQuantidade;

    switch(testeQuantidade.run()) {
        case TUQuantidade::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUQuantidade::FALHA:cout << "FALHA" << endl;
        break;
    }

// Senha
    TUSenha testeSenha;

    switch(testeSenha.run()) {
        case TUSenha::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUSenha::FALHA:cout << "FALHA" << endl;
        break;
    }

// Entidades
// Conta
    TUConta testeConta;

    switch(testeConta.run()) {
        case TUConta::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUConta::FALHA:cout << "FALHA" << endl;
        break;
    }

// Carteira
    TUCarteira testeCarteira;

    switch(testeCarteira.run()) {
        case TUCarteira::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUCarteira::FALHA:cout << "FALHA" << endl;
        break;
    }

// Ordem
    TUOrdem testeOrdem;

    switch(testeOrdem.run()) {
        case TUOrdem::SUCESSO:cout << "SUCESSO" << endl;
        break;
    case TUOrdem::FALHA:cout << "FALHA" << endl;
        break;
    }

    return 0;
}
