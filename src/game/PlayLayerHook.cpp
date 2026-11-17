#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/random.hpp>
#include "archipelago/APUtils.hpp"
#include "Archipelago.h"

using namespace geode::prelude;

class $modify(APPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        return true;
    }

    // delete portals when needed
    void addObject(GameObject* p0) {
        if (APUtils::checkPortal(p0->m_objectID)) {
            PlayLayer::addObject(p0);
        }
    }

    // Clear collected coins set
    void resetLevel() {
        APUtils::collectedCoins.clear();
        geode::log::info("Cleared collected coins set.");
        PlayLayer::resetLevel();
    }

    void levelComplete() {
        // level completed
        auto ccsched = cocos2d::CCScheduler::get();
        auto levelID = this->m_level->m_levelID.value();

        levelID = APUtils::checkIfTower(levelID);
        levelID -= 1;

        if (ccsched->getTimeScale() != 1.00) {
            ccsched->setTimeScale(1.00);
            FMOD::ChannelGroup* masterGroup;
            if (FMODAudioEngine::get()->m_system->getMasterChannelGroup(&masterGroup) != FMOD_OK) return;
                masterGroup->setPitch(1.00);
        }

        if (levelID == 25) {
            
            AchievementNotifier::sharedState()->notifyAchievement(
                "Victory!", 
                "You completed your goal and released all items!", 
                "APLogo.png"_spr, 
                true
            );

            Mod::get()->setSavedValue<bool>("Won", true);
            APUtils::goal();
            PlayLayer::levelComplete();

            return;
        }

        if (levelID > 128 || this->m_isPracticeMode) {
            PlayLayer::levelComplete();
            log::info("Just called levelComplete() from levelID > 128");
            return; // reaches into thje zone of normal levels
        }
        
        APUtils::sendItem(levelID); 
        
        auto status = AP_GetConnectionStatus();
        if (status == AP_ConnectionStatus::Disconnected || status == AP_ConnectionStatus::ConnectionRefused) {
            geode::Notification::create(
                "Error: You currently aren't connected to Archipelago!", 
                geode::NotificationIcon::None, 
                4.f
            )->show();
        }
        
        // increment levels beaten by one
        auto level = APUtils::levels.at(levelID);
        auto levelBeaten = APUtils::levelsBeaten;
        auto it = std::find(levelBeaten.begin(), levelBeaten.end(), level);

        // Level not beaten yet
        if (it == levelBeaten.end()){
            APUtils::levelsBeaten.push_back(level);
        }
        else {
            geode::Notification::create("You already beat this level! It won't be counted towards the goal.", geode::NotificationIcon::None, 4.f)->show();
            log::info("Level already beaten. Did nothing.");
        }

        Mod::get()->setSavedValue("Levels Beaten", levelBeaten);

        std::string result;
        for (const auto& coinID : APUtils::collectedCoins) {
            if (!result.empty()) {
                result += ", ";
            }

            result += std::to_string(coinID);
        }

        geode::log::info("Collected Coins: [{}]", result);

        PlayLayer::levelComplete();
    }
    void onQuit() {
        auto ccsched = cocos2d::CCScheduler::get();

        if (ccsched->getTimeScale() != 1.00) {
            ccsched->setTimeScale(1.00);
            FMOD::ChannelGroup* masterGroup;
            if (FMODAudioEngine::get()->m_system->getMasterChannelGroup(&masterGroup) != FMOD_OK) return;
                masterGroup->setPitch(1.00);
        }
        
        APUtils::collectedCoins.clear();
        PlayLayer::onQuit();
    }
};
