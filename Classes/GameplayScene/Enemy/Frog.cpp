#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Frog.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"

bool
Frog::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->enemyTag = tag;
    this->setTag(enemyCategoryTag);
    this->setName(tag);

    EnemyData _enemyData = GameData::getInstance()->getEnemyByTag(tag);
    enemySprite = Sprite::create(_enemyData.defaultTexture);
    this->addChild(enemySprite);

    //设置属性值
    this->hp = _enemyData.healthPoint;

    //设置刚体
    body = PhysicsBody::createBox(
        enemySprite->getContentSize()); //这里要用包围盒，如果用圆形的话会导致滚动
    body->setDynamic(true);
    body->setMass(1);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    body->getFirstShape()->setDensity(0);
    body->getFirstShape()->setFriction(0.2);
    body->getFirstShape()->setRestitution(0); //设置刚体回弹力
    body->setCategoryBitmask(enemyCategory);
    body->setCollisionBitmask(groundCategory | bulletCategory);
    body->setContactTestBitmask(groundCategory | bulletCategory);
    this->setPhysicsBody(body);

    //索敌检测框
    auto rect = PhysicsShapeBox::create(Size(200, 150));
    rect->setCategoryBitmask(lockCategory);
    rect->setCollisionBitmask(0);
    rect->setContactTestBitmask(playerCategory);
    body->addShape(rect);

    //设置速度上限
    body->setVelocityLimit(300);

    //设置动画
    idleAnimation = Animation::create();
    idleAnimation->retain(); //如果不手动增加引用计数，会被释放掉，未知原因
    for (auto v : _enemyData.standFrame) {
        idleAnimation->addSpriteFrameWithFile(v);
    }
    idleAnimation->setDelayPerUnit(_enemyData.standFrameDelay);
    idleAnimation->setLoops(-1);
    AnimationCache::getInstance()->addAnimation(idleAnimation, "FrogIdleAnimation");

    jumpAnimation = Animation::create();
    jumpAnimation->retain();
    for (auto v : _enemyData.jumpFrame) {
        jumpAnimation->addSpriteFrameWithFile(v);
    }
    jumpAnimation->setDelayPerUnit(_enemyData.jumpFrameDelay);
    AnimationCache::getInstance()->addAnimation(jumpAnimation, "FrogJumpAnimation");

    fallAnimation = Animation::create();
    fallAnimation->retain();
    for (auto v : _enemyData.fallFrame) {
        fallAnimation->addSpriteFrameWithFile(v);
    }
    fallAnimation->setDelayPerUnit(_enemyData.fallFrameDelay);
    AnimationCache::getInstance()->addAnimation(fallAnimation, "FrogFallAnimation");

    enemySprite->runAction(Animate::create(idleAnimation));

    //启动状态更新
    this->schedule(CC_SCHEDULE_SELECTOR(Frog::autoSwitchAnimation), 0.1);

    return true;
}

void
Frog::run(float dt)
{
    return;
}

void
Frog::jump()
{
    if (this->enemyDirection == Direction::LEFT) {
        body->applyImpulse(Vec2(-250.0f, 600.0f));
    } else {
        body->applyImpulse(Vec2(250.0f, 600.0f));
    }
}

void
Frog::decreaseHp(Node* node)
{
    auto enemy = (Enemy*)node;
    enemy->hp = enemy->hp - 5;
    if (enemy->hp < 0) {
        enemy->removeFromParentAndCleanup(true);
    }
}

void
Frog::switchMode()
{
    if (this->isScheduled(CC_SCHEDULE_SELECTOR(Frog::alertMode))) {
        this->unschedule(CC_SCHEDULE_SELECTOR(Frog::alertMode));
    }
    if (this->isScheduled(CC_SCHEDULE_SELECTOR(Frog::patrolMode))) {
        this->unschedule(CC_SCHEDULE_SELECTOR(Frog::patrolMode));
    }

    if (this->curState == EnemyActionMode::Patrol) {
        this->schedule(CC_SCHEDULE_SELECTOR(Frog::patrolMode), 1.0);
    } else if (this->curState == EnemyActionMode::Alert) {
        this->schedule(CC_SCHEDULE_SELECTOR(Frog::alertMode), 0.30);
        this->schedule(CC_SCHEDULE_SELECTOR(Frog::autoChangeDirection), 0.50);
    }
}

void
Frog::alertMode(float dt)
{
    int weight = random(0, 100);
    if (weight >= 80) {
        if (_canJump) {
            jump();
            this->_canJump = false;
        }
    }
}

void
Frog::patrolMode(float dt)
{
    int scale;
    int weight = random(0, 100);
    if (weight >= 50) {
        scale = 1;
        this->enemyDirection = Direction::LEFT;
    } else {
        scale = -1;
        this->enemyDirection = Direction::RIGHT;
    }
    enemySprite->setScaleX(scale);
}

void
Frog::autoChangeDirection(float dt)
{
    Point enemyPos = this->getPosition();
    Vec2 playerPos = (*curPlayer)->getPosition();
    if (enemyPos.x > playerPos.x) {
        this->enemyDirection = Direction::LEFT;
        enemySprite->setScaleX(1);
    } else {
        this->enemyDirection = Direction::RIGHT;
        enemySprite->setScaleX(-1);
    }
}

void
Frog::autoSwitchAnimation(float dt)
{
    Vec2 velocity = body->getVelocity();
    if (-15 < velocity.y && velocity.y < 15) {
        if (curAction != ActionState::Stand) { //站立
            if (curAction != ActionState::Jump) {
                if (curAction != ActionState::Fall) {
                    curAction = ActionState::Stand;
                    enemySprite->stopAllActions();
                    enemySprite->runAction(Animate::create(this->idleAnimation));
                }
            }
        }
    }
    if (-15 > velocity.y || velocity.y > 15) {
        if (velocity.y > 15) { //跳跃
            if (curAction != ActionState::Jump) {
                curAction = ActionState::Jump;
                enemySprite->stopAllActions();
                enemySprite->runAction(Animate::create(this->jumpAnimation));
            }
        } else if (-15 > velocity.y) { //下降
            if (curAction != ActionState::Fall) {
                curAction = ActionState::Fall;
                enemySprite->stopAllActions();
                enemySprite->runAction(Animate::create(this->fallAnimation));
            }
        }
    }
}
