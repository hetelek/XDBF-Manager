#include "xdbf.h";
#include <algorithm>

XDBF::XDBF(const char* path)
{
    opened_file = new FileIO(path);

    if(opened_file == NULL)
    {
        //File could not be opened
        throw "File could not be opened.";
    }

    opened_file->setPosition(0);
    h->magic = opened_file->readUInt32();
    h->version = opened_file->readUInt32();
    h->entry_table_length = opened_file->readUInt32();
    h->entry_count = opened_file->readUInt32();
    h->free_memory_table_length = opened_file->readUInt32();
    h->free_memory_table_entry_count = opened_file->readUInt32();

    if(h->magic != 0x58444246)
    {
        throw "Invalid file. (MAGIC)";
    }

    Entry *temp_entries = new Entry[h->entry_count];
    for(unsigned int i = 0; i < h->entry_count; i++)
    {
        temp_entries[i].type = opened_file->readUInt16();
        temp_entries[i].identifier = opened_file->readUInt64();
        temp_entries[i].address = opened_file->readUInt32();
        temp_entries[i].length = opened_file->readUInt32();

        temp_entries[i].address = get_offset(temp_entries[i].address, h);
        private_entries.push_back(temp_entries[i]);
    }

    int freeMemoryOffset = (h->entry_table_length * 0x12) + 0x18;

    freeMemTable.entryCount = h->free_memory_table_entry_count - 1;
    freeMemTable.tableLength = h->free_memory_table_length;

    opened_file->setPosition(freeMemoryOffset);

    freeMemTable.entries = new vector<FreeMemoryEntry>(freeMemTable.entryCount);
    for(int i = 0; i < freeMemTable.entryCount; i++)
    {
        FreeMemoryEntry entry;
        entry.offsetSpecifier = opened_file->readUInt32();
        entry.length = opened_file->readUInt32();
        freeMemTable.entries->push_back(entry);
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

    opened_file->setPosition(entry->address);
    char *data = new char[entry->length];
    opened_file->read(data, entry->length);

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

    opened_file->close();
    opened_file = NULL;
}

int XDBF::get_offset(unsigned int offset_specifier, Header* h)
{
    int entry_table_size = (h->entry_table_length * 18);
    int free_space_size = (h->free_memory_table_length * 8);
    return ((entry_table_size + free_space_size) + 24) + offset_specifier;
}

int XDBF::getFakeOffset(unsigned int realAddress)
{
    int entry_table_size = (h->entry_table_length * 18);
    int free_space_size = (h->free_memory_table_length * 8);
    return (realAddress - ((entry_table_size + free_space_size) + 24));
}

FileIO *XDBF::get_file()
{
    return opened_file;
}

Setting_Entry* XDBF::get_setting_entry(Entry *entry)
{
    if (entry->type != ET_SETTING)
        return NULL;

    Setting_Entry *s_entry = new Setting_Entry;
    opened_file->setPosition(entry->address + 0x8);
    opened_file->read(&s_entry->type, 1);
    opened_file->setPosition(entry->address + 0x10);

    wchar_t *str_temp;
    switch (s_entry->type)
    {
        case SET_CONTEXT:
            //idk
            break;

        case SET_INT32:
            s_entry->i32_data = opened_file->readInt32();
            break;

        case SET_INT64:
            s_entry->i64_data = opened_file->readInt64();
            break;

        case SET_DOUBLE:
            s_entry->double_data = opened_file->readDouble();
            break;

        case SET_UNICODE:
            s_entry->unicode_string.str_len_in_bytes = (unsigned long)opened_file->readUInt32();
            opened_file->setPosition(entry->address + 0x18);
            s_entry->unicode_string.str = &opened_file->readUnicodeString();
            break;

        case SET_FLOAT:
            s_entry->float_data = opened_file->readFloat();
            break;

        case SET_BINARY:
            unsigned long bin_size;
            bin_size = opened_file->readUInt32();
            s_entry->binary.size = bin_size;
            s_entry->binary.data = new char[bin_size];
            opened_file->setPosition(entry->address + 0x18);
            opened_file->read(s_entry->binary.data, bin_size);
            break;

        case SET_DATETIME:
            //UINT64 fTime = opened_file->readUInt64();
            //s_entry->time_stamp = *(FILETIME*)&fTime;
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
    opened_file->setPosition(entry->address);
    opened_file->read(t_entry, 40);

    opened_file->setPosition(entry->address + 0x28);
    t_entry->gameName = &opened_file->readUnicodeString();

    return t_entry;
}

Achievement_Entry* XDBF::get_achievement_entry(Entry *entry)
{
    if (entry->type != ET_ACHIEVEMENT)
        return NULL;

    Achievement_Entry *chiev = new Achievement_Entry;

    opened_file->setPosition(entry->address);
    chiev->size = opened_file->readUInt32();
    chiev->id = opened_file->readUInt32();
    chiev->imageID = opened_file->readUInt32();
    chiev->gamerscore = opened_file->readUInt32();
    chiev->flags = opened_file->readUInt32();
    chiev->unlockedTime.dwHighDateTime = opened_file->readUInt32();
    chiev->unlockedTime.dwLowDateTime = opened_file->readUInt32();

    opened_file->setPosition(entry->address + 0x1C);
    wstring *str1 = new wstring(opened_file->readUnicodeString());
    chiev->name = str1;
    int str1Size = WSTRING_BYTES(str1->size());

    opened_file->setPosition(entry->address + 0x1C + str1Size);
    wstring *str2 = new wstring(opened_file->readUnicodeString());
    chiev->lockedDescription = str2;
    int str2Size = WSTRING_BYTES(str2->size());

    opened_file->setPosition(entry->address + 0x1C + str1Size + str2Size);
    wstring *str3 = new wstring(opened_file->readUnicodeString());
    chiev->unlockedDescription = str3;

    return chiev;
}

Sync_List XDBF::get_sync_list(int et_type, unsigned long long identifier)
{
    // make sure the entry exists
    Entry *syncListTarget = get_entry_by_id(identifier, et_type);
    if(syncListTarget == NULL)
        throw "Unable to locate entry.";

    int syncsInList = syncListTarget->length / 0x10;

    // initialize 'list'
    Sync_List list;
    list.sync_data = get_sync_data(et_type, identifier * 2);
    list.list_entry = syncListTarget;
    list.entry_count = syncsInList;
    list.entries = new vector<Sync_Entry>(syncsInList);

    // read all the entries in the sync list
    opened_file->setPosition(syncListTarget->address);
    for(int i = 0; i < syncsInList; i++)
    {
        list.entries->at(i).identifier = opened_file->readUInt64();
        list.entries->at(i).sync_id = opened_file->readUInt64();
    }

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
    // check if status is already requested status
    if(entry.sync_id != 0 && status == Enqueue)
        return;
    else if(entry.sync_id == 0 && status == Dequeue)
        return;

    // get the sync list
    Sync_List list = get_sync_list(et_type, identifier);

    bool found = false;
    vector<Sync_Entry> entries;

    if(status == Enqueue)
    {
        int highestSyncId = 0;
        int highestSyncIdIndex = 0;
        for(int i = 0; i < list.entry_count; i++)
        {
            if(list.entries->at(i).sync_id > highestSyncId)
            {
                // update the highest sync id, for sorting later
                highestSyncId = list.entries->at(i).sync_id;
                highestSyncIdIndex = i;
            }

            if(memcmp(&entry, &list.entries->at(i), sizeof(entry)) == 0)
            {
                // the inputted ID was found
                found = true;
            }
            else
            {
                // add the entry to the list (if it's not the requested one)
                entries.push_back(list.entries->at(i));
            }
        }

        if(!found)
            throw "Could not find given Sync List Entry.";

        // update the sync ID for the one we are writing
        entry.sync_id = highestSyncId + 1;

        // insert the entry at the end of the QUEUED list
        entries.insert(entries.begin() + (highestSyncIdIndex - 1), 1, entry);

        // write all the entries
        opened_file->setPosition(list.list_entry->address);
        for(int i = 0; i < list.entry_count; i++)
        {
            opened_file->writeUInt64(entries.at(i).identifier);
            opened_file->writeUInt64(entries.at(i).sync_id);
        }
    }
    else if(status == Dequeue)
    {
        for(int i = 0; i < list.entry_count; i++)
            if(memcmp(&entry, &list.entries->at(i), sizeof(entry)) == 0)
            {
                // make the requested entry's sync ID 0, so it dequeues it
                list.entries->at(i).sync_id = 0;
                found = true;
            }

        if(!found)
            throw "Could not find given Sync List Entry.";

        // write the new sync list
        write_sync_list(&list);
    }
}

void XDBF::write_sync_list(Sync_List *sl)
{
    // check size
    Entry *entr1 = sl->list_entry;
    if(sl->entry_count * 0x10 != entr1->length)
        throw "Requested sync list(to be written) differs in size from current sync list.";

    vector<Sync_Entry> queuedEntries, nonqueuedEntries;

    // sort entries by queued, and not queued
    for(int i = 0; i < sl->entry_count; i++)
    {
        if(sl->entries->at(i).sync_id != 0)
        {
            queuedEntries.push_back(sl->entries->at(i));

            // Sync_Entry *entr = &queuedEntries.at(queuedEntries.size() - 1);
            // SwapEndian(&entr->identifier);
            // SwapEndian(&entr->sync_id);
        }
        else
        {
            nonqueuedEntries.push_back(sl->entries->at(i));

            // Sync_Entry *entr = &nonqueuedEntries.at(nonqueuedEntries.size() - 1);
            // SwapEndian(&entr->identifier);
            // SwapEndian(&entr->sync_id);
        }
    }

    // write non-queued entries
    for(int i = 0; i < nonqueuedEntries.size(); i++)
    {
        opened_file->setPosition(entr1->address + (0x10 * i));
        opened_file->write(&nonqueuedEntries.at(i), 0x10);
    }

    // write queued entries
    int startingPos = nonqueuedEntries.size() * 0x10;
    for(int i = 0; i < queuedEntries.size(); i++)
    {
        opened_file->setPosition(entr1->address + (startingPos + (0x10 * i)));
        opened_file->write(&queuedEntries.at(i), 0x10);
    }

    // update corresponding sync data entry
    unsigned long long next = queuedEntries.size() + 1;
    sl->sync_data.next_sync_id = next;
    opened_file->setPosition(sl->sync_data.data_entry->address);
    opened_file->writeUInt64(next);
}

Sync_Data XDBF::get_sync_data(int et_type, unsigned long long identifier)
{
    // make sure there is a corresponding sync data
    Entry *target = get_entry_by_id(identifier, et_type);
    if(target == NULL)
        throw "Sync data could not be found with given entry type.";

    Sync_Data data;
    data.data_entry = target;

    // read the sync data's info
    opened_file->setPosition(target->address);

    data.next_sync_id = opened_file->readUInt64();
    data.last_sync_id = opened_file->readUInt64();
    data.last_synced_time.dwHighDateTime = opened_file->readUInt32();
    data.last_synced_time.dwLowDateTime = opened_file->readUInt32();

    return data;
}

Sync_Entry* XDBF::get_sync(int et_type, unsigned long long identifier)
{
    Sync_List syncs = get_sync_list(et_type, identifier);

    for (int i = 0; i < syncs.entry_count; i++)
        if (syncs.entries->at(i).identifier == identifier)
                return &(syncs.entries->at(i));

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

    opened_file->setPosition(entry->address);
    opened_file->write(&temp, 0x1C);

    update_sync_list_entry(*get_sync(ET_ACHIEVEMENT, entry->address), ET_ACHIEVEMENT, Enqueue);
}

Avatar_Award_Entry* XDBF::get_avatar_award_entry(Entry *entry)
{
    Avatar_Award_Entry *entryAA = new Avatar_Award_Entry;
    entryAA->entry = *entry;
    opened_file->setPosition(entry->address);

    entryAA->size = opened_file->readUInt32();
    entryAA->clothingType = opened_file->readUInt32();
    entryAA->flags64 = opened_file->readUInt64();
    entryAA->titleID = opened_file->readUInt32();
    entryAA->imageID = opened_file->readUInt32();
    entryAA->flags32 = opened_file->readUInt32();
    entryAA->unlockTime.dwHighDateTime = opened_file->readUInt32();
    entryAA->unlockTime.dwLowDateTime = opened_file->readUInt32();
    entryAA->unknown[0] = opened_file->readUInt32();
    entryAA->unknown[3] = opened_file->readUInt32();

    opened_file->setPosition(entry->address + 0x2C);
    wstring *str1 = new wstring(opened_file->readUnicodeString());
    entryAA->name = str1;
    int str1Size = WSTRING_BYTES(str1->size());

    opened_file->setPosition(entry->address + 0x2C + str1Size);
    wstring *str2 = new wstring(opened_file->readUnicodeString());
    entryAA->unlockedDescription = str2;
    int str2Size = WSTRING_BYTES(str2->size());

    opened_file->setPosition(entry->address + 0x2C + str1Size + str2Size);
    wstring *str3 = new wstring(opened_file->readUnicodeString());
    entryAA->lockedDescription = str3;

    return entryAA;
}

void XDBF::writeEntry(Avatar_Award_Entry *entry)
{
    opened_file->setPosition(entry->entry.address);
    opened_file->writeUInt32(entry->size);
    opened_file->writeUInt32(entry->clothingType);
    opened_file->writeUInt64(entry->flags64);
    opened_file->writeUInt32(entry->titleID);
    opened_file->writeUInt32(entry->imageID);
    opened_file->writeUInt32(entry->flags32);
    opened_file->writeUInt32(entry->unlockTime.dwHighDateTime);
    opened_file->writeUInt32(entry->unlockTime.dwLowDateTime);
}

void XDBF::injectEntry_private(unsigned int type, char *entryData, unsigned int dataLen, unsigned long long id)
{
    h->entry_count++;

    int indexWithClosestVal = -1;
    for(int i = 0; i < freeMemTable.entryCount; i++)
    {
        if(freeMemTable.entries->at(i).length == dataLen)
        {
            indexWithClosestVal = i;
            break;
        }
        if(freeMemTable.entries->at(i).length > dataLen)
        {
            if(indexWithClosestVal == -1)
                indexWithClosestVal = i;
            else
            {
                if(freeMemTable.entries->at(indexWithClosestVal).length > freeMemTable.entries->at(i).length)
                    indexWithClosestVal = i;
            }
        }
    }

    Entry newEntry = { type, id, 0, dataLen };

    // need to update sync list stuffs


    // update entry count
    opened_file->setPosition(0xC);
    opened_file->writeUInt32(h->entry_count);

    // append entry to file
    opened_file->setPosition(0, ios_base::cur);
    newEntry.address = opened_file->getPosition();
    opened_file->write(entryData, dataLen);

    // add the new entry to the table
    private_entries.push_back(newEntry);
    sort(private_entries.begin(), private_entries.end(), &compareFunction);

    writeEntryTable();
}

void XDBF::injectAchievementEntry(Achievement_Entry *entry, unsigned long long id)
{
    int nameLen = (entry->name->size() + 1) * 2;
    int lockedDescLen = (entry->lockedDescription->size() + 1) * 2;
    int unlockedDescLen = (entry->unlockedDescription->size() + 1) * 2;

    if (id == 0)
        entry->id = id = getNextId(ET_ACHIEVEMENT);
    if (entry->imageID == 0)
        entry->imageID = getNextId(ET_IMAGE);

    entry->size = 0x1C;

    char *data = new char[0x1C + nameLen + lockedDescLen + unlockedDescLen];
    wchar_t *nameCpy = new wchar_t[entry->name->size()];
    wchar_t *lockedDescCpy = new wchar_t[entry->lockedDescription->size()];
    wchar_t *unlockedDescCpy = new wchar_t[entry->unlockedDescription->size()];

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

    injectEntry_private(ET_ACHIEVEMENT, data, 0x1C + nameLen + lockedDescLen + unlockedDescLen, id);

    delete[] nameCpy;
    delete[] lockedDescCpy;
    delete[] unlockedDescCpy;
}

void XDBF::injectImageEntry(char *imageData, unsigned int len, unsigned long long id)
{
    injectEntry_private(ET_IMAGE, imageData, len, (id == 0) ? getNextId(ET_IMAGE) : id);
}

unsigned long long XDBF::getNextId(unsigned short type)
{
    unsigned long long maxId = private_entries[0].identifier;
    for (int i = 1; i < private_entries.size(); i++)
    {
        if ((maxId < private_entries[i].identifier) && (private_entries[i].type == type) && (private_entries[i].identifier != SYNC_LIST && private_entries[i].identifier != SYNC_DATA && private_entries[i].identifier != TITLE_INFORMATION))
            maxId = private_entries[i].identifier;
    }
    return maxId + 1;
}

void XDBF::writeEntryTable()
{
    // re-write entry table
    for (int i = 0; i < h->entry_count; i++)
    {
        opened_file->setPosition((i * 0x12) + 0x18);
        opened_file->writeUInt16(private_entries[i].type);
        opened_file->writeUInt64(private_entries[i].identifier);
        opened_file->writeUInt32(private_entries[i].length);
        opened_file->writeUInt32(getFakeOffset(private_entries[i].address));
    }
}

void XDBF::removeEntry(Entry *entry)
{
    // update entry count
    h->entry_count--;
    opened_file->setPosition(0xC);
    opened_file->writeUInt32(h->entry_count);

    // remove entry from table
    for (int i = 0; i < private_entries.size(); i++)
        if (private_entries.at(i).identifier == entry->identifier && private_entries.at(i).type == entry->type)
            private_entries.erase(private_entries.begin() + i);

    // re-write the entry table
    writeEntryTable();

    // mark entry as unused memory
    FreeMemoryEntry freeMem = { getFakeOffset(entry->address), entry->length };
    freeMemTable.entries->insert(freeMemTable.entries->begin() + (freeMemTable.entries->size() - 2), freeMem);

    // re-write the free memory table
    writeFreeMemoryTable();
}

void XDBF::writeFreeMemoryTable()
{
    int freeMemoryOffset = (h->entry_table_length * 0x12) + 0x18;
    opened_file->setPosition(freeMemoryOffset);
    for (int i = 0; i < freeMemTable.entryCount; i++)
    {
        opened_file->writeUInt32(freeMemTable.entries->at(i).offsetSpecifier);
        opened_file->writeUInt32(freeMemTable.entries->at(i).length);
    }
}

//for sorting entries
bool compareFunction(Entry e1, Entry e2)
{
    if (e1.type != e2.type)
        return (e1.type < e2.type);
    else
        return (e1.identifier < e2.identifier);
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
