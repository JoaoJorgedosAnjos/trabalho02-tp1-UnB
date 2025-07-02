#include "controladorasServico.hpp"
#include <string>
#include <iostream>
#include <algorithm>

// =================================================================================================
// FUNÇÕES AUXILIARES PARA CONVERSÃO DE DINHEIRO COM PRECISÃO
// =================================================================================================

/**
 * @brief Converte um objeto Dinheiro (formato brasileiro) para centavos como inteiro longo
 * 
 * @details Esta função implementa uma abordagem precisa para cálculos monetários,
 * convertendo valores em formato brasileiro (#.###.###,##) para centavos representados
 * como inteiros longos. Por exemplo:
 * - "1.500,50" se torna 150050 centavos
 * - "10,25" se torna 1025 centavos
 * - "1.000.000,00" se torna 100000000 centavos
 * 
 * Esta abordagem elimina problemas de precisão de ponto flutuante comuns em
 * operações financeiras, garantindo que operações como (0.1 + 0.2) não resultem
 * em valores como 0.30000000000000004.
 * 
 * @param dinheiro Objeto do domínio Dinheiro no formato brasileiro
 * @return long long Valor em centavos (exemplo: R$ 10,50 = 1050 centavos)
 * @throws std::invalid_argument se não conseguir converter o valor
 * 
 * @note Esta função é thread-safe e não modifica o objeto original
 * @see centavosParaDinheiro() para conversão inversa
 * 
 * @example
 * @code
 * Dinheiro valor;
 * valor.setValor("1.234,56");
 * long long centavos = dinheiroParaCentavos(valor); // Resultado: 123456
 * @endcode
 */
long long dinheiroParaCentavos(const Dinheiro& dinheiro) {
    std::string valor = dinheiro.getValor();
    
    // Remove todos os pontos (separadores de milhares)
    // Exemplo: "1.234.567,89" -> "1234567,89"
    valor.erase(std::remove(valor.begin(), valor.end(), '.'), valor.end());
    
    // Localiza a vírgula (separador decimal)
    size_t posVirgula = valor.find(',');
    if (posVirgula == std::string::npos) {
        // Se não há vírgula, assume que são apenas reais inteiros
        // Exemplo: "1000" -> 100000 centavos
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
        // Exemplo: "1234,56" -> 1234 * 100 + 56 = 123456 centavos
        long long reais = std::stoll(parteInteira);
        long long centavos = std::stoll(parteDecimal);
        return reais * 100 + centavos;
    } catch (const std::exception&) {
        throw std::invalid_argument("Erro na conversão de valor monetário");
    }
}

