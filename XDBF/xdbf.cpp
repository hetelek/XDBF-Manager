#include "xdbf.h";
#include <algorithm>

XDBF::XDBF(const char* path)
{
    int i = 1;
    if (*(char*)&i == 1)
        reverse = true;
    else
        reverse = false;

    fopen_s(&opened_file, path, "rb+");

    if(opened_file == NULL)
    {
        //File could not be opened
        throw "File could not be opened.";
    }

    fseek(opened_file, 0, SEEK_SET);
    fread(h, sizeof(Header), 1, opened_file);

    SwapEndian(&h->magic);
    SwapEndian(&h->entry_count);
    SwapEndian(&h->entry_table_length);
    SwapEndian(&h->free_space_table_entry_count);
    SwapEndian(&h->free_space_table_length);
    SwapEndian(&h->version);

    if(h->magic != 0x58444246)
    {
        throw "Invalid file. (MAGIC)";
    }

    Entry *temp_entries = new Entry[h->entry_count];

    fread(temp_entries, sizeof(Entry), h->entry_count, opened_file);

    for(unsigned int i = 0; i < h->entry_count; i++)
    {
        SwapEndian(&temp_entries[i].type);
        SwapEndian(&temp_entries[i].identifier);
        SwapEndian(&temp_entries[i].length);
        SwapEndian(&temp_entries[i].address);

        temp_entries[i].address = get_offset(temp_entries[i].address, h);
        private_entries.push_back(temp_entries[i]);
    }

    delete[] temp_entries;
}

XDBF::~XDBF()
{
    delete opened_file;
}

Entry* XDBF::get_entry_by_id(long long identifier, int type)
{
    Entry *entries = get_entries();
    for(unsigned int i = 0; i < get_header()->entry_count; i++)
        if(entries[i].identifier == identifier && entries[i].type == type)
            return &entries[i];

    return NULL;
}

char* XDBF::extract_entry(Entry *entry)
{
    if(opened_file == NULL)
        throw "File has been closed/never opened.";

    fseek(opened_file, entry->address, SEEK_SET);
    char *data = new char[entry->length];

    fread(data, entry->length, sizeof(char), opened_file);

    return data;
}

Entry* XDBF::get_entries()
{
    return &private_entries.at(0);
}

Header* XDBF::get_header()
{
    return h;
}

void XDBF::close()
{
    if(opened_file == NULL)
        return;

    fclose(opened_file);
    opened_file = NULL;
}

int XDBF::get_offset(unsigned int offset_specifier, Header* h)
{
    int entry_table_size = (h->entry_table_length * 18);
    int free_space_size = (h->free_space_table_length * 8);
    return ((entry_table_size + free_space_size) + 24) + offset_specifier;
}

int XDBF::getFakeOffset(unsigned int realAddress)
{
    int entry_table_size = (h->entry_table_length * 18);
    int free_space_size = (h->free_space_table_length * 8);
    return (realAddress - ((entry_table_size + free_space_size) + 24));
}

FILE *XDBF::get_file()
{
    return opened_file;
}

Setting_Entry* XDBF::get_setting_entry(Entry *entry)
{
    if (entry->type != ET_SETTING)
        return NULL;

    Setting_Entry *s_entry = new Setting_Entry;
    fseek(opened_file, entry->address + 0x8, SEEK_SET);
    fread(&s_entry->type, 1, 1, opened_file);
    fseek(opened_file, entry->address + 0x10, SEEK_SET);

    wchar_t *str_temp;
    switch (s_entry->type)
    {
        case SET_CONTEXT:
            //idk
            break;

        case SET_INT32:
            fread(&s_entry->i32_data, 4, 1, opened_file);
            SwapEndian((unsigned int*)&s_entry->i32_data);
            break;

        case SET_INT64:
            fread(&s_entry->i64_data, 8, 1, opened_file);
            SwapEndian((unsigned long long*)&s_entry->i64_data);
            break;

        case SET_DOUBLE:
            fread(&s_entry->double_data, 8, 1, opened_file);
            SwapEndian((unsigned long long*)&s_entry->double_data);
            break;

        case SET_UNICODE:
            fread(&s_entry->unicode_string.str_len_in_bytes, 4, 1, opened_file);
            SwapEndian((unsigned int*)&s_entry->unicode_string.str_len_in_bytes);
            fseek(opened_file, entry->address + 0x18, SEEK_SET);
            str_temp = (wchar_t*)malloc(s_entry->unicode_string.str_len_in_bytes);
            fread(str_temp, 1, s_entry->unicode_string.str_len_in_bytes, opened_file);
            s_entry->unicode_string.str = str_temp;
            SwapEndianUnicode(s_entry->unicode_string.str, s_entry->unicode_string.str_len_in_bytes);
            break;

        case SET_FLOAT:
            fread(&s_entry->float_data, 4, 1, opened_file);
            SwapEndian((unsigned int*)&s_entry->float_data);
            break;

        case SET_BINARY:
            unsigned long bin_size;
            fread(&bin_size, 4, 1, opened_file);
            SwapEndian((unsigned int*)&bin_size);
            s_entry->binary.size = bin_size;
            s_entry->binary.data = new char[bin_size];
            fseek(opened_file, entry->address + 0x18, SEEK_SET);
            fread(s_entry->binary.data, bin_size, 1, opened_file);
            break;

        case SET_DATETIME:
            fread(&s_entry->time_stamp, 8, 1, opened_file);
            SwapEndian(&s_entry->time_stamp, 1, 8);
            break;

        case SET_NULL:
            break;

        default:
            throw "Invalid setting entry type.";
    }
    return s_entry;
}

