#include "testesEntidades.hpp"

//Teste Unitario entidade: Conta
const string TUConta::CPF_VALIDO = "123.456.789-09";
const string TUConta::NOME_VALIDO = "Maria Clara";
const string TUConta::SENHA_VALIDA = "A1b$2c";

void TUConta::setUp() {
    conta = new Conta();
    estado = SUCESSO;
}

void TUConta::tearDown() {
    delete conta;
}

void TUConta::testarCenario() {
    Ncpf cpf;
    cpf.setValor(CPF_VALIDO);
    conta->setNcpf(cpf);
    if(conta->getNcpf().getValor() != CPF_VALIDO)
         estado = FALHA;

    Nome nome;
    nome.setValor(NOME_VALIDO);
    conta->setNome(nome);
    if(conta->getNome().getValor() != NOME_VALIDO)
         estado = FALHA;

    Senha senha;
    senha.setValor(SENHA_VALIDA);
    conta->setSenha(senha);
    if(conta->getSenha().getValor() != SENHA_VALIDA)
         estado = FALHA;
}

int TUConta::run() {
    setUp();
    testarCenario();
    tearDown();
    return estado;
}

//Teste Unitario entidade: Carteira
const string TUCarteira::CODIGO_VALIDO = "12345";
const string TUCarteira::NOME_VALIDO = "Maria Clara";
const string TUCarteira::PERFIL_VALIDO = "Moderado";

void TUCarteira::setUp() {
    carteira = new Carteira();
    estado = SUCESSO;
}

void TUCarteira::tearDown() {
    delete carteira;
}

void TUCarteira::testarCenario() {
    Codigo codigo;
    codigo.setValor(CODIGO_VALIDO);
    carteira->setCodigo(codigo);
    if(carteira->getCodigo().getValor() != CODIGO_VALIDO)
         estado = FALHA;

    Nome nome;
    nome.setValor(NOME_VALIDO);
    carteira->setNome(nome);
    if(carteira->getNome().getValor() != NOME_VALIDO)
         estado = FALHA;

    TipoPerfil perfil;
    perfil.setValor(PERFIL_VALIDO);
    carteira->setTipoPerfil(perfil);
    if(carteira->getTipoPerfil().getValor() != PERFIL_VALIDO)
         estado = FALHA;
}

int TUCarteira::run() {
    setUp();
    testarCenario();
    tearDown();
    return estado;
}

//Teste Unitario entidade: Ordem
const string TUOrdem::CODIGO_VALIDO = "12345";
const string TUOrdem::COD_NEG_VALIDO = "ABC 123 DEF ";
const string TUOrdem::DATA_VALIDA = "20240229";
const string TUOrdem::SALDO_VALIDO = "999.999,99";
const string TUOrdem::QUANTIDADE_VALIDA = "100.000";

void TUOrdem::setUp() {
    ordem = new Ordem();
    estado = SUCESSO;
}

void TUOrdem::tearDown() {
    delete ordem;
}

void TUOrdem::testarCenario() {
    Codigo codigo;
    codigo.setValor(CODIGO_VALIDO);
    ordem->setCodigo(codigo);
    if(ordem->getCodigo().getValor() != CODIGO_VALIDO)
         estado = FALHA;

    CodigoNeg codigoNeg;
    codigoNeg.setValor(COD_NEG_VALIDO);
    ordem->setCodigoNeg(codigoNeg);
    if(ordem->getCodigoNeg().getValor() != COD_NEG_VALIDO)
         estado = FALHA;

    Data data;
    data.setValor(DATA_VALIDA);
    ordem->setData(data);
    if(ordem->getData().getValor() != DATA_VALIDA)
         estado = FALHA;

    Dinheiro dinheiro;
    dinheiro.setValor(SALDO_VALIDO);
    ordem->setDinheiro(dinheiro);
    if(ordem->getDinheiro().getValor() != SALDO_VALIDO)
         estado = FALHA;

    Quantidade quantidade;
    quantidade.setValor(QUANTIDADE_VALIDA);
    ordem->setQuantidade(quantidade);
    if(ordem->getQuantidade().getValor() != QUANTIDADE_VALIDA)
         estado = FALHA;
}

int TUOrdem::run() {
    setUp();
    testarCenario();
    tearDown();
    return estado;
}
