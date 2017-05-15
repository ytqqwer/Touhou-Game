#ifndef CONVERSATION_LAYER_H
#define CONVERSATION_LAYER_H

#include "cocos2d.h"
#include <string>

USING_NS_CC;

class ConversationLayer : public Layer
{
public:
    CREATE_FUNC(ConversationLayer);
    static Scene* createDebugScene();

    virtual bool init();
    virtual void update(float dt);

private:
    ConversationLayer(); // for autorelease consideration, make ctor private

private:
    // intorspection
    Size _visibleSize;
};

#endif // CONVERSATION_LAYER_H
