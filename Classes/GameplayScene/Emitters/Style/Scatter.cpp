#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Scatter.h"

Scatter::Scatter()
{
    //敌人默认参数
    this->sc.style = StyleType::SCATTER;
    this->sc.frequency = 0.5f;
    this->sc.bulletDuration = 6.0;
    this->sc.number = 13;

    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->sc.bc.name = "b1_3_3.png";
    this->sc.bc.length = 20;
    this->sc.bc.width = 10;
    this->sc.bc.harm = 10;
    this->sc.bc._categoryBitmask = 0;
    this->sc.bc._collisionBitmask = 0;
    this->sc.bc._contactTestBitmask = 0;

    this->sc.startAngle = 90;
    this->sc.endAngle = 180;
    this->sc.deltaAngle = 5;

    this->isPlayer = false;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Scatter::Scatter(const StyleConfig& sc)
{
    this->sc = sc;

    this->isPlayer = false;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Scatter::Scatter(Direction* direction)
{
    //角色默认参数
    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->sc.style = StyleType::SCATTER;
    this->sc.frequency = 0.2f;
    this->sc.bulletDuration = 3.0;
    this->sc.number = 2;
    this->sc.bc.name = "b1_3_3.png";
    this->sc.startAngle = 269;
    this->sc.endAngle = 271;
    this->sc.deltaAngle = 5;
    this->isPlayer = true;

    this->direction = direction;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Scatter::Scatter(const StyleConfig& sc, Direction* direction)
{
    this->sc = sc;

    this->isPlayer = true;
    this->direction = direction;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

void
Scatter::startShoot()
{
    this->schedule(schedule_selector(Scatter::shootBullet), sc.frequency);
}

void
Scatter::stopShoot()
{
    this->unschedule(schedule_selector(Scatter::shootBullet));
}

void
Scatter::shootBullet(float dt)
{
    timeAccumulation += dt;
    elapsed += dt;
    if (timeAccumulation >= sc.frequency) {
        spawnBullet();
        spawnBulletCycleTimes++;
        timeAccumulation = 0;
        if (spawnBulletCycleTimes >= sc.cycleTimes) {
            stopShoot();
        }
    }
    if (elapsed >= sc.totalDuration) {
        stopShoot();
    }
}

void
Scatter::spawnBullet()
{
    auto emitter = this->getParent();
    auto character = emitter->getParent();
    auto mapLayer = character->getParent();

    auto winSize = Director::getInstance()->getWinSize();
    sc.startAngle += sc.deltaAngle;
    sc.endAngle += sc.deltaAngle;

    float angle = CC_DEGREES_TO_RADIANS(sc.endAngle - sc.startAngle) / (sc.number - 1);
    float distance = sqrt(winSize.width * winSize.width + winSize.height * winSize.height);

    for (int i = 0; i < sc.number; i++) {

        Bullet* spriteBullet = Bullet::create(sc.bc);
        bullets.pushBack(spriteBullet);
        spriteBullet->setAnchorPoint(Vec2(0.5, 0.5));
        spriteBullet->setRotation(-sc.startAngle - i * angle);

        auto pos = character->getPosition();
        spriteBullet->setPosition(pos);
        mapLayer->addChild(spriteBullet);

        Vec2 deltaP = Vec2(distance * cos(CC_DEGREES_TO_RADIANS(sc.startAngle + 90) + i * angle),
                           distance * sin(CC_DEGREES_TO_RADIANS(sc.startAngle + 90) + i * angle));
        auto actionMove = MoveBy::create(sc.bulletDuration, deltaP);
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
