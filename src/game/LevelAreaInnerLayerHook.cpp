#include <Geode/Geode.hpp>
#include <Geode/modify/LevelAreaInnerLayer.hpp>
#include "archipelago/APUtils.hpp"

using namespace geode::prelude;

// Tower level page
class $modify(APTowerLevelPage, LevelAreaInnerLayer) {
    static void onModify(auto& self) {
        if (!self.setHookPriority("LevelAreaInnerLayer::init", -1)) {
            log::warn("uh oh");
        }
    }

    bool isDoorUnlocked(int doorID) {
        switch (doorID) {
            case 5001:
                return Mod::get()->getSavedValue<bool>(
                    "The Tower: Unlock", false
                );
            case 5002:
                return Mod::get()->getSavedValue<bool>(
                    "The Sewers: Unlock", false
                );
            case 5003:
                return Mod::get()->getSavedValue<bool>(
                    "The Cellar: Unlock", false
                );
            case 5004:
                // Final level
                // We already checked for goal requirement before this so there's no need to check it again
                return Mod::get()->getSavedValue<bool>(
                    "The Secret Hollow: Unlock", false
                );
            default:
                return true;
        }
    }

	bool init(bool p0) {
		if (!LevelAreaInnerLayer::init(p0)) {
			return false;
        }
        geode::log::debug("Called LevelAreaInnerLayer::init()");
        auto mainNode = this->getChildByID("main-node");
        if (!mainNode) {
            log::warn("could not find main node");
            return true;
        }
        auto menu = mainNode->getChildByID("main-menu");
        if (!menu) {
            log::warn("could not find menu");
            return true;
        }
        auto TheTowerDoor = geode::cast::typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("level-5001-button"));        
        if (!TheTowerDoor) {
            log::warn("could not find tower door");
            return true;
        }
        auto TheSewersDoor = geode::cast::typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("level-5002-button"));
        if (!TheSewersDoor) {
            log::warn("could not find sewers door");
            return true;
        }
        auto TheCellarDoor = geode::cast::typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("level-5003-button"));
        if (!TheCellarDoor) {
            log::warn("could not find cellar door");
            return true;
        }
        auto TheSecretHollowDoor = geode::cast::typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("level-5004-button"));
        if (!TheSecretHollowDoor) {
            log::warn("could not find secret hollow door");
            return true;
        }

        // Update Tower door appearance based on Archipelago unlock state
        // using lambda function
        auto updateDoor = [](CCMenuItemSpriteExtra* door, const char* itemName) {
            bool isUnlocked = Mod::get()->getSavedValue<bool>(itemName, false);

            auto sprite = CCSprite::createWithSpriteFrameName(
                isUnlocked
                    ? "towerDoor_unlocked_001.png"
                    : "towerDoor_locked_001.png"
            );

            if (!sprite) {
                log::warn("Failed to create door sprite for {}", itemName);
                return;
            }

            door->setNormalImage(sprite);

            // Keep the button clickable so onDoor() can handle locked doors.
            door->setEnabled(true);
        };

        // Update all four doors
        updateDoor(TheTowerDoor, "The Tower: Unlock");
        updateDoor(TheSewersDoor, "The Sewers: Unlock");
        updateDoor(TheCellarDoor, "The Cellar: Unlock");
        updateDoor(TheSecretHollowDoor, "The Secret Hollow: Unlock");

        auto levelBeaten = APUtils::levelsBeaten;

        if (levelBeaten.size() < Mod::get()->getSavedValue<int>("Goal Requirement")) {
            // Draw a number on the door to indicate requirement number of levels beaten
            int numOfLevelsNeeded = Mod::get()->getSavedValue<int>("Goal Requirement") - levelBeaten.size();
            auto requirement = CCLabelBMFont::create(std::to_string(numOfLevelsNeeded).c_str(), "bigFont.fnt");
            requirement->setPosition(28.f, 20.f);
            TheSecretHollowDoor->addChild(requirement);
        }

        geode::log::debug("Finished LevelAreaInnerLayer::init()");
        return true;
    }

    // similar to onPlay()
    void onDoor(CCObject* sender) {
        auto id = sender->getTag();
        int goal = Mod::get()->getSavedValue<int>("Goal Requirement");
        auto levelBeaten = APUtils::levelsBeaten;
        if (id == 5004 && levelBeaten.size() < goal) { // secret hollow
            FLAlertLayer::create(
                "Locked",
                "You need to beat " + std::to_string((goal - levelBeaten.size())) + " levels to access the final level.",
                "OK"
            )->show();
            return;
        }
        if (!isDoorUnlocked(id)){
            FLAlertLayer::create(
                "Locked",
                "You haven't received this level yet.",
                "OK"
            )->show();
            return;
        }
        LevelAreaInnerLayer::onDoor(sender);
};  
};