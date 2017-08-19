#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Emitter.h"
#include "Style/OddEven.h"
#include "Style/Parabola.h"
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

        case StyleType::PARABOLA:
            if (isPlayer) {
                style = Parabola::create(sc, direction);
            } else {
                return 0;
            }
            dynamic_cast<Parabola*>(style)->createBullet();
            break;
        case StyleType::ODDEVEN:
            if (isPlayer) {
                return 0;
            } else {
                style = OddEven::create(sc, target);
            }
            dynamic_cast<OddEven*>(style)->createBullet();
            break;
        case StyleType::SCATTER:
            if (isPlayer) {
                style = Scatter::create(sc, direction);
            } else {
                style = Scatter::create(sc);
            }
            dynamic_cast<Scatter*>(style)->createBullet();
            break;
        default:
            return false;
            break;
    }

    style->setTag(styleTag);
    this->addChild(style);
    styles.insert(styleTag++, style);

    return styleTag;
}

int
Emitter::playStyle(StyleType st)
{
    EmitterStyle* style;

    switch (st) {

        case StyleType::PARABOLA:
            if (isPlayer) {
                style = Parabola::create(direction);
            } else {
                return 0;
            }
            dynamic_cast<Parabola*>(style)->createBullet();
            break;
        case StyleType::ODDEVEN:
            if (isPlayer) {
                return 0;
            } else {
                style = OddEven::create(target);
            }
            dynamic_cast<OddEven*>(style)->createBullet();
            break;
        case StyleType::SCATTER:
            if (isPlayer) {
                style = Scatter::create(direction);
            } else {
                style = Scatter::create();
            }
            dynamic_cast<Scatter*>(style)->createBullet();
            break;
        default:
            return false;
            break;
    }

    style->setTag(styleTag);
    this->addChild(style);
    styles.insert(styleTag++, style);

    return styleTag;
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
    auto style = styles.at(styleTag);
    style->removeFromParent();
    styles.erase(styleTag);
}

void
Emitter::stopAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        s->second->removeFromParent();
    }
    styles.clear();
}
