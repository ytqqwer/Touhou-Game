#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Opossum.h"

bool
Opossum::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->enemyTag = tag;
    this->setTag(enemyCategoryTag);
    this->setName(tag);

    enemySprite = Sprite::create(
        "gameplayscene/Enemy/opossum-1.png"); //此处必须初始化一张纹理，否则后面无法切换纹理
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
    auto rect = PhysicsShapeBox::create(Size(150, 200));
    rect->setCategoryBitmask(lockCategory);
    rect->setCollisionBitmask(0);
    rect->setContactTestBitmask(playerCategory);
    body->addShape(rect);

    //设置速度上限
    body->setVelocityLimit(500);

    //设置动画
    enemyAnimation = Animation::create();
    for (int i = 1; i <= 6; i++)
        enemyAnimation->addSpriteFrameWithFile("gameplayscene/Enemy/opossum-" + std::to_string(i) +
                                               ".png");
    enemyAnimation->setDelayPerUnit(0.15f);
    AnimationCache::getInstance()->addAnimation(enemyAnimation, "OpossumAnimation");

    enemySprite->runAction(RepeatForever::create(Animate::create(enemyAnimation))); //初始时刻在奔跑

    return true;
}

void
Opossum::run()
{
    //朝玩家移动
    Vec2 poi = (*curPlayer)->getPosition();
    Point enemyPos = this->getPosition();

    if (enemyPos.x > poi.x) {
        this->getPhysicsBody()->applyImpulse(Vec2(-30.0f, 0.0f));
    } else {
        this->getPhysicsBody()->applyImpulse(Vec2(30.0f, 0.0f));
    }
}

void
Opossum::jump()
{
    auto body = this->getPhysicsBody();
    Vec2 impluse = Vec2(0.0f, 300.0f);
    body->applyImpulse(impluse);
}

void
Opossum::AI(float dt)
{
    //警戒状态
    if (curState == EnemyState::Alert) {
        int weight = random(0, 1000); //权重
        if (weight >= 50) {
            run();
        } else if (weight < 50) {
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
Opossum::decreaseHp(Node* node)
{
    auto enemy = (Enemy*)node;
    enemy->hp = enemy->hp - 10;
    if (enemy->hp < 0) {
        enemy->removeFromParentAndCleanup(true);
    }
}
