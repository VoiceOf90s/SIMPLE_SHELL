#pragma once

#include "core/Command.h"
#include <string>

namespace BuiltinCommands {
    // ѕровер€ет, €вл€етс€ ли команда встроенной
    bool isBuiltin(const std::string& program);

    // ¬озвращает true, если оболочка должна продолжить работу
    bool execute(const Command& command);
}#pragma once

#include "core/Command.h"
#include <string>

namespace BuiltinCommands {
    // ѕровер€ет, €вл€етс€ ли команда встроенной
    bool isBuiltin(const std::string& program);

    // ¬озвращает true, если оболочка должна продолжить работу
    bool execute(const Command& command);
}