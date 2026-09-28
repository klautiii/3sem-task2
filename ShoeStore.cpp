#include "ShoeStore.h"
#include "FormatUtils.h"
#include <stdexcept>

namespace {

    constexpr int kDaysInWeek = 7;    ///< Длина периода "неделя", дней.
    constexpr int kDaysInMonth = 30;  ///< Длина периода "месяц", дней.
    constexpr int kDaysInYear = 365;  ///< Длина периода "год", дней.

    /** @brief Скидка, если покупатель не предъявил дисконтную карту, %. */
    constexpr int kNoDiscountPercent = 0;

    /**
     * @brief Проверяет, подходит ли обувь под условия задания 1
     *        (std::nullopt в любом условии означает "любой").
     */
    bool MatchesStockQuery(const Shoe& shoe,
        const std::optional<std::string>& article,
        const std::optional<int>& size,
        const std::optional<std::string>& color) {
        const bool articleMatches = !article || FormatUtils::EqualsIgnoreCase(shoe.GetArticle(), *article);
        const bool sizeMatches = !size || shoe.GetPairsCount(*size) > 0;
        const bool colorMatches = !color || FormatUtils::EqualsIgnoreCase(shoe.GetColor(), *color);
        return articleMatches && sizeMatches && colorMatches;
    }

}

ShoeStore::ShoeStore(std::string name) : name_(std::move(name)) {}

const std::string& ShoeStore::GetName() const { return name_; }

void ShoeStore::AddShoe(std::shared_ptr<Shoe> shoe) {
    if (!shoe) {
        throw std::invalid_argument("ShoeStore::AddShoe: пустой указатель на обувь");
    }
    if (FindShoeForUpdate(shoe->GetArticle())) {
        throw std::invalid_argument("ShoeStore::AddShoe: обувь с таким артикулом уже есть");
    }
    shoes_.push_back(std::move(shoe));
}

void ShoeStore::RegisterDiscountCard(const DiscountCard& card) {
    if (FindDiscountCard(card.GetNumber())) {
        throw std::invalid_argument("ShoeStore::RegisterDiscountCard: карта с таким номером уже есть");
    }
    discountCards_.push_back(card);
}

void ShoeStore::RegisterSale(const Sale& sale) {
    sales_.push_back(sale);
}

std::vector<std::shared_ptr<const Shoe>> ShoeStore::GetAllShoes() const {
    return std::vector<std::shared_ptr<const Shoe>>(shoes_.begin(), shoes_.end());
}

const std::vector<DiscountCard>& ShoeStore::GetAllDiscountCards() const {
    return discountCards_;
}

std::optional<DiscountCard> ShoeStore::FindDiscountCard(const std::string& number) const {
    for (const auto& card : discountCards_) {
        if (card.GetNumber() == number) {
            return card;
        }
    }
    return std::nullopt;
}

std::shared_ptr<Shoe> ShoeStore::FindShoeForUpdate(const std::string& article) const {
    for (const auto& shoe : shoes_) {
        if (FormatUtils::EqualsIgnoreCase(shoe->GetArticle(), article)) {
            return shoe;
        }
    }
    return nullptr;
}

// ---- Задание 1 ----

std::vector<std::shared_ptr<const Shoe>> ShoeStore::FindShoes(const std::optional<std::string>& article,
    const std::optional<int>& size,
    const std::optional<std::string>& color) const {
    std::vector<std::shared_ptr<const Shoe>> result;
    for (const auto& shoe : shoes_) {
        if (MatchesStockQuery(*shoe, article, size, color)) {
            result.push_back(shoe);
        }
    }
    return result;
}

int ShoeStore::CountPairs(const std::optional<std::string>& article,
    const std::optional<int>& size,
    const std::optional<std::string>& color) const {
    int total = 0;
    for (const auto& shoe : FindShoes(article, size, color)) {
        total += size ? shoe->GetPairsCount(*size) : shoe->GetTotalPairs();
    }
    return total;
}

