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
        this->emitter->resumeStyle(type1Tag);
        // this->emitter->playStyle(sc1);//传入弹幕参数
    } else {
        // this->emitter->playStyle(sc2);
    }
    this->currentAttackType = startType;
}

void
Player::stopAttackType()
{
    // emitter->stopAllStyle();
    emitter->pauseAllStyle();
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
