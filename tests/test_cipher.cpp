#include "cipher.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

// Счётчики тестов
int testsRun = 0;
int testsPassed = 0;

void assert_equal(const string& actual, const string& expected, const string& testName) {
    testsRun++;
    if (actual == expected) {
        cout << "✓ " << testName << endl;
        testsPassed++;
    } else {
        cout << "✗ " << testName << endl;
        cout << "  Ожидалось: \"" << expected << "\"" << endl;
        cout << "  Получено:  \"" << actual << "\"" << endl;
    }
}

void assert_throws(const function<void()>& func, const string& testName) {
    testsRun++;
    try {
        func();
        cout << "✗ " << testName << " (исключение не выброшено)" << endl;
    } catch (const exception&) {
        cout << "✓ " << testName << endl;
        testsPassed++;
    }
}

int main() {
    cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║                 ТЕСТИРОВАНИЕ ФУНКЦИЙ ШИФРА                   ║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    // === Тесты шифрования английского текста ===
    cout << "1. ШИФРОВАНИЕ АНГЛИЙСКОГО ТЕКСТА\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_equal(encryptCaesar("Hello", 1, 'E'), "Ifmmp", "encryptCaesar('Hello', 1, 'E')");
    assert_equal(encryptCaesar("Hello", 5, 'E'), "Mjqqt", "encryptCaesar('Hello', 5, 'E')");
    assert_equal(encryptCaesar("ABC", 1, 'E'), "BCD", "encryptCaesar('ABC', 1, 'E')");
    assert_equal(encryptCaesar("XYZ", 3, 'E'), "ABC", "encryptCaesar('XYZ', 3, 'E')");
    assert_equal(encryptCaesar("Hello, World!", 7, 'E'), "Olmmp, Dsvmk!", "encryptCaesar с пунктуацией");
    assert_equal(encryptCaesar("", 5, 'E'), "", "encryptCaesar пустой строки");
    assert_equal(encryptCaesar("123!@#", 5, 'E'), "123!@#", "encryptCaesar только спецсимволы");
    
    // === Тесты дешифрования английского текста ===
    cout << "\n2. ДЕШИФРОВАНИЕ АНГЛИЙСКОГО ТЕКСТА\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_equal(decryptCaesar("Ifmmp", 1, 'E'), "Hello", "decryptCaesar('Ifmmp', 1, 'E')");
    assert_equal(decryptCaesar("Mjqqt", 5, 'E'), "Hello", "decryptCaesar('Mjqqt', 5, 'E')");
    assert_equal(decryptCaesar("BCD", 1, 'E'), "ABC", "decryptCaesar('BCD', 1, 'E')");
    
    // === Циклическое шифрование ===
    cout << "\n3. ЦИКЛИЧЕСКОЕ ШИФРОВАНИЕ\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_equal(encryptCaesar("XYZ", 3, 'E'), "ABC", "XYZ + 3 = ABC");
    assert_equal(encryptCaesar("ABC", 25, 'E'), "ZAB", "ABC + 25 = ZAB");
    assert_equal(decryptCaesar("ABC", 1, 'E'), "ZAB", "ABC - 1 = ZAB");
    
    // === Сохранение регистра ===\n    cout << "\n4. СОХРАНЕНИЕ РЕГИСТРА\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_equal(encryptCaesar("AaBbCc", 1, 'E'), "BbCcDd", "Сохранение регистра");
    assert_equal(encryptCaesar("HELLO", 1, 'E'), "IFMMP", "Все заглавные");
    assert_equal(encryptCaesar("hello", 1, 'E'), "ifmmp", "Все прописные");
    
    // === Тесты русского текста ===
    cout << "\n5. ШИФРОВАНИЕ РУССКОГО ТЕКСТА\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_equal(encryptCaesar("абв", 1, 'R'), "бвг", "encryptCaesar русского");
    assert_equal(encryptCaesar("Привет", 1, 'R'), "Сулвhg", "encryptCaesar 'Привет' с ключом 1");
    assert_equal(encryptCaesar("ЯЮЭ", 1, 'R'), "АБВ", "Циклический сдвиг русского");
    
    cout << "\n6. ДЕШИФРОВАНИЕ РУССКОГО ТЕКСТА\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_equal(decryptCaesar("бвг", 1, 'R'), "абв", "decryptCaesar русского");
    assert_equal(decryptCaesar("Сулвhg", 1, 'R'), "Привет", "decryptCaesar 'Сулвhg'");
    
    // === Тесты валидации ключа ===
    cout << "\n7. ВАЛИДАЦИЯ КЛЮЧА\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    cout << (isValidKey(1, 'E') ? "✓" : "✗") << " isValidKey(1, 'E') = true" << endl;
    testsRun++; if (isValidKey(1, 'E')) testsPassed++;
    
    cout << (isValidKey(25, 'E') ? "✓" : "✗") << " isValidKey(25, 'E') = true" << endl;
    testsRun++; if (isValidKey(25, 'E')) testsPassed++;
    
    cout << (!isValidKey(0, 'E') ? "✓" : "✗") << " isValidKey(0, 'E') = false" << endl;
    testsRun++; if (!isValidKey(0, 'E')) testsPassed++;
    
    cout << (!isValidKey(26, 'E') ? "✓" : "✗") << " isValidKey(26, 'E') = false" << endl;
    testsRun++; if (!isValidKey(26, 'E')) testsPassed++;
    
    cout << (isValidKey(1, 'R') ? "✓" : "✗") << " isValidKey(1, 'R') = true" << endl;
    testsRun++; if (isValidKey(1, 'R')) testsPassed++;
    
    cout << (isValidKey(32, 'R') ? "✓" : "✗") << " isValidKey(32, 'R') = true" << endl;
    testsRun++; if (isValidKey(32, 'R')) testsPassed++;
    
    cout << (!isValidKey(33, 'R') ? "✓" : "✗") << " isValidKey(33, 'R') = false" << endl;
    testsRun++; if (!isValidKey(33, 'R')) testsPassed++;
    
    // === Обработка исключений ===
    cout << "\n8. ОБРАБОТКА ОШИБОК\n";
    cout << "─────────────────────────────────────────────────────────────\n";
    
    assert_throws([](){ encryptCaesar("text", 0, 'E'); }, "encryptCaesar с ключом 0");
    assert_throws([](){ encryptCaesar("text", 26, 'E'); }, "encryptCaesar с ключом 26");
    assert_throws([](){ encryptCaesar("text", 33, 'R'); }, "encryptCaesar с ключом 33");
    
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
