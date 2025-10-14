#include "core/Executor.h"
#include "utils/Logger.h"
#include "utils/CustomException.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <sstream>
    #define NOMINMAX
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <cstring>
    #include <cstdlib>
#endif

namespace Executor {

    bool execute(const Command& command) {
        if (command.program.empty()) {
            return true;
        }

        Logger::getInstance().log("Executor", "Выполнение команды '" + command.program + "'");

        // Встроенные команды (кроссплатформные)
        if (command.program == "exit") {
            Logger::getInstance().log("Executor", "Получена команда 'exit'. Завершение работы.");
            return false;
        }
        
        if (command.program == "cd") {
            executeCdCommand(command);
            return true;
        }

        if (command.program == "pwd") {
            executePwdCommand();
            return true;
        }

        // Внешние команды
        executeExternalCommand(command);
        return true;
    }


    void executeCdCommand(const Command& command) {
        if (command.arguments.empty()) {
            Logger::getInstance().log("Executor::cd", "Ошибка: не указана директория для 'cd'.");
            std::cerr << "cd: ожидается аргумент" << std::endl;
            return;
        }

        #ifdef _WIN32
            if (!SetCurrentDirectoryA(command.arguments[0].c_str())) {
                Logger::getInstance().log("Executor::cd", "Ошибка при смене директории");
                std::cerr << "cd: не удалось изменить директорию на '" 
                          << command.arguments[0] << "'" << std::endl;
            }
        #else
            if (chdir(command.arguments[0].c_str()) != 0) {
                perror("cd");
                Logger::getInstance().log("Executor::cd", 
                    "Ошибка при смене директории на '" + command.arguments[0] + "'");
            }
        #endif
        else {
            Logger::getInstance().log("Executor::cd", 
                "Успешная смена директории на '" + command.arguments[0] + "'");
        }
    }


    void executePwdCommand() {
        char cwd[1024];
        #ifdef _WIN32
            if (GetCurrentDirectoryA(sizeof(cwd), cwd)) {
                std::cout << cwd << std::endl;
                Logger::getInstance().log("Executor::pwd", std::string(cwd));
            } else {
                std::cerr << "pwd: ошибка получения текущей директории" << std::endl;
            }
        #else
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                std::cout << cwd << std::endl;
                Logger::getInstance().log("Executor::pwd", std::string(cwd));
            } else {
                perror("pwd");
            }
        #endif
    }


    #ifdef _WIN32
    void executeExternalCommand(const Command& command) {
        std::string cmdLine = command.program;
        for (const auto& arg : command.arguments) {
            cmdLine += " " + arg;
        }

        Logger::getInstance().log("Executor(Windows)", 
            "Выполнение команды: " + cmdLine);

        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi = {};

        // Модифицируем строку, так как CreateProcessA требует неконстантный указатель
        std::string cmdLineCopy = cmdLine;
        
        if (!CreateProcessA(
            nullptr,
            &cmdLineCopy[0],
            nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            
            Logger::getInstance().log("Executor(Windows)", 
                "Ошибка CreateProcess для команды '" + command.program + "'");
            std::cerr << "Ошибка: не удалось запустить команду '" 
                      << command.program << "'" << std::endl;
            return;
        }

        Logger::getInstance().log("Executor(Windows)", 
            "Ожидание завершения процесса PID: " + std::to_string(pi.dwProcessId));

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        Logger::getInstance().log("Executor(Windows)", 
            "Процесс завершился с кодом " + std::to_string(exitCode));

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    #else  // POSIX системы (Linux, macOS)
    void executeExternalCommand(const Command& command) {
        pid_t pid = fork();

        if (pid < 0) {
            throw CustomException("Не удалось создать дочерний процесс (fork failed).");
        } 
        else if (pid == 0) {
            // Дочерний процесс
            std::vector<char*> argv;
            argv.push_back(const_cast<char*>(command.program.c_str()));
            for (const auto& arg : command.arguments) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);

            execvp(argv[0], argv.data());

            // Если execvp вернулся - это ошибка
            perror("execvp");
            Logger::getInstance().log("Executor(Child)", 
                "Ошибка execvp для команды '" + command.program + "'");
            _exit(EXIT_FAILURE);
        } 
        else {
            // Родительский процесс
            Logger::getInstance().log("Executor(Parent)", 
                "Ожидание завершения дочернего процесса PID: " + std::to_string(pid));
            int status;
            waitpid(pid, &status, 0);
            Logger::getInstance().log("Executor(Parent)", 
                "Дочерний процесс " + std::to_string(pid) + 
                " завершился с кодом " + std::to_string(WEXITSTATUS(status)));
        }
    }
    #endif

}