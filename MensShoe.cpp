#include "MensShoe.h"

MensShoe::MensShoe(std::string article,
    const std::string model,
    const std::string style,
    const std::string color,
    const Season season,
    const double price,
    const std::string country,
    const std::map<int, int> pairsBySize,
    const std::string closureType)
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
