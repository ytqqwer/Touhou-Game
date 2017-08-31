﻿#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/GameplayScene.h"
#include "GameplayScene/CtrlPanel/CtrlPanelLayer.h"
#include "GameplayScene/Elevator.h"
#include "GameplayScene/Emitters/Bullet.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/EventFilterManager.h"
#include "GameplayScene/LoadingLayer.h"
#include "GameplayScene/Player/Player.h"
#include "GameplayScene/common.h"
#include "Layers/ConversationLayer.h"
#include "Layers/SettingsLayer.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#define PTM_RATIO 1

const std::string GameplayScene::TAG{ "GameplayScene" };

void
GameplayScene::onEnter()
{
    Scene::onEnter();
}

void
GameplayScene::onEnterTransitionDidFinish()
{
    Scene::onEnterTransitionDidFinish();

    testEventFilterManager();
}

void
GameplayScene::onExit()
{
    Scene::onExit();
}

void
GameplayScene::cleanup()
{
    Scene::cleanup();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    _eventFilterMgr->removeAllEventFilters();
}

bool
GameplayScene::init()
{
    if (!Scene::init()) {
        return false;
    }
    //初始化子弹素材
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/bullet1.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/bullet2.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/bullet3.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/laser1.plist");

    //设置背景音乐
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playBackgroundMusic("gameplayscene/bgm001.mp3",
                                                          true); //开启循环

    gameData = GameData::getInstance();
    visibleSize = Director::getInstance()->getVisibleSize();

    this->initWithPhysics();                                                //初始化物理世界
    Vect gravity(0, -gameGravity);                                          //游戏场景的重力
    this->getPhysicsWorld()->setGravity(gravity);                           //设置重力
    this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); //调试模式看包围盒

    // 用于支持符卡 buf 效果的 EventFilterManager
    this->_eventFilterMgr = EventFilterManager::create();
    this->_eventFilterMgr->retain();

    auto loadingLayer = LoadingLayer::create();
    this->addChild(loadingLayer);

    // 不要在场景初始化的时候执行任何需要获取当前运行场景的代码
    // 初始化的时候场景还没有被替换上
    // 否则就需要使用下列代码延迟初始化动作
    // 将初始化动作延迟执行，保证在后续初始化工作中当前scene已经准备完毕
    // std::function<void(Ref*)> delayInit = [&](Ref*) {
    //;
    //};
    // auto init = CallFuncN::create(delayInit);
    // this->runAction(Sequence::create(DelayTime::create(1.2), init, NULL));

    // 初始化地图背景层
    this->initBackGround();
    // 初始化地图层
    this->initMap();
    // 初始化控制面板
    this->initCtrlPanel();
    // 加载角色
    this->initCharacter();
    // 加载首个区域
    this->initArea();
    // 加载摄像机
    this->initCamera();
    // 加载物理碰撞监听器
    this->initPhysicsContactListener();
    // 加载自定义事件监听器
    this->initCustomEventListener();
    // 启动帧调度器
    this->scheduleUpdate();

    return true;
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

    scheduleOnce(
        [this](float dt) {
            EventCustom event("loading_event");
            LoadingInfo loadingInfo;
            loadingInfo.progress = 10;
            loadingInfo.information = "加载背景";
            event.setUserData((void*)&loadingInfo);
            event.retain();
            _eventDispatcher->dispatchEvent(&event);
        },
        0.1, "bg");
}

void
GameplayScene::initMap()
{
    mapLayer = Layer::create();
    _map = TMXTiledMap::create("gameplayscene/test.tmx");
    //设置地图大小的倍率
    _map->setScale(1.0f);
    mapLayer->addChild(_map);
    mapLayer->setName("mapLayer");
    this->addChild(mapLayer, 0);

    //创建静态刚体墙
    createPhysical(1);

    scheduleOnce(
        [this](float dt) {
            EventCustom event("loading_event");
            LoadingInfo loadingInfo;
            loadingInfo.progress = 30;
            loadingInfo.information = "加载地图";
            event.setUserData((void*)&loadingInfo);
            _eventDispatcher->dispatchEvent(&event);
        },
        0.4, "map");
}

