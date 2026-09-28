#include "Sale.h"
#include "DiscountCard.h"
#include "FormatUtils.h"
#include <stdexcept>

namespace {

    /** @brief 100% - база для перевода процентов скидки в доли. */
    constexpr double kPercentBase = 100.0;

    /**
     * @brief Считает итоговую сумму продажи: цена пары * количество,
     *        уменьшенная на скидку в процентах.
     */
    double CalculateTotal(double unitPrice, int quantity, int discountPercent) {
        const double fullPrice = unitPrice * quantity;
        const double discountShare = discountPercent / kPercentBase;
        return fullPrice * (1.0 - discountShare);
    }

}

Sale::Sale(std::string article,
    const std::string model,
    const int size,
    const int quantity,
    const double unitPrice,
    const int discountPercent,
    const std::string cardNumber,
    const Date saleDate)
    const : article_(std::move(article)),
    const model_(std::move(model)),
    const size_(size),
    const quantity_(quantity),
    const unitPrice_(unitPrice),
    const discountPercent_(discountPercent),
    const cardNumber_(std::move(cardNumber)),
    saleDate_(saleDate) {
    if (quantity_ <= 0) {
        throw std::invalid_argument("Sale: количество пар должно быть больше нуля");
    }
    if (unitPrice_ < 0.0) {
        throw std::invalid_argument("Sale: цена не может быть отрицательной");
    }
    if (discountPercent_ < 0 || discountPercent_ > DiscountCard::kMaxDiscountPercent) {
        throw std::invalid_argument("Sale: скидка должна быть от 0 до 100%");
    }
    totalPrice_ = CalculateTotal(unitPrice_, quantity_, discountPercent_);
}

const std::string& Sale::GetArticle() const { return article_; }
const std::string& Sale::GetModel() const { return model_; }
int Sale::GetSize() const { return size_; }
int Sale::GetQuantity() const { return quantity_; }
double Sale::GetUnitPrice() const { return unitPrice_; }
int Sale::GetDiscountPercent() const { return discountPercent_; }
const std::string& Sale::GetCardNumber() const { return cardNumber_; }
const Date& Sale::GetSaleDate() const { return saleDate_; }
double Sale::GetTotalPrice() const { return totalPrice_; }

std::string Sale::GetInfo() const {
    const std::string discountInfo = cardNumber_.empty()
        ? ", без скидки"
        : ", скидка " + std::to_string(discountPercent_) + "% (карта " + cardNumber_ + ")";

    return saleDate_.ToString() + ": " + article_ + " «" + model_ + "»" +
        ", размер " + std::to_string(size_) +
        ", пар: " + std::to_string(quantity_) +
        " x " + FormatUtils::FormatMoney(unitPrice_) + " руб." +
        discountInfo +
        " = " + FormatUtils::FormatMoney(totalPrice_) + " руб.";
}
