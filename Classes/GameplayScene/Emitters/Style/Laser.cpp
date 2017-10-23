#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Laser.h"

Laser::Laser(Node** target)
{
    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->isPlayer = false;
    this->target = target;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Laser::Laser(const StyleConfig& sc, Node** target)
{
    this->isPlayer = false;
    this->sc = sc;
    this->target = target;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Laser::Laser(Direction* direction)
{
    this->sc.totalDuration = FLT_MAX;
    this->sc.cycleTimes = -1;

    this->isPlayer = true;
    this->direction = direction;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

Laser::Laser(const StyleConfig& sc, Direction* direction)
{
    this->isPlayer = true;
    this->direction = direction;
    this->sc = sc;

    this->counter = 0;
    this->spawnBulletCycleTimes = 0;
    this->timeAccumulation = 0;
    this->elapsed = 0;
}

void
Laser::startShoot()
{
    this->schedule(schedule_selector(Laser::shootBullet), sc.frequency);
}

void
Laser::stopShoot()
{
    this->unschedule(schedule_selector(Laser::shootBullet));
}

void
Laser::shootBullet(float dt)
{
    spawnBullet();
}

void
Laser::spawnBullet()
{
}