/**
 * @brief Converte centavos (inteiro longo) para formato brasileiro de dinheiro
 * 
 * @details Esta função converte valores em centavos de volta para o formato brasileiro
 * aceito pelo domínio Dinheiro. Ela aplica formatação com separadores de milhares
 * (pontos) e separador decimal (vírgula), garantindo que o resultado seja válido
 * para o domínio Dinheiro.
 * 
 * A função também aplica os limites do domínio Dinheiro automaticamente:
 * - Mínimo: 1 centavo (0,01)
 * - Máximo: 100.000.000 centavos (1.000.000,00)
 * 
 * @param centavos Valor em centavos como inteiro longo
 * @return std::string Valor formatado no padrão brasileiro (#.###.###,##)
 * 
 * @note A função garante que o resultado sempre seja válido para o domínio Dinheiro
 * @see dinheiroParaCentavos() para conversão inversa
 * 
 * @example
 * @code
 * long long centavos = 123456;
 * std::string formatado = centavosParaDinheiro(centavos); // Resultado: "1.234,56"
 * @endcode
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
    // Exemplo: "1234567" -> "1.234.567"
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

// Construtor: Vamos criar um usuário de teste com carteiras e ordens para demonstração
ControladoraServico::ControladoraServico() {
    // =================================================================================================
    // CRIAR CONTA DE TESTE
    // =================================================================================================
    Conta contaTeste;
    Ncpf cpfTeste;
    Nome nomeTeste;
    Senha senhaTeste;

    // Define dados válidos para o usuário de teste
    cpfTeste.setValor("043.488.481-22");
    nomeTeste.setValor("Usuario Teste");
    senhaTeste.setValor("Test1#");
    
    contaTeste.setNcpf(cpfTeste);
    contaTeste.setNome(nomeTeste);
    contaTeste.setSenha(senhaTeste);

    // Adiciona a conta de teste ao nosso "banco de dados" em memória
    contas[cpfTeste.getValor()] = contaTeste;

    // =================================================================================================
    // CRIAR CARTEIRAS DE TESTE
    // =================================================================================================
    
    // Carteira 1: Conservadora
    Carteira carteira1;
    Codigo codigo1;
    Nome nomeCarteira1;
    TipoPerfil perfil1;
    
    codigo1.setValor("10001");
    nomeCarteira1.setValor("Carteira Conserv");
    perfil1.setValor("Conservador");
    
    carteira1.setCodigo(codigo1);
    carteira1.setNome(nomeCarteira1);
    carteira1.setTipoPerfil(perfil1);
    
    carteiras[codigo1.getValor()] = carteira1;
    
    // Carteira 2: Agressiva
    Carteira carteira2;
    Codigo codigo2;
    Nome nomeCarteira2;
    TipoPerfil perfil2;
    
    codigo2.setValor("10002");
    nomeCarteira2.setValor("Carteira Agressiva");
    perfil2.setValor("Agressivo");
    
    carteira2.setCodigo(codigo2);
    carteira2.setNome(nomeCarteira2);
    carteira2.setTipoPerfil(perfil2);
    
    carteiras[codigo2.getValor()] = carteira2;
    
    // Associa as carteiras à conta de teste
    std::list<Codigo> carteirasUsuario;
    carteirasUsuario.push_back(codigo1);
    carteirasUsuario.push_back(codigo2);
    mapaContaCarteiras[cpfTeste.getValor()] = carteirasUsuario;

    // =================================================================================================
    // CRIAR ORDENS DE TESTE
    // =================================================================================================
    
    // Ordem 1 - Carteira Conservadora
    Ordem ordem1;
    Codigo codigoOrdem1;
    CodigoNeg codigoNeg1;
    Data data1;
    Dinheiro dinheiro1;
    Quantidade quantidade1;
    
    codigoOrdem1.setValor("20001");
    codigoNeg1.setValor("PETR4       "); // 12 caracteres
    data1.setValor("20240315");
    dinheiro1.setValor("1.500,50");
    quantidade1.setValor("100");
    
    ordem1.setCodigo(codigoOrdem1);
    ordem1.setCodigoNeg(codigoNeg1);
    ordem1.setData(data1);
    ordem1.setDinheiro(dinheiro1);
    ordem1.setQuantidade(quantidade1);
    
    ordens[codigoOrdem1.getValor()] = ordem1;
    
    // Ordem 2 - Carteira Conservadora
    Ordem ordem2;
    Codigo codigoOrdem2;
    CodigoNeg codigoNeg2;
    Data data2;
    Dinheiro dinheiro2;
    Quantidade quantidade2;
    
    codigoOrdem2.setValor("20002");
    codigoNeg2.setValor("VALE3       ");
    data2.setValor("20240320");
    dinheiro2.setValor("2.300,75");
    quantidade2.setValor("150");
    
    ordem2.setCodigo(codigoOrdem2);
    ordem2.setCodigoNeg(codigoNeg2);
    ordem2.setData(data2);
    ordem2.setDinheiro(dinheiro2);
    ordem2.setQuantidade(quantidade2);
    
    ordens[codigoOrdem2.getValor()] = ordem2;
    
    // Ordem 3 - Carteira Agressiva
    Ordem ordem3;
    Codigo codigoOrdem3;
    CodigoNeg codigoNeg3;
    Data data3;
    Dinheiro dinheiro3;
    Quantidade quantidade3;
    
    codigoOrdem3.setValor("20003");
    codigoNeg3.setValor("MGLU3       ");
    data3.setValor("20240325");
    dinheiro3.setValor("850,25");
    quantidade3.setValor("200");
    
    ordem3.setCodigo(codigoOrdem3);
    ordem3.setCodigoNeg(codigoNeg3);
    ordem3.setData(data3);
    ordem3.setDinheiro(dinheiro3);
    ordem3.setQuantidade(quantidade3);
    
    ordens[codigoOrdem3.getValor()] = ordem3;
    
    // Associa as ordens às carteiras
    std::list<Codigo> ordensCarteira1;
    ordensCarteira1.push_back(codigoOrdem1);
    ordensCarteira1.push_back(codigoOrdem2);
    mapaCarteiraOrdens[codigo1.getValor()] = ordensCarteira1;
    
    std::list<Codigo> ordensCarteira2;
    ordensCarteira2.push_back(codigoOrdem3);
    mapaCarteiraOrdens[codigo2.getValor()] = ordensCarteira2;
}


// Implementação do método de autenticação
bool ControladoraServico::autenticar(const Ncpf& cpf, const Senha& senha) {
    // Procura a conta no map usando o CPF como chave
    auto it = contas.find(cpf.getValor());

    // Se o iterador for igual a 'end', significa que a conta não foi encontrada.
    if (it == contas.end()) {
        return false; // Usuário não existe.
    }

    // Se encontrou, 'it->second' é o objeto Conta.
    // Compara a senha fornecida com a senha armazenada na conta.
    if (it->second.getSenha().getValor() == senha.getValor()) {
        return true; // Senha correta. Autenticação bem-sucedida.
    }

    return false; // Senha incorreta.
}


// =================================================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE GERENCIAMENTO DE CONTA
// =================================================================================================

bool ControladoraServico::cadastrarConta(const Conta& conta) {
    // Verifica se já existe uma conta com o mesmo CPF
    std::map<std::string, Conta>::iterator it = contas.find(conta.getNcpf().getValor());
    // auto it = contas.find(conta.getNcpf().getValor());  // Versão com auto para comparação
    if (it != contas.end()) {
        return false; // CPF já existe
    }
    
    // Adiciona a nova conta ao map
    contas[conta.getNcpf().getValor()] = conta;
    return true;
}

/**
 * @brief Consulta uma conta específica e calcula seu saldo total consolidado
 * 
 * @details Este método implementa a regra de negócio principal do sistema:
 * "O saldo de uma conta é a soma dos saldos de todas as carteiras associadas a ela".
 * 
 * Este é o método de mais alto nível para cálculo de saldos, agregando valores
 * de múltiplas carteiras que, por sua vez, agregam valores de múltiplas ordens.
 * A hierarquia é: Conta -> Carteiras -> Ordens
 * 
 * O cálculo utiliza aritmética de centavos para garantir precisão absoluta:
 * 1. Para cada carteira da conta, obtém o saldo em centavos
 * 2. Soma todos os saldos das carteiras em centavos
 * 3. Converte o total final para formato brasileiro
 * 
 * @param cpf CPF do titular da conta (chave de busca)
 * @param conta Ponteiro para objeto Conta onde serão armazenados os dados
 * @param saldo Ponteiro para objeto Dinheiro onde será armazenado o saldo consolidado
 * @return true se a conta foi encontrada e processada com sucesso, false caso contrário
 * 
 * @note Este método é essencial para a funcionalidade "Consultar meus dados" da interface
 * @see consultarCarteira() para cálculo individual de carteiras
 * 
 * @warning Se alguma carteira tiver erro no cálculo, ela é ignorada no total consolidado
 * 
 * @example
 * @code
 * Ncpf cpfUsuario;
 * cpfUsuario.setValor("123.456.789-00");
 * Conta dadosConta;
 * Dinheiro saldoTotal;
 * 
 * if (servico.consultarConta(cpfUsuario, &dadosConta, &saldoTotal)) {
 *     std::cout << "Saldo total: R$ " << saldoTotal.getValor() << std::endl;
 * }
 * @endcode
 */
