#include "HPManaBar.h"
#include "gameplayScene/common.h"

USING_NS_CC;
using namespace std;

HpManaBar * HpManaBar::create(const std::string tag, const int maxHpValue, const int maxManaValue)
{
	auto bar = new (std::nothrow) HpManaBar(tag, maxHpValue, maxManaValue);
	if (bar && bar->init()) {
		bar->autorelease();
		return bar;
	}
	else {
		delete bar;
		bar = nullptr;
		return nullptr;
	}
}

HpManaBar::HpManaBar(const std::string tag, const int maxHpValue, const int maxManaValue) {
	_tag = tag;
	_maxHp = maxHpValue;
	_maxMana = maxManaValue;
}

bool
HpManaBar::init()
{
    if (!Sprite::init()) {
        return false;
    }

	/*  1. Hp */

    _hpBar = Sprite::create("gameplayscene/hpBar.png");
    _hpBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	  
    _hpBarBlack = Sprite::create("place_holder.png");
    _hpBarBlack->setColor(Color3B::BLACK);
    _hpBarBlack->setOpacity(128);
    _hpBarBlack->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _hpBarBlack->setContentSize(Size(_hpBar->getContentSize().width, _hpBar->getContentSize().height * 1.10));
    this->addChild(_hpBarBlack);  

	hpClippingNode = ClippingNode::create();
	hpClippingNode->setInverted(true);//显示底板
	hpClippingNode->setContentSize(_hpBar->getContentSize());
	_hpStencil = Sprite::create("place_holder.png");//模板的样式并不会显示
	_hpStencil->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	_hpStencil->setContentSize(Size(0 , _hpBar->getContentSize().height));
	hpClippingNode->setStencil(_hpStencil);//设置模板
	hpClippingNode->addChild(_hpBar);//设置底板
	this->addChild(hpClippingNode);

    /*  2. Mana */

    _manaBar = Sprite::create("gameplayscene/manaBar.png");
    _manaBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);

    _manaBarBlack = Sprite::create("place_holder.png");
    _manaBarBlack->setColor(Color3B::BLACK);
    _manaBarBlack->setOpacity(128);
    _manaBarBlack->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _manaBarBlack->setContentSize(Size(_manaBar->getContentSize().width, _manaBar->getContentSize().height * 1.10));
	_manaBarBlack->setPosition(Vec2(0, -25));
    this->addChild(_manaBarBlack);
	
	manaClippingNode = ClippingNode::create();
	manaClippingNode->setInverted(true);
	manaClippingNode->setContentSize(_manaBar->getContentSize());
	manaClippingNode->setPosition(Vec2(0, -25));
	_manaStencil = Sprite::create("place_holder.png");//模板的样式并不会显示
	_manaStencil->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	_manaStencil->setContentSize(Size(0, _manaBar->getContentSize().height));
	manaClippingNode->setStencil(_manaStencil);//设置模板
	manaClippingNode->addChild(_manaBar);//设置底板
	this->addChild(manaClippingNode);

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(
		"hp_change", [this](EventCustom* e) {
		auto change = (Hp_Mp_Change*)e->getUserData();
		if (this->_tag == change->tag) {
			auto currentWidth = _hpStencil->getContentSize().width;
			auto originWidth = this->_hpBar->getContentSize().width;

			double changePercent = -(double)change->value / this->_maxHp;//取反
			double decreasePercent = (double)currentWidth / originWidth;
			double currentHpPercent = changePercent + decreasePercent;
			if (currentHpPercent >= 1) {
				currentHpPercent = 1;
			}
			else if (currentHpPercent <= 0) {
				currentHpPercent = 0;
			}
			_hpStencil->setContentSize(Size(this->_hpBar->getContentSize().width * currentHpPercent, this->_hpBar->getContentSize().height));
		}
	});

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(
		"mana_change", [this](EventCustom* e) {
		auto change = (Hp_Mp_Change*)e->getUserData();
		if (this->_tag == change->tag) {
			auto currentWidth = _manaStencil->getContentSize().width;
			auto originWidth = this->_manaBar->getContentSize().width;

			double changePercent = -(double)change->value / this->_maxMana;//取反
			double decreasePercent = (double)currentWidth / originWidth;
			double currentMpPercent = changePercent + decreasePercent;
			if (currentMpPercent >= 1) {
				currentMpPercent = 1;
			}
			else if (currentMpPercent <= 0) {
				currentMpPercent = 0;
			}
			_manaStencil->setContentSize(Size(this->_manaBar->getContentSize().width * currentMpPercent, this->_manaBar->getContentSize().height));

		}
	});
	
	this->setScaleX(-1);//反转

    return true;
}
