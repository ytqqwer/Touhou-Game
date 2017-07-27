#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "GameData/GameData.h"
#include "cocos2d.h"

#include "GameplayScene/EventFilterManager.h"
#include "GameplayScene/Player.h"

USING_NS_CC;

class GameplayScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(GameplayScene);

    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;
    virtual bool init() override;

    void initBackGround();
    void initMap();
    void initCtrlPanel();
    void initCharacter();
    void initCamera();
    void initLauncher();
    void initEnemy();
    void initListener();

    void update(float dt);

    void testEventFilterManager();

    // void moveCamera(float);

    GameData* gameData;
    Size visibleSize;

    Player* curPlayer;
    Player* p1Player;
    Player* p2Player;

    Sprite* camera;
    TMXTiledMap* _map;

    Layer* mapLayer;
    Layer* controlPanel;
    Layer* p1ControlPanel;
    Layer* p2ControlPanel;

    //监听器使用的回掉函数
    bool onContactGround(const PhysicsContact& contact);
    bool onContactBullet(const PhysicsContact& contact);

    //用来接受触摸操作
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);

    //切换攻击方式
    void changeAttackType(const std::string& startType);
    void stopAttackType(const std::string& stopType);

    //使用道具或符卡
    void useItem(Player*& player, const std::string& itemTag);
    void useSpellCard(Player*& player, const std::string& cardTag);

    //暂时的发射子弹的函数
    Vector<Sprite*> vecBullet;        //子弹容器
    SpriteBatchNode* bulletBatchNode; //批次渲染节点
    void ShootBullet(float dt);
    void removeBullet(Node* pNode);

private:
    static const std::string TAG;

    bool createPhysical(float scale);

    EventFilterManager* _eventFilterMgr;
};

#endif
