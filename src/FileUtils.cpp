#include "FileUtils.h"

char* ExtractFileFromArchive(const char* a_fileName, mz_zip_archive& a_archive, bool a_term)
{
    int index = mz_zip_reader_locate_file(&a_archive, a_fileName, "", 0);

    if (index != -1 && !mz_zip_reader_is_file_a_directory(&a_archive, index))
    {
        mz_zip_archive_file_stat fileStat;

        if (!mz_zip_reader_file_stat(&a_archive, index, &fileStat))
        {
            return nullptr;
        }

        size_t uncompressedSize;
        char* data = (char*)mz_zip_reader_extract_file_to_heap(&a_archive, a_fileName, &uncompressedSize, 0);

        // For some reason the null terminator is missing on rare occasions
        // I am not sure what causes it but this seems to fix it
        // Supprised it took months for it to start causing issues
        if (a_term)
        {
            data[uncompressedSize - 1] = 0;
        }

        if (data == nullptr)
        {
            return nullptr;
        }

        if (uncompressedSize != fileStat.m_uncomp_size)
        {
            mz_free(data);

            return nullptr;
        }

        return data;
    }

    return nullptr;
}