#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Frog.h"

bool
Frog::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->enemyTag = tag;
    this->setTag(enemyCategoryTag);
    this->setName(tag);

    animateManager = AnimateManager::getInstance();
    enemySprite = Sprite::create(
        animateManager->addEnemyTexture(tag)); //此处必须初始化一张角色纹理，否则后面无法切换纹理
    this->addChild(enemySprite);

    //设置属性值
    EnemyData enemyData = GameData::getInstance()->getEnemyByTag(tag);
    this->hp = enemyData.healthPoint;

    //设置刚体
    body = PhysicsBody::createBox(
        enemySprite->getContentSize()); //这里要用包围盒，如果用圆形的话会导致滚动
    body->setDynamic(true);
    body->setMass(1);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    body->getFirstShape()->setDensity(0);
    body->getFirstShape()->setFriction(0.2);
    body->getFirstShape()->setRestitution(0); //设置刚体回弹力
    body->setCategoryBitmask(enemyCategory);
    body->setCollisionBitmask(groundCategory | bulletCategory);
    body->setContactTestBitmask(groundCategory | bulletCategory);
    this->setPhysicsBody(body);

    //索敌检测框
    auto rect = PhysicsShapeBox::create(Size(200, 150));
    rect->setCategoryBitmask(lockCategory);
    rect->setCollisionBitmask(0);
    rect->setContactTestBitmask(playerCategory);
    body->addShape(rect);

    //设置速度上限
    body->setVelocityLimit(400);

    //设置动画
    enemyAnimation = animateManager->addEnemyCache(tag);
    enemySprite->runAction(RepeatForever::create(Animate::create(enemyAnimation))); //初始时刻在奔跑

    return true;
}

void
Frog::run()
{
    //朝玩家移动
    Vec2 poi = (*curPlayer)->getPosition();
    Point enemyPos = this->getPosition();

    if (enemyPos.x > poi.x) {
        this->getPhysicsBody()->applyImpulse(Vec2(-20.0f, 0.0f));
    } else {
        this->getPhysicsBody()->applyImpulse(Vec2(20.0f, 0.0f));
    }
}

void
Frog::jump()
{
    auto body = this->getPhysicsBody();
    Vec2 impluse = Vec2(0.0f, 400.0f);
    body->applyImpulse(impluse);
}

void
Frog::AI(float dt)
{
    //警戒状态
    if (curState == EnemyState::Alert) {
        int weight = random(0, 1000); //权重
        if (weight >= 200) {
            run();
        } else if (weight < 200) {
            if (_canJump == false)
                return; //当敌人在空中的时候不可以再跳跃
            if (_canJump) {
                jump();
                this->_canJump = false;
            }
        }

    }
    //巡逻状态
    else if (curState == EnemyState::Patrol) {
        ; //留空，随机寻路
    }
}

void
Frog::decreaseHp(Node* node)
{
    auto enemy = (Enemy*)node;
    enemy->hp = enemy->hp - 5;
    if (enemy->hp < 0) {
        enemy->removeFromParentAndCleanup(true);
    }
}

void
Frog::startSchedule(Player*& player)
{
    curPlayer = &player;

    this->schedule(CC_SCHEDULE_SELECTOR(Enemy::AI), 0.05);
}
