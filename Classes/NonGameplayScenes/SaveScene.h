#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef SAVE_SCENE_H
#define SAVE_SCENE_H

#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class SaveScene : public Scene
{
public:
    static Scene* create(bool isSaveAction = false);

    virtual bool init();

private:
    SaveScene(bool isSaveAction);

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;

    // game logic data
    bool _isSaveAction = false;
};

#endif // SAVE_SCENE_H
