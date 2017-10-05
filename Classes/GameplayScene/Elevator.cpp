#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Elevator.h"
#include "GameplayScene/common.h"

bool
Elevator::init()
{
    this->setTag(elevatorCategoryTag);

    auto _elevator = Sprite::create();
    this->addChild(_elevator);

    PhysicsBody* _body = PhysicsBody::createEdgeSegment(Vec2(0, 0), Vec2(300, 0),
                                                        PHYSICSBODY_MATERIAL_DEFAULT, 3.0f);
    _body->getFirstShape()->setDensity(0);
    _body->getFirstShape()->setFriction(1.0);
    _body->getFirstShape()->setRestitution(0);
    _body->setCategoryBitmask(elevatorCategory);
    _body->setCollisionBitmask(playerCategory | enemyCategory);
    _body->setContactTestBitmask(playerCategory | enemyCategory);
    this->setPhysicsBody(_body);

    this->prePosition = this->getPosition();
    this->schedule(CC_SCHEDULE_SELECTOR(Elevator::moveTogether));

    return true;
}

void
Elevator::moveTogether(float dt)
{
    auto posE = this->getPosition();
    auto offX = posE.x - prePosition.x;
    auto offY = posE.y - prePosition.y;
    prePosition = posE;

    if (!(passengers.empty())) {
        for (auto it = passengers.begin(); it != passengers.end(); ++it) {
            auto posN = (*it)->getPosition();
            (*it)->setPosition(posN.x + offX, posN.y + offY);
        }
    }
}
