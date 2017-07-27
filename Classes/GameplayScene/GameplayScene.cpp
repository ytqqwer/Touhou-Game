#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/GameplayScene.h"
#include "Emitters/FirstEmitter.h"
#include "GameplayScene/Enemy.h"
#include "GameplayScene/common.h"
#include "Layers/SettingsLayer.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#define PTM_RATIO 1

const std::string GameplayScene::TAG{ "GameplayScene" };

void
GameplayScene::onEnter()
{
    Scene::onEnter();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playBackgroundMusic("gameplayscene/bgm001.mp3",
                                                          true); //开启循环
}

void
GameplayScene::onExit()
{
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    Scene::onExit();
}

void
GameplayScene::cleanup()
{
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
    // this->removeAllChildren();
}

bool
GameplayScene::init()
{
    if (!Scene::init()) {
        return false;
    }
    gameData = GameData::getInstance();
    visibleSize = Director::getInstance()->getVisibleSize();

    this->initWithPhysics();                      //初始化物理世界
    Vect gravity(0, -1000.0f);                    //游戏场景的重力
    this->getPhysicsWorld()->setGravity(gravity); //设置重力
    // this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); //调试模式看包围盒

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

    // 用于支持符卡 buf 效果的 EventFilterManager
    this->_eventFilterMgr = EventFilterManager::create();
    this->_eventFilterMgr->retain();

    return true;
}

void
GameplayScene::onEnterTransitionDidFinish()
{
    Scene::onEnterTransitionDidFinish();

    testEventFilterManager();
}

void
GameplayScene::testEventFilterManager()
{
    const string filterName = "test_filter";
    const float lastTime = 2;

    /*  1. 添加一个 filter */

    _eventFilterMgr->addEventFilter(
        [](EventCustom* event) -> void {
            log("[EventFilter 1] Filtering event: %s", event->getEventName().c_str());
        },
        2, filterName);

    /*  2. 发出事件 & 待 filter 有效期过去再发事件 */

    this->_eventDispatcher->dispatchCustomEvent("test_event_1");
    this->_eventDispatcher->dispatchCustomEvent("test_event_2");

    scheduleOnce(
        [this](float unused) {
            this->_eventDispatcher->dispatchCustomEvent("test_event_1");
            this->_eventDispatcher->dispatchCustomEvent("test_event_2");
        },
        lastTime + 1, "schedule_test_event_2");
}

void
GameplayScene::initBackGround()
{
    auto backGroundLayer = Layer::create();

    Sprite* bg = Sprite::create("gameplayscene/gbg.png");
    bg->setAnchorPoint(Point::ZERO);
    bg->setPosition(Point::ZERO);
    bg->setScale(1.8);

    backGroundLayer->addChild(bg);

    this->addChild(backGroundLayer, -100);
}

void
GameplayScene::initMap()
{
    mapLayer = Layer::create();
    _map = TMXTiledMap::create("gameplayscene/test.tmx");

    _map->setScale(1.0f);
    mapLayer->addChild(_map);
    mapLayer->setName("mapLayer");
    this->addChild(mapLayer, 0);

    //创建静态刚体墙
    createPhysical(1);
}

