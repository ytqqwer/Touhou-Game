#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Emitter.h"

Emitter*
Emitter::create(Node* character)
{
    Emitter* pRet = new Emitter(character);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

Emitter::Emitter(Node* character)
{
    this->character = character;
    this->tag = 1;
}

Emitter*
Emitter::create(Node* character, Node* target)
{
    Emitter* pRet = new Emitter(character, target);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

Emitter::Emitter(Node* character, Node* target)
{
    this->character = character;
    this->target = target;
    this->tag = 1;
}

int
Emitter::playStyle(const StyleConfig& sc)
{
    EmitterStyle* style;

    switch (sc.style) {
        case StyleType::SCATTER:
            style = Scatter::create(sc, character);
            dynamic_cast<Scatter*>(style)->createBullet();
            break;
        case StyleType::ODDEVEN:
            break;
        default:
            return false;
            break;
    }

    int tag = tag++;
    style->setTag(tag);
    this->addChild(style);
    styles.insert(tag++, style);

    return tag;
}

void
Emitter::pauseStyle(int tag)
{
    auto style = styles.at(tag);
    style->pauseSchedulerAndActions();
}

void
Emitter::pauseAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        s->second->pauseSchedulerAndActions();
    }
}

void
Emitter::resumeStyle(int tag)
{
    auto style = styles.at(tag);
    style->resumeSchedulerAndActions();
}

void
Emitter::resumeAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        s->second->resumeSchedulerAndActions();
    }
}

void
Emitter::stopStyle(int tag)
{
    auto style = styles.at(tag);
    style->removeFromParent();
    styles.erase(tag);
}

void
Emitter::stopAllStyle()
{
    for (auto s = styles.begin(); s != styles.end(); s++) {
        s->second->removeFromParent();
    }
    styles.clear();
}