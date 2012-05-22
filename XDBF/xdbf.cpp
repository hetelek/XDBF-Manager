#include "xdbf.h";
#include <algorithm>

XDBF::XDBF(string path) : filePath(path)
{
    openedFile = new FileIO(path);

    if(openedFile == NULL)
    {
        //File could not be opened
        throw "File could not be opened.";
    }

    openedFile->setPosition(0);
    h->magic = openedFile->readUInt32();
    h->version = openedFile->readUInt32();
    h->entryTableLength = openedFile->readUInt32();
    h->entryCount = openedFile->readUInt32();
    h->freeMemoryTableLength = openedFile->readUInt32();
    h->freeMemoryTableEntryCount = openedFile->readUInt32();

    if(h->magic != 0x58444246)
    {
        throw "Invalid file. (MAGIC)";
    }

    privateEntries = new vector<Entry>();
    for(unsigned int i = 0; i < h->entryCount; i++)
    {
        Entry temp;
        temp.type = openedFile->readUInt16();
        temp.identifier = openedFile->readUInt64();
        temp.address = openedFile->readUInt32();
        temp.length = openedFile->readUInt32();

        temp.address = get_offset(temp.address);
        privateEntries->push_back(temp);
    }

    freeMemTable.entryCount = h->freeMemoryTableEntryCount;
    freeMemTable.tableLength = h->freeMemoryTableLength;

    int freeMemoryOffset = (h->entryTableLength * 0x12) + 0x18;
    openedFile->setPosition(freeMemoryOffset);

    freeMemTable.entries = new vector<FreeMemoryEntry>();
    for(int i = 0; i < freeMemTable.entryCount; i++)
    {
        FreeMemoryEntry entry;
        entry.offsetSpecifier = openedFile->readUInt32();
        entry.length = openedFile->readUInt32();
        freeMemTable.entries->push_back(entry);
    }

    int a = 0;
}

XDBF::~XDBF()
{
    delete openedFile;
}

Entry* XDBF::getEntryById(long long identifier, int type)
{
    Entry *entries = getEntries();
    for(unsigned int i = 0; i < getHeader()->entryCount; i++)
        if(entries[i].identifier == identifier && entries[i].type == type)
        {
            return &entries[i];
        }

    return NULL;
}

char* XDBF::extractEntry(Entry *entry)
{
    if(openedFile == NULL)
        throw "File has been closed/never opened.";

    openedFile->setPosition(entry->address);
    char *data = new char[entry->length];
    openedFile->read(data, entry->length);

    return data;
}

Entry* XDBF::getEntries()
{
    if (privateEntries->size() == 0)
        return NULL;
    return &privateEntries->at(0);
}

Header* XDBF::getHeader()
{
    return h;
}

void XDBF::close()
{
    if(openedFile == NULL)
        return;

    openedFile->close();
    openedFile = NULL;
}

int XDBF::get_offset(unsigned int offset_specifier)
{
    int entry_table_size = (h->entryTableLength * 18);
    int free_space_size = (h->freeMemoryTableLength * 8);
    return ((entry_table_size + free_space_size) + 24) + offset_specifier;
}

int XDBF::getFakeOffset(unsigned int realAddress)
{
    int entry_table_size = (h->entryTableLength * 18);
    int free_space_size = (h->freeMemoryTableLength * 8);
    return (realAddress - ((entry_table_size + free_space_size) + 24));
}

FileIO *XDBF::getFile()
{
    return openedFile;
}

Setting_Entry* XDBF::getSettingEntry(Entry *entry)
{
    if (entry->type != ET_SETTING)
        return NULL;

    Setting_Entry *s_entry = new Setting_Entry;
    s_entry->entry = entry;
    openedFile->setPosition(entry->address + 0x8);
    openedFile->read(&s_entry->type, 1);
    openedFile->setPosition(entry->address + 0x10);

    wstring *str;
    switch (s_entry->type)
    {
        case SET_CONTEXT:
            //idk
            break;

        case SET_INT32:
            s_entry->i32_data = openedFile->readInt32();
            break;

        case SET_INT64:
            s_entry->i64_data = openedFile->readInt64();
            break;

        case SET_DOUBLE:
            s_entry->double_data = openedFile->readDouble();
            break;

        case SET_UNICODE:
            s_entry->unicode_string.str_len_in_bytes = (unsigned long)openedFile->readUInt32();
            openedFile->setPosition(entry->address + 0x18);
            str = new wstring(openedFile->readUnicodeString());
            s_entry->unicode_string.str = str;
            break;

        case SET_FLOAT:
            s_entry->float_data = openedFile->readFloat();
            break;

        case SET_BINARY:
            unsigned long bin_size;
            bin_size = openedFile->readUInt32();
            s_entry->binary.size = bin_size;
            s_entry->binary.data = new char[bin_size];
            openedFile->setPosition(entry->address + 0x14);
            openedFile->read(s_entry->binary.data, bin_size);
            break;

        case SET_DATETIME:
            s_entry->time_stamp.dwHighDateTime = openedFile->readUInt32();
            s_entry->time_stamp.dwLowDateTime = openedFile->readUInt32();
            break;

        case SET_NULL:
            break;

        default:
            throw "Invalid setting entry type.";
    }
    return s_entry;
}

