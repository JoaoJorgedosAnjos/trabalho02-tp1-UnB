#ifndef CARTEIRACONTROLLER_HPP_INCLUDED
#define CARTEIRACONTROLLER_HPP_INCLUDED

#include "interfaces.hpp"
#include "telaUtils.hpp"
#include <iomanip>
#include <iostream>
#include <list>

/**
 * @brief Controlador especializado para operações de carteiras
 *
 * @details Esta classe é responsável por gerenciar todas as operações
 * relacionadas a carteiras de investimento, incluindo criação, edição,
 * exclusão e consulta.
 */
class CarteiraController
{
  private:
    IServicoInvestimento *servicoInvestimento;

  public:
    /**
     * @brief Construtor
     *
     * @param servico Ponteiro para o serviço de investimentos
     */
    CarteiraController(IServicoInvestimento *servico);

    /**
     * @brief Executa o menu de gerenciamento de carteiras
     *
     * @param cpf CPF do usuário autenticado
     */
    void executarMenu(const Ncpf &cpf);

    /**
     * @brief Cria uma nova carteira
     *
     * @param cpf CPF do usuário autenticado
     */
    void criarCarteira(const Ncpf &cpf);

    /**
     * @brief Lista todas as carteiras do usuário
     *
     * @param cpf CPF do usuário autenticado
     */
    void listarCarteiras(const Ncpf &cpf);

    /**
     * @brief Edita uma carteira existente
     *
     * @param cpf CPF do usuário autenticado
     */
    void editarCarteira(const Ncpf &cpf);

    /**
     * @brief Exclui uma carteira
     *
     * @param cpf CPF do usuário autenticado
     */
    void excluirCarteira(const Ncpf &cpf);

  private:
    /**
     * @brief Exibe o menu de carteiras
     */
    void exibirMenu();

    /**
     * @brief Exibe a lista de carteiras em formato tabular
     *
     * @param carteiras Lista de carteiras a ser exibida
     */
    void exibirListaCarteiras(const std::list<Carteira> &carteiras);

    /**
     * @brief Exibe detalhes de uma carteira específica
     *
     * @param carteira Carteira a ser exibida
     * @param saldo Saldo da carteira
     */
    void exibirDetalhesCarteira(const Carteira &carteira, const Dinheiro &saldo);

    /**
     * @brief Edita uma carteira específica
     *
     * @param cpf CPF do usuário
     * @param carteiraAtual Carteira a ser editada
     */
    void editarCarteiraEspecifica(const Ncpf &cpf, const Carteira &carteiraAtual);

    /**
     * @brief Exclui uma carteira específica
     *
     * @param cpf CPF do usuário
     * @param carteiraAtual Carteira a ser excluída
     * @return bool true se excluída com sucesso
     */
    bool excluirCarteiraEspecifica(const Ncpf &cpf, const Carteira &carteiraAtual);
};

#endif // CARTEIRACONTROLLER_HPP_INCLUDED