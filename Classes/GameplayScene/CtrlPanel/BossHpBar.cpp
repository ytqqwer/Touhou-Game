#include "BossHpBar.h"
#include "GameplayScene/common.h"

USING_NS_CC;
using namespace std;

BossHpBar*
BossHpBar::create(const Node* target, const int maxHpValue, const std::string& face)
{
    auto bar = new (std::nothrow) BossHpBar(target, maxHpValue, face);
    if (bar && bar->init()) {
        bar->autorelease();
        return bar;
    } else {
        delete bar;
        bar = nullptr;
        return nullptr;
    }
}

BossHpBar::BossHpBar(const Node* target, const int maxHpValue, const std::string& face)
{
    this->_target = target;
    this->_maxHp = maxHpValue;
    this->_face = face;
}

bool
BossHpBar::init()
{
    if (!Sprite::init()) {
        return false;
    }

    _hpBar = Sprite::create("gameplayscene/hpBar.png");
    _hpBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _hpBarBlack = Sprite::create("place_holder.png");
    _hpBarBlack->setColor(Color3B::BLACK);
    _hpBarBlack->setOpacity(128);
    _hpBarBlack->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _hpBarBlack->setContentSize(
        Size(_hpBar->getContentSize().width, _hpBar->getContentSize().height * 1.10));

    hpClippingNode = ClippingNode::create();
    hpClippingNode->setInverted(true); //显示底板
    hpClippingNode->setContentSize(_hpBar->getContentSize());
    _hpStencil = Sprite::create("place_holder.png"); //模板的样式并不会显示
    _hpStencil->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _hpStencil->setContentSize(Size(0, _hpBar->getContentSize().height));
    hpClippingNode->setStencil(_hpStencil); //设置模板
    hpClippingNode->addChild(_hpBar);       //设置底板

    lifeGage = Sprite::create("gameplayscene/2PlifeGage.png");
    lifeGage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    lifeGage->setPosition(_hpBarBlack->getPosition().x - 20, _hpBarBlack->getPosition().y - 10);

    avatar = Sprite::create(_face);
    avatar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    avatar->setScaleX(-1);
    avatar->setPosition(lifeGage->getPosition().x + lifeGage->getContentSize().width / 2.0 - 20,
                        lifeGage->getPosition().y + lifeGage->getContentSize().height / 2.0 - 13);

    centerObject = Sprite::create("gameplayscene/centerObject.png");
    centerObject->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    centerObject->setPosition(-50, 0);

    this->addChild(lifeGage);
    this->addChild(avatar);
    this->addChild(_hpBarBlack);
    this->addChild(hpClippingNode);
    this->addChild(centerObject);

    Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        "hp_change", [this](EventCustom* e) {
            auto change = (Hp_Mp_Change*)e->getUserData();
            if (this->_target == change->target) {
                auto currentWidth = _hpStencil->getContentSize().width;
                auto originWidth = this->_hpBar->getContentSize().width;

                double changePercent = -(double)change->value / this->_maxHp; //取反
                double decreasePercent = (double)currentWidth / originWidth;
                double currentHpPercent = changePercent + decreasePercent;
                if (currentHpPercent >= 1) {
                    currentHpPercent = 1;
                } else if (currentHpPercent <= 0) {
                    currentHpPercent = 0;
                }
                _hpStencil->setContentSize(
                    Size(this->_hpBar->getContentSize().width * currentHpPercent,
                         this->_hpBar->getContentSize().height));
            }
        });

    return true;
}
