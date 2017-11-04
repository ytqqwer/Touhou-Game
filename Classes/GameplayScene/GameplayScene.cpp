#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/GameplayScene.h"
#include "GameplayScene/CtrlPanel/CtrlPanelLayer.h"
#include "GameplayScene/Elevator.h"
#include "GameplayScene/Emitters/Bullet.h"
#include "GameplayScene/Emitters/Emitter.h"
#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/EventFilterManager.h"
#include "GameplayScene/EventScriptHanding.h"
#include "GameplayScene/Player/Player.h"
#include "GameplayScene/common.h"

#include "Layers/ConversationLayer.h"
#include "Layers/SettingsLayer.h"

#include "AudioController.h"

#define PTM_RATIO 1

#define PARALLAX_BACK_ZORDER -1
#define PARALLAX_FORE_ZORDER 1
#define CTRLPANEL_LAYER_ZORDER 3
#define SETTING_LAYER_ZORDER 1000

#define MAPLAYER_ZORDER 0

#define MAPLAYER_TMXMAP_ZORDER 0
#define MAPLAYER_CHARACTER_ZORDER 1
#define MAPLAYER_ENEMY_ZORDER 1
#define MAPLAYER_CAMERA_ZORDER -1
#define MAPLAYER_OTHER_ZORDER 2

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
    AudioController::getInstance()->clearMusic();
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
    _eventFilterMgr->removeAllEventFilters();

    AnimationCache::getInstance()->destroyInstance();
}

GameplayScene*
GameplayScene::create(const std::string& map)
{
    GameplayScene* pRet = new (std::nothrow) GameplayScene(map);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

GameplayScene::GameplayScene(const std::string& map)
{
    selectedMap = map;
    visibleSize = Director::getInstance()->getVisibleSize();
}

GameplayScene::~GameplayScene()
{
    delete _eventScriptHanding;
}

bool
GameplayScene::init()
{
    if (!Scene::init()) {
        return false;
    }

    this->initWithPhysics();                                                //初始化物理世界
    Vect gravity(0, -gameGravity);                                          //游戏场景的重力
    this->getPhysicsWorld()->setGravity(gravity);                           //设置重力
    this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); //调试模式看包围盒

    // 用于支持符卡 buf 效果的 EventFilterManager
    this->_eventFilterMgr = EventFilterManager::create();
    this->_eventFilterMgr->retain();

    _eventScriptHanding = new EventScriptHanding(this);

    return true;
}

void
GameplayScene::initBackgroundAndForeground()
{
    backParallaxNode = ParallaxNode::create();
    backParallaxNode->setAnchorPoint(Point::ZERO);
    this->addChild(backParallaxNode, PARALLAX_BACK_ZORDER);

    backParallaxNode->setPosition(visibleSize.width / 2.0, visibleSize.height / 2.0);

    //背景图片之一，锚点为图片中心点
    backgroundPicture = Sprite::create();
    backgroundPicture->setAnchorPoint(Point::ANCHOR_MIDDLE);
    //加入视差结点，子节点位置只和偏移量，比率因子等有关
    backParallaxNode->addChild(backgroundPicture, 0, Vec2(0.06, 0.03),
                               Vec2(visibleSize.width / 2.0, visibleSize.height / 2.0));
}

