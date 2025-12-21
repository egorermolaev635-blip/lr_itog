#include "json_parser.h"
#include <iostream>
#include <string>

using namespace std;

int testsRun = 0;
int testsPassed = 0;

void testParse(const string& jsonStr, const string& testName) {
    testsRun++;
    try {
        JsonValue result = parseJson(jsonStr);
        cout << "✓ " << testName << endl;
        testsPassed++;
    } catch (const exception& e) {
        cout << "✗ " << testName << " (ошибка: " << e.what() << ")" << endl;
    }
}

void testParseInvalid(const string& jsonStr, const string& testName) {
    testsRun++;
    try {
        JsonValue result = parseJson(jsonStr);
        cout << "✗ " << testName << " (должна быть ошибка)" << endl;
    } catch (const exception&) {
        cout << "✓ " << testName << endl;
        testsPassed++;
    }
}

int main() {
    cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    ТЕСТИРОВАНИЕ JSON ПАРСЕРА                  ║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    // === Тесты валидного JSON ===
    cout << "1. ПАРСИНГ ВАЛИДНОГО JSON\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    testParse("null", "parseJson('null')");
    testParse("true", "parseJson('true')");
    testParse("false", "parseJson('false')");
    testParse("42", "parseJson('42')");
    testParse("3.14", "parseJson('3.14')");
    testParse("\"Hello\"", "parseJson('\"Hello\"')");
    testParse("[]", "parseJson('[]') - пустой массив");
    testParse("[1, 2, 3]", "parseJson('[1, 2, 3]')");
    testParse("[\"a\", \"b\"]", "parseJson('[\"a\", \"b\"]')");
    testParse("{}", "parseJson('{}') - пустой объект");
    testParse("{\"key\": \"value\"}", "parseJson('{\"key\": \"value\"}')");
    testParse("{\"id\": 1, \"name\": \"test\"}", "parseJson объект с несколькими полями");
    testParse("[{\"id\": 1, \"name\": \"test\"}]", "parseJson массив объектов");
    
    // === Тесты с пробелами ===
    cout << "\n2. ПАРСИНГ С ПРОБЕЛАМИ\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    testParse("  null  ", "parseJson с пробелами вокруг null");
    testParse("[ 1 , 2 , 3 ]", "parseJson с пробелами в массиве");
    testParse("{ \"key\" : \"value\" }", "parseJson с пробелами в объекте");
    
    // === Тесты экранирования ===
    cout << "\n3. ПАРСИНГ ЭКРАНИРОВАНИЯ В СТРОКАХ\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    testParse("\"hello\\\\world\"", "parseJson с экранированием \\\\");
    testParse("\"line1\\nline2\"", "parseJson с экранированием \\n");
    testParse("\"tab\\there\"", "parseJson с экранированием \\t");
    testParse("\"quote\\\"here\"", "parseJson с экранированием \\\"");
    
    // === Тесты невалидного JSON ===\n    cout << "\n4. ОБРАБОТКА НЕВАЛИДНОГО JSON\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    testParseInvalid("{", "parseJson незакрытый объект");
    testParseInvalid("[", "parseJson незакрытый массив");
    testParseInvalid("\"", "parseJson незакрытая строка");
    testParseInvalid("{key: value}", "parseJson без кавычек вокруг ключа");
    testParseInvalid("[1, 2, ]", "parseJson запятая перед закрытием");
    testParseInvalid("undefined", "parseJson undefined не поддерживается");
    
    // === Тесты сериализации ===
    cout << "\n5. СЕРИАЛИЗАЦИЯ JSON\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    JsonValue val1(std::string("test"));
    string json1 = jsonToString(val1, false);
    cout << (json1 == "\"test\"" ? "✓" : "✗") << " Сериализация строки" << endl;
    testsRun++; if (json1 == "\"test\"") testsPassed++;
    
    JsonValue val2(42.0);
    string json2 = jsonToString(val2, false);
    cout << (json2.find("42") != string::npos ? "✓" : "✗") << " Сериализация числа" << endl;
    testsRun++; if (json2.find("42") != string::npos) testsPassed++;
    
    JsonValue val3;
    val3.type = JsonType::Array;
    val3.arrayValue.push_back(JsonValue(1.0));
    val3.arrayValue.push_back(JsonValue(2.0));
    string json3 = jsonToString(val3, false);
    cout << (json3.find("[") != string::npos && json3.find("]") != string::npos ? "✓" : "✗") 
         << " Сериализация массива" << endl;
    testsRun++; if (json3.find("[") != string::npos && json3.find("]") != string::npos) testsPassed++;
    
    // === Результаты ===
    cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║                        РЕЗУЛЬТАТЫ ТЕСТОВ                      ║\n";
    cout << "╠════════════════════════════════════════════════════════════════╣\n";
    cout << "║ Всего тестов:        " << testsRun << " " << string(30 - to_string(testsRun).length(), ' ') << "║\n";
    cout << "║ Пройдено:            " << testsPassed << " " << string(30 - to_string(testsPassed).length(), ' ') << "║\n";
    cout << "║ Не пройдено:         " << (testsRun - testsPassed) << " " << string(30 - to_string(testsRun - testsPassed).length(), ' ') << "║\n";
    cout << "║ Процент успеха:      " << (testsPassed * 100 / testsRun) << "% " << string(28 - to_string(testsPassed * 100 / testsRun).length(), ' ') << "║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    return (testsPassed == testsRun) ? 0 : 1;
}
