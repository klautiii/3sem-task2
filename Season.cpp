#include "Season.h"

std::string SeasonName(Season season) {
    switch (season) {
    case Season::Winter:     return "зимняя";
    case Season::Summer:     return "летняя";
    case Season::DemiSeason: return "весна-осень";
    case Season::AllSeason:  return "всесезонная";
    }
    return "неизвестный сезон";
}

const std::vector<Season>& AllSeasons() {
    static const std::vector<Season> kAllSeasons = {
        Season::Winter,
        Season::Summer,
        Season::DemiSeason,
        Season::AllSeason,
    };
    return kAllSeasons;
}
