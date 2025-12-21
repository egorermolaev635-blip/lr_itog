#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @file json_parser.h
 * @brief Минимальный парсер JSON для шифра Цезаря
 *
 * Реализует чтение и запись базового подмножества JSON:
 * - объекты {key: value}
 * - массивы [value, value]
 * - строки с экранированием
 * - числа
 * - логические значения и null
 */

enum class JsonType {
    Null,       // null
    Boolean,    // true/false
    Number,     // целое или дробное число
    String,     // строка
    Array,      // массив
    Object      // объект
};

/**
 * @brief Простое представление JSON значения
 */
struct JsonValue {
    JsonType type;
    
    // В зависимости от type используется один из полей:
    bool boolValue;                                    // для Boolean
    double numberValue;                                // для Number
    std::string stringValue;                           // для String
    std::vector<JsonValue> arrayValue;                 // для Array
    std::map<std::string, JsonValue> objectValue;      // для Object
    
    // Конструкторы
    JsonValue() : type(JsonType::Null) {}
    JsonValue(bool b) : type(JsonType::Boolean), boolValue(b) {}
    JsonValue(double n) : type(JsonType::Number), numberValue(n) {}
    JsonValue(const std::string& s) : type(JsonType::String), stringValue(s) {}
    
    // Получить значение как строку (для удобства)
    std::string asString() const;
    double asNumber() const;
    bool asBoolean() const;
};

/**
 * @brief Парсит JSON строку и возвращает корневое значение
 *
 * @param jsonStr JSON-строка для парсинга
 * @return JsonValue с результатом парсинга
 * @throw std::runtime_error если JSON невалидный (с указанием позиции)
 */
JsonValue parseJson(const std::string& jsonStr);

/**
 * @brief Сохраняет JsonValue в JSON строку
 *
 * @param value JSON значение для сохранения
 * @param pretty Если true, добавляет красивое форматирование
 * @return JSON-строка
 */
std::string jsonToString(const JsonValue& value, bool pretty = true);

/**
 * @brief Загружает JSON из файла
 *
 * @param filename Путь к файлу
 * @return JsonValue с содержимым файла
 * @throw std::runtime_error если файл не может быть прочитан или JSON невалидный
 */
JsonValue loadJsonFile(const std::string& filename);

/**
 * @brief Сохраняет JsonValue в файл
 *
 * @param filename Путь к файлу для сохранения
 * @param value JSON значение для сохранения
 * @param pretty Если true, добавляет красивое форматирование
 * @throw std::runtime_error если файл не может быть записан
 */
void saveJsonFile(const std::string& filename, const JsonValue& value, bool pretty = true);

/**
 * @brief Проверяет валидность JSON парсинга
 *
 * @param jsonStr JSON-строка для проверки
 * @param errorMsg Если парсинг не удался, здесь будет сообщение об ошибке
 * @return true если JSON валиден, false иначе
 */
bool isValidJson(const std::string& jsonStr, std::string& errorMsg);

#endif // JSON_PARSER_H
