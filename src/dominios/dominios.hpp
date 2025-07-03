 /**
 * @file dominios.hpp
 * @brief Biblioteca de dominios do sistema de investimentos financeiros.
 *
 * Este arquivo define os tipos fundamentais utilizados como base para
 * validacao e representacao de valores primitivos com regras de negocio
 * especificas no sistema de investimentos.
 *
 * Cada dominio encapsula um tipo basico (como string) com regras
 * de validacao proprias, garantindo a integridade e a consistencia dos dados
 * desde sua criacao ate o uso nas entidades do sistema.
 *
 * @details
 * Os dominios foram projetados para validar automaticamente os valores
 * atribuidos a eles, evitando inconsistencias e entradas invalidas no sistema.
 * Exemplos incluem CPF, nome, senha, codigo de ativos, datas, quantidades,
 * valores monetarios e tipos de perfil.
 *
 * Estes tipos sao amplamente utilizados nas entidades do sistema, como
 * @ref Conta, @ref Carteira e @ref Ordem, promovendo reutilizacao e seguranca
 * de dados nas operacoes financeiras.
 *
 * @see entidades.hpp
 */
//---------------------------------------------------------------------
#ifndef DOMINIOS_HPP_INCLUDED
#define DOMINIOS_HPP_INCLUDED

//---------------------------------------------------------------------
#include <string>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <iomanip>
#include <set>

using namespace std;

///---------------------------------------------------------------------
//  Dominio Codigo     (Responsavel: Micaele 231021450)
/**
* @class Codigo
* @brief Classe que representa uma sequencia numerica unica de identificacao, com exatamente 5 digitos numericos.
*
* Esta classe e utilizada para representar um valor unico (codigo) no sistema de investimentos,
* podendo estar associado a Carteiras de investimento ou Ordens de compra/venda. Ela garante que os valores
* dos codigos obedecam a um formato estrito: exatamente 5 caracteres, todos digitos de 0 a 9.
*
* A classe e usada para armazenar e validar um valor (codigo) representado como uma string, que deve
* obedecer a regras especificas de formato, garantindo que o valor interno sempre esteja em conformidade
* com as regras estabelecidas.
*
* Regras de validacao:
* - O codigo deve conter exatamente 5 caracteres.
* - Todos os caracteres devem ser digitos numericos.
*
* Metodos disponiveis:
* - `setValor`: Metodo publico que atribui um novo valor ao codigo, apos validacao.
* - `getValor`: Metodo publico que retorna o valor atual armazenado.
*
* O codigo invalido resultara em uma excecao `std::invalid_argument`.
*/
class Codigo {
    private:
        /**
        * @brief Quantidade fixa de digitos exigidos para o codigo.
        */
        static const int Q_DIGITOS = 5;

        /**
        * @brief Armazena o valor do codigo apos validacao.
        */
        string valor;

        /**
        * @brief Metodo privado que valida se o valor fornecido atende ao formato correto para a classe Codigo.
        *
        * Este metodo verifica se a string fornecida tem exatamente 5 caracteres
        * e se todos os caracteres sao digitos numericos (0 a 9).
        * Lanca uma excecao se o valor for invalido
        *
        * @param valor Valor de codigo a ser validado.
        * @throw std::invalid_argument Se o valor nao tiver 5 digitos ou contiver caracteres nao numericos.
        */
        void validar(const string&);

    public:
        /**
        * @brief Metodo publico que define o valor do codigo apos realizar validacao.
        *
        * Utiliza o metodo interno `validar` para garantir que o valor esteja correto
        * antes de armazena-lo.
        *
        * @param valor Valor de codigo a ser atribuido.
        * @throw std::invalid_argument Se o valor for invalido.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna o valor atual do codigo.
        *
        * @return string contendo Valor do codigo armazenado.
        */
        string getValor() const;
};

//---------------------------------------------------------------------
/**
* @brief Implementacao inline do metodo getValor.
*
* Retorna o valor atualmente armazenado no objeto.
*
* @return Valor do codigo como string.
*/
inline string Codigo::getValor()const { return valor; }

