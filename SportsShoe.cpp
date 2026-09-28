#include "SportsShoe.h"

SportsShoe::SportsShoe(std::string article,
    const std::string model,
    const std::string style,
    const std::string color,
    const Season season,
    const double price,
    const std::string country,
    const std::map<int, int> pairsBySize,
    const std::string sportType)
    : Shoe(std::move(article), std::move(model), std::move(style), std::move(color),
        season, price, std::move(country), std::move(pairsBySize)),
    sportType_(std::move(sportType)) {
}

const std::string& SportsShoe::GetSportType() const { return sportType_; }

Department SportsShoe::GetDepartment() const {
    return Department::Sports;
}

std::string SportsShoe::GetSpecificInfo() const {
    return "вид спорта: " + sportType_;
}
