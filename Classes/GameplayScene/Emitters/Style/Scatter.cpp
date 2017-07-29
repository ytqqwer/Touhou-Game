#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Scatter.h"

Scatter::Scatter(const StyleConfig& sc, Node* character)
{
    this->sc = sc;
    this->character = character;
}

Scatter::Scatter(Node* character)
{
    //默认参数
    this->character = character;
    this->count = 0;
}

void
Scatter::createBullet()
{
    this->schedule(schedule_selector(Scatter::shootBullet));
}

void
Scatter::shootBullet(float dt)
{
    if (this->count == sc.count) {
        return;
    }

    Size winSize = Director::getInstance()->getWinSize();
    auto characterPos = character->getPosition();
    float angle = (sc.endAngle - sc.startAngle) * Pi / (sc.number - 1);
    float s = sqrt(winSize.width * winSize.width + winSize.height * winSize.height);

    for (int i = 0; i < sc.number; i++) {

        Sprite* spriteBullet;

        bullets.pushBack(spriteBullet);
        spriteBullet->setAnchorPoint(Vec2(0.5, 0.0));
        spriteBullet->setRotation(-sc.startAngle - i * angle);
        spriteBullet->setPosition(characterPos);
        addChild(spriteBullet);

        Vec2 deltaP = Vec2(s * cos((sc.startAngle + 0.5) * Pi + i * angle),
                           s * sin((sc.startAngle + 0.5) * Pi + i * angle));
        auto actionMoveBy = MoveBy::create(6, deltaP);
        auto actionInOut = EaseInOut::create(actionMoveBy, 1);
        // auto actionInOut = EaseExponentialInOut::create(actionMoveBy);
        // auto actionInOut = EaseElasticInOut::create(actionMoveBy,0.8);
        // auto actionInOut = EaseBounceInOut::create(actionMoveBy);
        // auto actionInOut = EaseBackInOut::create(actionMoveBy);
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(Scatter::removeBullet, this));
        Sequence* sequence = Sequence::create(actionInOut, actionDone, NULL);
        spriteBullet->runAction(sequence);
    }
}
