#include "DiscountCard.h"
#include <stdexcept>

DiscountCard::DiscountCard(std::string number, std::string ownerName, int discountPercent)
    : number_(std::move(number)),
    ownerName_(std::move(ownerName)),
    discountPercent_(discountPercent) {
    if (number_.empty()) {
        throw std::invalid_argument("DiscountCard: номер карты не может быть пустым");
    }
    if (discountPercent_ < 0 || discountPercent_ > kMaxDiscountPercent) {
        throw std::invalid_argument("DiscountCard: скидка должна быть от 0 до 100%");
    }
}

const std::string& DiscountCard::GetNumber() const { return number_; }
const std::string& DiscountCard::GetOwnerName() const { return ownerName_; }
int DiscountCard::GetDiscountPercent() const { return discountPercent_; }

std::string DiscountCard::GetInfo() const {
    return "карта " + number_ + " - " + ownerName_ +
        ", скидка " + std::to_string(discountPercent_) + "%";
}
