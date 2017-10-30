#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Layers/ConfirmButton.h"
#include "GameData/GameData.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace cocos2d::ui;

ConfirmButton::ConfirmButton(std::function<void()> func, std::function<void()> callBack)
{
    _visibleSize = _director->getVisibleSize();
    this->func = func;
    this->callBack = callBack;
}

ConfirmButton*
ConfirmButton::create(std::function<void()> func, std::function<void()> callBack)
{
    ConfirmButton* pRet = new (std::nothrow) ConfirmButton(func, callBack);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
ConfirmButton::init()
{
    /*  1. super init first */

    if (!Layer::init()) {
        return false;
    }

    //触摸截断
    this->setLocalZOrder(8888);
    this->setTouchEnabled(true);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    /*  构建界面背景 */
    auto layout = Layout::create();
    layout->setContentSize(Size(_visibleSize.width / 2.0, _visibleSize.height / 2.0));
    layout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    layout->setBackGroundColor(Color3B(255, 255, 255));
    layout->setBackGroundColorOpacity(90);
    layout->setAnchorPoint(Vec2(0.5, 0.5));
    layout->setPosition(Vec2(_visibleSize.width / 2.0, _visibleSize.height / 2.0));

    /* 按钮组*/
    auto layoutSize = layout->getSize();

    auto confirmButton = Button::create();
    confirmButton->setTitleText("确定");
    confirmButton->setTitleFontSize(36);
    confirmButton->loadTextures("settings_layer/buttonNormal.png",
                                "settings_layer/buttonPressed.png");
    confirmButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    confirmButton->setScale(0.7);
    confirmButton->setPosition(Vec2(layoutSize.width / 4.0, layoutSize.height / 2.0));
    confirmButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            func();
            callBack();
            this->removeFromParentAndCleanup(true);
        }
    });
    layout->addChild(confirmButton);

    auto cancelButton = Button::create();
    cancelButton->setTitleText("取消");
    cancelButton->setTitleFontSize(36);
    cancelButton->loadTextures("settings_layer/buttonNormal.png",
                               "settings_layer/buttonPressed.png");
    cancelButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    cancelButton->setScale(0.7);
    cancelButton->setPosition(Vec2(layoutSize.width * 3.0 / 4.0, layoutSize.height / 2.0));
    cancelButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            this->removeFromParentAndCleanup(true);
        }
    });
    layout->addChild(cancelButton);

    addChild(layout);
    return true;
}