std::string XDBF::get_setting_entry_name(Setting_Entry *s_entry)
{
    if(s_entry->type == 0xFF)
        return "NULL";
    else if(s_entry->type > 7)
        throw "Invalid setting entry type.";

    std::string friendly_setting_types[8] = { "Sync Entry", "Int32", "Int64", "Double", "Unicode String", "Float", "Binary", "Datetime"};
    return friendly_setting_types[s_entry->type];
}

Title_Entry* XDBF::get_title_entry(Entry *entry)
{
    if (entry->type != ET_TITLE || entry->identifier == 0x200000000 || entry->identifier == 0x100000000)
        return NULL;

    Title_Entry *t_entry = new Title_Entry;
    fseek(opened_file, entry->address, SEEK_SET);
    fread(t_entry, 1, 40, opened_file);

    SwapEndian((unsigned int*)&t_entry->achievementCount);
    SwapEndian((unsigned int*)&t_entry->achievementUnlockedCount);
    SwapEndian(&t_entry->flags);
    SwapEndian(&t_entry->gamerscoreUnlocked);
    SwapEndian((unsigned int*)&t_entry->lastPlayed);
    SwapEndian(&t_entry->titleID);
    SwapEndian(&t_entry->totalGamerscore);

    wchar_t *game_name = (wchar_t*)malloc(entry->length - 0x28);
    fseek(opened_file, (entry->address + 0x28), SEEK_SET);
    fread(game_name, 1, (entry->length - 0x28), opened_file);
    SwapEndianUnicode(game_name, (entry->length - 0x28));
    t_entry->gameName = game_name;

    return t_entry;
}

Achievement_Entry* XDBF::get_achievement_entry(Entry *entry)
{
    if (entry->type != ET_ACHIEVEMENT)
        return NULL;

    Achievement_Entry *chiev = new Achievement_Entry;

    fseek(opened_file, entry->address, SEEK_SET);
    fread(chiev, 1, 0x1C, opened_file);

    swapAchievementEndianness(chiev);

    int nameLen = wide_string_length(entry->address + 0x1C);
    chiev->name = (wchar_t*)malloc(nameLen);
    fseek(opened_file, entry->address + 0x1C, SEEK_SET);
    fread(chiev->name, 1, nameLen, opened_file);
    SwapEndianUnicode(chiev->name, nameLen);

    int lockedDescLen = wide_string_length(entry->address + 0x1C + nameLen);
    chiev->lockedDescription = (wchar_t*)malloc(lockedDescLen);
    fseek(opened_file, entry->address + 0x1C + nameLen, SEEK_SET);
    fread(chiev->lockedDescription, 1, lockedDescLen, opened_file);
    SwapEndianUnicode(chiev->lockedDescription, lockedDescLen);

    int unlockedDescLen = wide_string_length(entry->address + 0x1C + nameLen + lockedDescLen);
    chiev->unlockedDescription = (wchar_t*)malloc(unlockedDescLen);
    fseek(opened_file, entry->address + 0x1C + nameLen + lockedDescLen, SEEK_SET);
    fread(chiev->unlockedDescription, 1, unlockedDescLen, opened_file);
    SwapEndianUnicode(chiev->unlockedDescription, unlockedDescLen);

    return chiev;
}

int XDBF::wide_string_length(long pos)
{
    fseek(opened_file, pos, SEEK_SET);

    int strLen = 0;
    for (;;)
    {
        wchar_t temp;
        fread(&temp, 2, 1, opened_file);
        if (temp != 0)
            strLen += 2;
        else
            return strLen + 2;
    }
}

