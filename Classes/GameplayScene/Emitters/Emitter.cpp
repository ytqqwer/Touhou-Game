﻿#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Emitter.h"
#include "Style/EmitterStyle.h"
#include "Style/Laser.h"
#include "Style/OddEven.h"
#include "Style/Parabola.h"
#include "Style/Parallel.h"
#include "Style/Scatter.h"

Emitter*
Emitter::create(Direction* direction)
{
    Emitter* pRet = new (std::nothrow) Emitter(direction);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

Emitter::Emitter(Direction* direction)
{
    this->isPlayer = true;
    this->direction = direction;
    this->styleTag = 1;
}

Emitter*
Emitter::create(Node** target)
{
    Emitter* pRet = new (std::nothrow) Emitter(target);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

Emitter::Emitter(Node** target)
{
    this->isPlayer = false;
    this->target = target;
    this->styleTag = 1;
}

int
Emitter::playStyle(const StyleConfig& sc)
{
    EmitterStyle* style;

    switch (sc.style) {

        case StyleType::LASER:
            if (isPlayer) {
                style = Laser::create(sc, direction);
            } else {
                style = Laser::create(sc, target);
            }
            dynamic_cast<Laser*>(style)->startShoot();
            break;
        case StyleType::ODDEVEN:
            if (isPlayer) {
                return 0;
            } else {
                style = OddEven::create(sc, target);
            }
            dynamic_cast<OddEven*>(style)->startShoot();
            break;
        case StyleType::PARABOLA:
            if (isPlayer) {
                style = Parabola::create(sc, direction);
            } else {
                return 0;
            }
            dynamic_cast<Parabola*>(style)->startShoot();
            break;
        case StyleType::PARALLEL:
            if (isPlayer) {
                style = Parallel::create(sc, direction);
            } else {
                style = Parallel::create(sc, target);
            }
            dynamic_cast<Parallel*>(style)->startShoot();
            break;
        case StyleType::SCATTER:
            if (isPlayer) {
                style = Scatter::create(sc, direction);
            } else {
                style = Scatter::create(sc);
            }
            dynamic_cast<Scatter*>(style)->startShoot();
            break;
        default:
            return false;
            break;
    }

    style->setTag(styleTag);
    this->addChild(style);
    int trueTag = styleTag;
    styles.insert(styleTag++, style);

    return trueTag;
}

int
Emitter::playStyle(StyleType st)
{
    EmitterStyle* style;

    switch (st) {

        case StyleType::LASER:
            if (isPlayer) {
                style = Laser::create(direction);
            } else {
                style = Laser::create(target);
            }
            dynamic_cast<Laser*>(style)->startShoot();
            break;
        case StyleType::ODDEVEN:
            if (isPlayer) {
                return 0;
            } else {
                style = OddEven::create(target);
            }
            dynamic_cast<OddEven*>(style)->startShoot();
            break;
        case StyleType::PARABOLA:
            if (isPlayer) {
                style = Parabola::create(direction);
            } else {
                return 0;
            }
            dynamic_cast<Parabola*>(style)->startShoot();
            break;
        case StyleType::PARALLEL:
            if (isPlayer) {
                style = Parallel::create(direction);
            } else {
                style = Parallel::create(target);
            }
            dynamic_cast<Parallel*>(style)->startShoot();
            break;
        case StyleType::SCATTER:
            if (isPlayer) {
                style = Scatter::create(direction);
            } else {
                style = Scatter::create();
            }
            dynamic_cast<Scatter*>(style)->startShoot();
            break;
        default:
            return false;
            break;
    }

    style->setTag(styleTag);
    this->addChild(style);
    int trueTag = styleTag;
    styles.insert(styleTag++, style);

    return trueTag;
}

void
Emitter::pauseStyle(int styleTag)
{
    auto style = styles.at(styleTag);
    style->pause();
}

void
Emitter::pauseAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        s->second->pause();
    }
}

void
Emitter::resumeStyle(int styleTag)
{
    auto style = styles.at(styleTag);
    style->resume();
}

void
Emitter::resumeAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        s->second->resume();
    }
}

void
Emitter::stopStyle(int styleTag)
{
    auto style = (EmitterStyle*)styles.at(styleTag);
    style->stopShoot();
    std::function<void(Ref*)> clean = [style](Ref*) {
        style->cleanup();
        log("[Emitter] style cleaned");
    };
    auto removeSelf = CallFuncN::create(clean);
    style->runAction(Sequence::create(DelayTime::create(15), removeSelf, NULL));
    styles.erase(styleTag);
}

void
Emitter::stopAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        auto style = (EmitterStyle*)s->second;
        style->stopShoot();

        std::function<void(Ref*)> clean = [style](Ref*) {
            style->cleanup();
            log("[Emitter] style cleaned");
        };
        auto removeSelf = CallFuncN::create(clean);
        style->runAction(Sequence::create(DelayTime::create(15), removeSelf, NULL));
    }
    styles.clear();
    styleTag = 1;
}
