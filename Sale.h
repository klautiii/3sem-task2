#pragma once
//
// Sale.h
// Класс "Продажа" - запись об одной продаже обуви покупателю.
// Создаётся в ShoeStore::Sell() (задание 4) и используется для отчёта о
// продажах за выбранный период (задание 5 варианта "Магазин обуви").
//
#include <string>
#include "Date.h"

/**
 * @brief Неизменяемая запись об одной продаже: какая обувь (артикул и
 *        модель) какого размера продана, сколько пар, в какую дату, по
 *        какой цене и с какой скидкой. Итоговая сумма считается один раз
 *        в конструкторе.
 */
class Sale {
public:
    /**
     * @brief Создаёт запись о продаже.
     * @param article         Артикул проданной обуви.
     * @param model           Название модели (для удобного вывода отчёта).
     * @param size            Размер.
     * @param quantity        Количество проданных пар (больше нуля).
     * @param unitPrice       Цена одной пары без скидки, руб.
     * @param discountPercent Скидка по дисконтной карте, % (0 - без скидки).
     * @param cardNumber      Номер дисконтной карты (пустая строка - без карты).
     * @param saleDate        Дата продажи.
     * @throws std::invalid_argument если количество не положительное,
     *         цена отрицательная или скидка вне диапазона 0..100%.
     */
    Sale(std::string article,
        std::string model,
        int size,
        int quantity,
        double unitPrice,
        int discountPercent,
        std::string cardNumber,
        Date saleDate);

    /** @brief Возвращает артикул проданной обуви. */
    const std::string& GetArticle() const;

    /** @brief Возвращает название модели. */
    const std::string& GetModel() const;

    /** @brief Возвращает размер. */
    int GetSize() const;

    /** @brief Возвращает количество проданных пар. */
    int GetQuantity() const;

    /** @brief Возвращает цену одной пары без скидки, руб. */
    double GetUnitPrice() const;

    /** @brief Возвращает скидку, % (0 - продажа без скидки). */
    int GetDiscountPercent() const;

    /** @brief Возвращает номер дисконтной карты (пустая строка - без карты). */
    const std::string& GetCardNumber() const;

    /** @brief Возвращает дату продажи. */
    const Date& GetSaleDate() const;

    /** @brief Возвращает итоговую сумму продажи с учётом скидки, руб. */
    double GetTotalPrice() const;

    /**
     * @brief Формирует однострочное человекочитаемое описание продажи.
     *        Потоки ввода-вывода не используются - вызывающий код сам
     *        печатает возвращённую строку.
     */
    std::string GetInfo() const;

private:
    std::string article_;
    std::string model_;
    // Инициализаторы по умолчанию - защита от забытого поля в будущем конструкторе.
    int size_ = 0;
    int quantity_ = 0;
    double unitPrice_ = 0.0;
    int discountPercent_ = 0;
    std::string cardNumber_;
    Date saleDate_;
    double totalPrice_ = 0.0;
};
