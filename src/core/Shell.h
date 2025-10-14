#pragma once

#include <string>

class Shell {
public:
    void run();


private:
    void printPrompt();
    std::string readCommand();

    bool isRunning = true;
};