///---------------------------------------------------------------------
//  Dominio Codigo Negociacao     (Responsavel: Maria 231021431)
/**
* @class CodigoNeg
* @brief Classe que representa uma sequencia unica de identificacao, com exatamente 12 caracteres alfanumericos ou espacos.
*
* Esta classe e utilizada para representar um valor unico (codigo de negociacao) no sistema de investimentos,
* podendo estar associado a Ordens de compra/venda. Ela garante que os valores dos codigos de negociacao obedecam a um
* formato estrito: exatamente 12 caracteres, podendo conter letras, numeros e espacos.
*
* A classe e usada para armazenar e validar um valor (codigo de negociacao) representado como uma string, que
* deve obedecer a regras especificas de formato, garantindo que o valor interno sempre esteja em conformidade
* com as regras estabelecidas.
*
* Regras de validacao:
* - O codigo deve conter exatamente 12 caracteres.
* - Os caracteres devem ser alfanumericos (letras e/ou numeros) ou espacos em branco.
*
* Metodos disponiveis:
* - `setValor`: Metodo publico que atribui um novo valor ao codigo de negociacao, apos validacao.
* - `getValor`: Metodo publico que retorna o valor atual armazenado.
*
* Codigos invalidos resultam em uma excecao `std::invalid_argument`.
*/
class CodigoNeg {
    private:
        /**
        * @brief Quantidade fixa de caracteres exigidos para o codigo de negociacao.
        */
        static const int Q_DIGITOS = 12;

        /**
        * @brief Armazena o valor do codigo de negociacao apos validacao.
        */
        string valor;

        /**
        * @brief Metodo privado responsavel por validar se o valor fornecido atende o formato correto para o codigo de negociacao.
        *
        * Verifica se a string tem exatamente 12 caracteres e se todos sao alfanumericos ou espacos em branco.
        * Lanca uma excecao se o valor for invalido
        *
        * @param valor Valor de codigo de negociacao a ser atribuido.
        * @throws std::invalid_argument Se o valor nao tiver 12 caracteres ou contiver caracteres invalidos.
        */
        void validar(const string&);

    public:
        /**
        * @brief Metodo publico que atribui um valor ao codigo de negociacao, apos validacao.
        *
        * Utiliza o metodo `validar()` para verificar se o valor atende aos criterios definidos.
        *
        * @param valor Valor de codigo de negociacao a ser atribuido.
        * @throws std::invalid_argument Se o valor nao for valido.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna o valor atual do codigo de negociacao.
        *
        * @return string contendo o codigo de negociacao armazenado.
        */
        string getValor() const;
};

//---------------------------------------------------------------------
/**
* @brief Implementacao inline do metodo que recupera o valor do codigo de negociacao.
*
* @return string com o valor atual do codigo de negociacao.
*/
inline string CodigoNeg::getValor() const { return valor; }

///---------------------------------------------------------------------
//  Dominio CPF   (Responsavel: Karina 231006140)
/**
* @class Ncpf
* @brief Classe que representa uma sequencia unica de identificacao, com exatamente 14 caracteres no formato XXX.XXX.XXX-XX.
*
* Esta classe e utilizada para representar um CPF (Cadastro de Pessoa Fisica) no sistema de investimentos,
* conforme as regras de formatacao estabelecidas pela Receita Federal do Brasil. O CPF pode estar associado, por exemplo, a contas de usuario.
*
* O valor deve seguir rigorosamente o formato padrao: tres blocos de tres digitos separados por pontos,
* seguidos por um bloco de dois digitos separado por hifen.
*
* A classe `Ncpf` e responsavel por armazenar e validar um numero de CPF, garantindo que o valor interno esteja em conformidade
* com o formato e que os digitos verificadores sejam validos conforme regras oficiais.
*
* Regras de validacao:
* - Deve conter exatamente 14 caracteres no formato XXX.XXX.XXX-XX.
* - Apenas caracteres numericos, pontos e hifens sao permitidos nas posicoes corretas.
* - Os digitos verificadores (os dois ultimos) devem ser validos de acordo com o algoritmo oficial do CPF.
* - CPFs com todos os digitos iguais sao invalidos (ex: "111.111.111-11").
*
* Metodos disponiveis:
* - `setValor`: Metodo publico que atribui um novo valor ao CPF, apos validacao.
* - `getValor`: Metodo publico que retorna o valor atual do CPF armazenado.
*
* Lanca `std::invalid_argument` em caso de formato incorreto ou CPF invalido.
*/
class Ncpf {
    private:
        /**
        * @brief Quantidade fixa de caracteres exigidos para um CPF formatado (incluindo pontos e hifen).
        */
        static const int Q_DIGITOS = 14;

