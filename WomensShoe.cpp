#include "WomensShoe.h"
#include "FormatUtils.h"
#include <stdexcept>

WomensShoe::WomensShoe(std::string article,
    std::string model,
    std::string style,
    std::string color,
    Season season,
    double price,
    std::string country,
    std::map<int, int> pairsBySize,
    double heelHeightCm)
    : Shoe(std::move(article), std::move(model), std::move(style), std::move(color),
        season, price, std::move(country), std::move(pairsBySize)),
    heelHeightCm_(heelHeightCm) {
    if (heelHeightCm_ < 0.0) {
        throw std::invalid_argument("WomensShoe: высота каблука не может быть отрицательной");
    }
}

double WomensShoe::GetHeelHeightCm() const { return heelHeightCm_; }

Department WomensShoe::GetDepartment() const {
    return Department::Women;
}

std::string WomensShoe::GetSpecificInfo() const {
    return "высота каблука: " + FormatUtils::FormatDecimal(heelHeightCm_) + " см";
}
