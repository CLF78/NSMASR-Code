#include <common.h>

class TimeKeeper {
	public:
		void *vtable; // 0x8031B358
		u32 timePlusFFFTimes40000;
		u16 startingTime;
		u8 isAmbush;
		u8 isTimeLessThan100;
		u8 isPaused;

		static TimeKeeper* instance; // 0x8042A350

		TimeKeeper(u32 *buffer, u32 initialTime); // 0x800E38E0
		~TimeKeeper(); // 0x800E3910

		void setTime(u32 time); // 0x800E3A00 - updates the u32. Maybe you need to manually set _B...
		void tick(); // 0x800E3A20 - updates the display and speeds up the music if needed.

		void handleTimeUp(); // 0x800E3B50
};
