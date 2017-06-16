#include "GameScene.h"
#include "Enemy.h"
#include "SettingsLayer.h"

#include "resources.h.dir\common.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#define PTM_RATIO 1

void GameScene::onEnter()
{
	Scene::onEnter();
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->playBackgroundMusic("gameScene/bgm001.mp3", true);//开启循环
}

void GameScene::onExit()
{
	Scene::onExit();
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

bool GameScene::init()
{
	if (!Scene::init()) {
		return false;
	}

	this->initWithPhysics();//初始化物理世界	
	Vect gravity(0, -1000.0f);	//游戏场景的重力
	this->getPhysicsWorld()->setGravity(gravity); //设置重力
	//this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); //调试模式看包围盒
	
	//初始化地图背景
	initBackGround();

	// 初始化地图
	initMap();

	// 初始化控制面板
	initCtrlPanel();

	// 加载角色
	initCharacter();

	// 加载摄像机
	initCamera();

	// 加载发射器
	initLauncher();

	// 加载敌人
	initEnemy();

	// 加载监听器
	initListener();

	// 启动帧定时器
	scheduleUpdate();

	return true;
}


void GameScene::initBackGround()
{
	auto backGroundLayer = Layer::create();

	Sprite* bg = Sprite::create("gamescene/gbg.png");
	bg->setAnchorPoint(Point::ZERO);
	bg->setPosition(Point::ZERO);
	bg->setScale(1.5);

	backGroundLayer->addChild(bg);

	this->addChild(backGroundLayer, -100);
}


void GameScene::initMap()
{
	mapLayer = Layer::create();
	_map = TMXTiledMap::create("gamescene/test.tmx");
	_map->setScale(1.0f);
	mapLayer->addChild(_map);
	this->addChild(mapLayer,0);

	//创建静态刚体墙  
	createPhysical(1);

}

//创建静态刚体，接受参数设置刚体大小倍率
bool GameScene::createPhysical(float scale)
{
	// 找出阻挡区域所在的层
	TMXObjectGroup* group = _map->getObjectGroup("physics");
	auto objects = group->getObjects();
	//Value objectsVal = Value(objects);
	//log("%s", objectsVal.getDescription().c_str());

	for (auto& v : objects)
	{
		auto dict = v.asValueMap();

		if (dict.size() == 0)
			continue;

		// 读取所有形状的起始点  
		float x = dict["x"].asFloat() * scale;
		float y = dict["y"].asFloat() * scale;
		//log("x:%f\ny:%f", x, y);

		//多边形polygonPoints
		if (dict.find("points") != dict.end()) {
			auto polygon_points = dict["points"].asValueVector();

			Vec2 points[20];
			int shapeVecAmount = 0;					//每个shape的顶点个数

													// 必须将所有读取的定点逆向，因为翻转y之后，三角形定点的顺序已经逆序了，构造b2PolygonShape会crash  
			int c = polygon_points.size();
			polygon_points.resize(c);
			c--;

			for (auto obj : polygon_points)
			{
				// 相对于起始点的偏移
				float offx = obj.asValueMap()["x"].asFloat() * scale;
				float offy = obj.asValueMap()["y"].asFloat() * scale;

				points[c] = Vec2((x + offx) / PTM_RATIO, (y - offy) / PTM_RATIO);
				c--;
				shapeVecAmount++;
			}

			PhysicsBody* _pBody = PhysicsBody::createPolygon(points, shapeVecAmount);
			_pBody->getFirstShape()->setDensity(0);
			_pBody->getFirstShape()->setFriction(1.0);
			_pBody->getFirstShape()->setRestitution(0);
			_pBody->setDynamic(false);

			_pBody->setCategoryBitmask(groundCategory);		//给多边形地面设置掩码，默认值为0xFFFFFFFF
			_pBody->setCollisionBitmask(playerCategory | enemyCategory);				//默认值为0xFFFFFFFF
			_pBody->setContactTestBitmask(playerCategory);			//默认值为0


			auto sprite = Sprite::create();
			sprite->setTag(98);
			sprite->setPhysicsBody(_pBody);
			mapLayer->addChild(sprite);
		}
		else if (dict.find("polylinePoints") != dict.end()) {
			auto polyline_points = dict["polylinePoints"].asValueVector();

			int shapeVecAmount = 0;					//每个shape的顶点个数
			Vec2 points[20];

			int i = 0;
			for (auto obj : polyline_points)
			{
				float offx = obj.asValueMap()["x"].asFloat() * scale;
				float offy = obj.asValueMap()["y"].asFloat() * scale;

				points[i] = Vec2((x + offx) / PTM_RATIO, (y - offy) / PTM_RATIO);
				i++;
				shapeVecAmount++;
			}

			PhysicsBody* _pBody = PhysicsBody::createEdgeChain(points, shapeVecAmount);
			_pBody->getFirstShape()->setDensity(0);
			_pBody->getFirstShape()->setFriction(1.0);
			_pBody->getFirstShape()->setRestitution(0);
			_pBody->setDynamic(false);

			_pBody->setCategoryBitmask(groundCategory);		//给折线地面设置掩码，默认值为0xFFFFFFFF
			_pBody->setCollisionBitmask(playerCategory | enemyCategory);	//默认值为0xFFFFFFFF
			_pBody->setContactTestBitmask(playerCategory | enemyCategory);	//默认值为0


			auto sprite = Sprite::create();
			sprite->setPhysicsBody(_pBody);
			sprite->setTag(99);

			mapLayer->addChild(sprite);
		}
		else {
			PhysicsBody* _pBody;

			float width = dict["width"].asFloat() * scale;
			float height = dict["height"].asFloat() * scale;

			_pBody = PhysicsBody::createBox(Size(width, height));
			_pBody->getFirstShape()->setDensity(0);
			_pBody->getFirstShape()->setFriction(1.0);
			_pBody->getFirstShape()->setRestitution(0);
			_pBody->setDynamic(false);

			//_pBody->setTag(100);
			_pBody->setCategoryBitmask(groundCategory);		//给矩形地面设置掩码，默认值为0xFFFFFFFF
			_pBody->setCollisionBitmask(playerCategory | enemyCategory);				//默认值为0xFFFFFFFF
			_pBody->setContactTestBitmask(playerCategory | enemyCategory);			//默认值为0

			auto sprite = Sprite::create();
			sprite->setTag(100);
			sprite->setPosition(x + width / 2.0f, y + height / 2.0f);
			sprite->setPhysicsBody(_pBody);
			mapLayer->addChild(sprite);
		}
	}
	return true;
}

bool GameScene::onContactBegin(const PhysicsContact& contact) {

	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	if (nodeA && nodeB)
	{
		if (nodeA->getTag() == 99)
		{
			return contact.getContactData()->normal.y > 0;
		}
		else if (nodeB->getTag() == 99)
		{
			return contact.getContactData()->normal.y > 0;
		}
	}
	return true;
}


void GameScene::initCtrlPanel()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

	auto controlPanel = Layer::create();
	this->addChild(controlPanel);

	auto set_button = Button::create("CloseNormal.png");
	set_button->setPosition(Vec2(visibleSize.width * 0.080, visibleSize.height * 0.950));
	set_button->setScale(1.5);
	set_button->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			auto layer = SettingsLayer::create("HomeScene");
			this->addChild(layer, 1000);//最大优先级
		}
	});	
	controlPanel->addChild(set_button);
	
	//auto jumpBtn = Sprite::create("jumpButton.png");
	//jumpBtn->setScale(0.5);
	////jumpBtn->setPosition(Point(visibleOrigin.x + visibleSize.width - 100, visibleOrigin.y + 50));
	//jumpBtn->setPosition(Point(visibleSize.width/2, visibleOrigin.y/2 + 50));
	//this->addChild(jumpBtn, 1000);

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

	auto dispatcher = Director::getInstance()->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	//this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	//this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

}

