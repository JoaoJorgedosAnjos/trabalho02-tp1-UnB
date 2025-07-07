# Sistema de Investimentos - Trabalho 02 TP1 UnB

## Descrição

Sistema de gerenciamento de investimentos desenvolvido em C++ moderno, com interface de usuário via console. O projeto foi construído seguindo uma arquitetura em camadas (Apresentação, Serviço, Persistência) com persistência de dados em SQLite e lógica de negócio desacoplada por meio de interfaces.

## Características Principais

-   ✅ **Arquitetura Profissional:** Design em camadas (Apresentação, Serviço, Persistência) com interfaces para desacoplamento, garantindo um código limpo e de fácil manutenção.
-   ✅ **Persistência de Dados:** Uso do SQLite para armazenamento de dados, garantindo que as informações dos usuários, carteiras e ordens persistam entre as sessões.
-   ✅ **Precisão Financeira:** Cálculos monetários realizados com `long long` (representando centavos) para eliminar erros de arredondamento de ponto flutuante.
-   ✅ **Lógica de Negócio Realista:** Criação de ordens com cálculo de preço baseado em dados de um arquivo histórico externo no formato da B3.
-   ✅ **Interface de Usuário Robusta:** Sistema de menus multinível com validação de entrada em tempo real e mensagens de ajuda contextuais.

## Estrutura do Projeto

```
trabalho02-tp1-UnB/
├── src/                    # Código fonte (.cpp, .hpp)
├── data/                   # Dados externos
│   └── DADOS_HISTORICOS.txt
├── database/               # Banco de dados
│   ├── schema.sql
│   └── sistema_investimentos.db (criado automaticamente aqui)
├── build/                  # Apenas arquivos de compilação
│   └── T2_TP1_241004686    # Executável final
└── CMakeLists.txt          # Script de build
```

## Compilação e Execução

### Pré-requisitos
-   CMake (versão 3.15 ou superior)
-   Um compilador C++17 (g++, Clang, etc.)
-   Ninja Build (`sudo apt install ninja-build`) ou make (`sudo apt install make`)
-   Biblioteca de desenvolvimento do SQLite3 (`sudo apt install libsqlite3-dev`)

### Comandos

Execute os seguintes comandos a partir do diretório raiz do projeto:

```bash
# 1. Crie e acesse o diretório de build
mkdir -p build && cd build

# 2. Configure o projeto com CMake
cmake ..

# 3. Compile o projeto com Ninja
ninja

# 4. Execute o programa (ainda de dentro da pasta 'build')
./T2_TP1_241004686
```

## Autores

-   **João Jorge** - Matrícula: 241004686