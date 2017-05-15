#ifndef HOME_SCENE_H
#define HOME_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class HomeScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(HomeScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    HomeScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // HOME_SCENE_H
