#include "core/Executor.h"
#include "utils/Logger.h"
#include "utils/CustomException.h"
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>

namespace Executor {

    bool execute(const Command& command) {
        if (command.program.empty()) {
            return true;
        }

        Logger::getInstance().log("Executor", "Выполнение команды '" + command.program + "'");

        if (command.program == "exit") {
            Logger::getInstance().log("Executor", "Получена команда 'exit'. Завершение работы.");
            return false;
        }
        if (command.program == "cd") {
            executeCdCommand(command);
            return true;
        }

        executeExternalCommand(command);
        return true;
    }


    void executeCdCommand(const Command& command) {
        if (command.arguments.empty()) {
            Logger::getInstance().log("Executor::cd", "Ошибка: не указана директория для 'cd'.");
            std::cerr << "cd: ожидается аргумент" << std::endl;
            return;
        }

        if (chdir(command.arguments[0].c_str()) != 0) {
            perror("cd");
            Logger::getInstance().log("Executor::cd", "Ошибка при смене директории на '" + command.arguments[0] + "'");
        } else {
            Logger::getInstance().log("Executor::cd", "Успешная смена директории на '" + command.arguments[0] + "'");
        }
    }


    void executeExternalCommand(const Command& command) {
        pid_t pid = fork();

        if (pid < 0) {
            throw CustomException("Не удалось создать дочерний процесс (fork failed).");
        } else if (pid == 0) {

            // execvp требует массив указателей на char (C-style strings),
            // завершающийся NULL. Готовим его из std::string.
            std::vector<char*> argv;
            argv.push_back(const_cast<char*>(command.program.c_str()));
            for (const auto& arg : command.arguments) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);

            execvp(argv[0], argv.data());

            perror("execvp");
            Logger::getInstance().log("Executor(Child)", "Ошибка execvp для команды '" + command.program + "'");
            _exit(EXIT_FAILURE);
        } else {
            Logger::getInstance().log("Executor(Parent)", "Ожидание завершения дочернего процесса PID: " + std::to_string(pid));
            int status;
            waitpid(pid, &status, 0);
            Logger::getInstance().log("Executor(Parent)", "Дочерний процесс " + std::to_string(pid) + " завершился с кодом " + std::to_string(status));
        }
    }
}