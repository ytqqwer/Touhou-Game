#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif 
#ifndef HOME_SCENE_H
#define HOME_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>
#include <vector>
#include "GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"
#include "ui/CocosGUI.h"

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
private:
    HomeScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
	int order;
	vector<Character> people_array;
	vector<SpellCard> card;
	GameData *gamedata;
	Sprite *cards[3];
};

#endif // HOME_SCENE_H
