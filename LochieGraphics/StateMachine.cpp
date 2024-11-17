#include "StateMachine.h"

#include "State.h"
#include "Condition.h"

StateMachine::StateMachine(State* state) :
	currentState(state)
{
}

StateMachine::~StateMachine()
{
	for (State* s : states)
	{
		delete s;
	}
	for (Condition* c : conditions)
	{
		delete c;
	}
}

void StateMachine::AddState(State* state)
{
	states.push_back(state);
}

void StateMachine::AddCondition(Condition* condition)
{
	conditions.push_back(condition);
}

void StateMachine::Update(SceneObject* so, float delta)
{
	State* newState = nullptr;

	for (State::Transition t : currentState->getTransitions())
	{
		if (t.condition->IsTrue(so))
			newState = t.targetState;
	}

	// State has changed
	if (newState != nullptr && newState != currentState)
	{
		currentState->Exit(so);
		currentState = newState;
		Enter(so);
	}

	// update the current state
	currentState->Update(so, delta);


}

void StateMachine::Enter(SceneObject* so)
{
	currentState->Enter(so);
}

void StateMachine::Exit(SceneObject* so)
{
	currentState->Exit(so);
}