        /**
        * @brief Armazena o valor do CPF apos validacao.
        */
        string valor;

        /**
        * @brief Metodo privado responsavel por verificar se o valor fornecido atende o formato correto do CPF.
        *
        * Verifica se a string possui exatamente 14 caracteres no formato XXX.XXX.XXX-XX; se o formato contem
        * pontos e hifen nas posicoes corretas; se contem 11 digitos numericos validos; se possui digitos verificadores corretos;
        * e rejeita CPFs com todos os digitos iguais.
        * Lanca excecoes se:
        * - O tamanho for diferente de 14 caracteres;
        * - O formato for incorreto;
        * - Todos os digitos forem iguais;
        * - Os digitos verificadores nao forem validos.
        *
        * @param valor String contendo o CPF a ser validado.
        * @throws std::invalid_argument Se o CPF estiver em formato invalido ou for matematicamente incorreto.
        */
        void validar(const string&);

    public:
        /**
        * @brief Metodo publico que atribui um CPF ao objeto apos validar seu conteudo.
        *
        * Chama internamente o metodo `validar()` para garantir a conformidade do valor
        * com o formato e regras do CPF.
        *
        * @param valor CPF no formato XXX.XXX.XXX-XX.
        * @throws std::invalid_argument Se o CPF for invalido.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna o CPF armazenado.
        *
        * @return string com o CPF armazenado no formato XXX.XXX.XXX-XX.
        */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna o valor do CPF.
 *
 * @return string CPF armazenado, no formato XXX.XXX.XXX-XX.
 */
inline string Ncpf::getValor() const { return valor; }

///---------------------------------------------------------------------
//  Dominio Data   (Responsavel: Bruno 241022460)
/**
* @class Data
* @brief Classe que representa uma abstracao de tempo por meio de uma sequencia de 8 digitos numericos no formato AAAAMMDD.
*
* Esta classe e utilizada para representar uma data relevante para operacoes no sistema de investimentos, correspondendo
* ao calendario gregoriano,e composta por ano, mes e dia, conforme as regras de formatacao estabelecidas. Ela e usada para
* registrar eventos como: data de transacoes (compra/venda), vencimentos de aplicacoes, agendamentos futuros, entre outros.
*
* A classe `Data` e responsavel por armazenar uma data em formato numerico (string com 8 digitos) e garantir que
* essa data seja valida de acordo com as regras do calendario gregoriano. A data e representada no formato
* "AAAAMMDD", onde:
* - AAAA: ano (quatro digitos)
* - MM: mes (01 a 12)
* - DD: dia (dependendo do mes e se o ano e bissexto)
*
* Regras de validacao:
* - Verificacao se a string possui exatamente 8 caracteres numericos.
* - Validacao do intervalo valido para ano, mes e dia.
* - Validacao do numero de dias permitido para o mes especifico.
* - Verificacao de ano bissexto em fevereiro.
*
* Metodos disponiveis:
* - `setValor`: Metodo publico que define o valor da data apos validar.
* - `getValor`: Metodo publico que retorna a data armazenada como string.
*
* Em caso de valor invalido, lanca `std::invalid_argument`.
*/
class Data {
    private:
        /**
        * @brief Quantidade fixa de caracteres exigidos para o formato AAAAMMDD.
        */
        static const int Q_DIGITOS = 8;

        /**
        * @brief Armazena a data como string no formato AAAAMMDD.
        */
        string valor;