Sync_List XDBF::get_sync_list(int et_type, unsigned long long identifier)
{
    Entry *syncListTarget = get_entry_by_id(identifier, et_type);
    if(syncListTarget == NULL)
        throw "Unable to locate entry.";

    int syncsInList = syncListTarget->length / 0x10;

    Sync_List list;
    list.sync_data = get_sync_data(et_type, identifier * 2);

    list.list_entry = syncListTarget;
    list.entry_count = syncsInList;

    Sync_Entry *sync_entries = new Sync_Entry[syncsInList];

    fseek(opened_file, syncListTarget->address, SEEK_SET);
    fread(sync_entries, 0x10, syncsInList, opened_file);

    for(int i = 0; i < syncsInList; i++)
    {
        SwapEndian(&sync_entries[i].identifier);
        SwapEndian(&sync_entries[i].sync_id);
    }

    list.entries = sync_entries;
    return list;
}

std::string XDBF::FILETIME_to_string(FILETIME *pft)
{
    struct tm *timeinfo = FILETIME_to_tm(pft);
    if (timeinfo == NULL)
        return "<i>N/A</i>";

    return string(asctime(timeinfo));
}

void XDBF::update_sync_list_entry(Sync_Entry entry, int et_type, SyncEntryStatus status, unsigned long long identifier)
{
    if(entry.sync_id != 0 && status == Enqueue)
        return; //Already on sync queue.
    else if(entry.sync_id == 0 && status == Dequeue)
        return;

    Sync_List list = get_sync_list(et_type, identifier);

    bool found = false;
    vector<Sync_Entry> entries;

    if(status == Enqueue)
    {
        int highestSyncId = 0;
        int highestSyncIdIndex = 0;
        for(int i = 0; i < list.entry_count; i++)
        {
            if(list.entries[i].sync_id > highestSyncId)
            {
                highestSyncId = list.entries[i].sync_id;
                highestSyncIdIndex = i;
            }

            if(memcmp(&entry, &list.entries[i], sizeof(entry)) == 0)
                found = true;
            else
            {
                entries.push_back(list.entries[i]);

                Sync_Entry *entr = &entries.at(entries.size() - 1);
                SwapEndian(&entr->identifier);
                SwapEndian(&entr->sync_id);
            }
        }

        if(!found)
            throw "Could not find given Sync List Entry.";

        entry.sync_id = highestSyncId + 1;
        SwapEndian(&entry.sync_id);
        SwapEndian(&entry.identifier);

        entries.insert(entries.begin() + (highestSyncIdIndex - 1), 1, entry);
        fseek(opened_file, list.list_entry->address, SEEK_SET);
        fwrite(&entries.at(0), 0x10, list.entry_count, opened_file);
    }
    else if(status == Dequeue)
    {
        for(int i = 0; i < list.entry_count; i++)
            if(memcmp(&entry, &list.entries[i], sizeof(entry)) == 0)
            {
                list.entries[i].sync_id = 0;
                found = true;
            }

        if(!found)
            throw "Could not find given Sync List Entry.";
        write_sync_list(&list);
    }
}

void XDBF::write_sync_list(Sync_List *sl)
{
    Entry *entr1 = sl->list_entry;
    if(sl->entry_count * 0x10 != entr1->length)
        throw "Requested sync list(to be written) differs in size from current sync list.";

    vector<Sync_Entry> queuedEntries, entries;

    for(int i = 0; i < sl->entry_count; i++)
    {
        if(sl->entries[i].sync_id != 0)
        {
            queuedEntries.push_back(sl->entries[i]);

            Sync_Entry *entr = &queuedEntries.at(queuedEntries.size() - 1);
            SwapEndian(&entr->identifier);
            SwapEndian(&entr->sync_id);
        }
        else
        {
            entries.push_back(sl->entries[i]);

            Sync_Entry *entr = &entries.at(entries.size() - 1);
            SwapEndian(&entr->identifier);
            SwapEndian(&entr->sync_id);
        }
    }

    for(int i = 0; i < entries.size(); i++)
    {
        fseek(opened_file, entr1->address + (0x10 * i), SEEK_SET);
        fwrite(&entries.at(i), 0x10, 1, opened_file);
    }

    int startingPos = entries.size() * 0x10;
    for(int i = 0; i < queuedEntries.size(); i++)
    {
        fseek(opened_file, entr1->address + (startingPos + (0x10 * i)), SEEK_SET);
        fwrite(&queuedEntries.at(i), 0x10, 1, opened_file);
    }

    unsigned long long next = queuedEntries.size() + 1;
    sl->sync_data.next_sync_id = next;
    SwapEndian(&next);
    fseek(opened_file, sl->sync_data.data_entry->address, SEEK_SET);
    fwrite(&next, 8, 1, opened_file);
}

