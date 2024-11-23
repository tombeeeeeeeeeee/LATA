#pragma once

#include "Behaviour.h"

#include <vector>

class SceneObject;
class State;
class Condition;
class StateMachine : public Behaviour
{
private:
	// Owns the states
	std::vector<State*> states;
	// Owns the conditions
	// TODO: Don't like how the state machine itself needs to own the conditions, should the transitions just own them
	std::vector<Condition*> conditions;
	State* currentState;
	State* initialState = nullptr;

public:
	
	StateMachine(State* state);
	StateMachine() = default;
	~StateMachine();

	// Note: Only use for setup
	void setInitialState(State* state);
	State* getInitialState() const;
	void AddState(State* state);
	void AddCondition(Condition* condition);

	void Update(SceneObject* so, float delta) override;

	void Enter(SceneObject* so) override;
	void Exit(SceneObject* so) override;
};

