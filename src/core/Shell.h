#pragma once

#include <string>

class Shell {
public:
    // Запускает основной цикл оболочки
    void run();

private:
    // Выводит приглашение с текущей директорией
    void printPrompt();
    
    // Читает команду из stdin
    std::string readCommand();

    bool isRunning = true;
};