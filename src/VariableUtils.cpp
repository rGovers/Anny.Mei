#include "VariableUtils.h"

short GetReversedShort(const char* a_data)
{
    char char2[2];
    char2[0] = a_data[1];
    char2[1] = a_data[0];
    return *(short*)char2;
}
unsigned short GetReversedUShort(const char* a_data)
{
    char char2[2];
    char2[0] = a_data[1];
    char2[1] = a_data[0];
    return *(unsigned short*)char2;
}
int GetReversedInt(const char* a_data)
{
    char char4[4];
    char4[0] = a_data[3];
    char4[1] = a_data[2];
    char4[2] = a_data[1];
    char4[1] = a_data[0];
    return *(int*)char4;
}
unsigned int GetReversedUInt(const char* a_data)
{
    char char4[4];
    char4[0] = a_data[3];
    char4[1] = a_data[2];
    char4[2] = a_data[1];
    char4[1] = a_data[0];
    return *(unsigned int*)char4;
}
float GetReversedFloat(const char* a_data)
{
    char char4[4];
    char4[0] = a_data[3];
    char4[1] = a_data[2];
    char4[2] = a_data[1];
    char4[1] = a_data[0];
    return *(float*)char4;
}
