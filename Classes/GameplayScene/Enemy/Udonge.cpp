#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Udonge.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"
#include "GameplayScene/Emitters/Emitter.h"

#include "AudioController.h"

bool
Udonge::init(const std::string& tag)
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
    stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());

    return true;
}

void
Udonge::horizontallyAccelerate(float dt)
{
    timeAccumulation += dt;
    if (timeAccumulation >= 0.1) {
        if (this->enemyDirection == Direction::LEFT) {
            this->getPhysicsBody()->applyImpulse(Vec2(-35.0f, 0.0f));
        } else {
            this->getPhysicsBody()->applyImpulse(Vec2(35.0f, 0.0f));
        }
        timeAccumulation = 0;
    }
}

void
Udonge::jump()
{
    body->applyImpulse(Vec2(0.0f, 1500.0f));
}

void
Udonge::dash()
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
Udonge::decreaseHp(int damage)
{
    damageAccumulation += damage;

    this->CurrentHp = this->CurrentHp - damage;
    if (this->CurrentHp < 0) {
        stateMachine->changeState(Udonge::Knockdown::getInstance());

        EventCustom event("kill_boss");
        _eventDispatcher->dispatchEvent(&event);
    }
    if (damageAccumulation >= 400) {
        this->stateMachine->changeState(Udonge::Hit::getInstance());
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
Udonge::autoChangeDirection(float dt)
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

Udonge::StandAndChooseAction*
Udonge::StandAndChooseAction::getInstance()
{
    static StandAndChooseAction instance;
    return &instance;
}

void
Udonge::StandAndChooseAction::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;

    if (udonge->CurrentHp < 0) {
        udonge->stateMachine->changeState(Udonge::Knockdown::getInstance());
        return;
    }

    udonge->currentAnimateAction = Animate::create(udonge->standAnimation);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);
    udonge->schedule(CC_SCHEDULE_SELECTOR(Udonge::autoChangeDirection), 0.50);

    udonge->schedule(
        [udonge](float dt) {
            int weight = random(0, 100);
            if (udonge->CurrentHp <= udonge->BaseHp * 0.4 && weight >= 70) {
                udonge->stateMachine->changeState(Udonge::UseSpellCard::getInstance());
            } else if (weight >= 60) {
                udonge->stateMachine->changeState(Udonge::Shoot::getInstance());
            } else if (weight >= 50) {
                udonge->stateMachine->changeState(Udonge::Walk::getInstance());
            } else if (weight >= 20) {
                udonge->stateMachine->changeState(Udonge::Jump::getInstance());
            } else if (weight >= 0) {
                udonge->stateMachine->changeState(Udonge::Dash::getInstance());
            }
        },
        0.50, "StandAndChooseAction");
}

void
Udonge::StandAndChooseAction::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
    udonge->unschedule("StandAndChooseAction");
    udonge->unschedule(CC_SCHEDULE_SELECTOR(Udonge::autoChangeDirection));
}

void
Udonge::StandAndChooseAction::defaultChangeState(Enemy*)
{
}

Udonge::Walk*
Udonge::Walk::getInstance()
{
    static Walk instance;
    return &instance;
}

void
Udonge::Walk::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->currentAnimateAction = Animate::create(udonge->moveAnimation);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);

    auto target = *(udonge->curTarget);
    auto distance = target->getPosition().x - enemy->getPosition().x;
    distance = std::abs(distance);
    double walkTime = (double)distance / 200; // 200没什么特殊含义

    udonge->schedule(CC_SCHEDULE_SELECTOR(Udonge::horizontallyAccelerate));
    udonge->schedule(CC_SCHEDULE_SELECTOR(Udonge::autoChangeDirection), 0.30);

    udonge->scheduleOnce(
        [udonge](float dt) {
            udonge->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
        },
        walkTime, "walk");
}

