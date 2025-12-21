cmake_minimum_required(VERSION 3.10)
project(caesar_cipher VERSION 1.0.0 LANGUAGES CXX)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Директории
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Добавляем include директорию
include_directories(${INCLUDE_DIR})

# Исходные файлы
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/cipher.cpp
    ${SRC_DIR}/json_parser.cpp
    ${SRC_DIR}/logger.cpp
)

# Создание главного исполняемого файла
add_executable(caesar_cipher ${SOURCES})

# Опционально: включаем тесты
enable_testing()

# Юнит-тесты
add_executable(test_cipher 
    ${SRC_DIR}/cipher.cpp
    ${SRC_DIR}/json_parser.cpp
    ${SRC_DIR}/logger.cpp
    ${CMAKE_SOURCE_DIR}/tests/test_cipher.cpp
)
add_test(NAME CipherTest COMMAND test_cipher)

add_executable(test_json 
    ${SRC_DIR}/json_parser.cpp
    ${CMAKE_SOURCE_DIR}/tests/test_json.cpp
)
add_test(NAME JsonTest COMMAND test_json)

# Вывод информации о конфигурации
message(STATUS "Конфигурация сборки завершена")
message(STATUS "  Компилятор: ${CMAKE_CXX_COMPILER}")
message(STATUS "  Стандарт C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Флаги: ${CMAKE_CXX_FLAGS}")
