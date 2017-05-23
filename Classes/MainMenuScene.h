#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class MainMenuScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(MainMenuScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    MainMenuScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // MAIN_MENU_SCENE_H
