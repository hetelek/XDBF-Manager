#ifndef XDBF_H
#define XDBF_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdbfhelper.h>
#include <time.h>
#include <vector>
#include "FileIO.h"

using std::string;
using std::vector;

#define ET_ACHIEVEMENT (unsigned short)1
#define ET_IMAGE (unsigned short)2
#define ET_SETTING (unsigned short)3
#define ET_TITLE (unsigned short)4
#define ET_STRING (unsigned short)5
#define ET_AVATAR_AWARD (unsigned short)6

#define SET_CONTEXT     ((unsigned char)0)
#define SET_INT32       ((unsigned char)1)
#define SET_INT64       ((unsigned char)2)
#define SET_DOUBLE      ((unsigned char)3)
#define SET_UNICODE     ((unsigned char)4)
#define SET_FLOAT       ((unsigned char)5)
#define SET_BINARY      ((unsigned char)6)
#define SET_DATETIME    ((unsigned char)7)
#define SET_NULL        ((unsigned char)0xFF)

#define WSTRING_BYTES(wstringSize) ((wstringSize + 1) * 2)

class XDBF
{
public:
    XDBF(string path);
    ~XDBF();
    Entry* get_entries();
    char* extract_entry(Entry *entry);
    Entry* get_entry_by_id(long long identifier, int type);
    Header* get_header();
    FileIO *get_file();
    void close();
    static std::string get_setting_entry_name(Setting_Entry *s_entry);
    Setting_Entry* get_setting_entry(Entry *entry);
    Title_Entry* get_title_entry(Entry *entry);
    Achievement_Entry* get_achievement_entry(Entry *entry);
    Sync_List get_sync_list(int et_type, unsigned long long identifier = SYNC_LIST);
    Sync_Data get_sync_data(int et_type, unsigned long long identifier = SYNC_LIST);
    Sync_Entry* get_sync(int et_type, unsigned long long identifier = SYNC_LIST);
    void update_sync_list_entry(Sync_Entry entry, int et_type, SyncEntryStatus status, unsigned long long identifier = SYNC_LIST);
    void write_sync_list(Sync_List *sl);
    Avatar_Award_Entry* get_avatar_award_entry(Entry *entry);
    void writeEntry(Entry *entry, Achievement_Entry *chiev);
    void writeEntry(Title_Entry *entry);
    void writeEntry(Avatar_Award_Entry *e);
    void writeEntry(Setting_Entry* entry);
    void writeSettingEntryPrivate(void *data, int len, Entry *e);
    void injectAchievementEntry(Achievement_Entry *entry, unsigned long long id = 0);
    void injectImageEntry(char *imageData, unsigned int len, unsigned long long id);
    void injectTitleEntry(Title_Entry *entry, unsigned long long id = 0);
    void injectSettingEntry(Setting_Entry *entry, unsigned long long id = 0);
    void injectStringEntry(wstring wstr, unsigned long long id = 0);
    unsigned long long getNextId(unsigned short type);
    void removeEntry(Entry *entry);
    void removeSyncEntry(unsigned long long identifier, Sync_List *list);
    unsigned int fmalloc(size_t dataLen);
    void ffree(unsigned int address, size_t size);
    int get_offset(unsigned int offset_specifier);
    int getFakeOffset(unsigned int realAddress);
    void cleanGPD();

    static std::string FILETIME_to_string(FILETIME *pft);
    friend XDBF* XDBFcreate(string filePath, GPD_Type type, char *imageData = NULL, size_t imageDataLen = 0, wstring *gameName = NULL);
    friend void injectSyncStuff(XDBF *x, int type);

private:
    Header h[1];
    FreeMemoryTable freeMemTable;
    FileIO *opened_file;
    vector<Entry> *private_entries;
    bool reverse;
    string filePath;
    Entry* injectEntry_private(unsigned int type, char *entryData, unsigned int dataLen, unsigned long long id);
    void swapAchievementEndianness(Achievement_Entry *entry);
    void swapTitleEndianness(Title_Entry *entry);
    void writeEntryTable(FileIO *io);
    void writeFreeMemoryTable();
    void writeSettingMetaData(char *buffer, unsigned long long id, unsigned char type);
    void injectSettingEntry_private(void *value, int len, Setting_Entry *entry, unsigned long long id);
};

bool compareFunction(Entry e1, Entry e2);

#endif // XDBF_H