//创建静态刚体，接受参数设置刚体大小倍率
bool
GameplayScene::createPhysical(float scale)
{
    // 找出阻挡区域所在的层
    TMXObjectGroup* group = _map->getObjectGroup("physics");
    auto objects = group->getObjects();
    // 在控制台输出对象信息
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
            int polygonSize = polygon_points.size();
            polygon_points.resize(polygonSize);
            polygonSize--;

            for (auto obj : polygon_points) {
                // 相对于起始点的偏移
                float offx = obj.asValueMap()["x"].asFloat() * scale;
                float offy = obj.asValueMap()["y"].asFloat() * scale;

                points[polygonSize] = Vec2((x + offx) / PTM_RATIO, (y - offy) / PTM_RATIO);
                polygonSize--;
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
            sprite->setTag(polygonCategoryTag);
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
            sprite->setTag(polylineCategoryTag);
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
            sprite->setTag(groundCategoryTag);
            sprite->setPosition(x + width / 2.0f, y + height / 2.0f);
            sprite->setPhysicsBody(_pBody);
            mapLayer->addChild(sprite);
        }
    }
    return true;
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

    curPlayer->changeAttackType(p1Player->currentAttackType);

    scheduleOnce(
        [this](float dt) {

            EventCustom event("loading_event");
            LoadingInfo loadingInfo;
            loadingInfo.progress = 20;
            loadingInfo.information = "加载角色";
            event.setUserData((void*)&loadingInfo);
            _eventDispatcher->dispatchEvent(&event);
        },
        0.6, "character");
}

void
GameplayScene::initCtrlPanel()
{
    controlPanel = CtrlPanelLayer::create();

    this->addChild(controlPanel);

    scheduleOnce(
        [this](float dt) {
            EventCustom event("loading_event");
            LoadingInfo loadingInfo;
            loadingInfo.progress = 10;
            loadingInfo.information = "加载控制面板";
            event.setUserData((void*)&loadingInfo);
            event.retain();
            _eventDispatcher->dispatchEvent(&event);
        },
        0.7, "controlPanel");
}

void
GameplayScene::initArea()
{
    auto playerPos = curPlayer->getPosition();
    auto areas = _map->getObjectGroup("area")->getObjects();
    ;
    for (auto& v : areas) {
        auto dict = v.asValueMap();
        if (dict.size() == 0)
            continue;
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        curArea.setRect(x, y, width, height);
        if (curArea.containsPoint(playerPos)) {
            // 加载发射器
            initLauncher();
            // 加载电梯
            initElevator();

            // 加载敌人
            initEnemy();
            // 加载事件
            initEvent();
            break;
        }
    }
}

void
GameplayScene::initCamera()
{
    auto mapSize = _map->getMapSize();
    auto mapTileSize = _map->getTileSize();
    camera = Sprite::create();
    camera->setPosition(curPlayer->getPosition());
    this->addChild(camera);

    auto cameraFollow = Follow::create(camera, curArea);
    cameraFollow->setTag(cameraTag);
    mapLayer->runAction(cameraFollow);

    scheduleOnce(
        [this](float dt) {
            EventCustom event("loading_event");
            LoadingInfo loadingInfo;
            loadingInfo.progress = 10;
            loadingInfo.information = "加载摄像机";
            event.setUserData((void*)&loadingInfo);
            _eventDispatcher->dispatchEvent(&event);
        },
        0.8, "camera");
}

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

        if (curArea.containsPoint(Vec2(x, y))) {
            auto _launcher = Sprite::create("CloseNormal.png");
            _launcher->setPosition(x, y);
            mapLayer->addChild(_launcher); //不要忘记addChild

            auto fe = Emitter::create((Node**)(&curPlayer));
            _launcher->addChild(fe);
            // fe->playStyle(StyleType::SCATTER);
            fe->playStyle(StyleType::ODDEVEN);

            launcherList.pushBack(_launcher);
        }
    }
}

