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
		syncFootSteps00,
		syncFootSteps01,
		syncFootSteps02,
		syncFootSteps03,
		syncFootSteps04,
		syncFootSteps05,
		syncFootSteps06,
		syncFootSteps07,
		syncFootSteps08,
		syncFootSteps09,
		syncFootSteps10,
		syncFootSteps11,
		syncFootSteps12,
		syncFootSteps13,
		syncFootSteps14,
		syncFootSteps15,
		syncFootSteps16,
		syncFootSteps17,
		syncFootSteps18,
		syncFootSteps19,
		syncFootSteps20,
		syncFootSteps21,
		syncFootSteps22,
		syncFootSteps23,
		syncFootSteps24,
		syncFootSteps25,
		syncFootSteps26,
		syncFootSteps27,
		syncFootSteps28,
		syncFootSteps29,
		syncDeath,
		syncDamageTaken0,
		syncDamageTaken1,
		syncDamageTaken2,
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
		enemyHitByShot, //TODO: When is this supposed to be played, there is already enemy getting hit by other things
		dabMove00,
		dabMove01,
		dabMove02,
		dabMove03,
		dabMove04,
		dabMove05,
		dabMove06,
		dabMove07,
		dabMove08,
		dabMove09,
		dabMove10,
		dabMove11,
		dabMove12,
		dabMove13,
		dabMove14,
		dabMove15,
		dabMove16,
		dabMove17,
		dabMove18,
		dabMove19,
		dabMove20,
		dabMove21,
		dabMove22,
		dabMove23,
		dabMove24,
		dabMove25,
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
		unsigned int timesPlayed = 0;
	};

	class Mixer {
	public:
		SoLoud::Bus bus;
		int handle = -1;
		bool initialised = false;
		void Init(SoLoud::Soloud& soloud);
	};

	std::array<Sound, COUNT> sounds;

	SoLoud::Soloud soloud;
	void Initialise();
	void Deinitialise();

	void GUI();

	SoLoud::handle PlaySound(SoundIndex index, Mixer* mixer = nullptr);

	void EnsureAllSoundsLoaded();

	Mixer musicMixer;
	Mixer testMixer;
};

