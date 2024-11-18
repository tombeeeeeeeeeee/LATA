#pragma once

#include <vector>

class Behaviour;
class Condition;
class SceneObject;

class State
{
public:
	struct Transition {
		Condition* condition;
		State* targetState;
	};

private:
	std::vector<Behaviour*> behaviours;
	std::vector<Transition> transitions;

public:

	State() = default;
	State(Behaviour* behaviour);
	~State();

	const std::vector<State::Transition>& getTransitions();

	void AddTransition(Condition* condition, State* state);

	void Enter(SceneObject* so);
	void Update(SceneObject* so, float delta);
	void Exit(SceneObject* so);
};

