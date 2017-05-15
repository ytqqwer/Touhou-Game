#ifndef JUMP_TABLE_SCENE_H
#define JUMP_TABLE_SCENE_H

#include "AppMacros.h"
#include "cocos2d.h"

USING_NS_CC;

class JumpTableScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(JumpTableScene, TAG);

    virtual bool init();

private:
    JumpTableScene(); // for autorealease and cache consideration, make ctor private

private:
    static const std::string TAG;

    Menu* _jumpTable;

    // intorspection
    Size _visibleSize;
};

#endif
