#pragma once

#include <soloud.h>
#include <soloud_wav.h>
#undef min
#undef max

#include <string>
#include <array>

// TODO: Check if using what backend should be

// TODO: Record stats on times sounds have been played to ensure that they aren't missing

class Audio
{
private:

	void CheckForMissingPaths();

public:

	enum SoundIndex : unsigned int {
		railgunMisfire,
		railgunShotFirstCharged,
		railgunShotSecondCharged,
		railgunFirstChargeReached,
		railgunSecondChargeReached,
		railgunHoldMaxCharge,
		railgunHitEnemy,
		syncFootSteps,
		syncDeath,
		syncDamageTaken,
		eccoDrive,
		eccoBoost,
		eccoEnemyHit,
		eccoObstacleHit,
		eccoDeath,
		eccoDamageTaken,
		rainbowShot,
		rainbowShotMiss,
		enemyDeathRammed,
		enemyRandomNoise,
		enemyExplode,
		enemyRangedShot,
		enemyRangedCharging,
		enemyStunned,
		enemyDeathShot,
		enemyHitByShot,
		bollardMove,
		doorMove,
		pressurePlateActivate,
		healingAbilityActivate,
		healingAbilityFailOnCooldown,
		healingAbilityFailOutOfRange,
		healingAbilityDeactivate,
		uiButtonPress,
		pauseOpen,
		pauseClose,
		uiBack,
		mainMenuMusic,
		backTrack1,
		backTrack2,
		COUNT
	};

	class Sound {
	public:
		std::string filename = "";
		bool loaded = false;
		SoLoud::Wav wav;
		bool loop = false;
		void Load();
	};
	std::array<Sound, COUNT> sounds;

	SoLoud::Soloud soloud;
	void Initialise();
	void Deinitialise();

	void GUI();

	void PlaySound(SoundIndex index, SoLoud::Bus* bus = nullptr);

	void EnsureAllSoundsLoaded();

	// Busses are like mixers
	SoLoud::Bus musicBus;
	float musicBusVolume = 1.0f;
	int musicBusHandle;
};

