# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(SimpleShell VERSION 1.0 LANGUAGES CXX)

# Стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Директория для логов, если она не существует
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)

# Где искать заголовочные файлы
include_directories(src)

# Создание исполняемого файла из всех .cpp файлов
add_executable(SimpleShell
    src/main.cpp
    src/core/Shell.cpp
    src/core/Parser.cpp
    src/core/Executor.cpp
    src/utils/Logger.cpp
)

# Сообщение после успешной конфигурации
message(STATUS "Конфигурация завершена. Соберите проект командой 'make'.")
message(STATUS "Исполняемый файл будет создан в: ${CMAKE_BINARY_DIR}/SimpleShell")