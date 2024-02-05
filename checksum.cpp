//https://github.com/d-bahr/CRCpp

#include "FileTransfer.h"
#include "CRC.h"

/*
* FUNCTION    : hextoString()
* DESCRIPTION : This function takes an uint32_t and turns it into a hexadecimal string. 
*			  : The intention for this function is to support generateChecksum in creating a hexadecimal string which will be the checksum
*
* PARAMETERS  : uint32_t number - number to be turned into a hexadecimal string 
*			  
* RETURNS     : char hexaStr that contains the hexadecimal string
*/
string hextoString(uint32_t number) {
	const char* hexCode = "0123456789ABCDEF"; // All hex digits
	char hexaStr[9]; // 8 hex chars + null terminator for a 32-bit number
	hexaStr[8] = '\0'; // Set the null terminator on spot 8

	for (int i = 7; i >= 0; --i) {
		hexaStr[i] = hexCode[number & 0xF]; // Get the equivalent hex digit and fill it in the hexaStr
		number >>= 4; // Go or shift to the next hex digit
	} 

	return string(hexaStr); // Convert to string and return to generateChecksum
}


/*
* FUNCTION    : generateChecksum()
* DESCRIPTION : This function takes an const string containing the file name, it opens the file and generates a checksum with the help of the CRC library 
* PARAMETERS  : const string& fileName - Name of the file to be open 
*			 
* RETURNS     : string checksumStr on success and "" on failure
*/
string generateChecksum(const string& fileName)
{
	unsigned char buffer[kBufferSize]; //buffer variable

	// open the file to read binary 
	ifstream userFile(fileName, ios::binary);
	if (!userFile.is_open()) {
		cerr << "Failed to open file for checksum: " << fileName << endl;
		return "";
	}

	// initialize the crc variable according to the rules of the CRC library
	CRC::Table<uint32_t, 32> crcTable = CRC::CRC_32().MakeTable();
	uint32_t crc = CRC::CRC_32().initialValue;


	while (!userFile.eof()) {
		userFile.read(buffer, kBufferSize); 
	}


	//crc = CRC::Finalize(crc, );


	string checksumStr; // string that should contain the final checksum string
	return checksumStr; 

	//pseudocode to serve as a guide

	//Open file binary 
	//if File isnt open 
	//			Give error message
	//			return empty string

	// Declare a buffer (unsigned char)

	//initialize crc

	// While not at the end of the file
	//		Read some data into the buffer
	//		update crc with the buffer


	// Finalize CRC (can depend on which CRC we are using)
	
	
	
	// Declare checksum as a string or char * (we will have to check on that)
	// convert src to hexadecimal and store inside checksum


	//return checksum

}