#pragma once

#include <string>

struct Collider;
class Scene;
class Shader;
class Transform;
class ModelRenderer;
class Ecco;
class Sync;
class Health;
class Animator;
class RigidBody;
class ExitElevator;
struct Enemy;
namespace toml {
	inline namespace v3 {
		class table;
	}
}
// TODO: Look into how instancing should be handled

//TODO: Add GetPart Functions
enum Parts
{
	transform =     1 << 0,
	modelRenderer = 1 << 1,
	animator =      1 << 2,
	rigidBody =     1 << 3,
	light =         1 << 4,
	collider =      1 << 5,
	ecco =          1 << 6,
	sync =          1 << 7,
	health =        1 << 8,
	enemy =         1 << 9,
	exitElevator =	1 << 10,
	spikes =		1 << 11,
	plate = 		1 << 12,
};

class Scene;

class SceneObject
{
public:

	std::string name = "Unnamed Scene Object";
	unsigned long long GUID = 0;
	unsigned int parts = 0;
	Scene* scene = nullptr;
	SceneObject(Scene* _scene, std::string name = "Unnamed Scene Object");
	SceneObject(Scene* _scene, toml::table* table);

	~SceneObject();

	//SceneObject(const SceneObject& other) = delete;
	//SceneObject& operator=(const SceneObject& other) = delete;

	void GUI();

	toml::table Serialise() const;


#pragma region Part Get and Set

	// TODO: Consider getting rid of the setTransform?
	void setTransform(Transform* transform);
	Transform* transform() const;

	void setRenderer(ModelRenderer* renderer);
	ModelRenderer* renderer();

	void setAnimator(Animator* animator);
	Animator* animator();

	//void setLight(Light light);
	//Light* light();

	void setRigidBody(RigidBody* rb);
	// TODO: Should be a cap b to match the rest?
	RigidBody* rigidbody();

	void setCollider(Collider* collider);
	Collider* collider();

	void setEcco(Ecco* ecco);
	void setEcco();
	Ecco* ecco() const;

	void setSync(Sync* sync);
	void setSync();
	Sync* sync() const;

	void setHealth(Health* health);
	Health* health();

	void setEnemy(Enemy* enemy);
	Enemy* enemy();

	void setExitElevator(ExitElevator* exitElevator);
	ExitElevator* exitElevator();

	// Make this take a mask for stuff to delete
	void ClearParts();

#pragma endregion

};



