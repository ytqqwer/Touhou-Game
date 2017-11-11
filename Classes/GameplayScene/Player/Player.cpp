#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Player.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameplayScene/Player/Alice.h"
#include "GameplayScene/Player/Marisa.h"
#include "GameplayScene/Player/Reimu.h"

#include "GameplayScene/EventFilterManager.h"

#include "GameplayScene/Shaders/BlendAction.h"

#include "GameData/GameData.h"

Player*
Player::create(const std::string& tag)
{
    Player* pRet;
    if (tag == "Reimu") {
        pRet = new (std::nothrow) Reimu();
    } else if (tag == "Marisa") {
        pRet = new (std::nothrow) Marisa();
    } else if (tag == "Alice") {
        pRet = new (std::nothrow) Alice();
    }

    if (pRet && pRet->init(tag)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

void
Player::changeAttackType(const std::string& startType)
{
    if (startType == this->type1.tag) {
        // this->emitter->playStyle(sc1);//传入弹幕参数

        /*临时代码*/
        StyleConfig sc1;
        sc1.style = StyleType::PARABOLA;
        sc1.frequency = 0.15f;
        sc1.bulletDuration = 1.5;
        sc1.number = 13;
        sc1.countThenChangePos = 4;
        sc1.cycleTimes = -1;
        sc1.totalDuration = FLT_MAX;
        sc1.height = 50;
        sc1.distance = 345;
        sc1.startAngle = -13;
        sc1.endAngle = 53;
        sc1.bc.name = "b3_1_3.png";
        sc1.bc.length = 30;
        sc1.bc.width = 30;
        sc1.bc.harm = 5;
        sc1.bc._categoryBitmask = bulletCategory;
        sc1.bc._collisionBitmask = enemyCategory;
        sc1.bc._contactTestBitmask = enemyCategory;
        emitter->playStyle(sc1);
        /*临时代码*/
    } else {
        // this->emitter->playStyle(sc2);//传入弹幕参数

        /*临时代码*/
        StyleConfig sc2;
        sc2.style = StyleType::SCATTER;
        sc2.frequency = 0.15f;
        sc2.bulletDuration = 2;
        sc2.number = 2;
        sc2.countThenChangePos = 4;
        sc2.cycleTimes = -1;
        sc2.totalDuration = FLT_MAX;
        sc2.startAngle = 269;
        sc2.endAngle = 271;
        sc2.deltaAngle = 0;
        sc2.bc.name = "b1_3_3.png";
        sc2.bc.length = 30;
        sc2.bc.width = 30;
        sc2.bc.harm = 5;
        sc2.bc._categoryBitmask = bulletCategory;
        sc2.bc._collisionBitmask = enemyCategory;
        sc2.bc._contactTestBitmask = enemyCategory;
        emitter->playStyle(sc2);
    }
    this->currentAttackType = startType;
}

void
Player::stopAttackType()
{
    emitter->stopAllStyle();
}

void
Player::resetJump()
{
    this->jumpCounts = 2;
}

void
Player::getHit(DamageInfo* damageInfo, EventFilterManager* eventFilterManager)
{
    //小跳
    auto velocity = body->getVelocity();
    Vec2 impluse = Vec2(0.0f, 300.0f);
    body->applyImpulse(impluse);

    //更新血条
    Hp_Mp_Change hpChange;
    hpChange.tag = this->playerTag;
    hpChange.value = -(damageInfo->damage); //此处取反，因为伤害值总是正数
    EventCustom event("hp_change");
    event.setUserData((void*)&hpChange);
    _eventDispatcher->dispatchEvent(&event);

    this->currentHP -= damageInfo->damage;
    if (this->currentHP < 0) {
        this->currentHP = 0;
    }

    //应用着色器
    auto rColor = BlendAction::create(0.1f, Color4B::RED);
    auto bColor = rColor->reverse();
    auto sequence = Sequence::create(bColor, rColor, NULL);
    this->playerSprite->runAction(Repeat::create(sequence, 10));

    //阻隔
    eventFilterManager->addEventFilter(
        [](EventCustom* event) -> void {
            if (event->getEventName() == "bullet_hit_player") {
                event->stopPropagation();
            }
            log("[EventFilterManager] Propagation stopped");
        },
        1, "bullet_hit_player");
}

Player::Stand*
Player::Stand::getInstance()
{
    static Stand instance;
    return &instance;
}

void
Player::Stand::Enter(Player* player)
{
    player->currentAnimateAction = Animate::create(player->standAnimation);
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (-15 < velocity.y && velocity.y < 15) {
                if (velocity.x < -15 || 15 < velocity.x) {
                    player->stateMachine->changeState(Player::Walk::getInstance());
                }
            } else if (velocity.y < -15) {
                player->stateMachine->changeState(Player::Fall::getInstance());
            }
        },
        0.1, "StandUpdate");
}

