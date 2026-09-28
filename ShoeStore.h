#pragma once
//
// ShoeStore.h
// Класс "Магазин обуви" - ассортимент обуви, дисконтные карты и журнал
// продаж. Реализует пять заданий варианта "Магазин обуви":
//   1) Выдавать информацию о количестве пар по заданному артикулу,
//      размеру, цвету и т.д.
//   2) Выдавать ассортимент обуви по отделам, сезону.
//   3) Выдавать информацию о данной обуви (цена, страна-изготовитель).
//   4) Продавать выбранный товар.
//   5) Показывать продажи за выбранный период времени.
//
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Date.h"
#include "Department.h"
#include "DiscountCard.h"
#include "Sale.h"
#include "SaleStatus.h"
#include "SalesPeriod.h"
#include "Season.h"
#include "Shoe.h"

/**
 * @brief Объединяет ассортимент именованного магазина обуви, его
 *        дисконтные карты и журнал продаж, отвечая на запросы,
 *        требуемые вариантом задания "Магазин обуви".
 *
 *        Все методы-запросы возвращают указатели на КОНСТАНТНУЮ обувь
 *        (std::shared_ptr<const Shoe>): получить данные можно, а
 *        изменить остатки в обход журнала продаж - нельзя. Остатки
 *        меняются только через Sell().
 */
class ShoeStore {
public:
    /**
     * @brief Создаёт пустой магазин с заданным названием.
     * @param name Человекочитаемое название магазина.
     */
    explicit ShoeStore(std::string name);

    /** @brief Возвращает название магазина. */
    const std::string& GetName() const;

    /**
     * @brief Добавляет обувь в ассортимент.
     * @param shoe Умный указатель на экземпляр Shoe (класса-наследника).
     * @throws std::invalid_argument если указатель пустой или обувь с
     *         таким артикулом уже есть в магазине.
     */
    void AddShoe(std::shared_ptr<Shoe> shoe);

    /**
     * @brief Регистрирует дисконтную карту постоянного клиента.
     * @param card Карта для регистрации.
     * @throws std::invalid_argument если карта с таким номером уже зарегистрирована.
     */
    void RegisterDiscountCard(const DiscountCard& card);

    /**
     * @brief Добавляет в журнал запись о продаже, совершённой ранее
     *        (история продаж). Остатки на складе при этом НЕ меняются -
     *        для новой продажи используйте Sell().
     * @param sale Продажа для записи в журнал.
     */
    void RegisterSale(const Sale& sale);

    /** @brief Возвращает весь ассортимент магазина (только для чтения). */
    std::vector<std::shared_ptr<const Shoe>> GetAllShoes() const;

    /** @brief Возвращает все зарегистрированные дисконтные карты. */
    const std::vector<DiscountCard>& GetAllDiscountCards() const;

    /**
     * @brief Ищет дисконтную карту по номеру.
     * @param number Номер карты.
     * @return Копия найденной карты, либо std::nullopt, если такой карты нет.
     */
    std::optional<DiscountCard> FindDiscountCard(const std::string& number) const;

    // ---- Задание 1: количество пар по артикулу, размеру, цвету ----

    /**
     * @brief Находит обувь, подходящую под заданные условия. Каждое
     *        условие необязательное: std::nullopt означает "любой".
     *        Артикул и цвет сравниваются без учёта регистра.
     * @param article Артикул.
     * @param size    Размер (подходит обувь, у которой есть хотя бы одна пара этого размера).
     * @param color   Цвет.
     * @return Вся подходящая обувь, в порядке добавления в магазин.
     */
    std::vector<std::shared_ptr<const Shoe>> FindShoes(const std::optional<std::string>& article,
        const std::optional<int>& size,
        const std::optional<std::string>& color) const;