bool ControladoraServico::consultarConta(const Ncpf& cpf, Conta* conta, Dinheiro* saldo) {
    // Localiza a conta pelo CPF no mapa de contas
    std::map<std::string, Conta>::iterator it = contas.find(cpf.getValor());
    // auto it = contas.find(cpf.getValor());  // Versão com auto para comparação
    if (it == contas.end()) {
        return false; // Conta não encontrada
    }
    
    // Retorna os dados da conta encontrada
    *conta = it->second;
    
    // Inicializa acumulador do saldo total consolidado em centavos
    long long saldoTotalCentavos = 0;
    
    // Verifica se o usuário possui carteiras associadas
    std::map<std::string, std::list<Codigo>>::iterator itCarteiras = mapaContaCarteiras.find(cpf.getValor());
    // auto itCarteiras = mapaContaCarteiras.find(cpf.getValor());  // Versão com auto para comparação
    if (itCarteiras != mapaContaCarteiras.end()) {
        // Itera por todas as carteiras do usuário
        for (const Codigo& codigoCarteira : itCarteiras->second) {
        // for (const auto& codigoCarteira : itCarteiras->second) {  // Versão com auto para comparação
            Carteira dadosCarteira;
            Dinheiro saldoCarteira;
            
            // Consulta o saldo individual desta carteira
            if (consultarCarteira(codigoCarteira, &dadosCarteira, &saldoCarteira)) {
                try {
                    // Converte o saldo da carteira para centavos e soma ao total
                    // Exemplo: Se carteira tem "1.500,50", soma 150050 centavos
                    long long valorCarteiraCentavos = dinheiroParaCentavos(saldoCarteira);
                    saldoTotalCentavos += valorCarteiraCentavos;
                } catch (const std::exception& e) {
                    // Se há erro na conversão desta carteira, a ignora e continua
                    // (estratégia defensiva para manter o sistema funcionando)
                    std::cerr << "Aviso: Erro ao processar saldo da carteira " 
                              << codigoCarteira.getValor() << " - " << e.what() << std::endl;
                }
            }
        }
    }
    
    // Converte o saldo total consolidado de centavos para formato brasileiro
    try {
        std::string saldoFormatado = centavosParaDinheiro(saldoTotalCentavos);
        saldo->setValor(saldoFormatado);
    } catch (const std::exception&) {
        // Em caso de erro, define saldo como valor mínimo válido
        saldo->setValor("0,01");
    }
    
    return true;
}

