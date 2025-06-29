#ifndef TESTESDOMINIOS_HPP_INCLUDED
#define TESTESDOMINIOS_HPP_INCLUDED

#include<stdexcept>

#include "dominios.hpp"

using namespace std;

//Teste Unitario dominio: Codigo   (Responsavel: Micaele 231021450)
class TUCodigo {
    private:
        string VALOR_VALIDO = "12345";
        string VALOR_INVALIDO = "12a45";
        Codigo *codigo;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario dominio: Codigo Negociacao   (Responsavel: Maria 231021431)
class TUCodigoNeg {
    private:
        string VALOR_VALIDO = "ABC 123 DEF ";
        string VALOR_INVALIDO = "ABC123DEF456!";
        CodigoNeg *codigoNeg;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario dominio: CPF   (Responsavel: Karina 231006140)
class TUNcpf {
    private:
        string VALOR_VALIDO = "111.444.777-35";
        string VALOR_INVALIDO = "529.982.247.25";
        Ncpf *cpf;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};


//Teste Unitario dominio: Data   (Responsavel: Bruno 241022460)
class TUData {
    private:
        string VALOR_VALIDO = "20240229";
        string VALOR_INVALIDO = "20230229";
        Data *data;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};


//Teste Unitario dominio: Nome   (Responsavel: Jorge 241004686)
class TUNome {
    private:
        string VALOR_VALIDO = "Joao1 Silva";
        string VALOR_INVALIDO = "Ana  Clara";
        Nome *nome;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario dominio: Perfil   (Responsavel: Micaele 231021450)
class TUTipoPerfil {
    private:
        string VALOR_VALIDO = "Moderado";
        string VALOR_INVALIDO = "conservador";
        TipoPerfil *perfil;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario dominio: Dinheiro   (Responsavel: Karina 231006140)
class TUDinheiro {
    private:
        string VALOR_VALIDO = "999.999,99";
        string VALOR_INVALIDO = "0,001";
        Dinheiro *dinheiro;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario dominio: Quantidade   (Responsavel: Bruno 241022460)
class TUQuantidade {
    private:
        string VALOR_VALIDO = "1.000";
        string VALOR_INVALIDO = "01";
        Quantidade *quantidade;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario dominio: Senha   (Responsavel: Jorge 241004686)
class TUSenha {
    private:
        string VALOR_VALIDO = "A1b$2c";
        string VALOR_INVALIDO = "Ab1#Ab";
        Senha *senha;
        int estado;
        void setUp();
        void tearDown();
        void testarCenarioValorValido();
        void testarCenarioValorInvalido();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

#endif // TESTES_HPP_INCLUDED
