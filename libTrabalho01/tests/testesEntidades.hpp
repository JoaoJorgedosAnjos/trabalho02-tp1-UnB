#ifndef TESTESENTIDADES_HPP_INCLUDED
#define TESTESENTIDADES_HPP_INCLUDED

#include <stdexcept>

#include "../dominios/dominios.hpp"
#include "../entidades/entidades.hpp"

//Teste Unitario entidade: Conta   (Responsavel: Micaele 231021450)
class TUConta {
    private:
        const static string CPF_VALIDO;
        const static string NOME_VALIDO;
        const static string SENHA_VALIDA;
        Conta *conta;
        int estado;
        void setUp();
        void tearDown();
        void testarCenario();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario entidade: Carteira   (Responsavel: Jorge 241004686)
class TUCarteira {
    private:
        const static string CODIGO_VALIDO;
        const static string NOME_VALIDO;
        const static string PERFIL_VALIDO;
        Carteira *carteira;
        int estado;
        void setUp();
        void tearDown();
        void testarCenario();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

//Teste Unitario entidade: Ordem   (Responsavel: Karina 231006140)
class TUOrdem {
    private:
        const static string CODIGO_VALIDO;
        const static string COD_NEG_VALIDO;
        const static string DATA_VALIDA;
        const static string SALDO_VALIDO;
        const static string QUANTIDADE_VALIDA;
        Ordem *ordem;
        int estado;
        void setUp();
        void tearDown();
        void testarCenario();

    public:
        const static int SUCESSO = 0;
        const static int FALHA = -1;
        int run();
};

#endif // TESTESENTIDADES_HPP_INCLUDED
