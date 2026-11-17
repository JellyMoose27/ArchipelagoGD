#include "APUtils.hpp"
#include "Archipelago.h"
#include "Geode/binding/PlayLayer.hpp"
#include "Geode/loader/Loader.hpp"
#include "Geode/loader/Log.hpp"

using namespace geode::prelude;

const std::unordered_map<std::string, const char*> items = {
    {"Back On Track: Unlock", "backOnTrackUnlock.png"_spr},
    {"Polargeist: Unlock", "polargeistUnlock.png"_spr},
    {"Dry Out: Unlock", "dryOutUnlock.png"_spr},
    {"Base After Base: Unlock", "baseAfterBaseUnlock.png"_spr},
    {"Cant Let Go: Unlock", "cantLetGoUnlock.png"_spr},
    {"Jumper: Unlock", "jumperUnlock.png"_spr},
    {"Time Machine: Unlock", "timeMachineUnlock.png"_spr},
    {"Cycles: Unlock", "cyclesUnlock.png"_spr},
    {"xStep: Unlock", "xStepUnlock.png"_spr},
    {"Clutterfunk: Unlock", "clutterfunkUnlock.png"_spr},
    {"Theory of Everything: Unlock", "toeUnlock.png"_spr},
    {"Electroman Adventures: Unlock", "electromanAdventuresUnlock.png"_spr},
    {"Clubstep: Unlock", "clubstepUnlock.png"_spr},
    {"Electrodynamix: Unlock", "electrodynamixUnlock.png"_spr},
    {"Hexagon Force: Unlock", "hexagonForceUnlock.png"_spr},
    {"Blast Processing: Unlock", "blastProcessingUnlock.png"_spr},
    {"Theory of Everything 2: Unlock", "toe2Unlock.png"_spr},
    {"Geometrical Dominator: Unlock", "geometricalDominatorUnlock.png"_spr},
    {"Deadlocked: Unlock", "deadlockedUnlock.png"_spr},
    {"Fingerdash: Unlock", "fingerdashUnlock.png"_spr},
    {"Dash: Unlock", "dashUnlock.png"_spr},
    {"The Tower: Unlock", "theTowerUnlock.png"_spr},
    {"The Sewers: Unlock", "theSewersUnlock.png"_spr},
    {"The Cellar: Unlock", "theCellarUnlock.png"_spr},
    {"The Secret Hollow: Unlock", "theSecretHollowUnlock.png"_spr},

    {"Ball Portal", "ballUnlock.png"_spr},
    {"UFO Portal", "ufoUnlock.png"_spr},
    {"Wave Portal", "waveUnlock.png"_spr},
    {"Robot Portal", "robotUnlock.png"_spr},
    {"Spider Portal", "spiderUnlock.png"_spr},
    {"Swing Portal", "swingUnlock.png"_spr},

    {"100 Mana Orbs", "currencyOrbIcon_001.png"},
    {"5 Diamonds", "GJ_diamondsIcon_001.png"}
};

// Convert unordered_map to vector for easier indexing
int gdBaseID = 130820130;
// note to index staff: this is due to the way Archipelago itself works 
// fun fact, this is the GD release date in DD/MM/YYYY and a 0
const std::unordered_map<int64_t, std::string> itemIdToName = {
    {gdBaseID + 0, "Back On Track: Unlock"},
    {gdBaseID + 1, "Polargeist: Unlock"},
    {gdBaseID + 2, "Dry Out: Unlock"},
    {gdBaseID + 3, "Base After Base: Unlock"},
    {gdBaseID + 4, "Cant Let Go: Unlock"},
    {gdBaseID + 5, "Jumper: Unlock"},
    {gdBaseID + 6, "Time Machine: Unlock"},
    {gdBaseID + 7, "Cycles: Unlock"},
    {gdBaseID + 8, "xStep: Unlock"},
    {gdBaseID + 9, "Clutterfunk: Unlock"},
    {gdBaseID + 10, "Theory of Everything: Unlock"},
    {gdBaseID + 11, "Electroman Adventures: Unlock"},
    {gdBaseID + 12, "Clubstep: Unlock"},
    {gdBaseID + 13, "Electrodynamix: Unlock"},
    {gdBaseID + 14, "Hexagon Force: Unlock"},
    {gdBaseID + 15, "Blast Processing: Unlock"},
    {gdBaseID + 16, "Theory of Everything 2: Unlock"},
    {gdBaseID + 17, "Geometrical Dominator: Unlock"},
    {gdBaseID + 18, "Deadlocked: Unlock"},
    {gdBaseID + 19, "Fingerdash: Unlock"},
    {gdBaseID + 20, "Dash: Unlock"},
    {gdBaseID + 21, "The Tower: Unlock"},
    {gdBaseID + 22, "The Sewers: Unlock"},
    {gdBaseID + 23, "The Cellar: Unlock"},
    {gdBaseID + 24, "The Secret Hollow: Unlock"},

    {gdBaseID + 25, "Ball Portal"},
    {gdBaseID + 26, "UFO Portal"},
    {gdBaseID + 28, "Wave Portal"},
    {gdBaseID + 29, "Robot Portal"},
    {gdBaseID + 30, "Spider Portal"},
    {gdBaseID + 31, "Swing Portal"},

    {gdBaseID + 32, "100 Mana Orbs"},
    {gdBaseID + 33, "5 Diamonds"}
};

