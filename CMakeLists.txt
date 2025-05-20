# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Имя проекта
project(Pr7 C)

# Настройка стандарта C и предупреждений
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Создание исполняемого файла
add_executable(WS src/WS.c)