bool GameScene::onTouchBegan(Touch* touch, Event *event) {
	auto location = touch->getLocation();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	//Player* p = (Player *)this->getParent()->getChildByName("player");
	//auto body = p->getPhysicsBody();

	auto body = _player->getPhysicsBody();

	if (location.x > visibleSize.width / 2 && location.y < visibleSize.height / 2) {
		//触碰右下屏
		//_player->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRunRight));
		_player->setScaleX(1);//人物翻转
		_player->attackDirection = "right";
		_player->schedule(CC_SCHEDULE_SELECTOR(Player::playerRunRight));
	}

	else if (location.x < visibleSize.width / 2 && location.y < visibleSize.height / 2) {
		//触碰左下屏  
		//_player->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRunLeft));
		_player->setScaleX(-1);//人物翻转
		_player->attackDirection = "left";
		_player->schedule(CC_SCHEDULE_SELECTOR(Player::playerRunLeft));
	}

	else if (location.y >= visibleSize.height / 2) {
		//触碰上屏
		_player->playerJump();
	}
	return true;
}

void GameScene::onTouchEnded(Touch * touch, Event * event)
{
	auto location = touch->getLocation();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	if (location.x > visibleSize.width / 2 && location.y < visibleSize.height / 2) {
		//触碰右下方结束，解除向右状态 
		_player->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRunRight));
	}
	else if (location.x < visibleSize.width / 2 && location.y < visibleSize.height / 2) {
		//触碰左下方结束，解除向左状态
		_player->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRunLeft));
	}
	else if (location.y >= visibleSize.height / 2) {
		//请不要拖动鼠标
	}
	//自动减速还没有做
}

