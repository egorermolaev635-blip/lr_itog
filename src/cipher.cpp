#include "cipher.h"
#include <stdexcept>
#include <cctype>
#include <random>
#include <algorithm>

// === Константы для алфавитов ===
const std::string ENGLISH_LOWER = "abcdefghijklmnopqrstuvwxyz";
const std::string ENGLISH_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Русский алфавит: а..я (33 буквы, исключая ё)
const std::string RUSSIAN_LOWER = "абвгдежзийклмнопрстуфхцчшщъыьэюя";
const std::string RUSSIAN_UPPER = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

// === Вспомогательные функции ===

/**
 * @brief Шифрует символ с использованием алфавита
 */
char shiftChar(char ch, int key, const std::string& alphabet) {
    size_t pos = alphabet.find(ch);
    if (pos == std::string::npos) {
        return ch;  // Символ не в алфавите, не меняем
    }
    
    int size = alphabet.length();
    int newPos = (pos + key) % size;
    return alphabet[newPos];
}

/**
 * @brief Проверяет, является ли символ буквой указанного языка
 */
bool isLanguageLetter(char ch, char lang) {
    if (lang == 'E' || lang == 'e') {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    } else if (lang == 'R' || lang == 'r') {
        // Проверяем против русского алфавита
        return RUSSIAN_LOWER.find(ch) != std::string::npos ||
               RUSSIAN_UPPER.find(ch) != std::string::npos;
    }
    return false;
}

// === Основные функции ===

std::string encryptCaesar(const std::string& text, int key, char lang) {
    // Валидация ключа
    if (!isValidKey(key, lang)) {
        throw std::invalid_argument(getKeyRangeError(lang));
    }
    
    // Нормализуем язык
    lang = std::toupper(lang);
    
    std::string result;
    result.reserve(text.length());  // Оптимизация памяти
    
    for (char ch : text) {
        if (lang == 'E') {
            // Обработка английского текста
            if (ch >= 'a' && ch <= 'z') {
                result += shiftChar(ch, key, ENGLISH_LOWER);
            } else if (ch >= 'A' && ch <= 'Z') {
                result += shiftChar(ch, key, ENGLISH_UPPER);
            } else {
                result += ch;  // Спецсимволы, цифры, пробелы
            }
        } else if (lang == 'R') {
            // Обработка русского текста
            if (RUSSIAN_LOWER.find(ch) != std::string::npos) {
                result += shiftChar(ch, key, RUSSIAN_LOWER);
            } else if (RUSSIAN_UPPER.find(ch) != std::string::npos) {
                result += shiftChar(ch, key, RUSSIAN_UPPER);
            } else {
                result += ch;  // Спецсимволы, цифры, пробелы
            }
        } else {
            throw std::invalid_argument("Неподдерживаемый язык. Используйте 'E' для английского или 'R' для русского.");
        }
    }
    
    return result;
}

std::string decryptCaesar(const std::string& text, int key, char lang) {
    // Валидация ключа
    if (!isValidKey(key, lang)) {
        throw std::invalid_argument(getKeyRangeError(lang));
    }
    
    // Дешифрование — это шифрование с обратным ключом
    int alphabetSize = getAlphabetSize(lang);
    int reverseKey = alphabetSize - key;
    
    return encryptCaesar(text, reverseKey, lang);
}

bool isValidKey(int key, char lang) {
    lang = std::toupper(lang);
    
    if (lang == 'E') {
        return key >= 1 && key <= 25;
    } else if (lang == 'R') {
        return key >= 1 && key <= 32;
    }
    
    return false;
}

int generateRandomKey(char lang) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    lang = std::toupper(lang);
    
    if (lang == 'E') {
        std::uniform_int_distribution<> dis(1, 25);
        return dis(gen);
    } else if (lang == 'R') {
        std::uniform_int_distribution<> dis(1, 32);
        return dis(gen);
    }
    
    return 0;
}

int getAlphabetSize(char lang) {
    lang = std::toupper(lang);
    
    if (lang == 'E') {
        return 26;
    } else if (lang == 'R') {
        return 33;  // а-я = 33 буквы
    }
    
    return 0;
}

std::string getKeyRangeError(char lang) {
    lang = std::toupper(lang);
    
    if (lang == 'E') {
        return "Ошибка: ключ для английского языка должен быть от 1 до 25";
    } else if (lang == 'R') {
        return "Ошибка: ключ для русского языка должен быть от 1 до 32";
    }
    
    return "Ошибка: неподдерживаемый язык";
}
