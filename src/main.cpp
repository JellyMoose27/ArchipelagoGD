#include <Geode/modify/MenuLayer.hpp>
#include "ui/ConnectPopup.cpp"
#include "archipelago/APUtils.hpp"
#include "Archipelago.h"

using namespace geode::prelude;

$on_game(Loaded) {
    listenForKeybindSettingPresses("Skip All Notifications",[](Keybind const& keybind, bool down, bool repeat, double time){
        if (down && !repeat) {
            while (AchievementNotifier::sharedState()->m_activeAchievementBar) {
            AchievementNotifier::sharedState()->achievementDisplayFinished(); // skips the current notif - while loop will clear all of em
            }
        }   
    });
    listenForKeybindSettingPresses("Skip Notification",[](Keybind const& keybind, bool down, bool repeat, double time){
        if (down && !repeat) {
            if (AchievementNotifier::sharedState()->m_activeAchievementBar) {
                AchievementNotifier::sharedState()->achievementDisplayFinished();
            }
        }   
    });
}

// modify the MenuLayer to add the AP connect button
class $modify(APMenuLayer, MenuLayer) {
    bool init() {
        // call the original init to avoid override the MenuLayer
        if (!MenuLayer::init())
            return false;
        
        geode::log::debug("Called MenuLayer::init()");
        APUtils::inLoadingLayer = false;
        

        // Forgot to load them in the mod.json file lmao
        auto apBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSprite("APLogo.png"_spr, 1.0f, CircleBaseColor::Gray, CircleBaseSize::SmallAlt),
            this,
            menu_selector(APMenuLayer::onArchipelago)
        );

        // If the Overcharged Main Menu mod is installed, place the buttons in the bottom instead
        // Just a menu mod, nothing special. Prolly gonna leave this alone
        if (geode::Loader::get()->getLoadedMod("ninxout.redash")) {
            auto bm = this->getChildByID("bottom-menu");
            bm->addChild(apBtn);
            bm->updateLayout();
        } else {
            auto trm = static_cast<CCMenu*>(this->getChildByID("top-right-menu"));
            trm->addChild(apBtn);
            trm->updateLayout();
        }
        
        apBtn->setID("APConnectBtn"_spr);

        if (APUtils::manaOrbsToAdd != 0) {
            if (Mod::get()->getSavedValue<bool>("StayingInArchMode", false)) {
                GameStatsManager::sharedState()->incrementStat("14", APUtils::manaOrbsToAdd);
            }
            APUtils::manaOrbsToAdd = 0;
        }

        if (APUtils::diamondsToAdd != 0) {
            if (Mod::get()->getSavedValue<bool>("StayingInArchMode", false)) {
                GameStatsManager::sharedState()->incrementStat("13", APUtils::diamondsToAdd);
            }
            APUtils::diamondsToAdd = 0;
        }

        auto status = AP_GetConnectionStatus();

        // Are we starting Archipelago mode?
        if (Mod::get()->getSavedValue<bool>("InitArchMode", false)) {
            // Are we connected to the Archipelago server yet?
            geode::log::info("Status: {}", static_cast<int>(status));
            if (status == AP_ConnectionStatus::Disconnected || status == AP_ConnectionStatus::ConnectionRefused) {
                geode::Notification::create("Error: You currently aren't connected to Archipelago!", geode::NotificationIcon::None, 4.f)->show();
            } else {
                // We already checked for disconnected and connected refused so no need to check the others
                Mod::get()->setSavedValue("StayingInArchMode", true); 
            }
        }

        if (APUtils::errorMessage != "") {
            geode::Notification::create(APUtils::errorMessage.c_str(), geode::NotificationIcon::None, 4.f)->show();
        }

        // Initializing AP mode finished
        Mod::get()->setSavedValue<bool>("InitArchMode", false);
        auto res = Mod::get()->saveData();

        if (!res.isOk()) {
            geode::Notification::create("Warning: Autosave player data failed! You might lose your data if the game crashes!", geode::NotificationIcon::None, 4.f)->show();
        }

        geode::log::debug("MenuLayer::init() was finshed");
        return true;
    }

    void onArchipelago(CCObject*) {
        ConnectPopup::create()->show();
    }
};

