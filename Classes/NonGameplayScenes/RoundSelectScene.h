#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ROUND_SELECT_SCENE_H
#define ROUND_SELECT_SCENE_H

#include "GameData/Character.h"
#include "GameData/Round.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;
#include "ui/CocosGUI.h"
using namespace ui;

class RoundSelectScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(RoundSelectScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

private:
    RoundSelectScene();

    void setRoundInformation(Round round);
    void setSpellCard(const std::string&, Vector<Sprite*>&, float);
    void setItem(const std::string&, Vector<Sprite*>&, float);

private:
    static const std::string TAG;

    Size _visibleSize;

    Sprite* difficult[5];
    Sprite* preViewPic;
    cocos2d::ui::Text* preViewText;
    std::string selectedMap;

    vector<Round> rounds;

    Character p1;
    Sprite* p1Portrait;
    Vector<Sprite*> p1CardSprites;
    Vector<Sprite*> p1ItemSprites;

    Character p2;
    Sprite* p2Portrait;
    Vector<Sprite*> p2CardSprites;
    Vector<Sprite*> p2ItemSprites;
};

#endif // ROUND_SELECT_SCENE_H
