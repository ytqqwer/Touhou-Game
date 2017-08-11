#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LIFE_MANA_BAR_H
#define LIFE_MANA_BAR_H

#include "cocos2d.h"

class Player;

class HpManaBar : public cocos2d::Sprite
{
public:
    CREATE_FUNC(HpManaBar);
    virtual bool init() override;
    virtual void update(float) override;

    void setPlayer(Player*);

private:
    HpManaBar() = default;

private:
    Player* _player;

    Sprite* _hpBar;
    Sprite* _hpBarBlack;
    Sprite* _manaBar;
    Sprite* _manaBarBlack;
};

#endif // LIFE_MANA_BAR_H
