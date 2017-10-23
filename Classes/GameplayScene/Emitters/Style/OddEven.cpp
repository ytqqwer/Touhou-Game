#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "OddEven.h"

OddEven::OddEven(Node** target)
{
    this->sc.style = StyleType::ODDEVEN;
    this->sc.frequency = 0.5f;
    this->sc.bulletDuration = 3.0;
    this->sc.count = 3;
    this->sc.number = 5;

    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->sc.bc.name = "b2_2_1.png";
    this->sc.bc.length = 10;
    this->sc.bc.width = 20;
    this->sc.bc.harm = 10;
    this->sc.bc._categoryBitmask = 0;
    this->sc.bc._collisionBitmask = 0;
    this->sc.bc._contactTestBitmask = 0;

    this->angle = 10.0;
    this->target = target;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

OddEven::OddEven(const StyleConfig& sc, Node** target)
{
    this->sc = sc;
    this->target = target;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

void
OddEven::startShoot()
{
    this->targetPos = (*target)->getPosition();
    this->schedule(schedule_selector(OddEven::shootBullet), sc.frequency);
}

void
OddEven::stopShoot()
{
    this->unschedule(schedule_selector(OddEven::shootBullet));
}

void
OddEven::shootBullet(float dt)
{
    spawnBullet();
}

void
OddEven::spawnBullet()
{
    auto emitter = this->getParent();
    auto character = emitter->getParent();
    auto mapLayer = character->getParent();

    auto winSize = Director::getInstance()->getWinSize();
    auto distance =
        sqrt(winSize.width * winSize.width + winSize.height * winSize.height); //飞行距离

    if (sc.count != 0) {
        if (this->counter == sc.count) {
            this->counter = 0;
            this->targetPos = (*target)->getPosition(); //更新目标位置
            return;
        } else {

            auto startPos = character->getPosition();
            auto dis = targetPos - startPos;                                   //基准向量
            auto datum = CC_RADIANS_TO_DEGREES(Vec2(dis.y, dis.x).getAngle()); //基础偏转角
            auto startAngle = datum + (sc.number - 1) * (angle / 2);           //起始偏转角

            for (int i = 0; i < sc.number; i++) {

                auto actualAngle = startAngle - i * angle; //实际偏转角

                Sprite* spriteBullet = Bullet::create(sc.bc);
                bullets.pushBack(spriteBullet);
                spriteBullet->setAnchorPoint(Vec2(0.5, 0.5));
                spriteBullet->setRotation(actualAngle);

                spriteBullet->setPosition(startPos);
                mapLayer->addChild(spriteBullet);

                Vec2 deltaP = Vec2(distance * cos(CC_DEGREES_TO_RADIANS(90.0 - actualAngle)),
                                   distance * sin(CC_DEGREES_TO_RADIANS(90.0 - actualAngle)));
                auto actionMoveBy = MoveBy::create(sc.bulletDuration, deltaP);
                auto actionInOut = EaseInOut::create(actionMoveBy, 1.0);
                auto actionDone = CallFuncN::create(CC_CALLBACK_1(OddEven::removeBullet, this));
                auto sequence = Sequence::create(actionInOut, actionDone, NULL);
                spriteBullet->runAction(sequence);
            }
            this->counter++;
        }
    }
}