bool ControladoraServico::editarConta(const Conta& conta) {
    // Procura a conta pelo CPF
    std::map<std::string, Conta>::iterator it = contas.find(conta.getNcpf().getValor());
    // auto it = contas.find(conta.getNcpf().getValor());  // Versão com auto para comparação
    if (it == contas.end()) {
        return false; // Conta não encontrada
    }
    
    // Atualiza os dados da conta
    it->second = conta;
    return true;
}

bool ControladoraServico::excluirConta(const Ncpf& cpf) {
    // Verifica se a conta existe
    std::map<std::string, Conta>::iterator it = contas.find(cpf.getValor());
    // auto it = contas.find(cpf.getValor());  // Versão com auto para comparação
    if (it == contas.end()) {
        return false; // Conta não encontrada
    }
    
    // Verifica se a conta possui carteiras associadas
    std::map<std::string, std::list<Codigo>>::iterator itCarteiras = mapaContaCarteiras.find(cpf.getValor());
    // auto itCarteiras = mapaContaCarteiras.find(cpf.getValor());  // Versão com auto para comparação
    if (itCarteiras != mapaContaCarteiras.end() && !itCarteiras->second.empty()) {
        return false; // Conta possui carteiras, não pode ser excluída
    }
    
    // Remove a conta
    contas.erase(it);
    
    // Remove a entrada do mapa de carteiras (se existir, mas deve estar vazia)
    if (itCarteiras != mapaContaCarteiras.end()) {
        mapaContaCarteiras.erase(itCarteiras);
    }
    
    return true;
}
/**
 * @brief Cria uma nova carteira para um usuário específico
 * 
 * @details Este método implementa as regras de negócio fundamentais:
 * 1. **Código único**: O código da carteira deve ser único em todo o sistema
 * 2. **Limite por usuário**: Cada usuário pode ter no máximo 5 carteiras
 * 
 * O método realiza as seguintes validações:
 * - Verifica se a conta do usuário existe
 * - Confirma que o código da carteira não está em uso
 * - Valida que o usuário não excede o limite de 5 carteiras
 * - Armazena a carteira e cria a associação com o usuário
 * 
 * @param cpf CPF do titular da conta que criará a carteira
 * @param carteira Objeto Carteira com os dados (código, nome, perfil)
 * @return true se a carteira foi criada com sucesso, false caso contrário
 * 
 * @note A carteira é armazenada em dois mapas:
 *       - `carteiras`: Dados completos da carteira
 *       - `mapaContaCarteiras`: Associação usuário → lista de códigos
 * 
 * @warning Falha se:
 *          - Conta do usuário não existe
 *          - Código da carteira já está em uso
 *          - Usuário já possui 5 carteiras (limite máximo)
 */
