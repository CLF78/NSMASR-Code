#include <game.h>
#include "boss.h"

void DamagePlayer(dEn_c *actor, ActivePhysics *apThis, ActivePhysics *apOther) {
	actor->dEn_c::playerCollision(apThis, apOther);
	actor->_vf220(apOther->owner);

	// Fix multiple player collisions (by megazig)
	actor->deathInfo.isDead = 0;
	actor->flags_4FC |= (1<<(31-7));

	if (apOther->owner->which_player == 255 ) {
		for (int i = 0; i < 4; i++)
			actor->counter_504[i] = 0; }
	else
		actor->counter_504[apOther->owner->which_player] = 0;
}

void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const char *source) {
	int i = 0;
	wchar_t buffer[1024];
	while (i < 1023 && source[i]) {
		buffer[i] = source[i];
		i++;
	}
	buffer[i] = 0;

	tb->SetString(buffer);
}
