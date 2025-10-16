#pragma once

#include <string>
#include <vector>

// Структура для представления одной команды
struct Command {
    std::string program;                    // Имя программы/команды
    std::vector<std::string> arguments;     // Аргументы команды
    
    Command() = default;
    
    Command(const std::string& prog) 
        : program(prog) {}
    
    bool isEmpty() const {
        return program.empty();
    }
};