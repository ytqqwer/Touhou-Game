#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "GameData/Character.h"
#include "GameData/Item.h"
#include "GameData/SpellCard.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameplayScene/Emitters/StyleConfig.h"
#include "GameplayScene/common.h"
#include "cocos2d.h"

class Emitter;
class EventFilterManager;

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
    virtual void changeAttackType(const std::string& startType);
    virtual void stopAttackType();

    //更新角色状态
    virtual void updatePlayerStatus() = 0;

    //状态回复
    virtual void regainDashCounts(float dt) = 0;
    virtual void resetJump();

    virtual void getHit(DamageInfo*, EventFilterManager*);

    //动作切换
    void resetAction(Node* node);
    void autoSwitchAnimation(float dt);

public:
    std::string playerTag;
    Sprite* playerSprite;
    Direction playerDirection = Direction::RIGHT;

    //绑定发射器
    Emitter* emitter;

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
    StyleConfig sc1;
    StyleConfig sc2;
    int type1Tag;
    int type2Tag;

    //道具列表
    vector<Item> itemList;

    //符卡列表
    vector<SpellCard> spellCardList;

    //当前动作状态
    ActionState curActionState = ActionState::Default;

protected:
    PhysicsBody* body;
    std::string playerTexture;
    Animation* standAnimation;
    Animation* runAnimation;
    Animation* preJumpAnimation;
    Animation* jumpAnimation;
    Animation* preFallAnimation;
    Animation* fallAnimation;
    Animation* dashAnimation;
};

#endif