// std::vector<std::string> APUtils::shopItemsItem = {};

std::string APUtils::errorMessage = "";

std::unordered_set<int> APUtils::collectedCoins;

int APUtils::manaOrbsToAdd = 0;
int APUtils::diamondsToAdd = 0;
std::vector<std::string> APUtils::levelsBeaten = {};

bool APUtils::inLoadingLayer = true;
bool APUtils::GManagerRan = false;

// bool APUtils::coinsEnabled = true;
bool APUtils::coinLocksEnabled = true;
bool APUtils::ultimatesEnabled = false;
// bool APUtils::checkShopEnabled = true;


bool APUtils::notifsOff = Mod::get()->getSettingValue<bool>("Disable Notifications");

int APUtils::checkIfTower(int id) {
    switch (id) {
        case 5001:
            return 23; // The Tower
        case 5002:
            return 24; // The Sewers
        case 5003:
            return 25; // The Cellar
        case 5004:
            return 26; // The Secret Hollow
        default:
            return id;
        }
    }
    
void APUtils::receiveItem(int64_t id, bool notify) {
    // Find the item in the item map based on id
    geode::log::info("Id received: {}", id);
    auto it = itemIdToName.find(id);

    if (it == itemIdToName.end()) {
        geode::log::error("Unknown item ID received: {}", id);
        return;
    }

    std::string itemToRecieve = it->second;
    
    // TODO: Retreive sender name from the server
    // AP_NetworkPlayer sender = getPlayer()

    geode::log::info("Receiving item: {}", itemToRecieve);
    if (itemToRecieve == "100 Mana Orbs") {
        if (!APUtils::inLoadingLayer) {
            GameStatsManager::sharedState()->incrementStat("14", 100);
        }

        if (notify) {
            Loader::get()->queueInMainThread(
                [] {
                    APUtils::createNotification(
                        "100 Mana Orbs", "Placeholder", false
                    );
                }
            );
        }

        return;
    }
    else if (itemToRecieve == "5 Diamonds") {
        if (!APUtils::inLoadingLayer) {
            GameStatsManager::sharedState()->incrementStat("13", 5);
        }

        if (notify) {
            Loader::get()->queueInMainThread(
                [] {
                    APUtils::createNotification(
                        "5 Diamonds", "Placeholder", false
                    );
                }
            );
        }

        return;
    }
    Mod::get()->setSavedValue<bool>(itemToRecieve, true);
    if (notify) {
        Loader::get()->queueInMainThread(
            [itemToRecieve]{APUtils::createNotification(itemToRecieve, "Placeholder", false);}
        );
    }
}

void APUtils::clearItemState() {
    geode::log::debug("Called APUtils::clearItemState()");
    std::size_t itemAmount = items.size();

    // iterate directly through the map for faster lookup
    for (const auto& [itemID, itemName] : itemIdToName) {
        Mod::get()->setSavedValue<bool>(itemName, false);
    }
    geode::log::debug("Finished APUtils::clearItemState()");
}

void APUtils::checkLocationCallback(int64_t id) {
    
    geode::log::info("Called APUtils::checkLocationCallback");
    id -= gdBaseID;
    geode::log::info("ID checked: {}", id);
}


