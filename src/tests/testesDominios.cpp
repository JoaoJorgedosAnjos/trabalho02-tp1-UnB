#include "testesDominios.hpp"

//Teste Unitario dominio: Codigo
void TUCodigo::setUp() {
    codigo = new Codigo();
    estado = SUCESSO;
}

void TUCodigo::tearDown() {
    delete codigo;
}

void TUCodigo::testarCenarioValorValido() {
    try {
        codigo->setValor(VALOR_VALIDO);
        if (codigo->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUCodigo::testarCenarioValorInvalido() {
    try {
        codigo->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (codigo->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUCodigo::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Codigo Negociacao
void TUCodigoNeg::setUp() {
    codigoNeg = new CodigoNeg();
    estado = SUCESSO;
}

void TUCodigoNeg::tearDown() {
    delete codigoNeg;
}

void TUCodigoNeg::testarCenarioValorValido() {
    try {
        codigoNeg->setValor(VALOR_VALIDO);
        if (codigoNeg->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUCodigoNeg::testarCenarioValorInvalido() {
    try {
        codigoNeg->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (codigoNeg->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUCodigoNeg::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: CPF
void TUNcpf::setUp() {
    cpf = new Ncpf();
    estado = SUCESSO;
}

void TUNcpf::tearDown() {
    delete cpf;
}

void TUNcpf::testarCenarioValorValido() {
    try {
        cpf->setValor(VALOR_VALIDO);
        if (cpf->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUNcpf::testarCenarioValorInvalido() {
    try {
        cpf->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (cpf->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUNcpf::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Data
void TUData::setUp() {
    data = new Data();
    estado = SUCESSO;
}

void TUData::tearDown() {
    delete data;
}

void TUData::testarCenarioValorValido() {
    try {
        data->setValor(VALOR_VALIDO);
        if (data->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUData::testarCenarioValorInvalido() {
    try {
        data->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (data->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUData::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Nome
void TUNome::setUp() {
    nome = new Nome();
    estado = SUCESSO;
}

void TUNome::tearDown() {
    delete nome;
}

void TUNome::testarCenarioValorValido() {
    try {
        nome->setValor(VALOR_VALIDO);
        if (nome->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUNome::testarCenarioValorInvalido() {
    try {
        nome->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (nome->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUNome::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Perfil
void TUTipoPerfil::setUp() {
    perfil = new TipoPerfil();
    estado = SUCESSO;
}

void TUTipoPerfil::tearDown() {
    delete perfil;
}

void TUTipoPerfil::testarCenarioValorValido() {
    try {
        perfil->setValor(VALOR_VALIDO);
        if (perfil->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUTipoPerfil::testarCenarioValorInvalido() {
    try {
        perfil->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (perfil->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUTipoPerfil::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Dinheiro
void TUDinheiro::setUp() {
    dinheiro = new Dinheiro();
    estado = SUCESSO;
}

void TUDinheiro::tearDown() {
    delete dinheiro;
}

void TUDinheiro::testarCenarioValorValido() {
    try {
        dinheiro->setValor(VALOR_VALIDO);
        if (dinheiro->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUDinheiro::testarCenarioValorInvalido() {
    try {
        dinheiro->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (dinheiro->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUDinheiro::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Quantidade
void TUQuantidade::setUp() {
    quantidade = new Quantidade();
    estado = SUCESSO;
}

void TUQuantidade::tearDown() {
    delete quantidade;
}

void TUQuantidade::testarCenarioValorValido() {
    try {
        quantidade->setValor(VALOR_VALIDO);
        if (quantidade->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUQuantidade::testarCenarioValorInvalido() {
    try {
        quantidade->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (quantidade->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUQuantidade::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

//Teste Unitario dominio: Senha
void TUSenha::setUp() {
    senha = new Senha();
    estado = SUCESSO;
}

void TUSenha::tearDown() {
    delete senha;
}

void TUSenha::testarCenarioValorValido() {
    try {
        senha->setValor(VALOR_VALIDO);
        if (senha->getValor() != VALOR_VALIDO)
            estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        estado = FALHA;
    }
}

void TUSenha::testarCenarioValorInvalido() {
    try {
        senha->setValor(VALOR_INVALIDO);
        estado = FALHA;
    }
    catch (invalid_argument &excecao) {
        if (senha->getValor() == VALOR_INVALIDO)
            estado = FALHA;
    }
}

int TUSenha::run() {
    setUp();
    testarCenarioValorValido();
    testarCenarioValorInvalido();
    tearDown();
    return estado;
}

