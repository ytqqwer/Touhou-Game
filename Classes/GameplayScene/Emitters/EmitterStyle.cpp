#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "EmitterStyle.h"
#define Pi 3.1415926

bool
EmitterStyle::init()
{

    if (!Node::init()) {
        return false;
    }
    return true;
}

void
EmitterStyle::removeAllObject()
{
    bullets.clear();
    this->removeAllChildren();
}

// void EmitterStyle::createBullet() {}

// void EmitterStyle::shootBullet(float dt) {}

void
EmitterStyle::removeBullet(Node* pNode)
{
    if (NULL == pNode) {
        return;
    }
    Sprite* bullet = (Sprite*)pNode;
    this->removeChild(bullet);
    bullets.eraseObject(bullet);
}
