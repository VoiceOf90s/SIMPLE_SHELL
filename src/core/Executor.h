#pragma once

#include "core/Command.h"

namespace Executor {
    // Возвращает true, если оболочка должна продолжить работу, false - если выйти.
    bool execute(const Command& command);


private:
    void executeExternalCommand(const Command& command);
    void executeCdCommand(const Command& command);
}