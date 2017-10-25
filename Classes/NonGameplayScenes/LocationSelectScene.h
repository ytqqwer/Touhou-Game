#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LOCATION_SELECT_SCENE_H
#define LOCATION_SELECT_SCENE_H

#include "GameData/GameData.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class LocationSelectScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(LocationSelectScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

private:
    LocationSelectScene();

private:
    static const std::string TAG;
    GameData* gamedata;
    Size _visibleSize;

    Menu* LocationList;
    Sprite* backGround;
    vector<Location> locations;

    class SelectLocationMenuItem : public MenuItemSprite
    {
    public:
        static SelectLocationMenuItem* create(const Location& i);
        SelectLocationMenuItem(const Location& i);
        virtual bool init();
        void callBack();

    private:
        Location location;
    };
};

#endif
