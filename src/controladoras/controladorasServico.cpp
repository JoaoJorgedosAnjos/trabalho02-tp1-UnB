#include "controladorasServico.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

// =================================================================================================
// FUNÇÕES AUXILIARES PARA CONVERSÃO DE DINHEIRO COM PRECISÃO
// =================================================================================================

/**
 * @brief Converte um objeto Dinheiro (formato brasileiro) para centavos como inteiro longo
 */
long long dinheiroParaCentavos(const Dinheiro& dinheiro) {
    std::string valor = dinheiro.getValor();
    
    // Remove todos os pontos (separadores de milhares)
    valor.erase(std::remove(valor.begin(), valor.end(), '.'), valor.end());
    
    // Localiza a vírgula (separador decimal)
    size_t posVirgula = valor.find(',');
    if (posVirgula == std::string::npos) {
        // Se não há vírgula, assume que são apenas reais inteiros
        try {
            return std::stoll(valor) * 100;
        } catch (const std::exception&) {
            throw std::invalid_argument("Valor monetário inválido para conversão");
        }
    }
    
    // Separa parte inteira e parte decimal
    std::string parteInteira = valor.substr(0, posVirgula);
    std::string parteDecimal = valor.substr(posVirgula + 1);
    
    // Garante que a parte decimal tenha exatamente 2 dígitos
    if (parteDecimal.length() != 2) {
        throw std::invalid_argument("Formato de centavos inválido");
    }
    
    try {
        // Converte para centavos: parte_inteira * 100 + parte_decimal
        long long reais = std::stoll(parteInteira);
        long long centavos = std::stoll(parteDecimal);
        return reais * 100 + centavos;
    } catch (const std::exception&) {
        throw std::invalid_argument("Erro na conversão de valor monetário");
    }
}

/**
 * @brief Converte centavos (inteiro longo) para formato brasileiro de dinheiro
 */
std::string centavosParaDinheiro(long long centavos) {
    // Aplica os limites do domínio Dinheiro
    const long long CENTAVOS_MINIMO = 1;         // 0,01
    const long long CENTAVOS_MAXIMO = 100000000; // 1.000.000,00
    
    if (centavos < CENTAVOS_MINIMO) centavos = CENTAVOS_MINIMO;
    if (centavos > CENTAVOS_MAXIMO) centavos = CENTAVOS_MAXIMO;
    
    // Separa reais e centavos
    long long reais = centavos / 100;
    long long centavosRestantes = centavos % 100;
    
    // Converte a parte dos reais para string
    std::string parteReais = std::to_string(reais);
    
    // Adiciona separadores de milhares (pontos) da direita para esquerda
    if (parteReais.length() > 3) {
        for (int i = parteReais.length() - 3; i > 0; i -= 3) {
            parteReais.insert(i, ".");
        }
    }
    
    // Formata os centavos com 2 dígitos (com zero à esquerda se necessário)
    std::string parteCentavos = (centavosRestantes < 10) ? 
        "0" + std::to_string(centavosRestantes) : 
        std::to_string(centavosRestantes);
    
    // Combina no formato brasileiro: reais,centavos
    return parteReais + "," + parteCentavos;
}

// =================================================================================================
// IMPLEMENTAÇÃO DA CONTROLADORA COM SQLITE
// =================================================================================================

ControladoraServico::ControladoraServico() {
    dbManager = std::make_unique<DatabaseManager>("novo_teste.db");
}

ControladoraServico::~ControladoraServico() {
    // DatabaseManager se auto-desconecta no destrutor
}

bool ControladoraServico::inicializar() {
    if (!dbManager->conectar()) {
        std::cerr << "Erro: Não foi possível conectar ao banco de dados!" << std::endl;
        return false;
    }
    
    if (!dbManager->inicializarBanco()) {
        std::cerr << "Erro: Não foi possível inicializar o banco de dados!" << std::endl;
        return false;
    }
    

    return true;
}

// =================================================================================================
// MÉTODOS DE AUTENTICAÇÃO
// =================================================================================================

bool ControladoraServico::autenticar(const Ncpf& cpf, const Senha& senha) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->autenticarUsuario(cpf, senha);
}

