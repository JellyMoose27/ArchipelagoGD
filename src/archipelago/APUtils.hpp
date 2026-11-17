#pragma once
#include <string>
#include <vector>
#include <unordered_set>

namespace APUtils {

    extern std::string errorMessage; // used for errors before the main menu is up because them it will just crash

    extern std::unordered_set<int> collectedCoins;

    // extern int speed;

    // Filler items
    extern int manaOrbsToAdd;
    extern int diamondsToAdd;
    
    // Levels beaten to meet the goal requirement
    extern std::vector<std::string> levelsBeaten;

    // bool for settings and other stuffs
    extern bool inLoadingLayer;
    extern bool GManagerRan;
    // extern bool coinsEnabled;
    extern bool coinLocksEnabled;
    extern bool ultimatesEnabled;
    // extern bool checkShopEnabled;

    extern bool notifsOff;
    //extern bool modifiers;

    static std::vector<std::string> levels = {
        "Stereo Madness",
        "Back On Track",
        "Polargeist",
        "Dry Out",
        "Base After Base",
        "Cant Let Go",
        "Jumper",
        "Time Machine",
        "Cycles",
        "xStep",
        "Clutterfunk",
        "Theory of Everything",
        "Electroman Adventures",
        "Clubstep",
        "Electrodynamix",
        "Hexagon Force",
        "Blast Processing",
        "Theory of Everything 2",
        "Geometrical Dominator",
        "Deadlocked",
        "Fingerdash",
        "Dash",
        "The Tower",
        "The Sewers",
        "The Cellar",
        "The Secret Hollow",
    };

    static std::vector<std::string> generateCoinChecks() {
        std::vector<std::string> coins;

        for (const auto& level : levels) {
            for (int coin = 1; coin <= 3; ++coin) {
                coins.push_back(level + " - Coin " + std::to_string(coin));
            }
        }

        return coins;
    }
    static std::vector<std::string> coinChecks = generateCoinChecks();

    // static std::vector<std::string> generateShopItems(int count) {
    //     std::vector<std::string> shopItems;

    //     for (int i = 1; i <= count; i++) {
    //         shopItems.push_back("Check Shop #" + std::to_string(i));
    //     }
        
    //     return shopItems;
    // }
    // static std::vector<std::string> shopItems = generateShopItems(42);
    
    // determine which item is binded to the shop items from my understanding - Jelly Moose
    // extern std::vector<std::string> shopItemsItem;
    
    int checkIfTower(int id);

    void receiveItem(int64_t id, bool notify);

    void clearItemState();

    void checkLocationCallback(int64_t id);

    void createNotification(std::string name, std::string sender, bool location);

    void sendItem(int64_t id);

    void deathLinkRecieved();

    bool checkPortal(int id);

    void getStartingLevels(std::string ids);

    bool registerSprite(std::string const& filename);

    void getGoalRequiremnt(std::string num);

    // void setCoinsBool(int val);

    void setCoinLocksBool(int val);

    void setUltimatesBool(int val);

    void setPortalsToFalse();

    // void setCheckShopBool(int val);

    // const char* getImageForShopItem(std::string item);

    void goal();

    void startArchipelago(const char* url, const char* slot, const char* pass);
}