//
// main.cpp
// Демонстрационная консольная программа для варианта задания "Магазин обуви".
//
// Общее задание (единое для всех вариантов): создать коллекцию объектов
// базового типа, заполнить её объектами типов-наследников,
// проитерировать её как коллекцию базового класса и вывести информацию
// о каждом объекте в std::cout.
//
// Задания варианта "Магазин обуви":
//   1) Выдавать информацию о количестве пар по заданному артикулу,
//      размеру, цвету и т.д.
//   2) Выдавать ассортимент обуви по отделам, сезону.
//   3) Выдавать информацию о данной обуви (цена, страна-изготовитель).
//   4) Продавать выбранный товар.
//   5) Показывать продажи за выбранный период времени.
//
// Структура файла: сначала объявления (прототипы) всех вспомогательных
// функций с их документацией, затем main(), и уже после main() -
// реализации (тела) этих функций в том же порядке.
//
// Весь ввод читается целыми строками (std::getline) и разбирается
// функцией FormatUtils::TryParseNonNegativeInt, поэтому ввод букв вместо
// цифр или слишком длинного числа не ломает программу - пользователь
// просто получает сообщение об ошибке.
//
#include <cstdlib>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // без этого Windows.h объявляет макросы min/max, которые
                 // конфликтуют с std::min/std::max и std::numeric_limits<...>::max()
#include <Windows.h>
#endif

#include "ShoeStoreLib.h"

namespace {

    /**
     * @brief Пункты главного меню. Значения соответствуют тому, что
     *        пользователь вводит с клавиатуры. PrintMenu() выводит эти
     *        же числа на экран через static_cast<int>(...), а не
     *        отдельными захардкоженными цифрами в строке - так текст меню
     *        и код обработки выбора не могут разойтись между собой.
     */
    enum class MainMenuChoice {
        Exit = 0,             ///< "0 - Выход"
        ShowAllShoes = 1,     ///< Весь ассортимент (общее задание: полиморфный вывод)
        CountPairs = 2,       ///< Задание 1: количество пар по артикулу, размеру, цвету
        ShowAssortment = 3,   ///< Задание 2: ассортимент по отделам, сезону
        ShowShoeInfo = 4,     ///< Задание 3: информация о данной обуви
        SellShoe = 5,         ///< Задание 4: продажа
        ShowSalesReport = 6,  ///< Задание 5: продажи за выбранный период
    };

    /** @brief Пункты подменю "Ассортимент" (задание 2). */
    enum class AssortmentMenuChoice {
        ByDepartment = 1,           ///< "1 - по отделу"
        BySeason = 2,               ///< "2 - по сезону"
        ByDepartmentAndSeason = 3,  ///< "3 - по отделу и сезону"
    };

    /** @brief Пункты подменю выбора периода для отчёта о продажах (задание 5). */
    enum class PeriodMenuChoice {
        Week = 1,    ///< "1 - неделя"
        Month = 2,   ///< "2 - месяц"
        Year = 3,    ///< "3 - год"
        Custom = 4,  ///< "4 - произвольный период"
    };

#ifdef _WIN32
    /**
     * @brief Переключает кодовые страницы ввода/вывода консоли Windows
     *        в UTF-8. Проект собирается с флагом /utf-8, поэтому без
     *        этого консоль печатала бы русский текст в виде кракозябр.
     *        На платформах, отличных от Windows, функция не вызывается.
     */
    void EnableUtf8Console();
#endif

    /** @brief Печатает горизонтальную разделительную линию в std::cout. */
    void PrintDivider();

    /**
     * @brief Возвращает зафиксированную "текущую" дату демо-программы.
     *        Захардкожена (а не взята из системных часов), чтобы отчёты
     *        за неделю/месяц/год были воспроизводимы при каждом запуске.
     *        Этой же датой оформляются новые продажи (задание 4).
     */
    Date Today();