void GameScene::initCharacter()
{
	TMXObjectGroup* temp = _map->getObjectGroup("player");
	auto ts = temp->getObject("birthPoint");
	//log("%s", Value(ts).getDescription().c_str());

	float a = ts["x"].asFloat();
	float b = ts["y"].asFloat();

	_player = Player::create();
	_player->setTag(101);
	_player->setName("player");
	_player->setPosition(a, b);

	mapLayer->addChild(_player);
}

void GameScene::initCamera()
{
	auto mapSize = _map->getMapSize();
	auto mapTileSize = _map->getTileSize();

	camera = Sprite::create();
	camera->setPosition(_player->getPosition());
	this->addChild(camera);

	auto follow = Follow::create(camera, Rect(0, 0, mapSize.width * mapTileSize.width * 3, mapSize.height * mapTileSize.height - 50));
	mapLayer->runAction(follow);

	// camera->schedule(CC_SCHEDULE_SELECTOR(GameScene::moveCamera));
	//this->scheduleUpdate();
}

//void GameScene::moveCamera(float dt)
//{
//	//	Player* p = (Player *)this->getChildByName("player");
//
//	Vec2 poi = _player->getPosition();
//	//Vec2 poi = p->getPosition();
//	camera->setPosition(poi.x + 100, poi.y + 70);
//}

void GameScene::initLauncher()
{
	auto s = Director::getInstance()->getWinSize();

	//创建BatchNode节点，成批渲染子弹
	bulletBatchNode = SpriteBatchNode::create("gamescene/bullet1.png");
	mapLayer->addChild(bulletBatchNode);

	//每隔0.2S调用一次发射子弹函数  
	//this->schedule(CC_SCHEDULE_SELECTOR(GameScene::ShootBullet), 0.2f);
	//每隔0.4S调用一次发射子弹函数  
	this->schedule(CC_SCHEDULE_SELECTOR(GameScene::ShootBullet), 0.4f);
}


