#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef KOURINDOU_SCENE_H
#define KOURINDOU_SCENE_H

#include "NonGameplayScenes/ConversationIndicatorButton.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class KourindouScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(KourindouScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

private:
    KourindouScene();

private:
    static const std::string TAG;

    Size _visibleSize;
    Label* money_text;
    Vector<ConversationIndicatorButton*> vector_indicator;
};

#endif // KOURINDOU_SCENE_H
