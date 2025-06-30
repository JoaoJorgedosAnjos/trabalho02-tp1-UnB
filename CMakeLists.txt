# Versão mínima do CMake
cmake_minimum_required(VERSION 3.15)

# Nome do projeto
project(T2_TP1_241004686)

# Define o padrão C++ para 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontra todos os arquivos .cpp recursivamente dentro da pasta src
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Cria o executável principal com todos os arquivos .cpp encontrados
add_executable(${PROJECT_NAME} ${SOURCES})

# Define os diretórios onde o compilador deve procurar por arquivos de cabeçalho (.hpp)
# Qualquer subpasta dentro de 'src' que tenha arquivos .hpp deve ser listada aqui.
target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dominios"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/entidades"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interfaces"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/controladoras"
)

# Mensagem para o usuário após a configuração
message(STATUS "Projeto configurado. Para compilar, use 'make' ou seu gerador de build.")