#pragma once

#include <istream>

#define GETFILEDATA(dataBuffer, file) { std::istream* stream = file->GetDecompressionStream(); std::size_t len = file->GetSize(); dataBuffer = new char[len]; stream->read(dataBuffer, len); file->CloseDecompressionStream(); }
#define IFSETTOATTVAL(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = attVal; }
#define IFSETTOATTVALCPY(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = new char[strlen(attVal)]; strcpy(val, attVal); }
#define IFSETTOATTVALI(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stoi(attVal); }
#define IFSETTOATTVALF(aCmp, bCmp, val, attVal) if (strcmp(aCmp, bCmp) == 0) { val = std::stof(attVal); }