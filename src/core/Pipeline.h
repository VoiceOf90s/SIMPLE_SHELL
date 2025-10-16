#pragma once

#include <string>
#include <vector>
#include "Command.h"

// Структура для представления конвейера команд
struct Pipeline {
    std::vector<Command> commands;  // Цепь команд: cmd1 | cmd2 | cmd3
    
    Pipeline() = default;
    
    explicit Pipeline(const Command& single) {
        commands.push_back(single);
    }
    
    bool isSimple() const {
        return commands.size() == 1;
    }
    
    size_t commandCount() const {
        return commands.size();
    }
};