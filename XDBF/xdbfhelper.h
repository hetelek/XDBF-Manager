#ifndef XDBFHELPER_H
#define XDBFHELPER_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using std::string;
using std::wstring;

#define SYNC_LIST (unsigned long long)0x100000000
#define SYNC_DATA (unsigned long long)0x200000000
#define OPTION_CONTROLLER_VIBRATION (unsigned long long)0x10040003
#define TITLE_SPECIFIC1 (unsigned long long)0x63E83FFF
#define TITLE_SPECIFIC2 (unsigned long long)0x63E83FFE
#define TITLE_SPECIFIC3 (unsigned long long)0x63E83FFD
#define GAMER_YAXIS_INVERSION (unsigned long long)0x10040002
#define GAMERCARD_ZONE (unsigned long long)0x10040004
#define GAMERCARD_REGION (unsigned long long)0x10040005
#define GAMERCARD_CRED (unsigned long long)0x10040006
#define GAMERCARD_REP (unsigned long long)0x50040011
#define OPTION_VOICE_MUTED (unsigned long long)0x10040012
#define OPTION_VOICE_THRU_SPEAKERS (unsigned long long)0x10040013
#define OPTION_VOICE_THRU_SPEAKERS_RAW (unsigned long long)0x10040063
#define OPTION_VOICE_VOLUME (unsigned long long)0x10040014
#define GAMERCARD_TITLES_PLAYED (unsigned long long)0x10040018
#define GAMERCARD_ACHIEVEMENTS_EARNED (unsigned long long)0x10040019
#define GAMER_DIFFICULTY (unsigned long long)0x10040021
#define GAMER_CONTROL_SENSITIVITY (unsigned long long)0x10040024
#define GAMER_PREFERRED_COLOR_FIRST (unsigned long long)0x10040029
#define GAMER_PREFERRED_COLOR_SECOND (unsigned long long)0x10040030
#define GAMER_ACTION_AUTO_AIM (unsigned long long)0x10040034
#define GAMER_ACTION_AUTO_CENTER (unsigned long long)0x10040035
#define GAMER_ACTION_MOVEMENT_CONTROL (unsigned long long)0x10040036
#define GAMER_RACE_TRANSMISSION (unsigned long long)0x10040038
#define GAMER_RACE_CAMERA_LOCATION (unsigned long long)0x10040039
#define GAMER_RACE_BRAKE_CONTROL (unsigned long long)0x10040040
#define GAMER_RACE_ACCELERATOR_CONTROL (unsigned long long)0x10040041
#define GAMERCARD_TITLE_CRED_EARNED (unsigned long long)0x10040056
#define GAMERCARD_TITLE_ACHIEVEMENTS_EARNED (unsigned long long)0x10040057
#define AVATAR_METADATA (unsigned long long)0x63E80068
#define GAMERCARD_PICTURE_KEY (unsigned long long)0x4064000F
#define GAMERCARD_MOTTO (unsigned long long)0x402C0011
#define TITLE_INFORMATION (unsigned long long)0x8000
#define GAMER_NAME (unsigned long long)0x41040040
#define GAMER_LOCATION (unsigned long long)0x40520041
#define AVATAR_INFORMATION (unsigned long long)0x63e80044
#define AVATAR_IMAGE (unsigned long long)0x8007

//avatar awards
#define ID_FROM_FLAGS(flags) (flags >> 48)
#define GENDER_FROM_FLAGS(flags) ((flags >> 32) & 3)


typedef struct _FILETIME
{
    unsigned long dwHighDateTime;
    unsigned long dwLowDateTime;

} FILETIME;

typedef enum
{
    ACHIEVMENT_NOT_SYNCED = 1,
    DOWNLOAD_ACHIEVEMENT_IMAGE = 2,
    DOWNLOAD_AVATAR_AWARD = 16
} TitleEntryFlag;

