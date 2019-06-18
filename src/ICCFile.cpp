#include "FileLoaders/ICCFile.h"

#include <assert.h>
#include <string.h>

#include "VariableUtils.h"

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

void ICCFile::LoadHeader(const char* a_data)
{
    m_header = ICCFile::Header();

    m_header.Size = GetReversedUInt(a_data + 0);
    m_header.CMMType = GetReversedUInt(a_data + 4);
    m_header.ProfileVersion = *(ICCFileVersion*)(a_data + 8);
    m_header.DeviceClass = *(e_DeviceClass*)(a_data + 12);
    m_header.ColorSpace = *(e_ColorSpace*)(a_data + 16);
    m_header.PCS = *(e_ColorSpace*)(a_data + 20);
    m_header.DateTime = GetReversedDateTime(a_data + 24);
    m_header.ACSP = *(unsigned int*)(a_data + 36);
    m_header.PrimaryPlatform = *(e_PrimaryPlatform*)(a_data + 40);
    m_header.Flags = *(e_Flags*)(a_data + 44);
    m_header.DeviceManufacturer = GetReversedUInt(a_data + 52);
    m_header.DeviceModel = GetReversedUInt(a_data + 52);
    m_header.DeviceAttributes = *(e_Attributes*)(a_data + 56);
    m_header.RenderingIntent = *(e_RenderingIntent*)(a_data + 64);
    m_header.XYZValue[0] = GetReversedFloat(a_data + 68);
    m_header.XYZValue[1] = GetReversedFloat(a_data + 72);
    m_header.XYZValue[2] = GetReversedFloat(a_data + 76);
    m_header.ProfileCreator = GetReversedUInt(a_data + 80);
    m_header.ProfileSignature[0] = *(unsigned long*)(a_data + 84);
    m_header.ProfileSignature[1] = *(unsigned long*)(a_data + 92);
    m_header.SpectralPCS = GetReversedUInt(a_data + 100);
    m_header.SpectralWave = GetReversedUInt(a_data + 104);
    m_header.BISpectralWave = GetReversedUInt(a_data + 110);
    m_header.MCSSignature = GetReversedUInt(a_data + 116);
    m_header.DeviceSubClass = *(e_DeviceClass*)(a_data + 120);
}
void ICCFile::LoadTagDefinition(const char* a_data)
{
    const char* dataPtr = a_data + 128;

    m_tags = GetReversedUInt(dataPtr + 0);
    m_endIndex = 4;

    m_tagDefinition = new TagDefinition[m_tags];

    TagDefinition tagDef;
    for (int i = 0; i < m_tags; ++i)
    {
        tagDef.TagSignature = *(e_TagSignature*)(dataPtr + m_endIndex + 0);
        tagDef.DataOffset = GetReversedUInt(dataPtr + m_endIndex + 4);
        tagDef.ElementSize = GetReversedUInt(dataPtr + m_endIndex + 8);

        m_endIndex += 12;

        m_tagDefinition[i] = tagDef;
    }
}

ICCFile::ICCFile(const char* a_data, int a_size)
{
    LoadHeader(a_data);

    assert(m_header.ACSP == 0x70736361);

    LoadTagDefinition(a_data);

    const int index = m_header.Size + m_endIndex;
    const int size = a_size - index;
    m_dataBlob = new char[size];
    memcpy(m_dataBlob, a_data + index, size);
}
ICCFile::~ICCFile()
{
    delete[] m_tagDefinition;
    delete[] m_dataBlob;
}

char* ICCFile::GetColorData()
{
    CurveType redTRC;
    CurveType greenTRC;
    CurveType blueTRC;

    TagDefinition* redColorantDef;
    TagDefinition* greenColorantDef;
    TagDefinition* blueColorantDef;

    // TODO: Sort out this mess
    // I have been looking around and the offsets on the tags seem to be incorrect
    // I am not sure if I have stuffed up, Krita is exporting an invalid file or I have misunderstood something
    // I am going to put this to the side considering there are other file formats I can use

    return new char[0];
}