void
GameplayScene::initMap()
{
    mapLayer = Layer::create();
    _map = TMXTiledMap::create(selectedMap);
    //设置地图大小的倍率
    _map->setScale(1.0f);
    mapLayer->addChild(_map, MAPLAYER_TMXMAP_ZORDER);
    mapLayer->setName("mapLayer");
    this->addChild(mapLayer, MAPLAYER_ZORDER);

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

#define CREATE_AND_ADD_ANIMATION_CACHE(frames, delayPerUnit, key)                                  \
    if (frames.size() > 0) {                                                                       \
        auto animation = Animation::create();                                                      \
        for (auto& v : frames) {                                                                   \
            animation->addSpriteFrameWithFile(v);                                                  \
        }                                                                                          \
        animation->setDelayPerUnit(delayPerUnit);                                                  \
        AnimationCache::getInstance()->addAnimation(animation, key);                               \
    }

void
GameplayScene::initAnimationCache()
{
    //初始化子弹素材
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/bullet1.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/bullet2.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/bullet3.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emitter/bullets/laser1.plist");

    auto characterTags = GameData::getInstance()->getOnStageCharacterTagList();

    std::set<string> enemyTags;
    TMXObjectGroup* group = _map->getObjectGroup("enemy");
    auto objects = group->getObjects();
    for (auto v : objects) {
        auto dict = v.asValueMap();
        if (dict.size() == 0)
            continue;
        std::string tag = dict["tag"].asString();
        enemyTags.insert(tag);
    }

    for (auto& s : characterTags) {
        Character _character = GameData::getInstance()->getCharacterByTag(s);

        CREATE_AND_ADD_ANIMATION_CACHE(_character.standFrame, _character.standFrameDelay,
                                       _character.standAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_character.runFrame, _character.runFrameDelay,
                                       _character.runAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_character.preJumpFrame, _character.preJumpFrameDelay,
                                       _character.preJumpAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_character.jumpFrame, _character.jumpFrameDelay,
                                       _character.jumpAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_character.preFallFrame, _character.preFallFrameDelay,
                                       _character.preFallAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_character.fallFrame, _character.fallFrameDelay,
                                       _character.fallAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_character.dashFrame, _character.dashFrameDelay,
                                       _character.dashAnimationKey);
    }

    for (auto& s : enemyTags) {
        EnemyData _enemy = GameData::getInstance()->getEnemyByTag(s);

        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.standFrame, _enemy.standFrameDelay,
                                       _enemy.standAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.runFrame, _enemy.runFrameDelay,
                                       _enemy.runAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.preJumpFrame, _enemy.preJumpFrameDelay,
                                       _enemy.preJumpAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.jumpFrame, _enemy.jumpFrameDelay,
                                       _enemy.jumpAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.preFallFrame, _enemy.preFallFrameDelay,
                                       _enemy.preFallAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.fallFrame, _enemy.fallFrameDelay,
                                       _enemy.fallAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.dashFrame, _enemy.dashFrameDelay,
                                       _enemy.dashAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.hitFrame, _enemy.hitFrameDelay,
                                       _enemy.hitAnimationKey);
        CREATE_AND_ADD_ANIMATION_CACHE(_enemy.downFrame, _enemy.downFrameDelay,
                                       _enemy.downAnimationKey);
    }

    auto sakuyaAttackA_1 = Animation::create();
    sakuyaAttackA_1->addSpriteFrameWithFile("character/Sakuya/shotAb000.png");
    sakuyaAttackA_1->addSpriteFrameWithFile("character/Sakuya/shotAb001.png");
    sakuyaAttackA_1->addSpriteFrameWithFile("character/Sakuya/shotAb002.png");
    sakuyaAttackA_1->addSpriteFrameWithFile("character/Sakuya/shotAb003.png");
    sakuyaAttackA_1->addSpriteFrameWithFile("character/Sakuya/shotAb004.png");
    sakuyaAttackA_1->addSpriteFrameWithFile("character/Sakuya/shotAb005.png");
    sakuyaAttackA_1->setDelayPerUnit(0.10);
    AnimationCache::getInstance()->addAnimation(sakuyaAttackA_1, "sakuyaAttackA_1");
    auto sakuyaAttackA_2 = Animation::create();
    sakuyaAttackA_2->addSpriteFrameWithFile("character/Sakuya/shotAb006.png");
    sakuyaAttackA_2->addSpriteFrameWithFile("character/Sakuya/shotAb007.png");
    sakuyaAttackA_2->addSpriteFrameWithFile("character/Sakuya/shotAb008.png");
    sakuyaAttackA_2->addSpriteFrameWithFile("character/Sakuya/shotAb009.png");
    sakuyaAttackA_2->addSpriteFrameWithFile("character/Sakuya/shotAb010.png");
    sakuyaAttackA_2->setDelayPerUnit(0.10);
    AnimationCache::getInstance()->addAnimation(sakuyaAttackA_2, "sakuyaAttackA_2");

    auto sakuyaAttackB_1 = Animation::create();
    sakuyaAttackB_1->addSpriteFrameWithFile("character/Sakuya/shotAa000.png");
    sakuyaAttackB_1->addSpriteFrameWithFile("character/Sakuya/shotAa001.png");
    sakuyaAttackB_1->addSpriteFrameWithFile("character/Sakuya/shotAa002.png");
    sakuyaAttackB_1->addSpriteFrameWithFile("character/Sakuya/shotAa003.png");
    sakuyaAttackB_1->addSpriteFrameWithFile("character/Sakuya/shotAa004.png");
    sakuyaAttackB_1->addSpriteFrameWithFile("character/Sakuya/shotAa005.png");
    sakuyaAttackB_1->setDelayPerUnit(0.10);
    AnimationCache::getInstance()->addAnimation(sakuyaAttackB_1, "sakuyaAttackB_1");
    auto sakuyaAttackB_2 = Animation::create();
    sakuyaAttackB_2->addSpriteFrameWithFile("character/Sakuya/shotAa006.png");
    sakuyaAttackB_2->addSpriteFrameWithFile("character/Sakuya/shotAa007.png");
    sakuyaAttackB_2->addSpriteFrameWithFile("character/Sakuya/shotAa008.png");
    sakuyaAttackB_2->addSpriteFrameWithFile("character/Sakuya/shotAa009.png");
    sakuyaAttackB_2->addSpriteFrameWithFile("character/Sakuya/shotAa010.png");
    sakuyaAttackB_2->setDelayPerUnit(0.10);
    AnimationCache::getInstance()->addAnimation(sakuyaAttackB_2, "sakuyaAttackB_2");

    auto sakuyaUseSpellCard = Animation::create();
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa000.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa001.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa002.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa003.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa004.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa005.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa006.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa007.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa008.png");
    sakuyaUseSpellCard->addSpriteFrameWithFile("character/Sakuya/spellDa009.png");
    sakuyaUseSpellCard->setDelayPerUnit(0.10);
    AnimationCache::getInstance()->addAnimation(sakuyaUseSpellCard, "sakuyaUseSpellCard");

    //////////////////////////////////////

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("enemy/Stump.plist");
    auto stumpStand = Animation::create();
    stumpStand->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_stand.png"));
    stumpStand->setDelayPerUnit(0.50);
    AnimationCache::getInstance()->addAnimation(stumpStand, "stumpStand");
    auto stumpMove = Animation::create();
    stumpMove->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_move_1.png"));
    stumpMove->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_move_2.png"));
    stumpMove->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_move_3.png"));
    stumpMove->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_move_4.png"));
    stumpMove->setDelayPerUnit(0.20);
    AnimationCache::getInstance()->addAnimation(stumpMove, "stumpMove");
    auto stumpHit = Animation::create();
    stumpHit->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_hit.png"));
    stumpHit->setDelayPerUnit(0.60);
    AnimationCache::getInstance()->addAnimation(stumpHit, "stumpHit");
    auto stumpDown = Animation::create();
    stumpDown->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_down_1.png"));
    stumpDown->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_down_2.png"));
    stumpDown->addSpriteFrame(
        SpriteFrameCache::getInstance()->getSpriteFrameByName("stump_down_3.png"));
    stumpDown->setDelayPerUnit(0.10);
    AnimationCache::getInstance()->addAnimation(stumpDown, "stumpDown");
}

