#pragma once

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance();

    void log(const std::string& source, const std::string& message);

private:
    Logger();
    ~Logger();

    std::ofstream logFileStream;
    std::mutex logMutex;
};