#include "InputValidator.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

const std::string InputValidator::CAMINHO_ARQUIVO_B3 = "../data/DADOS_HISTORICOS.txt";

/**
 * @brief Valida se uma combinação de código de negociação e data existe nos dados históricos da B3
 * @param codigoNegociacao Código de negociação a ser validado
 * @param data Data no formato AAAAMMDD a ser validada
 * @return true se a combinação existe nos dados históricos, false caso contrário
 * @details Abre o arquivo de dados históricos da B3 e verifica se existe uma entrada
 *          correspondente ao código de negociação e data fornecidos. Ignora linhas
 *          vazias e comentários iniciados com '#'.
 * @see CAMINHO_ARQUIVO_B3
 * @see extrairCodigoB3()
 * @see extrairDataB3()
 */
bool InputValidator::validarCombinacaoB3(const CodigoNeg &codigoNegociacao, const std::string &data)
{
    std::ifstream arquivoB3(CAMINHO_ARQUIVO_B3);
    if (!arquivoB3.is_open())
    {
        return false;
    }

    std::string codigoLimpo = removerEspacosFinais(codigoNegociacao.getValor());
    std::string linhaB3;

    while (std::getline(arquivoB3, linhaB3))
    {
        if (linhaB3.empty() || linhaB3[0] == '#')
        {
            continue;
        }

        if (linhaB3.length() >= 24)
        {
            std::string codigoLinha = extrairCodigoB3(linhaB3);
            std::string dataLinha = extrairDataB3(linhaB3);

            if (codigoLinha == codigoLimpo && dataLinha == data)
            {
                arquivoB3.close();
                return true;
            }
        }
    }

    arquivoB3.close();
    return false;
}

/**
 * @brief Busca todas as datas disponíveis para um código de negociação específico
 * @param codigoNegociacao Código de negociação para buscar datas
 * @param datasDisponiveis Conjunto onde serão armazenadas as datas encontradas
 * @return true se encontrou pelo menos uma data, false caso contrário
 * @details Percorre o arquivo de dados históricos da B3 e coleta todas as datas
 *          disponíveis para o código de negociação fornecido. As datas são
 *          armazenadas em um conjunto para evitar duplicatas.
 * @see CAMINHO_ARQUIVO_B3
 * @see extrairCodigoB3()
 * @see extrairDataB3()
 */
bool InputValidator::buscarDatasDisponiveis(const CodigoNeg &codigoNegociacao, std::set<std::string> &datasDisponiveis)
{
    std::ifstream arquivoB3(CAMINHO_ARQUIVO_B3);
    if (!arquivoB3.is_open())
    {
        return false;
    }

    std::string codigoLimpo = removerEspacosFinais(codigoNegociacao.getValor());
    std::string linhaB3;

    while (std::getline(arquivoB3, linhaB3))
    {
        if (linhaB3.empty() || linhaB3[0] == '#')
        {
            continue;
        }

        if (linhaB3.length() >= 24)
        {
            std::string codigoLinha = extrairCodigoB3(linhaB3);

            if (codigoLinha == codigoLimpo)
            {
                std::string dataLinha = extrairDataB3(linhaB3);
                datasDisponiveis.insert(dataLinha);
            }
        }
    }

    arquivoB3.close();
    return !datasDisponiveis.empty();
}

/**
 * @brief Extrai o código de negociação de uma linha do arquivo B3
 * @param linhaB3 Linha completa do arquivo de dados históricos
 * @return Código de negociação extraído e limpo, ou string vazia se inválido
 * @details Extrai o código de negociação da posição 12-23 da linha (12 caracteres)
 *          e remove espaços em branco no final. Valida se a linha tem tamanho suficiente.
 * @see removerEspacosFinais()
 */
std::string InputValidator::extrairCodigoB3(const std::string &linhaB3)
{
    if (linhaB3.length() >= 24)
    {
        std::string codigo = linhaB3.substr(12, 12);
        return removerEspacosFinais(codigo);
    }
    return "";
}

