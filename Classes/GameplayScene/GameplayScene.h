#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "GameData/GameData.h"
#include "cocos2d.h"

#include "GameplayScene/EventFilterManager.h"
#include "GameplayScene/Player/Player.h"

USING_NS_CC;

class GameplayScene : public cocos2d::Scene
{
public:
    CREATE_FUNC(GameplayScene);

    virtual bool init() override;
    //重载场景生命周期中的几个函数
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;

    //初始化工作
    void initBackGround();
    void initMap();
    void initCtrlPanel();
    void initCharacter();
    void initArea();
    void initCamera();
    void initLauncher();
    void initEnemy();
    void initEvent();
    void initPhysicsContactListener();
    void initCustomEventListener();

    void update(float dt);
    void testEventFilterManager();

    void eventHandling(EventCustom*);
    void eventActionHandling(float delay, float duration);
    void nextEvent();

    //对碰撞进行处理
    bool contactFilter(const PhysicsContact& contact);

public:
    void onEventLeftKeyPressed(EventCustom*);
    void onEventRightKeyPressed(EventCustom*);
    void onEventMotionKeyReleased(EventCustom*);
    void onEventJumpKeyPressed(EventCustom*);
    void onEventDashKeyPressed(EventCustom*);
    void onEventSwitchCharacter(EventCustom*);
    void onEventSwitchAttackType(EventCustom*);
    void onEventSettingsKeyPressed(EventCustom*);

private:
    // 在地图中生成静态刚体
    bool createPhysical(float scale);

private:
    //实用的全局量
    GameData* gameData;
    Size visibleSize;
    EventFilterManager* _eventFilterMgr;

    //对curPlayer的操作就是对当前指定角色的操作
    Player* curPlayer;
    Player* p1Player;
    Player* p2Player;

    //摄像机节点
    Sprite* camera;

    //瓦片地图对象，需要从中读取数据
    TMXTiledMap* _map;
    Rect curArea;

    Vector<Node*> eventPoint;
    Vector<Node*> enemyList;
    Vector<Node*> launcherList;

    vector<EventData> eventList;
    int _curEventIndex;

    //游戏场景的几个重要层
    Layer* mapLayer;
    Layer* controlPanel;

    static const std::string TAG;
};

#endif