// =================================================================================================
// MÉTODOS DE USUÁRIO
// =================================================================================================

bool ControladoraServico::cadastrarConta(const Conta& conta) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    

    
    // Verifica se a conta já existe
    Conta contaExistente;
    if (dbManager->buscarConta(conta.getNcpf(), &contaExistente)) {
        std::cout << "Erro: Conta com este CPF já existe!" << std::endl;
        return false;
    }
    
    return dbManager->inserirConta(conta);
}

bool ControladoraServico::consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) {
    if (!dbManager->estaConectado() || !conta || !saldo) {
        return false;
    }
    
    // Busca a conta
    if (!dbManager->buscarConta(cpf, conta)) {
        return false;
    }
    
    // Calcula saldo total das carteiras do usuário
    std::list<Carteira> carteiras;
    if (!dbManager->listarCarteiras(cpf, &carteiras)) {
        // Se não tem carteiras, saldo é zero
        try {
            saldo->setValor("0,00");
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    long long saldoTotalCentavos = 0;
    
    for (const auto& carteira : carteiras) {
        Dinheiro saldoCarteira;
        if (dbManager->calcularSaldoCarteira(carteira.getCodigo(), &saldoCarteira)) {
            try {
                long long saldoCarteiraCentavos = dinheiroParaCentavos(saldoCarteira);
                saldoTotalCentavos += saldoCarteiraCentavos;
            } catch (const std::exception& e) {
                // Ignora erros de conversão e continua
            }
        }
    }
    
    try {
        std::string saldoFormatado = centavosParaDinheiro(saldoTotalCentavos);
        saldo->setValor(saldoFormatado);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ControladoraServico::editarConta(const Conta& conta) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->atualizarConta(conta);
}

bool ControladoraServico::excluirConta(const Ncpf& cpf) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirConta(cpf);
}

// =================================================================================================
// MÉTODOS DE INVESTIMENTO
// =================================================================================================

bool ControladoraServico::criarCarteira(const Ncpf& cpf, const Carteira& carteira) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    // Verifica se a conta existe
    Conta conta;
    if (!dbManager->buscarConta(cpf, &conta)) {
        std::cout << "Erro: Conta não encontrada!" << std::endl;
        return false;
    }
    
    // Verifica se já existe carteira com este código
    Carteira carteiraExistente;
    if (dbManager->buscarCarteira(carteira.getCodigo(), &carteiraExistente)) {
        std::cout << "Erro: Já existe uma carteira com este código!" << std::endl;
        return false;
    }
    
    return dbManager->inserirCarteira(carteira, cpf);
}

bool ControladoraServico::listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) {
    if (!dbManager->estaConectado() || !listaCarteiras) {
        return false;
    }
    
    return dbManager->listarCarteiras(cpf, listaCarteiras);
}

bool ControladoraServico::consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) {
    if (!dbManager->estaConectado() || !carteira || !saldo) {
        return false;
    }
    
    // Busca a carteira
    if (!dbManager->buscarCarteira(codigo, carteira)) {
        return false;
    }
    
    // Calcula o saldo da carteira
    return dbManager->calcularSaldoCarteira(codigo, saldo);
}

bool ControladoraServico::editarCarteira(const Carteira& carteira) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->atualizarCarteira(carteira);
}

bool ControladoraServico::excluirCarteira(const Codigo& codigo) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirCarteira(codigo);
}