    /**
     * @brief Считает количество пар по тем же условиям, что и FindShoes().
     *        Если размер задан - считаются только пары этого размера,
     *        иначе - пары всех размеров.
     * @param article Артикул (std::nullopt - любой).
     * @param size    Размер (std::nullopt - любой).
     * @param color   Цвет (std::nullopt - любой).
     * @return Суммарное количество пар.
     */
    int CountPairs(const std::optional<std::string>& article,
        const std::optional<int>& size,
        const std::optional<std::string>& color) const;

    // ---- Задание 2: ассортимент по отделам и сезону ----

    /**
     * @brief Возвращает ассортимент выбранного отдела и/или сезона.
     * @param department Отдел (std::nullopt - все отделы).
     * @param season     Сезон (std::nullopt - все сезоны).
     * @return Подходящая обувь, в порядке добавления в магазин.
     */
    std::vector<std::shared_ptr<const Shoe>> GetAssortment(const std::optional<Department>& department,
        const std::optional<Season>& season) const;

    // ---- Задание 3: информация о данной обуви ----

    /**
     * @brief Ищет обувь по артикулу (без учёта регистра).
     * @param article Артикул.
     * @return Найденная обувь, либо nullptr, если такого артикула нет.
     */
    std::shared_ptr<const Shoe> FindShoeByArticle(const std::string& article) const;

    // ---- Задание 4: продажа ----

    /**
     * @brief Продаёт обувь: проверяет наличие и дисконтную карту,
     *        списывает пары со склада и записывает продажу в журнал.
     *        Если какая-то проверка не прошла, магазин не меняется.
     * @param article    Артикул.
     * @param size       Размер.
     * @param quantity   Количество пар.
     * @param cardNumber Номер дисконтной карты (пустая строка - без карты).
     * @param saleDate   Дата продажи.
     * @return SaleStatus::Success при успешной продаже (запись о ней -
     *         последний элемент GetAllSales()), иначе - причина отказа.
     */
    SaleStatus Sell(const std::string& article,
        int size,
        int quantity,
        const std::string& cardNumber,
        const Date& saleDate);

    // ---- Задание 5: продажи за выбранный период ----

    /** @brief Возвращает весь журнал продаж в порядке регистрации. */
    const std::vector<Sale>& GetAllSales() const;

    /**
     * @brief Возвращает продажи за стандартный период (неделя/месяц/год),
     *        заканчивающийся датой @p asOf (включительно).
     * @param period Отчётный период.
     * @param asOf   Опорная дата (как правило - текущая дата).
     * @return Подходящие продажи в порядке регистрации.
     */
    std::vector<Sale> GetSalesForPeriod(SalesPeriod period, const Date& asOf) const;

    /**
     * @brief Возвращает продажи за произвольный период с @p from по @p to
     *        (обе даты включительно).
     * @param from Начало периода.
     * @param to   Конец периода.
     * @return Подходящие продажи в порядке регистрации; пустой список,
     *         если @p from позже @p to.
     */
    std::vector<Sale> GetSalesBetween(const Date& from, const Date& to) const;

    /** @brief Суммирует количество проданных пар по списку продаж. */
    static int CountSoldPairs(const std::vector<Sale>& sales);

    /** @brief Суммирует выручку (с учётом скидок) по списку продаж. */
    static double SumRevenue(const std::vector<Sale>& sales);

private:
    std::string name_;
    std::vector<std::shared_ptr<Shoe>> shoes_;
    std::vector<DiscountCard> discountCards_;
    std::vector<Sale> sales_;

    /**
     * @brief Ищет обувь по артикулу и возвращает ИЗМЕНЯЕМЫЙ указатель -
     *        только для внутреннего использования в Sell(), где нужно
     *        списать пары со склада.
     */
    std::shared_ptr<Shoe> FindShoeForUpdate(const std::string& article) const;

    /** @brief Возвращает длину стандартного отчётного периода в днях (7 / 30 / 365). */
    static int PeriodLengthDays(SalesPeriod period);
};
