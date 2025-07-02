/**
 * @file entidades.hpp
 * @brief Biblioteca de entidades do sistema de investimentos financeiros.
 *
 * Este arquivo contem a definicao das entidades principais utilizadas
 * na modelagem do sistema, que representam os objetos de negocio como
 * Conta, Carteira e Ordem.
 *
 * As entidades encapsulam dominios especificos que garantem a integridade
 * dos dados atraves de regras de validacao rigorosas. Sao responsaveis por
 * organizar e manter os dados estruturados no sistema, atuando como elo entre
 * a camada de dominio e as funcionalidades da aplicacao.
 *
 * @details
 * Cada entidade e formada por atributos privados representados por dominios,
 * e metodos publicos de acesso (getters e setters). As validacoes sao garantidas
 * pela definicao de cada dominio, promovendo seguranca e padronizacao.
 *
 * Entidades disponiveis:
 * - @ref Conta
 * - @ref Carteira
 * - @ref Ordem
 *
 * @see dominios.hpp
 */
//---------------------------------------------------------------------
#ifndef ENTIDADES_HPP_INCLUDED
#define ENTIDADES_HPP_INCLUDED

//---------------------------------------------------------------------
#include "../dominios/dominios.hpp"

using namespace std;

///---------------------------------------------------------------------
//Entidade: Conta   (Responsavel: Maria 231021431)
/**
 * @class Conta
 * @brief Classe que representa um conjunto de dados que identifica um usuario no sistema de investimentos financeiros.
 *
 * A classe `Conta` e utilizada para armazenar as informacoes essenciais de um usuario
 * autenticado no sistema, permitindo seu acesso, personalizacao e controle de permissoes
 * protegidas por credenciais.
 * Cada conta esta associada a um CPF unico, um nome e uma senha segura.
 *
 * Esta entidade e fundamental para a gestao de acessos e identificacao dos usuarios
 * no sistema de investimentos. Ela utiliza os dominios previamente definidos para garantir
 * que os valores atribuidos a seus atributos obedecam as regras de negocio.
 *
 * Atributos:
 * - `cpf`: Numero do CPF do usuario, encapsulado pelo dominio `Ncpf`.
 * - `nome`: Nome completo do usuario, encapsulado pelo dominio `Nome`.
 * - `senha`: Senha de acesso do usuario, encapsulada pelo dominio `Senha`.
 *
 * Metodos publicos:
 * - `setNcpf(Ncpf)`: Define o CPF do usuario apos validacao.
 * - `getNcpf()`: Retorna o CPF armazenado.
 * - `setNome(Nome)`: Define o nome do usuario apos validacao.
 * - `getNome()`: Retorna o nome armazenado.
 * - `setSenha(Senha)`: Define a senha do usuario apos validacao.
 * - `getSenha()`: Retorna a senha armazenada.
 */
class Conta {
    private:
        /**
         * @brief Objeto do dominio `Ncpf` representando o CPF do usuario.
         */
        Ncpf cpf;

        /**
         * @brief Objeto do dominio `Nome` representando o nome do usuario.
         */
        Nome nome;

        /**
         * @brief Objeto do dominio `Senha` representando a senha do usuario.
         */
        Senha senha;

    public:
        /**
         * @brief Metodo publico que define o CPF da conta.
         * @param cpf Objeto `Ncpf` contendo o CPF a ser atribuido.
         */
        void setNcpf(Ncpf);

        /**
         * @brief Metodo publico que retorna o CPF armazenado na conta.
         * @return Objeto `Ncpf` com o CPF do usuario.
         */
        Ncpf getNcpf() const;

        /**
         * @brief Metodo publico que define o nome do titular da conta.
         * @param nome Objeto `Nome` contendo o nome a ser atribuido.
         */
        void setNome(Nome);

        /**
         * @brief Metodo publico que retorna o nome do titular da conta.
         * @return Objeto `Nome` com o nome do usuario.
         */
        Nome getNome() const;

        /**
         * @brief Metodo publico que define a senha da conta.
         * @param senha Objeto `Senha` contendo a senha a ser atribuida.
         */
        void setSenha(Senha);

