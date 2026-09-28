#pragma once
//
// DiscountCard.h
// Класс "Дисконтная карта" - система скидок для постоянных клиентов
// (по описанию предметной области "Магазин обуви").
//
#include <string>

/**
 * @brief Дисконтная карта постоянного клиента: номер карты, владелец и
 *        размер скидки в процентах.
 */
class DiscountCard {
public:
    /** @brief Максимально допустимая скидка, %. */
    static constexpr int kMaxDiscountPercent = 100;

    /**
     * @brief Создаёт дисконтную карту.
     * @param number          Номер карты (например, "1001").
     * @param ownerName       ФИО владельца.
     * @param discountPercent Скидка, % (от 0 до kMaxDiscountPercent).
     * @throws std::invalid_argument если номер карты пустой или скидка
     *         вне диапазона 0..kMaxDiscountPercent.
     */
    DiscountCard(std::string number, std::string ownerName, int discountPercent);

    /** @brief Возвращает номер карты. */
    const std::string& GetNumber() const;

    /** @brief Возвращает ФИО владельца карты. */
    const std::string& GetOwnerName() const;

    /** @brief Возвращает размер скидки, %. */
    int GetDiscountPercent() const;

    /**
     * @brief Формирует однострочное описание карты, например
     *        "карта 1001 - Иванов Иван, скидка 5%".
     */
    std::string GetInfo() const;

private:
    std::string number_;
    std::string ownerName_;
    int discountPercent_ = 0; // инициализатор по умолчанию - защита от забытого поля в будущем конструкторе
};