bool ControladoraServico::criarCarteira(const Ncpf& cpf, const Carteira& carteira) {
    // 1. Verificar se a conta do usuário existe
    std::map<std::string, Conta>::iterator itConta = contas.find(cpf.getValor());
    // auto itConta = contas.find(cpf.getValor());  // Versão com auto para comparação
    if (itConta == contas.end()) {
        return false; // Conta não encontrada
    }
    
    // 2. Verificar se o código da carteira é único no sistema
    std::string codigoCarteira = carteira.getCodigo().getValor();
    std::map<std::string, Carteira>::iterator itCarteira = carteiras.find(codigoCarteira);
    // auto itCarteira = carteiras.find(codigoCarteira);  // Versão com auto para comparação
    if (itCarteira != carteiras.end()) {
        return false; // Código já existe - deve ser único no sistema
    }
    
    // 3. Verificar limite de carteiras por usuário (máximo 5)
    std::string cpfString = cpf.getValor();
    std::map<std::string, std::list<Codigo>>::iterator itUsuarioCarteiras = mapaContaCarteiras.find(cpfString);
    // auto itUsuarioCarteiras = mapaContaCarteiras.find(cpfString);  // Versão com auto para comparação
    
    size_t quantidadeAtual = 0;
    if (itUsuarioCarteiras != mapaContaCarteiras.end()) {
        quantidadeAtual = itUsuarioCarteiras->second.size();
    }
    
    if (quantidadeAtual >= 5) {
        return false; // Usuário já possui o máximo de 5 carteiras
    }
    
    // 4. Todas as validações passaram - criar a carteira
    
    // Armazena a carteira no mapa principal
    carteiras[codigoCarteira] = carteira;
    
    // Associa a carteira ao usuário
    if (itUsuarioCarteiras != mapaContaCarteiras.end()) {
        // Usuário já tem carteiras - adiciona à lista existente
        itUsuarioCarteiras->second.push_back(carteira.getCodigo());
    } else {
        // Primeira carteira do usuário - cria nova entrada
        std::list<Codigo> novaListaCarteiras;
        novaListaCarteiras.push_back(carteira.getCodigo());
        mapaContaCarteiras[cpfString] = novaListaCarteiras;
    }
    
    return true; // Carteira criada com sucesso
}
/**
 * @brief Lista todas as carteiras associadas a uma conta específica
 * 
 * @details Este método busca todas as carteiras que pertencem ao usuário identificado pelo CPF.
 * Utiliza o mapa de relacionamento `mapaContaCarteiras` para encontrar os códigos das carteiras
 * e então busca os dados completos de cada carteira no mapa `carteiras`.
 * 
 * O método preenche a lista fornecida como parâmetro com todas as carteiras encontradas.
 * Não modifica a lista se nenhuma carteira for encontrada.
 * 
 * @param cpf CPF do titular da conta (chave de busca)
 * @param listaCarteiras Ponteiro para lista que será preenchida com as carteiras encontradas
 * @return true se encontrou carteiras associadas à conta, false caso contrário
 * 
 * @note A lista de retorno é limpa antes de ser preenchida
 * @see criarCarteira() para associar novas carteiras a uma conta
 * 
 * @example
 * @code
 * Ncpf cpfUsuario;
 * cpfUsuario.setValor("123.456.789-00");
 * std::list<Carteira> minhasCarteiras;
 * 
 * if (servico.listarCarteiras(cpfUsuario, &minhasCarteiras)) {
 *     for (const auto& carteira : minhasCarteiras) {
 *         std::cout << "Carteira: " << carteira.getNome().getValor() << std::endl;
 *     }
 * }
 * @endcode
 */
