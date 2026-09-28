#include "FormatUtils.h"
#include <cmath>
#include <utility>

namespace {

    /** @brief Сколько цифр максимум принимает TryParseNonNegativeInt (9 цифр всегда помещаются в int). */
    constexpr size_t kMaxIntDigits = 9;

    /** @brief Основание десятичной системы счисления (для разбора чисел по цифрам). */
    constexpr int kDecimalBase = 10;

    /** @brief Сколько сотых в единице (для округления до двух знаков после точки). */
    constexpr long long kHundredthsPerUnit = 100;

    /** @brief Меньшие числа дробной части печатаются с ведущим нулём ("5" -> "05"). */
    constexpr long long kTwoDigitThreshold = 10;

    /** @brief Пробельные символы, которые Trim() убирает по краям строки. */
    const char* const kWhitespace = " \t\r\n";

    /**
     * @brief Таблица замен для сравнения без учёта регистра: заглавная
     *        русская буква -> строчная, плюс Ё/ё -> е. Файл собирается с
     *        флагом /utf-8, поэтому каждая буква в кавычках - это уже
     *        готовая строка из тех же байт, которыми она записана в
     *        тексте программы. Буквы просто ищутся в строке как обычный
     *        текст - разбираться во внутреннем устройстве кодировки UTF-8
     *        не нужно.
     */
    const std::pair<std::string, std::string> kLetterReplacements[] = {
        {"А", "а"}, {"Б", "б"}, {"В", "в"}, {"Г", "г"}, {"Д", "д"},
        {"Е", "е"}, {"Ж", "ж"}, {"З", "з"}, {"И", "и"}, {"Й", "й"},
        {"К", "к"}, {"Л", "л"}, {"М", "м"}, {"Н", "н"}, {"О", "о"},
        {"П", "п"}, {"Р", "р"}, {"С", "с"}, {"Т", "т"}, {"У", "у"},
        {"Ф", "ф"}, {"Х", "х"}, {"Ц", "ц"}, {"Ч", "ч"}, {"Ш", "ш"},
        {"Щ", "щ"}, {"Ъ", "ъ"}, {"Ы", "ы"}, {"Ь", "ь"}, {"Э", "э"},
        {"Ю", "ю"}, {"Я", "я"},
        {"Ё", "е"}, {"ё", "е"},
    };

    /**
     * @brief Проверяет, начинается ли с позиции @p pos строки @p s одна
     *        из букв таблицы kLetterReplacements.
     * @param s   Строка, в которой выполняется поиск.
     * @param pos Позиция, с которой нужно проверить совпадение.
     * @return Указатель на найденную пару "что заменить -> на что",
     *         либо nullptr, если замена не требуется.
     */
    const std::pair<std::string, std::string>* FindReplacement(const std::string& s, size_t pos) {
        for (const auto& replacement : kLetterReplacements) {
            if (s.compare(pos, replacement.first.size(), replacement.first) == 0) {
                return &replacement;
            }
        }
        return nullptr;
    }

    /**
     * @brief Приводит строку к нижнему регистру: русские буквы - через
     *        таблицу kLetterReplacements, латинские A-Z - сдвигом к a-z.
     *        Все остальные символы (цифры, пробелы, знаки препинания,
     *        уже строчные буквы) копируются без изменений.
     * @param s Строка для приведения к нижнему регистру.
     * @return Строка @p s в нижнем регистре.
     */
    std::string ToLower(const std::string& s) {
        std::string result;
        result.reserve(s.size());

        size_t pos = 0;
        while (pos < s.size()) {
            if (const auto* const replacement = FindReplacement(s, pos)) {
                result += replacement->second;
                pos += replacement->first.size();
                continue;
            }

            const char c = s[pos];
            if (c >= 'A' && c <= 'Z') {
                result += static_cast<char>(c - 'A' + 'a');
            }
            else {
                result += c;
            }
            ++pos;
        }

        return result;
    }

}

namespace FormatUtils {

    std::string FormatDecimal(double value) {
        // Ни одна из переменных ниже не меняется после инициализации:
        // все составляющие результата сразу считаются как константы.
        const bool negative = value < 0.0;
        const double absValue = negative ? -value : value;

        const long long hundredths = static_cast<long long>(
            std::floor(absValue * static_cast<double>(kHundredthsPerUnit) + 0.5));
        const long long whole = hundredths / kHundredthsPerUnit;
        const long long frac = hundredths % kHundredthsPerUnit;

        const std::string sign = (negative && hundredths != 0) ? "-" : "";
        const std::string result = sign + std::to_string(whole) + "." +
            (frac < kTwoDigitThreshold ? "0" : "") + std::to_string(frac);
        return result;
    }

    std::string FormatMoney(double value) {
        return FormatDecimal(value);
    }

    bool EqualsIgnoreCase(const std::string& a, const std::string& b) {
        return ToLower(a) == ToLower(b);
    }

    std::string Trim(const std::string& s) {
        const size_t first = s.find_first_not_of(kWhitespace);
        if (first == std::string::npos) {
            return ""; // строка состоит только из пробельных символов
        }
        const size_t last = s.find_last_not_of(kWhitespace);
        return s.substr(first, last - first + 1);
    }

    std::optional<int> TryParseNonNegativeInt(const std::string& text) {
        const std::string digits = Trim(text);
        if (digits.empty() || digits.size() > kMaxIntDigits) {
            return std::nullopt;
        }

        int value = 0; // меняется в цикле ниже, поэтому не const
        for (const char c : digits) {
            if (c < '0' || c > '9') {
                return std::nullopt;
            }
            value = value * kDecimalBase + (c - '0');
        }
        return value;
    }

}