void
GameplayScene::initElevator()
{
    TMXObjectGroup* group = _map->getObjectGroup("elevator");
    auto objects = group->getObjects();

    for (auto v : objects) {
        auto dict = v.asValueMap();
        if (dict.size() == 0)
            continue;
        //读取起始点
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();

        if (curArea.containsPoint(Vec2(x, y))) {
            auto polyline_points = dict["polylinePoints"].asValueVector();
            vector<Vec2> vertex;
            for (auto obj : polyline_points) {
                float offx = obj.asValueMap()["x"].asFloat();
                float offy = obj.asValueMap()["y"].asFloat();
                vertex.push_back(Vec2((x + offx), (y - offy)));
            }
            auto seq = Sequence::create(DelayTime::create(1.0f), nullptr);

            for (auto obj : vertex) {
                seq = Sequence::createWithTwoActions(seq, MoveTo::create(1.0f, obj));
            }

            // Sequence::reverse()方法不支持moveTo
            auto seq_reverse = Sequence::create(DelayTime::create(1.0f), nullptr);
            vector<Vec2>::reverse_iterator it = vertex.rbegin();
            while (it != vertex.rend()) {
                seq_reverse =
                    Sequence::createWithTwoActions(seq_reverse, MoveTo::create(1.0f, *it));
                ++it;
            }

            auto _elevator = Elevator::create();
            _elevator->setPosition(x, y);
            mapLayer->addChild(_elevator);
            _elevator->runAction(
                RepeatForever::create(Sequence::create(seq, seq_reverse, nullptr)));

            elevatorList.pushBack(_elevator);
        }
    }
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

        if (curArea.containsPoint(Vec2(x, y))) {
            std::string tag = dict["tag"].asString();
            Enemy* _enemy = Enemy::create(tag);
            _enemy->setPosition(x, y);
            mapLayer->addChild(_enemy);
            _enemy->setTarget(curPlayer);
            _enemy->switchMode();
            enemyList.pushBack(_enemy);
        }
    }
}

void
GameplayScene::initEvent()
{
    TMXObjectGroup* group = _map->getObjectGroup("event");
    auto objects = group->getObjects();
    for (auto v : objects) {
        auto dict = v.asValueMap();
        if (dict.size() == 0)
            continue;

        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();

        if (curArea.containsPoint(Vec2(x, y))) {
            std::string tag = dict["tag"].asString();
            auto _event = Sprite::create("gameplayscene/unknownEvent.png");
            _event->setPosition(x, y);
            _event->setName(tag);
            _event->setTag(eventCategoryTag);

            auto body = PhysicsBody::createBox(Size(15, 25));
            body->setGravityEnable(false);
            body->setRotationEnable(false);
            body->setCategoryBitmask(eventCategory);
            body->setCollisionBitmask(0);
            body->setContactTestBitmask(playerCategory);
            _event->setPhysicsBody(body);
            mapLayer->addChild(_event);

            eventPoint.pushBack(_event);
        }
    }
}

