#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Sakuya.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"
#include "GameplayScene/Emitters/Emitter.h"

bool
Sakuya::init(std::string tag)
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
    body = PhysicsBody::createBox(Size(55, 90));
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
    body->setPositionOffset(Vec2(0, -15));

    //设置速度上限
    body->setVelocityLimit(400);

    //获得动画缓存
    standAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.standAnimationKey);
    runAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.runAnimationKey);
    preJumpAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.preJumpAnimationKey);
    jumpAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.jumpAnimationKey);
    preFallAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.preFallAnimationKey);
    fallAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.fallAnimationKey);
    dashAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.dashAnimationKey);

    standAnimation->setLoops(-1);
    runAnimation->setLoops(-1);
    jumpAnimation->setLoops(-1);
    fallAnimation->setLoops(-1);

    //状态机
    stateMachine = new StateMachine<Enemy>(this);
    stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());

    return true;
}

void
Sakuya::horizontallyAccelerate(float dt)
{
    timeAccumulation += dt;
    if (timeAccumulation >= 0.1) {
        if (this->enemyDirection == Direction::LEFT) {
            this->getPhysicsBody()->applyImpulse(Vec2(-40.0f, 0.0f));
        } else {
            this->getPhysicsBody()->applyImpulse(Vec2(40.0f, 0.0f));
        }
        timeAccumulation = 0;
    }
}

void
Sakuya::jump()
{
    body->applyImpulse(Vec2(0.0f, 900.0f));
}

void
Sakuya::dash()
{
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0));

    if (this->enemyDirection == Direction::RIGHT) {
        Vec2 impluse = Vec2(400, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(-400, 0.0f);
        body->applyImpulse(impluse);
    }
}

void
Sakuya::decreaseHp(int damage)
{
    damageAccumulation += damage;

    this->hp = this->hp - damage;
    if (this->hp < 0) {
        this->removeFromParentAndCleanup(true);
    }
}

void
Sakuya::autoChangeDirection(float dt)
{
    Point enemyPos = this->getPosition();
    Vec2 playerPos = (*curTarget)->getPosition();
    if (enemyPos.x > playerPos.x) {
        this->enemyDirection = Direction::LEFT;
        enemySprite->setScaleX(-1);
    } else {
        this->enemyDirection = Direction::RIGHT;
        enemySprite->setScaleX(1);
    }
}

Sakuya::StandAndChooseAction*
Sakuya::StandAndChooseAction::getInstance()
{
    static StandAndChooseAction instance;
    return &instance;
}

void
Sakuya::StandAndChooseAction::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->currentAnimateAction = Animate::create(sakuya->standAnimation);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);
    sakuya->schedule(CC_SCHEDULE_SELECTOR(Sakuya::autoChangeDirection), 0.50);

    sakuya->schedule(
        [sakuya](float dt) {
            int weight = random(0, 100);

            if (weight >= 60) {
                sakuya->stateMachine->changeState(Sakuya::ShootA::getInstance());
            } else if (weight >= 50) {
                sakuya->stateMachine->changeState(Sakuya::Walk::getInstance());
            } else if (weight >= 25) {
                sakuya->stateMachine->changeState(Sakuya::Jump::getInstance());
            } else if (weight >= 0) {
                sakuya->stateMachine->changeState(Sakuya::Dash::getInstance());
            }

        },
        0.50, "StandAndChooseAction");
}

void
Sakuya::StandAndChooseAction::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
    sakuya->unschedule(CC_SCHEDULE_SELECTOR(Sakuya::autoChangeDirection));
    sakuya->unschedule("StandAndChooseAction");
}

void
Sakuya::StandAndChooseAction::defaultChangeState(Enemy*)
{
}

Sakuya::Walk*
Sakuya::Walk::getInstance()
{
    static Walk instance;
    return &instance;
}

void
Sakuya::Walk::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->currentAnimateAction = Animate::create(sakuya->runAnimation);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);

    auto target = *(sakuya->curTarget);
    auto distance = target->getPosition().x - enemy->getPosition().x;
    distance = std::abs(distance);
    double walkTime = (double)distance / 200; // 200没什么特殊含义

    sakuya->schedule(CC_SCHEDULE_SELECTOR(Sakuya::horizontallyAccelerate));
    sakuya->schedule(CC_SCHEDULE_SELECTOR(Sakuya::autoChangeDirection), 0.50);

    sakuya->scheduleOnce(
        [enemy](float dt) {
            enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
        },
        walkTime, "walk");
}

