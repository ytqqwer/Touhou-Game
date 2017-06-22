#include "Enemy.h"
#include "GameplayScene.h"
#include "Player.h"

bool
Enemy::init()
{
    if (!Node::init())
        return false;

    Animation* enemyAnimation = Animation::create();
    for (int i = 1; i <= 6; i++)
        enemyAnimation->addSpriteFrameWithFile("gameplayscene/opossum-" + std::to_string(i) +
                                               ".png");

    enemyAnimation->setDelayPerUnit(0.15f);

    enemySprite = Sprite::create(
        "gameplayscene/opossum-1.png"); //此处必须初始化一张角色纹理，否则后面无法切换纹理

    enemyAnim = Animate::create(enemyAnimation);
    enemySprite->runAction(RepeatForever::create(enemyAnim)); //初始时刻在奔跑
    this->addChild(enemySprite);

    auto enemyBody = PhysicsBody::createBox(
        enemySprite->getContentSize()); //这里要用包围盒，如果用圆形的话会导致滚动
    enemyBody->setDynamic(true);
    enemyBody->setMass(1);
    enemyBody->setGravityEnable(true);
    enemyBody->setRotationEnable(false);

    enemyBody->getFirstShape()->setDensity(0);
    enemyBody->getFirstShape()->setFriction(0.2);
    enemyBody->getFirstShape()->setRestitution(0); //设置刚体回弹力

    enemyBody->setCategoryBitmask(enemyCategory);
    enemyBody->setCollisionBitmask(groundCategory | bulletCategory | playerCategory);
    enemyBody->setContactTestBitmask(groundCategory | bulletCategory);

    this->setPhysicsBody(enemyBody);
    // enemySprite->setPhysicsBody(enemyBody);//错误示例，应该给实例对象设置刚体，而不是一个用来显示图片的精灵

    this->setTag(103);

    return true;
}

void
Enemy::run()
{
    //朝玩家移动
    Player* p = (Player*)this->getParent()->getChildByName("player");
    Vec2 poi = p->getPosition();

    Point enemyPos = this->getPosition();

    if (enemyPos.x - poi.x > 500) {
        return;
    }
    if (enemyPos.x - poi.x > 0) {
        this->getPhysicsBody()->applyImpulse(Vec2(-3.0f, 0.0f));
        // this->getPhysicsBody()->setVelocity(Vec2(-200.f, 0.0f));
    } else {
        this->getPhysicsBody()->applyImpulse(Vec2(3.0f, 0.0f));
        // this->getPhysicsBody()->setVelocity(Vec2(200.f, 0.0f));
    }
}

void
Enemy::jump()
{
    auto body = this->getPhysicsBody();
    // auto curVelocity = body->getVelocity();
    // body->setVelocity(Vec2(curVelocity.x, 0));//再次跳跃时，重置Y轴速度为0
    Vec2 impluse = Vec2(0.0f, 400.0f);
    body->applyImpulse(impluse);
}

void
Enemy::AI(float dt)
{
    int weight = random(0, 1000); //权重
    if (weight >= 20) {
        run();
    } else if (weight < 20) {
        if (_canJump == false)
            return; //当敌人在空中的时候不可以再跳跃
        if (_canJump) {
            jump();
            this->_canJump = false;
        }
    }
}

void
Enemy::decreaseHp(Node* node)
{
    auto enemy = (Enemy*)node;
    enemy->hp = enemy->hp - 5;
    if (enemy->hp < 0) {
        enemy->removeFromParentAndCleanup(true);
    }
}
