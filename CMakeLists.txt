# Versão mínima do CMake
cmake_minimum_required(VERSION 3.15)

# Nome do projeto
project(T2_TP1_241004686)

# Define o padrão C++ para 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontra o pacote SQLite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/src/database"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tests"
    ${SQLITE3_INCLUDE_DIRS}
)

# Liga as bibliotecas SQLite3 ao executável
target_link_libraries(${PROJECT_NAME} 
    ${SQLITE3_LIBRARIES}
)

# Define as flags de compilação para SQLite3
target_compile_options(${PROJECT_NAME} PRIVATE ${SQLITE3_CFLAGS_OTHER})

# Mensagem para o usuário após a configuração
message(STATUS "Projeto configurado com SQLite3. Para compilar, use 'make' ou seu gerador de build.")
message(STATUS "SQLite3 versão: ${SQLITE3_VERSION}")
message(STATUS "SQLite3 bibliotecas: ${SQLITE3_LIBRARIES}")
message(STATUS "SQLite3 include dirs: ${SQLITE3_INCLUDE_DIRS}")