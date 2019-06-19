#pragma once

#include <istream>

#define GETFILEDATA(dataBuffer, file) { std::istream* stream = file->GetDecompressionStream(); std::size_t len = file->GetSize(); dataBuffer = new char[len]; stream->read(dataBuffer, len); file->CloseDecompressionStream(); }