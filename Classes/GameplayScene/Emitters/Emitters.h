#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef __EMITTERS_H__
#define __EMITTERS_H__

#include "EmitterStyle.h"
#include "FirstEmitter.h"
#include "cocos2d.h"

class Emitters : public Node
{
public:
    Emitters(Node* character);
    static Emitters* create(Node* character);
    bool init();
    Vector<Node*>& getEmitters();

private:
    Node* character;
    std::string filename;
    Vector<Node*> emitters;
};

#endif // !__EMITTERS_H__
