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
    void initBackgroundAndForeground();
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

    void initDecoration(Layer* layer, const std::string& objectGroup);

private:
    //实用的全局量
    Size visibleSize;
    EventFilterManager* _eventFilterMgr;
    EventScriptHanding* _eventScriptHanding;

    //视差节点以及辅助变量
    ParallaxNode* backgroundParallaxNode;
    ParallaxNode* foregroundParallaxNode;
    Vec2 backParallaxNodePrePosition;
    Vec2 foreParallaxNodePrePosition;
    Vec2 mapLayerPrePosition;
    bool isParallaxPositionInit = true;
    //背景
    Sprite* backgroundParallaxPicture;
    Layer* backgroundParallaxDecoration;
    Layer* backgroundStaticDecoration;
    //前景
    Layer* foregroundParallaxDecoration;
    Layer* foregroundStaticDecoration;

    //对curPlayer的操作就是对当前指定角色的操作
    Player* curPlayer;
    Player* p1Player;
    Player* p2Player;

    //摄像机节点
    Node* camera;

    //瓦片地图对象，需要从中读取数据
    std::string selectedMap;
    TMXTiledMap* _map;
    Rect curArea;

    // boss数目
    unsigned int _bosses;

    Vector<Node*> eventPoint;
    Vector<Node*> enemyList;

    /*临时项*/
    Vector<Node*> launcherList;
    Vector<Node*> elevatorList;
    /*临时项*/

    //游戏场景的几个重要层
    Layer* mapLayer;
    Layer* controlPanel;

    static const std::string TAG;
};

#endif
