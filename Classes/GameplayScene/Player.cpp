#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Player.h"
#include "GameplayScene.h"

bool
Player::init()
{
    if (!Node::init())
        return false;

    //定义两角色的序列帧动画,注意此处要经常切换纹理，所以不能用AnimationCache
    p1Animation = Animation::create();
    for (int i = 0; i <= 7; i++)
        p1Animation->addSpriteFrameWithFile("gameplayscene/walkFront00" + std::to_string(i) +
                                            ".png");
    p1Animation->setDelayPerUnit(0.15f);

    jumpAnimation = Animation::create();
    for (int i = 0; i <= 9; i++)
        jumpAnimation->addSpriteFrameWithFile("gameplayscene/jumpFront00" + std::to_string(i) +
                                              ".png");
    jumpAnimation->setDelayPerUnit(0.1f);

    dashAnimation = Animation::create();
    for (int i = 0; i <= 8; i++)
        dashAnimation->addSpriteFrameWithFile("gameplayscene/dashFront00" + std::to_string(i) +
                                              ".png");
    dashAnimation->setDelayPerUnit(0.07f);

    std::string playerTextureName = "gameplayscene/walkFront001.png";

    playerSprite =
        Sprite::create(playerTextureName); //此处必须初始化一张角色纹理，否则后面无法切换纹理
    this->addChild(playerSprite);

    Animation* playerAnimation;
    playerAnimation = p1Animation;

    playerAnim = Animate::create(playerAnimation);

    AnimationCache::getInstance()->addAnimation(p1Animation, "p1Animation");
    AnimationCache::getInstance()->addAnimation(jumpAnimation, "jumpAnimation");
    AnimationCache::getInstance()->addAnimation(dashAnimation, "dashAnimation");

    playerSprite->runAction(RepeatForever::create(playerAnim)); //初始时刻角色在奔跑
    // playerSprite->runAction(RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("p1Animation"))));//初始时刻角色在奔跑

    auto body = PhysicsBody::createBox(Size(50, 75));
    body->setDynamic(true);
    body->setMass(1);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    body->getFirstShape()->setDensity(0);
    body->getFirstShape()->setFriction(0.2);
    body->getFirstShape()->setRestitution(0);
    body->setCategoryBitmask(playerCategory);
    body->setCollisionBitmask(groundCategory | enemyCategory);
    body->setContactTestBitmask(groundCategory | enemyCategory);

    this->setTag(playerTag);
    this->setName("player");
    this->setPhysicsBody(body);
    return true;
}

void
Player::playerRun(float dt)
{
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();

    if (this->playerDirection.compare("right")) {
        Vec2 impluse = Vec2(0, 0);
        // Vec2 impluse = Vec2(-20.0f, 0.0f);
        // body->applyForce(Vec2(-100.0f, 0.0f));

        if (velocity.x > 10) {
            body->setVelocity(Vec2(-100, velocity.y));
        }

        if (velocity.x > -MAX_SPEED) {
            impluse.x = -std::min(MAX_SPEED / ACCELERATE_TIME * dt, MAX_SPEED + velocity.x);
        }
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(0, 0);
        // Vec2 impluse = Vec2(20.0f, 0.0f);
        // body->applyForce(Vec2(100.0f,0.0f));

        if (velocity.x < -10) {
            body->setVelocity(Vec2(100, velocity.y));
        }

        if (velocity.x < MAX_SPEED) {
            impluse.x = std::min(MAX_SPEED / ACCELERATE_TIME * dt, MAX_SPEED - velocity.x);
        }
        body->applyImpulse(impluse);
    }
}

void
Player::playerJump()
{
    if (this->jumpCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); //再次跳跃时，重置Y轴速度为0

    //留空，在空中时不再接受水平加速，只有惯性

    Vec2 impluse = Vec2(0.0f, 500.0f);
    body->applyImpulse(impluse);

    auto animate = Animate::create(AnimationCache::getInstance()->getAnimation("jumpAnimation"));
    playerSprite->stopAllActions();

    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);

    this->jumpCounts--;
}

void
Player::playerDash()
{
    //留空，阻止连续dash，可增加角色状态标志

    if (this->dashCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); // dash时，重置Y轴速度为0

    //留空，将y轴速度短暂锁定为0，可以使角色不受重力

    if (this->playerDirection.compare("right")) //当比对相等时返回false,wtf???
    {
        Vec2 impluse = Vec2(-350.0f, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(350.0f, 0.0f);
        body->applyImpulse(impluse);
    }
    auto animate = Animate::create(AnimationCache::getInstance()->getAnimation("dashAnimation"));
    playerSprite->stopAllActions();
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);

    this->dashCounts--;
}

void
Player::resetAction(Node* node)
{
    playerSprite->stopAllActions();

    playerSprite->runAction(RepeatForever::create(
        Animate::create(AnimationCache::getInstance()->getAnimation("p1Animation"))));
}

void
Player::regainDashCounts(float dt)
{
    this->dashCounts++;
}