Sync_Data XDBF::get_sync_data(int et_type, unsigned long long identifier)
{
    Entry *target = get_entry_by_id(identifier, et_type);
    if(target == NULL)
        throw "Sync data could not be found with given entry type.";

    Sync_Data data;
    data.data_entry = target;

    unsigned long long *entr_data = (unsigned long long*)extract_entry(target);
    for(int i = 0; i < 3; i++)
        SwapEndian(&entr_data[i]);

    data.next_sync_id = entr_data[0];
    data.last_sync_id = entr_data[1];
    data.last_synced_time = *(FILETIME*)&entr_data[2];

    return data;
}

Sync_Entry* XDBF::get_sync(int et_type, unsigned long long identifier)
{
    Sync_List syncs = get_sync_list(et_type, identifier);

    for (int i = 0; i < syncs.entry_count; i++)
        if (syncs.entries[i].identifier == identifier)
                return &syncs.entries[i];

    return NULL;
}

void XDBF::writeEntry(Entry *entry, Achievement_Entry *chiev)
{
    Achievement_Entry temp = *chiev;

    SwapEndian(&temp.size);
    SwapEndian(&temp.id);
    SwapEndian(&temp.imageID);
    SwapEndian(&temp.gamerscore);
    SwapEndian((unsigned long long*)&temp.unlockedTime);

    fseek(opened_file, entry->address, SEEK_SET);
    fwrite(&temp, 0x1C, 1, opened_file);

    update_sync_list_entry(*get_sync(ET_ACHIEVEMENT, entry->address), ET_ACHIEVEMENT, Enqueue);
}

Avatar_Award_Entry* XDBF::get_avatar_award_entry(Entry *entry)
{
    Avatar_Award_Entry *entryAA = new Avatar_Award_Entry;
    entryAA->entry = *entry;
    fseek(opened_file, entry->address, SEEK_SET);
    fread((char*)&entryAA->size, 0x2C, 1, opened_file);

    SwapEndian(&entryAA->size);
    SwapEndian(&entryAA->clothingType);
    SwapEndian(&entryAA->flags64);
    SwapEndian(&entryAA->titleID);
    SwapEndian(&entryAA->imageID);
    SwapEndian(&entryAA->flags32);
    SwapEndian((unsigned long long*)&entryAA->unlockTime);
    SwapEndian((unsigned int*)&entryAA->unknown[0]);
    SwapEndian((unsigned int*)&entryAA->unknown[3]);

    unsigned long temp = entryAA->unlockTime.dwHighDateTime;
    entryAA->unlockTime.dwHighDateTime = entryAA->unlockTime.dwLowDateTime;
    entryAA->unlockTime.dwLowDateTime = temp;

    int nameLen = wide_string_length(entry->address + 0x2C);
    entryAA->name = (wchar_t*)malloc(nameLen);
    fseek(opened_file, entry->address + 0x2C, SEEK_SET);
    fread(entryAA->name, 1, nameLen, opened_file);
    SwapEndianUnicode(entryAA->name, nameLen);

    int unlockedDescLen = wide_string_length(entry->address + 0x2C + nameLen);
    entryAA->unlockedDescription = (wchar_t*)malloc(unlockedDescLen);
    fseek(opened_file, entry->address + 0x2C + nameLen, SEEK_SET);
    fread(entryAA->unlockedDescription, 1, unlockedDescLen, opened_file);
    SwapEndianUnicode(entryAA->unlockedDescription, unlockedDescLen);

    int lockedDescLen = wide_string_length(entry->address + 0x2C + nameLen + unlockedDescLen);
    entryAA->lockedDescription = (wchar_t*)malloc(lockedDescLen);
    fseek(opened_file, entry->address + 0x2C + nameLen + unlockedDescLen, SEEK_SET);
    fread(entryAA->lockedDescription, 1, lockedDescLen, opened_file);
    SwapEndianUnicode(entryAA->lockedDescription, lockedDescLen);

    return entryAA;
}

void XDBF::writeEntry(Avatar_Award_Entry *entry)
{
    Avatar_Award_Entry temp = *entry;
    SwapEndian(&temp.size);
    SwapEndian(&temp.clothingType);
    SwapEndian(&temp.flags64);
    SwapEndian(&temp.titleID);
    SwapEndian(&temp.imageID);
    SwapEndian(&temp.flags32);
    SwapEndian((unsigned long long*)&temp.unlockTime);

    unsigned long tempLong = temp.unlockTime.dwHighDateTime;
    temp.unlockTime.dwHighDateTime = temp.unlockTime.dwLowDateTime;
    temp.unlockTime.dwLowDateTime = tempLong;

    fseek(opened_file, temp.entry.address, SEEK_SET);
    fwrite(&temp.size, 0x2C, 1, opened_file);
}