//创建静态刚体，接受参数设置刚体大小倍率
bool
GameplayScene::createPhysical(float scale)
{
    // 找出阻挡区域所在的层
    TMXObjectGroup* group = _map->getObjectGroup("physics");
    auto objects = group->getObjects();
    // Value objectsVal = Value(objects);
    // log("%s", objectsVal.getDescription().c_str());

    for (auto& v : objects) {
        auto dict = v.asValueMap();

        if (dict.size() == 0)
            continue;

        // 读取所有形状的起始点
        float x = dict["x"].asFloat() * scale;
        float y = dict["y"].asFloat() * scale;
        // log("x:%f\ny:%f", x, y);

        //多边形polygonPoints
        if (dict.find("points") != dict.end()) {
            auto polygon_points = dict["points"].asValueVector();

            Vec2 points[20];
            int shapeVecAmount = 0; //每个shape的顶点个数

            // 必须将所有读取的定点逆向，因为翻转y之后，三角形定点的顺序已经逆序了，构造PolygonShape会crash
            int c = polygon_points.size();
            polygon_points.resize(c);
            c--;

            for (auto obj : polygon_points) {
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
            _pBody->setCategoryBitmask(groundCategory); //给多边形地面设置掩码，默认值为0xFFFFFFFF
            _pBody->setCollisionBitmask(playerCategory | enemyCategory); //默认值为0xFFFFFFFF
            _pBody->setContactTestBitmask(playerCategory);               //默认值为0

            auto sprite = Sprite::create();
            sprite->setTag(polygonTag);
            sprite->setPhysicsBody(_pBody);
            mapLayer->addChild(sprite);
        } else if (dict.find("polylinePoints") != dict.end()) {
            auto polyline_points = dict["polylinePoints"].asValueVector();

            int shapeVecAmount = 0; //每个shape的顶点个数
            Vec2 points[20];

            int i = 0;
            for (auto obj : polyline_points) {
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
            _pBody->setCategoryBitmask(groundCategory); //给折线地面设置掩码，默认值为0xFFFFFFFF
            _pBody->setCollisionBitmask(playerCategory | enemyCategory);   //默认值为0xFFFFFFFF
            _pBody->setContactTestBitmask(playerCategory | enemyCategory); //默认值为0

            auto sprite = Sprite::create();
            sprite->setPhysicsBody(_pBody);
            sprite->setTag(polylineTag);

            mapLayer->addChild(sprite);
        } else {
            PhysicsBody* _pBody;

            float width = dict["width"].asFloat() * scale;
            float height = dict["height"].asFloat() * scale;

            _pBody = PhysicsBody::createBox(Size(width, height));
            _pBody->getFirstShape()->setDensity(0);
            _pBody->getFirstShape()->setFriction(1.0);
            _pBody->getFirstShape()->setRestitution(0);
            _pBody->setDynamic(false);
            _pBody->setCategoryBitmask(groundCategory); //给矩形地面设置掩码，默认值为0xFFFFFFFF
            _pBody->setCollisionBitmask(playerCategory | enemyCategory);   //默认值为0xFFFFFFFF
            _pBody->setContactTestBitmask(playerCategory | enemyCategory); //默认值为0

            auto sprite = Sprite::create();
            sprite->setTag(groundTag);
            sprite->setPosition(x + width / 2.0f, y + height / 2.0f);
            sprite->setPhysicsBody(_pBody);
            mapLayer->addChild(sprite);
        }
    }
    return true;
}

bool
GameplayScene::onContactGround(const PhysicsContact& contact)
{
    //留空，需要使水平接触墙壁时不做什么处理，暂时不考虑“蹬墙跳”

    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (nodeA && nodeB) {
        Node* entityA;
        Node* entityB;
        auto tagA = nodeA->getTag();
        auto tagB = nodeB->getTag();

        // enemy相关
        if (tagA == enemyTag || tagB == enemyTag) {
            if (tagA == enemyTag) {
                entityA = nodeA;
                entityB = nodeB;
            } else if (tagB == enemyTag) {
                entityA = nodeB;
                entityB = nodeA;
            }

            if (entityB->getTag() == polylineTag) {
                if (contact.getContactData()->normal.y > 0) {
                    auto enemy = (Enemy*)entityA;
                    enemy->_canJump = true;
                    return true;
                } else {
                    return false;
                }
            }
            if (entityB->getTag() == groundTag) {
                auto enemy = (Enemy*)entityA;
                enemy->_canJump = true;
            }
            //其他
        }
        // player相关
        if (tagA == playerTag || tagB == playerTag) {
            if (nodeA->getTag() == playerTag) {
                entityA = nodeA;
                entityB = nodeB;
            } else if (nodeB->getTag() == playerTag) {
                entityA = nodeB;
                entityB = nodeA;
            }

            if (entityB->getTag() == polylineTag) {
                if (contact.getContactData()->normal.y > 0) {
                    auto player = (Player*)entityA;
                    player->jumpCounts = 2;
                    return true;
                } else {
                    return false;
                }
            }
            if (entityB->getTag() == groundTag) {
                auto player = (Player*)entityA;
                player->jumpCounts = 2;
            }
            //其他
        }
    }
    return true;
}

bool
GameplayScene::onContactBullet(const PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (nodeA && nodeB) {
        Node* entityA;
        Node* entityB;
        auto tagA = nodeA->getTag();
        auto tagB = nodeB->getTag();

        if (tagA == bulletTag || tagB == bulletTag) {
            if (tagA == bulletTag) {
                entityA = nodeA;
                entityB = nodeB;
            } else if (tagB == bulletTag) {
                entityA = nodeB;
                entityB = nodeA;
            }

            if (entityB->getTag() == enemyTag) {
                ParticleSystem* ps = ParticleExplosion::createWithTotalParticles(5);
                ps->setTexture(Director::getInstance()->getTextureCache()->addImage(
                    "gameplayscene/smallOrb000.png"));

                //留空，粒子效果添加存在疑似相对坐标的问题，暂不知道如何解决

                // auto pos = mapLayer->convertToNodeSpace(entityA->getPosition());
                // auto pos = mapLayer->convertToWorldSpace(entityA->getPosition());
                // ps->setPosition(pos);
                ps->setPosition(entityA->getPosition());
                // this->getChildByName("mapLayer")->addChild(ps);
                entityB->getParent()->addChild(ps);
                // this->addChild(ps);

                auto enemy = (Enemy*)entityB;
                enemy->decreaseHp(entityB);
                entityA->removeFromParentAndCleanup(true); //移除子弹
            }
            //其他
        }
    }
    return true;
}

void
GameplayScene::initCtrlPanel()
{
    controlPanel = Layer::create();
    p1ControlPanel = Layer::create();
    p2ControlPanel = Layer::create();
    this->addChild(controlPanel);
    this->addChild(p1ControlPanel);
    this->addChild(p2ControlPanel);
    controlPanel->setVisible(true);
    p1ControlPanel->setVisible(true);
    p2ControlPanel->setVisible(false);

    auto characterTagList = gameData->getOnStageCharacterTagList();
    auto p1Character = gameData->getCharacterByTag(characterTagList[0]);
    auto p2Character = gameData->getCharacterByTag(characterTagList[1]);

    //返回按钮
    auto setButton = Button::create("CloseNormal.png");
    setButton->setPosition(Vec2(visibleSize.width * 0.060, visibleSize.height * 0.920));
    setButton->setScale(1.5);
    setButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            auto layer = SettingsLayer::create("GameplayScene");
            this->addChild(layer, 1000); //最大优先级
        }
    });
    controlPanel->addChild(setButton);

    // dash
    auto dashButton = Button::create("gameplayscene/dashButton.png");
    dashButton->setPosition(Vec2(visibleSize.width * 0.920, visibleSize.height * 0.080));
    dashButton->setScale(0.6);
    dashButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        switch (type) {
            case Widget::TouchEventType::BEGAN:
                curPlayer->playerDash();
                break;
            case Widget::TouchEventType::MOVED:
                break;
            case Widget::TouchEventType::ENDED:
                break;
            case Widget::TouchEventType::CANCELED:
                break;
            default:
                break;
        }
    });
    controlPanel->addChild(dashButton);

    //血条
    auto lifeBar = Sprite::create("gameplayscene/lifeBar.png");
    lifeBar->setAnchorPoint(Size(0, 0));
    lifeBar->setPosition(Vec2(visibleSize.width * 0.090, visibleSize.height * 0.920));
    controlPanel->addChild(lifeBar);

    //切换攻击方式
    auto switchAttackTypeButton = Button::create("gameplayscene/switchAttackType.png");
    switchAttackTypeButton->setPosition(
        Vec2(visibleSize.width * 0.840, visibleSize.height * 0.080));
    switchAttackTypeButton->setScale(1.5);
    switchAttackTypeButton->addTouchEventListener(
        [this](Ref* pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                stopAttackType(curPlayer->currentAttackType);
                if (curPlayer->currentAttackType == curPlayer->type1.tag) {
                    changeAttackType(curPlayer->type2.tag);
                } else {
                    changeAttackType(curPlayer->type1.tag);
                }
            }
        });
    controlPanel->addChild(switchAttackTypeButton);

    //切换角色
    auto p1SwitchCharacterButton = Button::create(p1Character.circularAvatar);
    p1SwitchCharacterButton->setPosition(
        Vec2(visibleSize.width * 0.060, visibleSize.height * 0.820));
    p1SwitchCharacterButton->setScale(0.2);
    p1SwitchCharacterButton->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        switch (type) {
            case Widget::TouchEventType::BEGAN:
                //切换前进行的一些处理
                p2ControlPanel->setVisible(true);
                p1ControlPanel->setVisible(false);
                stopAttackType(p1Player->currentAttackType);

                //将当前角色切换为p2
                mapLayer->addChild(p2Player);
                curPlayer = p2Player;
                p2Player->getPhysicsBody()->setVelocity(p1Player->getPhysicsBody()->getVelocity());
                p2Player->setPosition(p1Player->getPosition());
                changeAttackType(p2Player->currentAttackType);

                mapLayer->removeChild(p1Player, false);
                break;
            default:
                break;
        }
    });
    p1ControlPanel->addChild(p1SwitchCharacterButton);

    auto p2SwitchCharacterButton = Button::create(p2Character.circularAvatar);
    p2SwitchCharacterButton->setPosition(
        Vec2(visibleSize.width * 0.060, visibleSize.height * 0.820));
    p2SwitchCharacterButton->setScale(0.2);
    p2SwitchCharacterButton->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::BEGAN) {
            //切换前进行的一些处理
            p2ControlPanel->setVisible(false);
            p1ControlPanel->setVisible(true);
            stopAttackType(p2Player->currentAttackType);

            //将当前角色切换为p1
            mapLayer->addChild(p1Player);
            curPlayer = p1Player;
            p1Player->getPhysicsBody()->setVelocity(p2Player->getPhysicsBody()->getVelocity());
            p1Player->setPosition(p2Player->getPosition());
            changeAttackType(p1Player->currentAttackType);

            mapLayer->removeChild(p2Player, false);
        }
    });
    p2ControlPanel->addChild(p2SwitchCharacterButton);

    /*
    每一个道具按钮的name将和一种道具的tag相对应，按钮在初始化时指定唯一的name，
    在监听器中将传入的ref类型指针转换成button类型指针，然后通过按钮指针取得按钮的name，
    这样就完成了道具tag标签的传递。

    也可以不使用Button控件，而使用其他控件，例如使用TableView控件，但需要重载大量函数。
    显式创建按钮可以直接对相应按钮进行管理，比较适合界面元素固定的场合。
    */
    int increment = 0;
    auto p1ItemList = gameData->getCharacterItemList(characterTagList[0]);
    for (int i = 0; i < p1ItemList.size(); i++) {
        Button* item = Button::create(p1ItemList[i].icon);
        item->setScale(1.5);
        //区分tag和name，在cocos2dx中的tag是int整型的，而我们游戏上的tag则是string类型
        item->setName(p1ItemList[i].tag);
        item->setPosition(Vec2(visibleSize.width * 0.330 + increment, visibleSize.height * 0.080));
        item->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                auto button = (Button*)pSender;
                useItem(p1Player, button->getName());
            }
        });
        p1ControlPanel->addChild(item);
        increment = increment + item->getContentSize().width + 50;
    }

    increment = 0;
    auto p2ItemList = gameData->getCharacterItemList(characterTagList[1]);
    for (int i = 0; i < p2ItemList.size(); i++) {
        Button* item = Button::create(p2ItemList[i].icon);
        item->setScale(1.5);
        item->setName(p2ItemList[i].tag);
        item->setPosition(Vec2(visibleSize.width * 0.330 + increment, visibleSize.height * 0.080));
        item->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                auto button = (Button*)pSender;
                useItem(p2Player, button->getName());
            }
        });
        p2ControlPanel->addChild(item);
        increment = increment + item->getContentSize().width + 50;
    }

    //符卡
    increment = 0;
    auto p1SpellCardList = gameData->getCharacterSpellCardList(characterTagList[0]);
    for (int i = 0; i < p1SpellCardList.size(); i++) {
        Button* spellCard = Button::create(p1SpellCardList[i].icon);
        spellCard->setScale(1.5);
        spellCard->setName(p1SpellCardList[i].tag);
        spellCard->setPosition(
            Vec2(visibleSize.width * 0.550 + increment, visibleSize.height * 0.080));
        spellCard->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                auto button = (Button*)pSender;
                useSpellCard(p1Player, button->getName());
            }
        });
        p1ControlPanel->addChild(spellCard);
        increment = increment + spellCard->getContentSize().width + 50;
    }

    increment = 0;
    auto p2SpellCardList = gameData->getCharacterSpellCardList(characterTagList[1]);
    for (int i = 0; i < p2SpellCardList.size(); i++) {
        Button* spellCard = Button::create(p2SpellCardList[i].icon);
        spellCard->setScale(1.5);
        spellCard->setName(p2SpellCardList[i].tag);
        spellCard->setPosition(
            Vec2(visibleSize.width * 0.550 + increment, visibleSize.height * 0.080));
        spellCard->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                auto button = (Button*)pSender;
                useSpellCard(p2Player, button->getName());
            }
        });
        p2ControlPanel->addChild(spellCard);
        increment = increment + spellCard->getContentSize().width + 50;
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameplayScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GameplayScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameplayScene::onTouchEnded, this);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool
GameplayScene::onTouchBegan(Touch* touch, Event* event)
{
    auto location = touch->getLocation();
    auto body = curPlayer->getPhysicsBody();

    if (location.x > visibleSize.width / 2 && location.y < visibleSize.height / 2) {
        //触碰右下屏
        // curPlayer->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRunRight));
        curPlayer->setScaleX(1); //人物翻转
        curPlayer->playerDirection = "right";
        curPlayer->schedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
    }

    else if (location.x < visibleSize.width / 2 && location.y < visibleSize.height / 2) {
        //触碰左下屏
        // curPlayer->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRunLeft));
        curPlayer->setScaleX(-1); //人物翻转
        curPlayer->playerDirection = "left";
        curPlayer->schedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
    }

    else if (location.y >= visibleSize.height / 2) {
        //触碰上半屏
        curPlayer->playerJump();
    }
    return true;
}

