//https://github.com/d-bahr/CRCpp

#include "FileTransfer.h"
#include "CRC.h"



string generateChecksum(const string& fileName)
{
	unsigned char buffer[kBufferSize]; //buffer variable

	ifstream userFile(fileName, ios::binary);
	if (!userFile.is_open()) {
		cerr << "Failed to open file: " << fileName << endl;
		return "";
	}

	CRC::Table<uint32_t, 32> crcTable = CRC::CRC_32().MakeTable();
	uint32_t crc = CRC::CRC_32().initialValue;

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


	return "";
}