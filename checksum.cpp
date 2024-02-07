//https://github.com/d-bahr/CRCpp

#include "FileTransfer.h"
#include "CRC.h"
#include <iostream>
#include <vector>

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
* FUNCTION    : generateChecksum()
* DESCRIPTION : This function takes an const string containing the file name, it opens the file and generates a checksum with the help of the CRC library 
* PARAMETERS  : const string& fileName - Name of the file to be open 
*			 
* RETURNS     : string checksumStr on success and "" on failure
*/
void generateChecksum(const string& fileName, char checksumStr[kPayloadSize], ifstream* userFile)
{
	
	vector<char> buffer(kBufferSize); // creating file buffer

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
	while (userFile->read(buffer.data(), kBufferSize)) { 
		//for each line read into the buffer, crc is updated 
		// after the loop is finished, the final checksum value is hold inside the crc 
		crc = CRC::Calculate(buffer.data(), userFile->gcount(), crcTable, crc); 
	}
	userFile->seekg(0, userFile->beg); //go back to beginning of file
	hextoCharArray(crc, checksumStr); // string that should contain the final checksum string, passing the crc inside of the hextoString function 
									
}