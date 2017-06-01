#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LOCATION_SELECT_SCENE_H
#define LOCATION_SELECT_SCENE_H

#include "AppMacros.h"
#include "GameData.h"
#include "GameData/Location.h"
#include "GameData/SpellCard.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>
#include <vector>

USING_NS_CC;

class LocationSelectScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(LocationSelectScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void update(float dt);
    Sprite* makeLocationOption(const Size& size, const std::string& wordArt,
                               const std::string& previewPicture, int passedRound, int totalRound);
    virtual void menuItemCallback();

private:
    LocationSelectScene();

private:
    static const std::string TAG;
    GameData* gamedata;

    Menu* LocationList;

    // intorspection
    Size _visibleSize;
};

#endif // MAP_SELECT_SCENE_H