std::wstring XDBF::getStringEntry(Entry *e)
{
    if(e->type != ET_STRING)
        throw "Given entry is not a string entry.";

    openedFile->setPosition(e->address);
    return openedFile->readUnicodeString();
}

std::string XDBF::getSettingEntryName(Setting_Entry *s_entry)
{
    if(s_entry->type == 0xFF)
        return "NULL";
    else if(s_entry->type > 7)
        throw "Invalid setting entry type.";

    std::string friendly_setting_types[8] = { "Sync Entry", "Int32", "Int64", "Double", "Unicode String", "Float", "Binary", "Datetime"};
    return friendly_setting_types[s_entry->type];
}

Title_Entry* XDBF::getTitleEntry(Entry *entry)
{
    if (entry->type != ET_TITLE || entry->identifier == 0x200000000 || entry->identifier == 0x100000000)
        return NULL;

    Title_Entry *t_entry = new Title_Entry;
    t_entry->entry = entry;
    openedFile->setPosition(entry->address);
    t_entry->titleID = openedFile->readUInt32();
    t_entry->achievementCount = openedFile->readInt32();
    t_entry->achievementUnlockedCount = openedFile->readInt32();
    t_entry->gamerscoreUnlocked = openedFile->readUInt32();
    t_entry->totalGamerscore = openedFile->readUInt32();
    openedFile->read(&t_entry->unknown, 8);
    t_entry->flags = openedFile->readUInt32();
    t_entry->lastPlayed.dwHighDateTime = openedFile->readUInt32();
    t_entry->lastPlayed.dwLowDateTime = openedFile->readUInt32();

    openedFile->setPosition(entry->address + 0x28);
    wstring *a = new wstring(openedFile->readUnicodeString());
    t_entry->gameName = a;

    return t_entry;
}

Achievement_Entry* XDBF::getAchievementEntry(Entry *entry)
{
    if (entry->type != ET_ACHIEVEMENT)
        return NULL;

    Achievement_Entry *chiev = new Achievement_Entry;

    openedFile->setPosition(entry->address);
    chiev->size = openedFile->readUInt32();
    chiev->id = openedFile->readUInt32();
    chiev->imageID = openedFile->readUInt32();
    chiev->gamerscore = openedFile->readUInt32();
    chiev->flags = openedFile->readUInt32();
    chiev->unlockedTime.dwHighDateTime = openedFile->readUInt32();
    chiev->unlockedTime.dwLowDateTime = openedFile->readUInt32();

    openedFile->setPosition(entry->address + 0x1C);
    wstring *str1 = new wstring(openedFile->readUnicodeString());
    chiev->name = str1;
    int str1Size = WSTRING_BYTES(str1->size());

    openedFile->setPosition(entry->address + 0x1C + str1Size);
    wstring *str2 = new wstring(openedFile->readUnicodeString());
    chiev->unlockedDescription = str2;
    int str2Size = WSTRING_BYTES(str2->size());

    openedFile->setPosition(entry->address + 0x1C + str1Size + str2Size);
    wstring *str3 = new wstring(openedFile->readUnicodeString());
    chiev->lockedDescription = str3;

    return chiev;
}

Sync_List XDBF::getSyncList(int et_type, unsigned long long identifier)
{
    Sync_List list = { 0 };
    // make sure the entry exists
    Entry *syncListTarget = getEntryById(identifier, et_type);
    if(syncListTarget == NULL)
        return list;

    int syncsInList = syncListTarget->length / 0x10;

    // initialize 'list'
    list.syncData = getSyncData(et_type, identifier * 2);
    list.listEntry = syncListTarget;
    list.entryCount = syncsInList;
    list.entries = new vector<Sync_Entry>();
    list.entries = new vector<Sync_Entry>(syncsInList);

    // read all the entries in the sync list
    openedFile->setPosition(syncListTarget->address);
    for(int i = 0; i < syncsInList; i++)
    {
        list.entries->at(i).identifier = openedFile->readUInt64();
        list.entries->at(i).syncId = openedFile->readUInt64();
    }

    return list;
}

std::string XDBF::FILETIME_to_string(FILETIME *pft)
{
    struct tm *timeinfo = FILETIMEToTm(pft);
    if (timeinfo == NULL)
        return "<i>N/A</i>";

    return string(asctime(timeinfo));
}

