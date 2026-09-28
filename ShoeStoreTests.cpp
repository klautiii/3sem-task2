//
// ShoeStoreTests.cpp
// Все модульные тесты проекта в одном файле, на встроенном в Visual
// Studio фреймворке Microsoft CppUnitTest:
//   - TEST_CLASS(DateTests)        - класс Date;
//   - TEST_CLASS(FormatUtilsTests) - вспомогательные функции FormatUtils;
//   - TEST_CLASS(ShoeTests)        - базовый класс Shoe и классы-наследники
//                                    MensShoe/WomensShoe/KidsShoe/SportsShoe;
//   - TEST_CLASS(SaleTests)        - классы Sale и DiscountCard;
//   - TEST_CLASS(StoreQueryTests)  - ShoeStore, задания 1-3 (запросы);
//   - TEST_CLASS(StoreSaleTests)   - ShoeStore, задания 4-5 (продажа и отчёт).
//
// Стиль тестов: локальные объекты объявлены как const, если тест не
// меняет их после создания. Там, где объект меняется (RemovePairs(),
// Sell()), рядом стоит комментарий, почему он не const.
//
// Значения enum (Department, Season, SaleStatus) сравниваются через
// Assert::IsTrue(a == b): Assert::AreEqual умеет печатать только
// встроенные типы и строки, а для enum потребовал бы дополнительный код.
//
#include "CppUnitTest.h"
#include "ShoeStoreLib.h"
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ShoeStoreTests
{
    // ==== Тесты класса Date ====

    TEST_CLASS(DateTests)
    {
    public:
        /** @brief Parse() корректно извлекает год/месяц/день из допустимой строки. */
        TEST_METHOD(Parse_ValidIsoString_ReturnsCorrectComponents)
        {
            const Date d = Date::Parse("2026-09-24");
            Assert::AreEqual(2026, d.GetYear());
            Assert::AreEqual(9, d.GetMonth());
            Assert::AreEqual(24, d.GetDay());
        }

        /** @brief Parse() отклоняет строку, не соответствующую формату ГГГГ-ММ-ДД. */
        TEST_METHOD(Parse_WrongFormat_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                Date::Parse("24-09-2026");
                });
        }

        /** @brief Parse() отклоняет строку с буквой вместо цифры. */
        TEST_METHOD(Parse_LetterInsteadOfDigit_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                Date::Parse("2026-0a-24");
                });
        }

        /** @brief Конструктор отклоняет месяц вне диапазона 1..12. */
        TEST_METHOD(Constructor_InvalidMonth_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                const Date invalid(2026, 13, 1);
                (void)invalid;
                });
        }

        /** @brief Конструктор отклоняет день, которого нет в указанном месяце. */
        TEST_METHOD(Constructor_InvalidDayForMonth_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                const Date invalid(2026, 2, 30); // 2026 - не високосный год
                (void)invalid;
                });
        }

        /** @brief 29 февраля принимается в високосном году. */
        TEST_METHOD(LeapYear_February29_IsValid)
        {
            const Date d(2028, 2, 29); // 2028 - високосный год
            Assert::AreEqual(29, d.GetDay());
        }

        /** @brief DaysUntil() корректно работает при переходе через границу года. */
        TEST_METHOD(DaysUntil_AcrossYearBoundary_IsCorrect)
        {
            const Date start(2025, 12, 30);
            const Date end(2026, 1, 2);
            Assert::AreEqual(3L, start.DaysUntil(end));
        }

        /** @brief DaysUntil() учитывает 29 февраля високосного года. */
        TEST_METHOD(DaysUntil_AcrossLeapFebruary_IsCorrect)
        {
            const Date start(2024, 2, 28);
            const Date end(2024, 3, 1);
            Assert::AreEqual(2L, start.DaysUntil(end));
        }

        /** @brief ToString() дополняет месяц и день ведущим нулём. */
        TEST_METHOD(ToString_FormatsWithLeadingZeros)
        {
            const Date d(2026, 3, 5);
            Assert::AreEqual(std::string("2026-03-05"), d.ToString());
        }

        /** @brief Операторы сравнения работают как ожидается. */
        TEST_METHOD(ComparisonOperators_WorkAsExpected)
        {
            const Date a(2026, 1, 1);
            const Date b(2026, 1, 2);
            Assert::IsTrue(a < b);
            Assert::IsTrue(b > a);
            Assert::IsTrue(a <= a);
            Assert::IsTrue(a != b);
            Assert::IsTrue(a == Date(2026, 1, 1));
        }
    };

    // ==== Тесты вспомогательных функций FormatUtils ====

    TEST_CLASS(FormatUtilsTests)
    {
    public:
        /** @brief FormatMoney() печатает ровно два знака после точки с округлением. */
        TEST_METHOD(FormatMoney_RoundsToTwoDigits)
        {
            Assert::AreEqual(std::string("1234.50"), FormatUtils::FormatMoney(1234.5));
            Assert::AreEqual(std::string("10.46"), FormatUtils::FormatMoney(10.456));
            Assert::AreEqual(std::string("0.00"), FormatUtils::FormatMoney(0.0));
        }

        /** @brief EqualsIgnoreCase() не различает заглавные и строчные русские буквы. */
        TEST_METHOD(EqualsIgnoreCase_RussianLetters_IgnoresCase)
        {
            Assert::IsTrue(FormatUtils::EqualsIgnoreCase("ЧЕРНЫЙ", "черный"));
        }

        /** @brief EqualsIgnoreCase() считает буквы Ё/ё равными е. */
        TEST_METHOD(EqualsIgnoreCase_YoAndYe_AreEqual)
        {
            Assert::IsTrue(FormatUtils::EqualsIgnoreCase("Чёрный", "черный"));
        }

        /** @brief EqualsIgnoreCase() не различает заглавные и строчные латинские буквы. */
        TEST_METHOD(EqualsIgnoreCase_LatinLetters_IgnoresCase)
        {
            Assert::IsTrue(FormatUtils::EqualsIgnoreCase("m-1001", "M-1001"));
        }

        /** @brief EqualsIgnoreCase() возвращает false для разных слов. */
        TEST_METHOD(EqualsIgnoreCase_DifferentWords_ReturnsFalse)
        {
            Assert::IsFalse(FormatUtils::EqualsIgnoreCase("черный", "белый"));
        }

        /** @brief Trim() убирает пробелы и переводы строки по краям. */
        TEST_METHOD(Trim_RemovesWhitespaceAtBothEnds)
        {
            Assert::AreEqual(std::string("42"), FormatUtils::Trim("  42 \r\n"));
            Assert::AreEqual(std::string(""), FormatUtils::Trim("   "));
        }

        /** @brief TryParseNonNegativeInt() разбирает корректное число (с пробелами по краям). */
        TEST_METHOD(TryParseNonNegativeInt_ValidNumber_ReturnsValue)
        {
            const std::optional<int> value = FormatUtils::TryParseNonNegativeInt(" 42 ");
            Assert::IsTrue(value.has_value());
            Assert::AreEqual(42, *value);
        }

        /** @brief TryParseNonNegativeInt() отклоняет всё, что не является неотрицательным целым. */
        TEST_METHOD(TryParseNonNegativeInt_InvalidText_ReturnsNullopt)
        {
            Assert::IsFalse(FormatUtils::TryParseNonNegativeInt("").has_value());
            Assert::IsFalse(FormatUtils::TryParseNonNegativeInt("abc").has_value());
            Assert::IsFalse(FormatUtils::TryParseNonNegativeInt("-5").has_value());
            Assert::IsFalse(FormatUtils::TryParseNonNegativeInt("4 2").has_value());
            Assert::IsFalse(FormatUtils::TryParseNonNegativeInt("1234567890").has_value()); // 10 цифр - защита от переполнения
        }
    };

    // ==== Тесты базового класса Shoe и классов-наследников ====

    TEST_CLASS(ShoeTests)
    {
    public:
        /** @brief MensShoe относится к мужскому отделу и хранит тип застёжки. */
        TEST_METHOD(MensShoe_DepartmentAndClosureType)
        {
            const MensShoe shoe("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                1000.0, "Италия", std::map<int, int>{ {42, 1} }, "шнурки");
            Assert::IsTrue(shoe.GetDepartment() == Department::Men);
            Assert::AreEqual(std::string("Мужская обувь"), shoe.GetDepartmentName());
            Assert::AreEqual(std::string("шнурки"), shoe.GetClosureType());
        }

        /** @brief WomensShoe относится к женскому отделу и хранит высоту каблука. */
        TEST_METHOD(WomensShoe_DepartmentAndHeelHeight)
        {
            const WomensShoe shoe("W-1", "Элегант", "туфли", "красный", Season::Summer,
                2000.0, "Испания", std::map<int, int>{ {37, 1} }, 7.5);
            Assert::IsTrue(shoe.GetDepartment() == Department::Women);
            Assert::AreEqual(7.5, shoe.GetHeelHeightCm(), 0.0001);
        }

        /** @brief KidsShoe относится к детскому отделу и хранит диапазон возраста. */
        TEST_METHOD(KidsShoe_DepartmentAndAgeRange)
        {
            const KidsShoe shoe("K-1", "Непоседа", "сандалии", "синий", Season::Summer,
                500.0, "Китай", std::map<int, int>{ {27, 1} }, 3, 6);
            Assert::IsTrue(shoe.GetDepartment() == Department::Kids);
            Assert::AreEqual(3, shoe.GetMinAgeYears());
            Assert::AreEqual(6, shoe.GetMaxAgeYears());
        }

        /** @brief SportsShoe относится к спортивному отделу и хранит вид спорта. */
        TEST_METHOD(SportsShoe_DepartmentAndSportType)
        {
            const SportsShoe shoe("S-1", "Спринт", "кроссовки", "белый", Season::AllSeason,
                3000.0, "Вьетнам", std::map<int, int>{ {42, 1} }, "бег");
            Assert::IsTrue(shoe.GetDepartment() == Department::Sports);
            Assert::AreEqual(std::string("бег"), shoe.GetSportType());
        }

        /** @brief GetPairsCount() возвращает остаток для известного размера и 0 для неизвестного. */
        TEST_METHOD(GetPairsCount_KnownAndUnknownSize)
        {
            const MensShoe shoe("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                1000.0, "Италия", std::map<int, int>{ {41, 2}, {42, 3} }, "шнурки");
            Assert::AreEqual(3, shoe.GetPairsCount(42));
            Assert::AreEqual(0, shoe.GetPairsCount(45));
        }

        /** @brief GetTotalPairs() суммирует пары всех размеров. */
        TEST_METHOD(GetTotalPairs_SumsAllSizes)
        {
            const MensShoe shoe("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                1000.0, "Италия", std::map<int, int>{ {41, 2}, {42, 3} }, "шнурки");
            Assert::AreEqual(5, shoe.GetTotalPairs());
        }

        /** @brief GetAvailableSizes() пропускает размеры с нулевым остатком и сортирует по возрастанию. */
        TEST_METHOD(GetAvailableSizes_SkipsZeroAndSorted)
        {
            const MensShoe shoe("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                1000.0, "Италия", std::map<int, int>{ {43, 0}, {42, 2}, {41, 1} }, "шнурки");
            const std::vector<int> sizes = shoe.GetAvailableSizes();
            Assert::AreEqual(static_cast<size_t>(2), sizes.size());
            Assert::AreEqual(41, sizes[0]);
            Assert::AreEqual(42, sizes[1]);
        }

        /** @brief RemovePairs() уменьшает остаток указанного размера. */
        TEST_METHOD(RemovePairs_DecreasesStock)
        {
            // shoe не const: тест проверяет RemovePairs(), который меняет остатки.
            MensShoe shoe("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                1000.0, "Италия", std::map<int, int>{ {42, 3} }, "шнурки");
            shoe.RemovePairs(42, 2);
            Assert::AreEqual(1, shoe.GetPairsCount(42));
        }

        /** @brief RemovePairs() бросает исключение, если пар недостаточно, и не меняет остаток. */
        TEST_METHOD(RemovePairs_NotEnoughPairs_Throws)
        {
            // shoe не const: RemovePairs() - изменяющий метод.
            MensShoe shoe("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                1000.0, "Италия", std::map<int, int>{ {42, 3} }, "шнурки");
            Assert::ExpectException<std::out_of_range>([&shoe]() {
                shoe.RemovePairs(42, 10);
                });
            Assert::AreEqual(3, shoe.GetPairsCount(42));
        }

        /** @brief Конструктор отклоняет отрицательную цену. */
        TEST_METHOD(Constructor_NegativePrice_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                const MensShoe invalid("M-1", "Классик", "оксфорды", "черный", Season::DemiSeason,
                    -1.0, "Италия", std::map<int, int>{ {42, 1} }, "шнурки");
                (void)invalid;
                });
        }

        /** @brief KidsShoe отклоняет диапазон возраста, где "от" больше "до". */
        TEST_METHOD(KidsShoe_MinAgeGreaterThanMax_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                const KidsShoe invalid("K-1", "Непоседа", "сандалии", "синий", Season::Summer,
                    500.0, "Китай", std::map<int, int>{ {27, 1} }, 8, 3);
                (void)invalid;
                });
        }

        /**
         * @brief Вызов GetInfo() через указатель базового класса (Shoe*)
         *        вызывает переопределения наследника (полиморфизм):
         *        в описании есть и отдел, и поле, специфичное для отдела.
         */
        TEST_METHOD(GetInfo_PolymorphicCallThroughBasePointer_IncludesDepartmentAndSpecificInfo)
        {
            const std::shared_ptr<const Shoe> shoe = std::make_shared<WomensShoe>(
                "W-1", "Элегант", "туфли", "красный", Season::Summer,
                2000.0, "Испания", std::map<int, int>{ {37, 1} }, 7.5);

            const std::string info = shoe->GetInfo();
            Assert::IsTrue(info.find("Женская обувь") != std::string::npos);
            Assert::IsTrue(info.find("высота каблука: 7.50 см") != std::string::npos);
            Assert::IsTrue(info.find("Испания") != std::string::npos);
        }
    };

    // ==== Тесты классов Sale и DiscountCard ====

    TEST_CLASS(SaleTests)
    {
    public:
        /** @brief Итоговая сумма продажи учитывает скидку по карте. */
        TEST_METHOD(Sale_WithDiscount_CalculatesTotal)
        {
            const Sale sale("M-1", "Классик", 42, 2, 1000.0, 10, "1001", Date(2026, 9, 24));
            Assert::AreEqual(1800.0, sale.GetTotalPrice(), 0.001); // 1000 * 2 - 10%
        }

        /** @brief Без скидки сумма равна цене, умноженной на количество. */
        TEST_METHOD(Sale_WithoutDiscount_TotalIsPriceTimesQuantity)
        {
            const Sale sale("M-1", "Классик", 42, 3, 1000.0, 0, "", Date(2026, 9, 24));
            Assert::AreEqual(3000.0, sale.GetTotalPrice(), 0.001);
        }

        /** @brief Продажа нуля пар отклоняется. */
        TEST_METHOD(Sale_ZeroQuantity_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                const Sale invalid("M-1", "Классик", 42, 0, 1000.0, 0, "", Date(2026, 9, 24));
                (void)invalid;
                });
        }

        /** @brief Дисконтная карта со скидкой больше 100% отклоняется. */
        TEST_METHOD(DiscountCard_InvalidPercent_Throws)
        {
            Assert::ExpectException<std::invalid_argument>([]() {
                const DiscountCard invalid("1001", "Иванов Иван", 150);
                (void)invalid;
                });
        }

        /** @brief GetInfo() дисконтной карты содержит номер и размер скидки. */
        TEST_METHOD(DiscountCard_GetInfo_ContainsNumberAndPercent)
        {
            const DiscountCard card("1001", "Иванов Иван", 5);
            const std::string info = card.GetInfo();
            Assert::IsTrue(info.find("1001") != std::string::npos);
            Assert::IsTrue(info.find("5%") != std::string::npos);
        }
    };

    // ==== Общие данные для тестов класса ShoeStore ====

    namespace
    {
        // "Сегодня" для тестов - та же дата, что используется в демо
        // консольного приложения (ShoeStoreApp/main.cpp).
        const Date kToday(2026, 9, 24);

        /**
         * @brief Формирует небольшой тестовый магазин: по одной модели из
         *        каждого отдела, одна дисконтная карта (1001, скидка 10%)
         *        и три продажи в истории на известные даты.
         *        Всего пар на складе: M-1: 2+3, W-1: 1, K-1: 4, S-1: 1 = 11.
         * @return Готовый к запросам экземпляр ShoeStore.
         */
        ShoeStore BuildSampleStore()
        {
            // store не может быть const - в него добавляются обувь, карта и продажи.
            ShoeStore store("Тестовый магазин");

            store.AddShoe(std::make_shared<MensShoe>("M-1", "Классик", "оксфорды", "Чёрный",
                Season::DemiSeason, 1000.0, "Италия", std::map<int, int>{ {41, 2}, {42, 3} }, "шнурки"));
            store.AddShoe(std::make_shared<WomensShoe>("W-1", "Элегант", "туфли", "красный",
                Season::Summer, 2000.0, "Испания", std::map<int, int>{ {37, 1} }, 7.5));
            store.AddShoe(std::make_shared<KidsShoe>("K-1", "Непоседа", "сандалии", "черный",
                Season::Summer, 500.0, "Китай", std::map<int, int>{ {27, 4} }, 3, 6));
            store.AddShoe(std::make_shared<SportsShoe>("S-1", "Спринт", "кроссовки", "белый",
                Season::AllSeason, 3000.0, "Вьетнам", std::map<int, int>{ {42, 1} }, "бег"));

            store.RegisterDiscountCard(DiscountCard("1001", "Иванов Иван", 10));

            store.RegisterSale(Sale("M-1", "Классик", 42, 1, 1000.0, 0, "", Date(2026, 9, 20)));      // 4 дня назад  -> неделя
            store.RegisterSale(Sale("W-1", "Элегант", 37, 1, 2000.0, 10, "1001", Date(2026, 9, 1)));  // 23 дня назад -> месяц
            store.RegisterSale(Sale("K-1", "Непоседа", 27, 2, 500.0, 0, "", Date(2025, 1, 1)));       // больше года назад

            return store;
        }
    }

    // ==== Тесты ShoeStore: задания 1-3 (запросы) ====

    TEST_CLASS(StoreQueryTests)
    {
    public:
        // ---- Задание 1: количество пар по артикулу, размеру, цвету ----

        /** @brief Без условий считаются все пары в магазине. */
        TEST_METHOD(CountPairs_NoFilters_ReturnsAllPairs)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(11, store.CountPairs(std::nullopt, std::nullopt, std::nullopt));
        }

        /** @brief Артикул сравнивается без учёта регистра. */
        TEST_METHOD(CountPairs_ByArticle_CaseInsensitive)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(5, store.CountPairs(std::string("m-1"), std::nullopt, std::nullopt));
        }

        /** @brief При заданном размере считаются только пары этого размера. */
        TEST_METHOD(CountPairs_BySize_CountsOnlyThatSize)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(4, store.CountPairs(std::nullopt, 42, std::nullopt)); // M-1: 3 + S-1: 1
        }

        /** @brief Поиск по цвету не различает регистр и букву ё. */
        TEST_METHOD(CountPairs_ByColor_IgnoresCaseAndYo)
        {
            const ShoeStore store = BuildSampleStore();
            // "Чёрный" у M-1 (5 пар) и "черный" у K-1 (4 пары).
            Assert::AreEqual(9, store.CountPairs(std::nullopt, std::nullopt, std::string("ЧЕРНЫЙ")));
        }

        /** @brief Все три условия одновременно. */
        TEST_METHOD(CountPairs_ByArticleSizeAndColor)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(2, store.CountPairs(std::string("M-1"), 41, std::string("чёрный")));
        }

        /** @brief Для размера, которого нет в наличии, ничего не находится. */
        TEST_METHOD(FindShoes_UnknownSize_ReturnsEmpty)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.FindShoes(std::nullopt, 50, std::nullopt).empty());
        }

        // ---- Задание 2: ассортимент по отделам и сезону ----

        /** @brief Ассортимент женского отдела. */
        TEST_METHOD(GetAssortment_ByDepartment)
        {
            const ShoeStore store = BuildSampleStore();
            const auto shoes = store.GetAssortment(Department::Women, std::nullopt);
            Assert::AreEqual(static_cast<size_t>(1), shoes.size());
            Assert::AreEqual(std::string("W-1"), shoes[0]->GetArticle());
        }

        /** @brief Ассортимент летней обуви из всех отделов. */
        TEST_METHOD(GetAssortment_BySeason)
        {
            const ShoeStore store = BuildSampleStore();
            const auto shoes = store.GetAssortment(std::nullopt, Season::Summer);
            Assert::AreEqual(static_cast<size_t>(2), shoes.size()); // W-1 и K-1
        }

        /** @brief Отдел и сезон одновременно. */
        TEST_METHOD(GetAssortment_ByDepartmentAndSeason)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(static_cast<size_t>(1), store.GetAssortment(Department::Kids, Season::Summer).size());
            Assert::IsTrue(store.GetAssortment(Department::Kids, Season::Winter).empty());
        }

        /** @brief Без условий возвращается весь ассортимент. */
        TEST_METHOD(GetAssortment_NoFilters_ReturnsAll)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(static_cast<size_t>(4), store.GetAssortment(std::nullopt, std::nullopt).size());
        }

        // ---- Задание 3: информация о данной обуви ----

        /** @brief По артикулу находится обувь с верными ценой и страной-изготовителем. */
        TEST_METHOD(FindShoeByArticle_Existing_ReturnsPriceAndCountry)
        {
            const ShoeStore store = BuildSampleStore();
            const std::shared_ptr<const Shoe> shoe = store.FindShoeByArticle("s-1");
            Assert::IsNotNull(shoe.get());
            Assert::AreEqual(3000.0, shoe->GetPrice(), 0.001);
            Assert::AreEqual(std::string("Вьетнам"), shoe->GetCountry());
        }

        /** @brief Для неизвестного артикула возвращается nullptr. */
        TEST_METHOD(FindShoeByArticle_Missing_ReturnsNull)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::IsNull(store.FindShoeByArticle("X-999").get());
        }

        // ---- Целостность данных магазина ----

        /** @brief Второй товар с тем же артикулом добавить нельзя. */
        TEST_METHOD(AddShoe_DuplicateArticle_Throws)
        {
            // store не const: тест вызывает изменяющий метод AddShoe().
            ShoeStore store = BuildSampleStore();
            Assert::ExpectException<std::invalid_argument>([&store]() {
                store.AddShoe(std::make_shared<SportsShoe>("s-1", "Копия", "кроссовки", "белый",
                    Season::AllSeason, 1.0, "Китай", std::map<int, int>{ {42, 1} }, "бег"));
                });
        }

        /** @brief FindDiscountCard() находит зарегистрированную карту и не находит незарегистрированную. */
        TEST_METHOD(FindDiscountCard_ExistingAndMissing)
        {
            const ShoeStore store = BuildSampleStore();
            const std::optional<DiscountCard> card = store.FindDiscountCard("1001");
            Assert::IsTrue(card.has_value());
            Assert::AreEqual(10, card->GetDiscountPercent());
            Assert::IsFalse(store.FindDiscountCard("9999").has_value());
        }
    };

    // ==== Тесты ShoeStore: задания 4-5 (продажа и отчёт о продажах) ====

    TEST_CLASS(StoreSaleTests)
    {
    public:
        // ---- Задание 4: продажа ----

        /** @brief Успешная продажа уменьшает остаток и добавляет запись в журнал. */
        TEST_METHOD(Sell_Success_DecreasesStockAndRecordsSale)
        {
            // store не const: Sell() меняет остатки и журнал продаж.
            ShoeStore store = BuildSampleStore();
            const SaleStatus status = store.Sell("M-1", 42, 2, "", kToday);

            Assert::IsTrue(status == SaleStatus::Success);
            Assert::AreEqual(1, store.FindShoeByArticle("M-1")->GetPairsCount(42));
            Assert::AreEqual(static_cast<size_t>(4), store.GetAllSales().size());
            Assert::AreEqual(2000.0, store.GetAllSales().back().GetTotalPrice(), 0.001);
        }

        /** @brief Продажа по дисконтной карте применяет скидку. */
        TEST_METHOD(Sell_WithDiscountCard_AppliesDiscount)
        {
            // store не const: Sell() меняет магазин.
            ShoeStore store = BuildSampleStore();
            const SaleStatus status = store.Sell("M-1", 42, 1, "1001", kToday);

            Assert::IsTrue(status == SaleStatus::Success);
            Assert::AreEqual(900.0, store.GetAllSales().back().GetTotalPrice(), 0.001); // 1000 - 10%
        }

        /** @brief Неизвестный артикул - отказ, журнал не меняется. */
        TEST_METHOD(Sell_UnknownArticle_ReturnsUnknownArticle)
        {
            // store не const: вызывается изменяющий метод Sell().
            ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.Sell("X-999", 42, 1, "", kToday) == SaleStatus::UnknownArticle);
            Assert::AreEqual(static_cast<size_t>(3), store.GetAllSales().size());
        }

        /** @brief Размера нет в наличии - отказ. */
        TEST_METHOD(Sell_SizeNotAvailable_ReturnsSizeNotAvailable)
        {
            // store не const: вызывается изменяющий метод Sell().
            ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.Sell("M-1", 45, 1, "", kToday) == SaleStatus::SizeNotAvailable);
        }

        /** @brief Пар меньше, чем запрошено - отказ, остаток не меняется. */
        TEST_METHOD(Sell_NotEnoughPairs_StockUnchanged)
        {
            // store не const: вызывается изменяющий метод Sell().
            ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.Sell("M-1", 42, 10, "", kToday) == SaleStatus::NotEnoughPairs);
            Assert::AreEqual(3, store.FindShoeByArticle("M-1")->GetPairsCount(42));
        }

        /** @brief Нулевое количество пар - отказ. */
        TEST_METHOD(Sell_ZeroQuantity_ReturnsInvalidQuantity)
        {
            // store не const: вызывается изменяющий метод Sell().
            ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.Sell("M-1", 42, 0, "", kToday) == SaleStatus::InvalidQuantity);
        }

        /** @brief Неизвестная дисконтная карта - отказ, остаток не меняется. */
        TEST_METHOD(Sell_UnknownDiscountCard_StockUnchanged)
        {
            // store не const: вызывается изменяющий метод Sell().
            ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.Sell("M-1", 42, 1, "9999", kToday) == SaleStatus::UnknownDiscountCard);
            Assert::AreEqual(3, store.FindShoeByArticle("M-1")->GetPairsCount(42));
        }

        // ---- Задание 5: продажи за выбранный период ----

        /** @brief В отчёт за неделю попадает только продажа 4-дневной давности. */
        TEST_METHOD(GetSalesForPeriod_Week_OnlyRecentSale)
        {
            const ShoeStore store = BuildSampleStore();
            const std::vector<Sale> sales = store.GetSalesForPeriod(SalesPeriod::Week, kToday);
            Assert::AreEqual(static_cast<size_t>(1), sales.size());
            Assert::AreEqual(std::string("M-1"), sales[0].GetArticle());
        }

        /** @brief В отчёт за месяц попадают две продажи. */
        TEST_METHOD(GetSalesForPeriod_Month_TwoSales)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(static_cast<size_t>(2), store.GetSalesForPeriod(SalesPeriod::Month, kToday).size());
        }

        /** @brief В отчёт за год не попадает продажа старше 365 дней. */
        TEST_METHOD(GetSalesForPeriod_Year_ExcludesOlderThanYear)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::AreEqual(static_cast<size_t>(2), store.GetSalesForPeriod(SalesPeriod::Year, kToday).size());
        }

        /** @brief Произвольный период с ... по ... (обе даты включительно). */
        TEST_METHOD(GetSalesBetween_CustomRange)
        {
            const ShoeStore store = BuildSampleStore();
            const std::vector<Sale> sales = store.GetSalesBetween(Date(2026, 9, 1), Date(2026, 9, 10));
            Assert::AreEqual(static_cast<size_t>(1), sales.size());
            Assert::AreEqual(std::string("W-1"), sales[0].GetArticle());
        }

        /** @brief Если начало периода позже конца, продаж нет. */
        TEST_METHOD(GetSalesBetween_FromAfterTo_ReturnsEmpty)
        {
            const ShoeStore store = BuildSampleStore();
            Assert::IsTrue(store.GetSalesBetween(Date(2026, 9, 30), Date(2026, 9, 1)).empty());
        }

        /** @brief Итоги отчёта: количество пар и выручка с учётом скидок. */
        TEST_METHOD(CountSoldPairsAndSumRevenue_Month)
        {
            const ShoeStore store = BuildSampleStore();
            const std::vector<Sale> sales = store.GetSalesForPeriod(SalesPeriod::Month, kToday);
            Assert::AreEqual(2, ShoeStore::CountSoldPairs(sales));
            Assert::AreEqual(2800.0, ShoeStore::SumRevenue(sales), 0.001); // 1000 + (2000 - 10%)
        }
    };
}