bool
GameplayScene::contactBegin(const PhysicsContact& contact)
{
    auto shapeA = contact.getShapeA();
    auto shapeB = contact.getShapeB();
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (nodeA && nodeB) {
        auto tagA = nodeA->getTag();
        auto tagB = nodeB->getTag();
        // entityA和entityB对nodeA和nodeB进行映射
        Node* entityA;
        Node* entityB;
        PhysicsShape* entityA_shape;
        PhysicsShape* entityB_shape;

        // enemy相关
        if (tagA == enemyCategoryTag || tagB == enemyCategoryTag) {
            if (tagA == enemyCategoryTag) {
                entityA = nodeA;
                entityB = nodeB;
                entityA_shape = shapeA;
                entityB_shape = shapeB;
            } else if (tagB == enemyCategoryTag) {
                entityA = nodeB;
                entityB = nodeA;
                entityA_shape = shapeB;
                entityB_shape = shapeA;
            }

            // 当enemy碰到了折线刚体
            if (entityB->getTag() == polylineCategoryTag) {
                //当冲量方向向上时可以穿过折线刚体
                if (contact.getContactData()->normal.y > 0) {
                    auto enemy = (Enemy*)entityA;
                    enemy->_canJump = true;
                    enemy->curAction = ActionState::Default;
                    return true;
                } else {
                    return false;
                }
            }
            // 当enemy碰到了地面刚体
            else if (entityB->getTag() == groundCategoryTag) {
                //什么也不做
            }
            // 当enemy碰到了子弹
            else if (entityB->getTag() == bulletCategoryTag) {
                ParticleSystem* ps = ParticleExplosion::createWithTotalParticles(5);
                ps->setTexture(Director::getInstance()->getTextureCache()->addImage(
                    "gameplayscene/smallOrb000.png"));

                // cocos2dx的粒子系统有三种位置类型
                mapLayer->addChild(ps);
                ps->setPositionType(ParticleSystem::PositionType::RELATIVE);
                ps->setPosition(entityA->getPosition());

                auto enemy = (Enemy*)entityA;
                auto bullet = (Bullet*)entityB;

                DamageInfo _damageInfo;
                _damageInfo.damage = bullet->getDamage();
                _damageInfo.target = enemy;
                EventCustom event("bullet_hit_enemy");
                event.setUserData((void*)&_damageInfo);
                _eventDispatcher->dispatchEvent(&event);

                entityB->removeFromParentAndCleanup(true); //移除子弹
            }
            // 当enemy站在电梯上
            else if (entityB->getTag() == elevatorCategoryTag) {

                if (contact.getContactData()->normal.y > 0) {
                    auto _enemy = (Enemy*)entityA;
                    auto elevator = (Elevator*)entityB;
                    _enemy->_canJump = true;
                    _enemy->curAction = ActionState::Default;
                    elevator->addPassenger(_enemy);
                    return true;
                } else {
                    return false;
                }
            }

            //其他
        }
        // player相关
        if (tagA == playerCategoryTag || tagB == playerCategoryTag) {
            if (nodeA->getTag() == playerCategoryTag) {
                entityA = nodeA;
                entityB = nodeB;
                entityA_shape = shapeA;
                entityB_shape = shapeB;
            } else if (nodeB->getTag() == playerCategoryTag) {
                entityA = nodeB;
                entityB = nodeA;
                entityA_shape = shapeB;
                entityB_shape = shapeA;
            }

            //当player碰到了折线刚体
            if (entityB->getTag() == polylineCategoryTag) {
                if (contact.getContactData()->normal.y > 0) {
                    auto player = (Player*)entityA;
                    player->jumpCounts = 2;
                    player->curAction = ActionState::Default;
                    return true;
                } else {
                    return false;
                }
            }
            // 当player碰到了地面刚体
            else if (entityB->getTag() == groundCategoryTag) {
                // do nothing
            }
            // 当player碰到了敌人或索敌框
            else if (entityB->getTag() == enemyCategoryTag) {
                // 当player碰到了敌人的索敌框
                if (entityB_shape->getTag() == lockCategoryTag) {
                    auto enemy = (Enemy*)entityB;
                    enemy->curState = EnemyActionMode::Alert;
                    enemy->switchMode();
                }
                // 当player碰到了敌人本身
                else {
                    DamageInfo _damageInfo;
                    _damageInfo.damage = 10;      //碰撞伤害
                    _damageInfo.target = entityA; //受损目标
                    EventCustom event("bullet_hit_player");
                    event.setUserData((void*)&_damageInfo);
                    _eventDispatcher->dispatchEvent(&event);
                }

            }
            // 当player碰到了事件点或者宝箱
            else if (entityB->getTag() == eventCategoryTag) {
                //抛出event的tag
                EventCustom event("trigger_event");
                event.setUserData((void*)entityB->getName().c_str());
                _eventDispatcher->dispatchEvent(&event);
                entityB->removeFromParent();
            }
            // 当player站在电梯上
            else if (entityB->getTag() == elevatorCategoryTag) {

                if (contact.getContactData()->normal.y > 0) {
                    auto player = (Player*)entityA;
                    auto elevator = (Elevator*)entityB;
                    player->jumpCounts = 2;
                    player->curAction = ActionState::Default;
                    elevator->addPassenger(curPlayer);
                    return true;
                } else {
                    return false;
                }
            }
            //其他
        }
    }
    return true;
}

bool
GameplayScene::contactSeparate(const PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (nodeA && nodeB) {
        auto tagA = nodeA->getTag();
        auto tagB = nodeB->getTag();
        Node* entityA;
        Node* entityB;

        // enemy相关
        if (tagA == enemyCategoryTag || tagB == enemyCategoryTag) {
            if (tagA == enemyCategoryTag) {
                entityA = nodeA;
                entityB = nodeB;
            } else if (tagB == enemyCategoryTag) {
                entityA = nodeB;
                entityB = nodeA;
            }

            if (entityB->getTag() == elevatorCategoryTag) {
                auto _enemy = (Enemy*)entityA;
                auto elevator = (Elevator*)entityB;
                elevator->removePassenger(_enemy);
            }
            //其他
        }
        // player相关
        if (tagA == playerCategoryTag || tagB == playerCategoryTag) {
            if (nodeA->getTag() == playerCategoryTag) {
                entityA = nodeA;
                entityB = nodeB;
            } else if (nodeB->getTag() == playerCategoryTag) {
                entityA = nodeB;
                entityB = nodeA;
            }

            if (entityB->getTag() == elevatorCategoryTag) {
                auto elevator = (Elevator*)entityB;
                elevator->removePassenger(curPlayer);
            }
            //其他
        }
    }
    return true;
}

