#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include "json_parser.h"

/**
 * @file logger.h
 * @brief Логирование операций шифра
 *
 * Логирует все операции (шифрование/дешифрование) с временными метками
 * в формате JSON для последующего анализа.
 */

/**
 * @brief Структура для одной записи логирования
 */
struct LogEntry {
    std::string timestamp;      // ISO 8601 формат: YYYY-MM-DDTHH:MM:SS
    std::string operation;      // "encrypt" или "decrypt"
    int key;                    // Используемый ключ
    int id;                     // ID записи из JSON
    std::string status;         // "успешно" или "ошибка"
    std::string message;        // Дополнительная информация (ошибки, примечания)
    
    // Конвертирует в JsonValue для сохранения
    JsonValue toJson() const;
    
    // Создаёт LogEntry из JsonValue
    static LogEntry fromJson(const JsonValue& value);
};

/**
 * @brief Класс для управления логированием
 */
class Logger {
private:
    std::vector<LogEntry> entries;
    std::string logFile;
    
public:
    /**
     * @brief Конструктор логгера
     * 
     * @param filename Путь к файлу логов
     */
    Logger(const std::string& filename = "operations.log");
    
    /**
     * @brief Добавляет запись логирования
     * 
     * @param operation Тип операции ("encrypt" или "decrypt")
     * @param key Используемый ключ
     * @param id ID записи
     * @param status Статус ("успешно" или "ошибка")
     * @param message Дополнительное сообщение
     */
    void log(const std::string& operation, int key, int id, 
             const std::string& status, const std::string& message = "");
    
    /**
     * @brief Сохраняет логи в файл
     * 
     * @return true если успешно, false иначе
     */
    bool saveToFile();
    
    /**
     * @brief Загружает логи из файла
     * 
     * @return true если успешно, false иначе
     */
    bool loadFromFile();
    
    /**
     * @brief Возвращает все записи логирования
     */
    const std::vector<LogEntry>& getEntries() const;
    
    /**
     * @brief Очищает все логи
     */
    void clear();
    
    /**
     * @brief Возвращает количество записей
     */
    size_t size() const;
    
    /**
     * @brief Выводит логи в красивом формате в консоль
     */
    void printToConsole() const;
};

/**
 * @brief Получает текущее время в формате ISO 8601
 * 
 * @return Строка вида "2025-12-21T22:05:30"
 */
std::string getCurrentTimestamp();

#endif // LOGGER_H
