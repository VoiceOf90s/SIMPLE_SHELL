#include "core/BuiltinCommands.h"
#include "utils/Logger.h"
#include <iostream>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
    #define MAX_PATH_LEN 1024
#else
    #include <unistd.h>
    #include <cstdlib>
    #define MAX_PATH_LEN 4096
#endif

namespace BuiltinCommands {

    bool isBuiltin(const std::string& program) {
        static const char* builtins[] = {
            "exit", "cd", "pwd", "echo", "help", "clear", "export", "unset", nullptr
        };
        
        for (int i = 0; builtins[i] != nullptr; ++i) {
            if (program == builtins[i]) {
                return true;
            }
        }
        return false;
    }

    bool execute(const Command& command) {
        if (command.program == "exit") {
            Logger::getInstance().log("BuiltinCommands", "Команда 'exit'");
            return false;
        }
        
        if (command.program == "cd") {
            executeCD(command);
            return true;
        }
        
        if (command.program == "pwd") {
            executePWD();
            return true;
        }
        
        if (command.program == "echo") {
            executeEcho(command);
            return true;
        }
        
        if (command.program == "clear") {
            executeClear();
            return true;
        }
        
        if (command.program == "help") {
            executeHelp();
            return true;
        }
        
        if (command.program == "export") {
            executeExport(command);
            return true;
        }
        
        if (command.program == "unset") {
            executeUnset(command);
            return true;
        }
        
        return true;
    }

private:

    void executeCD(const Command& command) {
        if (command.arguments.empty()) {
            std::cerr << "cd: требуется аргумент" << std::endl;
            Logger::getInstance().log("BuiltinCommands::cd", "Ошибка: не указана директория");
            return;
        }

        const std::string& path = command.arguments[0];

        #ifdef _WIN32
            if (!SetCurrentDirectoryA(path.c_str())) {
                std::cerr << "cd: не удалось перейти в '" << path << "'" << std::endl;
                Logger::getInstance().log("BuiltinCommands::cd", 
                    "Ошибка смены директории на " + path);
            } else {
                Logger::getInstance().log("BuiltinCommands::cd", 
                    "Директория изменена на " + path);
            }
        #else
            if (chdir(path.c_str()) != 0) {
                perror("cd");
                Logger::getInstance().log("BuiltinCommands::cd", 
                    "Ошибка смены директории на " + path);
            } else {
                Logger::getInstance().log("BuiltinCommands::cd", 
                    "Директория изменена на " + path);
            }
        #endif
    }

    void executePWD() {
        char cwd[MAX_PATH_LEN];
        
        #ifdef _WIN32
            if (GetCurrentDirectoryA(MAX_PATH_LEN, cwd)) {
                std::cout << cwd << std::endl;
                Logger::getInstance().log("BuiltinCommands::pwd", std::string(cwd));
            } else {
                std::cerr << "pwd: ошибка получения текущей директории" << std::endl;
            }
        #else
            if (getcwd(cwd, MAX_PATH_LEN) != nullptr) {
                std::cout << cwd << std::endl;
                Logger::getInstance().log("BuiltinCommands::pwd", std::string(cwd));
            } else {
                perror("pwd");
                Logger::getInstance().log("BuiltinCommands::pwd", "Ошибка getcwd");
            }
        #endif
    }

    void executeEcho(const Command& command) {
        bool newline = true;
        size_t start = 0;
        
        // Обработка флага -n (не добавлять перевод строки)
        if (!command.arguments.empty() && command.arguments[0] == "-n") {
            newline = false;
            start = 1;
        }
        
        for (size_t i = start; i < command.arguments.size(); ++i) {
            std::cout << command.arguments[i];
            if (i < command.arguments.size() - 1) {
                std::cout << " ";
            }
        }
        
        if (newline) {
            std::cout << std::endl;
        }
        
        Logger::getInstance().log("BuiltinCommands::echo", "Выведено " + 
            std::to_string(command.arguments.size()) + " аргументов");
    }

    void executeClear() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        Logger::getInstance().log("BuiltinCommands::clear", "Экран очищен");
    }

    void executeHelp() {
        std::cout << "\n=== SimpleShell - справка ===\n" << std::endl;
        std::cout << "Встроенные команды:" << std::endl;
        std::cout << "  cd <dir>        - переход в директорию" << std::endl;
        std::cout << "  pwd             - показать текущую директорию" << std::endl;
        std::cout << "  echo <text>     - вывести текст" << std::endl;
        std::cout << "  clear           - очистить экран" << std::endl;
        std::cout << "  export VAR=val  - установить переменную окружения" << std::endl;
        std::cout << "  unset VAR       - удалить переменную окружения" << std::endl;
        std::cout << "  exit            - выход из оболочки" << std::endl;
        std::cout << "\nПоддерживаемые конвейеры:" << std::endl;
        std::cout << "  cmd1 | cmd2     - передача вывода cmd1 на вход cmd2" << std::endl;
        std::cout << "\nПримеры:" << std::endl;
        std::cout << "  ls -la | grep txt" << std::endl;
        std::cout << "  cat file.txt | grep pattern | wc -l" << std::endl;
        std::cout << "\n" << std::endl;
        
        Logger::getInstance().log("BuiltinCommands::help", "Справка выведена");
    }

    void executeExport(const Command& command) {
        if (command.arguments.empty()) {
            std::cerr << "export: требуется аргумент (VAR=value)" << std::endl;
            return;
        }
        
        const std::string& assignment = command.arguments[0];
        size_t eqPos = assignment.find('=');
        
        if (eqPos == std::string::npos) {
            std::cerr << "export: неверный формат (используйте VAR=value)" << std::endl;
            return;
        }
        
        std::string var = assignment.substr(0, eqPos);
        std::string val = assignment.substr(eqPos + 1);
        
        #ifdef _WIN32
            SetEnvironmentVariableA(var.c_str(), val.c_str());
        #else
            setenv(var.c_str(), val.c_str(), 1);
        #endif
        
        Logger::getInstance().log("BuiltinCommands::export", 
            "Установлена переменная " + var + " = " + val);
    }

    void executeUnset(const Command& command) {
        if (command.arguments.empty()) {
            std::cerr << "unset: требуется имя переменной" << std::endl;
            return;
        }
        
        const std::string& var = command.arguments[0];
        
        #ifdef _WIN32
            SetEnvironmentVariableA(var.c_str(), nullptr);
        #else
            unsetenv(var.c_str());
        #endif
        
        Logger::getInstance().log("BuiltinCommands::unset", 
            "Удалена переменная " + var);
    }

}