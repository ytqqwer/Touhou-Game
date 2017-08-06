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

	this->playerTag = tag;
    this->setTag(playerCategoryTag);
    this->setName(tag);

    animateManager = AnimateManager::getInstance();
    //此处必须初始化一张角色纹理，否则后面无法切换纹理
    playerSprite = Sprite::create(animateManager->addPlayerTexture(tag));
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
    body->setContactTestBitmask(groundCategory | enemyCategory | lockCategory);
    this->setPhysicsBody(body);

    //设置动画，Sequence不能执行RepeatForever，故在创建动画的时候设置循环属性
    standAnimation = animateManager->addStandCache(tag);
    runAnimation = animateManager->addRunCache(tag);
    preJumpAnimation = animateManager->addPreJumpCache(tag);
    jumpAnimation = animateManager->addJumpCache(tag);
    jumpAnimation->setLoops(-1);
    preFallAnimation = animateManager->addPreFallCache(tag);
    fallAnimation = animateManager->addFallCache(tag);
    fallAnimation->setLoops(-1);
    dashAnimation = animateManager->addDashCache(tag);

    curAction = PlayerActionState::Default;
    playerSprite->runAction(RepeatForever::create(Animate::create(standAnimation)));

    //创建BatchNode节点，成批渲染子弹
    bulletBatchNode = SpriteBatchNode::create("gameplayscene/bullet1.png");
    this->addChild(bulletBatchNode);

    //启动状态更新
    this->schedule(CC_SCHEDULE_SELECTOR(Player::updateStatus));

    return true;
}

void
Player::playerRun(float dt)
{
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();

    if (this->playerDirection == PlayerDirection::RIGHT) {
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

    this->jumpCounts--;

    playerSprite->stopAllActions();
    curAction = PlayerActionState::Default;
}

void
Player::playerDash()
{
    //留空，阻止连续dash

    if (this->dashCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); // dash时，重置Y轴速度为0

    //留空，将y轴速度短暂锁定为0，可以使角色不受重力
    //留空，对于不同的角色机制应有不同

    if (this->playerDirection == PlayerDirection::RIGHT) {
        Vec2 impluse = Vec2(dashAccelerationBase, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(-dashAccelerationBase, 0.0f);
        body->applyImpulse(impluse);
    }

    this->dashCounts--;
    curAction = PlayerActionState::Dash;

    playerSprite->stopAllActions();
    auto animate = Animate::create(dashAnimation);
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);
}

void
Player::resetAction(Node* node)
{
    playerSprite->stopAllActions();
    curAction = PlayerActionState::Default;
}

void
Player::regainDashCounts(float dt)
{
    this->dashCounts++;
}

void
Player::changeAttackType(const std::string& startType)
{
    if (startType == "reimu focus attack 1") {
        this->schedule(CC_SCHEDULE_SELECTOR(Player::ShootBullet), 0.5f);
    } else if (startType == "reimu focus attack 2") {

    } else if (startType == "reimu split attack 1") {

    } else if (startType == "reimu split attack 2") {

    } else if (startType == "marisa focus attack 1") {

    } else if (startType == "marisa focus attack 2") {

    } else if (startType == "marisa split attack 1") {

    } else if (startType == "marisa split attack 2") {
        this->schedule(CC_SCHEDULE_SELECTOR(Player::ShootBullet), 0.2f);
    }

    this->currentAttackType = startType;
}

void
Player::stopAttackType(const std::string& stopType)
{
    if (stopType == "reimu focus attack 1") {
        this->unschedule(CC_SCHEDULE_SELECTOR(Player::ShootBullet));
    } else if (stopType == "reimu focus attack 2") {

    } else if (stopType == "reimu split attack 1") {

    } else if (stopType == "reimu split attack 2") {

    } else if (stopType == "marisa focus attack 1") {

    } else if (stopType == "marisa focus attack 2") {

    } else if (stopType == "marisa split attack 1") {

    } else if (stopType == "marisa split attack 2") {
        this->unschedule(CC_SCHEDULE_SELECTOR(Player::ShootBullet));
    }
}

void
Player::useItem(const std::string& itemTag)
{
    if (itemTag == "I1") {
        this->playerJump();
    } else if (itemTag == "I2") {
        this->playerJump();
    } else if (itemTag == "I3") {

    } else if (itemTag == "I4") {
        this->playerJump();
    } else if (itemTag == "I5") {

    } else if (itemTag == "I6") {

    } else if (itemTag == "I7") {

    } else if (itemTag == "I8") {

    } else if (itemTag == "I9") {

    } else if (itemTag == "I10") {

    } else if (itemTag == "I11") {

    } else if (itemTag == "I12") {
    }
}

void
Player::useSpellCard(const std::string& cardTag)
{
    if (cardTag == "C1") {
        this->playerJump();
    } else if (cardTag == "C2") {
        this->playerDash();
    } else if (cardTag == "C3") {
        this->playerDash();
    } else if (cardTag == "C4") {
        this->playerJump();
    } else if (cardTag == "C5") {
        this->playerJump();
    }
}

//用缓存的方法创建子弹，并初始化子弹的运动和运动后的事件
void
Player::ShootBullet(float dt)
{
    Size winSize = Director::getInstance()->getWinSize();
    //从缓存中创建子弹
    auto spritebullet = Sprite::createWithTexture(bulletBatchNode->getTexture());
    spritebullet->setTag(bulletCategoryTag);

    //将创建好的子弹添加到BatchNode中进行批次渲染
    bulletBatchNode->addChild(spritebullet);

    //给创建好的子弹添加刚体
    do {
        auto _body = PhysicsBody::createBox(spritebullet->getContentSize());
        _body->setRotationEnable(false);
        _body->setGravityEnable(false);

        _body->setContactTestBitmask(bulletCategory);
        _body->setCollisionBitmask(enemyCategory);
        _body->setContactTestBitmask(enemyCategory);
        spritebullet->setPhysicsBody(_body);
    } while (0);

    //将创建好的子弹添加到容器
    vecBullet.pushBack(spritebullet);

    float realFlyDuration = 1.0;
    //子弹运行的距离和时间
    auto actionMove = MoveBy::create(realFlyDuration, Point(winSize.width, 0));
    auto fire1 = actionMove;

    if (this->playerDirection == PlayerDirection::LEFT) {
        fire1 = actionMove->reverse();
    }

    //子弹执行完动作后进行函数回调，调用移除子弹函数
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::removeBullet, this));

    //子弹开始跑动
    Sequence* sequence = Sequence::create(fire1, actionDone, NULL);

    spritebullet->runAction(sequence);
}

