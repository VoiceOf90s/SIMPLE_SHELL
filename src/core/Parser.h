#pragma once

#include "core/Command.h"
#include <string>

// Простой парсер одной команды (без конвейеров)
// Конвейеры обрабатывает PipelineParser
namespace Parser {
    // Парсит простую строку в одну команду
    // Разбивает по пробелам: первое слово - программа, остальное - аргументы
    Command parse(const std::string& inputLine);
}