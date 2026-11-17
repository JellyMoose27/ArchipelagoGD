#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/loader/Dirs.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
#include <fmt/core.h>
#include <string>
#include <Archipelago.h>
#include <Geode/binding/LoadingCircle.hpp>
#include "archipelago/APUtils.hpp"

using namespace geode::prelude; 
class ConnectPopup : public Popup {
    private:
        LoadingCircle* m_loadingCircle = nullptr;
        CCLabelBMFont* m_connectionStatusLabel = nullptr;

        float m_connectionElapsed = 0.f;

        bool m_isConnecting = false;
        bool m_connectionFinished = false;

    protected:

bool init() {
    auto const layerSize = CCSize { 346.f, 280.f };
    
    if (!Popup::init(layerSize)) {
        return false;
    }
    geode::log::debug("Called Popup::init()");
    m_noElasticity = true;
    if (Mod::get()->getSavedValue<bool>("StayingInArchMode", false)) {
        this->setTitle("Archipelago Room Info");
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto layerPos = (winSize - layerSize) / 2;
        AP_RoomInfo info;
        auto res = AP_GetRoomInfo(&info);
        if (res != 0) {
            geode::Notification::create("You are currently not connected to Archipelago!", geode::NotificationIcon::None, 4.f)->show();
            auto menu = CCMenu::create();
            auto disconnectButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Disconnect"),
            this,
            menu_selector(ConnectPopup::onClick)
            );
            disconnectButton->setPosition(0.f, -105.f);
            menu->addChild(disconnectButton);
            this->addChild(menu);
            return true;
        }
        auto APVersion = info.version;
        auto tags = info.tags;
        auto passwordRequired = info.password_required;
        auto permissions = info.permissions;
        auto hintCost = info.hint_cost;
        auto locationCheckPoints = info.location_check_points;
        auto seedName = info.seed_name;
        auto time = info.time;
        auto infoLabel = CCLabelBMFont::create(
            fmt::format(
                "AP Version: {}.{}.{}\n\nSeed: {}\n\nPassword Required: {}\n\nHint Cost: {}\n\nLocation Checkpoints: {}\n\nServer uptime: {} seconds\n\nTags: {}",
                APVersion.major, APVersion.minor, APVersion.build, seedName, passwordRequired ? "Yes" : "No", hintCost, locationCheckPoints, time, fmt::join(tags, ", ")
            ).c_str(),
            "bigFont.fnt"
        );
        infoLabel->setScale(0.45f);
        infoLabel->setPosition(0.f, 5.f);
        auto menu = CCMenu::create();
        menu->addChild(infoLabel);
        auto disconnectButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Disconnect"),
            this,
            menu_selector(ConnectPopup::onClick)
        );
        disconnectButton->setPosition(0.f, -105.f);
        menu->addChild(disconnectButton);
        this->addChild(menu);
        return true;
    }
    this->setTitle("Connect to Archipelago");

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto const layerPos = (winSize - layerSize) / 2;
    urlInput = TextInput::create(260, "archipelago.gg:12345", "chatFont.fnt");
    slotInput = TextInput::create(260, "1257Plays", "chatFont.fnt");
    passInput = TextInput::create(260, "topsecretpassword", "chatFont.fnt");
    urlInput->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.:;*/");
    urlInput->setString(Mod::get()->getSavedValue<std::string>("recent-url", ""));

    slotInput->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_;:/?!.,[]{}/=+* ");
    slotInput->setString(Mod::get()->getSavedValue<std::string>("recent-slot", ""));

    passInput->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_;:/?!.,[]{}/=+* ");
    passInput->setString(Mod::get()->getSavedValue<std::string>("recent-pass", ""));
    
    auto urlLabel = CCLabelBMFont::create("Archipelago URL", "bigFont.fnt");
    auto slotLabel = CCLabelBMFont::create("Slot Name", "bigFont.fnt");
    auto passLabel = CCLabelBMFont::create("Password (if not set, leave empty)", "bigFont.fnt");
    urlLabel->setScale(.5f);
    urlLabel->setPosition(0.f, 85.f);
    urlInput->setPosition(0.f, urlLabel->getPositionY()-25.f);

    slotLabel->setScale(.5f);
    slotLabel->setPosition(0.f, 25.f);
    slotInput->setPosition(0.f, slotLabel->getPositionY()-25.f);

    passLabel->setScale(.5f);
    passLabel->setPosition(0.f, -35.f);
    passInput->setPosition(0.f, passLabel->getPositionY()-25.f);

    auto menu = CCMenu::create();
    auto connectButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Connect"),
        this,
        menu_selector(ConnectPopup::onClick)
    );
    connectButton->setPosition(0.f, -105.f);
    menu->addChild(urlInput);
    menu->addChild(slotInput);
    menu->addChild(passInput);
    menu->addChild(urlLabel);
    menu->addChild(slotLabel);
    menu->addChild(passLabel);
    menu->addChild(connectButton);
    this->addChild(menu);
    urlInput->setID("APUrlInput"_spr);
    slotInput->setID("APSlotInput"_spr);
    passInput->setID("APPassInput"_spr);
    urlLabel->setID("APUrlLabel"_spr);
    slotLabel->setID("APSlotLabel"_spr);
    passLabel->setID("APPassLabel"_spr);
    menu->setID("APConnectMenu"_spr);
    geode::log::debug("Finished Popup::init()");
    return true;
}
    TextInput* urlInput;
    TextInput* slotInput;
    TextInput* passInput;

