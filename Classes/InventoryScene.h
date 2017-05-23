#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef INVENTORY_SCENE_H
#define INVENTORY_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class InventoryScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(InventoryScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    InventoryScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // INVENTORY_SCENE_H
