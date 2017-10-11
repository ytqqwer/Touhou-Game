#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef KOUMAKAN_LIBRARY_SCENE_H
#define KOUMAKAN_LIBRARY_SCENE_H

#include "NonGameplayScenes/ConversationIndicatorButton.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class KoumakanLibraryScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(KoumakanLibraryScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

private:
    KoumakanLibraryScene();

private:
    static const std::string TAG;

    Size _visibleSize;
    Label* money_text;
    Vector<ConversationIndicatorButton*> vector_indicator;
};

#endif // KOUMAKAN_LIBRARY_SCENE_H
