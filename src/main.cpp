#include "core/Shell.h"
#include "utils/Logger.h"
#include "utils/CustomException.h"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    // Включаем UTF-8 поддержку на Windows консоли
    void enableUTF8Console() {
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    }
#else
    void enableUTF8Console() {
        // На Unix уже поддерживается
    }
#endif

int main() {
    enableUTF8Console();
    
    auto& logger = Logger::getInstance();
    logger.log("main", "╔══════════════════════════════╗");
    logger.log("main", "║     SimpleShell v2.0 запущена         ║");
    logger.log("main", "║  Поддержка конвейеров и новых команд  ║");
    logger.log("main", "╚══════════════════════════════╝");

    std::cout << "\n╔════════════════════════════╗" << std::endl;
    std::cout << "║     SimpleShell v2.0 запущена        ║" << std::endl;
    std::cout << "║  Поддержка конвейеров и новых команд ║" << std::endl;
    std::cout << "║  Введите 'help' для справки          ║" << std::endl;
    std::cout << "╚═════════════════════════════╝\n" << std::endl;

    try {
        Shell myShell;
        myShell.run();
    } 
    catch (const CustomException& e) {
        logger.log("main", "КРИТИЧЕСКАЯ ОШИБКА: " + std::string(e.what()));
        std::cerr << "\nКритическая ошибка: " << e.what() << std::endl;
        return 1;
    } 
    catch (const std::exception& e) {
        logger.log("main", "СТАНДАРТНОЕ ИСКЛЮЧЕНИЕ: " + std::string(e.what()));
        std::cerr << "\nОшибка: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        logger.log("main", "Неизвестная критическая ошибка.");
        std::cerr << "\nПроизошла неизвестная ошибка." << std::endl;
        return 1;
    }

    logger.log("main", "SimpleShell штатно завершила работу.");
    std::cout << "\nДо свидания!" << std::endl;
    return 0;
}