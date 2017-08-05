#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ARMS_STORE_SCENE_H
#define ARMS_STORE_SCENE_H

#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/GameData.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
#include <vector>
using namespace ui;

USING_NS_CC;

class ArmsStorePurchaseScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(ArmsStorePurchaseScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    ArmsStorePurchaseScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif
