#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "ConversationIndicatorButton.h"

ConversationIndicatorButton*
ConversationIndicatorButton::create(const ConversationIndicator& i)
{
    ConversationIndicatorButton* pRet;
    pRet = new (std::nothrow) ConversationIndicatorButton(i);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

ConversationIndicatorButton::ConversationIndicatorButton(const ConversationIndicator& i)
    : _indicator(i)
{
}

bool
ConversationIndicatorButton::init()
{
    if (!Button::init())
        return false;

    setName(_indicator.conversationTag);
    loadTextureNormal(_indicator.icon);

    newTip = Sprite::create("homescene/p7.png");
    // newTip->setContentSize(Size());
    newTip->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height));
    this->addChild(newTip);

    this->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            this->newTip->setOpacity(0);
            auto conversationTag = this->_indicator.conversationTag;
            //通过tag创建对话
        }
    });

    return true;
}