void
GameplayScene::onTouchMoved(Touch* touch, Event* event)
{
    auto location = touch->getLocation();
    auto visibleSize = Director::getInstance()->getVisibleSize();

    if (location.y >= visibleSize.height / 2) {
        if (curPlayer->isScheduled(CC_SCHEDULE_SELECTOR(Player::playerRun))) {
            curPlayer->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
        }
    }
}

void
GameplayScene::onTouchEnded(Touch* touch, Event* event)
{
    auto location = touch->getLocation();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (curPlayer->isScheduled(CC_SCHEDULE_SELECTOR(Player::playerRun))) {
        curPlayer->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
    }
    //自动减速还没有做
}

void
GameplayScene::initCharacter()
{
    TMXObjectGroup* temp = _map->getObjectGroup("player");
    auto ts = temp->getObject("birthPoint");
    // log("%s", Value(ts).getDescription().c_str());

    float x = ts["x"].asFloat();
    float y = ts["y"].asFloat();

    auto characterTagList = gameData->getOnStageCharacterTagList();
    p1Player = Player::create(characterTagList[0]);
    p2Player = Player::create(characterTagList[1]);
    p1Player->setPosition(x, y);
    p2Player->setPosition(x, y);
    p1Player->retain();
    p2Player->retain();

    curPlayer = p1Player;
    curPlayer->setName("curPlayer");
    mapLayer->addChild(p1Player);

    /*changeAttackType(p1Player->currentAttackType);*/
}

