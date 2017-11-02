#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Parallel.h"

Parallel::Parallel(Node** target)
{
    //敌人默认参数
    this->sc.style = StyleType::PARALLEL;
    this->sc.frequency = 0.5f;
    this->sc.bulletDuration = 6.0;
    this->sc.countThenChangePos = 1;
    this->sc.number = 4;
    this->sc.interval = 1.5;

    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->sc.bc.name = "b2_2_1.png";
    this->sc.bc.length = 20;
    this->sc.bc.width = 10;
    this->sc.bc.harm = 10;
    this->sc.bc._categoryBitmask = 0;
    this->sc.bc._collisionBitmask = 0;
    this->sc.bc._contactTestBitmask = 0;

    this->isPlayer = false;
    this->target = target;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Parallel::Parallel(const StyleConfig& sc, Node** target)
{
    this->sc = sc;

    this->isPlayer = false;
    this->target = target;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Parallel::Parallel(Direction* direction)
{
    //角色默认参数
    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->sc.style = StyleType::PARALLEL;
    this->sc.frequency = 1.0f;
    this->sc.bulletDuration = 3.0;
    this->sc.countThenChangePos = 0;
    this->sc.number = 2;
    this->sc.interval = 2;
    this->sc.bc.name = "b1_3_3.png";

    this->isPlayer = true;
    this->direction = direction;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Parallel::Parallel(const StyleConfig& sc, Direction* direction)
{
    this->sc = sc;

    this->isPlayer = true;
    this->direction = direction;

    this->counterInside = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

void
Parallel::startShoot()
{
    if (isPlayer == false) {
        this->targetPos = (*target)->getPosition();
    }
    this->schedule(CC_SCHEDULE_SELECTOR(Parallel::shootBullet));
}

void
Parallel::stopShoot()
{
    this->unschedule(CC_SCHEDULE_SELECTOR(Parallel::shootBullet));
}

void
Parallel::shootBullet(float dt)
{
    timeAccumulation += dt;
    elapsed += dt;
    if (timeAccumulation >= sc.frequency) {
        spawnBullet();
        spawnBulletCycleTimes++;
        timeAccumulation = 0;
        if (spawnBulletCycleTimes >= sc.cycleTimes) {
            stopShoot();
        }
    }
    if (elapsed >= sc.totalDuration) {
        stopShoot();
    }
}

void
Parallel::spawnBullet()
{
    if (sc.countThenChangePos != 0) {
        if (this->counterInside == sc.countThenChangePos) {
            this->counterInside = 0;
            if (isPlayer == false) {
                this->targetPos = (*target)->getPosition(); //更新目标位置
            }
            // return;
        } else {
            this->counterInside++;
        }
    }

    auto emitter = this->getParent();
    auto character = emitter->getParent();
    auto mapLayer = character->getParent();

    auto winSize = Director::getInstance()->getWinSize();
    auto intervalDis = winSize.height / 36.0;
    float distance = sqrt(winSize.width * winSize.width + winSize.height * winSize.height);
    auto datumPos = character->getPosition(); //基准位置
    Vec2 startPos;
    Vec2 deltaP; //位移
    float angle; //自机夹角
    float intervalX;
    float intervalY;

    if (isPlayer == false) {
        auto dis = targetPos - datumPos;                              //基准向量
        angle = CC_RADIANS_TO_DEGREES(Vec2(dis.y, dis.x).getAngle()); //基础偏转角

        auto intervalAngle = CC_DEGREES_TO_RADIANS(angle - 180.0); //间距夹角
        intervalX = intervalDis * cos(intervalAngle);
        intervalY = intervalDis * sin(intervalAngle);

        startPos = Vec2(datumPos.x - (sc.number - 1) * (intervalX / 2),
                        datumPos.y + (sc.number - 1) * (intervalX / 2));
        deltaP = Vec2(distance * cos(CC_DEGREES_TO_RADIANS(90.0 - angle)),
                      distance * sin(CC_DEGREES_TO_RADIANS(90.0 - angle)));
    } else {
        startPos = Vec2(datumPos.x, datumPos.y + (sc.number - 1) * (intervalDis / 2.0));
        deltaP = Vec2(0, distance);
    }

    for (int i = 0; i < sc.number; i++) {

        Bullet* spriteBullet = Bullet::create(sc.bc);
        bullets.pushBack(spriteBullet);
        spriteBullet->setAnchorPoint(Vec2(0.5, 0.5));

        if (isPlayer == false) {
            spriteBullet->setRotation(angle);
            auto actualPos = Vec2(startPos.x + i * intervalX, startPos.y - i * intervalY);
            spriteBullet->setPosition(actualPos);
        } else {
            auto actualPos = Vec2(startPos.x, startPos.y - i * intervalDis);
            spriteBullet->setPosition(actualPos);
        }
        mapLayer->addChild(spriteBullet);

        auto actionMove = MoveBy::create(sc.bulletDuration, deltaP);
        auto actionMoveBy = actionMove;
        if (isPlayer == true) { //建议角色使用中心对称型子弹
            if ((*direction) == Direction::LEFT) {
                actionMoveBy = actionMove->reverse();
            }
        }
        auto actionInOut = EaseInOut::create(actionMoveBy, 1.0);
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(Parallel::removeBullet, this));
        auto sequence = Sequence::create(actionInOut, actionDone, NULL);
        spriteBullet->runAction(sequence);
    }
}
