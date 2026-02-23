#include "State.h"

#include "Behaviour.h"
#include "Condition.h"

State::State(Behaviour* behaviour)
{
	behaviours.push_back(behaviour);
}

State::~State()
{
	// This owns the behaviours
	for (Behaviour* b : behaviours)
	{
		delete b;
	}
}

const std::vector<State::Transition>& State::getTransitions()
{
	return transitions;
}

void State::AddTransition(Condition* condition, State* state)
{
	transitions.push_back({ condition, state });
}

void State::Enter(SceneObject* so)
{
	for (Behaviour* b : behaviours)
	{
		b->Enter(so);
	}
}

void State::Update(SceneObject* so, float delta)
{
	for (Behaviour* b : behaviours)
	{
		b->Update(so, delta);
	}
}

void State::Exit(SceneObject* so)
{
	for (Behaviour* b : behaviours)
	{
		b->Exit(so);
	}
}
