#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Emitter.h"

Emitter::Emitter()
{
   this->styleTag = 1;
}

Emitter*
Emitter::create(Node* target)
{
    Emitter* pRet = new Emitter(target);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

Emitter::Emitter(Node* target)
{
    this->target = target;
    this->styleTag = 1;
}

int
Emitter::playStyle(const StyleConfig& sc)
{
    EmitterStyle* style;

    switch (sc.style) {
        case StyleType::SCATTER:
            style = Scatter::create(sc);
            dynamic_cast<Scatter*>(style)->createBullet();
            break;
        case StyleType::ODDEVEN:
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
	case StyleType::SCATTER:
		style = Scatter::create();
		dynamic_cast<Scatter*>(style)->createBullet();
		break;
	case StyleType::ODDEVEN:
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
Emitter::pauseStyle(int tag)
{
    auto style = styles.at(tag);
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
Emitter::resumeStyle(int tag)
{
    auto style = styles.at(tag);
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