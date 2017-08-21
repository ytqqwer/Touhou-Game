#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Player.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameplayScene/Player/Marisa.h"
#include "GameplayScene/Player/Reimu.h"

Player*
Player::create(std::string tag)
{
    Player* pRet;
    if (tag == "Reimu") {
        pRet = new (std::nothrow) Reimu();
    } else if (tag == "Marisa") {
        pRet = new (std::nothrow) Marisa();
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
Player::regainDashCounts(float dt)
{
    this->dashCounts++;
}

void
Player::resetAction(Node* node)
{
    playerSprite->stopAllActions();
    curAction = ActionState::Default;
}

void
Player::autoSwitchAnimation(float dt)
{
    if (curAction != ActionState::Dash) {
        Vec2 velocity = body->getVelocity();
        if (-15 < velocity.y && velocity.y < 15) {
            if (-15 < velocity.x && velocity.x < 15) { //站立
                if (curAction != ActionState::Stand) {
                    if (curAction != ActionState::Jump) {
                        if (curAction != ActionState::Fall) {
                            curAction = ActionState::Stand;
                            playerSprite->stopAllActions();
                            playerSprite->runAction(
                                RepeatForever::create(Animate::create(standAnimation)));
                        }
                    }
                }
            } else if (-15 > velocity.x || velocity.x > 15) { //行走
                if (-15 < velocity.y || velocity.y < 15) {
                    if (curAction != ActionState::Run) {
                        curAction = ActionState::Run;
                        playerSprite->stopAllActions();
                        playerSprite->runAction(
                            RepeatForever::create(Animate::create(runAnimation)));
                    }
                }
            }
        }
        if (-15 > velocity.y || velocity.y > 15) {
            if (velocity.y > 15) { //向上跳跃
                if (curAction != ActionState::Jump) {
                    curAction = ActionState::Jump;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preJumpAnimation),
                                                     Animate::create(jumpAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            } else if (-15 > velocity.y) { //下降
                if (curAction != ActionState::Fall) {
                    curAction = ActionState::Fall;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preFallAnimation),
                                                     Animate::create(fallAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            }
        }
    }
}
