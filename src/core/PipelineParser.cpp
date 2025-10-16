#include "core/PipelineParser.h"
#include "core/Parser.h"
#include "utils/Logger.h"
#include <sstream>
#include <algorithm>

namespace PipelineParser {

    Pipeline parse(const std::string& inputLine) {
        Logger::getInstance().log("PipelineParser", 
            "Начало парсинга строки с поддержкой конвейеров: \"" + inputLine + "\"");
        
        Pipeline pipeline;
        
        // Разделяем по символу '|'
        std::stringstream ss(inputLine);
        std::string segment;
        
        while (std::getline(ss, segment, '|')) {
            // Удаляем ведущие и конечные пробелы
            segment.erase(0, segment.find_first_not_of(" \t\n\r"));
            segment.erase(segment.find_last_not_of(" \t\n\r") + 1);
            
            if (!segment.empty()) {
                Command cmd = Parser::parse(segment);
                pipeline.commands.push_back(cmd);
                Logger::getInstance().log("PipelineParser", 
                    "Добавлена команда: " + cmd.program);
            }
        }
        
        if (pipeline.commands.empty()) {
            Logger::getInstance().log("PipelineParser", "Пустой конвейер!");
        } else {
            Logger::getInstance().log("PipelineParser", 
                "Парсинг завершен. Команд в конвейере: " + 
                std::to_string(pipeline.commands.size()));
        }
        
        return pipeline;
    }

}