#pragma once

// index, lower, container stores, container, access from container, usual rep, set, get, addGUI, cls/strct, collection saved name, load func
#define ALL_PARTS \
	PART_ENTRY(1,  modelRenderer, ModelRenderer, renderers,     .,   , *, &, ModelRenderer,   class , Renderers,     ModelRenderer   )\
	PART_ENTRY(2,  animator,      Animator,      animators,     ->, *,  ,  , Animator,	      class , Animators,     Animator::Load  )\
	PART_ENTRY(3,  rigidBody ,    RigidBody,     rigidBodies,   .,   , *, &, RigidBody,	      class , RigidBodies,   RigidBody       )\
	PART_ENTRY(4,  pointLight,    PointLight,    pointLights,   .,   , *, &, PointLight,      class , PointLights,   PointLight      )\
	PART_ENTRY(5,  collider,      Collider,      colliders,     ->, *,  ,  , PolygonCollider, struct, Colliders,     Collider::Load  )\
	PART_ENTRY(6,  ecco,          Ecco,          eccos,         .,   , *, &, Ecco,            class , Ecco,          Ecco            )\
	PART_ENTRY(7,  sync,          Sync,          syncs,         .,   , *, &, Sync,            class , Sync,          Sync            )\
	PART_ENTRY(8,  health,        Health,        healths,       .,   , *, &, Health,          class , Healths,       Health          )\
	PART_ENTRY(9,  enemy,         Enemy,         enemies,       .,   , *, &, Enemy,           struct, Enemies,       Enemy           )\
	PART_ENTRY(10, exitElevator,  ExitElevator,  exits,         .,   , *, &, ExitElevator,    class , Exits,         ExitElevator    )\
	PART_ENTRY(11, spotlight,     Spotlight,     spotlights,    .,   , *, &, Spotlight,	      class , Spotlights,    Spotlight       )\
	PART_ENTRY(12, plate,         PressurePlate, plates,        .,   , *, &, PressurePlate,   class , Plates,        PressurePlate   )\
	PART_ENTRY(13, spawnManager,  SpawnManager,  spawnManagers, .,   , *, &, SpawnManager,    class , SpawnManagers, SpawnManager    )\
	PART_ENTRY(14, door,          Door,          doors,         .,   , *, &, Door,            class , Doors,         Door            )\
	PART_ENTRY(15, bollard,       Bollard,       bollards,      .,   , *, &, Bollard,         class , Bollards,      Bollard         )\
	PART_ENTRY(16, triggerable,   Triggerable,   triggerables,  .,   , *, &, Triggerable,     class , Triggerables,  Triggerable     )\
	PART_ENTRY(17, decal,         Decal,         decals,        .,   , *, &, Decal,           class , Decals,        Decal           )\
	PART_ENTRY(18, shadowWall,    ShadowWall,    shadowWalls,   .,   , *, &, ShadowWall,      struct, ShadowWalls,   ShadowWall      )\