void
Sakuya::Walk::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
    sakuya->unschedule(CC_SCHEDULE_SELECTOR(Sakuya::horizontallyAccelerate));
    sakuya->unschedule(CC_SCHEDULE_SELECTOR(Sakuya::autoChangeDirection));

    auto currentVelocity = sakuya->body->getVelocity();
    sakuya->body->setVelocity(Vec2(currentVelocity.x / 3.0f, currentVelocity.y));
}

void
Sakuya::Walk::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
}

Sakuya::Jump*
Sakuya::Jump::getInstance()
{
    static Jump instance;
    return &instance;
}

void
Sakuya::Jump::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->jump();
    sakuya->currentAnimateAction = Sequence::create(Animate::create(sakuya->preJumpAnimation),
                                                    Animate::create(sakuya->jumpAnimation), NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);

    sakuya->enemySprite->schedule(
        [sakuya](float dt) {
            Vec2 velocity = sakuya->body->getVelocity();
            if (velocity.y < 10) {
                sakuya->stateMachine->changeState(Sakuya::Fall::getInstance());
            }
        },
        0.1, "JumpUpdate");
}

void
Sakuya::Jump::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
    sakuya->enemySprite->unschedule("JumpUpdate");
}

void
Sakuya::Jump::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->RevertToPreviousState();
}

Sakuya::Fall*
Sakuya::Fall::getInstance()
{
    static Fall instance;
    return &instance;
}

void
Sakuya::Fall::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->currentAnimateAction = Sequence::create(Animate::create(sakuya->preFallAnimation),
                                                    Animate::create(sakuya->fallAnimation), NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);

    sakuya->enemySprite->schedule(
        [sakuya](float dt) {
            Vec2 velocity = sakuya->body->getVelocity();
            if (-10 < velocity.y) {
                sakuya->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
            }
        },
        0.1, "FallUpdate");
}

void
Sakuya::Fall::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
    sakuya->enemySprite->unschedule("FallUpdate");
}

void
Sakuya::Fall::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
}

Sakuya::Dash*
Sakuya::Dash::getInstance()
{
    static Dash instance;
    return &instance;
}

void
Sakuya::Dash::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->dash();
    auto animate = Animate::create(sakuya->dashAnimation);
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Sakuya::Dash::defaultChangeState, this, sakuya));

    sakuya->currentAnimateAction = Sequence::create(animate, actionDone, NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);
}

void
Sakuya::Dash::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
}

void
Sakuya::Dash::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->RevertToPreviousState();
}

Sakuya::ShootA*
Sakuya::ShootA::getInstance()
{
    static ShootA instance;
    return &instance;
}

void
Sakuya::ShootA::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    auto animate1 = Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackA_1"));
    auto animate2 = Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackA_2"));
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Sakuya::ShootA::defaultChangeState, this, sakuya));
    std::function<void(Ref*)> shoot = [sakuya](Ref*) {
        StyleConfig sc1;
        sc1.style = StyleType::PARALLEL;
        sc1.frequency = 0.0f;
        sc1.bulletDuration = 4.0f;
        sc1.number = 4;
        sc1.count = 1;
        sc1.interval = 2.0f;
        sc1.cycleTimes = 1;
        sc1.totalDuration = 1.0;
        sc1.bc.name = "b2_2_1.png";
        sc1.bc.length = 10;
        sc1.bc.width = 10;
        sc1.bc.harm = 10;
        sc1.bc._categoryBitmask = 0;
        sc1.bc._collisionBitmask = 0;
        sc1.bc._contactTestBitmask = 0;
        sakuya->emitter->playStyle(sc1);
    };
    sakuya->currentAnimateAction =
        Sequence::create(animate1, CallFuncN::create(shoot), animate2, actionDone, NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);
}

void
Sakuya::ShootA::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);

    sakuya->emitter->stopAllStyle();
}

void
Sakuya::ShootA::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->RevertToPreviousState();
}

Sakuya::ShootB*
Sakuya::ShootB::getInstance()
{
    static ShootB instance;
    return &instance;
}

void
Sakuya::ShootB::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
}

void
Sakuya::ShootB::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
}

void
Sakuya::ShootB::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->RevertToPreviousState();
}

Sakuya::UseSpellCard*
Sakuya::UseSpellCard::getInstance()
{
    static UseSpellCard instance;
    return &instance;
}

void
Sakuya::UseSpellCard::Enter(Enemy*)
{
}

void
Sakuya::UseSpellCard::Exit(Enemy*)
{
}

void
Sakuya::UseSpellCard::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
}

Sakuya::Knockdown*
Sakuya::Knockdown::getInstance()
{
    static Knockdown instance;
    return &instance;
}

void
Sakuya::Knockdown::Enter(Enemy*)
{
}

void
Sakuya::Knockdown::Exit(Enemy*)
{
}

void
Sakuya::Knockdown::defaultChangeState(Enemy* enemy)
{
}
