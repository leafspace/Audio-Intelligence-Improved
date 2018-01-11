#pragma once
#include <stdio.h>
#include <fstream>
#include <stdint.h>
using namespace std;

enum Type
{
    i8Bit = 1,
    i16Bit,
    i32Bit,
};

class DataType
{
private:
    static short sizeFlag;
    int8_t *data8B = NULL;
    int16_t *data16B = NULL;
    int32_t *data32B = NULL;
    inline static void clearBuffer(void* pointer);
public:
    DataType();
    DataType(const int data);
    ~DataType();
    operator int() const;
    int operator=(const int &dataType) const;
    static void setSizeFlag(const short sizeFlag);
};

short DataType::sizeFlag = i8Bit;