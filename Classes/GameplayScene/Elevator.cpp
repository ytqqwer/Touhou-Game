#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Elevator.h"
#include "GameplayScene/common.h"

bool
Elevator::init()
{
    this->setTag(elevatorCategoryTag);

    auto _elevator = Sprite::create("gameplayscene/manaBar.png");
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

    return true;
}

void
Elevator::moveTogether(float dt)
{
    auto posE = this->getPosition();
    auto offX = posE.x - prePosition.x;
    auto offY = posE.y - prePosition.y;
    prePosition = posE;
    auto posP = (*curTarget)->getPosition();
    (*curTarget)->setPosition(posP.x + offX, posP.y + offY);
}

void
Elevator::setTarget(Player*& target)
{
    curTarget = (Node**)(&target);
}
