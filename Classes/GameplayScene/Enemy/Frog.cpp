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
    standAnimation->setLoops(-1);

    //状态机
    stateMachine = new StateMachine<Enemy>(this);
    stateMachine->changeState(Patrol::getInstance());

    return true;
}

void
Frog::jump()
{
    if (this->enemyDirection == Direction::LEFT) {
        body->applyImpulse(Vec2(-300.0f, 800.0f));
    } else {
        body->applyImpulse(Vec2(300.0f, 800.0f));
    }
}

void
Frog::decreaseHp(int damage)
{
    this->hp = this->hp - std::abs(damage);
    if (this->hp < 0) {
        this->removeFromParentAndCleanup(true);
    }
}

void
Frog::Patrol::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = Animate::create(frog->standAnimation);
    frog->enemySprite->runAction(frog->currentAnimateAction);

    frog->schedule(
        [frog](float dt) {
            int scale;
            int weight = random(0, 100);
            if (weight >= 50) {
                scale = 1;
                frog->enemyDirection = Direction::LEFT;
            } else {
                scale = -1;
                frog->enemyDirection = Direction::RIGHT;
            }
            frog->enemySprite->setScaleX(scale);
        },
        1.0, "FrogPatrol");
}

void
Frog::Patrol::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->unschedule("FrogPatrol");
    frog->enemySprite->stopAction(frog->currentAnimateAction);
}

void
Frog::Patrol::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Frog::Alert::getInstance());
}

Frog::Jump*
Frog::Jump::getInstance()
{
    static Frog::Jump instance;
    return &instance;
}

Frog::Alert*
Frog::Alert::getInstance()
{
    static Frog::Alert instance;
    return &instance;
}

void
Frog::Alert::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = Animate::create(frog->standAnimation);
    frog->enemySprite->runAction(frog->currentAnimateAction);

    frog->schedule(
        [frog](float dt) {
            int weight = random(0, 100);
            if (weight >= 80) {
                if (frog->_canJump) {
                    frog->stateMachine->changeState(Frog::Jump::getInstance());
                    frog->_canJump = false;
                }
            }
        },
        0.30, "FrogAlert");
    frog->schedule(CC_SCHEDULE_SELECTOR(Frog::autoChangeDirection), 0.50);
}

void
Frog::Alert::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->unschedule("FrogAlert");
    frog->unschedule(CC_SCHEDULE_SELECTOR(Frog::autoChangeDirection));
    frog->enemySprite->stopAction(frog->currentAnimateAction);
}

void
Frog::Alert::defaultChangeState(Enemy* enemy)
{
}

Frog::Patrol*
Frog::Patrol::getInstance()
{
    static Frog::Patrol instance;
    return &instance;
}

void
Frog::Jump::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = Animate::create(frog->jumpAnimation);
    frog->enemySprite->runAction(frog->currentAnimateAction);
    frog->jump();
    frog->enemySprite->schedule(
        [frog](float dt) {
            Vec2 velocity = frog->body->getVelocity();
            if (velocity.y < -15) {
                frog->stateMachine->changeState(Frog::Fall::getInstance());
            }
        },
        "JumpUpdate");
}

void
Frog::Jump::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->enemySprite->stopAction(frog->currentAnimateAction);
    frog->enemySprite->unschedule("JumpUpdate");
}

void
Frog::Jump::defaultChangeState(Enemy* enemy)
{
}

Frog::Fall*
Frog::Fall::getInstance()
{
    static Frog::Fall instance;
    return &instance;
}

void
Frog::Fall::Enter(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->currentAnimateAction = Animate::create(frog->fallAnimation);
    frog->enemySprite->runAction(frog->currentAnimateAction);

    frog->enemySprite->schedule(
        [frog](float dt) {
            Vec2 velocity = frog->body->getVelocity();
            if (velocity.y > -15) {
                frog->stateMachine->changeState(Frog::Alert::getInstance());
            }
        },
        0.1, "FallUpdate");
}

void
Frog::Fall::Exit(Enemy* enemy)
{
    auto frog = (Frog*)enemy;
    frog->enemySprite->stopAction(frog->currentAnimateAction);
    frog->enemySprite->unschedule("FallUpdate");
}

void
Frog::Fall::defaultChangeState(Enemy* enemy)
{
}
