cmake_minimum_required(VERSION 3.16)

# Use um nome sem espaços e caracteres especiais
project(T2_TP1_241004686)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Listar explicitamente os arquivos fonte
set(SOURCES
    main.cpp
    dominios.cpp
    entidades.cpp
    testesDominios.cpp
    testesEntidades.cpp
)

# Listar os headers (opcional mas recomendado)
set(HEADERS
    dominios.hpp
    entidades.hpp
    testesDominios.hpp
    testesEntidades.hpp
)

# Criar o executável
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Configurações do compilador
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)