        /**
        * @brief Metodo privado responsavel por validar a data fornecida.
        *
        * Realiza as seguintes validacoes:
        * - Verifica se a string contem exatamente 8 caracteres numericos.
        * - Extrai ano, mes e dia da string e valida se estao dentro das faixas permitidas.
        * - Verifica se o numero de dias e compativel com o mes e se o ano e bissexto, quando aplicavel.
        *
        * @param valor String representando a data no formato AAAAMMDD.
        * @throws std::invalid_argument Se a data estiver fora do formato ou for invalida.
        */
        void validar(const string&);

    public:
        /**
        *@brief Metodo publico que define o valor da data, validando antes de armazenar.
        *
        * Utiliza o metodo `validar()` para garantir que a data esteja dentro dos criterios exigidos.
        * Armazena o valor apenas se a validacao for bem-sucedida.
        *
        * @param valor String com a data no formato AAAAMMDD.
        * @throws std::invalid_argument Se o valor nao representar uma data valida.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna a data armazenada no formato AAAAMMDD.
        *
        * @return string representando a data.
        */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna o valor data.
 *
 * @return string data armazenada, no formato AAAAMMDD
 */
inline string Data::getValor() const { return valor; }

///---------------------------------------------------------------------
// Dominio Nome   (Responsavel: Jorge 241004686)
/**
 * @class Nome
 * @brief Classe que representa a identificacao textual de uma pessoa, objeto ou entidade.
 * Pode ser composta por um ou mais componentes, com ate 20 caracteres alfanumericos e espacos.
 *
 * Esta classe e utilizada para representar nomes de usuarios ou clientes no sistema de investimentos,
 * responsaveis por transacoes ou beneficiarios. Ela garante que o nome informado siga um padrao estabelecido
 * que assegura legibilidade e integridade nos registros.
 *
 * A classe `Nome` e responsavel por armazenar uma sequencia de caracteres alfanumericos (e espacos simples),
 * com um limite de ate 20 caracteres. O nome pode conter letras, numeros e espacos, desde que obedeca as
 * restricoes de formatacao.
 *
 * Regras de validacao:
 * - O nome deve conter no maximo 20 caracteres.
 * - So sao permitidos caracteres alfanumericos (letras e digitos) e espacos.
 * - Nao sao permitidos dois espacos consecutivos.
 *
 * Metodos disponiveis:
 * - `setValor`: Metodo publico que define o valor do nome apos validar.
 * - `getValor`: Metodo publico que retorna o nome armazenado como string.
 *
 * Em caso de valor invalido, lanca `std::invalid_argument`.
 */
class Nome {
    private:
        /**
        * @brief Quantidade maxima de caracteres permitida em um nome.
        */
        static const int Q_CARATERES = 20;

        /**
        * @brief Armazena o valor do nome validado.
        */
        string valor;

        /**
        * @brief Metodo privado responsavel por verificar se o nome fornecido esta de acordo com as regras da aplicacao.
        *
        * Regras de validacao:
        * - Deve conter no maximo 20 caracteres;
        * - Apenas caracteres alfanumericos e espacos sao permitidos;
        * - Nao pode conter dois espacos consecutivos.
        *
        * @param valor Nome a ser validado.
        * @throws std::invalid_argument Se o nome for invalido.
        */
        void validar(const string&);

