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
    this->sc.count = 3;
    this->sc.number = 20;

    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

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
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Scatter::Scatter(const StyleConfig& sc)
{
    this->sc = sc;

    this->isPlayer = false;

    this->counter = 0;
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
    this->sc.count = 0;
    this->sc.number = 2;
    this->sc.bc.name = "b1_3_3.png";
    this->sc.startAngle = 269;
    this->sc.endAngle = 271;
    this->isPlayer = true;

    this->direction = direction;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Scatter::Scatter(const StyleConfig& sc, Direction* direction)
{
    this->sc = sc;

    this->isPlayer = true;
    this->direction = direction;

    this->counter = 0;
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
    if (sc.count != 0) {
        if (this->counter == sc.count) {
            this->counter = 0;
            return;
        } else {
            this->counter++;
        }
    }

    auto emitter = this->getParent();
    auto character = emitter->getParent();
    auto mapLayer = character->getParent();

    auto winSize = Director::getInstance()->getWinSize();
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