//移除子弹，将子弹从容器中移除，同时也从SpriteBatchNode中移除
void
Player::removeBullet(Node* pNode)
{
    if (NULL == pNode) {
        return;
    }
    Sprite* bullet = (Sprite*)pNode;
    this->bulletBatchNode->removeChild(bullet, true);
    vecBullet.eraseObject(bullet);
}

void
Player::autoSwitchAction()
{
    if (curAction != PlayerActionState::Dash) {
        Vec2 velocity = body->getVelocity();
        if (-15 < velocity.y && velocity.y < 15) {
            if (-15 < velocity.x && velocity.x < 15) { //站立
                if (curAction != PlayerActionState::Stand) {
                    if (curAction != PlayerActionState::Jump) {
                        if (curAction != PlayerActionState::Fall) {
                            curAction = PlayerActionState::Stand;
                            playerSprite->stopAllActions();
                            playerSprite->runAction(
                                RepeatForever::create(Animate::create(standAnimation)));
                        }
                    }
                }
            } else if (-15 > velocity.x || velocity.x > 15) { //行走
                if (-15 < velocity.y || velocity.y < 15) {
                    if (curAction != PlayerActionState::Run) {
                        curAction = PlayerActionState::Run;
                        playerSprite->stopAllActions();
                        playerSprite->runAction(
                            RepeatForever::create(Animate::create(runAnimation)));
                    }
                }
            }
        }
        if (-15 > velocity.y || velocity.y > 15) {
            if (velocity.y > 15) { //向上跳跃
                if (curAction != PlayerActionState::Jump) {
                    curAction = PlayerActionState::Jump;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preJumpAnimation),
                                                     Animate::create(jumpAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            } else if (-15 > velocity.y) { //下降
                if (curAction != PlayerActionState::Fall) {
                    curAction = PlayerActionState::Fall;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preFallAnimation),
                                                     Animate::create(fallAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            }
        }
    }
}

void
Player::updateStatus(float dt)
{
    this->autoSwitchAction();

    //回复dash次数
    if (this->dashCounts < 2) {
        if (this->isScheduled(CC_SCHEDULE_SELECTOR(Player::regainDashCounts))) {
            ;
        } else {
            this->scheduleOnce(CC_SCHEDULE_SELECTOR(Player::regainDashCounts), 3.0f);
        }
    }
}
