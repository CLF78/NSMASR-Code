#include <game.h>
#include "boss.h"

void DamagePlayer(dEn_c *actor, ActivePhysics *apThis, ActivePhysics *apOther) {

	actor->dEn_c::playerCollision(apThis, apOther);
	actor->_vf220(apOther->owner);

	// fix multiple player collisions via megazig
	actor->deathInfo.isDead = 0;
	actor->flags_4FC |= (1<<(31-7));
	if (apOther->owner->which_player == 255 ) {
		actor->counter_504[0] = 0;
		actor->counter_504[1] = 0;
		actor->counter_504[2] = 0;
		actor->counter_504[3] = 0;
	}
	else {
		actor->counter_504[apOther->owner->which_player] = 0;
	}
}
