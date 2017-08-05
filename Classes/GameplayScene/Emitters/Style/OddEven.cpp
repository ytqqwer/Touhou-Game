#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "OddEven.h"

OddEven::OddEven(Node* target)
{
    this->sc.style = StyleType::ODDEVEN;
    this->sc.frequency = 0.5f;
    this->sc.duration = 4.0;
    this->sc.count = 3;
    this->sc.number = 10;
    this->sc.bc.name = "b2_2_1.png";

    this->target = target;
    this->counter = 0;
}

OddEven::OddEven(const StyleConfig& sc, Node* target)
{
    this->sc = sc;
    this->target = target;
    this->counter = 0;
}

void
OddEven::createBullet()
{
    this->targetPos = target->getParent()->convertToWorldSpace(target->getPosition());
    this->schedule(schedule_selector(OddEven::shootBullet), sc.frequency);
}

void
OddEven::shootBullet(float dt)
{
    if (this->counter == sc.count) {
        this->counter = 0;
        this->targetPos = target->getParent()->convertToWorldSpace(target->getPosition());
        return;
    } else {

        Vec2 startPos = this->getParent()->convertToWorldSpace(this->getPosition());
        Vec2 dis = targetPos - startPos;

        for (int i = 0; i < sc.number; i++) {

            Sprite* spriteBullet = Bullet::create(sc.bc);
            bullets.pushBack(spriteBullet);
            spriteBullet->setAnchorPoint(Vec2(0.5, 0.5));
            spriteBullet->setRotation(CC_RADIANS_TO_DEGREES(Vec2(dis.y, dis.x).getAngle()));
            this->addChild(spriteBullet);

            auto actionMoveTo = MoveTo::create(sc.duration, targetPos);
            // auto actionInOut = EaseInOut::create(actionMoveTo, 1);
            auto actionDone = CallFuncN::create(CC_CALLBACK_1(OddEven::removeBullet, this));
            auto sequence = Sequence::create(actionMoveTo, actionDone, NULL);
            spriteBullet->runAction(sequence);
        }
        this->counter++;
    }
}