public:
void startConnectionMonitor() {
    m_isConnecting = true;
    m_connectionFinished = false;
    m_connectionElapsed = 0.f;

    m_connectionStatusLabel = CCLabelBMFont::create(
        "Connecting to Archipelago...",
        "bigFont.fnt"
    );

    m_connectionStatusLabel->setScale(0.45f);
    m_connectionStatusLabel->setPosition(0.f, -65.f);

    this->addChild(m_connectionStatusLabel);

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->show();
    m_loadingCircle->setParentLayer(this);

    // TODO: Disallow the player to do anything while the LoadingCircle is still there, much like logging in


    this->schedule(
        schedule_selector(ConnectPopup::updateConnection),
        0.25f
    );
}

void updateConnection(float dt) {
    if (!m_isConnecting || m_connectionFinished) {
        return;
    }

    m_connectionElapsed += dt;

    auto status = AP_GetConnectionStatus();

    switch (status) {
        case AP_ConnectionStatus::Disconnected:
            m_connectionStatusLabel->setString(
                "Connecting to server..."
            );
            break;

        case AP_ConnectionStatus::Connected:
            m_connectionStatusLabel->setString(
                "Authenticating..."
            );
            break;

        case AP_ConnectionStatus::Authenticated:
            m_connectionFinished = true;
            m_isConnecting = false;

            this->unschedule(
                schedule_selector(ConnectPopup::updateConnection)
            );

            if (m_loadingCircle) {
                m_loadingCircle->fadeAndRemove();
                m_loadingCircle = nullptr;
            }

            m_connectionStatusLabel->setString(
                "Successfully connected!"
            );

            geode::log::info("Authenticated with Archipelago!");
            
            Mod::get()->setSavedValue<bool>("InitArchMode", true);
            // restarting the game does not work.

        case AP_ConnectionStatus::ConnectionRefused:
            m_connectionFinished = true;
            m_isConnecting = false;

            this->unschedule(
                schedule_selector(ConnectPopup::updateConnection)
            );

            if (m_loadingCircle) {
                m_loadingCircle->fadeAndRemove();
                m_loadingCircle = nullptr;
            }

            m_connectionStatusLabel->setString(
                "Connection refused!"
            );

            break;
    }

    if (!m_connectionFinished && m_connectionElapsed >= 30.f) {
        m_connectionFinished = true;
        m_isConnecting = false;

        this->unschedule(
            schedule_selector(ConnectPopup::updateConnection)
        );

        if (m_loadingCircle) {
            m_loadingCircle->fadeAndRemove();
            m_loadingCircle = nullptr;
        }

        m_connectionStatusLabel->setString(
            "Connection timed out!"
        );
    }
}

void onExit() {
    this->unschedule(
        schedule_selector(ConnectPopup::updateConnection)
    );

    Popup::onExit();
}
void onClick(CCObject* sender) {
    if (Mod::get()->getSavedValue<bool>("StayingInArchMode", false)) {
        geode::createQuickPopup(
            "Disconnect",
            "Are you sure you want to disconnect from Archipelago? This will restart your game and restore your save prior to restarting.",
            "No", "Yes",
            [this](auto, bool btn2) {
                if (btn2) {
                    Mod::get()->setSavedValue<bool>("LoadBackup", true);
                    Mod::get()->setSavedValue<bool>("StayingInArchMode", false);
                    auto res = Mod::get()->saveData();
                    if (!res.isOk()) {
                        FLAlertLayer::create("Error", fmt::format("The mod did not save data correctly! Error: {}", res.unwrapErr()), "Ok")->show();
                        return;
                    }
                    geode::utils::game::restart(false);
                }
            }
        );
    } else {
    geode::createQuickPopup(
        "Connect",
        fmt::format("Are you sure you want to connect to Archipelago? When you agree to this, your game will restart into Archipelago mode. This will <cr>RESET YOUR GAME</c> (including settings) and back your save data up locally, twice. You might want to save on the cloud. To leave Archipelago mode, open this menu when connected and choose <cr>\"Disconnect\".</c>\n<cg>Server:</c> {} \n<cb>Slot:</c> {}",  urlInput->getString(), slotInput->getString()),
        "No", "Yes",
        [this](auto, bool btn2) { 
            if (btn2) {
                std::error_code error;
                std::string url = urlInput->getString();
                std::string slot = slotInput->getString();
                std::string pass = passInput->getString();
                if (url == "" || slot == "") {
                    FLAlertLayer::create("Error", "You haven't provided a url and/or slot name! Please fill out those fields. If you need help, look at the setup guide!", "Ok")->show();
                    return;
                }
                Mod::get()->setSavedValue<std::string>("recent-url", url);
                Mod::get()->setSavedValue<std::string>("recent-slot", slot);
                Mod::get()->setSavedValue<std::string>("recent-pass", pass);
                auto res = Mod::get()->saveData();
                if (!res.isOk()) {
                    FLAlertLayer::create("Error", fmt::format("The mod did not save data correctly! Error: {}", res.unwrapErr()), "Ok")->show();
                    return;
                }
                geode::log::info("Attempting to connect to AP world...");
                APUtils::startArchipelago(url.c_str(), slot.c_str(), pass.c_str());
                startConnectionMonitor();                
            }
        }
     );
    }
}

static ConnectPopup* create() {
    auto* ret = new ConnectPopup();

   
    if (ret && ret->init()) {
        ret->autorelease();   
    } else {
        CC_SAFE_DELETE(ret);
    }

    return ret;
};
};
