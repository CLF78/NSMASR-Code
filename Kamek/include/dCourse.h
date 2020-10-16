#ifndef __DCOURSE_H
#define __DCOURSE_H

// Proper headers for Course, not the old shit

class dCourse_c {
	public:
		virtual ~dCourse_c();

		void loadFromFile(int courseID);

		struct tilesets_s {
			char name0[32];
			char name1[32];
			char name2[32];
			char name3[32];
		};

		struct settings_s {
			enum CourseFlag {
				UNK = 8,
				WRAP_ACROSS_EDGES = 1,
			};

			enum ToadHouseFlag {
				GREEN_HOUSE = 3,
				RED_HOUSE = 2,
				YELLOW_HOUSE = 1,
			};

			u64 defaultFlags;
			u16 courseFlag;
			s16 timeLimit;
			u8 creditsFlag;
			u8 unk[3];
			u8 startEntrance;
			u8 ambushFlag;
			u8 toadHouseFlag;
			u8 __;
		};

		struct bounding_s {
			s32 yBoundNormalTop;
			s32 yBoundNormalBottom;
			s32 yBoundSpecialTop;
			s32 yBoundSpecialBottom;
			u16 entryID;
			u16 yScrollEnabled;
			u8 _[4];
		};

		struct unk_s {
			u8 _[2];
			u16 unk;
			u16 unk2;
			u8 __[2];
		};

		struct bg_s {
			u16 entryID;
			s16 xScrollRate;
			s16 yScrollRate;
			s16 yPosOffset;
			s16 xPosOffset;
			u16 fileID1;
			u16 fileID2;
			u16 fileID3;
			u8 _[3];
			u8 scale;
			u8 __[4];
		};

		struct nextGoto_s {
			enum Flag {
				NO_ENTRY = 0x80,
				CONNECTED_PIPE = 8,
				LINK_FORWARD = 4,
				UNKNOWN_FLAG = 2,
				CONNECTED_REVERSE = 1,
			};

			u16 xPos;
			u16 yPos;
			s16 cameraXPos; // ?
			s16 cameraYPos; // ?
			u8 entryID;
			u8 destArea;
			u8 destEntrance;
			u8 type;
			u8 _; // Could be player suppression like in NSMBU?
			u8 zoneID;
			u8 layerID;
			u8 pathID;
			u8 __; // Could be player distance like in NSMBU?
			u8 flags;
			u16 pathExitDirection;
		};

		struct sprite_s {
			u16 type;
			s16 xPos;
			s16 yPos;
			u16 eventIDs;
			u32 settings;
			u8 zoneID;
			u8 layerID;
			u8 _[2];
		};

		struct load_s {
			u16 type;
			u8 _[2];
		};

		struct zone_s {
			s16 xPos;
			s16 yPos;
			s16 xSize;
			s16 ySize;
			u16 modelShading;
			u16 terrainShading;
			u8 entryID;
			u8 boundingID;
			u8 camMode;
			u8 camZoom;
			u8 relatedToEventCamera;
			u8 visibility;
			u8 fgID;
			u8 bgID;
			u8 mpBias; // Values known up to 3, but RE says it goes up to 7. Needs testing.
			u8 _;
			u8 music;
			u8 audioModifier;
		};

		struct rect_s {
			u16 xPos;
			u16 yPos;
			u16 xSize;
			u16 ySize;
			u8 entryID;
			u8 _[3];
		};

		struct cameraFlag_s {
			u8 _[12];
			u8 boundingId;
			u8 camMode;
			u8 camZoom;
			u8 zone_byte_0x10;
			u8 __[2];
			u8 eventId;
			u8 ___;
		};

		struct rail_s {
			enum Flag {
				LOOP = 2,
			};

			u8 entryID;
			u8 _;
			u16 startNode;
			u16 nodeCount;
			u16 flags;
		};

		struct railNode_s {
			u16 xPos;
			u16 yPos;
			float speed;
			float accel;
			s16 delay;
			u8 _[2];
		};


		int areaNum;


		union {
			struct {
				tilesets_s *tilesets;
				settings_s *settings;
				bounding_s *bounding;
				unk_s *unk;
				bg_s *topBG;
				bg_s *bottomBG;
				nextGoto_s *nextGoto;
				sprite_s *sprite;
				load_s *load;
				zone_s *zone;
				rect_s *rect;
				cameraFlag_s *cameraFlag;
				rail_s *rail;
				railNode_s *railNode;
			};
			void *blocks[14];
		};

		union {
			struct {
				int tilesetsSize, settingsSize, boundingSize, unkSize;
				int topBGSize, bottomBGSize, nextGotoSize, spriteSize;
				int loadSize, zoneSize, rectSize, cameraFlagSize;
				int railSize, railNodeSize;
			};
			int blockSizes[14];
		};

		union {
			struct {
				int tilesetsCount, settingsCount, boundingCount, unkCount;
				int topBGCount, bottomBGCount, nextGotoCount, spriteCount;
				int loadCount, zoneCount, rectCount, cameraFlagCount;
				int railCount, railNodeCount;
			};
			int blockCounts[14];
		};

		sprite_s *zoneFirstSprite[64];
		int zoneSpriteCount[64];
		int zoneFirstSpriteIdx[64];



		bounding_s *getBoundingByID(u8 id);
		bg_s *getTopBGByID(u16 id);
		bg_s *getBottomBGByID(u16 id);
		nextGoto_s *getNextGotoByID(u8 id);
		zone_s *getZoneByID(u8 id, mRect *output = 0);

		u8 getZoneID(u8 id);
		u8 getBoundingIDForZone(u8 id);
		u8 getScrollForZone(u8 id);
		u8 getZoomForZone(u8 id);
		u8 getUnk10ForZone(u8 id);
		u8 getMusicForZone(u8 id);
		u8 getAudioModifierForZone(u8 id);
		u8 getVisibilityForZone(u8 id);
		u8 getTopBGIDForZone(u8 id);
		u8 getBottomBGIDForZone(u8 id);
		u16 getModelShadingForZone(u8 id);
		u16 getTerrainShadingForZone(u8 id);
		u8 getMPBiasForZone(u8 id);
		u16 getWidthForZone(u8 id);

		rect_s *getRectByID(u8 id, mRect *output = 0);

		u8 getZoneIDContainingPosition(Vec *pos);
		u8 getZoneIDContainingRect(mRect16 *rect);

		bool doesZoneContainPosition(Vec *pos, zone_s *zone);
		bool doesZoneContainRect(mRect16 *rect, zone_s *zone);
};


class dCourseFull_c {
	public:
		dCourse_c courses[4];

		void loadFromFile();

		dCourse_c *get(int id) {
			if (courses[id].zone)
				return &courses[id];
			else
				return 0;
		}

		static dCourseFull_c *instance;

		static void createOnHeap(/*EGG::Heap*/void *heap);
		static void deleteInstance();

		// might not be part of this class, dunno
		static void generateLevelFilename(int world, int level, char *outBuffer);
		static char levelFilename[10];
};

#endif

