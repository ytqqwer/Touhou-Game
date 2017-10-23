#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/Enemy/Frog.h"
#include "GameplayScene/Enemy/Opossum.h"
#include "GameplayScene/Enemy/Sakuya.h"

Enemy*
Enemy::create(std::string tag)
{
    Enemy* pRet;
    if (tag == "Frog") {
        pRet = new (std::nothrow) Frog();
    } else if (tag == "Opossum") {
        pRet = new (std::nothrow) Opossum();
    } else if (tag == "Sakuya") {
        pRet = new (std::nothrow) Sakuya();
    }

    if (pRet && pRet->init(tag)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

void
Enemy::resetJump()
{
    this->_canJump = true;
}

void
Enemy::setTarget(Player*& player)
{
    curTarget = (Node**)&player;
}

void
Enemy::setEmitter()
{
    emitter = Emitter::create(curTarget);
    this->addChild(emitter);
}

void
Enemy::autoChangeDirection(float dt)
{
    Point enemyPos = this->getPosition();
    Vec2 playerPos = (*curTarget)->getPosition();
    if (enemyPos.x > playerPos.x) {
        this->enemyDirection = Direction::LEFT;
        enemySprite->setScaleX(1);
    } else {
        this->enemyDirection = Direction::RIGHT;
        enemySprite->setScaleX(-1);
    }
}
