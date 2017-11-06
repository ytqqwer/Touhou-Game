#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "cocos2d.h"

#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/Player/Player.h"

USING_NS_CC;

class Elevator : public Node
{
public:
    bool init();

    static Elevator* create()
    {
        Elevator* pRet;
        pRet = new (std::nothrow) Elevator();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    template <class T>
    inline void addPassenger(T* target)
    {
        passengers.push_back(target);
    }

    template <class T>
    inline void removePassenger(T* target)
    {
        for (auto it = passengers.begin(); it != passengers.end(); ++it) {
            if (*it == target) {
                passengers.erase(it);
                break;
            }
        }
    }

private:
    vector<Node*> passengers;
    Vec2 prePosition;
    void moveTogether(float dt);
};

class Broom : public Node
{
public:
    bool init();

    static Broom* create()
    {
        Broom* pRet = new (std::nothrow) Broom();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    template <class T>
    inline void addPassenger(T* target)
    {
        passengers.push_back(target);
    }

    template <class T>
    inline void removePassenger(T* target)
    {
        for (auto it = passengers.begin(); it != passengers.end(); ++it) {
            if (*it == target) {
                passengers.erase(it);
                break;
            }
        }
    }

private:
    vector<Node*> passengers;
    Vec2 prePosition;
    void moveTogether(float dt);
};

#endif
