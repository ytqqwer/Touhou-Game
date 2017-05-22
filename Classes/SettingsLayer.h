#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef SETTINGS_LAYER_H
#define SETTINGS_LAYER_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <string>

USING_NS_CC;
using namespace std;
using namespace cocos2d::ui;

class SettingsLayer : public Layer
{
public:
    static Scene* createDebugScene();
    static Layer* create(const string& TAG);
    virtual bool init();
    void sliderEvent(Ref* pSender, Slider::EventType type);
    void touchEvent(Object* pSender, TouchEventType type);

private:
    SettingsLayer(const string& TAG); // for autorelease consideration, make ctor private

private:
    // intorspection
    Size _visibleSize;
    string currentScene;
};
#endif // SETTINGS_LAYER_H