void
GameplayScene::initCamera()
{
    auto mapSize = _map->getMapSize();
    auto mapTileSize = _map->getTileSize();
    camera = Sprite::create();
    camera->setPosition(curPlayer->getPosition());
    this->addChild(camera);
    auto follow = Follow::create(camera, Rect(0, 0, mapSize.width * mapTileSize.width,
                                              mapSize.height * mapTileSize.height - 50));
    mapLayer->runAction(follow);

    // camera->schedule(CC_SCHEDULE_SELECTOR(GameplayScene::moveCamera));
    // this->scheduleUpdate();
}

// void GameplayScene::moveCamera(float dt)
//{
//	//	Player* p = (Player *)this->getChildByName("curPlayer");
//
//	Vec2 poi = curPlayer->getPosition();
//	//Vec2 poi = p->getPosition();
//	camera->setPosition(poi.x + 100, poi.y + 70);
//}

void
GameplayScene::initLauncher()
{
    TMXObjectGroup* group = _map->getObjectGroup("launcher");
    auto objects = group->getObjects();

    for (auto v : objects) {
        auto dict = v.asValueMap();
        if (dict.size() == 0)
            continue;
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();

        auto _launcher = Sprite::create("CloseNormal.png");
        _launcher->setPosition(x, y);
        mapLayer->addChild(_launcher); //不要忘记addChild
        auto fe = FirstEmitter::create(_launcher);
        mapLayer->addChild(fe);
        fe->schedule(CC_SCHEDULE_SELECTOR(FirstEmitter::createBullet), 6);
    }

    //以后将利用自动批绘制
    //创建BatchNode节点，成批渲染子弹
    bulletBatchNode = SpriteBatchNode::create("gameplayscene/bullet1.png");
    mapLayer->addChild(bulletBatchNode);
}

