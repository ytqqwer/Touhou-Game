#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/ConversationIndicatorButton.h"
#include "Layers/ConversationLayer.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

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

    // this->ignoreContentAdaptWithSize(true);	//忽视根据纹理大小进行的自动缩放，需要注意调用顺序
    // widget默认采用auto大小方案，大小为纹理的实际大小，并混合缩放来实现大小控制
    //打开scale9 可以拉伸图片
    // setContentSize(Size(100, 100)); //设置按钮大小，必须setScale9Enabled(true)，不然没有用

    loadTextureNormal(_indicator.icon);

    auto width = this->getContentSize().width;
    auto height = this->getContentSize().height;
    auto bigger = width > height ? width : height;
    float scale = 120.0 / bigger;
    this->setScale(scale);

    newTip = Sprite::create("homescene/p7.png");
    // newTip->setContentSize(Size());
    newTip->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height));
    newTip->setScale(1 / scale);
    this->addChild(newTip);

    this->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            this->newTip->setOpacity(0);
            auto conversationTag = this->_indicator.conversationTag;

            auto layer = ConversationLayer::create(conversationTag);
            Director::getInstance()->getRunningScene()->addChild(layer, 1000);
        }
    });

    return true;
}
