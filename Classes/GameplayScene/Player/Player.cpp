#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Player/Player.h"
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
Player::regainDashCounts(float dt)
{
    this->dashCounts++;
}

void
Player::resetAction(Node* node)
{
    playerSprite->stopAllActions();
    curAction = PlayerActionState::Default;
}

void
Player::autoSwitchAction()
{
    if (curAction != PlayerActionState::Dash) {
        Vec2 velocity = body->getVelocity();
        if (-15 < velocity.y && velocity.y < 15) {
            if (-15 < velocity.x && velocity.x < 15) { //站立
                if (curAction != PlayerActionState::Stand) {
                    if (curAction != PlayerActionState::Jump) {
                        if (curAction != PlayerActionState::Fall) {
                            curAction = PlayerActionState::Stand;
                            playerSprite->stopAllActions();
                            playerSprite->runAction(
                                RepeatForever::create(Animate::create(standAnimation)));
                        }
                    }
                }
            } else if (-15 > velocity.x || velocity.x > 15) { //行走
                if (-15 < velocity.y || velocity.y < 15) {
                    if (curAction != PlayerActionState::Run) {
                        curAction = PlayerActionState::Run;
                        playerSprite->stopAllActions();
                        playerSprite->runAction(
                            RepeatForever::create(Animate::create(runAnimation)));
                    }
                }
            }
        }
        if (-15 > velocity.y || velocity.y > 15) {
            if (velocity.y > 15) { //向上跳跃
                if (curAction != PlayerActionState::Jump) {
                    curAction = PlayerActionState::Jump;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preJumpAnimation),
                                                     Animate::create(jumpAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            } else if (-15 > velocity.y) { //下降
                if (curAction != PlayerActionState::Fall) {
                    curAction = PlayerActionState::Fall;
                    playerSprite->stopAllActions();
                    auto sequence = Sequence::create(Animate::create(preFallAnimation),
                                                     Animate::create(fallAnimation), NULL);
                    playerSprite->runAction(sequence);
                }
            }
        }
    }
}

//用缓存的方法创建子弹，并初始化子弹的运动和运动后的事件
void
Player::ShootBullet(float dt)
{
    Size winSize = Director::getInstance()->getWinSize();
    //从缓存中创建子弹
    auto spritebullet = Sprite::createWithTexture(bulletBatchNode->getTexture());
    spritebullet->setTag(bulletCategoryTag);

    //将创建好的子弹添加到BatchNode中进行批次渲染
    bulletBatchNode->addChild(spritebullet);

    //给创建好的子弹添加刚体
    do {
        auto _body = PhysicsBody::createBox(spritebullet->getContentSize());
        _body->setRotationEnable(false);
        _body->setGravityEnable(false);

        _body->setContactTestBitmask(bulletCategory);
        _body->setCollisionBitmask(enemyCategory);
        _body->setContactTestBitmask(enemyCategory);
        spritebullet->setPhysicsBody(_body);
    } while (0);

    //将创建好的子弹添加到容器
    vecBullet.pushBack(spritebullet);

    float realFlyDuration = 1.0;
    //子弹运行的距离和时间
    auto actionMove = MoveBy::create(realFlyDuration, Point(winSize.width, 0));
    auto fire1 = actionMove;

    if (this->playerDirection == PlayerDirection::LEFT) {
        fire1 = actionMove->reverse();
    }

    //子弹执行完动作后进行函数回调，调用移除子弹函数
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Marisa::removeBullet, this));

    //子弹开始跑动
    Sequence* sequence = Sequence::create(fire1, actionDone, NULL);

    spritebullet->runAction(sequence);
}

//移除子弹，将子弹从容器中移除，同时也从SpriteBatchNode中移除
void
Player::removeBullet(Node* pNode)
{
    if (NULL == pNode) {
        return;
    }
    Sprite* bullet = (Sprite*)pNode;
    this->bulletBatchNode->removeChild(bullet, true);
    vecBullet.eraseObject(bullet);
}
