#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "GameData/Character.h"
#include "GameData/Item.h"
#include "GameData/SpellCard.h"
#include "GameplayScene/AnimateManager.h"
#include "cocos2d.h"

using namespace cocos2d;

typedef enum { RIGHT = 1, LEFT = 2 } PlayerDirection;

typedef enum { Stand = 1, Run = 2, Jump = 3, Fall = 4, Dash = 5, Default = 6 } PlayerActionState;

class Player : public Node
{
public:
    virtual bool init(std::string tag);

    static Player* create(std::string tag)
    {
        Player* pRet = new (std::nothrow) Player();
        if (pRet && pRet->init(tag)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

public:
    void playerRun(float dt);
    void playerJump();
    void playerDash();

    //动作切换
    void resetAction(Node* node);
    void autoSwitchAction();

    //状态回复
    void regainDashCounts(float dt);

    //切换攻击方式
    void changeAttackType(const std::string& startType);
    void stopAttackType(const std::string& stopType);

    //使用道具或符卡
    void useItem(const std::string& itemTag);
    void useSpellCard(const std::string& cardTag);

    //更新角色状态
    void updateStatus(float dt);

    //暂时的发射子弹的函数
    Vector<Sprite*> vecBullet;        //子弹容器
    SpriteBatchNode* bulletBatchNode; //批次渲染节点
    void ShootBullet(float dt);
    void removeBullet(Node* pNode);

public:
    Sprite* playerSprite;
    PhysicsBody* body;
    PlayerDirection playerDirection = PlayerDirection::RIGHT;

    //生命值
    int healthPointBase;
    //灵力值
    int manaBase;

    //行走相关
    int walkSpeedBase;
    int walkMaxSpeed;
    float walkAccelerationTimeBase;
    float walkAccelerationBase;

    // dash相关
    float dashAccelerationBase;

    //动作次数上限
    int jumpCounts = 2;
    int dashCounts = 2;

    //攻击方式
    std::string currentAttackType;
    Character::Attack type1;
    Character::Attack type2;

    //道具列表
    vector<Item> itemList;

    //符卡列表
    vector<SpellCard> spellCardList;

    //当前动作状态
    PlayerActionState curAction = PlayerActionState::Default;

private:
    AnimateManager* animateManager;
    std::string playerTextureName;
    Animation* standAnimation;
    Animation* runAnimation;
    Animation* preJumpAnimation;
    Animation* jumpAnimation;
    Animation* preFallAnimation;
    Animation* fallAnimation;
    Animation* dashAnimation;
};

#endif
