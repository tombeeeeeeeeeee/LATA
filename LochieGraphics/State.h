#pragma once
#include <vector>

class SceneObject;
struct Condition;

struct State
{
public:

    struct Transition
    {
        Condition* condition;
        State* targetState;
    };

	virtual void Enter(SceneObject* sceneObject) {};
	virtual void Update(SceneObject* sceneObject) = 0;
	virtual void Exit(SceneObject* sceneObject) {};
    virtual void AddTransition(Condition* con, State* target) { transitions.push_back({ con, target }); }
    std::vector<Transition> transitions;
};

