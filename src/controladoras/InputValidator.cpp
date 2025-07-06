#include "InputValidator.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

const std::string InputValidator::CAMINHO_ARQUIVO_B3 = "../data/DADOS_HISTORICOS.txt";

bool InputValidator::validarCombinacaoB3(const CodigoNeg& codigoNegociacao, const std::string& data) {
    std::ifstream arquivoB3(CAMINHO_ARQUIVO_B3);
    if (!arquivoB3.is_open()) {
        return false;
    }
    
    std::string codigoLimpo = removerEspacosFinais(codigoNegociacao.getValor());
    std::string linhaB3;
    
    while (std::getline(arquivoB3, linhaB3)) {
        if (linhaB3.empty() || linhaB3[0] == '#') {
            continue;
        }
        
        if (linhaB3.length() >= 24) {
            std::string codigoLinha = extrairCodigoB3(linhaB3);
            std::string dataLinha = extrairDataB3(linhaB3);
            
            if (codigoLinha == codigoLimpo && dataLinha == data) {
                arquivoB3.close();
                return true;
            }
        }
    }
    
    arquivoB3.close();
    return false;
}

bool InputValidator::buscarDatasDisponiveis(const CodigoNeg& codigoNegociacao, std::set<std::string>& datasDisponiveis) {
    std::ifstream arquivoB3(CAMINHO_ARQUIVO_B3);
    if (!arquivoB3.is_open()) {
        return false;
    }
    
    std::string codigoLimpo = removerEspacosFinais(codigoNegociacao.getValor());
    std::string linhaB3;
    
    while (std::getline(arquivoB3, linhaB3)) {
        if (linhaB3.empty() || linhaB3[0] == '#') {
            continue;
        }
        
        if (linhaB3.length() >= 24) {
            std::string codigoLinha = extrairCodigoB3(linhaB3);
            
            if (codigoLinha == codigoLimpo) {
                std::string dataLinha = extrairDataB3(linhaB3);
                datasDisponiveis.insert(dataLinha);
            }
        }
    }
    
    arquivoB3.close();
    return !datasDisponiveis.empty();
}

std::string InputValidator::extrairCodigoB3(const std::string& linhaB3) {
    if (linhaB3.length() >= 24) {
        std::string codigo = linhaB3.substr(12, 12);
        return removerEspacosFinais(codigo);
    }
    return "";
}

std::string InputValidator::extrairDataB3(const std::string& linhaB3) {
    if (linhaB3.length() >= 10) {
        return linhaB3.substr(2, 8);
    }
    return "";
}

std::string InputValidator::extrairPrecoB3(const std::string& linhaB3) {
    if (linhaB3.length() >= 24) {
        std::string preco = linhaB3.substr(24, 10);
        return removerEspacosFinais(preco);
    }
    return "";
}

std::string InputValidator::formatarCodigoNegociacao(const std::string& codigo) {
    std::string codigoCompleto = codigo;
    while (codigoCompleto.length() < 12) {
        codigoCompleto += " ";
    }
    return codigoCompleto;
}

std::string InputValidator::removerEspacosFinais(const std::string& str) {
    size_t posFim = str.find_last_not_of(' ');
    if (posFim != std::string::npos) {
        return str.substr(0, posFim + 1);
    }
    return str;
}

bool InputValidator::contemApenasDigitos(const std::string& str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string InputValidator::formatarValorMonetario(const std::string& valor) {
    // Remove pontos e vírgulas existentes
    std::string valorLimpo;
    for (char c : valor) {
        if (std::isdigit(c)) {
            valorLimpo += c;
        }
    }
    
    if (valorLimpo.empty()) {
        return "0,00";
    }
    
    // Garante que tenha pelo menos 3 dígitos (centavos)
    while (valorLimpo.length() < 3) {
        valorLimpo = "0" + valorLimpo;
    }
    
    // Separa reais de centavos
    std::string reais = valorLimpo.substr(0, valorLimpo.length() - 2);
    std::string centavos = valorLimpo.substr(valorLimpo.length() - 2);
    
    if (reais.empty()) {
        reais = "0";
    }
    
    // Adiciona separadores de milhares
    std::string reaisFormatado;
    for (int i = reais.length() - 1, j = 0; i >= 0; i--, j++) {
        if (j > 0 && j % 3 == 0) {
            reaisFormatado = "." + reaisFormatado;
        }
        reaisFormatado = reais[i] + reaisFormatado;
    }
    
    return reaisFormatado + "," + centavos;
} 