void
GameplayScene::changeAttackType(const std::string& startType)
{
    if (startType == "reimu focus attack 1") {
        //留空，暂时使用主场景的发射子弹函数
        this->schedule(CC_SCHEDULE_SELECTOR(GameplayScene::ShootBullet), 0.5f);

    } else if (startType == "reimu focus attack 2") {

    } else if (startType == "reimu split attack 1") {

    } else if (startType == "reimu split attack 2") {

    } else if (startType == "marisa focus attack 1") {

    } else if (startType == "marisa focus attack 2") {

    } else if (startType == "marisa split attack 1") {

    } else if (startType == "marisa split attack 2") {
        this->schedule(CC_SCHEDULE_SELECTOR(GameplayScene::ShootBullet), 0.2f);
    }

    curPlayer->currentAttackType = startType;
}

void
GameplayScene::stopAttackType(const std::string& stopType)
{
    if (stopType == "reimu focus attack 1") {
        this->unschedule(CC_SCHEDULE_SELECTOR(GameplayScene::ShootBullet));
    } else if (stopType == "reimu focus attack 2") {

    } else if (stopType == "reimu split attack 1") {

    } else if (stopType == "reimu split attack 2") {

    } else if (stopType == "marisa focus attack 1") {

    } else if (stopType == "marisa focus attack 2") {

    } else if (stopType == "marisa split attack 1") {

    } else if (stopType == "marisa split attack 2") {
        this->unschedule(CC_SCHEDULE_SELECTOR(GameplayScene::ShootBullet));
    }
}

