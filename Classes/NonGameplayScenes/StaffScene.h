#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef STAFF_SCENE_H
#define STAFF_SCENE_H

#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class StaffScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(StaffScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    StaffScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // STAFF_SCENE_H
