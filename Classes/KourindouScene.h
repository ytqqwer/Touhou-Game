#ifndef KOURINDOU_SCENE_H
#define KOURINDOU_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class KourindouScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(KourindouScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    KourindouScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // KOURINDOU_SCENE_H