    public:
         /**
        * @brief Metodo publico que define o valor do nome apos validacao.
        *
        * Utiliza o metodo `validar()` para garantir que o nome esteja dentro dos criterios exigidos.
        * Armazena o valor apenas se a validacao for bem-sucedida.
        *
        * @param valor Nome a ser definido.
        * @throws std::invalid_argument Se o nome nao atender aos criterios de validacao.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna o valor atual do nome.
        *
        * @return std::string Valor do nome armazenado.
        */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna o valor do nome.
 *
 * @return std::string Nome armazenado, com ate 20 caracteres.
 */
inline string Nome::getValor() const { return valor; }

///---------------------------------------------------------------------
// Dominio Perfil   (Responsavel: Micaele 231021450)
/**
 * @class TipoPerfil
 * @brief Classe que representa o tipo de investidor de um usuario.
 *
 * Esta classe e utilizada no sistema de investimentos para indicar o perfil do investidor de
 * um usuario, levando em conta sua disposicao para correr riscos, objetivos financeiros e tempo que
 * pretende deixar o dinheiro investido. Ela permite que recomendacoes e decisoes financeiras sejam
 * personalizadas conforme o grau de tolerancia a riscos e preferencias pessoais.
 *
 * O perfil do investidor pode ser classificado em uma das seguintes categorias:
 * - "Conservador"
 * - "Moderado"
 * - "Agressivo"
 *
 * A classe `TipoPerfil` e responsavel por armazenar e validar o valor do perfil atribuido ao investidor,
 * garantindo que ele esteja entre as opcoes validas predefinidas pelo sistema.
 *
 * Regras de validacao:
 * - O valor deve ser exatamente uma das tres strings: "Conservador", "Moderado" ou "Agressivo".
 *
 * Metodos disponiveis:
 * - `setValor`: Metodo publico que define o perfil apos validar.
 * - `getValor`: Metodo publico que retorna o perfil atualmente armazenado.
 *
 * Em caso de valor invalido, lanca `std::invalid_argument`.
 */
class TipoPerfil {
    private:
        /**
        * @brief Armazena o valor do perfil de investimento validado.
        */
        string valor;

        /**
        * @brief Metodo privado responsavel por validar se o valor fornecido corresponde a um perfil permitido.
        *
        * Regras de validacao:
        * - Deve ser exatamente "Conservador", "Moderado" ou "Agressivo".
        *
        * @param valor String contendo o perfil a ser validado.
        * @throws std::invalid_argument Se o valor nao corresponder a um perfil valido.
        */
        void validar(const string&);

    public:
        /**
        * @brief Metodo publico que define o valor do perfil apos validacao.
        *
        * Utiliza o metodo `validar()` para assegurar que o valor esta dentro das opcoes permitidas.
        * O valor e armazenado somente se a validacao for bem-sucedida.
        *
        * @param valor String contendo o perfil a ser atribuido.
        * @throws std::invalid_argument Se o valor for invalido.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna o perfil armazenado.
        *
        * @return std::string Valor do perfil de investimento atual.
        */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna o valor do perfil de investimento.
 *
 * @return std::string Valor atual do perfil ("Conservador", "Moderado" ou "Agressivo").
 */
inline string TipoPerfil::getValor() const { return valor; }

///---------------------------------------------------------------------
// Dominio Dinheiro   (Responsavel: Karina 231006140)
/**
* @class Dinheiro
* @brief Classe que representa um valor monetario ou recurso financeiro aplicado, resgatado e movimentado
* pelos usuarios, em formato textual.
*
* Esta classe e utilizada no sistema de investimentos para armazenar valores que o investidor aplica
* em produtos financeiros, como acoes, titulos, fundos ou poupanca, representados no formato brasileiro
* com separadores de milhar e virgula como separador decimal.
* Ela garante que os valores informados estejam no formato correto e dentro do intervalo permitido
* para operacoes financeiras no sistema.
*
* O valor deve estar no seguinte formato: `#.###.###,##`, contendo:
* - Separadores de milhar com pontos (`.`)
* - Duas casas decimais separadas por virgula (`,`), obrigatoriamente
*
* A classe `Dinheiro` e responsavel por armazenar uma string que representa um valor monetario
* valido, realizando a validacao do formato e convertendo internamente para numero real para verificar
* se esta dentro dos limites estabelecidos.
*
* Regras de validacao:
* - O valor deve estar no formato: `#.###.###,##`
* - Deve estar no intervalo entre 0,01 e 100.000.000,00 inclusive
* - Aceita ate 8 digitos antes da virgula e 2 apos
*
* Metodos disponiveis:
* - `setValor`: Metodo publico que define o valor monetario apos validacao.
* - `getValor`: Metodo publico que retorna o valor monetario armazenado como string.
*
* Em caso de valor invalido, lanca `std::invalid_argument`.
*/
class Dinheiro {
    private:
        /**
        * @brief Valor minimo permitido (inclusive): R$ 0,01.
        */
        static const float MINIMO;

