# Sistema de Investimentos - Trabalho 02 TP1 UnB

## Descrição
Sistema de gerenciamento de investimentos com interface de usuário, banco de dados SQLite e cálculos financeiros precisos.

## Características Principais
- ✅ **Precisão Total**: Cálculos financeiros usando `long long` (centavos) para eliminar erros de arredondamento
- ✅ **Banco de Dados**: SQLite com nome profissional `sistema_investimentos.db`
- ✅ **Dados Históricos**: Arquivo `DADOS_HISTORICOS.txt` copiado automaticamente pelo CMake
- ✅ **Interface**: Sistema completo de cadastro, carteiras e ordens de investimento

## Compilação e Execução

### Pré-requisitos
- CMake 3.15+
- Compilador C++17
- SQLite3

### Comandos
```bash
# Compilar
cd build
cmake ..
make

# Executar
./T2_TP1_241004686
```

## Estrutura do Projeto
```
trabalho02-tp1-UnB/
├── src/                    # Código fonte
├── data/                   # Dados históricos
│   └── DADOS_HISTORICOS.txt
├── database/               # Banco de dados e schema
│   └── sistema_investimentos.db (criado automaticamente)
├── build/                  # Arquivos de compilação
│   ├── T2_TP1_241004686   # Executável
│   ├── database/          # Banco de dados (criado automaticamente)
│   └── DADOS_HISTORICOS.txt # Copiado automaticamente
└── CMakeLists.txt         # Configuração do projeto
```

## Melhorias Implementadas
- Refatoração para usar `long long` em cálculos monetários
- Nome profissional para o banco de dados
- Configuração automática do CMake para copiar arquivos necessários
- Centralização das funções de conversão monetária no DatabaseManager
