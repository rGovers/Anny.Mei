#pragma once

struct ICCFileVersion;
struct ICCFileDateTime;

struct ICCFileDateTime
{
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
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
    enum e_Flags : unsigned int
    {
        Flags_None        = 0,
        Flags_Embedded    = 1,
        Flags_Independent = 2,
    };
    enum e_Attributes : unsigned long
    {
        Attributes_None        = 0,
        Attributes_Reflective  = 1,
        Attributes_Glossy      = 2,
        Attributes_Positive    = 4,
        Attributes_Color       = 8
    };
    enum class e_RenderingIntent : unsigned int
    {
        Perceptual = 0,
        MediaRelativeColorimetric = 1,
        Saturation = 2,
        ICCAbsoluteColorimetric = 3
    };
    enum class e_TagSignature : unsigned int
    {
        AToB0               = 0x30423241,
        AToB1               = 0x31423241,
        AToB2               = 0x32423241,
        BlueColorant        = 0x5A595862,
        BlueTRC             = 0x43525462,
        BToA0               = 0x30413242,
        BToA1               = 0x31413242,
        BToA2               = 0x32413242,
        CalibrationDataTime = 0x746C6163,
        CharTarget          = 0x67726174,
        ChromaticAdaptation = 0x64616863,
        Chromaticity        = 0x6D726863,
        Copyright           = 0x74727063,
        CrdInfo             = 0x69647263,
        DeviceMfgDesc       = 0x646E6D64,
        DeviceModelDesc     = 0x64646D64,
        DeviceSettings      = 0x73766564,
        Gamut               = 0x746D6167,
        GreyTRC             = 0x4352546B,
        GreenColorant       = 0x5A595867,
        GreenTRC            = 0x43525467,
        Luminance           = 0x696D756C,
        Measurement         = 0x7361656D,
        MediaBlackPoint     = 0x74706B62,
        MediaWhitePoint     = 0x74707477,
        NamedColor          = 0x6C6F636E,
        NamedColor2         = 0x326C636E,
        // Covered by patent apparently...
        OutputResponse      = 0x70736572,
        Preview0            = 0x30657270,
        Preview1            = 0x31657270,
        Preview2            = 0x32657270,
        ProfileDescription  = 0x63736564,
        ProfileSequenceDesc = 0x71657370,
        Ps2CRD0             = 0x30647370,
        Ps2CRD1             = 0x31647370,
        Ps2CRD2             = 0x32647370,
        Ps2CRD3             = 0x33647370,
        Ps2CSA              = 0x73327370,
        Ps2RenderingIntent  = 0x69327370,
        RedColorantTag      = 0x5A595872,
        RedTRC              = 0x43525472,
        ScreeningDesc       = 0x64726373,
        Screening           = 0x6E726373,
        Technology          = 0x68636574,
        Ucrbg               = 0x20646662,
        ViewingCondDesc     = 0x64657576,
        ViewingConditions   = 0x77656976
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
        e_TagSignature TagSignature;
        unsigned int DataOffset;
        unsigned int ElementSize;
    };

    Header         m_header;
    unsigned int   m_tags;
    TagDefinition* m_tagDefinition;

    void LoadHeader(const char* a_data);
    void LoadTagDefinition(const char* a_data);
protected:

public:
    ICCFile() = delete;
    ICCFile(const char* a_data);
    ~ICCFile();
};