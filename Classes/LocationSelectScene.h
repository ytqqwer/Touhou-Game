#ifndef MAP_SELECT_SCENE_H
#define MAP_SELECT_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class LocationSelectScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(LocationSelectScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    LocationSelectScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // MAP_SELECT_SCENE_H
