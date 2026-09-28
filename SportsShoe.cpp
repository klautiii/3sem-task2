#include "SportsShoe.h"

SportsShoe::SportsShoe(std::string article,
    std::string model,
    std::string style,
    std::string color,
    Season season,
    double price,
    std::string country,
    std::map<int, int> pairsBySize,
    std::string sportType)
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
