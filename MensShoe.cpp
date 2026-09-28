#include "MensShoe.h"

MensShoe::MensShoe(std::string article,
    std::string model,
    std::string style,
    std::string color,
    Season season,
    double price,
    std::string country,
    std::map<int, int> pairsBySize,
    std::string closureType)
    : Shoe(std::move(article), std::move(model), std::move(style), std::move(color),
        season, price, std::move(country), std::move(pairsBySize)),
    closureType_(std::move(closureType)) {
}

const std::string& MensShoe::GetClosureType() const { return closureType_; }

Department MensShoe::GetDepartment() const {
    return Department::Men;
}

std::string MensShoe::GetSpecificInfo() const {
    return "застёжка: " + closureType_;
}
