#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "GameData/GameData.h"
#include "cocos2d.h"

class Player;
class EventFilterManager;
class EventScriptHanding;

USING_NS_CC;

class GameplayScene : public cocos2d::Scene
{
public:
    friend class EventScriptHanding;

    static GameplayScene* create(const std::string&);
    virtual bool init() override;

    //重载场景生命周期中的几个函数
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;

    void update(float dt);

    //初始化工作
    void initBackGround();
    void initMap();
    void initAnimationCache();
    void initCtrlPanel();
    void initCharacter();
    void initArea();
    void initCamera();
    void initEnemy();
    void initEvent();
    void initPhysicsContactListener();
    void initCustomEventListener();

    /*临时项*/
    void initLauncher();
    void initElevator();
    /*临时项*/

    //对碰撞进行处理
    bool contactBegin(const PhysicsContact& contact);
    bool contactSeparate(const PhysicsContact& contact);

    //
    void endGame();

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

    GameplayScene(const std::string&);
    //析构函数，释放事件脚本处理器内存等
    ~GameplayScene();

private:
    //实用的全局量
    Size visibleSize;
    EventFilterManager* _eventFilterMgr;
    EventScriptHanding* _eventScriptHanding;

    //对curPlayer的操作就是对当前指定角色的操作
    Player* curPlayer;
    Player* p1Player;
    Player* p2Player;

    //摄像机节点
    Sprite* camera;

    //背景图片
    Sprite* backgroundPicture;

    //瓦片地图对象，需要从中读取数据
    std::string selectedMap;
    TMXTiledMap* _map;
    Rect curArea;

    //是否有boss
    bool _hasBoss;

    Vector<Node*> eventPoint;
    Vector<Node*> enemyList;

    /*临时项*/
    Vector<Node*> launcherList;
    Vector<Node*> elevatorList;
    /*临时项*/

    //游戏场景的几个重要层
    Layer* backgroundLayer;
    Layer* mapLayer;
    Layer* controlPanel;

    static const std::string TAG;
};

#endif
