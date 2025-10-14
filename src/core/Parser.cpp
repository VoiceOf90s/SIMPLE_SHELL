#include "core/Parser.h"
#include "utils/Logger.h"
#include <sstream>

Command Parser::parse(const std::string& inputLine) {
    Logger::getInstance().log("Parser", "Начало парсинга строки: \"" + inputLine + "\"");
    
    std::stringstream ss(inputLine);
    std::string token;
    Command command;
    
    // Первое слово - это программа
    if (ss >> token) {
        command.program = token;
    }
    
    // Все последующие - аргументы
    while (ss >> token) {
        command.arguments.push_back(token);
    }
    
    Logger::getInstance().log("Parser", "Парсинг завершен. Программа: '" + command.program + "', Аргументов: " + std::to_string(command.arguments.size()));
    return command;
}