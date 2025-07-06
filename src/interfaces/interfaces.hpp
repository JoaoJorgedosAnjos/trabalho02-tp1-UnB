#ifndef INTERFACES_H_INCLUDED
#define INTERFACES_H_INCLUDED

#include "dominios.hpp"
#include "entidades.hpp"

#include <list>
#include <string>

class IServicoAutenticacao;
class IServicoUsuario;
class IServicoInvestimento;

/**
 * @brief Interface para os serviços de autenticação na camada de apresentação.
 */
class IApresentacaoAutenticacao {
public:
    virtual bool autenticar(Ncpf* cpf) = 0;
    virtual void setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) = 0;
    virtual ~IApresentacaoAutenticacao() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de usuário na camada de apresentação.
 */
class IApresentacaoUsuario {
public:
    virtual bool executar(const Ncpf& cpf) = 0;
    virtual void cadastrar() = 0;
    virtual void setControladoraServico(IServicoUsuario* cntrServicoUsuario) = 0;
    virtual ~IApresentacaoUsuario() {}
};


/**
 * @brief Interface para os serviços de gerenciamento de investimentos na camada de apresentação.
 */
class IApresentacaoInvestimento {
public:
    virtual void executar(const Ncpf& cpf) = 0;
    virtual void setControladoraServico(IServicoInvestimento* cntrServicoInvestimento) = 0;
    virtual ~IApresentacaoInvestimento() {}
};

/**
 * @brief Interface para os serviços de autenticação providos pela camada de serviço.
 */
class IServicoAutenticacao {
public:
    virtual bool autenticar(const Ncpf& cpf, const Senha& senha) = 0;
    virtual ~IServicoAutenticacao() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de usuário providos pela camada de serviço.
 */
class IServicoUsuario {
public:
    virtual bool cadastrarConta(const Conta& conta) = 0;
    virtual bool consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) = 0;
    virtual bool editarConta(const Conta& conta) = 0;
    virtual bool excluirConta(const Ncpf& cpf) = 0;
    virtual ~IServicoUsuario() {}
};

/**
 * @brief Interface para os serviços de gerenciamento de investimentos providos pela camada de serviço.
 */
class IServicoInvestimento {
public:
    virtual bool criarCarteira(const Ncpf& cpf, const Carteira& carteira) = 0;
    virtual bool listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) = 0;
    virtual bool consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) = 0;
    virtual bool editarCarteira(const Carteira& carteira) = 0;
    virtual bool excluirCarteira(const Codigo& codigo) = 0;
    virtual bool criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) = 0;
    virtual bool listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) = 0;
    virtual bool excluirOrdem(const Codigo& codigo) = 0;
    virtual ~IServicoInvestimento() {}
};


#endif // INTERFACES_H_INCLUDED