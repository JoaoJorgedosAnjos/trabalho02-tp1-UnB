cmake_minimum_required(VERSION 3.15)
project(T2_TP1_241004686)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source directories
set(DOMINIOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/dominios)
set(ENTIDADES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/entidades)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/tests)

# Create library for domain classes
add_library(trabalho01_lib
    ${DOMINIOS_DIR}/dominios.cpp
    ${DOMINIOS_DIR}/dominios.hpp
    ${ENTIDADES_DIR}/entidades.cpp
    ${ENTIDADES_DIR}/entidades.hpp
    ${TESTS_DIR}/testesDominios.cpp
    ${TESTS_DIR}/testesDominios.hpp
    ${TESTS_DIR}/testesEntidades.cpp
    ${TESTS_DIR}/testesEntidades.hpp
)

# Create main executable
add_executable(${PROJECT_NAME} main.cpp)

# Link the library to the main executable
target_link_libraries(${PROJECT_NAME} PRIVATE trabalho01_lib)

# Include directories for the library
target_include_directories(trabalho01_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/dominios
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/entidades
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/tests
)

# Include directories for the main executable
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/dominios
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/entidades
    ${CMAKE_CURRENT_SOURCE_DIR}/libTrabalho01/tests
)