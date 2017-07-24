#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ROUND_SELECT_SCENE_H
#define ROUND_SELECT_SCENE_H

#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class RoundSelectScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(RoundSelectScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float dt);

private:
    RoundSelectScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // ROUND_SELECT_SCENE_H
