#pragma once

struct ICCFileVersion;
struct ICCFileDateTime;

struct ICCFileDateTime
{
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hours;
    unsigned short Minutes;
    unsigned short Seconds;
};
struct ICCFileVersion 
{
    char Major;
    char Minor;
    short Reserved;
};

// No image libraries that I know of support it but Krita uses it
// Love it when you have to reinvent the wheel just to do the other things
class ICCFile
{
private:
    enum class e_DeviceClass : unsigned int
    {
        InputDevice          = 0x726E6373,
        DisplayDevice        = 0x72746E6D,
        OutputDevice         = 0x72747270,
        DeviceLink           = 0x6B6E696C,
        ColorSpaceConversion = 0x63617073,
        Abstract             = 0x74736261,
        NamedColor           = 0x6C636D6E
    };
    enum class e_ColorSpace : unsigned int
    {
        XYZ     = 0x205A5958,
        LAB     = 0x2062614C,
        LUV     = 0x2076754C,
        YCbCr   = 0x72624359,
        YXY     = 0x20797859,
        RGB     = 0x20424752,
        Grey    = 0x59415247,
        HSV     = 0x20565348,
        HLS     = 0x20534C48,
        CMYK    = 0x4B594D43,
        CMY     = 0x20594D43,
        CLR2    = 0x524C4332,
        CLR3    = 0x524C4333,
        CLR4    = 0x524C4334,
        CLR5    = 0x524C4335,
        CLR6    = 0x524C4336,
        CLR7    = 0x524C4337,
        CLR8    = 0x524C4338,
        CLR9    = 0x524C4339,
        CLR10   = 0x524C433A,
        CLR11   = 0x524C433B,
        CLR12   = 0x524C433C,
        CLR13   = 0x524C433D,
        CLR14   = 0x524C433E,
        CLR15   = 0x524C433F
    };
    enum class e_PrimaryPlatform : unsigned int
    {
        Apple       = 0x4C505041,
        Microsoft   = 0x5446534D,
        Silicon     = 0x20494753,
        SunMicro    = 0x574E5553,
        Taligent    = 0x544E4754
    };
    enum class e_Flags : unsigned int
    {
        Embedded    = 1,
        Independent = 2,
    };
    enum class e_Attributes : unsigned long
    {
        Reflective  = 1,
        Glossy      = 2,
        Positive    = 4,
        Color       = 8
    };
    enum class e_RenderingIntent : unsigned int
    {
        Perceptual = 0,
        MediaRelativeColorimetric = 1,
        Saturation = 2,
        ICCAbsoluteColorimetric = 3
    };

    struct Header
    {
        unsigned int Size;
        unsigned int CMMType;
        ICCFileVersion ProfileVersion;
        e_DeviceClass DeviceClass;
        e_ColorSpace ColorSpace;
        e_ColorSpace PCS;
        ICCFileDateTime DateTime;
        unsigned int ACSP;
        e_PrimaryPlatform PrimaryPlatform;
        e_Flags Flags;
        unsigned int DeviceManufacturer;
        unsigned int DeviceModel;
        e_Attributes DeviceAttributes;
        e_RenderingIntent RenderingIntent;
        float XYZValue[3];
        int ProfileSignature;
    };
    struct TagDefinition
    {

    };

    Header m_header;

protected:

public:
    ICCFile() = delete;
    ICCFile(const char* a_data);
    ~ICCFile();
};