void
Udonge::Walk::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
    udonge->unschedule(CC_SCHEDULE_SELECTOR(Udonge::horizontallyAccelerate));
    udonge->unschedule(CC_SCHEDULE_SELECTOR(Udonge::autoChangeDirection));

    auto currentVelocity = udonge->body->getVelocity();
    udonge->body->setVelocity(Vec2(currentVelocity.x / 3.0f, currentVelocity.y));
}

void
Udonge::Walk::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
}

Udonge::Jump*
Udonge::Jump::getInstance()
{
    static Jump instance;
    return &instance;
}

void
Udonge::Jump::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->jump();
    udonge->currentAnimateAction = Sequence::create(Animate::create(udonge->preJumpAnimation),
                                                    Animate::create(udonge->jumpAnimation), NULL);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);

    udonge->enemySprite->schedule(
        [udonge](float dt) {
            Vec2 velocity = udonge->body->getVelocity();
            if (velocity.y < 10) {
                int weight = random(0, 100);
                if (weight >= 50) {
                    udonge->stateMachine->changeState(Udonge::ShootFromAir::getInstance());
                } else {
                    udonge->stateMachine->changeState(Udonge::Fall::getInstance());
                }
            }
        },
        0.1, "JumpUpdate");
}

void
Udonge::Jump::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
    udonge->enemySprite->unschedule("JumpUpdate");
}

void
Udonge::Jump::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->RevertToPreviousState();
}

Udonge::Fall*
Udonge::Fall::getInstance()
{
    static Fall instance;
    return &instance;
}

void
Udonge::Fall::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->currentAnimateAction = Sequence::create(Animate::create(udonge->preFallAnimation),
                                                    Animate::create(udonge->fallAnimation), NULL);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);

    udonge->enemySprite->schedule(
        [udonge](float dt) {
            Vec2 velocity = udonge->body->getVelocity();
            if (-10 < velocity.y) {
                udonge->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
            }
        },
        0.1, "FallUpdate");
}

void
Udonge::Fall::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
    udonge->enemySprite->unschedule("FallUpdate");
}

void
Udonge::Fall::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
}

Udonge::Dash*
Udonge::Dash::getInstance()
{
    static Dash instance;
    return &instance;
}

void
Udonge::Dash::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->dash();
    auto animate = Animate::create(udonge->dashAnimation);
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Udonge::Dash::defaultChangeState, this, udonge));

    udonge->currentAnimateAction = Sequence::create(animate, actionDone, NULL);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);
}

void
Udonge::Dash::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
}

void
Udonge::Dash::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->RevertToPreviousState();
}

Udonge::Shoot*
Udonge::Shoot::getInstance()
{
    static Shoot instance;
    return &instance;
}

void
Udonge::Shoot::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Udonge::Shoot::defaultChangeState, this, udonge));

    int weight = random(0, 100);
    if (weight >= 50) {
        auto animateAa1 =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackAa_1"));
        auto animateAa2 =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackAa_2"));
        std::function<void(Ref*)> shoot = [udonge](Ref*) {
            StyleConfig sc1;
            sc1.style = StyleType::PARALLEL;
            sc1.frequency = 0.1f;
            sc1.bulletDuration = 2.0f;
            sc1.number = 1;
            sc1.countThenChangePos = 1;
            sc1.interval = 2.0f;
            sc1.cycleTimes = 2;
            sc1.totalDuration = 1.0;
            sc1.bc.name = "b1_7_2.png";
            sc1.bc.length = 5;
            sc1.bc.width = 5;
            sc1.bc.harm = 10;
            sc1.bc._categoryBitmask = 0;
            sc1.bc._collisionBitmask = 0;
            sc1.bc._contactTestBitmask = 0;
            udonge->emitter->playStyle(sc1);
        };
        udonge->currentAnimateAction =
            Sequence::create(animateAa1, CallFuncN::create(shoot), animateAa2, actionDone, NULL);
    } else {
        auto animateBa1 =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackBa_1"));
        auto animateBa2 =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackBa_2"));
        std::function<void(Ref*)> shoot = [udonge](Ref*) {
            StyleConfig sc1;
            sc1.style = StyleType::PARALLEL;
            sc1.frequency = 0.1f;
            sc1.bulletDuration = 2.0f;
            sc1.number = 1;
            sc1.countThenChangePos = 1;
            sc1.interval = 2.0f;
            sc1.cycleTimes = 1;
            sc1.totalDuration = 1.0;
            sc1.bc.name = "b1_7_2.png";
            sc1.bc.length = 5;
            sc1.bc.width = 5;
            sc1.bc.harm = 10;
            sc1.bc._categoryBitmask = 0;
            sc1.bc._collisionBitmask = 0;
            sc1.bc._contactTestBitmask = 0;
            udonge->emitter->playStyle(sc1);
        };
        udonge->currentAnimateAction =
            Sequence::create(animateBa1, CallFuncN::create(shoot), animateBa2, actionDone, NULL);
    }

    udonge->enemySprite->runAction(udonge->currentAnimateAction);
}

