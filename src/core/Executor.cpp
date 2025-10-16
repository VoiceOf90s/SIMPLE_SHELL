#include "core/Executor.h"
#include "core/BuiltinCommands.h"
#include "utils/Logger.h"
#include "utils/CustomException.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
    #include <fcntl.h>
    #include <io.h>
    #define NOMINMAX
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <cstring>
#endif

namespace Executor {

    bool execute(const Pipeline& pipeline) {
        if (pipeline.commands.empty()) {
            return true;
        }

        const Command& firstCmd = pipeline.commands[0];
        
        // Если встроенная команда и это простая команда (не конвейер)
        if (pipeline.isSimple() && BuiltinCommands::isBuiltin(firstCmd.program)) {
            Logger::getInstance().log("Executor", 
                "Встроенная команда: " + firstCmd.program);
            return BuiltinCommands::execute(firstCmd);
        }

        // Если это конвейер
        if (!pipeline.isSimple()) {
            return executePipeline(pipeline);
        }

        // Внешняя команда
        executeExternalCommand(firstCmd);
        return true;
    }

    #ifdef _WIN32
    
    bool executePipeline(const Pipeline& pipeline) {
        Logger::getInstance().log("Executor(Windows)", 
            "Выполнение конвейера из " + std::to_string(pipeline.commandCount()) + 
            " команд");
        
        std::vector<HANDLE> pipes;
        std::vector<PROCESS_INFORMATION> processes;

        try {
            // Создаем трубы между командами
            for (size_t i = 0; i < pipeline.commandCount() - 1; ++i) {
                HANDLE hRead, hWrite;
                SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

                if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
                    throw CustomException("Не удалось создать pipe в Windows");
                }

                pipes.push_back(hRead);
                pipes.push_back(hWrite);
            }

            // Запускаем каждую команду
            for (size_t i = 0; i < pipeline.commandCount(); ++i) {
                const Command& cmd = pipeline.commands[i];
                
                std::string cmdLine = cmd.program;
                for (const auto& arg : cmd.arguments) {
                    cmdLine += " \"" + arg + "\"";
                }

                STARTUPINFOA si = { sizeof(STARTUPINFOA) };
                si.dwFlags = STARTF_USESTDHANDLES;

                // Установка stdin
                if (i > 0) {
                    si.hStdInput = pipes[(i - 1) * 2];  // Чтение из предыдущей трубы
                } else {
                    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
                }

                // Установка stdout
                if (i < pipeline.commandCount() - 1) {
                    si.hStdOutput = pipes[i * 2 + 1];  // Запись в трубу
                } else {
                    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
                }

                si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

                PROCESS_INFORMATION pi = {};
                std::string cmdLineCopy = cmdLine;

                if (!CreateProcessA(nullptr, &cmdLineCopy[0], nullptr, nullptr, 
                                   TRUE, 0, nullptr, nullptr, &si, &pi)) {
                    throw CustomException("Не удалось запустить команду: " + cmd.program);
                }

                processes.push_back(pi);
                Logger::getInstance().log("Executor(Windows)", 
                    "Запущена команда: " + cmd.program + " (PID: " + 
                    std::to_string(pi.dwProcessId) + ")");

                CloseHandle(pi.hThread);
            }

            // Закрываем все трубы в родительском процессе
            for (auto pipe : pipes) {
                CloseHandle(pipe);
            }

            // Ждем завершения всех процессов
            for (const auto& pi : processes) {
                WaitForSingleObject(pi.hProcess, INFINITE);
                DWORD exitCode;
                GetExitCodeProcess(pi.hProcess, &exitCode);
                Logger::getInstance().log("Executor(Windows)", 
                    "Процесс завершился с кодом " + std::to_string(exitCode));
                CloseHandle(pi.hProcess);
            }

        } catch (const std::exception& e) {
            Logger::getInstance().log("Executor(Windows)", 
                std::string("Ошибка при выполнении конвейера: ") + e.what());
            
            // Закрываем оставшиеся ресурсы
            for (auto pipe : pipes) {
                CloseHandle(pipe);
            }
            for (const auto& pi : processes) {
                TerminateProcess(pi.hProcess, 1);
                CloseHandle(pi.hProcess);
            }
            throw;
        }

