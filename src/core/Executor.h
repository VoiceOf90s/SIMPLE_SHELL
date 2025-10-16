#pragma once

#include "core/Pipeline.h"

namespace Executor {
    // Выполняет конвейер команд. Возвращает true для продолжения, false для выхода
    bool execute(const Pipeline& pipeline);
}