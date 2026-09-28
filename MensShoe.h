#pragma once
//
// MensShoe.h
// Класс-наследник "Мужская обувь". Дополнительное (специфичное для
// отдела) поле: тип застёжки (шнурки, молния, без застёжки и т.п.).
//
#include "Shoe.h"

/**
 * @brief Обувь мужского отдела. Добавляет к общим полям Shoe тип застёжки.
 */
class MensShoe : public Shoe {
public:
    /**
     * @brief Создаёт мужскую обувь.
     * @param article      Артикул.
     * @param model        Название модели.
     * @param style        Фасон.
     * @param color        Цвет.
     * @param season       Сезон.
     * @param price        Цена одной пары, руб.
     * @param country      Страна-изготовитель.
     * @param pairsBySize  Остатки на складе: размер -> количество пар.
     * @param closureType  Тип застёжки, например "шнурки".
     */
    MensShoe(std::string article,
        std::string model,
        std::string style,
        std::string color,
        Season season,
        double price,
        std::string country,
        std::map<int, int> pairsBySize,
        std::string closureType);

    /** @brief Возвращает тип застёжки. */
    const std::string& GetClosureType() const;

    /** @brief Возвращает Department::Men. */
    Department GetDepartment() const override;

protected:
    /** @brief Возвращает строку вида "застёжка: шнурки". */
    std::string GetSpecificInfo() const override;

private:
    std::string closureType_;
};
