#include <Geode/Geode.hpp>
#include "Geode/binding/LoadingLayer.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/loader/Dirs.hpp>
#include "archipelago/APUtils.hpp"
#include <Archipelago.h>

using namespace geode::prelude;

// Screen startup
class $modify(APLoadingLayer, LoadingLayer) {
    bool init(bool p0) {
        if (!LoadingLayer::init(p0)) return false;

        geode::log::info("InitArchMode is: {}", Mod::get()->getSavedValue<bool>("InitArchMode"));
        std::error_code error;
        APUtils::inLoadingLayer = true;
        AP_RoomInfo info;
        auto saves = dirs::getSaveDir();
        auto dir = dirs::getGameDir();
        auto res = AP_GetRoomInfo(&info);
        
        // Check if Initializing AP flag is on or not
        if (Mod::get()->getSavedValue<bool>("InitArchMode", false) || res == 0 || Mod::get()->getSavedValue<bool>("StayingInArchMode", false)) { // we are also checking if we got a resul;t because we can reload LoadingLayer messing stuff up
            geode::log::info("We're in Archipelago mode");
            auto url = Mod::get()->getSavedValue<std::string>("recent-url", "");
            auto slot = Mod::get()->getSavedValue<std::string>("recent-slot", "");
            auto pass = Mod::get()->getSavedValue<std::string>("recent-pass", "");

            auto shopKeeperShop = GameStatsManager::sharedState()->shopTypeForItemID(1);
            auto archImage = CCSprite::create("APLogo.png"_spr);
            archImage->setTag(100);

            // for (int i = 0; i <= APUtils::shopItems.size(); i++)
            // {
            //     GameStatsManager::sharedState()->addStoreItem(121 + i, 0, 1000, 200, shopKeeperShop);
            // }
                // }
            geode::log::debug("Finished setting up Archipelago mode");
        } else {
            geode::log::debug("We're not in Archipelago mode");
            auto apLabel = CCLabelBMFont::create("ArchipelagoGD: Not connected", "goldFont.fnt");
            apLabel->setPosition(CCDirector::get()->getWinSize().width / 2, CCDirector::get()->getWinSize().height - 10);
            apLabel->setScale(.45f);
            apLabel->setID("ap-status-label"_spr);
            this->addChild(apLabel);
            for (auto& hook : Mod::get()->getHooks()) {
                if (hook->isEnabled() && !(hook->getDisplayName() == "LoadingLayer::init")){
                    hook->disable();
                }
                if ( // enable back the menu hooks so we can connect
                hook->getDisplayName() == "ConnectPopup::onConnect" || 
                hook->getDisplayName() == "GManager::setup" ||
                hook->getDisplayName() == "MenuLayer::init" ||
                hook->getDisplayName() == "MenuLayer::onArchipelago" ||
                hook->getDisplayName() == "MenuLayer::debug" ||
                hook->getDisplayName() == "MenuLayer::debug2"
                ) {
                    hook->enable();
                }
            }
            geode::log::debug("Finished disabling hooks");
        }

        return true;
    }
};