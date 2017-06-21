#ifndef __EMITTERS_H__
#define __EMITTERS_H__

#include "cocos2d.h"
#include "EmitterStyle.h"
#include "FirstEmitter.h"

class Emitters :public Node{
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

