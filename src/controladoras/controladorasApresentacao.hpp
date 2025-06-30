#ifndef CONTROLADORASAPRESENTACAO_HPP_INCLUDED
#define CONTROLADORASAPRESENTACAO_HPP_INCLUDED

#include "interfaces.hpp"

// =================================================================================================
/**
 * @class ControladoraApresentacaoAutenticacao
 * @brief Implementação concreta da interface de apresentação para autenticação.
 * @details Responsável por exibir o diálogo de login e capturar os dados do usuário.
 */
class ControladoraApresentacaoAutenticacao : public IApresentacaoAutenticacao {
private:
    IServicoAutenticacao* cntrServicoAutenticacao; // Ponteiro para a camada de serviço

public:
    bool autenticar(Ncpf* cpf) override;
    void setControladoraServico(IServicoAutenticacao* cntrServicoAutenticacao) override;
};

// As outras controladoras de apresentação virão aqui depois...

#endif // CONTROLADORASAPRESENTACAO_HPP_INCLUDED