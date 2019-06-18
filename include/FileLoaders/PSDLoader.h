#pragma once

#include "FileLoaders/ImageLoader.h"

#include <list>

#include "PascalString.h"

// https://www.adobe.com/devnet-apps/photoshop/fileformatashtml/
class PSDLoader : public ImageLoader
{
private:
    enum class e_ColorMode : unsigned short
    {
        Bitmap,
        Greyscale,
        Indexed,
        RGB,
        CMYK,
        Multichannel,
        Duotone,
        Lab
    };
    enum class e_ImageResourceID : unsigned short
    {
        MacPrintInfoRecord              = 1001,
        MacPageFormatInfo               = 1002,
        IndexedColorTable               = 1003,
        ResolutionInfo                  = 1005,
        AlphaName                       = 1006,
        CaptionPascal                   = 1008,
        BorderInfo                      = 1009,
        BackgroundColor                 = 1010,
        PrintFlags                      = 1011,
        GreyscaleHalftoneInfo           = 1012,
        ColorHalftoneInfo               = 1013,
        DuotoneHalftoneInfo             = 1014,
        GreyscaleTransferFunc           = 1015,
        ColorTransferFunc               = 1016,
        DuotoneTransferFunc             = 1017,
        DuotoneImageInfo                = 1018,
        BlackWhiteDot                   = 1019,
        EPSOptions                      = 1021,
        QuickMaskInfo                   = 1022,
        LayerStateInfo                  = 1024,
        WorkingPath                     = 1025,
        LayerGroupInfo                  = 1026,
        IPTCNAARecord                   = 1028,
        ImageMode                       = 1029,
        JPEGQualtity                    = 1030,
        PS4GridGuideInfo                = 1032,
        PS4ThumbnailResource            = 1033,
        PS4CopyrightFlag                = 1034,
        PS4URL                          = 1035,
        PS5ThumbnailResource            = 1036,
        PS5GlobalAngle                  = 1037,
        PS5ColorSamplerResource         = 1038,
        PS5ICCProfile                   = 1039,
        PS5Watermark                    = 1040,
        PS5ICCUTagProfile               = 1041,
        PS5EffectsVisible               = 1042,
        PS5SpotHalftone                 = 1043,
        PS5IDSeed                       = 1044,
        PS5UnicodeAlphaName             = 1045,
        PS6IndexedColorTableCount       = 1046,
        PS6TransparencyIndex            = 1047,
        PS6GlobalAltitude               = 1049,
        PS6Slices                       = 1050,
        PS6WorkflowURL                  = 1051,
        PS6XPEPJump                     = 1052,
        PS6AlphaIdentifiers             = 1053,
        PS6URLList                      = 1054,
        PS6VersionInfo                  = 1057,
        PS7EXIFData1                    = 1058,
        PS7EXIFData3                    = 1059,
        PS7XMPMetadata                  = 1060,
        PS7CaptionDigest                = 1061,
        PS7PrintScale                   = 1062,
        PSCSPixelAspectRatio            = 1064,
        PSCSLayerComps                  = 1065,
        PSCSAltDuotoneColors            = 1066,
        PSCSAltSpotColors               = 1067,
        PSCS2LayerSelectionID           = 1069,
        PSCS2HDRToningInfo              = 1070,
        PSCS2PrintInfo                  = 1071,
        PSCS2LayerGroups                = 1072,
        PSCS3ColorSamplerResource       = 1073,
        PSCS3MeasurementScale           = 1074,
        PSCS3TimelineInfo               = 1075,
        PSCS3SheetDisclosure            = 1076,
        PSCS3DisplayInfo                = 1077,
        PSCS3OnionSkins                 = 1078,
        PSCS4CountInfo                  = 1080,
        PSCS5PrintInfo                  = 1082,
        PSCS5PrintStyle                 = 1083,
        PSCS5MacNSPrintInfo             = 1084,
        PSCS5WindowsDevMode             = 1085,
        PSCS6AutoSaveFilePath           = 1086,
        PSCS6AutoSaveFormat             = 1087,
        PSCCPathSelectionState          = 1088,
        PathInfo                        = 2000,
        NameOfClippingPath              = 2999,
        PSCCOriginPathInfo              = 3000,
        PlugInResouces                  = 4000,
        ImageReadyVariable              = 7000,
        ImageReadyDataSets              = 7001,
        ImageReadyDefaultSelectState    = 7002,
        ImageReady7RolloverExpState     = 7003,
        ImageReadyRolloverExpState      = 7004,
        ImageReadySaveLayerSettings     = 7005,
        ImageReadyVersion               = 7006,
        PSCS3LightroomWorkflow          = 8000,
        PrintFlagsInfo                  = 10000 
    };

    struct Header
    {
        unsigned int Signature;
        unsigned short Version;
        unsigned short Channels;
        unsigned int Height;
        unsigned int Width;
        unsigned short ColorDepth;
        e_ColorMode ColorMode;
    };
    struct ImageResourceData
    {
        unsigned int Signature;
        e_ImageResourceID Identifier;
        PascalString<unsigned int> String;
        unsigned int ResourceSize;
        void* Data;
    };

    Header*                      m_header;
    std::list<ImageResourceData> m_imageResourceData;

    unsigned int ReadHeader(const char* a_data);
    unsigned int ReadColorModeData(const char* a_data);
    unsigned int ReadImageResources(const char* a_data);
    unsigned int ReadLayerMaskInfo(const char* a_data);
    unsigned int ReadImageData(const char* a_data);
protected:

public:
    PSDLoader() = delete;
    PSDLoader(const char* a_path);
    ~PSDLoader();
    
    virtual int GetLayerCount() const;
    virtual LayerMeta* GetLayerMeta(int a_index) const;
    virtual Layer* GetLayer(int a_index) const;
};