void APUtils::sendItem(int64_t id) {
    // Checks are done here
    // coin checks
    geode::log::info("ID checked: {}", id);
    if (id >= 1000) {
        AP_SendItem(id);
        id -= gdBaseID;
        auto coinNum = id % 1000;
        id -= coinNum;
        id /= 1000;
        id -= 1;
        if (id > APUtils::levels.size() || id < 0) {
            geode::log::warn("The id of {} was not in the levels vector!", id);
            return;
        }
        auto levelName = APUtils::levels.at(id);
        geode::log::info("checked location {}, coin number {}", levelName, coinNum);
        levelName += " - Coin " + geode::utils::numToString(coinNum);
        Loader::get()->queueInMainThread(
        [levelName]{
            APUtils::createNotification(levelName, "Placeholder", true);
        });
    
    // level checks
    } else if (id < 99) {
        AP_SendItem(id + gdBaseID);
        if (id > APUtils::levels.size() || id < 0) {
            geode::log::warn("The id of {} was not in the levels vector!", id);
            return;
        }
        auto locationChecked = APUtils::levels.at(id);
        geode::log::info("checked location {}", locationChecked);
        Loader::get()->queueInMainThread(
            [locationChecked]{APUtils::createNotification(locationChecked, "Placeholder", true);}
        );
    }
};

/*
Current issue: level check notification is created every time a coin check notification is created 
(this also happens when we disconnect and reconnect to the AP server,
As if the checks notification were buffered.
*/
void APUtils::createNotification(std::string name, std::string sender, bool location) {
    if (APUtils::inLoadingLayer) return;
    if (notifsOff) return;

    geode::log::debug("Called APUtils::createNotification()");
    const char* title;
    std::string desc;
    auto slotname = Mod::get()->getSavedValue<std::string>("recent-slot", "Unknown");

    if (location) {
        title = "Location checked!";
        desc = "You checked <cg>" + name + " </c>and sent an item.";
    } else if (sender == slotname) {
        title = "Item found!";
        desc = "You found <cg>" + name + "</c>.";
    } else {
        title = "Item received!";
        desc = "You received <cg>" + name + "</c> from <cg>" + sender + "</c>.";
    }
    geode::log::info("{}", title);
    geode::log::info("{}", desc);
    geode::log::info("{}", name);
    geode::log::info("location = {}", location);

    // Find the sprite corresponding to the item
    const char * icon;

    if (location) {
        icon = "APLogo.png"_spr;
    } else {
        auto it = items.find(name);

        if (it == items.end()) {
            geode::log::warn("Unknown item name: {}", name);
            return;
        }

        icon = it->second;
    }

    if (!APUtils::registerSprite(icon)) {
        log::warn("Failed to load sprite: {}", icon);
    }
    AchievementNotifier::sharedState()->notifyAchievement(title, desc.c_str(), icon, true);

    geode::log::debug("Finished APUtils::createNotification()");
}

void APUtils::deathLinkRecieved() {
    Loader::get()->queueInMainThread(
        []{
            if (PlayLayer* playLayer = GameManager::get()->getPlayLayer()) playLayer->destroyPlayer(playLayer->m_player1, nullptr);
            geode::log::info("deathlink received. time to die");
        }
    );
}

bool APUtils::checkPortal(int id) {
    switch (id) {
        default:
        return true;
        // Ship portal (always available)
        case 13:
        return true;
        case 47:
        return Mod::get()->getSavedValue<bool>("Ball Portal", true);
        case 111:
        return Mod::get()->getSavedValue<bool>("UFO Portal", true);
        case 660:
        return Mod::get()->getSavedValue<bool>("Wave Portal", true);
        case 745:  
        return Mod::get()->getSavedValue<bool>("Robot Portal", true);
        case 1331:
        return Mod::get()->getSavedValue<bool>("Spider Portal", true);
        case 1933:
        return Mod::get()->getSavedValue<bool>("Swing Portal", true);
    }
}

// Get the number of levels required to access the goal (taken from yaml)
void APUtils::getGoalRequiremnt(std::string num) {
    // lambda function
    Loader::get() ->queueInMainThread([num] {
        // extract the goal requirement from the AP world (Generated from the yaml)
        std::istringstream iss(num);
        int goal_num{};
        iss >> goal_num;
        Mod::get()->setSavedValue<int>("Goal Requirement", goal_num);
    });
}

// unused
void APUtils::getStartingLevels(std::string ids) {
    // We'll start with Stereo Madness for now
    geode::log::debug("Called APUtils::getStartingLevels()");
    Loader::get()->queueInMainThread(
        [ids]{
    std::istringstream iss(ids);
    std::vector<std::string> levelIds((std::istream_iterator<std::string>(iss)),
    std::istream_iterator<std::string>());
    
    levelIds.pop_back(); // rem,ove rnadom "
    for (auto& id : levelIds) {
        if (id.contains("\"")) {
            size_t pos = id.find("\"");
            id.erase(pos, 1);
        }
        geode::log::info("got starting level {}", id);
        auto levelIntRes = geode::utils::numFromString<int>(id, 10);
        int levelInt = levelIntRes.unwrap();
        levelInt = checkIfTower(levelInt);
        auto level = APUtils::levels.at(levelInt);
        geode::log::info("aka {}", level);

        Mod::get()->setSavedValue<bool>(level + ": Unlock", true);
        geode::log::debug("Finished APUtils::getStartingLevels()");
    }
}
);
}