void
Udonge::Shoot::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
}

void
Udonge::Shoot::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
}

Udonge::ShootFromAir*
Udonge::ShootFromAir::getInstance()
{
    static ShootFromAir instance;
    return &instance;
}

void
Udonge::ShootFromAir::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    //不会受重力影响
    udonge->body->setGravityEnable(false);

    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Udonge::ShootFromAir::defaultChangeState, this, udonge));

    int weight = random(0, 100);
    if (weight >= 50) {
        auto animateAd =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackAd_1"));
        std::function<void(Ref*)> shoot = [udonge](Ref*) {
            StyleConfig sc;
            sc.style = StyleType::ODDEVEN;
            sc.frequency = 0.0f;
            sc.bulletDuration = 5.0;
            sc.number = 7;
            sc.countThenChangePos = 1;
            sc.totalDuration = 1.0;
            sc.cycleTimes = 1;
            sc.bc.name = "b1_7_2.png";
            sc.bc.length = 10;
            sc.bc.width = 20;
            sc.bc.harm = 10;
            sc.bc._categoryBitmask = 0;
            sc.bc._collisionBitmask = 0;
            sc.bc._contactTestBitmask = 0;
            udonge->emitter->playStyle(sc);
        };
        udonge->currentAnimateAction =
            Sequence::create(animateAd, CallFuncN::create(shoot), actionDone, NULL);
    } else {
        auto animateBd_1 =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackBd_1"));
        auto animateBd_2 =
            Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackBd_2"));
        std::function<void(Ref*)> shoot = [udonge](Ref*) {
            StyleConfig sc;
            sc.style = StyleType::ODDEVEN;
            sc.frequency = 0.0f;
            sc.bulletDuration = 5.0;
            sc.number = 7;
            sc.countThenChangePos = 1;
            sc.totalDuration = 1.0;
            sc.cycleTimes = 1;
            sc.bc.name = "b1_7_2.png";
            sc.bc.length = 10;
            sc.bc.width = 20;
            sc.bc.harm = 10;
            sc.bc._categoryBitmask = 0;
            sc.bc._collisionBitmask = 0;
            sc.bc._contactTestBitmask = 0;
            udonge->emitter->playStyle(sc);
        };
        udonge->currentAnimateAction =
            Sequence::create(animateBd_1, CallFuncN::create(shoot), animateBd_2, actionDone, NULL);
    }
    udonge->enemySprite->runAction(udonge->currentAnimateAction);
}

void
Udonge::ShootFromAir::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);

    //恢复重力
    udonge->body->setGravityEnable(true);
}

void
Udonge::ShootFromAir::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::Fall::getInstance());
}

Udonge::UseSpellCard*
Udonge::UseSpellCard::getInstance()
{
    static UseSpellCard instance;
    return &instance;
}

