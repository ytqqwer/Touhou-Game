#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Player.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameplayScene/Player/Alice.h"
#include "GameplayScene/Player/Marisa.h"
#include "GameplayScene/Player/Reimu.h"

#include "GameplayScene/EventFilterManager.h"

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
    this->curActionState = ActionState::Default;
}

void
Player::getHit(DamageInfo* damageInfo, EventFilterManager* eventFilterManager)
{
    //暂时代替受损
    auto velocity = body->getVelocity();
    Vec2 impluse = Vec2(0.0f, 300.0f);
    body->applyImpulse(impluse);

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

void
Player::resetAction(Node* node)
{
    playerSprite->stopAllActions();
    curActionState = ActionState::Default;
}

void
Player::autoSwitchAnimation(float dt)
{
    if (curActionState != ActionState::Dash) {
        Vec2 velocity = body->getVelocity();
        if (-25 < velocity.y && velocity.y < 25) {
            if (-25 < velocity.x && velocity.x < 25) { //站立
                if (curActionState != ActionState::Stand) {
                    if (curActionState != ActionState::Jump) {
                        if (curActionState != ActionState::Fall) {
                            curActionState = ActionState::Stand;
                            playerSprite->stopAllActions();
                            playerSprite->runAction(
                                RepeatForever::create(Animate::create(standAnimation)));
                        }
                    }
                }
            } else if (-25 > velocity.x || velocity.x > 25) { //行走
                if (-25 < velocity.y || velocity.y < 25) {
                    if (curActionState != ActionState::Run) {
                        curActionState = ActionState::Run;
                        playerSprite->stopAllActions();
                        playerSprite->runAction(
                            RepeatForever::create(Animate::create(runAnimation)));
                    }
                }
            }
        }
        if (-25 > velocity.y || velocity.y > 25) {
            if (velocity.y > 25) { //向上跳跃
                if (curActionState != ActionState::Jump) {
                    curActionState = ActionState::Jump;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preJumpAnimation),
                                                     Animate::create(jumpAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            } else if (-25 > velocity.y) { //下降
                if (curActionState != ActionState::Fall) {
                    curActionState = ActionState::Fall;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preFallAnimation),
                                                     Animate::create(fallAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            }
        }
    }
}
