#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef KOUMAKAN_LIBRARY_SCENE_H
#define KOUMAKAN_LIBRARY_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class KoumakanLibraryScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(KoumakanLibraryScene, TAG);

    virtual bool init();
    virtual void update(float dt);

private:
    KoumakanLibraryScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;
};

#endif // KOUMAKAN_LIBRARY_SCENE_H
