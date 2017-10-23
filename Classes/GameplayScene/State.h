#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMESCENE_STATE_H
#define GAMESCENE_STATE_H

#include <stdlib.h>

template <class entity_type>
class State
{
public:
    virtual void Enter(entity_type*) = 0;
    virtual void Exit(entity_type*) = 0;
    virtual void defaultChangeState(entity_type*) = 0;
    virtual ~State() {}
};

template <class entity_type>
class StateMachine
{
private:
    entity_type* owner;
    State<entity_type>* currentState;
    State<entity_type>* previousState;

public:
    StateMachine(entity_type* owner)
        : owner(owner)
        , currentState(NULL)
        , previousState(NULL)
    {
    }

    void changeState(State<entity_type>* newState)
    {
        if (currentState) {
            previousState = currentState;
            currentState->Exit(owner);
        }
        currentState = newState;
        currentState->Enter(owner);
    }

    void autoChangeState() { currentState->defaultChangeState(owner); }

    void RevertToPreviousState() { changeState(previousState); }

    void SetPreviousState(State<entity_type>* s) { previousState = s; }
    void SetCurrentState(State<entity_type>* s) { currentState = s; }

    State<entity_type>* getCurrentState() const { return currentState; }
    State<entity_type>* getPreviousState() const { return previousState; }
};

#endif
