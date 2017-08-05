#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef INVENTORY_SCENE_H
#define INVENTORY_SCENE_H

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

USING_NS_CC;
using namespace ui;

class InventoryScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(InventoryScene, TAG);

    virtual bool init();
    virtual void update(float dt);
	void getitems(Ref*, Widget::TouchEventType,int n,float arrow_y);
    Sprite* getRect(double, double, double, double);

private:
    InventoryScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
    GameData* gamedata;
    int sum;
	string userflag[100];
    int typeflag[100] = { 0 };
	float arrow_y;
};

#endif // INVENTORY_SCENE_H
