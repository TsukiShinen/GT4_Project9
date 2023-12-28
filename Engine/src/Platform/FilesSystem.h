#pragma once
#include <cvt/wstring>

namespace Engine
{
	struct File
	{
		void* Handle;
		bool IsValid;
	};

	enum FileModes
	{
		FileModeRead = 0x1,
		FileModeWrite = 0x2
	};

	class FilesSystem
	{
	public:
		/**
		 * \brief Checks if a file with the given path exist.
		 * \param pPath The path of the file to be checked.
		 * \return True if exist; otherwise false.
		 */
		static bool Exist(const char* pPath);

		/**
		 * \brief Attempt to open file located at path.
		 * \param pPath The path of the file to be checked.
		 * \param pModes Mode flags for the file when opened (read/write).
		 * \param pInBinary Indicate if the file should be opened in binary mode.
		 * \param pOutFile A pointer to a File struct which holds the handle information.
		 * \return True if opened successfully; otherwise false.
		 */
		static bool TryOpen(const char* pPath, FileModes pModes, bool pInBinary, File* pOutFile);

		/**
		 * \brief Close the provided handle to a file.
		 * \param pFile A pointer to a File struct which holds the handle to be closed.
		 */
		static void Close(File* pFile);

		/**
		 * \brief Reads up to a newline or EOF.
		 * \param pFile A pointer to a File struct.
		 * \param pLine A reference to a string which will be populated by this method.
		 * \return True if opened successfully; otherwise false.
		 */
		static bool TryReadLine(const File* pFile, std::string& pLine);

		/**
		 * \brief Writes text to the provided file, appending a '\n' afterward.
		 * \param pFile A pointer to a File struct.
		 * \param pText The text to be written.
		 * \return True if opened successfully; otherwise false.
		 */
		static bool TryWriteLine(const File* pFile, const std::string& pText);

		/**
		 * \brief Reads up to pDataSize bytes of data into pBytesRead.
		 * Allocate *pOutData, which must be freed by the caller.
		 * \param pFile A pointer to a File struct.
		 * \param pDataSize The number of bytes to read.
		 * \param pOutData A pointer to a block of memory to be populated by this method.
		 * \param pOutBytesRead A pointer to a number which will be populated with the number of bytes actually read from the file.
		 * \return True if opened successfully; otherwise false.
		 */
		static bool TryRead(const File* pFile, uint64_t pDataSize, void* pOutData, uint64_t* pOutBytesRead);

		/**
		 * \brief Reads all the bytes into pOutBytes.
		 * Allocate *pOutBytes, which must be freed by the caller.
		 * \param pFile A pointer to a File struct.
		 * \param pOutBytes A pointer to a byte array which will be allocated and populated by this method.
		 * \param pOutBytesRead A pointer to a number which will be populated with the number of bytes actually read from the file.
		 * \return True if opened successfully; otherwise false.
		 */
		static bool TryReadAllBytes(const File* pFile, char** pOutBytes, uint64_t* pOutBytesRead);

		/**
		 * \brief Write the provided data to the file.
		 * \param pFile A pointer to a File struct.
		 * \param pDataSize The size of the data in bytes.
		 * \param pData The data to be written.
		 * \param pOutBytesWritten A pointer to a number which will be populated with the number of bytes actually written to the file.
		 * \return True if opened successfully; otherwise false.
		 */
		static bool TryWrite(const File* pFile, uint64_t pDataSize, const void* pData, uint64_t* pOutBytesWritten);
	};
}
