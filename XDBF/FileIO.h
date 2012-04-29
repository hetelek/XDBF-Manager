#pragma once

#include <iostream>
#include <fstream>
#include <vector>

using std::fstream;
using std::ios;
using std::string;
using std::wstring;
using std::vector;

typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef long long INT64;

enum origin
{
    FileStart,
    CurrentPosition,
    FileEnd
};

class FileIO
{
    public:
        bool isOpened();
        FileIO(string path);
        void setPosition(long pos, origin = FileStart);
        int getPosition();
        void close();

        // reading
        void read(void *destination, size_t len);
        short readInt16();
        UINT16 readUInt16();
        int readInt32();
        UINT32 readUInt32();
        INT64 readInt64();
        UINT64 readUInt64();
        double readDouble();
        float readFloat();
        string readASCIIString(size_t len = 0);
        wstring readUnicodeString(size_t len = 0);

        // writing
        void write(void *destination, size_t len);
        void writeInt16(short num, size_t len = sizeof(short));
        void writeUInt16(UINT16 num, size_t len = sizeof(UINT16));
        void writeInt32(int num, size_t len = sizeof(int));
        void writeUInt32(UINT32 num, size_t len = sizeof(UINT32));
        void writeInt64(INT64 num, size_t len = sizeof(INT64));
        void writeUInt64(UINT64 num, size_t len = sizeof(UINT64));
        void writeASCIIString(string str);
        void writeUnicodeString(wstring wstr);

        // endian swapping
        static void SwapEndian(void *arr, int elemSize, int len);
        static void SwapEndian(UINT16 *s);
        static void SwapEndian(UINT32 *i);
        static void SwapEndian(UINT64 *l);
        static void SwapEndianUnicode(wchar_t *str, int unicode_len);

        ~FileIO(void);
    private:
        fstream *io;
        bool isLitttleEndian;
        void checkEndian();
};
