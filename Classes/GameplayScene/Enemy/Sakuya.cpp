#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Sakuya.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"
#include "GameplayScene/Emitters/Emitter.h"

#include "AudioController.h"

bool
Sakuya::init(const std::string& tag)
{
    if (!Node::init())
        return false;

    this->enemyTag = tag;
    this->setTag(enemyCategoryTag);
    this->setName(tag);

    EnemyData _enemyData = GameData::getInstance()->getEnemyByTag(tag);
    enemySprite = Sprite::create(_enemyData.defaultTexture);
    this->addChild(enemySprite);

    //设置头像
    this->face = _enemyData.face;

    //设置属性值
    this->CurrentHp = _enemyData.healthPoint;
    this->BaseHp = CurrentHp;

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
    body->setPositionOffset(Vec2(0, -15));
    this->setPhysicsBody(body);

    //设置速度上限
    body->setVelocityLimit(400);

    //获得动画缓存
    standAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.standAnimationKey);
    moveAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.runAnimationKey);
    preJumpAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.preJumpAnimationKey);
    jumpAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.jumpAnimationKey);
    preFallAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.preFallAnimationKey);
    fallAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.fallAnimationKey);
    dashAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.dashAnimationKey);
    hitAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.hitAnimationKey);
    downAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.downAnimationKey);

    standAnimation->setLoops(-1);
    moveAnimation->setLoops(-1);
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

    this->CurrentHp = this->CurrentHp - damage;
    if (this->CurrentHp < 0) {
        stateMachine->changeState(Sakuya::Knockdown::getInstance());

        EventCustom event("kill_boss");
        _eventDispatcher->dispatchEvent(&event);
    }
    if (damageAccumulation >= 400) {
        this->stateMachine->changeState(Sakuya::Hit::getInstance());
        damageAccumulation = 0;
    }

    Hp_Mp_Change hpChange;
    hpChange.tag = this->enemyTag;
    hpChange.target = this;
    hpChange.value = -(std::abs(damage)); //此处取反，因为伤害值总是正数
    EventCustom event("hp_change");
    event.setUserData((void*)&hpChange);
    _eventDispatcher->dispatchEvent(&event);
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
            if (sakuya->CurrentHp <= sakuya->BaseHp * 0.5 && weight >= 60) {
                sakuya->stateMachine->changeState(Sakuya::UseSpellCard::getInstance());
            } else if (weight >= 60) {
                sakuya->stateMachine->changeState(Sakuya::ShootB::getInstance());
            } else if (weight >= 40) {
                sakuya->stateMachine->changeState(Sakuya::ShootA::getInstance());
            } else if (weight >= 30) {
                sakuya->stateMachine->changeState(Sakuya::Walk::getInstance());
            } else if (weight >= 15) {
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
    sakuya->unschedule("StandAndChooseAction");
    sakuya->unschedule(CC_SCHEDULE_SELECTOR(Sakuya::autoChangeDirection));
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
    sakuya->currentAnimateAction = Animate::create(sakuya->moveAnimation);
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
        sc1.number = 5;
        sc1.countThenChangePos = 1;
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
}

void
Sakuya::ShootA::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
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
    auto animate1 = Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackB_1"));
    auto animate2 = Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackB_2"));
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Sakuya::ShootB::defaultChangeState, this, sakuya));
    std::function<void(Ref*)> shoot = [sakuya](Ref*) {
        StyleConfig sc;

        sc.style = StyleType::ODDEVEN;
        sc.frequency = 0.0f;
        sc.bulletDuration = 5.0;
        sc.number = 7;
        sc.countThenChangePos = 1;
        sc.totalDuration = 1.0;
        sc.cycleTimes = 1;
        sc.bc.name = "b2_2_1.png";
        sc.bc.length = 10;
        sc.bc.width = 20;
        sc.bc.harm = 10;
        sc.bc._categoryBitmask = 0;
        sc.bc._collisionBitmask = 0;
        sc.bc._contactTestBitmask = 0;

        sakuya->emitter->playStyle(sc);
    };
    sakuya->currentAnimateAction =
        Sequence::create(animate1, CallFuncN::create(shoot), animate2, actionDone, NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);
}