bool ControladoraServico::criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    // Verifica se a carteira existe
    Carteira carteira;
    if (!dbManager->buscarCarteira(codigoCarteira, &carteira)) {
        std::cout << "Erro: Carteira não encontrada!" << std::endl;
        return false;
    }
    
    // Verifica se já existe ordem com este código
    Ordem ordemExistente;
    if (dbManager->buscarOrdem(ordem.getCodigo(), &ordemExistente)) {
        std::cout << "Erro: Já existe uma ordem com este código!" << std::endl;
        return false;
    }
    
    // 3. Buscar preço histórico no arquivo DADOS_HISTORICOS.txt
    std::ifstream arquivo("DADOS_HISTORICOS.txt");
    if (!arquivo.is_open()) {
        std::cout << "Erro: Não foi possível abrir o arquivo DADOS_HISTORICOS.txt!" << std::endl;
        return false; // Não foi possível abrir o arquivo de dados históricos
    }
    
    std::string linha;
    std::string precoMedio;
    bool encontrouDados = false;
    
    // Preparar strings para comparação (remover espaços em branco)
    std::string codigoNegBusca = ordem.getCodigoNeg().getValor();
    std::string dataBusca = ordem.getData().getValor();
    
    // Remove espaços do código de negociação para comparação
    size_t posFimCodigo = codigoNegBusca.find_last_not_of(' ');
    if (posFimCodigo != std::string::npos) {
        codigoNegBusca = codigoNegBusca.substr(0, posFimCodigo + 1);
    }
    
    while (std::getline(arquivo, linha)) {
        // Ignora linhas de comentário
        if (linha.empty() || linha[0] == '#') {
            continue;
        }
        
        // Separa os campos usando '|' como delimitador
        std::vector<std::string> campos;
        std::stringstream ss(linha);
        std::string campo;
        
        while (std::getline(ss, campo, '|')) {
            campos.push_back(campo);
        }
        
        // Verifica se a linha tem o formato correto (8 campos)
        if (campos.size() != 8) {
            continue;
        }
        
        // Remove espaços em branco dos campos
        std::string codigoNegArquivo = campos[0];
        size_t posFimCodigoArq = codigoNegArquivo.find_last_not_of(' ');
        if (posFimCodigoArq != std::string::npos) {
            codigoNegArquivo = codigoNegArquivo.substr(0, posFimCodigoArq + 1);
        }
        
        std::string dataArquivo = campos[1];
        
        // Verifica se encontrou o papel e a data
        if (codigoNegArquivo == codigoNegBusca && dataArquivo == dataBusca) {
            precoMedio = campos[6]; // PrecoMedio está na posição 6
            encontrouDados = true;
            break;
        }
    }
    
    arquivo.close();
    
    if (!encontrouDados) {
        std::cout << "Erro: Não foram encontrados dados históricos para " << codigoNegBusca << " na data " << dataBusca << std::endl;
        return false; // Não encontrou dados históricos para o papel na data especificada
    }
    
    // 4. Calcular o preço da ordem (preço médio × quantidade)
    try {
        // Converter preço médio para centavos
        Dinheiro precoMedioObj;
        precoMedioObj.setValor(precoMedio);
        long long precoMedioCentavos = dinheiroParaCentavos(precoMedioObj);
        
        // Converter quantidade para número
        std::string quantidadeStr = ordem.getQuantidade().getValor();
        
        // Remove pontos da quantidade (formato brasileiro: 1.000 = 1000)
        std::string quantidadeLimpa;
        for (char c : quantidadeStr) {
            if (c != '.') {
                quantidadeLimpa += c;
            }
        }
        
        long long quantidade = std::stoll(quantidadeLimpa);
        
        // Calcula preço final da ordem
        long long precoFinalCentavos = precoMedioCentavos * quantidade;
        
        // Converte de volta para formato brasileiro
        std::string precoFinalFormatado = centavosParaDinheiro(precoFinalCentavos);
        
        // 5. Criar nova ordem com o preço calculado
        Ordem novaOrdem = ordem; // Copia todos os dados
        Dinheiro precoFinal;
        precoFinal.setValor(precoFinalFormatado);
        novaOrdem.setDinheiro(precoFinal);
        
        // 6. Salvar a ordem no banco de dados
        return dbManager->inserirOrdem(novaOrdem, codigoCarteira);
        
    } catch (const std::exception& e) {
        std::cout << "Erro no cálculo do preço: " << e.what() << std::endl;
        return false; // Erro no cálculo ou conversão
    }
}

bool ControladoraServico::listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) {
    if (!dbManager->estaConectado() || !listaOrdens) {
        return false;
    }
    
    return dbManager->listarOrdens(codigoCarteira, listaOrdens);
}

bool ControladoraServico::excluirOrdem(const Codigo& codigo) {
    if (!dbManager->estaConectado()) {
        return false;
    }
    
    return dbManager->excluirOrdem(codigo);
}
