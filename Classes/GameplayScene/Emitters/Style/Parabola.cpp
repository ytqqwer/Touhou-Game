#include "Parabola.h"

Parabola::Parabola(Direction* direction)
{
    //默认参数
    this->sc.style = StyleType::PARABOLA;
    this->sc.frequency = 0.2f;
    this->sc.duration = 1.5;
    this->sc.number = 5;
    this->sc.count = 3;
    this->sc.height = 100;
    this->sc.distance = 500;

    this->sc.bc.name = "b3_1_3.png";
    this->sc.bc.length = 20;
    this->sc.bc.width = 10;
    this->sc.bc.harm = 15;
    this->sc.bc._categoryBitmask = 0;
    this->sc.bc._collisionBitmask = 0;
    this->sc.bc._contactTestBitmask = 0;

    this->direction = direction;
    this->counter = 0;
}

Parabola::Parabola(const StyleConfig& sc, Direction* direction)
{
    this->sc = sc;
    this->direction = direction;
    this->counter = 0;
}

void
Parabola::createBullet()
{
    this->schedule(schedule_selector(Parabola::shootBullet), sc.frequency);
}

void
Parabola::shootBullet(float dt)
{
    if (this->counter == sc.count) {
        this->counter = 0;
        return;
    } else {
        this->counter++;
    }

    auto scene = Director::getInstance()->getRunningScene();

    for (int i = 0; i < sc.number; i++) {

        //构造贝塞尔结构参数
        auto startPoint = this->getParent()->convertToWorldSpace(this->getPosition());
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
        scene->addChild(spriteBullet);

        auto actionBezierTo = BezierTo::create(sc.duration, cfg);
        // auto actionInOut = EaseInOut::create(actionBezierTo, 0.5);
        auto actionRotate = RotateBy::create(sc.duration, (CCRANDOM_0_1() - 0.5) * 720);
        auto actionSpawn = Spawn::create(actionBezierTo, actionRotate, nullptr);
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(Parabola::removeBullet, this));
        auto sequence = Sequence::create(actionSpawn, actionDone, NULL);
        spriteBullet->runAction(sequence);
    }
}