bool ControladoraServico::listarCarteiras(const Ncpf& cpf, std::list<Carteira>* listaCarteiras) {
    // Limpa a lista de retorno para garantir que inicie vazia
    listaCarteiras->clear();
    
    // Verifica se a conta existe
    std::map<std::string, Conta>::iterator itConta = contas.find(cpf.getValor());
    // auto itConta = contas.find(cpf.getValor());  // Versão com auto para comparação
    if (itConta == contas.end()) {
        return false; // Conta não encontrada
    }
    
    // Busca as carteiras associadas a esta conta
    std::map<std::string, std::list<Codigo>>::iterator itCarteiras = mapaContaCarteiras.find(cpf.getValor());
    // auto itCarteiras = mapaContaCarteiras.find(cpf.getValor());  // Versão com auto para comparação
    if (itCarteiras == mapaContaCarteiras.end()) {
        return false; // Usuário não possui carteiras
    }
    
    // Itera por todos os códigos de carteiras associados ao usuário
    for (const Codigo& codigoCarteira : itCarteiras->second) {
    // for (const auto& codigoCarteira : itCarteiras->second) {  // Versão com auto para comparação
        
        // Busca os dados completos desta carteira
        std::map<std::string, Carteira>::iterator itCarteira = carteiras.find(codigoCarteira.getValor());
        // auto itCarteira = carteiras.find(codigoCarteira.getValor());  // Versão com auto para comparação
        if (itCarteira != carteiras.end()) {
            // Adiciona a carteira encontrada à lista de retorno
            listaCarteiras->push_back(itCarteira->second);
        }
    }
    
    // Retorna true se encontrou pelo menos uma carteira
    return !listaCarteiras->empty();
}
/**
 * @brief Consulta uma carteira específica e calcula seu saldo total
 * 
 * @details Este método implementa a regra de negócio fundamental do sistema:
 * "O saldo de uma carteira é a soma dos valores de todas as ordens associadas a ela".
 * 
 * O cálculo é feito com precisão usando aritmética de centavos (inteiros longos),
 * evitando problemas de precisão de ponto flutuante comuns em sistemas financeiros.
 * 
 * Fluxo de execução:
 * 1. Localiza a carteira pelo código fornecido
 * 2. Recupera todas as ordens associadas à carteira
 * 3. Soma os valores de cada ordem em centavos
 * 4. Converte o total de volta para o formato brasileiro
 * 
 * @param codigo Código identificador único da carteira
 * @param carteira Ponteiro para objeto Carteira onde serão armazenados os dados
 * @param saldo Ponteiro para objeto Dinheiro onde será armazenado o saldo calculado
 * @return true se a carteira foi encontrada e processada com sucesso, false caso contrário
 * 
 * @note O método não modifica nenhum estado interno da classe
 * @see consultarConta() para cálculo do saldo total do usuário
 * 
 * @warning Se houver erro na conversão de alguma ordem, essa ordem é ignorada no cálculo
 */
bool ControladoraServico::consultarCarteira(const Codigo& codigo, Carteira* carteira, Dinheiro* saldo) {
    // Procura a carteira pelo código no mapa de carteiras
    std::map<std::string, Carteira>::iterator it = carteiras.find(codigo.getValor());
    // auto it = carteiras.find(codigo.getValor());  // Versão com auto para comparação
    if (it == carteiras.end()) {
        return false; // Carteira não encontrada
    }
    
    // Retorna os dados da carteira encontrada
    *carteira = it->second;
    
    // Inicializa acumulador de saldo em centavos (precisão total)
    long long saldoTotalCentavos = 0;
    
    // Verifica se a carteira possui ordens associadas
    std::map<std::string, std::list<Codigo>>::iterator itOrdens = mapaCarteiraOrdens.find(codigo.getValor());
    // auto itOrdens = mapaCarteiraOrdens.find(codigo.getValor());  // Versão com auto para comparação
    if (itOrdens != mapaCarteiraOrdens.end()) {
        // Itera por todas as ordens da carteira
        for (const Codigo& codigoOrdem : itOrdens->second) {
        // for (const auto& codigoOrdem : itOrdens->second) {  // Versão com auto para comparação
            // Localiza a ordem específica no mapa de ordens
            std::map<std::string, Ordem>::iterator itOrdem = ordens.find(codigoOrdem.getValor());
            // auto itOrdem = ordens.find(codigoOrdem.getValor());  // Versão com auto para comparação
            if (itOrdem != ordens.end()) {
                try {
                    // Converte o valor da ordem para centavos e soma ao total
                    // Exemplo: "1.500,50" vira 150050 centavos
                    long long valorOrdemCentavos = dinheiroParaCentavos(itOrdem->second.getDinheiro());
                    saldoTotalCentavos += valorOrdemCentavos;
                } catch (const std::exception& e) {
                    // Se há erro na conversão desta ordem, a ignora e continua
                    // (estratégia defensiva para manter o sistema funcionando)
                    std::cerr << "Aviso: Erro ao processar ordem " << codigoOrdem.getValor() 
                              << " - " << e.what() << std::endl;
                }
            }
        }
    }
    
    // Converte o saldo total de centavos de volta para formato brasileiro
    try {
        std::string saldoFormatado = centavosParaDinheiro(saldoTotalCentavos);
        saldo->setValor(saldoFormatado);
    } catch (const std::exception&) {
        // Em caso de erro, define saldo como valor mínimo válido
        saldo->setValor("0,01");
    }
    
    return true;
}
/**
 * @brief Edita os dados de uma carteira existente
 * 
 * @details Este método permite modificar o nome e perfil de uma carteira.
 * O código da carteira não pode ser alterado pois é a chave primária do sistema.
 * 
 * Seguindo o padrão de edição de conta, a busca é feita pelo código (imutável)
 * e apenas o nome e perfil podem ser atualizados.
 * 
 * @param carteira Objeto Carteira com os novos dados (código usado para busca)
 * @return true se a carteira foi encontrada e editada com sucesso, false caso contrário
 * 
 * @note O código da carteira é usado apenas como chave de busca, não é modificado
 * @warning Falha se a carteira com o código fornecido não existir
 * 
 * @example
 * @code
 * Carteira carteiraEditada;
 * carteiraEditada.getCodigo().setValor("10001");  // Para busca
 * carteiraEditada.getNome().setValor("Novo Nome");
 * carteiraEditada.getTipoPerfil().setValor("Moderado");
 * 
 * if (servico.editarCarteira(carteiraEditada)) {
 *     std::cout << "Carteira editada com sucesso!" << std::endl;
 * }
 * @endcode
 */
