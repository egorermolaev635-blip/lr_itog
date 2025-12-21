#include "json_parser.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>

// === Реализация JsonValue ===

std::string JsonValue::asString() const {
    if (type == JsonType::String) return stringValue;
    if (type == JsonType::Number) return std::to_string(numberValue);
    if (type == JsonType::Boolean) return boolValue ? "true" : "false";
    return "";
}

double JsonValue::asNumber() const {
    if (type == JsonType::Number) return numberValue;
    try {
        return std::stod(stringValue);
    } catch (...) {
        return 0.0;
    }
}

bool JsonValue::asBoolean() const {
    return type == JsonType::Boolean ? boolValue : false;
}

// === Парсинг JSON ===

class JsonParser {
private:
    std::string input;
    size_t pos;
    
    void skipWhitespace() {
        while (pos < input.length() && std::isspace(input[pos])) {
            pos++;
        }
    }
    
    char peek() {
        skipWhitespace();
        return pos < input.length() ? input[pos] : '\0';
    }
    
    char consume() {
        skipWhitespace();
        if (pos >= input.length()) throw std::runtime_error("Неожиданный конец JSON");
        return input[pos++];
    }
    
    std::string parseString() {
        if (consume() != '"') throw std::runtime_error("Ожидается кавычка");
        
        std::string result;
        while (pos < input.length() && input[pos] != '"') {
            if (input[pos] == '\\') {
                pos++;
                if (pos >= input.length()) throw std::runtime_error("Неполное экранирование");
                
                switch (input[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    default: result += input[pos];
                }
                pos++;
            } else {
                result += input[pos++];
            }
        }
        
        if (pos >= input.length()) throw std::runtime_error("Незакрытая строка");
        pos++;  // Пропускаем закрывающую кавычку
        return result;
    }
    
    JsonValue parseNumber() {
        size_t start = pos;
        
        if (input[pos] == '-') pos++;
        
        while (pos < input.length() && std::isdigit(input[pos])) pos++;
        
        if (pos < input.length() && input[pos] == '.') {
            pos++;
            while (pos < input.length() && std::isdigit(input[pos])) pos++;
        }
        
        std::string numStr = input.substr(start, pos - start);
        try {
            return JsonValue(std::stod(numStr));
        } catch (...) {
            throw std::runtime_error("Невалидное число: " + numStr);
        }
    }
    
    JsonValue parseArray() {
        consume();  // '['
        JsonValue arr;
        arr.type = JsonType::Array;
        
        if (peek() == ']') {
            consume();
            return arr;
        }
        
        while (true) {
            arr.arrayValue.push_back(parseValue());
            
            if (peek() == ']') {
                consume();
                break;
            }
            
            if (consume() != ',') {
                throw std::runtime_error("Ожидается ',' или ']' в массиве");
            }
        }
        
        return arr;
    }
    
    JsonValue parseObject() {
        consume();  // '{'
        JsonValue obj;
        obj.type = JsonType::Object;
        
        if (peek() == '}') {
            consume();
            return obj;
        }
        
        while (true) {
            std::string key = parseString();
            
            if (consume() != ':') {
                throw std::runtime_error("Ожидается ':' после ключа объекта");
            }
            
            obj.objectValue[key] = parseValue();
            
            if (peek() == '}') {
                consume();
                break;
            }
            
            if (consume() != ',') {
                throw std::runtime_error("Ожидается ',' или '}' в объекте");
            }
        }
        
        return obj;
    }
    
public:
    JsonParser(const std::string& json) : input(json), pos(0) {}
    
    JsonValue parse() {
        JsonValue result = parseValue();
        skipWhitespace();
        if (pos < input.length()) {
            throw std::runtime_error("Дополнительные символы после JSON на позиции " + std::to_string(pos));
        }
        return result;
    }
    
    JsonValue parseValue() {
        skipWhitespace();
        
        if (pos >= input.length()) {
            throw std::runtime_error("Неожиданный конец JSON");
        }
        
        char ch = input[pos];
        
        if (ch == '"') return JsonValue(parseString());
        if (ch == '[') return parseArray();
        if (ch == '{') return parseObject();
        if (ch == 't' || ch == 'f') {
            if (input.substr(pos, 4) == "true") {
                pos += 4;
                return JsonValue(true);
            }
            if (input.substr(pos, 5) == "false") {
                pos += 5;
                return JsonValue(false);
            }
            throw std::runtime_error("Неожиданное значение");
        }
        if (ch == 'n') {
            if (input.substr(pos, 4) == "null") {
                pos += 4;
                return JsonValue();
            }
            throw std::runtime_error("Неожиданное значение");
        }
        if (std::isdigit(ch) || ch == '-') {
            return parseNumber();
        }
        
        throw std::runtime_error("Неожиданный символ в JSON: " + std::string(1, ch));
    }
};

// === Публичные функции ===

JsonValue parseJson(const std::string& jsonStr) {
    try {
        JsonParser parser(jsonStr);
        return parser.parse();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Ошибка парсинга JSON: ") + e.what());
    }
}

std::string jsonToString(const JsonValue& value, bool pretty) {
    std::ostringstream oss;
    
    std::function<void(const JsonValue&, int)> stringify = 
        [&](const JsonValue& val, int indent) {
        std::string indentStr(indent * 2, ' ');
        std::string nextIndentStr((indent + 1) * 2, ' ');
        
        switch (val.type) {
            case JsonType::Null:
                oss << "null";
                break;
            case JsonType::Boolean:
                oss << (val.boolValue ? "true" : "false");
                break;
            case JsonType::Number:
                oss << val.numberValue;
                break;
            case JsonType::String: {
                oss << '"';
                for (char ch : val.stringValue) {
                    if (ch == '"') oss << "\\\"";
                    else if (ch == '\\') oss << "\\\\";
                    else if (ch == '\n') oss << "\\n";
                    else if (ch == '\t') oss << "\\t";
                    else if (ch == '\r') oss << "\\r";
                    else oss << ch;
                }
                oss << '"';
                break;
            }
            case JsonType::Array:
                oss << "[";
                for (size_t i = 0; i < val.arrayValue.size(); i++) {
                    if (pretty && i > 0) oss << "\n" << nextIndentStr;
                    else if (i > 0) oss << ", ";
                    stringify(val.arrayValue[i], indent + 1);
                    if (pretty && i < val.arrayValue.size() - 1) oss << ",";
                }
                if (pretty && !val.arrayValue.empty()) oss << "\n" << indentStr;
                oss << "]";
                break;
            case JsonType::Object: {
                oss << "{";
                size_t i = 0;
                for (const auto& [key, obj_val] : val.objectValue) {
                    if (pretty) oss << "\n" << nextIndentStr;
                    else if (i > 0) oss << ", ";
                    oss << "\"" << key << "\": ";
                    stringify(obj_val, indent + 1);
                    if (pretty && i < val.objectValue.size() - 1) oss << ",";
                    i++;
                }
                if (pretty && !val.objectValue.empty()) oss << "\n" << indentStr;
                oss << "}";
                break;
            }
        }
    };
    
    stringify(value, 0);
    return oss.str();
}

JsonValue loadJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseJson(buffer.str());
}

void saveJsonFile(const std::string& filename, const JsonValue& value, bool pretty) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось создать файл: " + filename);
    }
    
    file << jsonToString(value, pretty);
    if (!file) {
        throw std::runtime_error("Ошибка записи в файл: " + filename);
    }
}

bool isValidJson(const std::string& jsonStr, std::string& errorMsg) {
    try {
        parseJson(jsonStr);
        return true;
    } catch (const std::exception& e) {
        errorMsg = e.what();
        return false;
    }
}
