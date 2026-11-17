#include "archipelago/APUtils.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(APGJBaseGameLayer, GJBaseGameLayer) {
    void processItems() {
        geode::log::debug("Called GJBaseGameLayer::processItems()");
        GJBaseGameLayer::processItems();
        auto levelObj = PlayLayer::get()->m_level;
        // if (APUtils::coinsEnabled) {
        auto levelID = levelObj->m_levelID.value();
        levelID = APUtils::checkIfTower(levelID);

        /* Current issue: This only checks if you collected the coin for the first time
        Meaning that if you collected the coins before, completing the level again will automatically
        send coin checks even if you don't collect all coins during gameplay.

        Solution 1: Completely erase the data (Automatically backup user's data into the cloud before erasing it)
                    Can do this when connect to the AP server (see APUtils::startArchipelago())

        Solution 2: Only count the checks when the coins are collected during gameplay.
                    Coins are objects of EffectGameObject class that are rendered during gameplay (PlayLayer)

        TODO: Find a suitable solution for this issue.
        */ 
        
        if (levelID < 100) {
            for (int i = 0; i < 3; i++) {
                auto key = levelObj->getCoinKey(i+1);
                auto hasCoin = GameStatsManager::sharedState()->hasSecretCoin(key);
                if (hasCoin) {
                    auto itemID = (levelID * 1000) + (i+1) + 130820130;
                    APUtils::sendItem(itemID);
                    geode::log::info("sent coin item with id {}", itemID);
                } else {
                    if (!key) {
                        geode::log::warn("couldnt find key for coin {}, level {}, id {}", i+1, levelID, levelObj->m_levelID.value());
                    }
                }
            }
        }
        // }
        geode::log::debug("Finished GJBaseGameLayer::processItems()");
    }
};
