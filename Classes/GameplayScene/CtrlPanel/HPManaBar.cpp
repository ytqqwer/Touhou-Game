#include "HPManaBar.h"

USING_NS_CC;
using namespace std;

bool
HpManaBar::init()
{
    if (!Sprite::init()) {
        return false;
    }

    /*  1. Hp */

    _hpBar = Sprite::create("gameplayscene/hpBar.png");
    _hpBar->setAnchorPoint(Vec2(0, 0));

    _hpBarBlack = Sprite::create("place_holder.png");
    _hpBarBlack->setColor(Color3B::BLACK);
    _hpBarBlack->setOpacity(128);
    _hpBarBlack->setAnchorPoint(Vec2(1, 0));
    // TODO, 此处先设置 30 做展示, HpManaBar 完成时记得改 30 为 0
    _hpBarBlack->setContentSize(Size(30, _hpBar->getContentSize().height * 0.90));
    _hpBarBlack->setPosition(_hpBar->getContentSize().width,
                             _hpBar->getContentSize().height * 0.05);

    this->addChild(_hpBar);
    this->addChild(_hpBarBlack);

    /*  2. Mana */

    _manaBar = Sprite::create("gameplayscene/manaBar.png");
    _manaBar->setAnchorPoint(Vec2(0, 0));
    _manaBar->setPosition(Vec2(0, -25));

    _manaBarBlack = Sprite::create("place_holder.png");
    _manaBarBlack->setColor(Color3B::BLACK);
    _manaBarBlack->setOpacity(128);
    _manaBarBlack->setAnchorPoint(Vec2(1, 0));
    // TODO, 此处先设置 30 做展示, HpManaBar 完成时记得改 30 为 0
    _manaBarBlack->setContentSize(Size(30, _manaBar->getContentSize().height * 0.90));
    _manaBarBlack->setPosition(_manaBar->getContentSize().width,
                               -25 + _manaBar->getContentSize().height * 0.05);

    this->addChild(_manaBar);
    this->addChild(_manaBarBlack);

    scheduleUpdate();

    return true;
}

void
HpManaBar::setPlayer(Player* p)
{
    _player = p;
}

void
HpManaBar::update(float)
{
    // TODO, 追踪玩家血蓝
}
