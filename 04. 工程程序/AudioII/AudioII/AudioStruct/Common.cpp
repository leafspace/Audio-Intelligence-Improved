#include "Common.h"
DataType::DataType()
{
    switch(DataType::sizeFlag) 
    {
        case i8Bit : this->data8B  = new int8_t(); break;
        case i16Bit: this->data16B = new int16_t(); break;
        case i32Bit: this->data32B = new int32_t(); break;
        default: break;
    }
}

DataType::DataType(const int data) : DataType()
{
    switch(DataType::sizeFlag) 
    {
        case i8Bit : (*this->data8B)  = data; break;
        case i16Bit: (*this->data16B) = data; break;
        case i32Bit: (*this->data32B) = data; break;
        default: break;
    }
}

DataType::~DataType()
{
    DataType::clearBuffer(this->data8B);
    DataType::clearBuffer(this->data16B);
    DataType::clearBuffer(this->data32B);
}

inline void DataType::clearBuffer(void* pointer)
{
    delete pointer;
    pointer = NULL;
}

DataType::operator int() const 
{
    switch(DataType::sizeFlag) 
    {
        case i8Bit : return *(this->data8B);
        case i16Bit: return *(this->data16B);
        case i32Bit: return *(this->data32B);
        default: return 0;
    }
}

int DataType::operator=(const int &dataType) const
{
    switch (DataType::sizeFlag)
	{
	case i8Bit : *this->data8B  = dataType;
	case i16Bit: *this->data16B = dataType;
	case i32Bit: *this->data16B = dataType;
	default: return 0;
	}

}

void DataType::setSizeFlag(const short sizeFlag)
{
    DataType::sizeFlag = sizeFlag;
}