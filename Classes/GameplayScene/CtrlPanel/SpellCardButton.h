#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef SPELLCARD_BUTTON_H
#define SPELLCARD_BUTTON_H

#include "CoolDownButton.h"
#include "GameData/SpellCard.h"

class SpellCardButton : public CoolDownButton
{
public:
    static CoolDownButton* create(const SpellCard& i);

    virtual bool init() override;

    virtual void initUseCountAndCoolDownTime() override;
    virtual void initTouchListener() override;

private:
    SpellCardButton(const SpellCard&);

private:
    SpellCard _spellCard;
};

#endif // SPELLCARD_BUTTON_H
