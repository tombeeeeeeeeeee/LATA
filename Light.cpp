#include "Light.h"

Light::Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular)
{
}

DirectionalLight::DirectionalLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction) : 
	Light(_ambient, _diffuse, _specular),
	direction(_direction)
{
}

PointLight::PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic) :
	Light(_ambient, _diffuse, _specular),
	position(_position),
	constant(_constant),
	linear(_linear),
	quadratic(_quadratic)
{
}

SpotLight::SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff) : PointLight(_ambient, _diffuse, _specular, _position, _constant, _linear, _quadratic),
	direction(_direction), cutOff(_cutOff), outerCutOff(_outerCutOff)
{
}
	