#include "Shoe.h"
#include "FormatUtils.h"
#include <stdexcept>

Shoe::Shoe(std::string article,
    const std::string model,
    const std::string style,
    const std::string color,
    const Season season,
    const double price,
    const std::string country,
    const std::map<int, int> pairsBySize)
    const : article_(std::move(article)),
    const model_(std::move(model)),
    const style_(std::move(style)),
    const color_(std::move(color)),
    const season_(season),
    const  price_(price),
    const country_(std::move(country)),
    const pairsBySize_(std::move(pairsBySize)) {
    // Проверяем входные данные сразу, чтобы в магазине никогда не
    // оказалось обуви с некорректными значениями.
    if (article_.empty()) {
        throw std::invalid_argument("Shoe: артикул не может быть пустым");
    }
    if (price_ < 0.0) {
        throw std::invalid_argument("Shoe: цена не может быть отрицательной");
    }
    for (const auto& [size, pairs] : pairsBySize_) {
        if (size <= 0) {
            throw std::invalid_argument("Shoe: размер должен быть положительным");
        }
        if (pairs < 0) {
            throw std::invalid_argument("Shoe: количество пар не может быть отрицательным");
        }
    }
}

const std::string& Shoe::GetArticle() const { return article_; }
const std::string& Shoe::GetModel() const { return model_; }
const std::string& Shoe::GetStyle() const { return style_; }
const std::string& Shoe::GetColor() const { return color_; }
Season Shoe::GetSeason() const { return season_; }
double Shoe::GetPrice() const { return price_; }
const std::string& Shoe::GetCountry() const { return country_; }

int Shoe::GetPairsCount(int size) const {
    const auto it = pairsBySize_.find(size);
    return it == pairsBySize_.end() ? 0 : it->second;
}

int Shoe::GetTotalPairs() const {
    int total = 0;
    for (const auto& sizeAndPairs : pairsBySize_) {
        total += sizeAndPairs.second; // second - количество пар этого размера
    }
    return total;
}

std::vector<int> Shoe::GetAvailableSizes() const {
    // std::map хранит ключи отсортированными, поэтому размеры
    // получаются сразу по возрастанию.
    std::vector<int> sizes;
    for (const auto& [size, pairs] : pairsBySize_) {
        if (pairs > 0) {
            sizes.push_back(size);
        }
    }
    return sizes;
}

void Shoe::RemovePairs(int size, int count) {
    if (count <= 0) {
        throw std::invalid_argument("Shoe::RemovePairs: количество пар должно быть больше нуля");
    }
    const int available = GetPairsCount(size);
    if (available < count) {
        throw std::out_of_range("Shoe::RemovePairs: недостаточно пар указанного размера");
    }
    pairsBySize_[size] = available - count;
}

std::string Shoe::GetDepartmentName() const {
    return DepartmentName(GetDepartment());
}

std::string Shoe::GetStockInfo() const {
    std::string stock;
    for (const int size : GetAvailableSizes()) {
        if (!stock.empty()) {
            stock += ", ";
        }
        stock += std::to_string(size) + " - " + std::to_string(GetPairsCount(size));
    }
    return stock.empty() ? "нет в наличии" : stock;
}

std::string Shoe::GetInfo() const {
    return article_ + " «" + model_ + "» (" + style_ + ")" +
        " [" + GetDepartmentName() + "]" +
        " | цвет: " + color_ +
        " | сезон: " + SeasonName(season_) +
        " | цена: " + FormatUtils::FormatMoney(price_) + " руб." +
        " | страна-изготовитель: " + country_ +
        " | " + GetSpecificInfo() +
        " | в наличии (размер - пар): " + GetStockInfo();
}