void
Udonge::UseSpellCard::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;

    AudioController::getInstance()->playEffect("se/use_spell_card.wav");

    auto portrait = Sprite::create("character/Udonge/useSpellCard.png");
    udonge->addChild(portrait);
    portrait->setOpacity(50);
    portrait->runAction(
        Sequence::create(MoveBy::create(0.8, Vec2(0, 150)), FadeOut::create(0.3f), NULL));
    portrait->scheduleOnce([portrait](float dt) { portrait->removeFromParent(); }, 1.5, "remove");

    auto useSpellCardAnimate =
        Animate::create(AnimationCache::getInstance()->getAnimation("udongeUseSpellCard"));

    auto animateAa1 =
        Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackAa_1"));
    auto animateAa2 =
        Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackAa_2"));

    auto animateBa1 =
        Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackBa_1"));
    auto animateBa2 =
        Animate::create(AnimationCache::getInstance()->getAnimation("udongeAttackBa_2"));

    std::function<void(Ref*)> shootA = [udonge](Ref*) {
        StyleConfig sc;
        sc.style = StyleType::ODDEVEN;
        sc.frequency = 0.1f;
        sc.bulletDuration = 3.5;
        sc.number = 12;
        sc.countThenChangePos = 1;
        sc.totalDuration = 1.0;
        sc.cycleTimes = 2;
        sc.bc.name = "b1_7_2.png";
        sc.bc.length = 5;
        sc.bc.width = 5;
        sc.bc.harm = 10;
        sc.bc._categoryBitmask = 0;
        sc.bc._collisionBitmask = 0;
        sc.bc._contactTestBitmask = 0;
        udonge->emitter->playStyle(sc);
    };

    std::function<void(Ref*)> shootB = [udonge](Ref*) {
        StyleConfig sc;
        sc.style = StyleType::ODDEVEN;
        sc.frequency = 0.05f;
        sc.bulletDuration = 2.5;
        sc.number = 2;
        sc.countThenChangePos = 1;
        sc.totalDuration = 5.0;
        sc.cycleTimes = 20;
        sc.bc.name = "b1_7_2.png";
        sc.bc.length = 5;
        sc.bc.width = 5;
        sc.bc.harm = 10;
        sc.bc._categoryBitmask = 0;
        sc.bc._collisionBitmask = 0;
        sc.bc._contactTestBitmask = 0;
        udonge->emitter->playStyle(sc);
    };

    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Udonge::UseSpellCard::defaultChangeState, this, udonge));
    udonge->currentAnimateAction = Sequence::create(
        useSpellCardAnimate, animateAa1, CallFuncN::create(shootA), animateAa2, animateBa1,
        CallFuncN::create(shootB), DelayTime::create(1.0f), animateBa2, actionDone, NULL);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);
}

void
Udonge::UseSpellCard::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
}

void
Udonge::UseSpellCard::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
}

Udonge::Hit*
Udonge::Hit::getInstance()
{
    static Hit instance;
    return &instance;
}

void
Udonge::Hit::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(Udonge::Hit::defaultChangeState, this, udonge));
    udonge->currentAnimateAction =
        Sequence::create(Animate::create(udonge->hitAnimation), actionDone, NULL);
    udonge->enemySprite->runAction(udonge->currentAnimateAction);
}

void
Udonge::Hit::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
}

void
Udonge::Hit::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::StandAndChooseAction::getInstance());
}

Udonge::Knockdown*
Udonge::Knockdown::getInstance()
{
    static Knockdown instance;
    return &instance;
}

void
Udonge::Knockdown::Enter(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);

    auto animate = Animate::create(udonge->downAnimation);
    udonge->enemySprite->runAction(animate);

    //使人物能倒在地上
    udonge->body->getFirstShape()->setCategoryBitmask(enemyCategory);
    udonge->body->getFirstShape()->setCollisionBitmask(groundCategory);
    udonge->body->getFirstShape()->setContactTestBitmask(groundCategory);
    udonge->body->setPositionOffset(Vec2(0, 40));
}

void
Udonge::Knockdown::Exit(Enemy* enemy)
{
    auto udonge = (Udonge*)enemy;
    udonge->enemySprite->stopAction(udonge->currentAnimateAction);
}

void
Udonge::Knockdown::defaultChangeState(Enemy* enemy)
{
    enemy->stateMachine->changeState(Udonge::Knockdown::getInstance());
}
