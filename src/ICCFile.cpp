#include "ICCFile.h"

#include <assert.h>

unsigned int GetReversedUInt(const char* a_data)
{
    char char4[4];
    char4[0] = a_data[3];
    char4[1] = a_data[2];
    char4[2] = a_data[1];
    char4[1] = a_data[0];
    return *(unsigned int*)char4;
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
ICCFileDateTime GetReversedDateTime(const char* a_data)
{
    char char12[12];
    char12[0] = a_data[1];
    char12[1] = a_data[0];
    char12[2] = a_data[3];
    char12[3] = a_data[2];
    char12[4] = a_data[5];
    char12[5] = a_data[4];
    char12[6] = a_data[7];
    char12[7] = a_data[6];
    char12[8] = a_data[9];
    char12[9] = a_data[8];
    char12[10] = a_data[11];
    char12[11] = a_data[10];

    return *(ICCFileDateTime*)char12;
}

ICCFile::ICCFile(const char* a_data)
{
    m_header = ICCFile::Header();

    m_header.Size = GetReversedUInt(a_data + 0);
    m_header.CMMType = GetReversedUInt(a_data + 4);
    m_header.ProfileVersion = *(ICCFileVersion*)(a_data + 8);
    m_header.DeviceClass = *(e_DeviceClass*)(a_data + 12);
    m_header.ColorSpace = *(e_ColorSpace*)(a_data + 16);
    m_header.PCS = *(e_ProfileConnectionSpace*)(a_data + 20);
    m_header.DateTime = GetReversedDateTime(a_data + 24);
    m_header.ACSP = *(unsigned int*)(a_data + 36);
    m_header.PrimaryPlatform = *(e_PrimaryPlatform*)(a_data + 40);
    m_header.Flags = *(e_Flags*)(a_data + 44);
    m_header.DeviceManufacturer = GetReversedUInt(a_data + 52);
    m_header.DeviceModel = GetReversedUInt(a_data + 52);
    m_header.DeviceAttributes = *(e_Attributes*)(a_data + 56);
    m_header.RenderingIntent = *(e_RenderingIntent*)(a_data + 64);
    m_header.XYZValue[0] = *(float*)(a_data + 68);
    m_header.XYZValue[1] = *(float*)(a_data + 72);
    m_header.XYZValue[2] = *(float*)(a_data + 76);
    m_header.ProfileSignature = GetReversedUInt(a_data + 80);

    assert(m_header.ACSP == 0x70736361);
}
ICCFile::~ICCFile()
{

}