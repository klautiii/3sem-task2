#include "KidsShoe.h"
#include <stdexcept>

KidsShoe::KidsShoe(std::string article,
    const std::string model,
    const std::string style,
    const std::string color,
    const Season season,
    const double price,
    const std::string country,
    const std::map<int, int> pairsBySize,
    const int minAgeYears,
    const int maxAgeYears)
    : Shoe(std::move(article), std::move(model), std::move(style), std::move(color),
        season, price, std::move(country), std::move(pairsBySize)),
    const minAgeYears_(minAgeYears),
    maxAgeYears_(maxAgeYears) {
    if (minAgeYears_ < 0 || minAgeYears_ > maxAgeYears_) {
        throw std::invalid_argument("KidsShoe: некорректный диапазон возраста");
    }
}

int KidsShoe::GetMinAgeYears() const { return minAgeYears_; }
int KidsShoe::GetMaxAgeYears() const { return maxAgeYears_; }

Department KidsShoe::GetDepartment() const {
    return Department::Kids;
}

std::string KidsShoe::GetSpecificInfo() const {
    return "возраст: от " + std::to_string(minAgeYears_) +
        " до " + std::to_string(maxAgeYears_) + " лет";
}