//初始化监听器，手动指定优先级
void
GameplayScene::initPhysicsContactListener()
{
    auto filter = EventListenerPhysicsContact::create();
    filter->onContactBegin = CC_CALLBACK_1(GameplayScene::contactBegin, this);
    filter->onContactSeparate = CC_CALLBACK_1(GameplayScene::contactSeparate, this);
    _eventDispatcher->addEventListenerWithFixedPriority(filter, 50);
}

void
GameplayScene::initCustomEventListener()
{
    _eventDispatcher->addCustomEventListener(
        "left_key_pressed", [this](EventCustom* e) { this->onEventLeftKeyPressed(e); });

    _eventDispatcher->addCustomEventListener(
        "right_key_pressed", [this](EventCustom* e) { this->onEventRightKeyPressed(e); });

    _eventDispatcher->addCustomEventListener(
        "motion_key_released", [this](EventCustom* e) { this->onEventMotionKeyReleased(e); });

    _eventDispatcher->addCustomEventListener(
        "jump_key_pressed", [this](EventCustom* e) { this->onEventJumpKeyPressed(e); });

    _eventDispatcher->addCustomEventListener(
        "dash_key_pressed", [this](EventCustom* e) { this->onEventDashKeyPressed(e); });

    _eventDispatcher->addCustomEventListener(
        "switch_character", [this](EventCustom* e) { this->onEventSwitchCharacter(e); });

    _eventDispatcher->addCustomEventListener(
        "switch_attack_type", [this](EventCustom* e) { this->onEventSwitchAttackType(e); });

    _eventDispatcher->addCustomEventListener(
        "settings_key_pressed", [this](EventCustom* e) { this->onEventSettingsKeyPressed(e); });

    _eventDispatcher->addCustomEventListener("trigger_event",
                                             [this](EventCustom* e) { this->eventHandling(e); });

    _eventDispatcher->addCustomEventListener("conversation_end",
                                             [this](EventCustom* e) { this->nextEvent(); });

    _eventDispatcher->addCustomEventListener("bullet_hit_enemy", [this](EventCustom* e) {
        auto _damageInfo = (DamageInfo*)e->getUserData();
        auto _enemy = (Enemy*)_damageInfo->target;
        _enemy->decreaseHp(_damageInfo->damage);
    });

    _eventDispatcher->addCustomEventListener("bullet_hit_player", [this](EventCustom* e) {
        auto _damageInfo = (DamageInfo*)e->getUserData();
        auto _player = (Player*)_damageInfo->target;

        //暂时代替受损
        auto body = _player->getPhysicsBody();
        auto velocity = body->getVelocity();
        Vec2 impluse = Vec2(0.0f, 300.0f);
        body->applyImpulse(impluse);

        //阻隔
        _eventFilterMgr->addEventFilter(
            [](EventCustom* event) -> void {
                if (event->getEventName() == "bullet_hit_player") {
                    event->stopPropagation();
                }
                log("[EventFilterManager] Propagation stopped");
            },
            1, "bullet_hit_player");

    });

    scheduleOnce(
        [this](float dt) {
            EventCustom event("loading_event");
            LoadingInfo loadingInfo;
            loadingInfo.progress = 20;
            loadingInfo.information = "加载事件";
            event.setUserData((void*)&loadingInfo);
            _eventDispatcher->dispatchEvent(&event);
        },
        1.0, "customEvent");
}

void
GameplayScene::eventHandling(EventCustom* e)
{
    auto eventTag = (char*)e->getUserData();
    eventList = gameData->getEventListByTag(eventTag);
    _curEventIndex = 0;
    nextEvent();
}

void
GameplayScene::nextEvent()
{
    if (_curEventIndex == eventList.size()) {
        return;
    }

    if (eventList[_curEventIndex].eventType == "conversation") {
        auto layer = ConversationLayer::create(eventList[_curEventIndex].conversationTag);
        layer->setPauseNode(mapLayer);
        mapLayer->onExit();
        this->addChild(layer, 1000);
        _curEventIndex++;
        return;
    } else if (eventList[_curEventIndex].eventType == "action") {
        float totalTime = 0;
        while (_curEventIndex < eventList.size()) {
            if (eventList[_curEventIndex].eventType == "action") {
                float delay = eventList[_curEventIndex].delay;
                float duration = eventList[_curEventIndex].duration;
                if (totalTime < delay + duration) {
                    totalTime = delay + duration;
                }
                eventActionHandling(delay, duration);

                _curEventIndex++;
            } else {
                break;
            }
        }
        auto done = CallFuncN::create(CC_CALLBACK_0(GameplayScene::nextEvent, this));
        Sequence* sequence = Sequence::create(DelayTime::create(totalTime), done, NULL);
        this->runAction(sequence);
        return;
    }
}

