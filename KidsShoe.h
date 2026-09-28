#pragma once
//
// KidsShoe.h
// Класс-наследник "Детская обувь". Дополнительные (специфичные для
// отдела) поля: возраст ребёнка, для которого предназначена обувь
// (от ... до ... лет).
//
#include "Shoe.h"

/**
 * @brief Обувь детского отдела. Добавляет к общим полям Shoe
 *        рекомендуемый возраст ребёнка.
 */
class KidsShoe : public Shoe {
public:
    /**
     * @brief Создаёт детскую обувь.
     * @param article      Артикул.
     * @param model        Название модели.
     * @param style        Фасон.
     * @param color        Цвет.
     * @param season       Сезон.
     * @param price        Цена одной пары, руб.
     * @param country      Страна-изготовитель.
     * @param pairsBySize  Остатки на складе: размер -> количество пар.
     * @param minAgeYears  Минимальный рекомендуемый возраст, лет.
     * @param maxAgeYears  Максимальный рекомендуемый возраст, лет.
     * @throws std::invalid_argument если возраст отрицательный или
     *         minAgeYears больше maxAgeYears (а также в случаях,
     *         описанных у конструктора Shoe).
     */
    KidsShoe(std::string article,
        const std::string model,
        const std::string style,
        const std::string color,
        const Season season,
        const double price,
        const std::string country,
        const std::map<int, int> pairsBySize,
        const int minAgeYears,
        const int maxAgeYears);

    /** @brief Возвращает минимальный рекомендуемый возраст, лет. */
    int GetMinAgeYears() const;

    /** @brief Возвращает максимальный рекомендуемый возраст, лет. */
    int GetMaxAgeYears() const;

    /** @brief Возвращает Department::Kids. */
    Department GetDepartment() const override;

protected:
    /** @brief Возвращает строку вида "возраст: от 3 до 6 лет". */
    std::string GetSpecificInfo() const override;

private:
    // Инициализаторы по умолчанию - защита от забытого поля в будущем конструкторе.
    int minAgeYears_ = 0;
    int maxAgeYears_ = 0;
};