void
GameplayScene::initCharacter()
{
    TMXObjectGroup* temp = _map->getObjectGroup("player");
    auto ts = temp->getObject("birthPoint");

    float x = ts["x"].asFloat();
    float y = ts["y"].asFloat();

    auto characterTagList = GameData::getInstance()->getOnStageCharacterTagList();
    p1Player = Player::create(characterTagList[0]);
    p2Player = Player::create(characterTagList[1]);
    p1Player->setPosition(x, y);
    p2Player->setPosition(x, y);
    p1Player->retain();
    p2Player->retain();

    curPlayer = p1Player;
    curPlayer->setName("curPlayer");
    mapLayer->addChild(p1Player, MAPLAYER_CHARACTER_ZORDER);

    curPlayer->changeAttackType(p1Player->currentAttackType);
}

void
GameplayScene::initCtrlPanel()
{
    controlPanel = CtrlPanelLayer::create();

    this->addChild(controlPanel, CTRLPANEL_LAYER_ZORDER);
}

void
GameplayScene::initArea()
{
    auto playerPos = curPlayer->getPosition();
    auto areas = _map->getObjectGroup("area")->getObjects();

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
            //替换背景图片
            backgroundPicture->setTexture(dict["background"].asString());
            auto width = backgroundPicture->getContentSize().width;
            auto height = backgroundPicture->getContentSize().height;
            auto bigger = width > height ? width : height;
            float scale = visibleSize.width / width;
            backgroundPicture->setScale(scale * 1.1);

            //替换背景音乐
            if (dict["bgm"].asString() == "") {
                AudioController::getInstance()->clearMusic();
            } else {
                AudioController::getInstance()->playMusic(dict["bgm"].asString(), true);
            }
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
    camera = Node::create();
    mapLayer->addChild(camera, MAPLAYER_CAMERA_ZORDER);

    Vec2 poi = curPlayer->getPosition();
    camera->setPosition(poi.x + 100, poi.y + 70); //移动摄像机

    auto cameraFollow = Follow::create(camera, curArea);
    cameraFollow->setTag(cameraTag);
    mapLayer->runAction(cameraFollow);
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
            mapLayer->addChild(_launcher, MAPLAYER_OTHER_ZORDER);

            auto fe = Emitter::create((Node**)(&curPlayer));
            _launcher->addChild(fe);
            // fe->playStyle(StyleType::SCATTER);
            // fe->playStyle(StyleType::ODDEVEN);
            fe->playStyle(StyleType::PARALLEL);

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
            mapLayer->addChild(_elevator, MAPLAYER_OTHER_ZORDER);
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

    _bosses = 0;

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
            mapLayer->addChild(_enemy, MAPLAYER_ENEMY_ZORDER);

            /*临时项*/
            _enemy->setTarget(curPlayer);
            _enemy->setEmitter();
            /*临时项*/

            enemyList.pushBack(_enemy);

            if (dict["type"].asString() == "boss") {
                _bosses++;
                auto ctrlLayer = (CtrlPanelLayer*)controlPanel;
                ctrlLayer->createBossHpBar(_enemy, _enemy->CurrentHp, _enemy->face);
            }
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
            mapLayer->addChild(_event, MAPLAYER_OTHER_ZORDER);

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
                    auto _enemy = (Enemy*)entityA;
                    _enemy->resetJump();
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
                ParticleSystem* _ps = ParticleExplosion::createWithTotalParticles(5);
                _ps->setTexture(Director::getInstance()->getTextureCache()->addImage(
                    "gameplayscene/smallOrb000.png"));

                // cocos2dx的粒子系统有三种位置类型
                mapLayer->addChild(_ps, MAPLAYER_OTHER_ZORDER);
                _ps->setPositionType(ParticleSystem::PositionType::RELATIVE);
                _ps->setPosition(entityA->getPosition());
                _ps->setLife(1.2);
                _ps->setLifeVar(0.3);
                _ps->setEndSize(0.0f);
                _ps->setAutoRemoveOnFinish(true);

                auto _enemy = (Enemy*)entityA;
                auto _bullet = (Bullet*)entityB;

                DamageInfo _damageInfo;
                _damageInfo.damage = _bullet->getDamage();
                _damageInfo.target = _enemy;
                EventCustom event("bullet_hit_enemy");
                event.setUserData((void*)&_damageInfo);
                _eventDispatcher->dispatchEvent(&event);

                entityB->removeFromParentAndCleanup(true); //移除子弹
            }
            // 当enemy站在电梯上
            else if (entityB->getTag() == elevatorCategoryTag) {
                if (contact.getContactData()->normal.y > 0) {
                    auto _enemy = (Enemy*)entityA;
                    auto _elevator = (Elevator*)entityB;
                    _enemy->resetJump();
                    _elevator->addPassenger(_enemy);
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
                    auto _player = (Player*)entityA;
                    _player->resetJump();
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
                    auto _enemy = (Enemy*)entityB;
                    _enemy->stateMachine->autoChangeState();
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
                    auto _player = (Player*)entityA;
                    auto _elevator = (Elevator*)entityB;
                    _player->resetJump();
                    _elevator->addPassenger(curPlayer);
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
                auto _elevator = (Elevator*)entityB;
                _elevator->removePassenger(_enemy);
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
                auto _elevator = (Elevator*)entityB;
                _elevator->removePassenger(curPlayer);
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

    _eventDispatcher->addCustomEventListener("bullet_hit_enemy", [this](EventCustom* e) {
        auto _damageInfo = (DamageInfo*)e->getUserData();
        auto _enemy = (Enemy*)_damageInfo->target;
        _enemy->decreaseHp(_damageInfo->damage);
    });

    _eventDispatcher->addCustomEventListener("bullet_hit_player", [this](EventCustom* e) {
        auto _damageInfo = (DamageInfo*)e->getUserData();
        auto _player = (Player*)_damageInfo->target;
        _player->getHit(_damageInfo, _eventFilterMgr);
    });

    _eventDispatcher->addCustomEventListener("use_item", [this](EventCustom* e) {
        string itemTag = (char*)e->getUserData();
        Hp_Mp_Change hpChange;
        if (itemTag == "I1") {
            hpChange.tag = curPlayer->playerTag;
            hpChange.value = 20;
        } else if (itemTag == "I2") {
            hpChange.tag = curPlayer->playerTag;
            hpChange.value = -40;
        } else if (itemTag == "I3") {
            hpChange.tag = curPlayer->playerTag;
            hpChange.value = -40;
        } else if (itemTag == "I4") {
            hpChange.tag = curPlayer->playerTag;
            hpChange.value = 50;
        }
        EventCustom event("hp_change");
        event.setUserData((void*)&hpChange);
        _eventDispatcher->dispatchEvent(&event);

        curPlayer->currentHP += hpChange.value;
    });

    _eventDispatcher->addCustomEventListener("use_spell_card", [this](EventCustom* e) {
        string spellTag = (char*)e->getUserData();
        Hp_Mp_Change mpChange;
        if (spellTag == "C1") {
            mpChange.tag = curPlayer->playerTag;
            mpChange.value = -20;
        } else if (spellTag == "C2") {
            mpChange.tag = curPlayer->playerTag;
            mpChange.value = -30;
        }
        EventCustom event("mana_change");
        event.setUserData((void*)&mpChange);
        _eventDispatcher->dispatchEvent(&event);

        curPlayer->currentMana += mpChange.value;
    });

    _eventDispatcher->addCustomEventListener("kill_boss", [this](EventCustom* e) {
        _bosses--;
        if (_bosses == 0) {
            auto ctrlLayer = (CtrlPanelLayer*)controlPanel;
            ctrlLayer->removeBossHpBar();

            auto sequence = Sequence::create(
                DelayTime::create(3.0f),
                CallFuncN::create(CC_CALLBACK_0(GameplayScene::endGame, this)), NULL);
            this->runAction(sequence);
        }
    });
}

void
GameplayScene::endGame()
{
    auto roundInformation = this->_map->getObjectGroup("RoundInformation");
    auto endEvent = roundInformation->getProperty("onExitTriggerEvent").asString();

    EventCustom event("trigger_event");
    event.setUserData((void*)endEvent.c_str());
    _eventDispatcher->dispatchEvent(&event);
}

void
GameplayScene::onEventLeftKeyPressed(EventCustom*)
{
    curPlayer->playerSprite->setScaleX(-1); //人物翻转
    curPlayer->playerDirection = Direction::LEFT;

    curPlayer->schedule(CC_SCHEDULE_SELECTOR(Player::horizontallyAccelerate));
}

void
GameplayScene::onEventRightKeyPressed(EventCustom*)
{
    curPlayer->playerSprite->setScaleX(1); //人物翻转
    curPlayer->playerDirection = Direction::RIGHT;

    curPlayer->schedule(CC_SCHEDULE_SELECTOR(Player::horizontallyAccelerate));
}

void
GameplayScene::onEventMotionKeyReleased(EventCustom*)
{
    if (curPlayer->isScheduled(CC_SCHEDULE_SELECTOR(Player::horizontallyAccelerate))) {
        curPlayer->unschedule(CC_SCHEDULE_SELECTOR(Player::horizontallyAccelerate));
    }

    if (curPlayer->stateMachine->getCurrentState() == Player::Walk::getInstance()) {
        //减速
        auto currentVelocity = curPlayer->getPhysicsBody()->getVelocity();
        curPlayer->getPhysicsBody()->setVelocity(Vec2(currentVelocity.x / 3.0f, currentVelocity.y));
    }
}

void
GameplayScene::onEventJumpKeyPressed(EventCustom*)
{
    if (curPlayer->jumpCounts == 0) {
        return;
    }
    curPlayer->stateMachine->changeState(Player::Jump::getInstance());
}

void
GameplayScene::onEventDashKeyPressed(EventCustom*)
{
    if (curPlayer->dashCounts == 0) {
        return;
    }
    curPlayer->stateMachine->changeState(Player::Dash::getInstance());
}

void
GameplayScene::onEventSwitchCharacter(EventCustom*)
{
    AudioController::getInstance()->playClickButtonEffect();

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
    mapLayer->addChild(theOther, MAPLAYER_CHARACTER_ZORDER);
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
    AudioController::getInstance()->playClickButtonEffect();

    auto layer = SettingsLayer::create("GameplayScene");
    layer->setPauseNode(mapLayer);
    mapLayer->onExit();
    this->addChild(layer, SETTING_LAYER_ZORDER);
}

void
GameplayScene::update(float dt)
{
    Vec2 poi = curPlayer->getPosition();
    camera->setPosition(poi.x + 100, poi.y + 70); //移动摄像机

    //如果地图切换区域后首次执行update函数，则首先进行以下初始化操作
    if (isPosUpdate) {
        backParallaxNodePrePosition = Vec2::ZERO;
        mapLayerPrePosition = mapLayer->getPosition();
        isPosUpdate = false;
    }
    //取得TMX地图层的偏移量
    auto mapLayerCurPosition = mapLayer->getPosition();
    auto offsetX = mapLayerCurPosition.x - mapLayerPrePosition.x;
    auto offsetY = mapLayerCurPosition.y - mapLayerPrePosition.y;
    mapLayerPrePosition = mapLayerCurPosition;
    //设置背景视差结点的偏移量
    backParallaxNode->setPosition(backParallaxNodePrePosition.x + offsetX,
                                  backParallaxNodePrePosition.y + offsetY);
    backParallaxNodePrePosition =
        Vec2(backParallaxNodePrePosition.x + offsetX, backParallaxNodePrePosition.y + offsetY);

    if (curArea.containsPoint(poi)) {
        ;
    } else {

        //移除前一个区域的物件
        for (auto v : enemyList) {
            v->removeFromParentAndCleanup(true);
        }
        for (auto v : eventPoint) {
            v->removeFromParentAndCleanup(true);
        }
        for (auto v : launcherList) {
            v->removeFromParentAndCleanup(true);
        }
        for (auto v : elevatorList) {
            v->removeFromParentAndCleanup(true);
        }
        if (_bosses != 0) {
            auto ctrlLayer = (CtrlPanelLayer*)controlPanel;
            ctrlLayer->removeBossHpBar();
        }

        initArea();

        //重置摄像机跟随
        mapLayer->stopActionByTag(cameraTag);
        auto cameraFollow = Follow::create(camera, curArea);
        cameraFollow->setTag(cameraTag);
        mapLayer->runAction(cameraFollow);

        //重置偏移量，重置视差节点位置
        backParallaxNode->setPosition(Vec2::ZERO);
        backParallaxNodePrePosition = Vec2::ZERO;
        mapLayerPrePosition = mapLayer->getPosition();
        isPosUpdate = true;
    }
}
