cmake_minimum_required(VERSION 3.10)

project(SimpleShell VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Кроссплатформная поддержка
if (WIN32)
    message(STATUS "Windows платформа обнаружена. Используется Windows API.")
elseif (UNIX AND NOT APPLE)
    message(STATUS "Linux платформа обнаружена.")
elseif (APPLE)
    message(STATUS "macOS платформа обнаружена.")
endif()

# Директория для логов
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)

# Где искать заголовочные файлы
include_directories(src)

# Создание исполняемого файла
add_executable(SimpleShell
    src/main.cpp
    src/core/Shell.cpp
    src/core/Parser.cpp
    src/core/PipelineParser.cpp
    src/core/Executor.cpp
    src/core/BuiltinCommands.cpp
    src/utils/Logger.cpp
)

# Линковка платформо-зависимых библиотек
if (WIN32)
    target_link_libraries(SimpleShell PUBLIC ws2_32)
endif()

# Сообщение после успешной конфигурации
message(STATUS "Конфигурация завершена для платформы: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Соберите проект командой 'cmake --build .' или 'make'")
message(STATUS "Исполняемый файл будет в: ${CMAKE_BINARY_DIR}/SimpleShell")