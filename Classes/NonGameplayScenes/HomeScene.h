#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#ifndef HOME_SCENE_H
#define HOME_SCENE_H

#include "GameData/GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"
#include "Layers/SettingsLayer.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
#include <vector>

USING_NS_CC;
using namespace ui;
class HomeScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(HomeScene, TAG);

    virtual bool init();
    virtual void update(float dt);
    virtual void onEnter();
    virtual void onExit();
    void getPeople();
    void getDialogue(Ref*, Widget::TouchEventType, string, Sprite*);

private:
    HomeScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
    int order;
    vector<Character> people_array;
    vector<SpellCard> card;
    GameData* gamedata;
    Sprite* cards[3];
    Sprite* bg;           //背景
    Sprite* word;         //艺术字
    Label* prog_text;     //进度
    Label* money_text;    //钱币
    ImageView* person[4]; //人物立绘
    int sum;
    Location loc; //地点
    string music;
};

#endif // HOME_SCENE_H
