#pragma once

#include "core/Command.h"
#include <string>

// Используем namespace, так как парсер не хранит состояние
// и может быть представлен набором статических функций.
namespace Parser {
    Command parse(const std::string& inputLine);
}