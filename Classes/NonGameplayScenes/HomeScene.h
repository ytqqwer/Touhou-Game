#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#ifndef HOME_SCENE_H
#define HOME_SCENE_H

#include "GameData/GameData.h"
#include "NonGameplayScenes/ConversationIndicatorButton.h"
#include "TouhouGame.h"
#include <string>
#include <vector>

#include "cocos2d.h"
USING_NS_CC;

class HomeScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(HomeScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

private:
    HomeScene();

    void getPeople();

private:
    static const std::string TAG;

    Size _visibleSize;
    GameData* gamedata;

    vector<Character> people_array;
    Sprite* personPortrait;
    int people_order;

    vector<SpellCard> card;
    Sprite* cards[3];

    Sprite* backGround;
    Sprite* wordArt;
    Label* prog_text;
    Label* money_text;

    string music;

    Vector<ConversationIndicatorButton*> vector_indicator;
};

#endif // HOME_SCENE_H
