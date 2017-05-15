#ifndef MAP_SELECT_SCENE_H
#define MAP_SELECT_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class MapSelectScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(MapSelectScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    MapSelectScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // MAP_SELECT_SCENE_H
