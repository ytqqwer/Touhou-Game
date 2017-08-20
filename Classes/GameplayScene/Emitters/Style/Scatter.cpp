#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Scatter.h"

Scatter::Scatter()
{
    //敌人默认参数
    this->sc.style = StyleType::SCATTER;
    this->sc.frequency = 0.5f;
    this->sc.duration = 6.0;
    this->sc.count = 3;
    this->sc.number = 20;

    this->sc.bc.name = "b2_2_1.png";
    this->sc.bc.length = 20;
    this->sc.bc.width = 10;
    this->sc.bc.harm = 10;
    this->sc.bc._categoryBitmask = 0;
    this->sc.bc._collisionBitmask = 0;
    this->sc.bc._contactTestBitmask = 0;

    this->sc.startAngle = 90;
    this->sc.endAngle = 180;
    this->isPlayer = false;
    this->counter = 0;
}

Scatter::Scatter(const StyleConfig& sc)
{
    this->isPlayer = false;
    this->sc = sc;
    this->counter = 0;
}

Scatter::Scatter(Direction* direction)
{
    //角色默认参数
    this->sc.style = StyleType::SCATTER;
    this->sc.frequency = 0.2f;
    this->sc.duration = 3.0;
    this->sc.count = 0;
    this->sc.number = 2;
    this->sc.bc.name = "b1_3_3.png";
    this->sc.startAngle = 269;
    this->sc.endAngle = 271;
    this->isPlayer = true;
    this->direction = direction;
    this->counter = 0;
}

Scatter::Scatter(const StyleConfig& sc, Direction* direction)
{
    this->isPlayer = true;
    this->sc = sc;
    this->direction = direction;
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
    if (sc.count != 0) {
        if (this->counter == sc.count) {
            this->counter = 0;
            return;
        } else {
            this->counter++;
        }
    }

    auto scene = Director::getInstance()->getRunningScene();
    auto layer = (GameplayScene*)scene;
    auto winSize = Director::getInstance()->getWinSize();
    float angle = CC_DEGREES_TO_RADIANS(sc.endAngle - sc.startAngle) / (sc.number - 1);
    float distance = sqrt(winSize.width * winSize.width + winSize.height * winSize.height);

    for (int i = 0; i < sc.number; i++) {

        Bullet* spriteBullet = Bullet::create(sc.bc);
        bullets.pushBack(spriteBullet);
        spriteBullet->setAnchorPoint(Vec2(0.5, 0.5));
        spriteBullet->setRotation(-sc.startAngle - i * angle);

        auto pos = this->getParent()->convertToWorldSpace(this->getPosition());
        spriteBullet->setPosition(pos);
        layer->addChild(spriteBullet);

        Vec2 deltaP = Vec2(distance * cos(CC_DEGREES_TO_RADIANS(sc.startAngle + 90) + i * angle),
                           distance * sin(CC_DEGREES_TO_RADIANS(sc.startAngle + 90) + i * angle));
        auto actionMove = MoveBy::create(sc.duration, deltaP);
        auto actionMoveBy = actionMove;
        if (isPlayer == true) { //强烈建议角色使用中心对称型子弹
            if ((*direction) == Direction::LEFT) {
                actionMoveBy = actionMove->reverse();
            }
        }
        auto actionInOut = EaseInOut::create(actionMoveBy, 1.0);
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(Scatter::removeBullet, this));
        auto sequence = Sequence::create(actionInOut, actionDone, NULL);
        spriteBullet->runAction(sequence);
    }
}
