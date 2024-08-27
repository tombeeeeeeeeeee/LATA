#pragma once
#include "Serialisation.h"

#include "Maths.h"

#include <vector>
#include <string>

class Collider;
class Scene;
class Shader;
class Transform;
class ModelRenderer;
class Ecco;
class Sync;
class Health;
class Animator;
class RigidBody;
struct Enemy;
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
};

class Scene;

class SceneObject
{
public:

	std::string name = "Unnamed Scene Object";
	unsigned long long GUID = 0;
	unsigned int parts = 0;
	Scene* scene;

	SceneObject(Scene* _scene, std::string name = "Unnamed Scene Object");
	SceneObject(Scene* scene, glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	~SceneObject();

	SceneObject(const SceneObject& other) = delete;
	SceneObject& operator=(const SceneObject& other) = delete;

	void Update(float delta);
	void GUI();

	toml::table Serialise();


#pragma region Part Get and Set

	void setTransform(Transform* transform);
	Transform* transform();

	void setRenderer(ModelRenderer* renderer);
	ModelRenderer* renderer();

	void setAnimator(Animator* animator);
	Animator* animator();

	//void setLight(Light light);
	//Light* light();

	void setRigidBody(RigidBody* rb);
	RigidBody* rigidbody();

	void setCollider(Collider* collider);
	Collider* collider();

	void setEcco(Ecco* ecco);
	void setEcco();
	Ecco* ecco();

	void setSync(Sync* sync);
	void setSync();
	Sync* sync();

	void setHealth(Health* health);
	Health* health();

	void setEnemy(Enemy* enemy);
	Enemy* enemy();

#pragma endregion

};



