#pragma once

#include <string>

#include "miniz.h"

inline float StringToFloat(const char* a_stringStart, const char* a_stringEnd)
{
    int diff = a_stringEnd - a_stringStart;

    char* tmp = new char[diff];
    memcpy(tmp, a_stringStart + 1, diff);
    tmp[diff - 1] = 0;

    float val = std::stof(tmp);

    delete[] tmp;

    return val;
}

#define STR2V3(attVal, val) { char* str = (char*)attVal; str = strchr(str, '{');  char* endStr = strchr(str, ','); val.x = StringToFloat(str, endStr);  str = endStr; endStr = strchr(str + 1, ','); val.y = StringToFloat(str, endStr); str = endStr; endStr = strchr(str + 1, '}'); val.z = StringToFloat(str, endStr); }
#define STR2V4(attVal, val) { char* str = (char*)attVal; str = strchr(str, '{');  char* endStr = strchr(str, ','); val.x = StringToFloat(str, endStr);  str = endStr; endStr = strchr(str + 1, ','); val.y = StringToFloat(str, endStr); str = endStr; endStr = strchr(str + 1, ','); val.z = StringToFloat(str, endStr); str = endStr; endStr = strchr(str + 1, '}'); val.w = StringToFloat(str, endStr); }

char* ExtractFileFromArchive(const char* a_fileName, mz_zip_archive& a_archive);
#define IFSETTOATTVAL(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = attVal; }
#define IFSETTOATTVALCPY(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = new char[strlen(attVal)]; strcpy(val, attVal); }
#define IFSETTOATTVALI(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stoi(attVal); }
#define IFSETTOATTVALF(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stof(attVal); }
#define IFSETTOATTVALD(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stod(attVal); }
#define IFSETTOATTVALV3(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) STR2V3(attVal, val)
#define IFSETTOATTVALV4(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) STR2V4(attVal, val)