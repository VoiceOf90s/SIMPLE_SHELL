#pragma once

#include "core/Command.h"

#ifdef _WIN32
    #include <windows.h>
    #include <string>
    #include <vector>
    
    namespace Platform {
        class ProcessHandle {
        public:
            ProcessHandle(HANDLE h) : handle(h) {}
            ~ProcessHandle() { if (handle) CloseHandle(handle); }
            HANDLE get() { return handle; }
        private:
            HANDLE handle;
        };
    }
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/types.h>
#endif

namespace Executor {
    // Возвращает true, если оболочка должна продолжить работу, false - если выйти.
    bool execute(const Command& command);

private:
    void executeExternalCommand(const Command& command);
    void executeCdCommand(const Command& command);
}