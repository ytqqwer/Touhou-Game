#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef SETTINGS_LAYER_H
#define SETTINGS_LAYER_H

#include "cocos2d.h"
#include <string>

USING_NS_CC;

class SettingsLayer : public Layer
{
public:
    CREATE_FUNC(SettingsLayer);
    static Scene* createDebugScene();

    virtual bool init();
    virtual void update(float dt);

private:
    SettingsLayer(); // for autorelease consideration, make ctor private

private:
    // intorspection
    Size _visibleSize;
};

#endif // SETTINGS_LAYER_H
