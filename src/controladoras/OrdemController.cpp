#include "OrdemController.hpp"
#include "InputValidator.hpp"
#include <limits>

/**
 * @brief Construtor da controladora de ordens
 * @param servico Ponteiro para o serviço de investimento
 * @details Inicializa a controladora com uma referência ao serviço de investimento
 *          necessário para realizar operações de persistência de ordens.
 */
OrdemController::OrdemController(IServicoInvestimento *servico) : servicoInvestimento(servico)
{
}

/**
 * @brief Executa o menu principal de gerenciamento de ordens
 * @param codigoCarteira Código da carteira para gerenciar ordens
 * @details Apresenta um menu interativo com opções para criar, listar e excluir ordens.
 *          Valida a existência da carteira antes de exibir o menu e atualiza o saldo
 *          automaticamente após operações que afetam o valor da carteira.
 * @see exibirMenu()
 * @see criarOrdem()
 * @see listarOrdens()
 * @see excluirOrdem()
 */
void OrdemController::executarMenu(const Codigo &codigoCarteira)
{
    Carteira carteiraAtual;
    Dinheiro saldoAtual;

    if (!servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual))
    {
        std::cout << "\nErro: Carteira nao encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    int opcao;

    while (true)
    {
        exibirMenu(carteiraAtual, saldoAtual);
        std::cin >> opcao;

        switch (opcao)
        {
        case 1:
            criarOrdem(codigoCarteira);
            servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual);
            break;
        case 2:
            listarOrdens(codigoCarteira);
            break;
        case 3:
            excluirOrdem(codigoCarteira);
            servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual);
            break;
        case 0:
            return;
        default:
            std::cout << "Opção inválida! Tente novamente." << std::endl;
        }
    }
}

/**
 * @brief Exibe o menu de opções para gerenciamento de ordens
 * @param carteiraAtual Dados da carteira atual
 * @param saldoAtual Saldo atual da carteira
 * @details Apresenta uma interface formatada com informações da carteira e opções
 *          disponíveis para gerenciamento de ordens. Inclui nome, código e saldo da carteira.
 */
void OrdemController::exibirMenu(const Carteira &carteiraAtual, const Dinheiro &saldoAtual)
{
    telaUtils::exibirCabecalho("MENU DE ORDENS");
    std::cout << "Carteira: " << carteiraAtual.getNome().getValor()
              << " (Código: " << carteiraAtual.getCodigo().getValor() << ")" << std::endl;
    std::cout << "Saldo Atual: R$ " << saldoAtual.getValor() << std::endl;
    telaUtils::exibirSeparador('-', 40);
    std::cout << "1. Criar nova ordem" << std::endl;
    std::cout << "2. Listar todas as ordens" << std::endl;
    std::cout << "3. Excluir ordem" << std::endl;
    std::cout << "0. Voltar ao menu anterior" << std::endl;
    telaUtils::exibirSeparador('-', 40);
    std::cout << "Escolha uma opção: ";
}

/**
 * @brief Cria uma nova ordem de investimento
 * @param codigoCarteira Código da carteira onde a ordem será criada
 * @details Processo interativo completo para criação de ordens incluindo:
 *          - Validação da carteira
 *          - Coleta de dados da ordem (código, papel, data, quantidade)
 *          - Validação contra dados históricos da B3
 *          - Confirmação do usuário
 *          - Criação da ordem com cálculo automático do valor
 * @see solicitarCodigoOrdem()
 * @see solicitarCodigoNegociacao()
 * @see solicitarDataOrdem()
 * @see solicitarQuantidade()
 */
