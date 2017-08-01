#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Bullet.h"

Bullet*
Bullet::create(const BulletConfig& bc)
{
    Bullet* sprite = new Bullet(bc);
    if (sprite && sprite->init()) {
        sprite->autorelease();
        return sprite;
    } else {
        delete sprite;
        sprite = NULL;
        return NULL;
    }
}

Bullet::Bullet(const BulletConfig& bc)
{
    this->bc = bc;
}

bool
Bullet::init()
{
    if (!Sprite::init()) {
        return false;
    }

    //设置纹理
    SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(bc.name);
    initWithSpriteFrame(frame);

    //绑定刚体，设置掩码值

    return true;
}