//用缓存的方法创建子弹，并初始化子弹的运动和运动后的事件
void
GameplayScene::ShootBullet(float dt)
{
    Size winSize = Director::getInstance()->getWinSize();
    auto playerPos = curPlayer->getPosition();
    // auto playerPos = _launcher->getPosition();//改为发射器发射
    //从缓存中创建子弹
    auto spritebullet = Sprite::createWithTexture(bulletBatchNode->getTexture());
    spritebullet->setTag(bulletTag);

    auto spritebullet2 = Sprite::createWithTexture(bulletBatchNode->getTexture());
    spritebullet2->setTag(bulletTag);

    //将创建好的子弹添加到BatchNode中进行批次渲染
    bulletBatchNode->addChild(spritebullet);
    bulletBatchNode->addChild(spritebullet2);

    //给创建好的子弹添加刚体
    do {
        auto _body = PhysicsBody::createBox(spritebullet->getContentSize());
        _body->setRotationEnable(false);
        _body->setGravityEnable(false);

        _body->setContactTestBitmask(bulletCategory);
        _body->setCollisionBitmask(enemyCategory);
        _body->setContactTestBitmask(enemyCategory);
        spritebullet->setPhysicsBody(_body);
    } while (0);

    do {
        auto _body = PhysicsBody::createBox(spritebullet->getContentSize());
        _body->setRotationEnable(false);
        _body->setGravityEnable(false);

        _body->setCategoryBitmask(bulletCategory);
        _body->setCollisionBitmask(enemyCategory);
        _body->setContactTestBitmask(enemyCategory);

        spritebullet2->setPhysicsBody(_body);
    } while (0);

    //将创建好的子弹添加到容器
    vecBullet.pushBack(spritebullet);
    vecBullet.pushBack(spritebullet2);

    Point bulletPos = (Point(playerPos.x, playerPos.y));

    spritebullet->setPosition(bulletPos);
    spritebullet2->setPosition(Point(playerPos.x, playerPos.y - 30));

    float realFlyDuration = 1.0;
    //子弹运行的距离和时间
    // auto actionMove = MoveBy::create(realFlyDuration, Point(0,winSize.height));
    // auto actionMove2 = MoveBy::create(realFlyDuration, Point(0,-winSize.height));
    auto actionMove = MoveBy::create(realFlyDuration, Point(winSize.width, 0));
    auto fire1 = actionMove;
    auto actionMove2 = MoveBy::create(realFlyDuration, Point(winSize.width, 0));
    auto fire2 = actionMove2;

    if (curPlayer->playerDirection.compare("right")) {
        fire1 = actionMove->reverse();
        fire2 = actionMove2->reverse();
    }

    //子弹执行完动作后进行函数回调，调用移除子弹函数
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(GameplayScene::removeBullet, this));

    //子弹开始跑动
    Sequence* sequence = Sequence::create(fire1, actionDone, NULL);
    Sequence* sequence2 = Sequence::create(fire2, actionDone, NULL);

    spritebullet->runAction(sequence);
    spritebullet2->runAction(sequence2);
}

