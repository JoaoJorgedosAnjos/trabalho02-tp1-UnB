#ifndef ORDEMCONTROLLER_HPP_INCLUDED
#define ORDEMCONTROLLER_HPP_INCLUDED

#include "interfaces.hpp"
#include "telaUtils.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <set>

/**
 * @brief Controlador especializado para operações de ordens
 *
 * @details Esta classe é responsável por gerenciar todas as operações
 * relacionadas a ordens de investimento, incluindo criação, listagem,
 * exclusão e validação de dados.
 */
class OrdemController
{
  private:
    IServicoInvestimento *servicoInvestimento;

  public:
    /**
     * @brief Construtor
     *
     * @param servico Ponteiro para o serviço de investimentos
     */
    OrdemController(IServicoInvestimento *servico);

    /**
     * @brief Executa o menu de gerenciamento de ordens
     *
     * @param codigoCarteira Código da carteira selecionada
     */
    void executarMenu(const Codigo &codigoCarteira);

    /**
     * @brief Cria uma nova ordem de investimento
     *
     * @param codigoCarteira Código da carteira onde a ordem será criada
     */
    void criarOrdem(const Codigo &codigoCarteira);

    /**
     * @brief Lista todas as ordens de uma carteira
     *
     * @param codigoCarteira Código da carteira
     */
    void listarOrdens(const Codigo &codigoCarteira);

    /**
     * @brief Exclui uma ordem de investimento
     *
     * @param codigoCarteira Código da carteira
     */
    void excluirOrdem(const Codigo &codigoCarteira);

  private:
    /**
     * @brief Exibe o menu de ordens
     *
     * @param carteiraAtual Carteira atual
     * @param saldoAtual Saldo atual da carteira
     */
    void exibirMenu(const Carteira &carteiraAtual, const Dinheiro &saldoAtual);

    /**
     * @brief Exibe informações da carteira selecionada
     *
     * @param carteiraAtual Carteira atual
     * @param saldoAtual Saldo atual
     */
    void exibirInfoCarteira(const Carteira &carteiraAtual, const Dinheiro &saldoAtual);

    /**
     * @brief Exibe ordens existentes na carteira
     *
     * @param ordensExistentes Lista de ordens existentes
     */
    void exibirOrdensExistentes(const std::list<Ordem> &ordensExistentes);

    /**
     * @brief Exibe instruções para criação de ordem
     */
    void exibirInstrucoesCriacao();

    /**
     * @brief Solicita e valida o código da ordem
     *
     * @param codigoOrdem Código da ordem a ser preenchido
     * @return bool true se válido
     */
    bool solicitarCodigoOrdem(Codigo &codigoOrdem);

    /**
     * @brief Solicita e valida o código de negociação
     *
     * @param codigoNegociacao Código de negociação a ser preenchido
     * @return bool true se válido
     */
    bool solicitarCodigoNegociacao(CodigoNeg &codigoNegociacao);

    /**
     * @brief Solicita e valida a data da ordem
     *
     * @param dataOrdem Data da ordem a ser preenchida
     * @param codigoNegociacao Código de negociação para validação
     * @return bool true se válido
     */
    bool solicitarDataOrdem(Data &dataOrdem, const CodigoNeg &codigoNegociacao);

    /**
     * @brief Solicita e valida a quantidade
     *
     * @param quantidadeOrdem Quantidade a ser preenchida
     * @return bool true se válido
     */
    bool solicitarQuantidade(Quantidade &quantidadeOrdem);

    /**
     * @brief Exibe resumo da ordem antes da confirmação
     *
     * @param codigoOrdem Código da ordem
     * @param codigoNegociacao Código de negociação
     * @param dataOrdem Data da ordem
     * @param quantidadeOrdem Quantidade
     * @param carteiraAtual Carteira atual
     */
    void exibirResumoOrdem(const Codigo &codigoOrdem, const CodigoNeg &codigoNegociacao, const Data &dataOrdem,
                           const Quantidade &quantidadeOrdem, const Carteira &carteiraAtual);

    /**
     * @brief Exibe detalhes da ordem criada
     *
     * @param codigoOrdem Código da ordem criada
     * @param codigoCarteira Código da carteira
     * @param carteiraAtual Carteira atual
     */
    void exibirDetalhesOrdemCriada(const Codigo &codigoOrdem, const Codigo &codigoCarteira,
                                   const Carteira &carteiraAtual);

    /**
     * @brief Valida se a combinação código+data existe no arquivo B3
     *
     * @param codigoNegociacao Código de negociação
     * @param data Data da ordem
     * @return bool true se a combinação existe
     */
    bool validarCombinacaoB3(const CodigoNeg &codigoNegociacao, const std::string &data);

    /**
     * @brief Exibe lista de ordens em formato tabular
     *
     * @param ordensCarteira Lista de ordens
     * @param saldoCarteira Saldo da carteira
     */
    void exibirListaOrdens(const std::list<Ordem> &ordensCarteira, const Dinheiro &saldoCarteira);

    /**
     * @brief Exibe ordens disponíveis para exclusão
     *
     * @param ordensCarteira Lista de ordens
     */
    void exibirOrdensParaExclusao(const std::list<Ordem> &ordensCarteira);

    /**
     * @brief Solicita código da ordem para exclusão
     *
     * @param codigoOrdem Código da ordem a ser excluída
     * @param ordensCarteira Lista de ordens disponíveis
     * @param ordemSelecionada Ordem selecionada (saída)
     * @return bool true se ordem encontrada
     */
    bool solicitarCodigoExclusao(Codigo &codigoOrdem, const std::list<Ordem> &ordensCarteira, Ordem &ordemSelecionada);
};

#endif // ORDEMCONTROLLER_HPP_INCLUDED