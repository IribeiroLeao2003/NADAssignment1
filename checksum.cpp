//https://github.com/d-bahr/CRCpp

#include "FileTransfer.h"
#include "CRC.h"
#include <iostream>
#include <vector>

#include <limits.h>
#include <windows.h>

/*
* FUNCTION    : hextoString()
* DESCRIPTION : This function takes an uint32_t and turns it into a hexadecimal string. 
*			  : The intention for this function is to support generateChecksum in creating a hexadecimal string which will be the checksum
*
* PARAMETERS  : uint32_t number - number to be turned into a hexadecimal string 
*			  
* RETURNS     : char hexaStr that contains the hexadecimal string
*/
void hextoCharArray(uint32_t number, char hexaStr[kPayloadSize]) {
	const char* hexCode = "0123456789ABCDEF"; // All hex digits
	hexaStr[8] = '\0'; // Set the null terminator on spot 8

	for (int i = 7; i >= 0; --i) {
		hexaStr[i] = hexCode[number & 0xF]; // Get the equivalent hex digit and fill it in the hexaStr
		number >>= 4; // Go or shift to the next hex digit
	}
}








/*
* FUNCTION    : getExecutablePath()
* DESCRIPTION : This function returns the current file path for the .exe running to make sure there are no conflicts with the checksum calculation
* PARAMETERS  : NONE
*			  :
*
* RETURNS     : "" if null (return path.substr(0, lastSlashPos) which is the current directory of the exe
*/
string getExecutablePath() {
	//create buffer with MAX_PATH (variable from windows library)
	char buffer[MAX_PATH];
	size_t copied = GetModuleFileNameA(NULL, buffer, MAX_PATH);
	if (copied > 0 && copied < MAX_PATH) {
		string currentpath(buffer, copied);
		size_t lastSlashPos = currentpath.find_last_of("\\/");
		if (lastSlashPos != string::npos) {
			return currentpath.substr(0, lastSlashPos); // Return the directory, excluding the executable name
		}
	}
	return ""; 
}


/*
* FUNCTION    : generateChecksum2()
* DESCRIPTION : This function takes an const string containing the file name, it opens the file and generates a checksum with the help of the CRC library
* PARAMETERS  : const string& fileName - Name of the file
*			  : char checksumStr[kPayloadSize] - buffer to store checksum
*			  :
*
* RETURNS     : simply update checksumStr with the
*/
void generateChecksum2(const string& fileName, char checksumStr[kPayloadSize]) {
	string executableDir = getExecutablePath();
	printf("\n%s\n", executableDir.c_str());
	string filePath = executableDir + "\\" + fileName; // Construct the full file path
	printf("\n%s\n", filePath.c_str());

	ifstream inputFile(filePath, std::ifstream::binary);
	if (!inputFile.is_open()) {
		printf("Failed to open file for checksum");
		return;
	}

	vector<char> buffer(kChecksumGenBufferSize);
	CRC::Table<uint32_t, 32> crcTable = CRC::CRC_32().MakeTable();
	uint32_t crc = CRC::CRC_32().initialValue;

	while (inputFile.read(buffer.data(), buffer.size())) {
		crc = CRC::Calculate(buffer.data(), inputFile.gcount(), crcTable, crc);
	}

	
	hextoCharArray(crc, checksumStr);
	printf("Checksum for file '%s': %s\n", filePath.c_str(), checksumStr);

	
	inputFile.close();
}



/*
* FUNCTION    : generateChecksum()
* DESCRIPTION : This function takes an const string containing the file name, it opens the file and generates a checksum with the help of the CRC library 
* PARAMETERS  : const string& fileName - Name of the file 
*			  : char checksumStr[kPayloadSize] - buffer to store checksum 
*			  : ifstream* userFile - pointer to file
*			 
* RETURNS     : simply update checksumStr with the 
*/
void generateChecksum(const string& fileName, char checksumStr[kPayloadSize], ifstream* userFile)
{
	
	vector<char> buffer(kChecksumGenBufferSize); // creating file buffer

	// open the file to read binary 
	if (!userFile->is_open()) {
		cerr << "Failed to open file for checksum: " << fileName << endl;
		return;
	}

	// initialize the crc variable according to the rules of the CRC library
	CRC::Table<uint32_t, 32> crcTable = CRC::CRC_32().MakeTable(); 

	//grab the first value 
	uint32_t crc = CRC::CRC_32().initialValue;

	

	//reading up to kBufferSize in bytes
	while (userFile->read(buffer.data(), kChecksumGenBufferSize)) {
		//for each line read into the buffer, crc is updated 
		// after the loop is finished, the final checksum value is hold inside the crc 
		crc = CRC::Calculate(buffer.data(), userFile->gcount(), crcTable, crc); 
	}
	userFile->seekg(0, userFile->beg); //go back to beginning of file
	hextoCharArray(crc, checksumStr); // string that should contain the final checksum string, passing the crc inside of the hextoString function 
									
}