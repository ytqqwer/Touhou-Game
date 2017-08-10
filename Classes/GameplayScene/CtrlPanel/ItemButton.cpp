#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "ItemButton.h"

CoolDownButton*
ItemButton::create(const Item& i)
{
    auto button = new (std::nothrow) ItemButton(i);
    if (button && button->init()) {
        button->autorelease();
        return button;
    } else {
        delete button;
        button = nullptr;
        return nullptr;
    }
}

ItemButton::ItemButton(const Item& i)
{
    _item = i;
}

bool
ItemButton::init()
{
    if (!CoolDownButton::init())
        return false;

    loadTextureNormal(_item.icon);
    setScale(1.5);
    setName(_item.tag);

    return true;
}

void
ItemButton::initUseCountAndCoolDownTime()
{
    _remainingUseCount = _item.maxUseCount;
    _coolDownTime = _item.cooldown;
}

void
ItemButton::initTouchListener()
{
    addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            this->_eventDispatcher->dispatchCustomEvent("use_item", (void*)this->_item.tag.c_str());
            this->useOnce();
        }
    });
}
