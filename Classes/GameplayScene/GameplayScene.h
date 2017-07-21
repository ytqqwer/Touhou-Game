﻿#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "GameData/GameData.h"
#include "cocos2d.h"

#include "EventFilterManager.h"
#include "Player.h"

USING_NS_CC;

class GameplayScene : public cocos2d::Scene
{
public:
    // static Scene* createScene();

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
    Player* _player;
    Size visibleSize;

    Sprite* camera;
    TMXTiledMap* _map;
    Layer* mapLayer;

    bool onContactGround(const PhysicsContact& contact);
    bool onContactBullet(const PhysicsContact& contact);

    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);

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