bool ControladoraServico::editarCarteira(const Carteira& carteira) {
    // Procura a carteira pelo código no mapa de carteiras
    std::string codigoCarteira = carteira.getCodigo().getValor();
    std::map<std::string, Carteira>::iterator it = carteiras.find(codigoCarteira);
    // auto it = carteiras.find(codigoCarteira);  // Versão com auto para comparação
    if (it == carteiras.end()) {
        return false; // Carteira não encontrada
    }
    
    // Atualiza os dados da carteira encontrada
    // IMPORTANTE: O código não muda (é a chave primária)
    // Apenas nome e perfil são atualizados
    it->second.setNome(carteira.getNome());
    it->second.setTipoPerfil(carteira.getTipoPerfil());
    
    return true; // Carteira editada com sucesso
}
/**
 * @brief Exclui uma carteira do sistema
 * 
 * @details Este método remove uma carteira e todas as suas associações.
 * Implementa a regra de negócio: uma carteira só pode ser excluída se não possuir
 * ordens de investimento associadas (similar à regra de exclusão de conta).
 * 
 * O processo de exclusão:
 * 1. Verifica se a carteira existe
 * 2. Confirma que não há ordens associadas
 * 3. Remove a carteira do mapa principal
 * 4. Remove a associação usuário → carteira
 * 
 * @param codigo Código identificador único da carteira a ser excluída
 * @return true se a carteira foi excluída com sucesso, false caso contrário
 * 
 * @warning Falha se:
 *          - Carteira não existe
 *          - Carteira possui ordens associadas (regra de integridade)
 * 
 * @note A exclusão é cascateada: remove da estrutura principal e dos relacionamentos
 */