//移除子弹，将子弹从容器中移除，同时也从SpriteBatchNode中移除
void
GameplayScene::removeBullet(Node* pNode)
{
    if (NULL == pNode) {
        return;
    }
    Sprite* bullet = (Sprite*)pNode;
    this->bulletBatchNode->removeChild(bullet, true);
    vecBullet.eraseObject(bullet);
}

void
GameplayScene::initEnemy()
{
    TMXObjectGroup* group = _map->getObjectGroup("enemy");
    auto objects = group->getObjects();
    // Value objectsVal = Value(objects);
    // log("%s", objectsVal.getDescription().c_str());

    for (auto v : objects) {
        auto dict = v.asValueMap();
        if (dict.size() == 0)
            continue;

        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();

        //留空，创建敌人时，需要从地图中的对象上读出tag
        Enemy* _enemy = Enemy::create("Enemy");
        _enemy->setPosition(x, y);
        mapLayer->addChild(_enemy);
        _enemy->startSchedule(curPlayer);
    }
}

//初始化监听器，手动指定优先级
void
GameplayScene::initListener()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();

    auto listener = EventListenerPhysicsContact::create();
    listener->onContactBegin = CC_CALLBACK_1(GameplayScene::onContactGround, this);
    // dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    dispatcher->addEventListenerWithFixedPriority(listener, 20);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameplayScene::onContactBullet, this);
    dispatcher->addEventListenerWithFixedPriority(contactListener, 10);

    // auto e = Enemy::create();
    // auto contactListener = EventListenerPhysicsContact::create();
    // contactListener->onContactBegin = std::bind(&Enemy::onContactBullet,e);
    ////dispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    // dispatcher->addEventListenerWithFixedPriority(contactListener, 10);
}

void
GameplayScene::update(float dt)
{
    Vec2 poi = curPlayer->getPosition();
    camera->setPosition(poi.x + 100, poi.y + 70); //移动摄像机

    //留空，应该在主界面里更新角色状态，例如自动切换下落动画，而不应使用预设的动画播放顺序

    //留空，更新界面状态

    //回复dash次数
    if (p1Player->dashCounts < 2) {
        if (p1Player->isScheduled(CC_SCHEDULE_SELECTOR(Player::regainDashCounts))) {
            ;
        } else {
            p1Player->scheduleOnce(CC_SCHEDULE_SELECTOR(Player::regainDashCounts), 3.0f);
        }
    }
    if (p2Player->dashCounts < 2) {
        if (p2Player->isScheduled(CC_SCHEDULE_SELECTOR(Player::regainDashCounts))) {
            ;
        } else {
            p2Player->scheduleOnce(CC_SCHEDULE_SELECTOR(Player::regainDashCounts), 3.0f);
        }
    }
}

void
GameplayScene::useItem(Player*& player, const std::string& itemTag)
{
    if (itemTag == "I1") {
        curPlayer->playerJump();
    } else if (itemTag == "I2") {
        curPlayer->playerJump();
    } else if (itemTag == "I3") {

    } else if (itemTag == "I4") {
        curPlayer->playerJump();
    } else if (itemTag == "I5") {

    } else if (itemTag == "I6") {

    } else if (itemTag == "I7") {

    } else if (itemTag == "I8") {

    } else if (itemTag == "I9") {

    } else if (itemTag == "I10") {

    } else if (itemTag == "I11") {

    } else if (itemTag == "I12") {
    }
}

void
GameplayScene::useSpellCard(Player*& player, const std::string& cardTag)
{
    if (cardTag == "C1") {
        curPlayer->playerJump();
    } else if (cardTag == "C2") {
        curPlayer->playerDash();
    } else if (cardTag == "C3") {
        curPlayer->playerJump();
    } else if (cardTag == "C4") {
        curPlayer->playerDash();
    } else if (cardTag == "C5") {
        curPlayer->playerJump();
    }
}