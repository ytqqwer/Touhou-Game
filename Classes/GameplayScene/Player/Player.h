#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "GameData/Character.h"
#include "GameData/GameData.h"
#include "GameData/Item.h"
#include "GameData/SpellCard.h"
#include "GameplayScene/common.h"
#include "cocos2d.h"

using namespace cocos2d;

class Player : public Node
{
public:
    virtual bool init(std::string tag) = 0;
    static Player* create(std::string tag);

public:
    virtual void playerRun(float dt) = 0;
    virtual void playerJump() = 0;
    virtual void playerDash() = 0;

    //切换攻击方式
    virtual void changeAttackType(const std::string& startType) = 0;
    virtual void stopAttackType(const std::string& stopType) = 0;

    //更新角色状态
    virtual void updatePlayerStatus(float dt) = 0;

    //状态回复
    void regainDashCounts(float dt);

    //动作切换
    void resetAction(Node* node);
    void autoSwitchAnimation(float dt);

    //发射子弹
    Vector<Sprite*> vecBullet;        //子弹容器
    SpriteBatchNode* bulletBatchNode; //批次渲染节点
    void ShootBullet(float dt);
    void removeBullet(Node* pNode);

public:
    std::string playerTag;
    Sprite* playerSprite;
    PhysicsBody* body;
    Direction playerDirection = Direction::RIGHT;

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
    ActionState curAction = ActionState::Default;

protected:
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