        /**
        * @brief Valor maximo permitido (inclusive): R$ 100.000.000,00.
        */
        static const float MAXIMO;

        /**
        * @brief Armazena o valor monetario validado no formato `#.###.###,##`.
        */
        string valor;

        /**
        * @brief Metodo privado responsavel por validar o formato e intervalo do valor monetario.
        *
        * Regras de validacao:
        * - O valor deve obedecer ao formato `#.###.###,##`;
        * - Deve conter apenas digitos, pontos como separador de milhar e virgula como separador decimal;
        * - O valor numerico (apos a conversao) deve estar no intervalo [0,01, 1.000.000,00].
        *
        * @param valor String representando um valor monetario a ser validado.
        * @throws std::invalid_argument Se o formato ou valor numerico estiver fora das regras.
        */
        void validar(const string&);

    public:
        /**
        * @brief Metodo publico que define o valor monetario apos validacao.
        *
        * Utiliza o metodo `validar()` para garantir que o valor esteja corretamente formatado
        * e dentro do intervalo permitido. O valor e armazenado apenas se a validacao for bem-sucedida.
        *
        * @param valor String com o valor a ser atribuido, no formato `#.###.###,##`.
        * @throws std::invalid_argument Se o valor for invalido.
        */
        void setValor(const string&);

        /**
        * @brief Metodo publico que retorna o valor monetario armazenado.
        *
        * @return std::string Valor no formato `#.###.###,##`.
        */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna o valor monetario.
 *
 * @return std::string Valor armazenado no formato `#.###.###,##`.
 */
inline string Dinheiro::getValor() const{ return valor; }

///---------------------------------------------------------------------
// Dominio Quantidade   (Responsavel: Bruno 241022460)
/**
* @class Quantidade
* @brief Classe que representa o numero inteiro  de unidades de um ativo financeiro, adquiridas,
* utilizadas ou mantidas por um investidor em transacoes financeiras.
*
* Esta classe e utilizada no sistema de investimentos para representar quantidades inteiras de ativos ou unidades
* movimentadas por usuarios, como acoes compradas ou cotas de fundos. A quantidade deve estar em um formato legivel,
* com separador de milhar, e respeitar o intervalo permitido.
*
* O valor e fornecido como uma string com numeros inteiros simples ou formato brasileiro, por exemplo:
* - "1000" ou "1.000"
* - "10000" ou "10.000"
* - "999999" ou "999.999"
*
* A classe `Quantidade` e responsavel por armazenar essa string, garantindo que esteja corretamente formatada
* e que o numero representado esteja dentro do intervalo permitido.
*
* Regras de validacao:
* - O valor deve ser um numero inteiro positivo simples (ex: "1000", "50000") ou formato brasileiro (ex: "1.000", "50.000").
* - Deve representar um numero inteiro entre 1 e 1.000.000 (inclusive).
* - Nao sao permitidas casas decimais ou outros simbolos alem de pontos como separadores de milhar.
*
* Metodos disponiveis:
* - `setValor`: Metodo publico que define a quantidade apos validacao.
* - `getValor`: Metodo publico que retorna a quantidade armazenada como string.
*
* Em caso de valor invalido, lanca `std::invalid_argument`.
*/
class Quantidade {
    private:
        /**
         * @brief Valor minimo permitido: 1.
         */
        static const int MINIMO = 1;

         /**
         * @brief Valor maximo permitido: 1.000.000.
         */
        static const int MAXIMO = 1000000;

        /**
         * @brief Armazena a quantidade validada como numero inteiro simples.
         */
        string valor;

                 /**
         * @brief Metodo privado responsavel por validar o formato e intervalo da quantidade.
         *
         * Regras de validacao:
         * - Deve ser um numero inteiro positivo simples (ex: "1000", "50000") ou formato brasileiro (ex: "1.000", "50.000");
         * - Deve conter apenas digitos e pontos como separadores de milhar;
         * - O numero deve estar no intervalo entre 1 e 1.000.000.
         *
         * @param valor String representando a quantidade a ser validada.
         * @throws std::invalid_argument Se o valor estiver em formato invalido ou fora do intervalo.
         */
        void validar(const string&);

