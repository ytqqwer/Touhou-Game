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

#include "GameplayScene/State.h"

class Emitter;
class EventFilterManager;

using namespace cocos2d;

class Player : public Node
{
public:
    virtual bool init(const std::string& tag) = 0;
    static Player* create(const std::string& tag);

    ~Player() { delete stateMachine; }

public:
    virtual void horizontallyAccelerate(float dt) = 0;
    virtual void jump() = 0;
    virtual void dash() = 0;

    //切换攻击方式
    virtual void changeAttackType(const std::string& startType);
    virtual void stopAttackType();

    //更新角色状态
    virtual void updatePlayerStatus() = 0;

    //状态回复
    virtual void regainDashCounts(float dt) = 0;
    virtual void resetJump();

    virtual void getHit(DamageInfo*, EventFilterManager*);

public:
    std::string playerTag;
    Sprite* playerSprite;
    Direction playerDirection = Direction::RIGHT;

    //绑定发射器
    Emitter* emitter;

    //生命值
    int baseHP;
    int currentHP;
    //灵力值
    int manaBase;
    int currentMana;

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

    //状态机
    StateMachine<Player>* stateMachine;

protected:
    PhysicsBody* body;
    Animation* standAnimation;
    Animation* runAnimation;
    Animation* preJumpAnimation;
    Animation* jumpAnimation;
    Animation* preFallAnimation;
    Animation* fallAnimation;
    Animation* dashAnimation;

    Action* currentAnimateAction;

public:
    class Stand : public State<Player>
    {
    public:
        static Stand* getInstance();
        void Enter(Player*);
        void Exit(Player*);
        void defaultChangeState(Player*);
    };

    class Walk : public State<Player>
    {
    public:
        static Walk* getInstance();
        void Enter(Player*);
        void Exit(Player*);
        void defaultChangeState(Player*);
    };

    class Jump : public State<Player>
    {
    public:
        static Jump* getInstance();
        void Enter(Player*);
        void Exit(Player*);
        void defaultChangeState(Player*);
    };

    class Fall : public State<Player>
    {
    public:
        static Fall* getInstance();
        void Enter(Player*);
        void Exit(Player*);
        void defaultChangeState(Player*);
    };

    class Dash : public State<Player>
    {
    public:
        static Dash* getInstance();
        void Enter(Player*);
        void Exit(Player*);
        void defaultChangeState(Player*);
    };
};

#endif
