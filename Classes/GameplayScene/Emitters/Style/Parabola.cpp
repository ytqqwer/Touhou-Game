#include "Parabola.h"

Parabola::Parabola(Direction* direction)
{
    //默认参数
    this->sc.style = StyleType::PARABOLA;
    this->sc.frequency = 0.2f;
    this->sc.bulletDuration = 1.5;
    this->sc.number = 5;
    this->sc.countThenChangePos = 3;
    this->sc.height = 100;
    this->sc.distance = 500;

    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->sc.bc.name = "b3_1_3.png";
    this->sc.bc.length = 20;
    this->sc.bc.width = 10;
    this->sc.bc.harm = 15;
    this->sc.bc._categoryBitmask = 0;
    this->sc.bc._collisionBitmask = 0;
    this->sc.bc._contactTestBitmask = 0;

    this->direction = direction;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Parabola::Parabola(const StyleConfig& sc, Direction* direction)
{
    this->sc = sc;

    this->direction = direction;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

void
Parabola::startShoot()
{
    this->schedule(schedule_selector(Parabola::shootBullet), sc.frequency);
}

void
Parabola::stopShoot()
{
    this->unschedule(schedule_selector(Parabola::shootBullet));
}

void
Parabola::shootBullet(float dt)
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
Parabola::spawnBullet()
{
    if (this->counterInside == sc.countThenChangePos) {
        this->counterInside = 0;
    } else {
        this->counterInside++;
    }

    auto emitter = this->getParent();
    auto character = emitter->getParent();
    auto mapLayer = character->getParent();

    for (int i = 0; i < (2 * sc.number * CCRANDOM_0_1()); i++) {

        //构造贝塞尔结构参数

        // auto startPoint = this->getParent()->convertToWorldSpace(this->getPosition());
        auto startPoint = character->getPosition();
        Vec2 endPoint;

        if ((*direction) == Direction::LEFT) {
            endPoint = Vec2(startPoint.x - sc.distance - 100.0 * CCRANDOM_0_1(),
                            startPoint.y + 50.0 - 100.0 * CCRANDOM_0_1());
        } else {
            endPoint = Vec2(startPoint.x + sc.distance + 100.0 * CCRANDOM_0_1(),
                            startPoint.y + 50.0 - 100.0 * CCRANDOM_0_1());
        }

        float height = sc.height + 50.0 * CCRANDOM_0_1();
        float angle = 15.0 + 60.0 * CCRANDOM_0_1();

        float q1x = startPoint.x + (endPoint.x - startPoint.x) / 4.0;
        auto controlPoint1 =
            Vec2(q1x, height + startPoint.y + cos(CC_DEGREES_TO_RADIANS(angle) * q1x));

        float q2x = startPoint.x + (endPoint.x - startPoint.x) / 2.0;
        auto controlPoint2 =
            Vec2(q2x, height + startPoint.y + cos(CC_DEGREES_TO_RADIANS(angle) * q2x));

        //曲线配置
        ccBezierConfig cfg;
        cfg.controlPoint_1 = controlPoint1;
        cfg.controlPoint_2 = controlPoint2;
        cfg.endPosition = endPoint;

        auto spriteBullet = Bullet::create(sc.bc);
        bullets.pushBack(spriteBullet);
        spriteBullet->setAnchorPoint(Vec2(0.5, 0.5));
        spriteBullet->setScale(0.3 + 0.5 * CCRANDOM_0_1());

        spriteBullet->setPosition(startPoint);
        mapLayer->addChild(spriteBullet);

        auto actionBezierTo = BezierTo::create(sc.bulletDuration, cfg);
        // auto actionInOut = EaseInOut::create(actionBezierTo, 0.5);
        auto actionRotate = RotateBy::create(sc.bulletDuration, (CCRANDOM_0_1() - 0.5) * 720);
        auto actionSpawn = Spawn::create(actionBezierTo, actionRotate, nullptr);
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(Parabola::removeBullet, this));
        auto sequence = Sequence::create(actionSpawn, actionDone, NULL);
        spriteBullet->runAction(sequence);
    }
}
