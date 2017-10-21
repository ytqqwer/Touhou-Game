#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Enemy/Sakuya.h"
#include "GameData/EnemyData.h"
#include "GameData/GameData.h"

bool
Sakuya::init(std::string tag)
{
    if (!Node::init())
        return false;

    this->enemyTag = tag;
    this->setTag(enemyCategoryTag);
    this->setName(tag);

    EnemyData _enemyData = GameData::getInstance()->getEnemyByTag(tag);
    enemySprite = Sprite::create(_enemyData.defaultTexture);
    this->addChild(enemySprite);

    //设置属性值
    this->hp = _enemyData.healthPoint;

    //设置刚体
    body = PhysicsBody::createBox(Size(55, 90));
    body->setDynamic(true);
    body->setMass(1);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    body->getFirstShape()->setDensity(0);
    body->getFirstShape()->setFriction(0.2);
    body->getFirstShape()->setRestitution(0); //设置刚体回弹力
    body->getFirstShape()->setCategoryBitmask(enemyCategory);
    body->getFirstShape()->setCollisionBitmask(groundCategory | bulletCategory | elevatorCategory);
    body->getFirstShape()->setContactTestBitmask(groundCategory | bulletCategory |
                                                 elevatorCategory | playerCategory);
    this->setPhysicsBody(body);
	body->setPositionOffset(Vec2(0,-15));

    //设置速度上限
    body->setVelocityLimit(800);

    //获得动画缓存
    standAnimation = AnimationCache::getInstance()->getAnimation(_enemyData.standAnimationKey);

    //行为模式状态机
    modeStateMachine = new StateMachine<Enemy>(this);
    // modeStateMachine->changeState(::getInstance());

    //动画状态机
    animateStateMachine = new StateMachine<Enemy>(this);
    // animateStateMachine->changeState(::StandAnimation::getInstance());

    return true;
}

void
Sakuya::run(float dt)
{
    timeAccumulation += dt;
    if (timeAccumulation >= 0.1) {
        if (this->enemyDirection == Direction::LEFT) {
            this->getPhysicsBody()->applyImpulse(Vec2(-30.0f, 0.0f));
        } else {
            this->getPhysicsBody()->applyImpulse(Vec2(30.0f, 0.0f));
        }
        timeAccumulation = 0;
    }
}

void
Sakuya::decreaseHp(int damage)
{
    damageAccumulation += damage;

    this->hp = this->hp - damage;
    if (this->hp < 0) {
        this->removeFromParentAndCleanup(true);
    }
}
