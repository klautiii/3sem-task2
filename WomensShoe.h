#pragma once
//
// WomensShoe.h
// Класс-наследник "Женская обувь". Дополнительное (специфичное для
// отдела) поле: высота каблука в сантиметрах.
//
#include "Shoe.h"

/**
 * @brief Обувь женского отдела. Добавляет к общим полям Shoe высоту каблука.
 */
class WomensShoe : public Shoe {
public:
    /**
     * @brief Создаёт женскую обувь.
     * @param article       Артикул.
     * @param model         Название модели.
     * @param style         Фасон.
     * @param color         Цвет.
     * @param season        Сезон.
     * @param price         Цена одной пары, руб.
     * @param country       Страна-изготовитель.
     * @param pairsBySize   Остатки на складе: размер -> количество пар.
     * @param heelHeightCm  Высота каблука, см (0 - без каблука).
     * @throws std::invalid_argument если высота каблука отрицательная
     *         (а также в случаях, описанных у конструктора Shoe).
     */
    WomensShoe(std::string article,
        std::string model,
        std::string style,
        std::string color,
        Season season,
        double price,
        std::string country,
        std::map<int, int> pairsBySize,
        double heelHeightCm);

    /** @brief Возвращает высоту каблука, см. */
    double GetHeelHeightCm() const;

    /** @brief Возвращает Department::Women. */
    Department GetDepartment() const override;

protected:
    /** @brief Возвращает строку вида "высота каблука: 8.00 см". */
    std::string GetSpecificInfo() const override;

private:
    double heelHeightCm_ = 0.0; // инициализатор по умолчанию - защита от забытого поля в будущем конструкторе
};