bool ControladoraServico::excluirCarteira(const Codigo& codigo) {
    std::string codigoCarteira = codigo.getValor();
    
    // 1. Verificar se a carteira existe
    std::map<std::string, Carteira>::iterator itCarteira = carteiras.find(codigoCarteira);
    // auto itCarteira = carteiras.find(codigoCarteira);  // Versão com auto para comparação
    if (itCarteira == carteiras.end()) {
        return false; // Carteira não encontrada
    }
    
    // 2. Verificar se a carteira possui ordens associadas (regra de integridade)
    std::map<std::string, std::list<Codigo>>::iterator itOrdens = mapaCarteiraOrdens.find(codigoCarteira);
    // auto itOrdens = mapaCarteiraOrdens.find(codigoCarteira);  // Versão com auto para comparação
    if (itOrdens != mapaCarteiraOrdens.end() && !itOrdens->second.empty()) {
        return false; // Carteira possui ordens, não pode ser excluída
    }
    
    // 3. Remover a carteira do mapa principal
    carteiras.erase(itCarteira);
    
    // 4. Remover a associação usuário → carteira
    // Buscar em qual conta esta carteira está associada
    for (std::map<std::string, std::list<Codigo>>::iterator itConta = mapaContaCarteiras.begin();
         itConta != mapaContaCarteiras.end(); ++itConta) {
        // for (auto itConta = mapaContaCarteiras.begin(); itConta != mapaContaCarteiras.end(); ++itConta) {  // Versão com auto
        
        // Buscar o código da carteira na lista desta conta
        std::list<Codigo>::iterator itCodigoCarteira = std::find_if(
            itConta->second.begin(), 
            itConta->second.end(),
            [&codigoCarteira](const Codigo& cod) { return cod.getValor() == codigoCarteira; }
        );
        
        if (itCodigoCarteira != itConta->second.end()) {
            // Encontrou a carteira nesta conta - remove da lista
            itConta->second.erase(itCodigoCarteira);
            
            // Se a lista ficou vazia, remove a entrada do mapa
            if (itConta->second.empty()) {
                mapaContaCarteiras.erase(itConta);
            }
            break; // Carteira encontrada e removida
        }
    }
    
    // 5. Remover entrada do mapa de ordens (se existir mas estiver vazia)
    if (itOrdens != mapaCarteiraOrdens.end()) {
        mapaCarteiraOrdens.erase(itOrdens);
    }
    
    return true; // Carteira excluída com sucesso
}
bool ControladoraServico::criarOrdem(const Codigo& codigoCarteira, const Ordem& ordem) { return false; }
/**
 * @brief Lista todas as ordens associadas a uma carteira específica
 * 
 * @details Este método busca todas as ordens que pertencem à carteira identificada pelo código.
 * Utiliza o mapa de relacionamento `mapaCarteiraOrdens` para encontrar os códigos das ordens
 * e então busca os dados completos de cada ordem no mapa `ordens`.
 * 
 * O método preenche a lista fornecida como parâmetro com todas as ordens encontradas.
 * Não modifica a lista se nenhuma ordem for encontrada.
 * 
 * Padrão idêntico ao método `listarCarteiras()` mas para a entidade Ordem.
 * 
 * @param codigoCarteira Código da carteira (chave de busca)
 * @param listaOrdens Ponteiro para lista que será preenchida com as ordens encontradas
 * @return true se encontrou ordens associadas à carteira, false caso contrário
 * 
 * @note A lista de retorno é limpa antes de ser preenchida
 * @see listarCarteiras() para padrão similar com carteiras
 * @see criarOrdem() para associar novas ordens a uma carteira
 * 
 * @example
 * @code
 * Codigo codigoCarteira;
 * codigoCarteira.setValor("10001");
 * std::list<Ordem> ordensCarteira;
 * 
 * if (servico.listarOrdens(codigoCarteira, &ordensCarteira)) {
 *     for (const auto& ordem : ordensCarteira) {
 *         std::cout << "Ordem: " << ordem.getCodigo().getValor() << std::endl;
 *     }
 * }
 * @endcode
 */
bool ControladoraServico::listarOrdens(const Codigo& codigoCarteira, std::list<Ordem>* listaOrdens) {
    // Limpa a lista de retorno para garantir que inicie vazia
    listaOrdens->clear();
    
    // Verifica se a carteira existe
    std::map<std::string, Carteira>::iterator itCarteira = carteiras.find(codigoCarteira.getValor());
    // auto itCarteira = carteiras.find(codigoCarteira.getValor());  // Versão com auto para comparação
    if (itCarteira == carteiras.end()) {
        return false; // Carteira não encontrada
    }
    
    // Busca as ordens associadas a esta carteira
    std::map<std::string, std::list<Codigo>>::iterator itOrdens = mapaCarteiraOrdens.find(codigoCarteira.getValor());
    // auto itOrdens = mapaCarteiraOrdens.find(codigoCarteira.getValor());  // Versão com auto para comparação
    if (itOrdens == mapaCarteiraOrdens.end()) {
        return false; // Carteira não possui ordens
    }
    
    // Itera por todos os códigos de ordens associados à carteira
    for (const Codigo& codigoOrdem : itOrdens->second) {
    // for (const auto& codigoOrdem : itOrdens->second) {  // Versão com auto para comparação
        
        // Busca os dados completos desta ordem
        std::map<std::string, Ordem>::iterator itOrdem = ordens.find(codigoOrdem.getValor());
        // auto itOrdem = ordens.find(codigoOrdem.getValor());  // Versão com auto para comparação
        if (itOrdem != ordens.end()) {
            // Adiciona a ordem encontrada à lista de retorno
            listaOrdens->push_back(itOrdem->second);
        }
    }
    
    // Retorna true se encontrou pelo menos uma ordem
    return !listaOrdens->empty();
}
bool ControladoraServico::excluirOrdem(const Codigo& codigo) { return false; }