#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Opossum.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"

bool
Opossum::init(const std::string& tag)
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
    auto rect = PhysicsShapeBox::create(Size(150, 200));
    rect->setTag(lockCategoryTag);
    rect->setCategoryBitmask(lockCategory);
    rect->setCollisionBitmask(0);
    rect->setContactTestBitmask(playerCategory);
    body->addShape(rect);

    //设置速度上限
    body->setVelocityLimit(500);

    //获得动画缓存
    runAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.runAnimationKey);
    runAnimation->setLoops(-1);

    //状态机
    stateMachine = new StateMachine<Enemy>(this);
    stateMachine->changeState(Opossum::Patrol::getInstance());

    return true;
}

void
Opossum::horizontallyAccelerate(float dt)
{
    timeAccumulation += dt;
    if (timeAccumulation >= 0.1) {
        if (this->enemyDirection == Direction::LEFT) {
            this->getPhysicsBody()->applyImpulse(Vec2(-30.0f, 0.0f));
        } else {
            this->getPhysicsBody()->applyImpulse(Vec2(30.0f, 0.0f));
        }
        timeAccumulation = 0;
    }
}

void
Opossum::decreaseHp(int damage)
{
    this->hp = this->hp - damage;
    if (this->hp < 0) {
        this->removeFromParentAndCleanup(true);
    }
}

Opossum::Alert*
Opossum::Alert::getInstance()
{
    static Alert instance;
    return &instance;
}

void
Opossum::Alert::Enter(Enemy* enemy)
{
    auto opossum = (Opossum*)enemy;
    opossum->currentAnimateAction = Animate::create(opossum->runAnimation);
    opossum->enemySprite->runAction(opossum->currentAnimateAction);

    opossum->schedule(CC_SCHEDULE_SELECTOR(Opossum::horizontallyAccelerate));
    opossum->schedule(CC_SCHEDULE_SELECTOR(Opossum::autoChangeDirection), 0.50);
}

void
Opossum::Alert::Exit(Enemy* enemy)
{
    auto opossum = (Opossum*)enemy;
    opossum->unschedule(CC_SCHEDULE_SELECTOR(Opossum::horizontallyAccelerate));
    opossum->unschedule(CC_SCHEDULE_SELECTOR(Opossum::autoChangeDirection));

    opossum->enemySprite->stopAction(opossum->currentAnimateAction);
}

void
Opossum::Alert::defaultChangeState(Enemy* enemy)
{
}

Opossum::Patrol*
Opossum::Patrol::getInstance()
{
    static Patrol instance;
    return &instance;
}

void
Opossum::Patrol::Enter(Enemy* enemy)
{
    auto opossum = (Opossum*)enemy;
    opossum->currentAnimateAction = RepeatForever::create(Animate::create(opossum->runAnimation));
    opossum->enemySprite->runAction(opossum->currentAnimateAction);

    enemy->schedule(
        [enemy](float dt) {
            int weight = random(0, 100);
            if (weight >= 60) {
                if (enemy->isScheduled(CC_SCHEDULE_SELECTOR(Opossum::horizontallyAccelerate))) {
                    enemy->unschedule(CC_SCHEDULE_SELECTOR(Opossum::horizontallyAccelerate));
                }
            } else {
                if (weight >= 30) {
                    enemy->enemySprite->setScaleX(-1);
                    enemy->enemyDirection = Direction::RIGHT;
                } else {
                    enemy->enemySprite->setScaleX(1);
                    enemy->enemyDirection = Direction::LEFT;
                }
                if (!(enemy->isScheduled(CC_SCHEDULE_SELECTOR(Opossum::horizontallyAccelerate)))) {
                    enemy->schedule(CC_SCHEDULE_SELECTOR(Opossum::horizontallyAccelerate));
                }
            }
        },
        1.0, "OpossumPatrol");
}

void
Opossum::Patrol::Exit(Enemy* enemy)
{
    auto opossum = (Opossum*)enemy;
    enemy->unschedule("OpossumPatrol");
    opossum->enemySprite->stopAction(opossum->currentAnimateAction);
}

void
Opossum::Patrol::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Opossum::Alert::getInstance());
}