void XDBF::injectEntry_private(unsigned int type, char *entryData, unsigned int dataLen)
{
    h->entry_count++;
    Entry newEntry = { type, 0, 0, dataLen };

    //need to update sync list stuffs

    //update entry count
    fseek(opened_file, 0xC, SEEK_SET);
    SwapEndian(&h->entry_count);
    fwrite(&h->entry_count, 1, 4, opened_file);
    SwapEndian(&h->entry_count);

    //append entry to file
    fseek(opened_file, 0, SEEK_END);
    newEntry.address = ftell(opened_file);
    fwrite(entryData, dataLen, 1, opened_file);

    //add the new entry to the table
    private_entries.push_back(newEntry);
    sort(private_entries.begin(), private_entries.end(), &compareFunction);

    //re-write entry table
    for (int i = 0; i < h->entry_count; i++)
    {
        private_entries[i].address = getFakeOffset(private_entries[i].address);

        SwapEndian(&private_entries[i].type);
        SwapEndian(&private_entries[i].identifier);
        SwapEndian(&private_entries[i].length);
        SwapEndian(&private_entries[i].address);

        fseek(opened_file, (i * 0x12) + 0x18, SEEK_SET);
        fwrite(&private_entries[i], 0x12, 1, opened_file);

        SwapEndian(&private_entries[i].type);
        SwapEndian(&private_entries[i].identifier);
        SwapEndian(&private_entries[i].length);
        SwapEndian(&private_entries[i].address);

        private_entries[i].address = get_offset(private_entries[i].address, h);
    }
}

void XDBF::injectAchievementEntry(Achievement_Entry *entry)
{
    int nameLen = (wcslen(entry->name) + 1) * 2;
    int lockedDescLen = (wcslen(entry->lockedDescription) + 1) * 2;
    int unlockedDescLen = (wcslen(entry->unlockedDescription) + 1) * 2;

    if (entry->id == 0)
    {
        int maxId = private_entries[0].identifier, maxImageID = private_entries[0].identifier;
        for (int i = 0; i < private_entries.size(); i++)
        {
            if (maxId < private_entries[i].identifier && private_entries[i].type == ET_ACHIEVEMENT)
                maxId = private_entries[i].identifier;
            else if (maxImageID < private_entries[i].identifier && private_entries[i].type == ET_IMAGE)
                maxImageID = private_entries[i].identifier;
        }
        entry->id = maxId;
    }

    entry->size = 0x1C;

    char *data = new char[0x1C + nameLen + lockedDescLen + unlockedDescLen];
    wchar_t *nameCpy = new wchar_t[wcslen(entry->name)];
    wchar_t *lockedDescCpy = new wchar_t[wcslen(entry->lockedDescription)];
    wchar_t *unlockedDescCpy = new wchar_t[wcslen(entry->unlockedDescription)];

    memcpy(nameCpy, entry->name, nameLen);
    memcpy(lockedDescCpy, entry->lockedDescription, lockedDescLen);
    memcpy(unlockedDescCpy, entry->unlockedDescription, unlockedDescLen);

    swapAchievementEndianness(entry);
    memcpy(data, &entry->size, 0x1C);
    swapAchievementEndianness(entry);

    SwapEndianUnicode(nameCpy, nameLen);
    SwapEndianUnicode(lockedDescCpy, lockedDescLen);
    SwapEndianUnicode(unlockedDescCpy, unlockedDescLen);

    memcpy(&data[0x1C], nameCpy, nameLen);
    memcpy(&data[0x1C + nameLen], lockedDescCpy, lockedDescLen);
    memcpy(&data[0x1C + nameLen + lockedDescLen], unlockedDescCpy, unlockedDescLen);

    injectEntry_private(ET_ACHIEVEMENT, data, 0x1C + nameLen + lockedDescLen + unlockedDescLen);
}

void XDBF::swapAchievementEndianness(Achievement_Entry *entry)
{
    SwapEndian(&entry->size);
    SwapEndian(&entry->id);
    SwapEndian(&entry->imageID);
    SwapEndian(&entry->gamerscore);
    SwapEndian(&entry->flags);
    SwapEndian((unsigned long long*)&entry->unlockedTime);
}

//for sorting entries
bool compareFunction(Entry e1, Entry e2)
{
    return (e1.type < e2.type);
}


