#ifndef INPUTVALIDATOR_HPP_INCLUDED
#define INPUTVALIDATOR_HPP_INCLUDED

#include <string>
#include <fstream>
#include <set>
#include "dominios/dominios.hpp"

/**
 * @brief Validador especializado para entradas do usuário
 * 
 * @details Esta classe é responsável por validar e processar
 * entradas do usuário, incluindo validação de dados B3,
 * formatação de códigos e validação de combinações.
 */
class InputValidator {
public:
    /**
     * @brief Valida se uma combinação código+data existe no arquivo B3
     * 
     * @param codigoNegociacao Código de negociação
     * @param data Data da ordem
     * @return bool true se a combinação existe
     */
    static bool validarCombinacaoB3(const CodigoNeg& codigoNegociacao, const std::string& data);

    /**
     * @brief Busca datas disponíveis para um código de negociação
     * 
     * @param codigoNegociacao Código de negociação
     * @param datasDisponiveis Set para armazenar as datas encontradas
     * @return bool true se encontrou datas
     */
    static bool buscarDatasDisponiveis(const CodigoNeg& codigoNegociacao, std::set<std::string>& datasDisponiveis);

    /**
     * @brief Extrai código de negociação de uma linha B3
     * 
     * @param linhaB3 Linha completa do arquivo B3
     * @return std::string Código extraído
     */
    static std::string extrairCodigoB3(const std::string& linhaB3);

    /**
     * @brief Extrai data de uma linha B3
     * 
     * @param linhaB3 Linha completa do arquivo B3
     * @return std::string Data extraída
     */
    static std::string extrairDataB3(const std::string& linhaB3);

    /**
     * @brief Extrai preço de uma linha B3
     * 
     * @param linhaB3 Linha completa do arquivo B3
     * @return std::string Preço extraído
     */
    static std::string extrairPrecoB3(const std::string& linhaB3);

    /**
     * @brief Formata código de negociação para 12 caracteres
     * 
     * @param codigo Código original
     * @return std::string Código formatado
     */
    static std::string formatarCodigoNegociacao(const std::string& codigo);

    /**
     * @brief Remove espaços em branco do final de uma string
     * 
     * @param str String original
     * @return std::string String sem espaços finais
     */
    static std::string removerEspacosFinais(const std::string& str);

    /**
     * @brief Valida se uma string contém apenas dígitos
     * 
     * @param str String a ser validada
     * @return bool true se contém apenas dígitos
     */
    static bool contemApenasDigitos(const std::string& str);

    /**
     * @brief Formata valor monetário com separadores de milhares
     * 
     * @param valor Valor numérico como string
     * @return std::string Valor formatado
     */
    static std::string formatarValorMonetario(const std::string& valor);

private:
    /**
     * @brief Caminho para o arquivo de dados históricos
     */
    static const std::string CAMINHO_ARQUIVO_B3;
};

#endif // INPUTVALIDATOR_HPP_INCLUDED 