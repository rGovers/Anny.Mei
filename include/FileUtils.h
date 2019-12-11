#pragma once

#include <istream>

#include "miniz.h"

char* ExtractFileFromArchive(const char* a_fileName, mz_zip_archive& a_archive);
#define IFSETTOATTVAL(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = attVal; }
#define IFSETTOATTVALCPY(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = new char[strlen(attVal)]; strcpy(val, attVal); }
#define IFSETTOATTVALI(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stoi(attVal); }
#define IFSETTOATTVALF(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stof(attVal); }