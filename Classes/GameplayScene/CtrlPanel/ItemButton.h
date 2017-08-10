#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ITEM_BUTTON_H
#define ITEM_BUTTON_H

#include "CoolDownButton.h"
#include "GameData/Item.h"

class ItemButton : public CoolDownButton
{
public:
    static CoolDownButton* create(const Item& i);

    virtual bool init() override;

    virtual void initUseCountAndCoolDownTime() override;
    virtual void initTouchListener() override;

private:
    ItemButton(const Item& i);

private:
    Item _item;
};

#endif // ITEM_BUTTON_H
