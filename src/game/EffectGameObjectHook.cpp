#include <Geode/modify/EffectGameObject.hpp>
#include "archipelago/APUtils.hpp"

using namespace geode::prelude;

class $modify(APEffectGameObject, EffectGameObject) {
    void triggerObject(
    GJBaseGameLayer* layer,
    int uniqueID,
    gd::vector<int> const* remapKeys
) {
    if (m_objectID == 142 && static_cast<int>(getType()) == 22) {
        APUtils::collectedCoins.insert(m_uniqueID);

        log::info("Collected coin object ID: {}", m_uniqueID);
    }

    EffectGameObject::triggerObject(layer, uniqueID, remapKeys);
}
};