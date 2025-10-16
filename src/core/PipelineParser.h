#pragma once

#include "core/Pipeline.h"
#include <string>

namespace PipelineParser {
    // Парсит строку и возвращает Pipeline с одной или несколькими командами
    Pipeline parse(const std::string& inputLine);
}