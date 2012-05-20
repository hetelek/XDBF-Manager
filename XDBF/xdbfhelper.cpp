#include "xdbfhelper.h"
#include <stdlib.h>
#include <stdio.h>

void SwapEndian(void *array, int elemSize, int len)
{
    void *temp = new char[elemSize];
    if (temp == NULL)
        throw 1;

    for (int i = 0; i < (len / 2); i++)
    {
        memcpy(temp, ((char*)array) + (i * elemSize), elemSize);
        //dat crazy math
        memcpy(((char*)array) + (i * elemSize), ((char*)array) + (((len - 1) * elemSize) - (i * elemSize)), elemSize);
        memcpy(((char*)array) + (((len - 1) * elemSize) - (i * elemSize)), temp, elemSize);
    }

    delete temp;
}

void SwapEndian(unsigned short *s)
{
    SwapEndian(s, sizeof(char), sizeof(short));
}

void SwapEndian(unsigned int *i)
{
    SwapEndian(i, sizeof(char), sizeof(int));
}

void SwapEndian(unsigned long *l)
{
    SwapEndian(l, sizeof(char), sizeof(long));
}

void SwapEndian(unsigned long long *ll)
{
    SwapEndian(ll, sizeof(char), sizeof(long long));
}

void SwapEndianUnicode(wchar_t *str, int unicode_len)
{
    for (int i = 0; i < unicode_len / 2; i++)
        SwapEndian(&str[i], 1, 2);
}

struct tm* FILETIME_to_tm(FILETIME *pft)
{
    time_t nip_lick = FILETIME_to_time_t(pft);
    return localtime(&nip_lick);
}

time_t FILETIME_to_time_t(FILETIME *pft)
{
    long long ll; // 64 bit value
    ll = (((long long)(pft->dwHighDateTime)) << 32) + pft->dwLowDateTime;
    return (time_t)((ll - 116444736000000000) / 10000000);
}

std::string te_flag_to_string(TitleEntryFlag flag)
{
    switch(flag)
    {
        case 0:
            return "None.";
        case 1:
            return "Achievemment not synced";
        case 2:
            return "Download achievement image";
        case 16:
            return "Download avatar award";
        default:
            return "Unknown flag";
    }
}

std::string Entry_ID_to_string(unsigned long long name)
{
    //stupid switches can't use ulonglong's
    if(name == SYNC_LIST)
        return "Sync List";
    else if(name == SYNC_DATA)
        return "Sync Data";
    else if(name == OPTION_CONTROLLER_VIBRATION)
        return "Option Controller Vibration";
    else if(name == TITLE_SPECIFIC1)
        return "Title Specific 1";
    else if(name == TITLE_SPECIFIC2)
        return "Title Specific 2";
    else if(name == TITLE_SPECIFIC3)
        return "Title Specific 3";
    else if(name == GAMER_YAXIS_INVERSION)
        return "Gamer Yaxis Inversion";
    else if(name == GAMERCARD_ZONE)
        return "Gamercard Zone";
    else if(name == GAMERCARD_REGION)
        return "Gamercard Region";
    else if(name == GAMERCARD_CRED)
        return "Gamercard Cred";
    else if(name == GAMERCARD_REP)
        return "Gamercard Rep";
    else if(name == OPTION_VOICE_MUTED)
        return "Option Voice Muted";
    else if(name == OPTION_VOICE_THRU_SPEAKERS)
        return "Option Voice Thru Speakers";
    else if(name == OPTION_VOICE_THRU_SPEAKERS_RAW)
        return "Option Voice Thru Speakers Raw";
    else if(name == OPTION_VOICE_VOLUME)
        return "Option Voice Volume";
    else if(name == GAMERCARD_TITLES_PLAYED)
        return "Gamercard Titles Played";
    else if(name == GAMERCARD_ACHIEVEMENTS_EARNED)
        return "Gamercard Achievements Earned";
    else if(name == GAMER_DIFFICULTY)
        return "Gamer Delse ifficulty";
    else if(name == GAMER_CONTROL_SENSITIVITY)
        return "Gamer Control Sensitivity";
    else if(name == GAMER_PREFERRED_COLOR_FIRST)
        return "Gamer Preferred Color First";
    else if(name == GAMER_PREFERRED_COLOR_SECOND)
        return "Gamer Preferred Color Second";
    else if(name == GAMER_ACTION_AUTO_AIM)
        return "Gamer Action Auto Aim";
    else if(name == GAMER_ACTION_AUTO_CENTER)
        return "Gamer Action Auto Center";
    else if(name == GAMER_ACTION_MOVEMENT_CONTROL)
        return "Gamer Action Movement Control";
    else if(name == GAMER_RACE_TRANSMISSION)
        return "Gamer Race Transmission";
    else if(name == GAMER_RACE_CAMERA_LOCATION)
        return "Gamer Race Camera Location";
    else if(name == GAMER_RACE_BRAKE_CONTROL)
        return "Gamer Race Brake Control";
    else if(name == GAMER_RACE_ACCELERATOR_CONTROL)
        return "Gamer Race Accelerator Control";
    else if(name == GAMERCARD_TITLE_CRED_EARNED)
        return "Gamercard Title Cred Earned";
    else if(name == GAMERCARD_TITLE_ACHIEVEMENTS_EARNED)
        return "Gamercard Title Achievements Earned";
    else if(name == AVATAR_METADATA)
        return "Avatar Metadata";
    else if(name == GAMERCARD_PICTURE_KEY)
        return "Gamercard Picture Key";
    else if(name == GAMERCARD_MOTTO)
        return "Gamercard Motto";
    else if(name == TITLE_INFORMATION)
        return "Title Information";
    else if(name == GAMER_NAME)
        return "Gamer Name";
    else if(name == GAMER_LOCATION)
        return "Gamer Location";
    else if(name == AVATAR_INFORMATION)
        return "Avatar Information";
    else if(name == AVATAR_IMAGE)
        return "Avatar Image";
    return "";
}

FILETIME time_t_to_FILETIME(time_t unixTime)
{
    FILETIME time;
    long long ll; // 64 bit value
    ll = (unixTime * 10000000L) + 116444736000000000L;
    time.dwLowDateTime = (unsigned long)ll;
    time.dwHighDateTime = (unsigned long)(ll >> 32);

    return time;
}

std::string getClothingType(Avatar_Award_Entry *item)
{
        std::string clothingTypeFlags[11] =
        {
            "Helmet",
            "Shirt",
            "Trousers",
            "Shoes",
            "Hat",
            "Ring",
            "Glasses",
            "Wristwear",
            "Earrings",
            "Gloves/Ring",
            "Prop"
        };

        std::string toReturn = "";
        unsigned int flags = item->clothingType >> 2;
        for (int i = 0; i < 11; i++)
        {
            if (flags & 1)
                toReturn += clothingTypeFlags[i].append(", ");
            flags >>= 1;
        }

        return toReturn.substr(0, toReturn.length() - 2);;
}

std::string guid(Avatar_Award_Entry *item)
{
    char *guid = new char[38];
    unsigned short *segments = (unsigned short*)&item->flags64;
    sprintf(guid, "%08x-%04x-%04x-%04x-%04x%08x\0", item->clothingType, segments[3], segments[2], segments[1], segments[0], item->titleID);

    return std::string(guid);
}