    /**
     * @brief Формирует демонстрационную коллекцию обуви, реализуя общее
     *        требование задания: коллекцию БАЗОВОГО типа (Shoe),
     *        заполненную объектами типов-наследников
     *        (MensShoe/WomensShoe/KidsShoe/SportsShoe).
     * @return Вектор умных указателей на Shoe.
     */
    std::vector<std::shared_ptr<Shoe>> CreateDemoShoes();

    /**
     * @brief Формирует демонстрационный магазин: добавляет в него всю
     *        обувь из @p shoes, регистрирует дисконтные карты и заполняет
     *        историю продаж, чтобы отчёту за период (задание 5) было что
     *        показать.
     * @param shoes Обувь для добавления в магазин.
     * @return Готовый к работе экземпляр ShoeStore.
     */
    ShoeStore CreateDemoStore(const std::vector<std::shared_ptr<Shoe>>& shoes);

    /**
     * @brief Главный цикл меню: печатает пункты, читает выбор и вызывает
     *        обработчик нужного задания, пока пользователь не выберет
     *        "Выход" или не закончится ввод.
     * @param shoes Коллекция базового типа для пункта "Весь ассортимент".
     * @param store Магазин (не const - пункт "Продажа" меняет остатки и журнал продаж).
     */
    void RunMenu(const std::vector<std::shared_ptr<Shoe>>& shoes, ShoeStore& store);

    /** @brief Печатает в std::cout пункты главного меню. */
    void PrintMenu();

    // ---- Ввод с консоли ----

    /**
     * @brief Печатает подсказку @p prompt и читает одну строку ввода
     *        (пробелы по краям убираются).
     * @return Прочитанная строка, либо std::nullopt, если ввод закончился.
     */
    std::optional<std::string> ReadLine(const std::string& prompt);

    /** @brief То же, что ReadLine(), но при окончании ввода возвращает пустую строку. */
    std::string AskText(const std::string& prompt);

    /**
     * @brief Спрашивает у пользователя неотрицательное целое число.
     * @return Число, либо std::nullopt, если введено не число.
     */
    std::optional<int> AskNumber(const std::string& prompt);

    /**
     * @brief Спрашивает дату в формате ГГГГ-ММ-ДД.
     * @return Дата, либо std::nullopt (с сообщением об ошибке), если дата некорректна.
     */
    std::optional<Date> AskDate(const std::string& prompt);

    /**
     * @brief Выводит список отделов (номер пункта = значение enum
     *        Department) и спрашивает выбор пользователя.
     * @return Выбранный отдел, либо std::nullopt (с сообщением), если такого номера нет.
     */
    std::optional<Department> AskDepartment();

    /**
     * @brief Выводит список сезонов (номер пункта = значение enum Season)
     *        и спрашивает выбор пользователя.
     * @return Выбранный сезон, либо std::nullopt (с сообщением), если такого номера нет.
     */
    std::optional<Season> AskSeason();

    /**
     * @brief Превращает введённый текст в условие поиска: пустая строка
     *        означает "любой" (std::nullopt).
     */
    std::optional<std::string> ToFilter(const std::string& text);

    // ---- Вывод ----

    /**
     * @brief Печатает список обуви (по одной строке GetInfo() на пару
     *        обуви) либо сообщение "Ничего не найдено.".
     */
    void PrintShoes(const std::vector<std::shared_ptr<const Shoe>>& shoes);

    // ---- Общее задание: коллекция базового типа + полиморфный вывод ----

    /**
     * @brief Проходит коллекцию обуви через указатели базового класса
     *        (Shoe) и печатает информацию о каждом элементе. GetInfo() и
     *        GetDepartment() виртуальные, поэтому вызывается реализация
     *        конкретного класса-наследника, хотя статический тип каждого
     *        элемента - "указатель на Shoe".
     * @param shoes Коллекция для вывода.
     */
    void PrintAllShoesPolymorphically(const std::vector<std::shared_ptr<Shoe>>& shoes);