/**
 * @brief Extrai a data de uma linha do arquivo B3
 * @param linhaB3 Linha completa do arquivo de dados históricos
 * @return Data extraída no formato AAAAMMDD, ou string vazia se inválido
 * @details Extrai a data da posição 2-9 da linha (8 caracteres) no formato AAAAMMDD.
 *          Valida se a linha tem tamanho suficiente antes da extração.
 */
std::string InputValidator::extrairDataB3(const std::string &linhaB3)
{
    if (linhaB3.length() >= 10)
    {
        return linhaB3.substr(2, 8);
    }
    return "";
}

/**
 * @brief Extrai o preço de uma linha do arquivo B3
 * @param linhaB3 Linha completa do arquivo de dados históricos
 * @return Preço extraído e limpo, ou string vazia se inválido
 * @details Extrai o preço da posição 24-33 da linha (10 caracteres) e remove
 *          espaços em branco no final. Valida se a linha tem tamanho suficiente.
 * @see removerEspacosFinais()
 */
std::string InputValidator::extrairPrecoB3(const std::string &linhaB3)
{
    if (linhaB3.length() >= 24)
    {
        std::string preco = linhaB3.substr(24, 10);
        return removerEspacosFinais(preco);
    }
    return "";
}

/**
 * @brief Formata um código de negociação para ter exatamente 12 caracteres
 * @param codigo Código de negociação a ser formatado
 * @return Código formatado com 12 caracteres, preenchido com espaços se necessário
 * @details Garante que o código de negociação tenha exatamente 12 caracteres,
 *          preenchendo com espaços em branco no final se necessário. Usado para
 *          padronizar códigos antes de comparações.
 */
std::string InputValidator::formatarCodigoNegociacao(const std::string &codigo)
{
    std::string codigoCompleto = codigo;
    while (codigoCompleto.length() < 12)
    {
        codigoCompleto += " ";
    }
    return codigoCompleto;
}

/**
 * @brief Remove espaços em branco no final de uma string
 * @param str String a ser processada
 * @return String sem espaços em branco no final
 * @details Função utilitária que remove todos os espaços em branco no final
 *          de uma string, preservando espaços no início e no meio. Usado para
 *          limpar dados extraídos do arquivo B3.
 */
std::string InputValidator::removerEspacosFinais(const std::string &str)
{
    size_t posFim = str.find_last_not_of(' ');
    if (posFim != std::string::npos)
    {
        return str.substr(0, posFim + 1);
    }
    return str;
}

/**
 * @brief Verifica se uma string contém apenas dígitos numéricos
 * @param str String a ser verificada
 * @return true se contém apenas dígitos, false caso contrário
 * @details Utiliza std::all_of com ::isdigit para verificar se todos os caracteres
 *          da string são dígitos numéricos (0-9). Útil para validação de entrada
 *          de dados numéricos.
 */
bool InputValidator::contemApenasDigitos(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

/**
 * @brief Formata um valor monetário para o padrão brasileiro
 * @param valor String contendo valor monetário a ser formatado
 * @return Valor formatado no padrão brasileiro (ex: "1.234,56")
 * @details Processa uma string removendo caracteres não numéricos, garante pelo
 *          menos 3 dígitos (para centavos), separa reais e centavos, e formata
 *          com pontos para milhares e vírgula para decimais. Retorna "0,00" se
 *          o valor estiver vazio.
 */
std::string InputValidator::formatarValorMonetario(const std::string &valor)
{
    std::string valorLimpo;
    for (char c : valor)
    {
        if (std::isdigit(c))
        {
            valorLimpo += c;
        }
    }

    if (valorLimpo.empty())
    {
        return "0,00";
    }

    while (valorLimpo.length() < 3)
    {
        valorLimpo = "0" + valorLimpo;
    }

    std::string reais = valorLimpo.substr(0, valorLimpo.length() - 2);
    std::string centavos = valorLimpo.substr(valorLimpo.length() - 2);

    if (reais.empty())
    {
        reais = "0";
    }

    std::string reaisFormatado;
    for (int i = reais.length() - 1, j = 0; i >= 0; i--, j++)
    {
        if (j > 0 && j % 3 == 0)
        {
            reaisFormatado = "." + reaisFormatado;
        }
        reaisFormatado = reais[i] + reaisFormatado;
    }

    return reaisFormatado + "," + centavos;
}