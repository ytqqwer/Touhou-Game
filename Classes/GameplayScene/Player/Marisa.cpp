#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Marisa.h"
#include "GameplayScene/Emitters/Emitter.h"

bool
Marisa::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->playerTag = tag;
    this->setTag(playerCategoryTag);
    this->setName(tag);

    //此处必须初始化一张角色纹理，否则后面无法切换纹理
    playerSprite = Sprite::create("gameplayscene/Marisa/walkFront000.png");
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
    Character character = GameData::getInstance()->getCharacterByTag(tag);
    this->healthPointBase = character.healthPointBase;
    this->manaBase = character.manaBase;
    this->walkSpeedBase = character.walkSpeedBase;
    this->walkMaxSpeed = character.walkMaxSpeed;
    this->walkAccelerationTimeBase = character.walkAccelerationTimeBase;
    this->walkAccelerationBase = character.walkAccelerationBase;
    this->dashAccelerationBase = character.dashAccelerationBase;

    //设置刚体
    body = PhysicsBody::createBox(Size(50, 75));
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
    for (int i = 0; i <= 9; i++)
        standAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/stand00" + std::to_string(i) +
                                               ".png");
    standAnimation->setDelayPerUnit(0.1f);
    AnimationCache::getInstance()->addAnimation(standAnimation, "MarisaStandAnimation");

    runAnimation = Animation::create();
    for (int i = 0; i <= 7; i++)
        runAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/walkFront00" +
                                             std::to_string(i) + ".png");
    runAnimation->setDelayPerUnit(0.15f);
    AnimationCache::getInstance()->addAnimation(runAnimation, "MarisaRunAnimation");

    preJumpAnimation = Animation::create();
    for (int i = 1; i <= 4; i++)
        preJumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump00" + std::to_string(i) +
                                                 ".png");
    preJumpAnimation->setDelayPerUnit(0.08f);
    AnimationCache::getInstance()->addAnimation(preJumpAnimation, "MarisaPreJumpAnimation");

    jumpAnimation = Animation::create();
    for (int i = 5; i <= 5; i++)
        jumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump00" + std::to_string(i) +
                                              ".png");
    jumpAnimation->setDelayPerUnit(0.2f);
    AnimationCache::getInstance()->addAnimation(jumpAnimation, "MarisaJumpAnimation");
    // Sequence不能执行RepeatForever，故在创建动画的时候设置循环属性
    jumpAnimation->setLoops(-1);

    preFallAnimation = Animation::create();
    for (int i = 6; i <= 9; i++)
        preFallAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump00" + std::to_string(i) +
                                                 ".png");
    preFallAnimation->setDelayPerUnit(0.08f);
    AnimationCache::getInstance()->addAnimation(preFallAnimation, "MarisaPreFallAnimation");

    fallAnimation = Animation::create();
    for (int i = 10; i <= 11; i++)
        fallAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump0" + std::to_string(i) +
                                              ".png");
    fallAnimation->setDelayPerUnit(0.15f);
    AnimationCache::getInstance()->addAnimation(fallAnimation, "MarisaFallAnimation");
    // Sequence不能执行RepeatForever，故在创建动画的时候设置循环属性
    fallAnimation->setLoops(-1);

    dashAnimation = Animation::create();
    for (int i = 0; i <= 8; i++)
        dashAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/dashFront00" +
                                              std::to_string(i) + ".png");
    dashAnimation->setDelayPerUnit(0.07f);
    AnimationCache::getInstance()->addAnimation(dashAnimation, "MarisaDashAnimation");

    curAction = ActionState::Default;
    playerSprite->runAction(RepeatForever::create(Animate::create(standAnimation)));

    //创建BatchNode节点，成批渲染子弹
    bulletBatchNode = SpriteBatchNode::create("gameplayscene/bullet1.png");
    this->addChild(bulletBatchNode);

    //启动状态更新
    this->schedule(CC_SCHEDULE_SELECTOR(Marisa::updatePlayerStatus));
    this->schedule(CC_SCHEDULE_SELECTOR(Marisa::autoSwitchAnimation), 0.1);

    return true;
}

void
Marisa::playerRun(float dt)
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
Marisa::playerJump()
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

    this->jumpCounts--;

    playerSprite->stopAllActions();
    curAction = ActionState::Default;
}

void
Marisa::playerDash()
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
        Vec2 impluse = Vec2(dashAccelerationBase + 200, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(-dashAccelerationBase + 200, 0.0f);
        body->applyImpulse(impluse);
    }

    this->dashCounts--;
    curAction = ActionState::Dash;

    playerSprite->stopAllActions();
    auto animate = Animate::create(dashAnimation);
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Marisa::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);
}

void
Marisa::changeAttackType(const std::string& startType)
{
    if (startType == "marisa focus attack 1") {

    } else if (startType == "marisa focus attack 2") {

    } else if (startType == "marisa split attack 1") {

    } else if (startType == "marisa split attack 2") {
        this->schedule(CC_SCHEDULE_SELECTOR(Marisa::ShootBullet), 0.5f);
    }

    this->currentAttackType = startType;
}

void
Marisa::stopAttackType(const std::string& stopType)
{
    if (stopType == "marisa focus attack 1") {

    } else if (stopType == "marisa focus attack 2") {

    } else if (stopType == "marisa split attack 1") {

    } else if (stopType == "marisa split attack 2") {
        this->unschedule(CC_SCHEDULE_SELECTOR(Marisa::ShootBullet));
    }
}

void
Marisa::updatePlayerStatus(float dt)
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
