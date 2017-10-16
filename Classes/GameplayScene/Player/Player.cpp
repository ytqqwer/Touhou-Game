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

Player*
Player::create(std::string tag)
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
        sc1.duration = 2;
        sc1.number = 7;
        sc1.count = 4;
        sc1.height = 150;
        sc1.distance = 600;
        sc1.bc.name = "b3_1_3.png";
        sc1.bc.length = 30;
        sc1.bc.width = 30;
        sc1.bc.harm = 8;
        sc1.bc._categoryBitmask = bulletCategory;
        sc1.bc._collisionBitmask = enemyCategory;
        sc1.bc._contactTestBitmask = enemyCategory;
        emitter->playStyle(sc1);
        /*临时代码*/
    } else {
        // this->emitter->playStyle(sc2);//传入弹幕参数
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
    //暂时代替受损
    auto velocity = body->getVelocity();
    Vec2 impluse = Vec2(0.0f, 300.0f);
    body->applyImpulse(impluse);

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

Player::StandAnimation*
Player::StandAnimation::getInstance()
{
    static StandAnimation instance;
    return &instance;
}

void
Player::StandAnimation::Enter(Player* player)
{
    player->currentAnimateAction = RepeatForever::create(Animate::create(player->standAnimation));
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (-25 < velocity.y && velocity.y < 25) {
                if (velocity.x < -25 || 25 < velocity.x) {
                    player->animateStateMachine->changeState(Player::RunAnimation::getInstance());
                }
            } else {
                if (25 < velocity.y) {
                    player->animateStateMachine->changeState(Player::JumpAnimation::getInstance());
                } else if (velocity.y < -25) {
                    player->animateStateMachine->changeState(Player::FallAnimation::getInstance());
                }
            }
        },
        0.1, "StandAnimationUpdate");
}

void
Player::StandAnimation::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("StandAnimationUpdate");
}

void
Player::StandAnimation::changeToState(Player* player)
{
}

Player::RunAnimation*
Player::RunAnimation::getInstance()
{
    static RunAnimation instance;
    return &instance;
}

void
Player::RunAnimation::Enter(Player* player)
{
    player->currentAnimateAction = RepeatForever::create(Animate::create(player->runAnimation));
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (-25 < velocity.y && velocity.y < 25) {
                if (-25 < velocity.x && velocity.x < 25) {
                    player->animateStateMachine->changeState(Player::StandAnimation::getInstance());
                }
            } else {
                if (25 < velocity.y) {
                    player->animateStateMachine->changeState(Player::JumpAnimation::getInstance());
                } else if (velocity.y < -25) {
                    player->animateStateMachine->changeState(Player::FallAnimation::getInstance());
                }
            }
        },
        0.1, "RunAnimationUpdate");
}

void
Player::RunAnimation::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("RunAnimationUpdate");
}

void
Player::RunAnimation::changeToState(Player* player)
{
}

Player::JumpAnimation*
Player::JumpAnimation::getInstance()
{
    static JumpAnimation instance;
    return &instance;
}

void
Player::JumpAnimation::Enter(Player* player)
{
    player->currentAnimateAction = Sequence::create(Animate::create(player->preJumpAnimation),
                                                    Animate::create(player->jumpAnimation), NULL);
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (velocity.y < -25) {
                player->animateStateMachine->changeState(Player::FallAnimation::getInstance());
            }
        },
        0.1, "JumpAnimationUpdate");
}

void
Player::JumpAnimation::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("JumpAnimationUpdate");
}

void
Player::JumpAnimation::changeToState(Player* player)
{
}

Player::FallAnimation*
Player::FallAnimation::getInstance()
{
    static FallAnimation instance;
    return &instance;
}

void
Player::FallAnimation::Enter(Player* player)
{
    player->currentAnimateAction = Sequence::create(Animate::create(player->preFallAnimation),
                                                    Animate::create(player->fallAnimation), NULL);
    player->playerSprite->runAction(player->currentAnimateAction);

    player->playerSprite->schedule(
        [player](float dt) {
            Vec2 velocity = player->body->getVelocity();
            if (-25 < velocity.y) {
                player->animateStateMachine->changeState(Player::StandAnimation::getInstance());
            }
        },
        0.1, "FallAnimationUpdate");
}

void
Player::FallAnimation::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
    player->playerSprite->unschedule("FallAnimationUpdate");
}

void
Player::FallAnimation::changeToState(Player* player)
{
}

Player::DashAnimation*
Player::DashAnimation::getInstance()
{
    static DashAnimation instance;
    return &instance;
}

void
Player::DashAnimation::Enter(Player* player)
{
    auto animate = Animate::create(player->dashAnimation);
    auto actionDone = CallFuncN::create(CC_CALLBACK_0(DashAnimation::changeToState, this, player));
    player->currentAnimateAction = Sequence::create(animate, actionDone, NULL);
    player->playerSprite->runAction(player->currentAnimateAction);
}

void
Player::DashAnimation::Exit(Player* player)
{
    player->playerSprite->stopAction(player->currentAnimateAction);
}

void
Player::DashAnimation::changeToState(Player* player)
{
    player->animateStateMachine->changeState(Player::StandAnimation::getInstance());
}
