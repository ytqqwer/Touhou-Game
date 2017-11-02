#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Stump.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"

bool
Stump::init(const std::string& tag)
{
    if (!Node::init())
        return false;

    this->enemyTag = tag;
    this->setTag(enemyCategoryTag);
    this->setName(tag);

    EnemyData _enemyData = GameData::getInstance()->getEnemyByTag(tag);
    enemySprite = Sprite::create();
    enemySprite->setSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_stand.png"));
    this->addChild(enemySprite);

    //设置属性值
    this->CurrentHp = _enemyData.healthPoint;
    this->BaseHp = CurrentHp;

    //设置刚体
    body = PhysicsBody::createBox(enemySprite->getContentSize());
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
    body->setPositionOffset(Vec2(0, 10));

    //索敌检测框
    auto rect = PhysicsShapeBox::create(Size(200, 200));
    rect->setTag(lockCategoryTag);
    rect->setCategoryBitmask(lockCategory);
    rect->setCollisionBitmask(0);
    rect->setContactTestBitmask(playerCategory);
    body->addShape(rect);

    //设置速度上限
    body->setVelocityLimit(200);

    //获得动画缓存
    standAnimation = AnimationCache::getInstance()->getAnimation("stumpStand");
    moveAnimation = AnimationCache::getInstance()->getAnimation("stumpMove");
    hitAnimation = AnimationCache::getInstance()->getAnimation("stumpHit");
    downAnimation = AnimationCache::getInstance()->getAnimation("stumpDown");
    moveAnimation->setLoops(-1);

    //状态机
    stateMachine = new StateMachine<Enemy>(this);
    stateMachine->changeState(Stump::Stand::getInstance());

    return true;
}

void
Stump::horizontallyAccelerate(float dt)
{
    timeAccumulation += dt;
    if (timeAccumulation >= 0.1) {
        if (this->enemyDirection == Direction::LEFT) {
            this->getPhysicsBody()->applyImpulse(Vec2(-24.0f, 0.0f));
        } else {
            this->getPhysicsBody()->applyImpulse(Vec2(24.0f, 0.0f));
        }
        timeAccumulation = 0;
    }
}

void
Stump::decreaseHp(int damage)
{
    damageAccumulation += damage;
    this->CurrentHp = this->CurrentHp - damage;

    if (damageAccumulation >= 100) {
        this->stateMachine->changeState(Stump::Hit::getInstance());
        damageAccumulation = 0;
    }
    if (this->CurrentHp < 0) {
        this->stateMachine->changeState(Stump::Down::getInstance());
    }
}

Stump::Stand*
Stump::Stand::getInstance()
{
    static Stand instance;
    return &instance;
}

void
Stump::Stand::Enter(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    stump->currentAnimateAction = Animate::create(stump->standAnimation);
    stump->enemySprite->runAction(stump->currentAnimateAction);
}

void
Stump::Stand::Exit(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    stump->enemySprite->stopAction(stump->currentAnimateAction);
}

void
Stump::Stand::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Stump::Alert::getInstance());
}

Stump::Alert*
Stump::Alert::getInstance()
{
    static Alert instance;
    return &instance;
}

void
Stump::Alert::Enter(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    stump->currentAnimateAction = Animate::create(stump->moveAnimation);
    stump->enemySprite->runAction(stump->currentAnimateAction);

    stump->schedule(CC_SCHEDULE_SELECTOR(Stump::horizontallyAccelerate));
    stump->schedule(CC_SCHEDULE_SELECTOR(Stump::autoChangeDirection), 0.50);
}

void
Stump::Alert::Exit(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    stump->unschedule(CC_SCHEDULE_SELECTOR(Stump::horizontallyAccelerate));
    stump->unschedule(CC_SCHEDULE_SELECTOR(Stump::autoChangeDirection));

    stump->enemySprite->stopAction(stump->currentAnimateAction);
}

void
Stump::Alert::defaultChangeState(Enemy* enemy)
{
}

Stump::Hit*
Stump::Hit::getInstance()
{
    static Hit instance;
    return &instance;
}

void
Stump::Hit::Enter(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    auto actionDone = CallFuncN::create(CC_CALLBACK_0(Stump::Hit::defaultChangeState, this, stump));
    stump->currentAnimateAction =
        Sequence::create(Animate::create(stump->hitAnimation), actionDone, NULL);
    stump->enemySprite->runAction(stump->currentAnimateAction);
}

void
Stump::Hit::Exit(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    stump->enemySprite->stopAction(stump->currentAnimateAction);
}

void
Stump::Hit::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Stump::Alert::getInstance());
}

Stump::Down*
Stump::Down::getInstance()
{
    static Down instance;
    return &instance;
}

void
Stump::Down::Enter(Enemy* enemy)
{
    auto stump = (Stump*)enemy;
    stump->currentAnimateAction = Animate::create(stump->downAnimation);
    stump->enemySprite->runAction(stump->currentAnimateAction);

    stump->body->getFirstShape()->setCategoryBitmask(enemyCategory);
    stump->body->getFirstShape()->setCollisionBitmask(groundCategory);
    stump->body->getFirstShape()->setContactTestBitmask(groundCategory);

    stump->scheduleOnce([stump](float dt) { stump->removeFromParentAndCleanup(true); }, 2,
                        "remove");
}

void
Stump::Down::Exit(Enemy*)
{
}

void
Stump::Down::defaultChangeState(Enemy*)
{
}
