#include "core/Shell.h"
#include "utils/Logger.h"
#include "utils/CustomException.h"
#include <iostream>


int main() {
    auto& logger = Logger::getInstance();
    logger.log("main", "Приложение SimpleShell запущено.");

    
    try {
        Shell myShell;
        myShell.run();
    } catch (const CustomException& e) {
        logger.log("main", "КРИТИЧЕСКАЯ ОШИБКА: " + std::string(e.what()));
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        logger.log("main", "Неизвестная критическая ошибка.");
        std::cerr << "Произошла неизвестная критическая ошибка." << std::endl;
        return 1;
    }

    
    logger.log("main", "Приложение SimpleShell штатно завершило работу.");
    std::cout << "Выход." << std::endl;
    return 0;
}