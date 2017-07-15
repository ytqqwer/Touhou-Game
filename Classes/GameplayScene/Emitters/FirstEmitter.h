#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef FIRST_EMITTER_H
#define FIRST_EMITTER_H

#include "EmitterStyle.h"

/*散射型发射器*/

class FirstEmitter : public EmitterStyle
{
public:
    FirstEmitter(Node* character);
    APP_CREATE_EMITTER(FirstEmitter);
    virtual void createBullet(float dt);
    virtual void shootBullet1(float dt);
    virtual void shootBullet2(float dt);
    virtual void shootBullet3(float dt);

private:
    int number; //条数
    int count;
    int mode;
    float startAngle; //起始角度
    float endAngle;   //终止角度
    float varAngle;   //变化角度
};

#endif
