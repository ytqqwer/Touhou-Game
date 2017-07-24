#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player.h"
#include "GameData/GameData.h"
#include "GameplayScene/common.h"

bool
Player::init(std::string tag)
{
    if (!Node::init())
        return false;

    animateManager = AnimateManager::getInstance();
    //此处必须初始化一张角色纹理，否则后面无法切换纹理
    playerSprite = Sprite::create(animateManager->addTexture(tag));
    this->addChild(playerSprite);

    //设置属性值
    //留空

    //设置刚体
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
    this->setPhysicsBody(body);
    this->setTag(playerTag);
    this->setName(tag);

    //设置动画
    runAnimation = animateManager->addRunCache(tag);
    jumpAnimation = animateManager->addJumpCache(tag);
    dashAnimation = animateManager->addDashCache(tag);
    playerSprite->runAction(
        RepeatForever::create(Animate::create(runAnimation))); //初始时刻角色在奔跑

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
    //留空，对于不同的角色机制应有不同

    Vec2 impluse = Vec2(0.0f, 500.0f);
    body->applyImpulse(impluse);

    playerSprite->stopAllActions();
    Animate* animate = Animate::create(jumpAnimation);
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
    //留空，对于不同的角色机制应有不同

    if (this->playerDirection.compare("right")) //当比对相等时返回false,wtf???
    {
        Vec2 impluse = Vec2(-350.0f, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(350.0f, 0.0f);
        body->applyImpulse(impluse);
    }

    playerSprite->stopAllActions();
    Animate* animate = Animate::create(dashAnimation);
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);

    this->dashCounts--;
}

void
Player::resetAction(Node* node)
{
    playerSprite->stopAllActions();
    playerSprite->runAction(RepeatForever::create(Animate::create(runAnimation)));
}

void
Player::regainDashCounts(float dt)
{
    this->dashCounts++;
}