bool APUtils::registerSprite(std::string const& filename) {
    auto texture = CCTextureCache::sharedTextureCache()
        ->addImage(filename.c_str(), false);

    if (!texture) {
        return false;
    }

    auto size = texture->getContentSize();

    auto frame = CCSpriteFrame::create(
        filename.c_str(),
        CCRect(0, 0, size.width, size.height)
    );

    if (!frame) {
        return false;
    }

    CCSpriteFrameCache::sharedSpriteFrameCache()
        ->addSpriteFrame(frame, filename.c_str());

    return true;
}

void APUtils::goal() {
    AP_StoryComplete();
}

// void APUtils::setCoinsBool(int val) {
//     if (val == 0) {
//       APUtils::coinsEnabled = false;  
//     } else {
//       APUtils::coinsEnabled = true;
//     }
// }

void APUtils::setUltimatesBool(int val) {
    APUtils::ultimatesEnabled = (val != 0);
}

void APUtils::setCoinLocksBool(int val) {
    APUtils::coinLocksEnabled = (val != 0);
}

// void APUtils::setShopItems(std::string item) {
//     APUtils::shopItemsItem.push_back(item);
// }

// const char* APUtils::getImageForShopItem(std::string item) { // there has GOTTA be a easier way to do ts 
// // if it's a key, value pair why not use unordered_map? - Jelly Moose
//     const auto it = items.find(item);

//     if (it != items.end()) {
//         return it->second;
//     }

//     return "APLogo.png"_spr;
// }

void APUtils::setPortalsToFalse() {
    Mod::get()->setSavedValue<bool>("Ball Portal", false);
    Mod::get()->setSavedValue<bool>("UFO Portal", false);
    Mod::get()->setSavedValue<bool>("Wave Portal", false);
    Mod::get()->setSavedValue<bool>("Robot Portal", false);
    Mod::get()->setSavedValue<bool>("Spider Portal", false);
    Mod::get()->setSavedValue<bool>("Swing Portal", false);
}

// Initialize AP session
void APUtils::startArchipelago(const char *url, const char *slot, const char *pass) {
    for (auto& level : APUtils::levels) {
        if (Mod::get()->getSavedValue<bool>(level + ": Unlock", true)) {
            Mod::get()->setSavedValue<bool>(level + ": Unlock", false);
        }
    }

    // Unlock Stereo Madess as the starting level
    Mod::get()->setSavedValue<bool>("Stereo Madness: Unlock", true);

    // Set the portals received to false so that the portals are properly removed
    setPortalsToFalse();

    // Initilize levels beaten so that it automatically saves on closing the game
    const std::vector<std::string> levelsBeaten = Mod::get()->getSavedValue<std::vector<std::string>>("Levels Beaten", {});
    if (levelsBeaten.empty()) {
        Mod::get()->setSavedValue<std::vector<std::string>>("Levels Beaten", {});
    } else {
        APUtils::levelsBeaten = levelsBeaten;
    }

    AP_Init(url, "Geometry Dash", slot, pass);
    AP_SetItemClearCallback(&APUtils::clearItemState);
    AP_SetItemRecvCallback(&APUtils::receiveItem);
    AP_SetLocationCheckedCallback(&APUtils::checkLocationCallback);
    AP_SetDeathLinkSupported(true);
    AP_SetDeathLinkRecvCallback(&APUtils::deathLinkRecieved);
    AP_RegisterSlotDataRawCallback("goal_requirement", &APUtils::getGoalRequiremnt);
    AP_RegisterSlotDataIntCallback("coin_locks", &APUtils::setCoinLocksBool);
    AP_RegisterSlotDataIntCallback("ultimate", &APUtils::setUltimatesBool);
    // for (int i = 1; i <= shopItems.size() - 1; ++i) {
    //     AP_RegisterSlotDataRawCallback("shop_item" + std::to_string(i), APUtils::setShopItems);
    // }

    AP_SetLoggingCallback([](std::string message) {
        log::info("[APCpp] {}", message);
    });
    AP_Start();
}
