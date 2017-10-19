#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LIFE_MANA_BAR_H
#define LIFE_MANA_BAR_H

#include "cocos2d.h"

class HpManaBar : public cocos2d::Sprite
{
public:
	static HpManaBar* create(const std::string tag,const int maxHpValue,const int maxManaValue);

    virtual bool init() override;
	
private:
    HpManaBar(const std::string tag, const int maxHpValue, const int maxManaValue);

private:
	std::string _tag;

	int _maxHp;
    Sprite* _hpBar;
	Sprite* _hpStencil;
    Sprite* _hpBarBlack; 
	cocos2d::ClippingNode* hpClippingNode;

	int _maxMana;
    Sprite* _manaBar;
	Sprite* _manaStencil;
    Sprite* _manaBarBlack; 
	cocos2d::ClippingNode* manaClippingNode;
};

#endif // LIFE_MANA_BAR_H
