#pragma once

enum class EnemyType
{
	basic = 1,
	smart = 2,
	other = 3,
};

struct Enemy
{
	float moveSpeed = 0.0f;
	float detectionRange = 0.0f;
	float damageAmount = 0.0f;
	EnemyType type = EnemyType::basic;
};

