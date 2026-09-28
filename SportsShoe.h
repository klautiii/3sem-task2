#pragma once
//
// SportsShoe.h
// Класс-наследник "Спортивная обувь". Дополнительное (специфичное для
// отдела) поле: вид спорта, для которого предназначена обувь.
//
#include "Shoe.h"

/**
 * @brief Обувь спортивного отдела. Добавляет к общим полям Shoe вид спорта.
 */
class SportsShoe : public Shoe {
public:
    /**
     * @brief Создаёт спортивную обувь.
     * @param article      Артикул.
     * @param model        Название модели.
     * @param style        Фасон.
     * @param color        Цвет.
     * @param season       Сезон.
     * @param price        Цена одной пары, руб.
     * @param country      Страна-изготовитель.
     * @param pairsBySize  Остатки на складе: размер -> количество пар.
     * @param sportType    Вид спорта, например "бег", "футбол".
     */
    SportsShoe(std::string article,
        std::string model,
        std::string style,
        std::string color,
        Season season,
        double price,
        std::string country,
        std::map<int, int> pairsBySize,
        std::string sportType);

    /** @brief Возвращает вид спорта. */
    const std::string& GetSportType() const;

    /** @brief Возвращает Department::Sports. */
    Department GetDepartment() const override;

protected:
    /** @brief Возвращает строку вида "вид спорта: бег". */
    std::string GetSpecificInfo() const override;

private:
    std::string sportType_;
};