void XDBF::updateSyncListEntry(Sync_Entry entry, int et_type, SyncEntryStatus status, unsigned long long identifier)
{
    // check if status is already requested status
    if(entry.syncId != 0 && status == Enqueue)
        return;
    else if(entry.syncId == 0 && status == Dequeue)
        return;

    // get the sync list
    Sync_List list = getSyncList(et_type, identifier);

    bool found = false;
    vector<Sync_Entry> entries;

    if(status == Enqueue)
    {
        int highestSyncId = 0;
        int highestSyncIdIndex = 0;
        for(int i = 0; i < list.entryCount; i++)
        {
            if(list.entries->at(i).syncId > highestSyncId)
            {
                // update the highest sync id, for sorting later
                highestSyncId = list.entries->at(i).syncId;
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
        entry.syncId = highestSyncId + 1;

        // insert the entry at the end of the QUEUED list
        entries.insert(entries.begin() + (highestSyncIdIndex - 1), 1, entry);

        // write all the entries
        openedFile->setPosition(list.listEntry->address);
        for(int i = 0; i < list.entryCount; i++)
        {
            openedFile->write(entries.at(i).identifier);
            openedFile->write(entries.at(i).syncId);
        }
    }
    else if(status == Dequeue)
    {
        for(int i = 0; i < list.entryCount; i++)
            if(memcmp(&entry, &list.entries->at(i), sizeof(entry)) == 0)
            {
                // make the requested entry's sync ID 0, so it dequeues it
                list.entries->at(i).syncId = 0;
                found = true;
            }

        if(!found)
            throw "Could not find given Sync List Entry.";

        // write the new sync list
        writeSyncList(&list);
    }
}

void XDBF::writeSyncList(Sync_List *sl)
{
    // check size
    Entry *entr1 = sl->listEntry;

    int newSize = (sl->entryCount * 0x10);
    int diff = 0;
    if(newSize != entr1->length)
        diff = entr1->length - newSize;

    if(diff < 0)
    {
        ffree(entr1->address, entr1->length);
        sl->listEntry->address = fmalloc(newSize);
    }

    vector<Sync_Entry> queuedEntries, nonqueuedEntries;

    // sort entries by queued, and not queued
    for(int i = 0; i < sl->entryCount; i++)
    {
        if(sl->entries->at(i).syncId != 0)
            queuedEntries.push_back(sl->entries->at(i));
        else
            nonqueuedEntries.push_back(sl->entries->at(i));
    }

    // write non-queued entries
    for(int i = 0; i < nonqueuedEntries.size(); i++)
    {
        openedFile->setPosition(entr1->address + (0x10 * i));
        openedFile->write(nonqueuedEntries.at(i).identifier);
        openedFile->write(nonqueuedEntries.at(i).syncId);
    }

    // write queued entries
    int startingPos = nonqueuedEntries.size() * 0x10;
    for(int i = 0; i < queuedEntries.size(); i++)
    {
        openedFile->setPosition(entr1->address + (startingPos + (0x10 * i)));
        openedFile->write(queuedEntries.at(i).identifier);
        openedFile->write(queuedEntries.at(i).syncId);
    }

    // update corresponding sync data entry
    unsigned long long next = queuedEntries.size() + 1;
    sl->syncData.nextSyncId = next;
    openedFile->setPosition(sl->syncData.dataEntry->address);
    openedFile->write(next);

    if(diff > 0)
        ffree(sl->listEntry->address, newSize);

    // update the entry size
    sl->listEntry->length = newSize;
    writeEntryTable(openedFile);
}

Sync_Data XDBF::getSyncData(int et_type, unsigned long long identifier)
{
    // make sure there is a corresponding sync data
    Entry *target = getEntryById(identifier, et_type);
    if(target == NULL)
        throw "Sync data could not be found with given entry type.";

    Sync_Data data;
    data.dataEntry = target;

    // read the sync data's info
    openedFile->setPosition(target->address);

    data.nextSyncId = openedFile->readUInt64();
    data.lastSyncId = openedFile->readUInt64();
    data.lastSyncedTime.dwHighDateTime = openedFile->readUInt32();
    data.lastSyncedTime.dwLowDateTime = openedFile->readUInt32();

    return data;
}

Sync_Entry* XDBF::getSync(int et_type, unsigned long long identifier)
{
    Sync_List syncs = getSyncList(et_type, identifier);

    for (int i = 0; i < syncs.entryCount; i++)
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

    openedFile->setPosition(entry->address);
    openedFile->write(&temp, 0x1C);

    updateSyncListEntry(*getSync(ET_ACHIEVEMENT, entry->address), ET_ACHIEVEMENT, Enqueue);
}

Avatar_Award_Entry* XDBF::getAvatarAwardEntry(Entry *entry)
{
    Avatar_Award_Entry *entryAA = new Avatar_Award_Entry;
    entryAA->entry = *entry;
    openedFile->setPosition(entry->address);

    entryAA->size = openedFile->readUInt32();
    entryAA->clothingType = openedFile->readUInt32();
    entryAA->flags64 = openedFile->readUInt64();
    entryAA->titleID = openedFile->readUInt32();
    entryAA->imageID = openedFile->readUInt32();
    entryAA->flags32 = openedFile->readUInt32();
    entryAA->unlockTime.dwHighDateTime = openedFile->readUInt32();
    entryAA->unlockTime.dwLowDateTime = openedFile->readUInt32();
    entryAA->unknown[0] = openedFile->readUInt32();
    entryAA->unknown[3] = openedFile->readUInt32();

    openedFile->setPosition(entry->address + 0x2C);
    wstring *str1 = new wstring(openedFile->readUnicodeString());
    entryAA->name = str1;
    int str1Size = WSTRING_BYTES(str1->size());

    openedFile->setPosition(entry->address + 0x2C + str1Size);
    wstring *str2 = new wstring(openedFile->readUnicodeString());
    entryAA->unlockedDescription = str2;
    int str2Size = WSTRING_BYTES(str2->size());

    openedFile->setPosition(entry->address + 0x2C + str1Size + str2Size);
    wstring *str3 = new wstring(openedFile->readUnicodeString());
    entryAA->lockedDescription = str3;

    return entryAA;
}

void XDBF::writeEntry(Avatar_Award_Entry *entry)
{
    openedFile->setPosition(entry->entry.address);
    openedFile->write(entry->size);
    openedFile->write(entry->clothingType);
    openedFile->write(entry->flags64);
    openedFile->write(entry->titleID);
    openedFile->write(entry->imageID);
    openedFile->write(entry->flags32);
    openedFile->write((unsigned int)entry->unlockTime.dwHighDateTime);
    openedFile->write((unsigned int)entry->unlockTime.dwLowDateTime);
}

Entry* XDBF::injectEntry_private(unsigned int type, char *entryData, unsigned int dataLen, unsigned long long id)
{
    Entry newEntry = { type, id, 0, dataLen };

    // update sync list stuffs
    // string/image entries don't have sync lists

    if (newEntry.type != ET_STRING && newEntry.type != ET_IMAGE && newEntry.type <= 6 && !(id == SYNC_LIST || id == SYNC_DATA))
    {
        Sync_Entry entry;
        entry.identifier = id;
        entry.syncId = 1;
        Sync_List sl = getSyncList(type, (type == ET_AVATAR_AWARD) ? 1 : SYNC_LIST);
        sl.entries->push_back(entry);
        sl.entryCount++;
        writeSyncList(&sl);
    }

    // update entry count
    h->entryCount++;
    openedFile->setPosition(0xC);
    openedFile->write(h->entryCount);

    // append entry to file
    long addr = fmalloc(dataLen);
    newEntry.address = addr;
    openedFile->setPosition(addr);
    openedFile->write(entryData, dataLen);

    // add the new entry to the table
    privateEntries->push_back(newEntry);
    sort(privateEntries->begin(), privateEntries->end(), &compareFunction);

    writeEntryTable(openedFile);

    return getEntryById(id, type);
}

void XDBF::injectAchievementEntry(Achievement_Entry *entry, unsigned long long id)
{
    int nameLen = WSTRING_BYTES(entry->name->size());
    int lockedDescLen = WSTRING_BYTES(entry->lockedDescription->size());
    int unlockedDescLen = WSTRING_BYTES(entry->unlockedDescription->size());

    if (id == 0)
        entry->id = id = getNextId(ET_ACHIEVEMENT);
    if (entry->imageID == 0)
        entry->imageID = getNextId(ET_IMAGE);

    entry->size = 0x1C;

    char *data = new char[0x1C + nameLen + lockedDescLen + unlockedDescLen];
    wchar_t *nameCpy = new wchar_t[entry->name->size() + 1];
    wchar_t *lockedDescCpy = new wchar_t[entry->lockedDescription->size() + 1];
    wchar_t *unlockedDescCpy = new wchar_t[entry->unlockedDescription->size() + 1];

    memcpy(nameCpy, entry->name->c_str(), nameLen);
    memcpy(lockedDescCpy, entry->lockedDescription->c_str(), lockedDescLen);
    memcpy(unlockedDescCpy, entry->unlockedDescription->c_str(), unlockedDescLen);

    swapAchievementEndianness(entry);
    memcpy(data, &entry->size, 0x1C);
    swapAchievementEndianness(entry);

    SwapEndianUnicode(nameCpy, nameLen);
    SwapEndianUnicode(lockedDescCpy, lockedDescLen);
    SwapEndianUnicode(unlockedDescCpy, unlockedDescLen);

    memcpy(&data[0x1C], nameCpy, nameLen);
    memcpy(&data[0x1C + nameLen], unlockedDescCpy, unlockedDescLen);
    memcpy(&data[0x1C + nameLen + unlockedDescLen], lockedDescCpy, lockedDescLen);

    injectEntry_private(ET_ACHIEVEMENT, data, 0x1C + nameLen + lockedDescLen + unlockedDescLen, id);
}

void XDBF::injectImageEntry(char *imageData, unsigned int len, unsigned long long id)
{
    injectEntry_private(ET_IMAGE, imageData, len, (id == 0) ? getNextId(ET_IMAGE) : id);
}

unsigned long long XDBF::getNextId(unsigned short type)
{
    unsigned long long maxId = 0;
    for (int i = 0; i < privateEntries->size(); i++)
    {
        if ((maxId < privateEntries->at(i).identifier) && privateEntries->at(i).type == type && !(privateEntries->at(i).identifier == SYNC_LIST || privateEntries->at(i).identifier == SYNC_DATA || privateEntries->at(i).identifier == TITLE_INFORMATION))
            maxId = privateEntries->at(i).identifier;
    }
    return maxId + 1;
}

unsigned int XDBF::fmalloc(size_t dataLen)
{
    int indexWithClosestVal = -1;

    // if the entry count is 0, then we need to skip the free mem table check
    if (freeMemTable.entryCount == 0)
        goto skipFreeMemCheck;

    // get the index of the entry whose length is closest in size
    // to the requested amount of memory
    for(unsigned int i = 0; i < freeMemTable.entryCount - 1; i++)
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

    skipFreeMemCheck:

    // if memory was found in the free entry table
    if (indexWithClosestVal != -1)
    {
        // store the len and addr for later user, because we're gonna delete the entry
        // from the list
        int addr = get_offset(freeMemTable.entries->at(indexWithClosestVal).offsetSpecifier);
        int len = freeMemTable.entries->at(indexWithClosestVal).length;

        // erase the entry from the table that we're using
        freeMemTable.entries->erase(freeMemTable.entries->begin() + indexWithClosestVal);

        // if the free memory isn't the perfect size, then give some back
        if (len != dataLen)
        {
            FreeMemoryEntry entry = { getFakeOffset(addr + dataLen),  (len - dataLen) };
            freeMemTable.entries->insert(freeMemTable.entries->begin() + (freeMemTable.entries->size() - 2), entry);
        }

        // update the free mem table
        h->freeMemoryTableEntryCount = freeMemTable.entryCount = freeMemTable.entries->size();

        // write the new entry count
        openedFile->setPosition(0x14);
        openedFile->write((unsigned int)freeMemTable.entryCount);

        // re-write the entry table
        writeFreeMemoryTable();

        return addr;
    }
    // if there wasn't enough consecutive free memory available, we'll append it to the file
    else
    {
        // seek to the end of the file
        openedFile->setPosition(0, ios_base::end);

        // store the address of the current file end, becasue when we append
        // the reqested amount of memory, this current position will be the location
        // of the allocated memory
        long freeMemAddr = openedFile->getPosition();

        // append the memory to the file
        char *temp = new char[dataLen];
        openedFile->write(temp, dataLen);

        delete[] temp;

        return freeMemAddr;
    }
}

void XDBF::ffree(unsigned int address, size_t size)
{
    if (size <= 0)
        return;

    FreeMemoryEntry entry = { getFakeOffset(address),  size };
    vector<FreeMemoryEntry>::iterator toInsert = freeMemTable.entries->end() - 1;
    freeMemTable.entries->insert(toInsert, entry);

    // update entry count
    freeMemTable.entryCount++;
    h->freeMemoryTableEntryCount++;

    // write the new entry count
    openedFile->setPosition(0x14);
    openedFile->write(freeMemTable.entryCount);

    writeFreeMemoryTable();
}

void XDBF::writeEntryTable(FileIO *io)
{
    // re-write entry table
    for (int i = 0; i < h->entryCount; i++)
    {
        io->setPosition((i * 0x12) + 0x18);
        io->write(privateEntries->at(i).type);
        io->write(privateEntries->at(i).identifier);
        unsigned int temp = getFakeOffset(privateEntries->at(i).address);
        io->write(temp);
        io->write(privateEntries->at(i).length);
    }
}

void XDBF::removeEntry(Entry *entry)
{
    if (entry == NULL)
        return;

    if (entry->identifier == SYNC_LIST || entry->identifier == SYNC_DATA || (entry->identifier == 1 && entry->type == ET_AVATAR_AWARD) || (entry->identifier == 2 && entry->type == ET_AVATAR_AWARD))
        throw "You can't remove sync data or sync lists.";

    // remove sync entry
    Sync_List list = getSyncList(entry->type, (entry->type == ET_AVATAR_AWARD) ? 1 : SYNC_LIST);
    removeSyncEntry(entry->identifier, &list);

    Entry temp = *entry;

    // update entry count
    h->entryCount--;
    openedFile->setPosition(0xC);
    openedFile->write(h->entryCount);

    // remove entry from table
    for (int i = 0; i < privateEntries->size(); i++)
        if (privateEntries->at(i).identifier == temp.identifier && privateEntries->at(i).type == temp.type)
        {
            privateEntries->erase(privateEntries->begin() + i);
            break;
        }

    // re-write the entry table
    writeEntryTable(openedFile);

    // update free memory table length
    h->freeMemoryTableEntryCount++;
    openedFile->setPosition(0x14);
    openedFile->write(h->freeMemoryTableEntryCount);

    // free the memory
    ffree(entry->address, entry->length);

    // re-write the free memory table
    writeFreeMemoryTable();
}

void XDBF::writeFreeMemoryTable()
{
    // update the last free mem table entry
    openedFile->setPosition(0, ios_base::end);
    unsigned int pos = getFakeOffset(openedFile->getPosition());
    freeMemTable.entries->at(freeMemTable.entries->size() - 1).offsetSpecifier = pos;
    freeMemTable.entries->at(freeMemTable.entries->size() - 1).length = 0xFFFFFFFF - pos;

    int freeMemoryOffset = (h->entryTableLength * 0x12) + 0x18;
    openedFile->setPosition(freeMemoryOffset);
    for (int i = 0; i < freeMemTable.entryCount; i++)
    {
        openedFile->write(freeMemTable.entries->at(i).offsetSpecifier);
        openedFile->write(freeMemTable.entries->at(i).length);
    }

    // null out the rest of the table
    for (int i = 0; i < h->freeMemoryTableLength - freeMemTable.entryCount; i++)
        openedFile->write((unsigned long long)0);
}

void XDBF::injectTitleEntry(Title_Entry *entry, unsigned long long id)
{
    // allocate memory to store the data to write
    char *data = new char[0x28 + WSTRING_BYTES(entry->gameName->length())];

    // swap the endianess of the data to prepare for write
    swapTitleEndianness(entry);

    // copy the title entry informaion to the data to write
    memcpy(data, &entry->titleID, 0x28);

    // swap the title endianess back so it's not screwed up for later use
    swapTitleEndianness(entry);

    // allocate memory for a temporary name, need to make a temporary name so we can
    // change the endianess of the string
    wchar_t *tempName = new wchar_t[entry->gameName->length() + 1];

    // copy the string to the temp string
    memcpy(tempName, entry->gameName->c_str(), WSTRING_BYTES(entry->gameName->length()));

    // change the endianess of the string
    SwapEndianUnicode(tempName, WSTRING_BYTES(entry->gameName->length()));

    // copy the string into the data to write
    memcpy(data + 0x28, tempName, WSTRING_BYTES(entry->gameName->length()));

    // inject the entry
    entry->entry = injectEntry_private(ET_TITLE, data, 0x28 + WSTRING_BYTES(entry->gameName->length()), id);

    // give the allocated memory back to the heap
    delete[] data;
    delete[] tempName;
}

void XDBF::removeSyncEntry(unsigned long long identifier, Sync_List *list)
{
    bool found = false;
    for(int i = 0; i < list->entryCount; i++)
        if (list->entries->at(i).identifier == identifier)
        {
            list->entries->erase(list->entries->begin() + i);
            list->entryCount--;
            found = true;
            break;
        }

    if(!found)
        return;

    writeSyncList(list);
}

void XDBF::swapTitleEndianness(Title_Entry *entry)
{
    SwapEndian(&entry->titleID);
    SwapEndian((unsigned int*)&entry->achievementCount);
    SwapEndian((unsigned int*)&entry->achievementUnlockedCount);
    SwapEndian(&entry->totalGamerscore);
    SwapEndian(&entry->gamerscoreUnlocked);
    SwapEndian(&entry->flags);
    SwapEndian(&entry->lastPlayed.dwHighDateTime);
    SwapEndian(&entry->lastPlayed.dwLowDateTime);
}

void XDBF::writeEntry(Title_Entry *entry)
{
    // seek to the entry's position
    openedFile->setPosition(entry->entry->address);

    // write the data
    openedFile->write(entry->titleID);
    openedFile->write(entry->achievementCount);
    openedFile->write(entry->achievementUnlockedCount);
    openedFile->write(entry->totalGamerscore);
    openedFile->write(entry->gamerscoreUnlocked);
    openedFile->write(*(unsigned long long*)&entry->unknown);
    openedFile->write(entry->flags);
    openedFile->write((unsigned int)entry->lastPlayed.dwHighDateTime);
    openedFile->write((unsigned int)entry->lastPlayed.dwLowDateTime);
}

void XDBF::injectSettingEntry(Setting_Entry *entry, unsigned long long id)
{
    char *data;
    if (id == 0)
        id = getNextId(ET_SETTING);
    switch (entry->type)
    {
        case SET_INT32:
            injectSettingEntry_private(&entry->i32_data, 4, entry, id);
            break;
        case SET_INT64:
            injectSettingEntry_private(&entry->i64_data, 8, entry, id);
            break;
        case SET_DOUBLE:
            injectSettingEntry_private(&entry->double_data, 8, entry, id);
            break;
        case SET_FLOAT:
            injectSettingEntry_private(&entry->float_data, 4, entry, id);
            break;
        case SET_UNICODE:
        {
            // allocate the perfect amount of memory for the entry
            data = new char[entry->unicode_string.str_len_in_bytes + 24];
            // zero the memory
            memset(data, 0, entry->unicode_string.str_len_in_bytes + 24);
            // write the entry meta data
            writeSettingMetaData(data, id, SET_UNICODE);

            // write the string length (in bytes)
            SwapEndian(&entry->unicode_string.str_len_in_bytes);
            memcpy(data + 0x10, &entry->unicode_string.str_len_in_bytes, 4);
            SwapEndian(&entry->unicode_string.str_len_in_bytes);

            // create a new temporary string so that we can change it to big endian
            wchar_t *tempString = new wchar_t[entry->unicode_string.str_len_in_bytes / sizeof(wchar_t)];
            memcpy(tempString, entry->unicode_string.str->c_str(), entry->unicode_string.str_len_in_bytes);
            SwapEndianUnicode(tempString, entry->unicode_string.str_len_in_bytes);

            // copy the correct string into the write buffer
            memcpy(data + 0x18, tempString, entry->unicode_string.str_len_in_bytes);

            // write the entry
            injectEntry_private(ET_SETTING, data, entry->unicode_string.str_len_in_bytes + 24, id);

            // give that string back to memory
            delete[] tempString;
            break;
        }
        case SET_DATETIME:
            // allocate the correct amount of memory for the entry
            data = new char[0x18];
            // zero the memory
            memset(data, 0, 0x18);
            // write the entry meta data
            writeSettingMetaData(data, id, SET_DATETIME);

            // swap the endian for writing
            SwapEndian(&entry->time_stamp.dwHighDateTime);
            SwapEndian(&entry->time_stamp.dwLowDateTime);

            // copy the data time to the write buffer
            memcpy(data + 0x10, &entry->time_stamp, 8);

            // swap the endian back so the value isn't screwed up
            SwapEndian(&entry->time_stamp.dwHighDateTime);
            SwapEndian(&entry->time_stamp.dwLowDateTime);

            // inject the entry
            injectEntry_private(ET_SETTING, data, 0x18, id);
            break;
        case SET_BINARY:
            if(entry->binary.size > 0x3E8)
                throw "Binary data too large to inject. Max = 1000(0x3E8)";

            int size = entry->binary.size + 0x14;
            char *buffer = new char[size];
            memset(buffer, 0, size);
            writeSettingMetaData(buffer, id, SET_BINARY);
            SwapEndian(&entry->binary.size);
            *(int*)&buffer[0x10] = entry->binary.size;
            SwapEndian(&entry->binary.size);
            memcpy(&buffer[0x14], entry->binary.data, entry->binary.size);
            injectEntry_private(ET_SETTING, buffer, size, id);
            break;
    }
}

void XDBF::writeSettingMetaData(char *buffer, unsigned long long id, unsigned char type)
{
    // copy the entry id to the data buffer
    SwapEndian(&id);
    memcpy(buffer, &id, 8);

    // copy the type to the setting data
    memcpy(&buffer[8], &type, 1);
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

void XDBF::injectSettingEntry_private(void* value, int len, Setting_Entry *entry, unsigned long long id)
{
    // allocate enough memory for an integral/floating point setting entry
    char *data = new char[0x18];
    // zero out the write buffer
    memset(data, 0, 0x18);

    // write the setting meta data, so the id and type
    writeSettingMetaData(data, id, entry->type);

    // copy the data to the write buffer
    SwapEndian(value, 1, len);
    memcpy(data + 0x10, value, len);
    SwapEndian(value, 1, len);

    // inject the new entry
    entry->entry = injectEntry_private(ET_SETTING, data, 0x18, id);
}

void XDBF::writeEntry(Setting_Entry* entry)
{
    // still need to write the cases for SET_BINARY
    switch (entry->type)
    {
        case SET_INT32:
        case SET_FLOAT:
            writeSettingEntryPrivate(&entry->i32_data, 4, entry->entry);
            break;
        case SET_INT64:
        case SET_DOUBLE:
            writeSettingEntryPrivate(&entry->i64_data, 8, entry->entry);
            break;
        case SET_DATETIME:
            // seek to the entry data position
            openedFile->setPosition(entry->entry->address + 0x10);

            // write the date time into the entry
            openedFile->write((unsigned int)entry->time_stamp.dwHighDateTime);
            openedFile->write((unsigned int)entry->time_stamp.dwLowDateTime);
        case SET_UNICODE:
            // if the entry size is less than the previous size then we're good to go
            if (entry->unicode_string.str_len_in_bytes <= (entry->entry->length - 0x18))
            {
                // seek to the position of the string length
                openedFile->setPosition(entry->entry->address + 0x10);
                // write the new string length
                openedFile->write((unsigned int)entry->unicode_string.str_len_in_bytes);
                // write the string
                openedFile->setPosition(entry->entry->address + 0x18);
                openedFile->write(*entry->unicode_string.str);
                // free the excess memory
                ffree(entry->entry->address + 0x18 + entry->unicode_string.str_len_in_bytes, entry->entry->length - (0x18 + entry->unicode_string.str_len_in_bytes));
            }
            // if the string is longer than it was previously, then we need to allocate more memory
            else
            {
                // free the old entry
                ffree(entry->entry->address, entry->entry->length);
                // allocate enough memory for the new entry
                int pos = fmalloc(entry->unicode_string.str_len_in_bytes + 0x18);
                // update the entry's position
                entry->entry->address = pos;
                // re-write the entry table
                writeEntryTable(openedFile);
                // write the entry meta data
                char temp[0x18];
                writeSettingMetaData(temp, entry->entry->identifier, SET_UNICODE);
                openedFile->setPosition(pos);
                openedFile->write(temp, 0x18);
                // write the string data
                openedFile->write(*entry->unicode_string.str);
            }
    }
}

void XDBF::writeSettingEntryPrivate(void *data, int len, Entry *e)
{
    // seek to the entry data position
    openedFile->setPosition(e->address + 0x10);

    // change the endian of the value for writing
    SwapEndian(data, 1, len);
    // write the data to the entry
    openedFile->write(data, len);
    // change the endian back to the original so it's not screwed up
    SwapEndian(data, 1, len);
}

void XDBF::cleanGPD()
{
    // create a new fileIo for writing the new gpd
    FileIO *newFile = new FileIO(filePath.append(".4253efd018451c05326e15f1e1bcf402"));

    // set the free mem table count to 0, because we're overwriting all the unused memory
    h->freeMemoryTableEntryCount = 0;

    // write the header to the file
    newFile->write(h->magic);
    newFile->write(h->version);
    newFile->write(h->entryTableLength);
    newFile->write(h->entryCount);
    newFile->write(h->freeMemoryTableLength);
    newFile->write(h->freeMemoryTableEntryCount);

    // length of the entry table and free mem table in bytes
    int paddingLen = get_offset(0) - 0x18;

    // null out that part of the file
    char *nullData = new char[paddingLen];
    memset(nullData, 0, paddingLen);
    newFile->write(nullData, paddingLen);
    delete[] nullData;

    // write all of the entries
    for (int i = 0; i < privateEntries->size(); i++)
    {
        // allocate some memory to hold the entry in
        char *temp = new char[privateEntries->at(i).length];

        // read the entry
        openedFile->setPosition(privateEntries->at(i).address);
        openedFile->read(temp, privateEntries->at(i).length);

        // update the entry's address
        privateEntries->at(i).address = newFile->getPosition();

        // write the new entry
        newFile->write(temp, privateEntries->at(i).length);

        // deallocate the memory
        delete[] temp;
    }

    // write all the entry table with the updated addresses
    writeEntryTable(newFile);

    // get the length of the file
    newFile->setPosition(0, ios_base::end);
    unsigned int addr = getFakeOffset(newFile->getPosition());

    // write file info free mem setting entry
    newFile->setPosition((h->entryTableLength * 0x12) + 0x18);
    newFile->write(addr);
    newFile->write(0xFFFFFFFF - addr);

    // the file to read/write from now will be the one we just created
    openedFile->close();
    delete openedFile;
    newFile->close();
    delete newFile;

    string s = filePath.substr(0, filePath.length() - 33);

    // delete the old file
    if (remove(s.c_str()) != 0)
        throw "Error deleting file.";

    // rename the new file to the original file's name
    if (rename(filePath.c_str(), s.c_str()) != 0)
        throw "Error renaming file.";

    filePath = s;
    openedFile = new FileIO(filePath);
}

void XDBF::injectStringEntry(wstring wstr, unsigned long long id)
{
    // if no id was provided, then we need to get the next available one
    if (id == 0)
        id = getNextId(ET_STRING);

    // create a character array to hold the data to write, we need to
    // make a copy so that we can reverse the endian of the wstring
    wchar_t *dataToWrite = new wchar_t[wstr.length() + 1];

    // copy the string to the temp location
    memcpy(dataToWrite, &wstr.at(0), WSTRING_BYTES(wstr.length()));

    // change the endian
    SwapEndianUnicode(dataToWrite, WSTRING_BYTES(wstr.length()));

    // inject the new string entry
    injectEntry_private(ET_STRING, (char*)dataToWrite, WSTRING_BYTES(wstr.length()), id);

    // give the memory back
    delete[] dataToWrite;
}


XDBF* XDBFcreate(string filePath, GPD_Type type, char *imageData, size_t imageDataLen, wstring *gameName)
{
    FileIO newFile(filePath);

    // write the header of the new gpd
    newFile.write("XDBF");
    newFile.write((unsigned int)0x10000);
    newFile.write((unsigned int)0x200);
    newFile.write((unsigned long long)0x200);
    newFile.write((unsigned int)1);

    // create a buffer to hold the crap to write
    char zeroBuff[0x340];
    memset(zeroBuff, 0, 0x340);

    // write the entry and memory table, since the file is new all the entries will be null
    for (int i = 0; i < 16; i++)
        newFile.write(zeroBuff, 0x340);

    unsigned int fileLen;
    if (type == Achievement)
        fileLen = (imageDataLen + ((gameName->length() + 1) * 2) + 0x30);
    else if (type == Dashboard)
        fileLen = (imageDataLen + 0x6E);
    else
        fileLen = 0x18;

    // set the file data free mem table entry
    newFile.setPosition(0x2418);
    newFile.write(fileLen);
    newFile.write(0xFFFFFFFF - fileLen);

    newFile.setPosition(0x3418);

    // close the new file
    newFile.close();

    // open the new file as an XDBF file
    XDBF *toReturn = new XDBF(filePath);

    switch (type)
    {
        case Achievement:
            // inject the sync information for all types of possible entries
            injectSyncStuff(toReturn, ET_ACHIEVEMENT);
            injectSyncStuff(toReturn, ET_SETTING);

            if (imageData == NULL)
                throw "Image must be provided for game GPD!";
            if (gameName == NULL)
                throw "Gamge name must be provided for game GPD!";

            // inject the game thumbnail
            toReturn->injectImageEntry(imageData, imageDataLen, TITLE_INFORMATION);

            // inject the title name
            toReturn->injectStringEntry(*gameName, TITLE_INFORMATION);
            break;

        case Dashboard:
            // inject the sync information for all types of possible entries
            injectSyncStuff(toReturn, ET_ACHIEVEMENT);
            injectSyncStuff(toReturn, ET_SETTING);
            injectSyncStuff(toReturn, ET_TITLE);

            if (imageData == NULL)
                throw "Image must be provided for dashboard GPD!";

            // inject the game thumbnail
            toReturn->injectImageEntry(imageData, imageDataLen, TITLE_INFORMATION);

            // inject the title name
            toReturn->injectStringEntry(L"Xbox 360 Dashboard", TITLE_INFORMATION);

            break;

        case AvatarAward:
            // inject the sync information for all types of possible entries
            injectSyncStuff(toReturn, ET_AVATAR_AWARD);

            break;

    }

    return toReturn;
}

vector<unsigned short> XDBF::getEntrySyncTypes()
{
    vector<unsigned short> toReturn;
    for (int i = 1; i < 6; i++)
        if (getEntryById((i == 6) ? 1 : SYNC_LIST, i) != NULL)
            toReturn.push_back(i);
    return toReturn;
}

void injectSyncStuff(XDBF *x, int type)
{
    // create a new sync data entry
    char syncData[0x18] = {0};
    syncData[7] = 1;

    // inject new sync data
    x->injectEntry_private(type, syncData, 0x18, SYNC_DATA);
    // inject new sync list
    x->injectEntry_private(type, syncData, 0, SYNC_LIST);
}