        /**
         * @brief Metodo publico que retorna a senha armazenada na conta.
         * @return Objeto `Senha` com a senha do usuario.
         */
        Senha getSenha() const;
};
//---------------------------------------------------------------------

// Implementacoes inline dos metodos da classe Conta
//---------------------------------------------------------------------
/**
 * @brief Atribui um objeto `Ncpf` ao atributo `cpf`.
 * @param cpf Objeto do dominio `Ncpf`.
 */
inline void Conta::setNcpf(Ncpf cpf){ this->cpf = cpf; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o CPF da conta.
 * @return Objeto `Ncpf`.
 */
inline Ncpf Conta::getNcpf() const { return cpf; }

//---------------------------------------------------------------------
/**
 * @brief Atribui um objeto `Nome` ao atributo `nome`.
 * @param nome Objeto do dominio `Nome`.
 */
inline void Conta::setNome(Nome nome){ this->nome = nome; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o nome da conta.
 * @return Objeto `Nome`.
 */
inline Nome Conta::getNome() const { return nome; }

//---------------------------------------------------------------------
/**
 * @brief Atribui um objeto `Senha` ao atributo `senha`.
 * @param senha Objeto do dominio `Senha`.
 */
inline void Conta::setSenha(Senha senha){ this->senha = senha; }

//---------------------------------------------------------------------
/**
 * @brief Retorna a senha da conta.
 * @return Objeto `Senha`.
 */
inline Senha Conta::getSenha() const { return senha; }

///---------------------------------------------------------------------
//Entidade: Carteira   (Responsavel: Bruno 241022460)
/**
 * @class Carteira
 * @brief Classe que representa a estrutura ou composicao dos diferentes ativos de investimentos (como acoes,
 * titulos publicos, fundos, imoveis, etc.) pertencentes a um investidor no sistema.
 *
 * A classe `Carteira` encapsula os dados relacionados a uma carteira de investimentos,
 * sendo ela uma instancia que agrupa produtos ou aplicacoes financeiras sob um perfil
 * de investidor especifico.
 *
 * Esta entidade e essencial para controlar, organizar, acompanhar e gerenciar aplicacoes das carteiras criadas
 * pelos usuarios, conforme seu perfil de risco e objetivos financeiros. Permite identificar sua designacao,
 * codigo e o tipo de perfil ao qual se relaciona.
 *
 * Atributos:
 * - `codigo`: Codigo identificador unico da carteira (dominio `Codigo`).
 * - `nome`: Nome ou designacao da carteira (dominio `Nome`).
 * - `perfil`: Tipo de perfil do investidor associado (dominio `TipoPerfil`).
 *
 * Metodos publicos:
 * - `setCodigo(Codigo)`: Define o codigo da carteira.
 * - `getCodigo()`: Retorna o codigo armazenado.
 * - `setNome(Nome)`: Define o nome da carteira.
 * - `getNome()`: Retorna o nome armazenado.
 * - `setTipoPerfil(TipoPerfil)`: Define o tipo de perfil associado.
 * - `getTipoPerfil()`: Retorna o tipo de perfil associado.
 */
class Carteira {
    private:
        /**
         * @brief Objeto do dominio `Codigo` representando o codigo da carteira.
         */
        Codigo codigo;

        /**
         * @brief Objeto do dominio `Nome` representando o nome da carteira.
         */
        Nome nome;

        /**
         * @brief Objeto do dominio `TipoPerfil` representando o perfil de investidor da carteira.
         */
        TipoPerfil perfil;

    public:
        /**
         * @brief Metodo publico que define o codigo da carteira.
         * @param codigo Objeto `Codigo` a ser atribuido.
         */
        void setCodigo(Codigo);

        /**
         * @brief Metodo publico que retorna o codigo da carteira.
         * @return Objeto `Codigo` com o valor atribuido.
         */
        Codigo getCodigo() const;

        /**
         * @brief Metodo publico que define o nome da carteira.
         * @param nome Objeto `Nome` a ser atribuido.
         */
        void setNome(Nome);

        /**
         * @brief Metodo publico que retorna o nome da carteira.
         * @return Objeto `Nome` com o valor atribuido.
         */
        Nome getNome() const;

        /**
         * @brief Metodo publico que define o tipo de perfil do investidor associado a carteira.
         * @param perfil Objeto `TipoPerfil` a ser atribuido.
         */
        void setTipoPerfil(TipoPerfil);

        /**
         * @brief Metodo publico que retorna o tipo de perfil associado a carteira.
         * @return Objeto `TipoPerfil` com o valor atribuido.
         */
        TipoPerfil getTipoPerfil() const;
};
//---------------------------------------------------------------------

// Implementacoes inline dos metodos da classe Carteira
//---------------------------------------------------------------------
/**
 * @brief Atribui um objeto `Codigo` ao atributo `codigo`.
 * @param codigo Objeto do dominio `Codigo`.
 */
inline void Carteira::setCodigo(Codigo codigo){ this->codigo = codigo; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o codigo da carteira.
 * @return Objeto `Codigo`.
 */
inline Codigo Carteira::getCodigo() const { return codigo; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o codigo da carteira.
 * @return Objeto `Codigo`.
 */
inline void Carteira::setNome(Nome nome){ this->nome = nome; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o nome da carteira.
 * @return Objeto `Nome`.
 */
inline Nome Carteira::getNome() const { return nome; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o nome da carteira.
 * @return Objeto `Nome`.
 */
inline void Carteira::setTipoPerfil(TipoPerfil perfil){ this->perfil = perfil; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o tipo de perfil da carteira.
 * @return Objeto `TipoPerfil`.
 */
inline TipoPerfil Carteira::getTipoPerfil() const { return perfil; }

///---------------------------------------------------------------------
//Entidade: Ordem   (Responsavel: Karina 231006140)
/**
 * @class Ordem
 * @brief Classe que representa uma instrucao formal enviada por um investidor (ou sistema automatizado)
 * para comprar ou vender ativos financeiros (como acoes, fundos, titulos ou criptomoedas)
 * ma plataforma de negociacao.
 *
 * A classe `Ordem` armazena os dados essenciais para registrar uma ordem realizada por um usuario,
 * como o codigo da ordem, o codigo do ativo negociado, a data, o valor financeiro e a quantidade.
 *
 * Esta entidade e fundamental para o controle de transacoes no sistema, garantindo rastreabilidade
 * e integridade nas operacoes.
 *
 * Atributos:
 * - `codigo`: Codigo identificador unico da ordem (dominio `Codigo`).
 * - `codigoNeg`: Codigo do ativo negociado (dominio `CodigoNeg`).
 * - `data`: Data da realizacao da ordem (dominio `Data`).
 * - `dinheiro`: Valor financeiro da ordem (dominio `Dinheiro`).
 * - `quantidade`: Quantidade de ativos envolvidos (dominio `Quantidade`).
 *
 * Metodos publicos:
 * - `setCodigo(Codigo)`: Define o codigo da ordem.
 * - `getCodigo()`: Retorna o codigo da ordem.
 * - `setCodigoNeg(CodigoNeg)`: Define o codigo do ativo negociado.
 * - `getCodigoNeg()`: Retorna o codigo do ativo negociado.
 * - `setData(Data)`: Define a data da ordem.
 * - `getData()`: Retorna a data da ordem.
 * - `setDinheiro(Dinheiro)`: Define o valor da ordem.
 * - `getDinheiro()`: Retorna o valor da ordem.
 * - `setQuantidade(Quantidade)`: Define a quantidade de ativos.
 * - `getQuantidade()`: Retorna a quantidade de ativos.
 */
class Ordem {
    private:
        /**
         * @brief Objeto do dominio `Codigo` representando o codigo da ordem.
         */
        Codigo codigo;

        /**
         * @brief Objeto do dominio `CodigoNeg` representando o codigo do ativo negociado.
         */
        CodigoNeg codigoNeg;

        /**
         * @brief Objeto do dominio `Data` representando a data da ordem.
         */
        Data data;

        /**
         * @brief Objeto do dominio `Dinheiro` representando o valor financeiro da ordem.
         */
        Dinheiro dinheiro;

        /**
         * @brief Objeto do dominio `Quantidade` representando a quantidade de ativos.
         */
        Quantidade quantidade;

    public:
        /**
         * @brief Metodo publico que define o codigo da ordem.
         * @param codigo Objeto `Codigo` a ser atribuido.
         */
        void setCodigo(Codigo);

         /**
         * @brief Metodo publico que retorna o codigo da ordem.
         * @return Objeto `Codigo`.
         */
        Codigo getCodigo() const;

        /**
         * @brief Metodo publico que define o codigo do ativo negociado.
         * @param codigoNeg Objeto `CodigoNeg` a ser atribuido.
         */
        void setCodigoNeg(CodigoNeg);

        /**
         * @brief Metodo publico que retorna o codigo do ativo negociado.
         * @return Objeto `CodigoNeg`.
         */
        CodigoNeg getCodigoNeg() const;

        /**
         * @brief Metodo publico que define a data da ordem.
         * @param data Objeto `Data` a ser atribuido.
         */
        void setData(Data);

        /**
         * @brief Metodo publico que retorna a data da ordem.
         * @return Objeto `Data`.
         */
        Data getData() const;

        /**
         * @brief Metodo publico que define o valor financeiro da ordem.
         * @param dinheiro Objeto `Dinheiro` a ser atribuido.
         */
        void setDinheiro(Dinheiro);

        /**
         * @brief Metodo publico que retorna o valor financeiro da ordem.
         * @return Objeto `Dinheiro`.
         */
        Dinheiro getDinheiro() const;

         /**
         * @brief Metodo publico que define a quantidade de ativos negociados.
         * @param quantidade Objeto `Quantidade` a ser atribuido.
         */
        void setQuantidade(Quantidade);

        /**
         * @brief Metodo publico que retorna a quantidade de ativos negociados.
         * @return Objeto `Quantidade`.
         */
        Quantidade getQuantidade() const;
};
//---------------------------------------------------------------------

// Implementacoes inline dos metodos da classe Ordem
//---------------------------------------------------------------------
/**
 * @brief Define o codigo da ordem.
 * @param codigo Objeto do dominio Codigo a ser atribuido.
 */
inline void Ordem::setCodigo(Codigo codigo){ this->codigo = codigo; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o codigo da ordem.
 * @return Objeto do dominio Codigo.
 */
inline Codigo Ordem::getCodigo() const { return codigo; }

//---------------------------------------------------------------------
/**
 * @brief Define o codigo do ativo negociado.
 * @param codigoNeg Objeto do dominio CodigoNeg a ser atribuido.
 */
inline void Ordem::setCodigoNeg(CodigoNeg codigoNeg){ this->codigoNeg = codigoNeg; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o codigo do ativo negociado.
 * @return Objeto do dominio CodigoNeg.
 */
inline CodigoNeg Ordem::getCodigoNeg() const { return codigoNeg; }

//---------------------------------------------------------------------
/**
 * @brief Define a data da ordem.
 * @param data Objeto do dominio Data a ser atribuido.
 */
inline void Ordem::setData(Data data){ this->data = data; }

//---------------------------------------------------------------------
/**
 * @brief Retorna a data da ordem.
 * @return Objeto do dominio Data.
 */
inline Data Ordem::getData() const { return data; }

//---------------------------------------------------------------------
/**
 * @brief Define o valor financeiro da ordem.
 * @param dinheiro Objeto do dominio Dinheiro a ser atribuido.
 */
inline void Ordem::setDinheiro(Dinheiro dinheiro){ this->dinheiro = dinheiro; }

//---------------------------------------------------------------------
/**
 * @brief Retorna o valor financeiro da ordem.
 * @return Objeto do dominio Dinheiro.
 */
inline Dinheiro Ordem::getDinheiro() const { return dinheiro; }

//---------------------------------------------------------------------
/**
 * @brief Define a quantidade de ativos da ordem.
 * @param quantidade Objeto do dominio Quantidade a ser atribuido.
 */
inline void Ordem::setQuantidade(Quantidade quantidade){ this->quantidade = quantidade; }

//---------------------------------------------------------------------
/**
 * @brief Retorna a quantidade de ativos da ordem.
 * @return Objeto do dominio Quantidade.
 */
inline Quantidade Ordem::getQuantidade() const { return quantidade; }

///---------------------------------------------------------------------

#endif // ENTIDADES_HPP_INCLUDED
