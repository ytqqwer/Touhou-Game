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

    void resetAction(Node* node);
    void autoSwitchAction();
    void regainDashCounts(float dt);

    //切换攻击方式
    void changeAttackType(const std::string& startType);
    void stopAttackType(const std::string& stopType);

    //使用道具或符卡
    void useItem(const std::string& itemTag);
    void useSpellCard(const std::string& cardTag);

    //暂时的发射子弹的函数
    Vector<Sprite*> vecBullet;        //子弹容器
    SpriteBatchNode* bulletBatchNode; //批次渲染节点
    void ShootBullet(float dt);
    void removeBullet(Node* pNode);

    void updateStatus(float dt);

public:
    std::string playerDirection = "right";
    int healthPointBase;
    int manaBase;

    int walkSpeedBase;
    int walkMaxSpeed;
    float walkAccelerationTimeBase;
    float walkAccelerationBase;

    float dashAccelerationBase;

    int jumpCounts = 2;
    int dashCounts = 2;

    std::string currentAttackType;
    Character::Attack type1;
    Character::Attack type2;

    vector<Item> itemList;
    vector<SpellCard> spellCardList;

    Sprite* playerSprite;

    PhysicsBody* body;

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
