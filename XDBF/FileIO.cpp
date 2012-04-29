#include "FileIO.h"

FileIO::FileIO(string path)
{
    io = new fstream(path.c_str(), fstream::in | fstream::out | fstream::binary);

    // check for little endian architecture
    checkEndian();
}

bool FileIO::isOpened()
{
    return io->is_open();
}

void FileIO::close()
{
    io->close();
}

int FileIO::getPosition()
{
    return io->tellg();
}

FileIO::~FileIO(void)
{
    if(io->is_open())
        io->close();
    delete io;
}

void FileIO::checkEndian()
{
    int i = 1;
    isLitttleEndian = (*(char*)&i == 1);
}

void FileIO::setPosition(long pos, origin o)
{
    io->clear();
    io->seekg(pos, 0);
}

void FileIO::read(void *destination, size_t len)
{
    io->read((char*)destination, len);
}

short FileIO::readInt16()
{
    short toReturn;
    read(&toReturn, 2);
    if (isLitttleEndian)
        SwapEndian((UINT16*)&toReturn);
    return toReturn;
}

UINT16 FileIO::readUInt16()
{
    return (UINT16)readInt16();
}

int FileIO::readInt32()
{
    int toReturn;
    read(&toReturn, 4);
    if (isLitttleEndian)
        SwapEndian((UINT32*)&toReturn);
    return toReturn;
}

UINT32 FileIO::readUInt32()
{
    int int32 = readInt32();
    return *(UINT32*)&int32;
}

INT64 FileIO::readInt64()
{
    INT64 toReturn;
    read(&toReturn, 8);
    if (isLitttleEndian)
        SwapEndian((UINT64*)&toReturn);
    return toReturn;
}

UINT64 FileIO::readUInt64()
{
    return (UINT64)readInt64();
}

double FileIO::readDouble()
{
    UINT64 int64 = readInt64();
    return *(double*)&int64;
}

float FileIO::readFloat()
{
    int int32 = readInt32();
    return *(float*)&int32;
}

string FileIO::readASCIIString(size_t len)
{
    string result;
    char character = -1;
    if (len == 0)
    {
        do
        {
            read(&character, 1);
            result.append(&character, 1);
        }
        while (character != 0);
    }
    else
    {
        for (UINT32 i = 0; i < len; i++)
        {
            read(&character, 1);
            result.append(&character, 1);
        }
    }
    return result;
}

wstring FileIO::readUnicodeString(size_t len)
{
    wstring result;
    vector<wchar_t> *chars = new vector<wchar_t>;
    wchar_t character = -1;
    if (len == 0)
    {
        do
        {
            character = (wchar_t)readUInt16();
            chars->push_back(character);
        }
        while (character != 0);

        result = wstring(&chars->at(0));
    }
    else
    {
        for (UINT32 i = 0; i < len; i++)
        {
            character = (wchar_t)readUInt16();
            result.append(&character);
        }
    }
    if (!isLitttleEndian)
    {
        wchar_t *temp = new wchar_t[len + 1];
        memcpy(temp, result.c_str(), len);
        SwapEndianUnicode(temp, len);
        result = wstring(temp);
    }
    return result;
}

// writing
void FileIO::write(void *destination, size_t len)
{
    io->write((char*)destination, len);
    io->flush();
}

void FileIO::writeInt16(short num, size_t len)
{
    if (isLitttleEndian)
        SwapEndian((UINT16*)&num);
    write(&num, len);
}

void FileIO::writeUInt16(UINT16 num, size_t len)
{
    if (isLitttleEndian)
        SwapEndian(&num);
    write(&num, len);
}

void FileIO::writeUInt32(UINT32 num, size_t len)
{
    if (isLitttleEndian)
        SwapEndian(&num);
    write(&num, len);
}

void FileIO::writeInt32(int num, size_t len)
{
    if (isLitttleEndian)
        SwapEndian((UINT32*)&num);
    write(&num, len);
}

void FileIO::writeInt64(INT64 num, size_t len)
{
    if (isLitttleEndian)
        SwapEndian((UINT64*)&num);
    write(&num, len);
}

void FileIO::writeUInt64(UINT64 num, size_t len)
{
    if (isLitttleEndian)
        SwapEndian(&num);
    write(&num, len);
}

void FileIO::writeASCIIString(string str)
{
    char *data = new char[str.size()];
    memcpy(data, str.c_str(), str.size());
    write(data, str.size());

    delete[] data;
}

void FileIO::writeUnicodeString(wstring wstr)
{
    if (isLitttleEndian)
        for (int i = 0; i < wstr.size(); i++)
            SwapEndian((UINT16*)&wstr[i]);
    write((void*)wstr.c_str(), (wstr.size() + 1) * 2);
}

// endian swaps
void FileIO::SwapEndian(void *arr, int elemSize, int len)
{
    void *temp = new char[elemSize];
    if (temp == NULL)
        throw 1;

    for (int i = 0; i < (len / 2); i++)
    {
        memcpy(temp, ((char*)arr) + (i * elemSize), elemSize);
        memcpy(((char*)arr) + (i * elemSize), ((char*)arr) + (((len - 1) * elemSize) - (i * elemSize)), elemSize);
        memcpy(((char*)arr) + (((len - 1) * elemSize) - (i * elemSize)), temp, elemSize);
    }

    delete temp;
}

void FileIO::SwapEndian(UINT16 *s)
{
    SwapEndian(s, sizeof(char), sizeof(UINT16));
}

void FileIO::SwapEndian(UINT32 *i)
{
    SwapEndian(i, sizeof(char), sizeof(UINT32));
}

void FileIO::SwapEndian(UINT64 *ll)
{
    SwapEndian(ll, sizeof(char), sizeof(UINT64));
}

void FileIO::SwapEndianUnicode(wchar_t *str, int unicode_len)
{
    for (int i = 0; i < unicode_len / 2; i++)
        SwapEndian(&str[i], 1, 2);
}