// ---- Задание 2 ----

std::vector<std::shared_ptr<const Shoe>> ShoeStore::GetAssortment(const std::optional<Department>& department,
    const std::optional<Season>& season) const {
    std::vector<std::shared_ptr<const Shoe>> result;
    for (const auto& shoe : shoes_) {
        const bool departmentMatches = !department || shoe->GetDepartment() == *department;
        const bool seasonMatches = !season || shoe->GetSeason() == *season;
        if (departmentMatches && seasonMatches) {
            result.push_back(shoe);
        }
    }
    return result;
}

// ---- Задание 3 ----

std::shared_ptr<const Shoe> ShoeStore::FindShoeByArticle(const std::string& article) const {
    return FindShoeForUpdate(article);
}

// ---- Задание 4 ----

SaleStatus ShoeStore::Sell(const std::string& article,
    int size,
    int quantity,
    const std::string& cardNumber,
    const Date& saleDate) {
    // Сначала выполняем ВСЕ проверки и только потом что-либо меняем -
    // так при любой ошибке магазин остаётся в исходном состоянии.
    if (quantity <= 0) {
        return SaleStatus::InvalidQuantity;
    }

    const std::shared_ptr<Shoe> shoe = FindShoeForUpdate(article);
    if (!shoe) {
        return SaleStatus::UnknownArticle;
    }

    const int available = shoe->GetPairsCount(size);
    if (available == 0) {
        return SaleStatus::SizeNotAvailable;
    }
    if (available < quantity) {
        return SaleStatus::NotEnoughPairs;
    }

    const bool withCard = !cardNumber.empty();
    const std::optional<DiscountCard> card = withCard ? FindDiscountCard(cardNumber) : std::nullopt;
    if (withCard && !card) {
        return SaleStatus::UnknownDiscountCard;
    }
    const int discountPercent = card ? card->GetDiscountPercent() : kNoDiscountPercent;

    shoe->RemovePairs(size, quantity);
    sales_.push_back(Sale(shoe->GetArticle(), shoe->GetModel(), size, quantity,
        shoe->GetPrice(), discountPercent, cardNumber, saleDate));
    return SaleStatus::Success;
}

// ---- Задание 5 ----

const std::vector<Sale>& ShoeStore::GetAllSales() const {
    return sales_;
}

int ShoeStore::PeriodLengthDays(SalesPeriod period) {
    switch (period) {
    case SalesPeriod::Week:  return kDaysInWeek;
    case SalesPeriod::Month: return kDaysInMonth;
    case SalesPeriod::Year:  return kDaysInYear;
    }
    return 0;
}

std::vector<Sale> ShoeStore::GetSalesForPeriod(SalesPeriod period, const Date& asOf) const {
    // Продажа учитывается, если она произошла не позже asOf и не
    // раньше, чем за длину периода дней до asOf.
    const long periodDays = PeriodLengthDays(period);
    std::vector<Sale> result;
    for (const auto& sale : sales_) {
        const long daysAgo = sale.GetSaleDate().DaysUntil(asOf); // asOf - дата продажи
        if (daysAgo >= 0 && daysAgo <= periodDays) {
            result.push_back(sale);
        }
    }
    return result;
}

std::vector<Sale> ShoeStore::GetSalesBetween(const Date& from, const Date& to) const {
    std::vector<Sale> result;
    for (const auto& sale : sales_) {
        if (from <= sale.GetSaleDate() && sale.GetSaleDate() <= to) {
            result.push_back(sale);
        }
    }
    return result;
}

int ShoeStore::CountSoldPairs(const std::vector<Sale>& sales) {
    int total = 0;
    for (const auto& sale : sales) {
        total += sale.GetQuantity();
    }
    return total;
}

double ShoeStore::SumRevenue(const std::vector<Sale>& sales) {
    double total = 0.0;
    for (const auto& sale : sales) {
        total += sale.GetTotalPrice();
    }
    return total;
}
