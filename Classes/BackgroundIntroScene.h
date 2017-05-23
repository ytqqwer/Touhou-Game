#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef BACKGROUND_INTRO_SCENE_H
#define BACKGROUND_INTRO_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class BackgroundIntroScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(BackgroundIntroScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    BackgroundIntroScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // BACKGROUND_INTRO_SCENE_H
