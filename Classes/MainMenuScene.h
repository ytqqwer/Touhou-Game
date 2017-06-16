#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "AppMacros.h"
#include "GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
#include <string>
#include <vector>

USING_NS_CC;

class MainMenuScene : public Scene
{
public:
	APP_SCENE_CREATE_FUNC(MainMenuScene, TAG);

	virtual bool init();
	virtual void onEnter();
	virtual void update(float dt);

private:
	MainMenuScene();

private:
	static const std::string TAG;

	// intorspection
	Size _visibleSize;
	GameData* gamedata;
};

#endif // MAIN_MENU_SCENE_H