void OrdemController::criarOrdem(const Codigo &codigoCarteira)
{
    telaUtils::exibirCabecalho("CRIACAO DE NOVA ORDEM");

    Carteira carteiraAtual;
    Dinheiro saldoAtual;

    Codigo codigoOrdem;
    CodigoNeg codigoNegociacao;
    Data dataOrdem;
    Quantidade quantidadeOrdem;

    if (!servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoAtual))
    {
        std::cout << "\n";
        std::cout << "┌─────────────────────────────────────────────────────────────┐" << std::endl;
        std::cout << "│                    ✗ ERRO: CARTEIRA NÃO ENCONTRADA ✗      │" << std::endl;
        std::cout << "└─────────────────────────────────────────────────────────────┘" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    exibirInfoCarteira(carteiraAtual, saldoAtual);

    std::list<Ordem> ordensExistentes;
    if (servicoInvestimento->listarOrdens(codigoCarteira, &ordensExistentes))
    {
        exibirOrdensExistentes(ordensExistentes);
    }

    exibirInstrucoesCriacao();

    if (!solicitarCodigoOrdem(codigoOrdem))
    {
        return;
    }

    if (!solicitarCodigoNegociacao(codigoNegociacao))
    {
        return;
    }

    if (!solicitarDataOrdem(dataOrdem, codigoNegociacao))
    {
        return;
    }

    if (!solicitarQuantidade(quantidadeOrdem))
    {
        return;
    }

    exibirResumoOrdem(codigoOrdem, codigoNegociacao, dataOrdem, quantidadeOrdem, carteiraAtual);

    char confirmacao;
    std::cout << "\nConfirma a criacao da ordem? (s/n): ";
    std::cin >> confirmacao;

    if (confirmacao != 's' && confirmacao != 'S')
    {
        std::cout << "\nCriacao da ordem cancelada pelo usuario." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    Ordem novaOrdem;
    novaOrdem.setCodigo(codigoOrdem);
    novaOrdem.setCodigoNeg(codigoNegociacao);
    novaOrdem.setData(dataOrdem);
    novaOrdem.setQuantidade(quantidadeOrdem);

    Dinheiro valorTemporario;
    valorTemporario.setValor("0,01");
    novaOrdem.setDinheiro(valorTemporario);

    if (servicoInvestimento->criarOrdem(codigoCarteira, novaOrdem))
    {
        std::cout << "\n";
        std::cout << "┌─────────────────────────────────────────────────────────────┐" << std::endl;
        std::cout << "│                    ✓ ORDEM CRIADA COM SUCESSO! ✓           │" << std::endl;
        std::cout << "└─────────────────────────────────────────────────────────────┘" << std::endl;

        exibirDetalhesOrdemCriada(codigoOrdem, codigoCarteira, carteiraAtual);

        std::cout << "\n📋 INFORMAÇÕES ADICIONAIS:" << std::endl;
        std::cout << "   • A ordem foi associada à carteira '" << carteiraAtual.getNome().getValor() << "'"
                  << std::endl;
        std::cout << "   • O valor foi calculado automaticamente com base nos dados históricos" << std::endl;
        std::cout << "   • Fórmula utilizada: Preço Médio × Quantidade" << std::endl;
        std::cout << "   • A ordem está pronta para ser visualizada no sistema" << std::endl;
    }
    else
    {
        std::cout << "\n";
        std::cout << "┌─────────────────────────────────────────────────────────────┐" << std::endl;
        std::cout << "│                    ✗ ERRO NA CRIAÇÃO DA ORDEM ✗           │" << std::endl;
        std::cout << "└─────────────────────────────────────────────────────────────┘" << std::endl;

        std::cout << "\n🔍 POSSÍVEIS CAUSAS:" << std::endl;
        std::cout << "   • Código da ordem já existe no sistema" << std::endl;
        std::cout << "   • Carteira não encontrada" << std::endl;
        std::cout << "   • Papel ou data não encontrados no arquivo histórico" << std::endl;
        std::cout << "   • Arquivo DADOS_HISTORICOS.TXT não encontrado" << std::endl;
        std::cout << "   • Erro no cálculo do preço" << std::endl;

        std::cout << "\n💡 DICAS PARA RESOLVER:" << std::endl;
        std::cout << "   • Verifique se o código da ordem é único" << std::endl;
        std::cout << "   • Use apenas produtos disponíveis: 00001, 00002, 00003, etc." << std::endl;
        std::cout << "   • Use datas entre 20240315 e 20240320" << std::endl;
        std::cout << "   • Certifique-se que o arquivo DADOS_HISTORICOS.txt existe" << std::endl;
        std::cout << "   • Tente novamente com dados diferentes" << std::endl;
    }

    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

/**
 * @brief Exibe informações detalhadas da carteira selecionada
 * @param carteiraAtual Dados da carteira a ser exibida
 * @param saldoAtual Saldo atual da carteira
 * @details Apresenta uma tabela formatada com código, nome, perfil e saldo da carteira.
 *          Utilizada durante o processo de criação de ordens para contexto do usuário.
 */
void OrdemController::exibirInfoCarteira(const Carteira &carteiraAtual, const Dinheiro &saldoAtual)
{
    std::cout << "\n═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                    CARTEIRA SELECIONADA" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "  Código     : " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "  Nome       : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "  Perfil     : " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "  Saldo Atual: R$ " << saldoAtual.getValor() << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
}

/**
 * @brief Exibe tabela formatada com ordens existentes na carteira
 * @param ordensExistentes Lista de ordens a serem exibidas
 * @details Apresenta uma tabela com colunas para código, papel, data, valor e quantidade.
 *          Formata datas no padrão brasileiro e remove espaços dos códigos de negociação.
 */
void OrdemController::exibirOrdensExistentes(const std::list<Ordem> &ordensExistentes)
{
    std::cout << "\n═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                    ORDENS EXISTENTES NA CARTEIRA" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    if (ordensExistentes.empty())
    {
        std::cout << "📝 Nenhuma ordem encontrada nesta carteira." << std::endl;
    }
    else
    {
        std::cout << "┌─────────┬────────────┬────────────┬──────────────┬────────────┐" << std::endl;
        std::cout << "│ CÓDIGO  │   PAPEL    │    DATA    │    VALOR     │ QUANTIDADE │" << std::endl;
        std::cout << "├─────────┼────────────┼────────────┼──────────────┼────────────┤" << std::endl;

        for (const Ordem &ordem : ordensExistentes)
        {
            std::string codigo = ordem.getCodigo().getValor();
            std::string papel = ordem.getCodigoNeg().getValor();
            std::string data = ordem.getData().getValor();
            std::string valor = ordem.getDinheiro().getValor();
            std::string quantidade = ordem.getQuantidade().getValor();

            papel = InputValidator::removerEspacosFinais(papel);

            std::string dataFormatada = data.substr(6, 2) + "/" + data.substr(4, 2) + "/" + data.substr(0, 4);

            std::cout << "│ " << std::setw(7) << std::left << codigo << " │ " << std::setw(10) << std::left << papel
                      << " │ " << std::setw(10) << std::left << dataFormatada << " │ " << std::setw(12) << std::left
                      << ("R$ " + valor) << " │ " << std::setw(10) << std::left << quantidade << " │" << std::endl;
        }
        std::cout << "└─────────┴────────────┴────────────┴──────────────┴────────────┘" << std::endl;
    }
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
}

/**
 * @brief Exibe instruções detalhadas para criação de ordens
 * @details Apresenta um guia passo-a-passo explicando todos os campos necessários
 *          para criar uma ordem, incluindo formatos esperados e validações realizadas.
 */
void OrdemController::exibirInstrucoesCriacao()
{
    std::cout << "\n📋 COMO CRIAR UMA ORDEM:" << std::endl;
    std::cout << "   Para criar uma ordem, você precisa fornecer:" << std::endl;
    std::cout << "   1. CÓDIGO DA ORDEM    - ID único de 5 dígitos (ex: 30001, 30002)" << std::endl;
    std::cout << "   2. CÓDIGO DE NEGOCIAÇÃO - Código do ativo (ex: JBSS3, JALL3) - até 12 caracteres" << std::endl;
    std::cout << "   3. DATA               - Data da operação (ex: 20250110)" << std::endl;
    std::cout << "   💡 DICA: O sistema validará se a combinação código+data existe no arquivo B3." << std::endl;
}

/**
 * @brief Solicita e valida o código da ordem ao usuário
 * @param codigoOrdem Referência para armazenar o código válido
 * @return true se o código foi inserido com sucesso, false se cancelado
 * @details Processo interativo para coleta do código da ordem com validação
 *          automática. Permite cancelamento digitando '0' e fornece feedback
 *          detalhado sobre erros de validação.
 */
bool OrdemController::solicitarCodigoOrdem(Codigo &codigoOrdem)
{
    std::cout << "\n🔢 1. CÓDIGO DA ORDEM     - ID único de 5 dígitos (ex: 30001, 30002)" << std::endl;
    std::cout << "   💡 DICA: Use códigos únicos que não existam no sistema" << std::endl;

    while (true)
    {
        try
        {
            std::cout << "\nDigite o CÓDIGO DA ORDEM (5 dígitos) ou '0' para cancelar: ";
            std::string valorCodigo;
            std::cin >> valorCodigo;

            if (valorCodigo == "0")
            {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return false;
            }

            codigoOrdem.setValor(valorCodigo);
            std::cout << "✅ Código da ordem válido: " << valorCodigo << std::endl;
            return true;
        }
        catch (const std::invalid_argument &exp)
        {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Dica: Use um código de 5 dígitos numéricos (ex: 30001)" << std::endl;
        }
    }
}

/**
 * @brief Solicita e valida o código de negociação ao usuário
 * @param codigoNegociacao Referência para armazenar o código válido
 * @return true se o código foi inserido com sucesso, false se cancelado
 * @details Processo interativo para coleta do código de negociação com:
 *          - Validação de tamanho (até 12 caracteres)
 *          - Suporte a extração de códigos de linhas B3 completas
 *          - Formatação automática para 12 caracteres
 *          - Opção de cancelamento
 */
bool OrdemController::solicitarCodigoNegociacao(CodigoNeg &codigoNegociacao)
{
    std::cout << "\n📈 2. CÓDIGO DE NEGOCIAÇÃO - Código do ativo (ex: JBSS3, JALL3) - até 12 caracteres" << std::endl;
    std::cout << "   💡 DICA: Digite o código do ativo que deseja negociar" << std::endl;

    while (true)
    {
        try
        {
            std::cout << "\nDigite o CÓDIGO DE NEGOCIAÇÃO (ex: JBSS3) ou '0' para cancelar: ";
            std::string entradaCodigo;
            std::cin.ignore();
            std::getline(std::cin, entradaCodigo);

            if (entradaCodigo == "0")
            {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.get();
                return false;
            }

            if (entradaCodigo.length() > 20)
            {
                if (entradaCodigo.length() >= 24)
                {
                    std::string codigoExtraido = InputValidator::extrairCodigoB3(entradaCodigo);
                    std::string codigoCompleto = InputValidator::formatarCodigoNegociacao(codigoExtraido);

                    codigoNegociacao.setValor(codigoCompleto);
                    std::cout << "✅ Código extraído da linha B3: '" << codigoExtraido << "'" << std::endl;
                    return true;
                }
                else
                {
                    std::cout << "❌ ERRO: Linha muito curta. Use apenas o código de negociação (ex: IVVB11)"
                              << std::endl;
                    continue;
                }
            }

            if (entradaCodigo.length() == 0 || entradaCodigo.length() > 12)
            {
                std::cout << "❌ ERRO: Código de negociação deve ter até 12 caracteres." << std::endl;
                std::cout << "   Exemplo: JBSS3, JALL3, HYPE3, IVVB11" << std::endl;
                continue;
            }

            std::string codigoCompleto = InputValidator::formatarCodigoNegociacao(entradaCodigo);
            codigoNegociacao.setValor(codigoCompleto);
            std::cout << "✅ Código de negociação válido: '" << entradaCodigo << "'" << std::endl;
            return true;
        }
        catch (const std::invalid_argument &exp)
        {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Exemplo: JBSS3, JALL3, HYPE3" << std::endl;
        }
    }
}

/**
 * @brief Solicita e valida a data da ordem ao usuário
 * @param dataOrdem Referência para armazenar a data válida
 * @param codigoNegociacao Código de negociação para validação cruzada
 * @return true se a data foi inserida com sucesso, false se cancelado
 * @details Processo interativo para coleta da data com validação contra dados históricos:
 *          - Formato AAAAMMDD obrigatório
 *          - Validação de existência da combinação código+data no arquivo B3
 *          - Feedback específico sobre disponibilidade dos dados
 *          - Opção de cancelamento
 */
bool OrdemController::solicitarDataOrdem(Data &dataOrdem, const CodigoNeg &codigoNegociacao)
{
    std::cout << "\n📄 3. DATA               - Data da operação (ex: 20250110)" << std::endl;
    std::cout << "   💡 DICA: O sistema validará se a combinação código+data existe no arquivo B3" << std::endl;

    while (true)
    {
        try
        {
            std::cout << "\nDigite a DATA da ordem (AAAAMMDD) ou '0' para cancelar: ";
            std::string valorData;
            std::cin >> valorData;

            if (valorData == "0")
            {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return false;
            }

            if (valorData.length() != 8)
            {
                std::cout << "❌ ERRO: Data deve ter 8 dígitos no formato AAAAMMDD (ex: 20250110)" << std::endl;
                continue;
            }

            if (!InputValidator::contemApenasDigitos(valorData))
            {
                std::cout << "❌ ERRO: Data deve conter apenas números (ex: 20250110)" << std::endl;
                continue;
            }

            std::string codigoLimpo = InputValidator::removerEspacosFinais(codigoNegociacao.getValor());

            if (!InputValidator::validarCombinacaoB3(codigoNegociacao, valorData))
            {
                std::cout << "❌ ERRO: Não há dados para o papel '" << codigoLimpo << "' na data '" << valorData << "'."
                          << std::endl;
                std::cout << "   Tente outra data ou verifique se o código está correto." << std::endl;
                continue;
            }

            dataOrdem.setValor(valorData);
            std::cout << "✅ Combinação válida: '" << codigoLimpo << "' na data '" << valorData
                      << "' encontrada no arquivo B3!" << std::endl;
            return true;
        }
        catch (const std::invalid_argument &exp)
        {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Dica: Use um formato de data válido (ex: 20250110)" << std::endl;
        }
    }
}

/**
 * @brief Solicita e valida a quantidade de papéis ao usuário
 * @param quantidadeOrdem Referência para armazenar a quantidade válida
 * @return true se a quantidade foi inserida com sucesso, false se cancelado
 * @details Processo interativo para coleta da quantidade com validação automática
 *          do domínio Quantidade. Suporta formatação com pontos para milhares.
 */
bool OrdemController::solicitarQuantidade(Quantidade &quantidadeOrdem)
{
    std::cout << "\n🔢 4. QUANTIDADE         - Quantos papéis (ex: 100, 1.000)" << std::endl;
    std::cout << "   💡 DICA: Digite números inteiros (ex: 1000 ou 1.000, 5000 ou 5.000)" << std::endl;

    while (true)
    {
        try
        {
            std::cout << "\nDigite a QUANTIDADE de papéis ou '0' para cancelar: ";
            std::string valorQuantidade;
            std::cin >> valorQuantidade;

            if (valorQuantidade == "0")
            {
                std::cout << "\nCriação de ordem cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return false;
            }

            quantidadeOrdem.setValor(valorQuantidade);
            std::cout << "✅ Quantidade válida: " << valorQuantidade << std::endl;
            return true;
        }
        catch (const std::invalid_argument &exp)
        {
            std::cout << "❌ ERRO: " << exp.what() << std::endl;
            std::cout << "   Exemplo: 100, 500, 1000 ou 1.000, 5000 ou 5.000, etc." << std::endl;
        }
    }
}

void OrdemController::exibirResumoOrdem(const Codigo &codigoOrdem, const CodigoNeg &codigoNegociacao,
                                        const Data &dataOrdem, const Quantidade &quantidadeOrdem,
                                        const Carteira &carteiraAtual)
{
    std::cout << "\n═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                    RESUMO DA ORDEM" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "  Código da Ordem     : " << codigoOrdem.getValor() << std::endl;
    std::cout << "  Papel (Cód. B3)     : " << codigoNegociacao.getValor() << std::endl;
    std::cout << "  Data                : " << dataOrdem.getValor() << std::endl;
    std::cout << "  Quantidade          : " << quantidadeOrdem.getValor() << std::endl;
    std::cout << "  Carteira            : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    std::cout << "\n📊 INFORMAÇÕES IMPORTANTES:" << std::endl;
    std::cout << "   • O PREÇO da ordem será calculado automaticamente" << std::endl;
    std::cout << "   • Fórmula: Preço Médio Histórico B3 × Quantidade" << std::endl;
    std::cout << "   • Os dados serão buscados no arquivo DADOS_HISTORICOS.txt" << std::endl;
}

void OrdemController::exibirDetalhesOrdemCriada(const Codigo &codigoOrdem, const Codigo &codigoCarteira,
                                                const Carteira &carteiraAtual)
{
    std::list<Ordem> ordensCarteira;
    if (servicoInvestimento->listarOrdens(codigoCarteira, &ordensCarteira))
    {
        for (const Ordem &ordem : ordensCarteira)
        {
            if (ordem.getCodigo().getValor() == codigoOrdem.getValor())
            {
                std::cout << "\n═══════════════════════════════════════════════════════════════" << std::endl;
                std::cout << "                    DETALHES DA ORDEM CRIADA" << std::endl;
                std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
                std::cout << "  Código da Ordem : " << ordem.getCodigo().getValor() << std::endl;
                std::cout << "  Papel           : " << ordem.getCodigoNeg().getValor() << std::endl;
                std::cout << "  Data            : " << ordem.getData().getValor() << std::endl;
                std::cout << "  Quantidade      : " << ordem.getQuantidade().getValor() << std::endl;
                std::cout << "  VALOR TOTAL     : R$ " << ordem.getDinheiro().getValor() << std::endl;
                std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
                break;
            }
        }
    }
}

/**
 * @brief Lista todas as ordens de uma carteira específica
 * @param codigoCarteira Código da carteira para listar ordens
 * @details Apresenta uma listagem completa das ordens da carteira incluindo:
 *          - Informações da carteira (nome, código, perfil, saldo)
 *          - Tabela formatada com todas as ordens
 *          - Saldo consolidado da carteira
 *          - Informações explicativas sobre o sistema
 * @see exibirListaOrdens()
 */
void OrdemController::listarOrdens(const Codigo &codigoCarteira)
{
    telaUtils::exibirCabecalho("LISTA DE ORDENS DA CARTEIRA");

    Carteira carteiraAtual;
    Dinheiro saldoCarteira;

    if (!servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoCarteira))
    {
        std::cout << "\nErro: Carteira nao encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << "\n=== CARTEIRA ===" << std::endl;
    std::cout << "Codigo: " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "SALDO TOTAL: R$ " << saldoCarteira.getValor() << std::endl;
    std::cout << "=================" << std::endl;

    std::list<Ordem> ordensCarteira;

    if (!servicoInvestimento->listarOrdens(codigoCarteira, &ordensCarteira))
    {
        std::cout << "\n=== ORDENS ===" << std::endl;
        std::cout << "Esta carteira ainda nao possui ordens de investimento." << std::endl;
        std::cout << "Use a opcao 'Criar nova ordem' para comecar a investir!" << std::endl;
        std::cout << "==============" << std::endl;
    }
    else if (ordensCarteira.empty())
    {
        std::cout << "\n=== ORDENS ===" << std::endl;
        std::cout << "Esta carteira ainda nao possui ordens de investimento." << std::endl;
        std::cout << "Use a opcao 'Criar nova ordem' para comecar a investir!" << std::endl;
        std::cout << "==============" << std::endl;
    }
    else
    {
        exibirListaOrdens(ordensCarteira, saldoCarteira);
    }

    std::cout << "\nNOTA IMPORTANTE:" << std::endl;
    std::cout << "- O saldo da carteira e a soma de todas as ordens listadas acima" << std::endl;
    std::cout << "- Cada ordem representa um investimento ja realizado" << std::endl;
    std::cout << "- O valor da ordem foi calculado com base nos dados historicos" << std::endl;

    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void OrdemController::exibirListaOrdens(const std::list<Ordem> &ordensCarteira, const Dinheiro &saldoCarteira)
{
    std::cout << "\n=== ORDENS DESTA CARTEIRA ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Codigo" << std::setw(15) << "Papel" << std::setw(12) << "Data"
              << std::setw(12) << "Quantidade" << std::setw(15) << "Valor Total" << std::endl;
    std::cout << std::string(62, '-') << std::endl;

    for (const Ordem &ordem : ordensCarteira)
    {
        std::string codigoNegLimpo = InputValidator::removerEspacosFinais(ordem.getCodigoNeg().getValor());

        std::cout << std::left << std::setw(8) << ordem.getCodigo().getValor() << std::setw(15) << codigoNegLimpo
                  << std::setw(12) << ordem.getData().getValor() << std::setw(12) << ordem.getQuantidade().getValor()
                  << std::setw(15) << ("R$ " + ordem.getDinheiro().getValor()) << std::endl;
    }

    std::cout << std::string(62, '-') << std::endl;
    std::cout << "Total de ordens: " << ordensCarteira.size() << std::endl;
    std::cout << "SALDO CONSOLIDADO: R$ " << saldoCarteira.getValor() << std::endl;
    std::cout << "==============================" << std::endl;
}

/**
 * @brief Exclui uma ordem específica da carteira
 * @param codigoCarteira Código da carteira que contém a ordem
 * @details Processo interativo para exclusão segura de ordens incluindo:
 *          - Validação da existência da carteira e ordens
 *          - Listagem das ordens disponíveis para exclusão
 *          - Confirmação de segurança com detalhes da ordem
 *          - Exclusão permanente e recálculo automático do saldo
 * @see exibirOrdensParaExclusao()
 * @see solicitarCodigoExclusao()
 */
void OrdemController::excluirOrdem(const Codigo &codigoCarteira)
{
    telaUtils::exibirCabecalho("EXCLUSAO DE ORDEM");

    std::list<Ordem> ordensCarteira;
    Carteira carteiraAtual;
    Dinheiro saldoCarteira;

    if (!servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &saldoCarteira))
    {
        std::cout << "\nErro: Carteira não encontrada!" << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    if (!servicoInvestimento->listarOrdens(codigoCarteira, &ordensCarteira) || ordensCarteira.empty())
    {
        std::cout << "\n=== NENHUMA ORDEM DISPONÍVEL ===" << std::endl;
        std::cout << "Esta carteira não possui ordens para excluir." << std::endl;
        std::cout << "===============================\n" << std::endl;
        std::cout << "Pressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::cout << "\n=== CARTEIRA ATUAL ===" << std::endl;
    std::cout << "Código: " << carteiraAtual.getCodigo().getValor() << std::endl;
    std::cout << "Nome  : " << carteiraAtual.getNome().getValor() << std::endl;
    std::cout << "Perfil: " << carteiraAtual.getTipoPerfil().getValor() << std::endl;
    std::cout << "Saldo : R$ " << saldoCarteira.getValor() << std::endl;
    std::cout << "======================\n" << std::endl;

    exibirOrdensParaExclusao(ordensCarteira);

    Codigo codigoOrdem;
    Ordem ordemSelecionada;

    if (!solicitarCodigoExclusao(codigoOrdem, ordensCarteira, ordemSelecionada))
    {
        return;
    }

    std::string codigoNegLimpo = InputValidator::removerEspacosFinais(ordemSelecionada.getCodigoNeg().getValor());

    std::cout << "\n*** ATENÇÃO - EXCLUSÃO PERMANENTE ***" << std::endl;
    std::cout << "Ordem selecionada:" << std::endl;
    std::cout << "  Código     : " << ordemSelecionada.getCodigo().getValor() << std::endl;
    std::cout << "  Papel      : " << codigoNegLimpo << std::endl;
    std::cout << "  Data       : " << ordemSelecionada.getData().getValor() << std::endl;
    std::cout << "  Quantidade : " << ordemSelecionada.getQuantidade().getValor() << std::endl;
    std::cout << "  Valor Total: R$ " << ordemSelecionada.getDinheiro().getValor() << std::endl;
    std::cout << "***********************************" << std::endl;

    std::cout << "\nIMPORTANTE:" << std::endl;
    std::cout << "- A exclusão é PERMANENTE e não pode ser desfeita" << std::endl;
    std::cout << "- A ordem será removida da carteira associada" << std::endl;
    std::cout << "- O saldo da carteira será recalculado automaticamente" << std::endl;

    std::cout << "\nTem certeza que deseja EXCLUIR esta ordem? (digite 'sim' para confirmar): ";
    std::string confirmacao;
    std::cin >> confirmacao;

    if (confirmacao != "SIM" && confirmacao != "sim" && confirmacao != "S" && confirmacao != "s")
    {
        std::cout << "\nExclusão cancelada pelo usuário." << std::endl;
        std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return;
    }

    if (servicoInvestimento->excluirOrdem(codigoOrdem))
    {
        std::cout << "\n*** ORDEM EXCLUÍDA COM SUCESSO ***" << std::endl;
        std::cout << "A ordem com código '" << codigoOrdem.getValor() << "' foi removida permanentemente." << std::endl;
        std::cout << "O saldo da carteira foi recalculado automaticamente." << std::endl;

        Dinheiro novoSaldo;
        if (servicoInvestimento->consultarCarteira(codigoCarteira, &carteiraAtual, &novoSaldo))
        {
            std::cout << "Novo saldo da carteira: R$ " << novoSaldo.getValor() << std::endl;
        }
    }
    else
    {
        std::cout << "\n*** ERRO NA EXCLUSÃO ***" << std::endl;
        std::cout << "Não foi possível excluir a ordem." << std::endl;
        std::cout << "Possíveis causas:" << std::endl;
        std::cout << "- Ordem com código '" << codigoOrdem.getValor() << "' não foi encontrada" << std::endl;
        std::cout << "- Erro interno do sistema" << std::endl;

        std::cout << "\nDICAS:" << std::endl;
        std::cout << "- Verifique se o código da ordem está correto" << std::endl;
        std::cout << "- Use a opção 'Listar ordens' para ver os códigos disponíveis" << std::endl;
        std::cout << "- Certifique-se de que a ordem pertence a alguma carteira" << std::endl;
    }

    std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
    std::cin.ignore();
    std::cin.get();
}

void OrdemController::exibirOrdensParaExclusao(const std::list<Ordem> &ordensCarteira)
{
    std::cout << "=== ORDENS DISPONÍVEIS PARA EXCLUSÃO ===" << std::endl;
    std::cout << std::left << std::setw(8) << "Código" << std::setw(15) << "Papel" << std::setw(12) << "Data"
              << std::setw(12) << "Quantidade" << std::setw(15) << "Valor Total" << std::endl;
    std::cout << std::string(62, '-') << std::endl;

    for (const Ordem &ordem : ordensCarteira)
    {
        std::string codigoNegLimpo = InputValidator::removerEspacosFinais(ordem.getCodigoNeg().getValor());

        std::cout << std::left << std::setw(8) << ordem.getCodigo().getValor() << std::setw(15) << codigoNegLimpo
                  << std::setw(12) << ordem.getData().getValor() << std::setw(12) << ordem.getQuantidade().getValor()
                  << std::setw(15) << ("R$ " + ordem.getDinheiro().getValor()) << std::endl;
    }

    std::cout << std::string(62, '-') << std::endl;
    std::cout << "Total de ordens: " << ordensCarteira.size() << std::endl;
    std::cout << "========================================\n" << std::endl;
}

bool OrdemController::solicitarCodigoExclusao(Codigo &codigoOrdem, const std::list<Ordem> &ordensCarteira,
                                              Ordem &ordemSelecionada)
{
    while (true)
    {
        try
        {
            std::cout << "Digite o código da ordem que deseja excluir (ou '0' para cancelar): ";
            std::string valorCodigo;
            std::cin >> valorCodigo;

            if (valorCodigo == "0")
            {
                std::cout << "\nExclusão cancelada pelo usuário." << std::endl;
                std::cout << "\nPressione qualquer tecla para continuar..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return false;
            }

            codigoOrdem.setValor(valorCodigo);

            bool ordemEncontrada = false;
            for (const Ordem &ordem : ordensCarteira)
            {
                if (ordem.getCodigo().getValor() == codigoOrdem.getValor())
                {
                    ordemEncontrada = true;
                    ordemSelecionada = ordem;
                    break;
                }
            }

            if (!ordemEncontrada)
            {
                std::cout << "Erro: Código '" << valorCodigo << "' não encontrado nas ordens desta carteira."
                          << std::endl;
                std::cout << "Por favor, escolha um código da lista acima." << std::endl;
                continue;
            }

            return true;
        }
        catch (const std::invalid_argument &exp)
        {
            std::cout << "Erro: " << exp.what() << std::endl;
            std::cout << "Dica: Use um código de 5 dígitos numéricos (ex: 30001)" << std::endl;
        }
    }
}