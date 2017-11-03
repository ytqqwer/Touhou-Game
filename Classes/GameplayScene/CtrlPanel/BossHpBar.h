#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef BOSS_HP_BAR_H
#define BOSS_HP_BAR_H

#include "cocos2d.h"

class BossHpBar : public cocos2d::Sprite
{
public:
    static BossHpBar* create(const Node* target, const int maxHpValue, const std::string& face);

    virtual bool init() override;

private:
    BossHpBar(const Node* target, const int maxHpValue, const std::string& face);

private:
    const Node* _target;
    std::string _face;

    int _maxHp;
    Sprite* _hpBar;
    Sprite* _hpStencil;
    Sprite* _hpBarBlack;
    cocos2d::ClippingNode* hpClippingNode;

    Sprite* avatar;
    Sprite* centerObject;
    Sprite* lifeGage;
};

#endif // BOSS_HP_BAR_H
