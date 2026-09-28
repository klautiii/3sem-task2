#pragma once
//
// Shoe.h
// Базовый класс "Обувь" магазина обуви.
//
// Важно: библиотека нигде не использует потоки ввода-вывода (<iostream>,
// <fstream>, <sstream> и т.п.) - весь вывод собирается в обычные
// std::string и печатается вызывающим кодом (см. ShoeStoreApp/main.cpp).
//
// Общие поля (по описанию предметной области "Магазин обуви"): артикул,
// модель, фасон, цвет, сезон, цена, страна-изготовитель и количество пар
// каждого размера на складе. Отдел (мужская, женская, детская,
// спортивная обувь) реализован через полиморфизм: каждый класс-наследник
// (MensShoe, WomensShoe, KidsShoe, SportsShoe) переопределяет
// GetDepartment() и добавляет собственные поля, специфичные для отдела.
//
#include <map>
#include <string>
#include <vector>
#include "Department.h"
#include "Season.h"

/**
 * @brief Базовый класс, представляющий одну модель обуви (один артикул)
 *        и её остатки на складе по размерам. Объявляет точки расширения
 *        (GetDepartment / GetSpecificInfo), которые классы-наследники
 *        переопределяют.
 */
class Shoe {
public:
    /**
     * @brief Создаёт обувь с общими атрибутами.
     * @param article      Артикул (уникальный номер модели в магазине), например "M-1001".
     * @param model        Название модели, например "Классик".
     * @param style        Фасон, например "оксфорды", "сапоги", "кроссовки".
     * @param color        Цвет.
     * @param season       Сезон (зимняя, летняя, весна-осень, всесезонная).
     * @param price        Цена одной пары, руб.
     * @param country      Страна-изготовитель.
     * @param pairsBySize  Остатки на складе: размер -> количество пар.
     * @throws std::invalid_argument если артикул пустой, цена
     *         отрицательная, размер не положительный или количество пар
     *         отрицательное.
     */
    Shoe(std::string article,
        std::string model,
        std::string style,
        std::string color,
        Season season,
        double price,
        std::string country,
        std::map<int, int> pairsBySize);

    /** @brief Виртуальный деструктор, необходим для полиморфных базовых классов. */
    virtual ~Shoe() = default;

    /** @brief Возвращает артикул. */
    const std::string& GetArticle() const;

    /** @brief Возвращает название модели. */
    const std::string& GetModel() const;

    /** @brief Возвращает фасон. */
    const std::string& GetStyle() const;

    /** @brief Возвращает цвет. */
    const std::string& GetColor() const;

    /** @brief Возвращает сезон. */
    Season GetSeason() const;

    /** @brief Возвращает цену одной пары, руб. */
    double GetPrice() const;

    /** @brief Возвращает страну-изготовителя. */
    const std::string& GetCountry() const;

    /**
     * @brief Возвращает количество пар указанного размера на складе.
     * @param size Размер.
     * @return Количество пар; 0, если такого размера нет.
     */
    int GetPairsCount(int size) const;

    /** @brief Возвращает общее количество пар всех размеров на складе. */
    int GetTotalPairs() const;

    /** @brief Возвращает размеры, которые сейчас есть в наличии (количество пар больше нуля), по возрастанию. */
    std::vector<int> GetAvailableSizes() const;

    /**
     * @brief Списывает со склада @p count пар размера @p size (используется
     *        при продаже, см. ShoeStore::Sell()).
     * @param size  Размер.
     * @param count Сколько пар списать.
     * @throws std::invalid_argument если @p count меньше или равен нулю.
     * @throws std::out_of_range если пар такого размера меньше, чем @p count.
     */
    void RemovePairs(int size, int count);

    /**
     * @brief Возвращает отдел магазина, к которому относится обувь.
     *        Переопределяется в каждом классе-наследнике.
     */
    virtual Department GetDepartment() const = 0;

    /** @brief Возвращает название отдела на русском языке (например, "Мужская обувь"). */
    std::string GetDepartmentName() const;

    /**
     * @brief Формирует однострочное человекочитаемое описание обуви:
     *        общие поля, поля, специфичные для отдела (через
     *        GetSpecificInfo()), и остатки по размерам.
     * @return Отформатированная строка (без завершающего перевода строки).
     */
    virtual std::string GetInfo() const;

protected:
    /**
     * @brief Возвращает часть описания, специфичную для отдела (например,
     *        высоту каблука для женской обуви). Реализуется в каждом
     *        классе-наследнике.
     */
    virtual std::string GetSpecificInfo() const = 0;

private:
    std::string article_;
    std::string model_;
    std::string style_;
    std::string color_;
    // Инициализаторы по умолчанию - защита от забытого поля в будущем конструкторе.
    Season season_ = Season::AllSeason;
    double price_ = 0.0;
    std::string country_;
    std::map<int, int> pairsBySize_;

    /** @brief Возвращает остатки в виде строки "40 - 2, 41 - 3" или "нет в наличии". */
    std::string GetStockInfo() const;
};