void
Player::Stand::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("StandUpdate");
}

void
Player::Stand::defaultChangeState(Player* player)
{
}

Player::Walk*
Player::Walk::getInstance()
{
    static Walk instance;
    return &instance;
}

void
Player::Walk::Enter(Player* player)
{
    player->currentAnimateAction = Animate::create(player->moveAnimation);
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (-15 < velocity.y && velocity.y < 15) {
                if (-15 < velocity.x && velocity.x < 15) {
                    player->stateMachine->changeState(Player::Stand::getInstance());
                }
            } else if (velocity.y < -15) {
                player->stateMachine->changeState(Player::Fall::getInstance());
            }
        },
        0.1, "WalkUpdate");
}

void
Player::Walk::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("WalkUpdate");
}

void
Player::Walk::defaultChangeState(Player* player)
{
    player->stateMachine->changeState(Player::Stand::getInstance());
}

Player::Jump*
Player::Jump::getInstance()
{
    static Jump instance;
    return &instance;
}

void
Player::Jump::Enter(Player* player)
{
    player->jump();
    player->currentAnimateAction = Sequence::create(Animate::create(player->preJumpAnimation),
                                                    Animate::create(player->jumpAnimation), NULL);
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (velocity.y <= 0) {
                player->stateMachine->changeState(Player::Fall::getInstance());
            }
        },
        0.1, "JumpUpdate");
}

void
Player::Jump::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("JumpUpdate");
}

void
Player::Jump::defaultChangeState(Player* player)
{
}

Player::Fall*
Player::Fall::getInstance()
{
    static Fall instance;
    return &instance;
}

void
Player::Fall::Enter(Player* player)
{
    player->currentAnimateAction = Sequence::create(Animate::create(player->preFallAnimation),
                                                    Animate::create(player->fallAnimation), NULL);
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (-15 < velocity.y) {
                auto preState = player->stateMachine->getPreviousState();
                if (preState == Player::Stand::getInstance() ||
                    preState == Player::Walk::getInstance()) {
                    player->stateMachine->RevertToPreviousState();
                } else {
                    player->stateMachine->changeState(Player::Stand::getInstance());
                }
            }
        },
        0.1, "FallUpdate");
}

void
Player::Fall::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("FallUpdate");
}

void
Player::Fall::defaultChangeState(Player* player)
{
    player->stateMachine->changeState(Player::Stand::getInstance());
}

Player::Dash*
Player::Dash::getInstance()
{
    static Dash instance;
    return &instance;
}

void
Player::Dash::Enter(Player* player)
{
    player->dash();

    auto animate = Animate::create(player->dashAnimation);
    auto actionDone = CallFuncN::create(CC_CALLBACK_0(Dash::defaultChangeState, this, player));
    player->currentAnimateAction = Sequence::create(animate, actionDone, NULL);
    player->playerSprite->runAction(player->currentAnimateAction);
}

void
Player::Dash::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);

    //减速
    auto currentVelocity = player->body->getVelocity();
    player->body->setVelocity(Vec2(currentVelocity.x / 3.0f, currentVelocity.y));
}

void
Player::Dash::defaultChangeState(Player* player)
{
    player->stateMachine->changeState(Player::Stand::getInstance());
}

Player::UseSpellCard*
Player::UseSpellCard::getInstance()
{
    static UseSpellCard instance;
    return &instance;
}

void
Player::UseSpellCard::Enter(Player* player)
{
    Sprite* portrait = Sprite::create(player->useSpellCardPortrait);
    player->addChild(portrait);
    portrait->setOpacity(50);
    portrait->runAction(
        Sequence::create(MoveBy::create(0.8, Vec2(0, 150)), FadeOut::create(0.3f), NULL));
    portrait->scheduleOnce([portrait](float dt) { portrait->removeFromParent(); }, 1.5, "remove");

    std::function<void(Ref*)> shoot = [player](Ref*) {
        player->emitter->playStyle(player->spellCardStyleConfig);
    };

    auto actionDone =
        CallFuncN::create(CC_CALLBACK_0(UseSpellCard::defaultChangeState, this, player));
    auto animate = Animate::create(player->useSpellCardAnimation);
    player->currentAnimateAction =
        Sequence::create(animate, CallFuncN::create(shoot), actionDone, NULL);
    player->playerSprite->runAction(player->currentAnimateAction);
}

void
Player::UseSpellCard::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
}

void
Player::UseSpellCard::defaultChangeState(Player* player)
{
    player->stateMachine->changeState(Player::Stand::getInstance());
}
