#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Scatter.h"

Scatter::Scatter()
{
    //默认参数
    this->sc.style = StyleType::SCATTER;
    this->sc.frequency = 0.5f;
    this->sc.duration = -1.0;
    this->sc.count = 3;
    this->sc.number = 20;
    this->sc.bc.name = "b2_2_1.png";
    this->sc.startAngle = 90;
    this->sc.endAngle = 180;

    this->counter = 0;
}

Scatter::Scatter(const StyleConfig& sc)
{
    this->sc = sc;
    this->counter = 0;
}

void
Scatter::createBullet()
{
    this->schedule(schedule_selector(Scatter::shootBullet), sc.frequency);
}

void
Scatter::shootBullet(float dt)
{
    if (this->counter == sc.count) {
        this->counter = 0;
        return;
    } else {
        this->counter++;
    }

    Size winSize = Director::getInstance()->getWinSize();
    float angle = CC_DEGREES_TO_RADIANS(sc.endAngle - sc.startAngle) / (sc.number - 1);
    float s = sqrt(winSize.width * winSize.width + winSize.height * winSize.height);

    for (int i = 0; i < sc.number; i++) {

        Sprite* spriteBullet = Bullet::create(sc.bc);

        bullets.pushBack(spriteBullet);
        spriteBullet->setAnchorPoint(Vec2(0.5, 0.0));
        spriteBullet->setRotation(-sc.startAngle - i * angle);
        this->addChild(spriteBullet);

        Vec2 deltaP = Vec2(s * cos(CC_DEGREES_TO_RADIANS(sc.startAngle + 90) + i * angle),
                           s * sin(CC_DEGREES_TO_RADIANS(sc.startAngle + 90) + i * angle));
        auto actionMoveBy = MoveBy::create(6, deltaP);
        auto actionInOut = EaseInOut::create(actionMoveBy, 1);
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(Scatter::removeBullet, this));
        auto sequence = Sequence::create(actionInOut, actionDone, NULL);
        spriteBullet->runAction(sequence);
    }
}
