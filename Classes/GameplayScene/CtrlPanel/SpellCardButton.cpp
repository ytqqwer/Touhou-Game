#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "SpellCardButton.h"
#include "CoolDownButton.h"

CoolDownButton*
SpellCardButton::create(const SpellCard& c)
{
    auto button = new (std::nothrow) SpellCardButton(c);

    if (button && button->init()) {
        button->autorelease();
        return button;
    } else {
        delete button;
        button = nullptr;
        return nullptr;
    }
}

SpellCardButton::SpellCardButton(const SpellCard& c)
{
    _spellCard = c;
}

void
SpellCardButton::subClassPreInit()
{
    _remainingUseCount = -1; // -1 表示没有使用次数限制
    _coolDownTime = _spellCard.cooldown;

    loadTextureNormal(_spellCard.icon);
    setScale(1.5);
    setName(_spellCard.tag);
}

void
SpellCardButton::initTouchListener()
{
    addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            this->_eventDispatcher->dispatchCustomEvent("use_spell_card",
                                                        (void*)this->_spellCard.tag.c_str());
            this->useOnce();
        }
    });
}