    // ---- Задание 1 ----

    /**
     * @brief Спрашивает артикул, размер и цвет (каждое можно пропустить
     *        клавишей Enter) и печатает подходящую обувь и общее
     *        количество пар.
     */
    void ShowPairsCount(const ShoeStore& store);

    // ---- Задание 2 ----

    /** @brief Спрашивает отдел и/или сезон и печатает соответствующий ассортимент. */
    void ShowAssortment(const ShoeStore& store);

    // ---- Задание 3 ----

    /** @brief Спрашивает артикул и печатает информацию об этой обуви (цена, страна-изготовитель и т.д.). */
    void ShowShoeInfo(const ShoeStore& store);

    // ---- Задание 4 ----

    /**
     * @brief Оформляет продажу: спрашивает артикул, размер, количество
     *        пар и номер дисконтной карты, вызывает ShoeStore::Sell() и
     *        печатает результат.
     * @param store Магазин (изменяется при успешной продаже).
     */
    void SellShoe(ShoeStore& store);

    /** @brief Возвращает текст сообщения для результата продажи. */
    std::string SaleStatusMessage(SaleStatus status);

    // ---- Задание 5 ----

    /** @brief Спрашивает период (неделя/месяц/год/произвольный) и печатает продажи за него. */
    void ShowSalesReport(const ShoeStore& store);

    /** @brief Печатает продажи за стандартный период (неделя/месяц/год) по состоянию на Today(). */
    void PrintSalesForPeriod(const ShoeStore& store, SalesPeriod period);

    /** @brief Спрашивает даты начала и конца периода и печатает продажи за него. */
    void PrintSalesBetween(const ShoeStore& store);

    /** @brief Возвращает название периода для заголовка отчёта ("неделю" / "месяц" / "год"). */
    std::string PeriodName(SalesPeriod period);

    /** @brief Печатает заголовок, список продаж и итог (пары и сумма). */
    void PrintSales(const std::string& title, const std::vector<Sale>& sales);

} // namespace

/**
 * @brief Точка входа программы. Создаёт коллекцию объектов базового типа
 *        Shoe, заполненную объектами типов-наследников, печатает её
 *        полиморфный обход и запускает меню с заданиями варианта.
 * @return EXIT_SUCCESS при нормальном завершении, EXIT_FAILURE при
 *         непредвиденной ошибке.
 */