void
Sakuya::ShootB::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
}

void
Sakuya::ShootB::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
}

Sakuya::UseSpellCard*
Sakuya::UseSpellCard::getInstance()
{
    static UseSpellCard instance;
    return &instance;
}

void
Sakuya::UseSpellCard::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;

    AudioController::getInstance()->playEffect("se/use_spell_card.wav");

    auto portrait = Sprite::create("character/Sakuya/useSpellCard.png");
    sakuya->addChild(portrait);
    portrait->setOpacity(50);
    portrait->runAction(
        Sequence::create(MoveBy::create(0.8, Vec2(0, 150)), FadeOut::create(0.3f), NULL));
    portrait->scheduleOnce([portrait](float dt) { portrait->removeFromParent(); }, 1.5, "remove");

    auto useSpellCardAnimate =
        Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaUseSpellCard"));
    auto animateA1 =
        Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackA_1"));
    auto animateA2 =
        Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackA_2"));
    auto animateB1 =
        Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackB_1"));
    auto animateB2 =
        Animate::create(AnimationCache::getInstance()->getAnimation("sakuyaAttackB_2"));

    std::function<void(Ref*)> shoot = [sakuya](Ref*) {
        StyleConfig sc;
        sc.style = StyleType::ODDEVEN;
        sc.frequency = 0.1f;
        sc.bulletDuration = 4.0;
        sc.number = 15;
        sc.countThenChangePos = 5;
        sc.totalDuration = 1.0;
        sc.cycleTimes = 5;
        sc.bc.name = "b2_2_1.png";
        sc.bc.length = 10;
        sc.bc.width = 20;
        sc.bc.harm = 10;
        sc.bc._categoryBitmask = 0;
        sc.bc._collisionBitmask = 0;
        sc.bc._contactTestBitmask = 0;
        sakuya->emitter->playStyle(sc);
    };

    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Sakuya::UseSpellCard::defaultChangeState, this, sakuya));
    sakuya->currentAnimateAction =
        Sequence::create(useSpellCardAnimate, animateA1, CallFuncN::create(shoot), animateB1,
                         animateA1, CallFuncN::create(shoot), animateA2, actionDone, NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);
}

void
Sakuya::UseSpellCard::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
}

void
Sakuya::UseSpellCard::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Sakuya::StandAndChooseAction::getInstance());
}

Sakuya::Hit*
Sakuya::Hit::getInstance()
{
    static Hit instance;
    return &instance;
}

void
Sakuya::Hit::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Sakuya::Hit::defaultChangeState, this, sakuya));
    sakuya->currentAnimateAction =
        Sequence::create(Animate::create(sakuya->hitAnimation), actionDone, NULL);
    sakuya->enemySprite->runAction(sakuya->currentAnimateAction);
}

void
Sakuya::Hit::Exit(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    sakuya->enemySprite->stopAction(sakuya->currentAnimateAction);
}

void
Sakuya::Hit::defaultChangeState(Enemy* enemy)
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
Sakuya::Knockdown::Enter(Enemy* enemy)
{
    auto sakuya = (Sakuya*)enemy;
    auto animate = Animate::create(sakuya->downAnimation);
    sakuya->enemySprite->runAction(animate);

    //使人物能倒在地上
    sakuya->body->getFirstShape()->setCategoryBitmask(enemyCategory);
    sakuya->body->getFirstShape()->setCollisionBitmask(groundCategory);
    sakuya->body->getFirstShape()->setContactTestBitmask(groundCategory);
    sakuya->body->setPositionOffset(Vec2(0, 40));
}

void
Sakuya::Knockdown::Exit(Enemy* enemy)
{
}

void
Sakuya::Knockdown::defaultChangeState(Enemy* enemy)
{
}
