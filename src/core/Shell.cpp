#include "core/Shell.h"
#include "core/Parser.h"
#include "core/Executor.h"
#include "utils/Logger.h"
#include <iostream>


void Shell::run() {
    Logger::getInstance().log("Shell", "Оболочка запущена. Введите 'exit' для выхода.");
    
    while (isRunning) {
        printPrompt();
        std::string line = readCommand();

        if (std::cin.eof()) {
            // Обработка Ctrl+D для выхода
            std::cout << std::endl;
            isRunning = false;
            continue;
        }

        auto command = Parser::parse(line);
        isRunning = Executor::execute(command);
    }

    Logger::getInstance().log("Shell", "Цикл работы оболочки завершен.");
}


void Shell::printPrompt() {
    // В реальном шелле здесь бы выводился текущий путь, имя пользователя и т.д.
    std::cout << "> ";
    std::flush(std::cout);
}


std::string Shell::readCommand() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}