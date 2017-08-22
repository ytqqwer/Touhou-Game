#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/Enemy/Frog.h"
#include "GameplayScene/Enemy/Opossum.h"

Enemy*
Enemy::create(std::string tag)
{
    Enemy* pRet;
    if (tag == "Frog") {
        pRet = new (std::nothrow) Frog();
    } else if (tag == "Opossum") {
        pRet = new (std::nothrow) Opossum();
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
Enemy::resetAction(Node* node)
{
    enemySprite->stopAllActions();
    curAction = ActionState::Default;
}

void
Enemy::setTarget(Player*& player)
{
    curTarget = (Node **)&player;
}
