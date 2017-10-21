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
    body->getFirstShape()->setCategoryBitmask(enemyCategory);
    body->getFirstShape()->setCollisionBitmask(groundCategory | bulletCategory | elevatorCategory);
    body->getFirstShape()->setContactTestBitmask(groundCategory | bulletCategory |
                                                 elevatorCategory | playerCategory);
    this->setPhysicsBody(body);

    //索敌检测框
    auto rect = PhysicsShapeBox::create(Size(200, 150));
    rect->setTag(lockCategoryTag);
    rect->setCategoryBitmask(lockCategory);
    rect->setCollisionBitmask(0);
    rect->setContactTestBitmask(playerCategory);
    body->addShape(rect);

    //设置速度上限
    body->setVelocityLimit(300);

    //获得动画缓存
    standAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.standAnimationKey);
    jumpAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.jumpAnimationKey);
    fallAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.fallAnimationKey);

    //行为模式状态机
    modeStateMachine = new StateMachine<Enemy>(this);
    modeStateMachine->changeState(FrogPatrolState::getInstance());
    //动画状态机
    animateStateMachine = new StateMachine<Enemy>(this);
    animateStateMachine->changeState(Frog::IdleAnimation::getInstance());

    return true;
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
Frog::decreaseHp(int damage)
{
    this->hp = this->hp - damage;
    if (this->hp < 0) {
        this->removeFromParentAndCleanup(true);
    }
}

FrogAlertState*
FrogAlertState::getInstance()
{
    static FrogAlertState instance;
    return &instance;
}

void
FrogAlertState::Enter(Enemy* entity)
{
    auto frog = (Frog*)entity;
    entity->schedule(
        [frog](float dt) {
            int weight = random(0, 100);
            if (weight >= 80) {
                if (frog->_canJump) {
                    frog->jump();
                    frog->_canJump = false;
                }
            }
        },
        0.30, "FrogAlertState");

    entity->schedule(CC_SCHEDULE_SELECTOR(Frog::autoChangeDirection), 0.50);
}

void
FrogAlertState::Exit(Enemy* entity)
{
    entity->unschedule("FrogAlertState");
    entity->unschedule(CC_SCHEDULE_SELECTOR(Frog::autoChangeDirection));
}

void
FrogAlertState::changeToState(Enemy* entity)
{
}

FrogPatrolState*
FrogPatrolState::getInstance()
{
    static FrogPatrolState instance;
    return &instance;
}

void
FrogPatrolState::Enter(Enemy* entity)
{
    entity->schedule(
        [entity](float dt) {
            int scale;
            int weight = random(0, 100);
            if (weight >= 50) {
                scale = 1;
                entity->enemyDirection = Direction::LEFT;
            } else {
                scale = -1;
                entity->enemyDirection = Direction::RIGHT;
            }
            entity->enemySprite->setScaleX(scale);
        },
        1.0, "FrogPatrolState");
}

void
FrogPatrolState::Exit(Enemy* entity)
{
    entity->unschedule("FrogPatrolState");
}

void
FrogPatrolState::changeToState(Enemy* entity)
{
    entity->modeStateMachine->changeState(FrogAlertState::getInstance());
}

Frog::IdleAnimation*
Frog::IdleAnimation::getInstance()
{
    static Frog::IdleAnimation instance;
    return &instance;
}

void
Frog::IdleAnimation::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = RepeatForever::create(Animate::create(frog->standAnimation));
    frog->enemySprite->runAction(frog->currentAnimateAction);

    frog->enemySprite->schedule(
        [frog](float dt) {
            Vec2 velocity = frog->body->getVelocity();
            if (velocity.y > 15) {
                frog->animateStateMachine->changeState(Frog::JumpAnimation::getInstance());
            } else if (velocity.y < -15) {
                frog->animateStateMachine->changeState(Frog::FallAnimation::getInstance());
            }
        },
        0.1, "StandAnimationUpdate");
}

void
Frog::IdleAnimation::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->enemySprite->stopAction(frog->currentAnimateAction);
    frog->enemySprite->unschedule("StandAnimationUpdate");
}

void
Frog::IdleAnimation::changeToState(Enemy* enemy)
{
}

Frog::JumpAnimation*
Frog::JumpAnimation::getInstance()
{
    static Frog::JumpAnimation instance;
    return &instance;
}

void
Frog::JumpAnimation::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = Animate::create(frog->jumpAnimation);
    frog->enemySprite->runAction(frog->currentAnimateAction);

    frog->enemySprite->schedule(
        [frog](float dt) {
            Vec2 velocity = frog->body->getVelocity();
            if (velocity.y < -15) {
                frog->animateStateMachine->changeState(Frog::FallAnimation::getInstance());
            }
        },
        0.1, "JumpAnimationUpdate");
}

void
Frog::JumpAnimation::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->enemySprite->stopAction(frog->currentAnimateAction);
    frog->enemySprite->unschedule("JumpAnimationUpdate");
}

void
Frog::JumpAnimation::changeToState(Enemy* enemy)
{
}

Frog::FallAnimation*
Frog::FallAnimation::getInstance()
{
    static Frog::FallAnimation instance;
    return &instance;
}

void
Frog::FallAnimation::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = Animate::create(frog->fallAnimation);
    frog->enemySprite->runAction(frog->currentAnimateAction);

    frog->enemySprite->schedule(
        [frog](float dt) {
            Vec2 velocity = frog->body->getVelocity();
            if (velocity.y > -15) {
                frog->animateStateMachine->changeState(Frog::IdleAnimation::getInstance());
            }
        },
        0.1, "FallAnimationUpdate");
}

void
Frog::FallAnimation::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->enemySprite->stopAction(frog->currentAnimateAction);
    frog->enemySprite->unschedule("FallAnimationUpdate");
}

void
Frog::FallAnimation::changeToState(Enemy* enemy)
{
}