typedef enum
{
    Completion = 1,             //The player has completed a game.
    Leveling,                   //The player has increased their level.
    Unlock,                     //The player has unlocked a new game feature.
    Event,                      //The player has completed a special goal in the game.
    Tournament,                 //The player has received an award in a tournament-level event.
    Checkpoint,                 //The player has reached a certain point or completed a specific portion of the game.
    Other,                      //Other achievement type.
    Secret,                     //Achievment details not shown before unlocked.
    UnlockedOnline = 0x10000,   //Indicates the achievement was achieved online.
    Unlocked = 0x20000          //Indicates the achievement was achieved.
} AchievmentFlag;

typedef enum
{
    Dequeue = 0,
    Enqueue
} SyncEntryStatus;

#pragma pack(push, 1)
//From Xbox 360 documentation
typedef struct
{
    unsigned char type;
    union
    {
        long i32_data;
        long long i64_data;
        double double_data;
        struct
        {
            unsigned long str_len_in_bytes;
            std::wstring *str;
        } unicode_string;

        float float_data;
        struct
        {
            unsigned long size;
            char *data;
        } binary;
        FILETIME time_stamp;
    };
} Setting_Entry;
//

typedef struct
{
    unsigned long long identifier, sync_id;
} Sync_Entry;

typedef struct
{
    unsigned int size;
    unsigned int id;
    unsigned int imageID;
    unsigned int gamerscore;
    unsigned int flags;
    FILETIME unlockedTime;
    std::wstring *name;
    std::wstring *lockedDescription;
    std::wstring *unlockedDescription;

} Achievement_Entry;

typedef struct
{
    unsigned int titleID;
    int achievementCount;
    int achievementUnlockedCount;
    unsigned int totalGamerscore;
    unsigned int gamerscoreUnlocked;
    unsigned char unknown; //need to find
    unsigned char achievementsUnlockedOnlineCount;
    unsigned char avatarAwardsEarned;
    unsigned char avatarAwardCount;
    unsigned char maleAvatarAwardsEarned;
    unsigned char maleAvatarAwardCount;
    unsigned char femaleAvatarAwardsEarned;
    unsigned char femaleAvaterAwardCount;
    unsigned int flags;
    FILETIME lastPlayed;
    std::wstring *gameName;

} Title_Entry;

struct Header
{
    unsigned int magic, version, entry_table_length, entry_count, free_memory_table_length, free_memory_table_entry_count;
};

struct FreeMemoryEntry
{
    unsigned int offsetSpecifier, length;
};

struct FreeMemoryTable
{
    unsigned int tableLength, entryCount;
    FreeMemoryEntry *entries;
};

struct Entry
{
    unsigned short type;
    unsigned long long identifier;
    unsigned int address, length;
};

typedef struct
{
    Entry *data_entry;
    unsigned long long next_sync_id, last_sync_id;
    FILETIME last_synced_time;
} Sync_Data;

typedef struct
{
    Sync_Data sync_data;
    Entry *list_entry;
    int entry_count;
    Sync_Entry *entries;
} Sync_List;

typedef struct
{
    Entry entry;

    unsigned int size;
    unsigned int clothingType;
    unsigned long long flags64; //top 16 bits are the id
    unsigned int titleID;
    unsigned int imageID;
    unsigned int flags32; //same as achievement
    FILETIME unlockTime;
    char unknown[8];
    std::wstring *name;
    std::wstring *lockedDescription;
    std::wstring *unlockedDescription;

} Avatar_Award_Entry;

#pragma pack(pop)

typedef enum
{
    male = 1,
    female,
    both

} AvatarAwardGender;


//Functions
void SwapEndian(void *array, int elemSize, int len);
void SwapEndian(unsigned short *s);
void SwapEndian(unsigned int *i);
void SwapEndian(unsigned long *l);
void SwapEndian(unsigned long long *ll);
void SwapEndianUnicode(wchar_t *str, int unicode_len);
std::string te_flag_to_string(TitleEntryFlag flag);
struct tm* FILETIME_to_tm(FILETIME *pft);
std::string Entry_ID_to_string(unsigned long long name);
time_t FILETIME_to_time_t(FILETIME *pft);
FILETIME time_t_to_FILETIME(time_t unixTime);
std::string getClothingType(Avatar_Award_Entry *item);
std::string guid(Avatar_Award_Entry *item);

#endif // CHANGE_ENDIANESS_H
