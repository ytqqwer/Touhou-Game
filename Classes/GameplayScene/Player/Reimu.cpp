#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Reimu.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameData/GameData.h"

bool
Reimu::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->playerTag = tag;
    this->setTag(playerCategoryTag);
    this->setName(tag);

    Character _character = GameData::getInstance()->getCharacterByTag(tag);
    //此处必须初始化一张角色纹理，否则后面无法切换纹理
    playerSprite = Sprite::create(_character.defaultTexture);
    this->addChild(playerSprite);

    // 设置攻击方式
    vector<Character::Attack> selectAttackList =
        GameData::getInstance()->getSelectedAttackList(tag);
    type1 = selectAttackList[0];
    type2 = selectAttackList[1];
    currentAttackType = type1.tag;

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
    body->setCategoryBitmask(playerCategory);
    body->setCollisionBitmask(groundCategory);
    body->setContactTestBitmask(groundCategory | enemyCategory | lockCategory | eventCategory);
    this->setPhysicsBody(body);

    //设置动画
    standAnimation = Animation::create();
    for (auto v : _character.standFrame) {
        standAnimation->addSpriteFrameWithFile(v);
    }
    standAnimation->setDelayPerUnit(_character.standFrameDelay);
    AnimationCache::getInstance()->addAnimation(standAnimation, "ReimuStandAnimation");

    runAnimation = Animation::create();
    for (auto v : _character.runFrame) {
        runAnimation->addSpriteFrameWithFile(v);
    }
    runAnimation->setDelayPerUnit(_character.runFrameDelay);
    AnimationCache::getInstance()->addAnimation(runAnimation, "ReimuRunAnimation");

    preJumpAnimation = Animation::create();
    for (auto v : _character.preJumpFrame) {
        preJumpAnimation->addSpriteFrameWithFile(v);
    }
    preJumpAnimation->setDelayPerUnit(_character.preJumpFrameDelay);
    AnimationCache::getInstance()->addAnimation(preJumpAnimation, "ReimuPreJumpAnimation");

    jumpAnimation = Animation::create();
    for (auto v : _character.jumpFrame) {
        jumpAnimation->addSpriteFrameWithFile(v);
    }
    jumpAnimation->setDelayPerUnit(_character.jumpFrameDelay);
    AnimationCache::getInstance()->addAnimation(jumpAnimation, "ReimuJumpAnimation");
    // Sequence不能执行RepeatForever，故在创建动画的时候设置循环属性
    jumpAnimation->setLoops(-1);

    preFallAnimation = Animation::create();
    for (auto v : _character.preFallFrame) {
        preFallAnimation->addSpriteFrameWithFile(v);
    }
    preFallAnimation->setDelayPerUnit(_character.preFallFrameDelay);
    AnimationCache::getInstance()->addAnimation(preFallAnimation, "ReimuPreFallAnimation");

    fallAnimation = Animation::create();
    for (auto v : _character.fallFrame) {
        fallAnimation->addSpriteFrameWithFile(v);
    }
    fallAnimation->setDelayPerUnit(_character.fallFrameDelay);
    AnimationCache::getInstance()->addAnimation(fallAnimation, "ReimuFallAnimation");
    // Sequence不能执行RepeatForever，故在创建动画的时候设置循环属性
    fallAnimation->setLoops(-1);

    dashAnimation = Animation::create();
    for (auto v : _character.dashFrame) {
        dashAnimation->addSpriteFrameWithFile(v);
    }
    dashAnimation->setDelayPerUnit(_character.dashFrameDelay);
    AnimationCache::getInstance()->addAnimation(dashAnimation, "ReimuDashAnimation");

    curAction = ActionState::Default;
    playerSprite->runAction(RepeatForever::create(Animate::create(standAnimation)));

    //创建BatchNode节点，成批渲染子弹
    bulletBatchNode = SpriteBatchNode::create("gameplayscene/bullet1.png");
    this->addChild(bulletBatchNode);

    //启动状态更新
    this->schedule(CC_SCHEDULE_SELECTOR(Reimu::updatePlayerStatus));
    this->schedule(CC_SCHEDULE_SELECTOR(Reimu::autoSwitchAnimation), 0.1);

    return true;
}

void
Reimu::playerRun(float dt)
{
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();

    if (this->playerDirection == Direction::RIGHT) {
        Vec2 impluse = Vec2(0, 0);

        if (velocity.x < -10) {
            body->setVelocity(Vec2(100, velocity.y));
        }
        if (velocity.x < walkMaxSpeed) {
            impluse.x =
                std::min(walkMaxSpeed / walkAccelerationTimeBase * dt, walkMaxSpeed - velocity.x);
        }
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(0, 0);

        if (velocity.x > 10) {
            body->setVelocity(Vec2(-100, velocity.y));
        }
        if (velocity.x > -walkMaxSpeed) {
            impluse.x =
                -std::min(walkMaxSpeed / walkAccelerationTimeBase * dt, walkMaxSpeed + velocity.x);
        }
        body->applyImpulse(impluse);
    }
}

void
Reimu::playerJump()
{
    if (this->jumpCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); //再次跳跃时，重置Y轴速度为0

    //留空，在空中时不再接受水平加速，只有惯性
    //留空，对于不同的角色机制应有不同

    Vec2 impluse = Vec2(0.0f, 600.0f);
    body->applyImpulse(impluse);

    this->jumpCounts--;

    playerSprite->stopAllActions();
    curAction = ActionState::Default;
}

void
Reimu::playerDash()
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
        Vec2 impluse = Vec2(dashAccelerationBase + 100, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(-dashAccelerationBase + 100, 0.0f);
        body->applyImpulse(impluse);
    }

    this->dashCounts--;
    curAction = ActionState::Dash;

    playerSprite->stopAllActions();
    auto animate = Animate::create(dashAnimation);
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Reimu::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);
}

void
Reimu::changeAttackType(const std::string& startType)
{
    if (startType == "reimu focus attack 1") {
        this->schedule(CC_SCHEDULE_SELECTOR(Reimu::ShootBullet), 0.3f);
    } else if (startType == "reimu focus attack 2") {

    } else if (startType == "reimu split attack 1") {

    } else if (startType == "reimu split attack 2") {
    }

    this->currentAttackType = startType;
}

void
Reimu::stopAttackType(const std::string& stopType)
{
    if (stopType == "reimu focus attack 1") {
        this->unschedule(CC_SCHEDULE_SELECTOR(Reimu::ShootBullet));
    } else if (stopType == "reimu focus attack 2") {

    } else if (stopType == "reimu split attack 1") {

    } else if (stopType == "reimu split attack 2") {
    }
}

void
Reimu::updatePlayerStatus(float dt)
{
    //回复dash次数
    if (this->dashCounts < 2) {
        if (this->isScheduled(CC_SCHEDULE_SELECTOR(Player::regainDashCounts))) {
            ;
        } else {
            this->scheduleOnce(CC_SCHEDULE_SELECTOR(Player::regainDashCounts), 3.0f);
        }
    }
}
