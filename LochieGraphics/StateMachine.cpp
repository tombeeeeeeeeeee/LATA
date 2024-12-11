#include "StateMachine.h"

#include "State.h"
#include "Condition.h"

#include <iostream>

StateMachine::StateMachine(State* state) :
	currentState(state),
	initialState(state)
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

void StateMachine::setInitialState(State* state)
{
	initialState = state;
	currentState = state;
}

State* StateMachine::getInitialState() const
{
	return initialState;
}

void StateMachine::AddState(State* state)
{
	states.push_back(state);
}

void StateMachine::AddCondition(Condition* condition)
{
	conditions.push_back(condition);
}

void StateMachine::AddAnyTransition(Condition* condition, State* state)
{
	anyTransitions.push_back({ condition, state });
}

void StateMachine::Update(SceneObject* so, float delta)
{
	if (!currentState) {
		currentState = initialState;
	}
	if (!currentState) {
		std::cout << "Error: State machine has not state active\n";
		return;
	}

	State* newState = nullptr;

	for (const State::Transition& t : currentState->getTransitions())
	{
		if (t.condition->IsTrue(so)) {
			newState = t.targetState;
		}
	}
	for (const State::Transition& t : anyTransitions)
	{
		if (t.condition->IsTrue(so)) {
			newState = t.targetState;
		}
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
	if (!currentState) {
		currentState = initialState;
	}
	if (!currentState) {
		std::cout << "Error: State machine has not state active\n";
		return;
	}
	currentState->Enter(so);
}

void StateMachine::Exit(SceneObject* so)
{
	currentState->Exit(so);
}
