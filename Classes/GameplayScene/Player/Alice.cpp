#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Alice.h"
#include "GameData/GameData.h"
#include "GameplayScene/Emitters/Emitter.h"

#define CREATE_AND_ADD_ANIMATION_CACHE(animation, character, frames, delayPerUnit, key)            \
    animation = Animation::create();                                                               \
    for (auto v : character.frames) {                                                              \
        animation->addSpriteFrameWithFile(v);                                                      \
    }                                                                                              \
    animation->setDelayPerUnit(character.delayPerUnit);                                            \
    AnimationCache::getInstance()->addAnimation(animation, key);

bool
Alice::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->playerTag = tag;
    this->setTag(playerCategoryTag);
    this->setName(tag);

    Character _character = GameData::getInstance()->getCharacterByTag(tag);
    playerSprite = Sprite::create(_character.defaultTexture);
    this->addChild(playerSprite);

    // 设置道具
    itemList = GameData::getInstance()->getCharacterItemList(tag);

    // 设置符卡
    spellCardList = GameData::getInstance()->getCharacterSpellCardList(tag);

    //设置属性值
    this->healthPointBase = _character.healthPointBase;
    this->manaBase = _character.manaBase;
    this->walkSpeedBase = _character.walkSpeedBase;
    this->walkMaxSpeed = _character.walkMaxSpeed;
    this->walkAccelerationTimeBase = _character.walkAccelerationTimeBase;
    this->walkAccelerationBase = _character.walkAccelerationBase;
    this->dashAccelerationBase = _character.dashAccelerationBase;

    //设置刚体
    body = PhysicsBody::createBox(Size(40, 75));
    body->setDynamic(true);
    body->setMass(1);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    body->getFirstShape()->setDensity(0);
    body->getFirstShape()->setFriction(0.2);
    body->getFirstShape()->setRestitution(0);
    body->getFirstShape()->setCategoryBitmask(playerCategory);
    body->getFirstShape()->setCollisionBitmask(groundCategory | elevatorCategory);
    body->getFirstShape()->setContactTestBitmask(groundCategory | enemyCategory | lockCategory |
                                                 eventCategory | elevatorCategory);
    this->setPhysicsBody(body);

    //设置动画
    CREATE_AND_ADD_ANIMATION_CACHE(standAnimation, _character, standFrame, standFrameDelay,
                                   "AliceStandAnimation");

    CREATE_AND_ADD_ANIMATION_CACHE(runAnimation, _character, runFrame, runFrameDelay,
                                   "AliceRunAnimation");

    CREATE_AND_ADD_ANIMATION_CACHE(preJumpAnimation, _character, preJumpFrame, preJumpFrameDelay,
                                   "AlicePreJumpAnimation");

    CREATE_AND_ADD_ANIMATION_CACHE(jumpAnimation, _character, jumpFrame, jumpFrameDelay,
                                   "AliceJumpAnimation");
    // Sequence不能执行RepeatForever，故在创建动画的时候设置循环属性
    jumpAnimation->setLoops(-1);

    CREATE_AND_ADD_ANIMATION_CACHE(preFallAnimation, _character, preFallFrame, preFallFrameDelay,
                                   "AlicePreFallAnimation");

    CREATE_AND_ADD_ANIMATION_CACHE(fallAnimation, _character, fallFrame, fallFrameDelay,
                                   "AliceFallAnimation");
    fallAnimation->setLoops(-1);

    CREATE_AND_ADD_ANIMATION_CACHE(dashAnimation, _character, dashFrame, dashFrameDelay,
                                   "AliceDashAnimation");

    // 设置攻击方式
    vector<Character::Attack> selectAttackList =
        GameData::getInstance()->getSelectedAttackList(tag);
    type1 = selectAttackList[0];
    type2 = selectAttackList[1];
    currentAttackType = type1.tag;

    // 根据攻击方式，从文件中读取弹幕参数sc1和sc2
    // sc1 = ;
    // sc2 = ;

    //设置发射器
    this->emitter = Emitter::create(&(this->playerDirection));
    this->addChild(this->emitter);

    //启动属性状态更新
    this->updatePlayerStatus();

    //动画更新
    animateStateMachine = new StateMachine<Player>(this);
    animateStateMachine->changeState(StandAnimation::getInstance());

    return true;
}

void
Alice::playerRun(float dt)
{
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();

    if (this->playerDirection == Direction::RIGHT) {
        Vec2 impluse = Vec2(0, 0);

        if (velocity.x < -10) {
            body->setVelocity(Vec2(80, velocity.y));
        }
        if (velocity.x < walkMaxSpeed) {
            impluse.x =
                std::min(walkMaxSpeed / walkAccelerationTimeBase * dt, walkMaxSpeed - velocity.x);
        }
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(0, 0);

        if (velocity.x > 10) {
            body->setVelocity(Vec2(-80, velocity.y));
        }
        if (velocity.x > -walkMaxSpeed) {
            impluse.x =
                -std::min(walkMaxSpeed / walkAccelerationTimeBase * dt, walkMaxSpeed + velocity.x);
        }
        body->applyImpulse(impluse);
    }
}

void
Alice::playerJump()
{
    if (this->jumpCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); //再次跳跃时，重置Y轴速度为0

    //留空，在空中时不再接受水平加速，只有惯性
    //留空，对于不同的角色机制应有不同

    Vec2 impluse = Vec2(0.0f, 550.0f);
    body->applyImpulse(impluse);

    this->jumpCounts--;
}

void
Alice::playerDash()
{
    //留空，阻止连续dash

    if (this->dashCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); // dash时，重置Y轴速度为0

    //留空，将y轴速度短暂锁定为0，可以使角色不受重力

    if (this->playerDirection == Direction::RIGHT) {
        Vec2 impluse = Vec2(dashAccelerationBase + 150, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(-(dashAccelerationBase + 150), 0.0f);
        body->applyImpulse(impluse);
    }

    this->dashCounts--;

    animateStateMachine->changeState(DashAnimation::getInstance());
}

void
Alice::regainDashCounts(float dt)
{
    regainDashTimeAccumulation += dt;
    if (regainDashTimeAccumulation >= 3.0) {
        if (this->dashCounts < 2) {
            this->dashCounts++;
        }
        regainDashTimeAccumulation = 0;
    }
}

void
Alice::updatePlayerStatus()
{
    //回复dash次数
    this->schedule(CC_SCHEDULE_SELECTOR(Player::regainDashCounts));
}
