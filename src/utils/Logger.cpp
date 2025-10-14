#include "utils/Logger.h"
#include "utils/Constants.h"
#include <iostream>
#include <chrono>
#include <iomanip>


Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}


Logger::Logger() {
    logFileStream.open(constants::LOG_FILE_PATH, std::ios::out | std::ios::app);
    if (!logFileStream.is_open()) {
        // Если лог-файл не удалось открыть, выводим ошибку в консоль
        // В реальном проекте здесь могла бы быть более сложная логика
        std::cerr << "CRITICAL ERROR: Could not open log file at " << constants::LOG_FILE_PATH << std::endl;
    }
}


Logger::~Logger() {
    if (logFileStream.is_open()) {
        logFileStream.close();
    }
}


void Logger::log(const std::string& source, const std::string& message) {
    if (!logFileStream.is_open()) return;

    std::lock_guard<std::mutex> guard(logMutex);

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    logFileStream << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
                  << " [" << source << "] " // Структурирование: указываем источник лога
                  << message << std::endl;
}