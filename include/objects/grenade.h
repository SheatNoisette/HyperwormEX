#ifndef __GRENADE_H__
#define __GRENADE_H__

#include "engine/actor.h"

#define ACT_T_GRENADE 2

// Dimensions (hitbox, collision and billboard)
#define GRENADE_SIZE 0.5f
// Launch speed of the grenade when shooting
#define GRENADE_LAUNCH_FORCE 20.0f
// Blast radius (in voxels) of the grenade
#define GRENADE_BLAST_RADIUS 4
// Damage radius (squared)
#define GRENADE_DAMAGE_RADIUS_2 50.0f
// Knockback force applied to actors when exploding
#define GRENADE_KNOCKBACK_FORCE 30.0f
#define GRENADE_KNOCKBACK_Y 8.0f
// Light intensity
#define GRENADE_BLAST_BRIGHTNESS 6.0f

typedef struct GrenadeData {
    // Nothing...
} GrenadeData;

// Function exports
ACTOR_DECLARE(grenade)

#endif