//用缓存的方法创建子弹，并初始化子弹的运动和运动后的事件
void GameScene::ShootBullet(float dt) {
	Size winSize = Director::getInstance()->getWinSize();
	auto playerPos = _player->getPosition();
	//auto playerPos = _launcher->getPosition();//改为发射器发射
	//从缓存中创建子弹  
	auto spritebullet = Sprite::createWithTexture(bulletBatchNode->getTexture());
	spritebullet->setTag(102);

	auto spritebullet2 = Sprite::createWithTexture(bulletBatchNode->getTexture());
	spritebullet2->setTag(102);

	//将创建好的子弹添加到BatchNode中进行批次渲染
	bulletBatchNode->addChild(spritebullet);
	bulletBatchNode->addChild(spritebullet2);

	//给创建好的子弹添加刚体
	do {
		auto _body = PhysicsBody::createBox(spritebullet->getContentSize());
		_body->setRotationEnable(false);
		_body->setGravityEnable(false);
		//_body->setTag(102);

		_body->setContactTestBitmask(bulletCategory);
		_body->setCollisionBitmask(enemyCategory);
		_body->setContactTestBitmask(enemyCategory);
		spritebullet->setPhysicsBody(_body);
	} while (0);

	do {
		auto _body = PhysicsBody::createBox(spritebullet->getContentSize());
		_body->setRotationEnable(false);
		_body->setGravityEnable(false);
		//_body->setTag(102);

		_body->setCategoryBitmask(bulletCategory);
		_body->setCollisionBitmask(enemyCategory);
		_body->setContactTestBitmask(enemyCategory);

		spritebullet2->setPhysicsBody(_body);
	} while (0);

	//将创建好的子弹添加到容器  
	vecBullet.pushBack(spritebullet);
	vecBullet.pushBack(spritebullet2);

	Point bulletPos = (Point(playerPos.x, playerPos.y));

	//spritebullet->setScale(1.3f);

	spritebullet->setPosition(bulletPos);
	//spritebullet2->setPosition(bulletPos);
	spritebullet2->setPosition(Point(playerPos.x, playerPos.y - 30));

	float realFlyDuration = 1.0;
	//子弹运行的距离和时间
	//auto actionMove = MoveBy::create(realFlyDuration, Point(0,winSize.height));
	//auto actionMove2 = MoveBy::create(realFlyDuration, Point(0,-winSize.height));
	auto actionMove = MoveBy::create(realFlyDuration, Point(winSize.width, 0));
	auto fire1 = actionMove;
	auto actionMove2 = MoveBy::create(realFlyDuration, Point(winSize.width, 0));
	auto fire2 = actionMove2;

	if (_player->attackDirection.compare("right"))
	{
		fire1 = actionMove->reverse();
		fire2 = actionMove2->reverse();
	}

	//子弹执行完动作后进行函数回调，调用移除子弹函数  
	auto actionDone = CallFuncN::create(CC_CALLBACK_1(GameScene::removeBullet, this));

	//子弹开始跑动
	Sequence* sequence = Sequence::create(fire1, actionDone, NULL);
	Sequence* sequence2 = Sequence::create(fire2, actionDone, NULL);

	spritebullet->runAction(sequence);
	spritebullet2->runAction(sequence2);
}

//移除子弹，将子弹从容器中移除，同时也从SpriteBatchNode中移除
void GameScene::removeBullet(Node* pNode) {
	if (NULL == pNode) {
		return;
	}
	Sprite* bullet = (Sprite*)pNode;
	this->bulletBatchNode->removeChild(bullet, true);
	vecBullet.eraseObject(bullet);
}

void GameScene::initEnemy()
{
	TMXObjectGroup* group = _map->getObjectGroup("enemy");
	auto objects = group->getObjects();
	//Value objectsVal = Value(objects);
	//log("%s", objectsVal.getDescription().c_str());

	for (auto v : objects)
	{
		auto dict = v.asValueMap();
		if (dict.size() == 0)
			continue;

		float x = dict["x"].asFloat();
		float y = dict["y"].asFloat();

		Enemy* _enemy = Enemy::create();
		_enemy->setPosition(x, y);
		mapLayer->addChild(_enemy);
		_enemy->schedule(CC_SCHEDULE_SELECTOR(Enemy::AI));
	}
}

//初始化监听器，手动指定优先级
void GameScene::initListener()
{
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	auto listener = EventListenerPhysicsContact::create();
	listener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);

	//dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	dispatcher->addEventListenerWithFixedPriority(listener, 20);

	auto e = Enemy::create();
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Enemy::onContactBegin, e);
	contactListener->onContactSeparate = CC_CALLBACK_1(Enemy::onContactSeparate, e);
	//dispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
	dispatcher->addEventListenerWithFixedPriority(contactListener, 10);

}


void GameScene::update(float dt)
{
	//Player* p = (Player *)this->getChildByName("player");
	//Vec2 poi = p->getPosition();
	Vec2 poi = _player->getPosition();
	camera->setPosition(poi.x + 100, poi.y + 70);//移动摄像机

	//简单的粒子特效
	//ps->setPosition(poi);
}