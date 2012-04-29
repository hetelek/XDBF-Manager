#pragma once

#include <iostream>
#include <fstream>
#include <vector>

using std::fstream;
using std::ios;
using std::string;
using std::wstring;
using std::vector;
using std::ios_base;

typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef long long INT64;

class FileIO
{
    public:
        bool isOpened();
        FileIO(string path);
        void setPosition(long pos, ios_base::seekdir o = ios_base::beg);
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
        void write(short num, size_t len = sizeof(short));
        void write(UINT16 num, size_t len = sizeof(UINT16));
        void write(int num, size_t len = sizeof(int));
        void write(UINT32 num, size_t len = sizeof(UINT32));
        void write(INT64 num, size_t len = sizeof(INT64));
        void write(UINT64 num, size_t len = sizeof(UINT64));
        void write(string str);
        void write(wstring wstr);

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
