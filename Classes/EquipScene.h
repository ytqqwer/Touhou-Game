#ifndef EQUIP_SCENE_H
#define EQUIP_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class EquipScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(EquipScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    EquipScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // EQUIP_SCENE_H
