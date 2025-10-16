#include "core/Shell.h"
#include "core/PipelineParser.h"
#include "core/Executor.h"
#include "utils/Logger.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

void Shell::run() {
    Logger::getInstance().log("Shell", "Оболочка запущена. Введите 'help' для справки или 'exit' для выхода.");
    
    while (isRunning) {
        printPrompt();
        std::string line = readCommand();

        if (std::cin.eof()) {
            // Обработка Ctrl+D (Unix) или Ctrl+Z (Windows) для выхода
            std::cout << std::endl;
            isRunning = false;
            continue;
        }

        if (line.empty()) {
            continue;
        }

        try {
            // Парсим строку с поддержкой конвейеров
            Pipeline pipeline = PipelineParser::parse(line);
            isRunning = Executor::execute(pipeline);
        }
        catch (const std::exception& e) {
            Logger::getInstance().log("Shell", 
                std::string("Ошибка при выполнении: ") + e.what());
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }

    Logger::getInstance().log("Shell", "Цикл работы оболочки завершен.");
}


void Shell::printPrompt() {
    char cwd[1024];
    std::string promptStr = "> ";

    #ifdef _WIN32
        if (GetCurrentDirectoryA(sizeof(cwd), cwd)) {
            promptStr = std::string(cwd) + "> ";
        }
    #else
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            promptStr = std::string(cwd) + "> ";
        }
    #endif

    std::cout << promptStr;
    std::flush(std::cout);
}


std::string Shell::readCommand() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}