void
GameplayScene::eventActionHandling(float delay, float duration)
{
    Sequence* sequence;
    if (eventList[_curEventIndex].jump) {
        auto moveBy = MoveBy::create(duration, Point(-200, 0));
        auto jump = CallFuncN::create(CC_CALLBACK_0(Player::playerJump, curPlayer));
        sequence = Sequence::create(DelayTime::create(delay), jump, moveBy, NULL);
    } else {
        auto moveBy = MoveBy::create(duration, Point(300, 0));
        sequence = Sequence::create(DelayTime::create(delay), moveBy, NULL);
    }
    curPlayer->runAction(sequence);
}

void
GameplayScene::onEventLeftKeyPressed(EventCustom*)
{
    curPlayer->playerSprite->setScaleX(-1); //人物翻转
    curPlayer->playerDirection = Direction::LEFT;
    curPlayer->schedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
}

void
GameplayScene::onEventRightKeyPressed(EventCustom*)
{
    curPlayer->playerSprite->setScaleX(1); //人物翻转
    curPlayer->playerDirection = Direction::RIGHT;
    curPlayer->schedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
}

void
GameplayScene::onEventMotionKeyReleased(EventCustom*)
{
    if (curPlayer->isScheduled(CC_SCHEDULE_SELECTOR(Player::playerRun))) {
        curPlayer->unschedule(CC_SCHEDULE_SELECTOR(Player::playerRun));
    }

    //自动减速还没有做
}

void
GameplayScene::onEventJumpKeyPressed(EventCustom*)
{
    curPlayer->playerJump();
}

void
GameplayScene::onEventDashKeyPressed(EventCustom*)
{
    curPlayer->playerDash();
}

void
GameplayScene::onEventSwitchCharacter(EventCustom*)
{
    Player* theOther = nullptr;

    if (curPlayer == p1Player) {
        theOther = p2Player;
    } else {
        theOther = p1Player;
    }

    curPlayer->stopAttackType();
    mapLayer->removeChild(curPlayer, false);

    if (curPlayer->playerDirection == Direction::RIGHT) {
        theOther->playerDirection = Direction::RIGHT;
        theOther->playerSprite->setScaleX(1);
    } else {
        theOther->playerDirection = Direction::LEFT;
        theOther->playerSprite->setScaleX(-1);
    }

    theOther->getPhysicsBody()->setVelocity(curPlayer->getPhysicsBody()->getVelocity());
    theOther->setPosition(curPlayer->getPosition());
    theOther->changeAttackType(theOther->currentAttackType);

    curPlayer = theOther;
    mapLayer->addChild(theOther);
}

void
GameplayScene::onEventSwitchAttackType(EventCustom*)
{
    curPlayer->stopAttackType();
    if (curPlayer->currentAttackType == curPlayer->type1.tag) {
        curPlayer->changeAttackType(curPlayer->type2.tag);
    } else {
        curPlayer->changeAttackType(curPlayer->type1.tag);
    }
}

void
GameplayScene::onEventSettingsKeyPressed(EventCustom*)
{
    auto layer = SettingsLayer::create("GameplayScene");
    layer->setPauseNode(mapLayer);
    mapLayer->onExit();
    this->addChild(layer, 1000);
}

void
GameplayScene::update(float dt)
{
    Vec2 poi = curPlayer->getPosition();
    camera->setPosition(poi.x + 100, poi.y + 70); //移动摄像机

    if (curArea.containsPoint(curPlayer->getPosition())) {
        ;
    } else {

        //移除前一个区域的物件
        for (auto v : enemyList) {
            v->removeFromParent();
        }
        for (auto v : eventPoint) {
            v->removeFromParent();
        }

        for (auto v : launcherList) {
            v->removeFromParent();
        }
        for (auto v : elevatorList) {
            v->removeFromParent();
        }

        initArea();

        mapLayer->stopActionByTag(cameraTag);
        auto cameraFollow = Follow::create(camera, curArea);
        cameraFollow->setTag(cameraTag);
        mapLayer->runAction(cameraFollow);
    }

    //留空，更新界面状态
}