        return true;
    }

    void executeExternalCommand(const Command& command) {
        std::string cmdLine = command.program;
        for (const auto& arg : command.arguments) {
            cmdLine += " \"" + arg + "\"";
        }

        Logger::getInstance().log("Executor(Windows)", 
            "Выполнение внешней команды: " + cmdLine);

        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi = {};

        std::string cmdLineCopy = cmdLine;
        
        if (!CreateProcessA(nullptr, &cmdLineCopy[0], nullptr, nullptr, FALSE, 0, 
                           nullptr, nullptr, &si, &pi)) {
            Logger::getInstance().log("Executor(Windows)", 
                "Ошибка CreateProcess для " + command.program);
            std::cerr << "Ошибка: не удалось запустить '" << command.program << "'" 
                      << std::endl;
            return;
        }

        Logger::getInstance().log("Executor(Windows)", 
            "Процесс запущен с PID: " + std::to_string(pi.dwProcessId));

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        Logger::getInstance().log("Executor(Windows)", 
            "Процесс завершился с кодом " + std::to_string(exitCode));

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    #else  // POSIX (Linux, macOS)

    bool executePipeline(const Pipeline& pipeline) {
        Logger::getInstance().log("Executor(POSIX)", 
            "Выполнение конвейера из " + std::to_string(pipeline.commandCount()) + 
            " команд");

        std::vector<pid_t> pids;
        int prevPipeRead = -1;  // Дескриптор чтения из предыдущей трубы

        for (size_t i = 0; i < pipeline.commandCount(); ++i) {
            int pipeFds[2] = {-1, -1};

            // Создаем трубу, если это не последняя команда
            if (i < pipeline.commandCount() - 1) {
                if (pipe(pipeFds) == -1) {
                    perror("pipe");
                    throw CustomException("Не удалось создать pipe");
                }
            }

            pid_t pid = fork();

            if (pid < 0) {
                throw CustomException("Не удалось создать дочерний процесс");
            }
            else if (pid == 0) {
                // Дочерний процесс

                // Перенаправляем stdin с предыдущей трубы (если не первая команда)
                if (i > 0) {
                    dup2(prevPipeRead, STDIN_FILENO);
                    close(prevPipeRead);
                }

                // Перенаправляем stdout в трубу (если не последняя команда)
                if (i < pipeline.commandCount() - 1) {
                    dup2(pipeFds[1], STDOUT_FILENO);
                    close(pipeFds[0]);
                    close(pipeFds[1]);
                }

                // Подготовляем аргументы для execvp
                const Command& cmd = pipeline.commands[i];
                std::vector<char*> argv;
                argv.push_back(const_cast<char*>(cmd.program.c_str()));
                for (const auto& arg : cmd.arguments) {
                    argv.push_back(const_cast<char*>(arg.c_str()));
                }
                argv.push_back(nullptr);

                execvp(argv[0], argv.data());

                perror("execvp");
                Logger::getInstance().log("Executor(Child)", 
                    "Ошибка execvp для " + cmd.program);
                _exit(EXIT_FAILURE);
            }
            else {
                // Родительский процесс
                pids.push_back(pid);

                // Закрываем трубы в родителе
                if (prevPipeRead != -1) {
                    close(prevPipeRead);
                }
                if (i < pipeline.commandCount() - 1) {
                    close(pipeFds[1]);
                    prevPipeRead = pipeFds[0];
                }

                Logger::getInstance().log("Executor(Parent)", 
                    "Запущена команда " + pipeline.commands[i].program + 
                    " (PID: " + std::to_string(pid) + ")");
            }
        }

        // Закрываем оставшуюся трубу
        if (prevPipeRead != -1) {
            close(prevPipeRead);
        }

        // Ждем завершения всех дочерних процессов
        int status;
        for (pid_t pid : pids) {
            waitpid(pid, &status, 0);
            Logger::getInstance().log("Executor(Parent)", 
                "Процесс " + std::to_string(pid) + " завершился с кодом " + 
                std::to_string(WEXITSTATUS(status)));
        }

        return true;
    }

    void executeExternalCommand(const Command& command) {
        pid_t pid = fork();

        if (pid < 0) {
            throw CustomException("Не удалось создать дочерний процесс");
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

            perror("execvp");
            Logger::getInstance().log("Executor(Child)", 
                "Ошибка execvp для " + command.program);
            _exit(EXIT_FAILURE);
        }
        else {
            // Родительский процесс
            Logger::getInstance().log("Executor(Parent)", 
                "Ожидание завершения процесса PID: " + std::to_string(pid));
            
            int status;
            waitpid(pid, &status, 0);
            Logger::getInstance().log("Executor(Parent)", 
                "Процесс завершился с кодом " + std::to_string(WEXITSTATUS(status)));
        }
    }

    #endif