#include "FilesSystem.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Debug/Log.h"

namespace Engine {
    bool FilesSystem::Exist(const char* pPath) {
	    const std::ifstream file(pPath);
        return file.good();
    }

    bool FilesSystem::TryOpen(const char* pPath, const FileModes pModes, const bool pInBinary, File* pOutFile) {
        pOutFile->Handle = nullptr;
        pOutFile->IsValid = false;

        std::ios_base::openmode mode = std::ios_base::in;

        if ((pModes & FileModeWrite) != 0) {
            mode |= std::ios_base::out;
        }

        if (pInBinary) {
            mode |= std::ios_base::binary;
        }

        auto* file = new std::fstream(pPath, mode);

        if (!file->is_open()) {
            delete file;
            CORE_ERROR("[FilesSystem] Error opening file: '%s'", pPath);
            return false;
        }

        pOutFile->Handle = file;
        pOutFile->IsValid = true;

        return true;
    }

    void FilesSystem::Close(File* pFile) {
        if (!pFile->Handle) {
            return;
        }

        auto* file = static_cast<std::fstream*>(pFile->Handle);
        file->close();
        delete file;

        pFile->Handle = nullptr;
        pFile->IsValid = false;
    }

    bool FilesSystem::TryReadLine(const File* pFile, std::string& pLine) {
        if (!pFile->Handle) {
            return false;
        }

        std::getline(*static_cast<std::ifstream*>(pFile->Handle), pLine);
        return true;
    }

    bool FilesSystem::TryWriteLine(const File* pFile, const std::string& pText) {
        if (!pFile->Handle) {
            return false;
        }

        *static_cast<std::ofstream*>(pFile->Handle) << pText << '\n';
        return true;
    }

    bool FilesSystem::TryRead(const File* pFile, const uint64_t pDataSize, void* pOutData, uint64_t* pOutBytesRead) {
        if (!pFile->Handle || !pOutData) {
            return false;
        }

        auto* file = static_cast<std::ifstream*>(pFile->Handle);
        file->read(static_cast<char*>(pOutData), pDataSize);
        *pOutBytesRead = file->gcount();

        return *pOutBytesRead == pDataSize;
    }

    bool FilesSystem::TryReadAllBytes(const File* pFile, char** pOutBytes, uint64_t* pOutBytesRead) {
        if (!pFile->Handle) {
            return false;
        }

        auto* file = static_cast<std::ifstream*>(pFile->Handle);
        file->seekg(0, std::ios::end);
        const uint64_t size = file->tellg();
        file->seekg(0, std::ios::beg);

        *pOutBytes = new char[size];
        file->read(*pOutBytes, size);
        *pOutBytesRead = file->gcount();

        return *pOutBytesRead == size;
    }

    bool FilesSystem::TryWrite(const File* pFile, const uint64_t pDataSize, const void* pData, uint64_t* pOutBytesWritten) {
        if (!pFile->Handle) {
            return false;
        }

        auto* file = static_cast<std::ofstream*>(pFile->Handle);
        file->write(static_cast<const char*>(pData), pDataSize);
        *pOutBytesWritten = pDataSize;

        return true;
    }
}
