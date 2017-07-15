#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Emitters.h"

Emitters::Emitters(Node* character)
{
    this->character = character;
}

Emitters*
Emitters::create(Node* character)
{
    Emitters* pRet = new Emitters(character);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool
Emitters::init()
{
    if (!Node::init()) {
        return false;
    }
    return true;
}

Vector<Node*>&
Emitters::getEmitters()
{
    return emitters;
}
