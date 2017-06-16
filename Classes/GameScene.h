#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"

#include "Player.h"

USING_NS_CC;

class GameScene : public cocos2d::Scene
{
public:
	//static Scene* createScene();

	CREATE_FUNC(GameScene);

	void onEnter() override;
	void onExit() override;
	virtual bool init() override;

	void initBackGround();
	void initMap();
	void initCtrlPanel();
	void initCharacter();
	void initCamera();
	void initLauncher();
	void initEnemy();
	void initListener();

	//void moveCamera(float);

	Player* _player;

	ParticleSystem* ps;

	void update(float dt);

	Sprite* camera;
	TMXTiledMap* _map;
	Layer* mapLayer;

	//PhysicsWorld* _pWorld;

	bool onContactBegin(const PhysicsContact& contact);
	bool onTouchBegan(Touch* touch, Event *event);
	void onTouchEnded(Touch* touch, Event *event);



	Vector <Sprite *> vecBullet;//子弹容器  
	SpriteBatchNode* bulletBatchNode;//批次渲染节点  
	void ShootBullet(float dt);
	void removeBullet(Node * pNode);


private:
	bool createPhysical(float scale);
	//Scene* gameScene;

};

#endif