#include "Date.h"
#include "FormatUtils.h"
#include <optional>
#include <stdexcept>

namespace {

    // ---- Правила календаря ----
    constexpr int kMinYear = 1;               ///< Минимальный поддерживаемый год.
    constexpr int kMaxYear = 9999;            ///< Максимальный поддерживаемый год (4 цифры).
    constexpr int kMonthsInYear = 12;         ///< Месяцев в году.
    constexpr int kFebruary = 2;              ///< Номер февраля.
    constexpr int kDaysInLeapFebruary = 29;   ///< Дней в феврале високосного года.
    constexpr long kDaysInCommonYear = 365;   ///< Дней в невисокосном году.
    constexpr int kLeapYearEvery = 4;         ///< Каждый 4-й год - високосный,
    constexpr int kCenturyYears = 100;        ///< кроме каждого 100-го,
    constexpr int kLeapCenturyEvery = 400;    ///< но каждый 400-й - снова високосный.

    /** @brief Количество дней в каждом месяце невисокосного года (январь - декабрь). */
    const int kDaysInMonths[kMonthsInYear] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // ---- Формат строки "ГГГГ-ММ-ДД" ----
    constexpr size_t kIsoDateLength = 10;     ///< Длина строки "ГГГГ-ММ-ДД".
    constexpr size_t kYearPos = 0;            ///< Позиция года.
    constexpr size_t kYearLength = 4;         ///< Количество цифр года.
    constexpr size_t kFirstDashPos = 4;       ///< Позиция первого дефиса.
    constexpr size_t kMonthPos = 5;           ///< Позиция месяца.
    constexpr size_t kSecondDashPos = 7;      ///< Позиция второго дефиса.
    constexpr size_t kDayPos = 8;             ///< Позиция дня.
    constexpr size_t kTwoDigitsLength = 2;    ///< Количество цифр месяца и дня.
    constexpr int kTwoDigitThreshold = 10;    ///< Числа меньше 10 печатаются с ведущим нулём.

    /** @brief Возвращает true, если @p year - високосный год по григорианскому календарю. */
    bool IsLeapYear(int year) {
        return (year % kLeapYearEvery == 0 && year % kCenturyYears != 0) ||
            (year % kLeapCenturyEvery == 0);
    }

    /** @brief Возвращает количество дней в месяце @p month года @p year (с учётом високосного февраля). */
    int DaysInMonth(int year, int month) {
        if (month == kFebruary && IsLeapYear(year)) {
            return kDaysInLeapFebruary;
        }
        return kDaysInMonths[month - 1];
    }

    /** @brief Возвращает количество дней от 1 января 1 года до 1 января года @p year. */
    long DaysBeforeYear(int year) {
        const long previousYears = year - 1;
        return previousYears * kDaysInCommonYear
            + previousYears / kLeapYearEvery      // + високосные годы
            - previousYears / kCenturyYears       // - века (они не високосные)
            + previousYears / kLeapCenturyEvery;  // + каждый 400-й год (снова високосный)
    }

    /** @brief Возвращает количество дней от 1 января до 1-го числа месяца @p month в году @p year. */
    long DaysBeforeMonth(int year, int month) {
        long days = 0;
        for (int m = 1; m < month; ++m) {
            days += DaysInMonth(year, m);
        }
        return days;
    }

    /** @brief Печатает число двумя цифрами, с ведущим нулём при необходимости (5 -> "05"). */
    std::string PadTwoDigits(int value) {
        return (value < kTwoDigitThreshold ? "0" : "") + std::to_string(value);
    }

} // namespace

void Date::Validate(int year, int month, int day) {
    if (year < kMinYear || year > kMaxYear) {
        throw std::invalid_argument("Date: год должен быть в диапазоне 1..9999");
    }
    if (month < 1 || month > kMonthsInYear) {
        throw std::invalid_argument("Date: месяц должен быть в диапазоне 1..12");
    }
    if (day < 1 || day > DaysInMonth(year, month)) {
        throw std::invalid_argument("Date: некорректный день для указанного месяца/года");
    }
}

Date::Date(int year, int month, int day)
    : year_(year), month_(month), day_(day) {
    Validate(year_, month_, day_);
}

Date Date::Parse(const std::string& isoDate) {
    // Ожидаемый формат: "ГГГГ-ММ-ДД" - ровно 10 символов, дефисы на
    // своих местах, на всех остальных позициях - только цифры.
    if (isoDate.size() != kIsoDateLength ||
        isoDate[kFirstDashPos] != '-' || isoDate[kSecondDashPos] != '-') {
        throw std::invalid_argument("Date::Parse: ожидается формат ГГГГ-ММ-ДД");
    }
    for (size_t i = 0; i < isoDate.size(); ++i) {
        const bool isDashPosition = (i == kFirstDashPos || i == kSecondDashPos);
        if (!isDashPosition && (isoDate[i] < '0' || isoDate[i] > '9')) {
            throw std::invalid_argument("Date::Parse: ожидается формат ГГГГ-ММ-ДД");
        }
    }

    const std::optional<int> year = FormatUtils::TryParseNonNegativeInt(isoDate.substr(kYearPos, kYearLength));
    const std::optional<int> month = FormatUtils::TryParseNonNegativeInt(isoDate.substr(kMonthPos, kTwoDigitsLength));
    const std::optional<int> day = FormatUtils::TryParseNonNegativeInt(isoDate.substr(kDayPos, kTwoDigitsLength));
    if (!year || !month || !day) {
        throw std::invalid_argument("Date::Parse: ожидается формат ГГГГ-ММ-ДД");
    }
    return Date(*year, *month, *day); // конструктор дополнительно проверит, что такая дата существует
}

int Date::GetYear() const { return year_; }
int Date::GetMonth() const { return month_; }
int Date::GetDay() const { return day_; }

std::string Date::ToString() const {
    return std::to_string(year_) + "-" + PadTwoDigits(month_) + "-" + PadTwoDigits(day_);
}

long Date::ToDayNumber() const {
    // Номер дня = сколько дней прошло с 1 января 1 года (1 января 1 года - день номер 1).
    return DaysBeforeYear(year_) + DaysBeforeMonth(year_, month_) + day_;
}

long Date::DaysUntil(const Date& other) const {
    return other.ToDayNumber() - ToDayNumber();
}

bool Date::operator==(const Date& other) const {
    return year_ == other.year_ && month_ == other.month_ && day_ == other.day_;
}
bool Date::operator!=(const Date& other) const { return !(*this == other); }
bool Date::operator<(const Date& other) const { return ToDayNumber() < other.ToDayNumber(); }
bool Date::operator<=(const Date& other) const { return ToDayNumber() <= other.ToDayNumber(); }
bool Date::operator>(const Date& other) const { return ToDayNumber() > other.ToDayNumber(); }
bool Date::operator>=(const Date& other) const { return ToDayNumber() >= other.ToDayNumber(); }