int main() {
#ifdef _WIN32
    EnableUtf8Console();
#endif

    try {
        // Общее задание: коллекция объектов БАЗОВОГО типа, заполненная
        // объектами типов-наследников. Сама коллекция не меняется - const.
        const std::vector<std::shared_ptr<Shoe>> shoes = CreateDemoShoes();

        // store НЕ const: пункт меню "Продажа" (задание 4) списывает пары
        // со склада и добавляет запись в журнал продаж.
        ShoeStore store = CreateDemoStore(shoes);

        std::cout << "=== " << store.GetName() << " ===\n";
        std::cout << "Сегодня: " << Today().ToString() << "\n";

        PrintAllShoesPolymorphically(shoes);
        RunMenu(shoes, store);
    }
    catch (const std::exception& error) {
        // Библиотека бросает исключения только при некорректных данных
        // (например, неверная дата). Все данные демо-программы корректны,
        // поэтому сюда можно попасть только при ошибке в самой программе.
        std::cout << "Непредвиденная ошибка: " << error.what() << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "Работа программы завершена.\n";
    return EXIT_SUCCESS;
}

// ============================================================================
// Реализации функций, объявленных выше (см. их документацию у прототипов).
// ============================================================================

namespace {

#ifdef _WIN32
    void EnableUtf8Console() {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
#endif

    void PrintDivider() {
        std::cout << "------------------------------------------------------------\n";
    }

    Date Today() {
        return Date(2026, 9, 24);
    }

    std::vector<std::shared_ptr<Shoe>> CreateDemoShoes() {
        std::vector<std::shared_ptr<Shoe>> shoes;

        shoes.push_back(std::make_shared<MensShoe>(
            "M-1001", "Классик", "оксфорды", "черный", Season::DemiSeason,
            6990.00, "Италия",
            std::map<int, int>{ {40, 2}, {41, 3}, {42, 4}, {43, 2} },
            "шнурки"));

        shoes.push_back(std::make_shared<MensShoe>(
            "M-1002", "Норд", "зимние ботинки", "коричневый", Season::Winter,
            8490.00, "Россия",
            std::map<int, int>{ {41, 1}, {42, 3}, {44, 2} },
            "молния"));

        shoes.push_back(std::make_shared<WomensShoe>(
            "W-2001", "Элегант", "туфли-лодочки", "красный", Season::Summer,
            5490.00, "Испания",
            std::map<int, int>{ {36, 2}, {37, 3}, {38, 1} },
            8.0));

        shoes.push_back(std::make_shared<WomensShoe>(
            "W-2002", "Снежана", "сапоги", "черный", Season::Winter,
            11990.00, "Россия",
            std::map<int, int>{ {37, 2}, {38, 2}, {39, 1} },
            5.0));

        shoes.push_back(std::make_shared<KidsShoe>(
            "K-3001", "Непоседа", "сандалии", "синий", Season::Summer,
            1990.00, "Китай",
            std::map<int, int>{ {26, 4}, {27, 3}, {28, 5} },
            3, 6));

        shoes.push_back(std::make_shared<KidsShoe>(
            "K-3002", "Мишутка", "дутики", "зеленый", Season::Winter,
            3290.00, "Россия",
            std::map<int, int>{ {28, 2}, {30, 3} },
            4, 8));

        shoes.push_back(std::make_shared<SportsShoe>(
            "S-4001", "Спринт", "кроссовки", "белый", Season::AllSeason,
            7490.00, "Вьетнам",
            std::map<int, int>{ {40, 3}, {41, 2}, {42, 5}, {43, 1} },
            "бег"));

        shoes.push_back(std::make_shared<SportsShoe>(
            "S-4002", "Страйкер", "бутсы", "черный", Season::Summer,
            5990.00, "Германия",
            std::map<int, int>{ {41, 2}, {42, 2} },
            "футбол"));

        return shoes;
    }

    ShoeStore CreateDemoStore(const std::vector<std::shared_ptr<Shoe>>& shoes) {
        // store не const - ниже в него добавляется обувь, карты и продажи.
        ShoeStore store("Магазин обуви «Твой шаг»");
        for (const auto& shoe : shoes) {
            store.AddShoe(shoe);
        }

        store.RegisterDiscountCard(DiscountCard("1001", "Иванов Иван", 5));
        store.RegisterDiscountCard(DiscountCard("1002", "Петрова Анна", 10));
        store.RegisterDiscountCard(DiscountCard("1003", "Сидоров Пётр", 15));

        // История продаж (даты отсчитаны от Today() = 2026-09-24). Это
        // прошлые продажи, поэтому остатки на складе они не меняют.
        store.RegisterSale(Sale("M-1001", "Классик", 42, 1, 6990.00, 5, "1001", Date(2026, 9, 22)));    // 2 дня назад  -> неделя
        store.RegisterSale(Sale("S-4001", "Спринт", 42, 1, 7490.00, 0, "", Date(2026, 9, 20)));         // 4 дня назад  -> неделя
        store.RegisterSale(Sale("W-2001", "Элегант", 37, 1, 5490.00, 10, "1002", Date(2026, 9, 5)));    // 19 дней      -> месяц
        store.RegisterSale(Sale("K-3001", "Непоседа", 27, 2, 1990.00, 0, "", Date(2026, 6, 15)));       // 101 день     -> год
        store.RegisterSale(Sale("W-2002", "Снежана", 38, 1, 11990.00, 15, "1003", Date(2025, 12, 10))); // 288 дней     -> год
        store.RegisterSale(Sale("M-1002", "Норд", 42, 1, 8490.00, 0, "", Date(2025, 3, 1)));            // больше года назад

        return store;
    }

    void RunMenu(const std::vector<std::shared_ptr<Shoe>>& shoes, ShoeStore& store) {
        bool running = true;
        while (running) {
            PrintMenu();
            const std::optional<std::string> input = ReadLine("Ваш выбор: ");
            if (!input) {
                break; // конец ввода (например, при перенаправлении из файла)
            }

            const std::optional<int> choice = FormatUtils::TryParseNonNegativeInt(*input);
            if (!choice) {
                std::cout << "Введите номер пункта меню цифрами.\n";
                continue;
            }

            switch (static_cast<MainMenuChoice>(*choice)) {
            case MainMenuChoice::ShowAllShoes:
                PrintAllShoesPolymorphically(shoes);
                break;
            case MainMenuChoice::CountPairs:
                ShowPairsCount(store);
                break;
            case MainMenuChoice::ShowAssortment:
                ShowAssortment(store);
                break;
            case MainMenuChoice::ShowShoeInfo:
                ShowShoeInfo(store);
                break;
            case MainMenuChoice::SellShoe:
                SellShoe(store);
                break;
            case MainMenuChoice::ShowSalesReport:
                ShowSalesReport(store);
                break;
            case MainMenuChoice::Exit:
                running = false;
                break;
            default:
                // default обязателен: число ввёл пользователь, и оно может
                // не совпадать ни с одним значением MainMenuChoice.
                std::cout << "Неизвестный пункт меню.\n";
                break;
            }
        }
    }

    void PrintMenu() {
        PrintDivider();
        std::cout << static_cast<int>(MainMenuChoice::ShowAllShoes)
            << " - Весь ассортимент (полиморфный вывод коллекции Shoe)\n";
        std::cout << static_cast<int>(MainMenuChoice::CountPairs)
            << " - Количество пар по артикулу, размеру, цвету (Задание 1)\n";
        std::cout << static_cast<int>(MainMenuChoice::ShowAssortment)
            << " - Ассортимент по отделам, сезону (Задание 2)\n";
        std::cout << static_cast<int>(MainMenuChoice::ShowShoeInfo)
            << " - Информация об обуви: цена, страна-изготовитель (Задание 3)\n";
        std::cout << static_cast<int>(MainMenuChoice::SellShoe)
            << " - Продать товар (Задание 4)\n";
        std::cout << static_cast<int>(MainMenuChoice::ShowSalesReport)
            << " - Продажи за выбранный период (Задание 5)\n";
        std::cout << static_cast<int>(MainMenuChoice::Exit)
            << " - Выход\n";
    }

    // ---- Ввод с консоли ----

    std::optional<std::string> ReadLine(const std::string& prompt) {
        std::cout << prompt;
        std::string line; // заполняется std::getline ниже, поэтому не const
        if (!std::getline(std::cin, line)) {
            return std::nullopt;
        }
        return FormatUtils::Trim(line);
    }

    std::string AskText(const std::string& prompt) {
        return ReadLine(prompt).value_or("");
    }

    std::optional<int> AskNumber(const std::string& prompt) {
        return FormatUtils::TryParseNonNegativeInt(AskText(prompt));
    }

    std::optional<Date> AskDate(const std::string& prompt) {
        const std::string text = AskText(prompt);
        try {
            return Date::Parse(text);
        }
        catch (const std::invalid_argument&) {
            std::cout << "Некорректная дата \"" << text << "\": ожидается формат ГГГГ-ММ-ДД.\n";
            return std::nullopt;
        }
    }

    std::optional<Department> AskDepartment() {
        std::cout << "Выберите отдел:\n";
        for (const Department department : AllDepartments()) {
            std::cout << "  " << static_cast<int>(department) << " - " << DepartmentName(department) << "\n";
        }

        const std::optional<int> choice = AskNumber("Ваш выбор: ");
        if (choice) {
            for (const Department department : AllDepartments()) {
                if (static_cast<int>(department) == *choice) {
                    return department;
                }
            }
        }
        std::cout << "Неизвестный отдел.\n";
        return std::nullopt;
    }

    std::optional<Season> AskSeason() {
        std::cout << "Выберите сезон:\n";
        for (const Season season : AllSeasons()) {
            std::cout << "  " << static_cast<int>(season) << " - " << SeasonName(season) << "\n";
        }

        const std::optional<int> choice = AskNumber("Ваш выбор: ");
        if (choice) {
            for (const Season season : AllSeasons()) {
                if (static_cast<int>(season) == *choice) {
                    return season;
                }
            }
        }
        std::cout << "Неизвестный сезон.\n";
        return std::nullopt;
    }

    std::optional<std::string> ToFilter(const std::string& text) {
        if (text.empty()) {
            return std::nullopt;
        }
        return text;
    }

    // ---- Вывод ----

    void PrintShoes(const std::vector<std::shared_ptr<const Shoe>>& shoes) {
        if (shoes.empty()) {
            std::cout << "  Ничего не найдено.\n";
            return;
        }
        for (const auto& shoe : shoes) {
            std::cout << "  " << shoe->GetInfo() << "\n";
        }
    }

    // ---- Общее задание ----

    void PrintAllShoesPolymorphically(const std::vector<std::shared_ptr<Shoe>>& shoes) {
        PrintDivider();
        std::cout << "Весь ассортимент (полиморфный обход коллекции базового типа Shoe):\n";
        PrintDivider();
        for (const std::shared_ptr<Shoe>& shoe : shoes) {
            std::cout << shoe->GetInfo() << "\n";
        }
    }

    // ---- Задание 1 ----

    void ShowPairsCount(const ShoeStore& store) {
        std::cout << "Условия поиска (Enter - любое значение):\n";
        const std::optional<std::string> article = ToFilter(AskText("  Артикул: "));

        const std::string sizeText = AskText("  Размер: ");
        const std::optional<int> size = FormatUtils::TryParseNonNegativeInt(sizeText);
        if (!sizeText.empty() && !size) {
            std::cout << "Некорректный размер \"" << sizeText << "\".\n";
            return;
        }

        const std::optional<std::string> color = ToFilter(AskText("  Цвет: "));

        PrintDivider();
        std::cout << "Артикул: " << article.value_or("любой")
            << ", размер: " << (size ? std::to_string(*size) : "любой")
            << ", цвет: " << color.value_or("любой") << "\n";

        const auto found = store.FindShoes(article, size, color);
        if (found.empty()) {
            std::cout << "  Подходящей обуви нет в наличии.\n";
        }
        for (const auto& shoe : found) {
            std::cout << "  " << shoe->GetArticle() << " «" << shoe->GetModel() << "» ("
                << shoe->GetStyle() << "), " << shoe->GetColor()
                << " - пар: " << store.CountPairs(shoe->GetArticle(), size, std::nullopt) << "\n";
        }
        std::cout << "Итого пар: " << store.CountPairs(article, size, color) << "\n";
    }

    // ---- Задание 2 ----

    void ShowAssortment(const ShoeStore& store) {
        std::cout << "Показать ассортимент:\n";
        std::cout << "  " << static_cast<int>(AssortmentMenuChoice::ByDepartment) << " - по отделу\n";
        std::cout << "  " << static_cast<int>(AssortmentMenuChoice::BySeason) << " - по сезону\n";
        std::cout << "  " << static_cast<int>(AssortmentMenuChoice::ByDepartmentAndSeason) << " - по отделу и сезону\n";

        const std::optional<int> choice = AskNumber("Ваш выбор: ");
        if (!choice) {
            std::cout << "Неизвестный пункт меню.\n";
            return;
        }

        const AssortmentMenuChoice mode = static_cast<AssortmentMenuChoice>(*choice);
        const bool needDepartment = mode == AssortmentMenuChoice::ByDepartment ||
            mode == AssortmentMenuChoice::ByDepartmentAndSeason;
        const bool needSeason = mode == AssortmentMenuChoice::BySeason ||
            mode == AssortmentMenuChoice::ByDepartmentAndSeason;
        if (!needDepartment && !needSeason) {
            std::cout << "Неизвестный пункт меню.\n";
            return;
        }

        const std::optional<Department> department = needDepartment ? AskDepartment() : std::nullopt;
        if (needDepartment && !department) {
            return; // сообщение об ошибке уже напечатано в AskDepartment()
        }
        const std::optional<Season> season = needSeason ? AskSeason() : std::nullopt;
        if (needSeason && !season) {
            return; // сообщение об ошибке уже напечатано в AskSeason()
        }

        PrintDivider();
        std::cout << "Ассортимент. Отдел: " << (department ? DepartmentName(*department) : "все")
            << ", сезон: " << (season ? SeasonName(*season) : "все") << "\n";
        PrintShoes(store.GetAssortment(department, season));
    }

    // ---- Задание 3 ----

    void ShowShoeInfo(const ShoeStore& store) {
        const std::string article = AskText("Введите артикул (например, M-1001): ");
        const std::shared_ptr<const Shoe> shoe = store.FindShoeByArticle(article);

        PrintDivider();
        if (!shoe) {
            std::cout << "Обувь с артикулом \"" << article << "\" не найдена.\n";
            return;
        }
        std::cout << "Артикул: " << shoe->GetArticle() << "\n";
        std::cout << "Модель: «" << shoe->GetModel() << "» (" << shoe->GetStyle() << ")\n";
        std::cout << "Отдел: " << shoe->GetDepartmentName() << "\n";
        std::cout << "Цена: " << FormatUtils::FormatMoney(shoe->GetPrice()) << " руб.\n";
        std::cout << "Страна-изготовитель: " << shoe->GetCountry() << "\n";
        std::cout << "Полная информация: " << shoe->GetInfo() << "\n";
    }

    // ---- Задание 4 ----

    void SellShoe(ShoeStore& store) {
        const std::string article = AskText("Введите артикул (например, M-1001): ");
        const std::shared_ptr<const Shoe> shoe = store.FindShoeByArticle(article);
        if (!shoe) {
            std::cout << "Обувь с артикулом \"" << article << "\" не найдена.\n";
            return;
        }
        std::cout << "  " << shoe->GetInfo() << "\n";

        const std::optional<int> size = AskNumber("Размер: ");
        if (!size) {
            std::cout << "Некорректный размер.\n";
            return;
        }
        const std::optional<int> quantity = AskNumber("Количество пар: ");
        if (!quantity) {
            std::cout << "Некорректное количество.\n";
            return;
        }

        std::cout << "Дисконтные карты магазина:\n";
        for (const auto& card : store.GetAllDiscountCards()) {
            std::cout << "  " << card.GetInfo() << "\n";
        }
        const std::string cardNumber = AskText("Номер дисконтной карты (Enter - без карты): ");

        const SaleStatus status = store.Sell(shoe->GetArticle(), *size, *quantity, cardNumber, Today());
        PrintDivider();
        std::cout << SaleStatusMessage(status) << "\n";
        if (status == SaleStatus::Success) {
            std::cout << "  " << store.GetAllSales().back().GetInfo() << "\n";
            std::cout << "  Осталось пар размера " << *size << ": " << shoe->GetPairsCount(*size) << "\n";
        }
    }

    std::string SaleStatusMessage(SaleStatus status) {
        switch (status) {
        case SaleStatus::Success:             return "Продажа оформлена.";
        case SaleStatus::InvalidQuantity:     return "Ошибка: количество пар должно быть больше нуля.";
        case SaleStatus::UnknownArticle:      return "Ошибка: обувь с таким артикулом не найдена.";
        case SaleStatus::SizeNotAvailable:    return "Ошибка: такого размера нет в наличии.";
        case SaleStatus::NotEnoughPairs:      return "Ошибка: недостаточно пар такого размера.";
        case SaleStatus::UnknownDiscountCard: return "Ошибка: дисконтная карта с таким номером не найдена.";
        }
        return "Неизвестный результат продажи.";
    }

    // ---- Задание 5 ----

    void ShowSalesReport(const ShoeStore& store) {
        std::cout << "Выберите период:\n";
        std::cout << "  " << static_cast<int>(PeriodMenuChoice::Week) << " - неделя\n";
        std::cout << "  " << static_cast<int>(PeriodMenuChoice::Month) << " - месяц\n";
        std::cout << "  " << static_cast<int>(PeriodMenuChoice::Year) << " - год\n";
        std::cout << "  " << static_cast<int>(PeriodMenuChoice::Custom) << " - произвольный период (с ... по ...)\n";

        const std::optional<int> choice = AskNumber("Ваш выбор: ");
        if (!choice) {
            std::cout << "Неизвестный пункт меню.\n";
            return;
        }

        switch (static_cast<PeriodMenuChoice>(*choice)) {
        case PeriodMenuChoice::Week:
            PrintSalesForPeriod(store, SalesPeriod::Week);
            break;
        case PeriodMenuChoice::Month:
            PrintSalesForPeriod(store, SalesPeriod::Month);
            break;
        case PeriodMenuChoice::Year:
            PrintSalesForPeriod(store, SalesPeriod::Year);
            break;
        case PeriodMenuChoice::Custom:
            PrintSalesBetween(store);
            break;
        default:
            // default обязателен: число ввёл пользователь, и оно может
            // не совпадать ни с одним значением PeriodMenuChoice.
            std::cout << "Неизвестный пункт меню.\n";
            break;
        }
    }

    void PrintSalesForPeriod(const ShoeStore& store, SalesPeriod period) {
        const Date today = Today();
        PrintSales("Продажи за " + PeriodName(period) + " (по состоянию на " + today.ToString() + ")",
            store.GetSalesForPeriod(period, today));
    }

    void PrintSalesBetween(const ShoeStore& store) {
        const std::optional<Date> from = AskDate("Начало периода (ГГГГ-ММ-ДД): ");
        if (!from) {
            return;
        }
        const std::optional<Date> to = AskDate("Конец периода (ГГГГ-ММ-ДД): ");
        if (!to) {
            return;
        }
        if (*from > *to) {
            std::cout << "Ошибка: начало периода позже его конца.\n";
            return;
        }
        PrintSales("Продажи с " + from->ToString() + " по " + to->ToString(),
            store.GetSalesBetween(*from, *to));
    }

    std::string PeriodName(SalesPeriod period) {
        switch (period) {
        case SalesPeriod::Week:  return "неделю";
        case SalesPeriod::Month: return "месяц";
        case SalesPeriod::Year:  return "год";
        }
        return "период";
    }

    void PrintSales(const std::string& title, const std::vector<Sale>& sales) {
        PrintDivider();
        std::cout << title << ":\n";
        if (sales.empty()) {
            std::cout << "  Продаж за указанный период не найдено.\n";
        }
        for (const auto& sale : sales) {
            std::cout << "  " << sale.GetInfo() << "\n";
        }
        std::cout << "Итого продано пар: " << ShoeStore::CountSoldPairs(sales)
            << " на сумму " << FormatUtils::FormatMoney(ShoeStore::SumRevenue(sales)) << " руб.\n";
    }

} // namespace
