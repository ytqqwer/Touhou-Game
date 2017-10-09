#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ITEM_BUTTON_H
#define ITEM_BUTTON_H

#include "GameData/Conversation.h"
#include "cocos2d.h"
USING_NS_CC;
#include "ui/CocosGUI.h"
using cocos2d::ui::Button;

class ConversationIndicatorButton : public Button
{
public:
    static ConversationIndicatorButton* create(const ConversationIndicator& i);
    virtual bool init();

private:
    ConversationIndicatorButton(const ConversationIndicator& i);

private:
    ConversationIndicator _indicator;
    Sprite* newTip;
};

#endif
