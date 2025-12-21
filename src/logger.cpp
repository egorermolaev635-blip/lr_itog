#include "logger.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

JsonValue LogEntry::toJson() const {
    JsonValue obj;
    obj.type = JsonType::Object;
    obj.objectValue["timestamp"] = JsonValue(timestamp);
    obj.objectValue["operation"] = JsonValue(operation);
    obj.objectValue["key"] = JsonValue(static_cast<double>(key));
    obj.objectValue["id"] = JsonValue(static_cast<double>(id));
    obj.objectValue["status"] = JsonValue(status);
    obj.objectValue["message"] = JsonValue(message);
    return obj;
}

LogEntry LogEntry::fromJson(const JsonValue& value) {
    LogEntry entry;
    if (value.type == JsonType::Object) {
        entry.timestamp = value.objectValue.at("timestamp").asString();
        entry.operation = value.objectValue.at("operation").asString();
        entry.key = static_cast<int>(value.objectValue.at("key").asNumber());
        entry.id = static_cast<int>(value.objectValue.at("id").asNumber());
        entry.status = value.objectValue.at("status").asString();
        entry.message = value.objectValue.at("message").asString();
    }
    return entry;
}

Logger::Logger(const std::string& filename) : logFile(filename) {
    // Пытаемся загрузить существующие логи
    loadFromFile();
}

void Logger::log(const std::string& operation, int key, int id,
                 const std::string& status, const std::string& message) {
    LogEntry entry;
    entry.timestamp = getCurrentTimestamp();
    entry.operation = operation;
    entry.key = key;
    entry.id = id;
    entry.status = status;
    entry.message = message;
    
    entries.push_back(entry);
}

bool Logger::saveToFile() {
    try {
        JsonValue arr;
        arr.type = JsonType::Array;
        
        for (const auto& entry : entries) {
            arr.arrayValue.push_back(entry.toJson());
        }
        
        saveJsonFile(logFile, arr, true);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при сохранении логов: " << e.what() << std::endl;
        return false;
    }
}

bool Logger::loadFromFile() {
    try {
        JsonValue data = loadJsonFile(logFile);
        entries.clear();
        
        if (data.type == JsonType::Array) {
            for (const auto& item : data.arrayValue) {
                entries.push_back(LogEntry::fromJson(item));
            }
        }
        return true;
    } catch (...) {
        // Файл не существует или пуст - это нормально
        return false;
    }
}

const std::vector<LogEntry>& Logger::getEntries() const {
    return entries;
}

void Logger::clear() {
    entries.clear();
}

size_t Logger::size() const {
    return entries.size();
}

void Logger::printToConsole() const {
    if (entries.empty()) {
        std::cout << "Логи пусты.\n";
        return;
    }
    
    // Заголовок
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                         ЛОГ ОПЕРАЦИЙ                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << std::left 
              << std::setw(20) << "Время"
              << std::setw(10) << "Операция"
              << std::setw(5) << "Ключ"
              << std::setw(5) << "ID"
              << std::setw(12) << "Статус"
              << "Сообщение\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& entry : entries) {
        std::cout << std::left
                  << std::setw(20) << entry.timestamp.substr(11, 8)  // Только время
                  << std::setw(10) << entry.operation
                  << std::setw(5) << entry.key
                  << std::setw(5) << entry.id
                  << std::setw(12) << entry.status
                  << entry.message << "\n";
    }
    
    std::cout << "\nВсего операций: " << entries.size() << "\n";
}