    public:
         /**
         * @brief Metodo publico que define o valor da quantidade apos validacao.
         *
         * Utiliza o metodo `validar()` para garantir que a quantidade esteja no formato correto e no intervalo permitido.
         * O valor e armazenado somente se a validacao for bem-sucedida.
         *
         * @param valor String com a quantidade a ser definida.
         * @throws std::invalid_argument Se o valor for invalido.
         */
        void setValor(const string&);

        /**
         * @brief Metodo publico que retorna a quantidade armazenada.
         *
         * @return std::string Valor atual da quantidade como numero inteiro simples.
         */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna a quantidade armazenada.
 *
 * @return std::string Quantidade como numero inteiro simples.
 */
inline string Quantidade::getValor() const { return valor; }

///---------------------------------------------------------------------
// Dominio Senha   (Responsavel: Jorge 241004686)
/**
 * @class Senha
 * @brief Classe que representa um conjunto de caracteres usado para autenticar a identidade de um usuario no sistema de investimentos.
 *
 * A classe `Senha` e utilizada para armazenar e validar uma senha segura composta exatamente por 6 caracteres.
 * O objetivo e garantir a seguranca de autenticacao, assegurando que a senha atenda aos requisitos minimos
 * de complexidade definidos pelo sistema.
 *
 * Regras de validacao:
 * - A senha deve conter exatamente 6 caracteres.
 * - Deve incluir pelo menos:
 *    - 1 letra maiuscula
 *    - 1 letra minuscula
 *    - 1 digito numerico
 *    - 1 caractere especial permitido: `#`, `$`, `%`, `&`
 * - Todos os caracteres devem ser distintos (sem repeticao).
 * - Apenas caracteres alfanumericos e os especiais acima sao permitidos.
 *
 * Metodos disponiveis:
 * - `setValor`: Metodo publico que define a senha apos validacao.
 * - `getValor`: Metodo publico que retorna a senha armazenada como string.
 *
 * Em caso de valor invalido, lanca `std::invalid_argument`.
 */
class Senha {
    private:
        /**
         * @brief Quantidade exata de caracteres exigida na senha.
         */
        static const int Q_CARATERES = 6;

        /**
         * @brief Armazena o valor validado da senha.
         */
        string valor;

        /**
         * @brief Metodo privado que valida os criterios de seguranca da senha.
         *
         * Regras de validacao:
         * - A senha deve conter exatamente 6 caracteres;
         * - Deve incluir pelo menos:
         *    - 1 letra maiuscula
         *    - 1 letra minuscula
         *    - 1 digito numerico
         *    - 1 caractere especial permitido: `#`, `$`, `%`, `&`
         * - Nao deve conter caracteres repetidos;
         * - Apenas os caracteres alfanumericos e os especiais permitidos: `#`, `$`, `%`, `&`.
         *
         * @param valor String com a senha a ser validada.
         * @throws std::invalid_argument Se a senha violar qualquer uma das regras.
         */
        void validar(const string&);

    public:
        /**
         * @brief Metodo publico que define o valor da senha apos validacao.
         *
         * Chama o metodo `validar()` para garantir que a senha esta em conformidade
         * com os criterios de seguranca. Armazena a senha apenas se a validacao for bem-sucedida.
         *
         * @param valor String contendo a senha a ser definida.
         * @throws std::invalid_argument Se a senha for invalida.
         */
        void setValor(const string&);

        /**
         * @brief Metodo publico que retorna o valor da senha armazenada.
         *
         * @return std::string Senha atualmente armazenada.
         */
        string getValor() const;
};
//---------------------------------------------------------------------
/**
 * @brief Implementacao inline do metodo que retorna o valor da senha.
 *
 * @return std::string Senha armazenada.
 */
inline string Senha::getValor() const { return valor; }

///---------------------------------------------------------------------

#endif // DOMINIOS_HPP_INCLUDED
