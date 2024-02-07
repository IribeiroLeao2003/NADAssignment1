/*
 * File: packetReader.cpp
 * Project: SENG 2040 A1
 * Programmer: Vivian Morton & Isaac
 * First Version: 23/01/2024 (dd/mm/yyyy)
 * Purpose: This file is responsible for the logic of reading the packet data
 */


#include "FileTransfer.h"
/*
* FUNCTION    : deserializeData()
* DESCRIPTION : This function takes a serialized data array of char and
              : deserializes the int and char from it
* PARAMETERS  : unsigned char serData[] - the serialized data array
*			  : char* packetType the type of packet that is being deserialized
*			  : int32_t& intData - variable containing reference to the int data to be extracted 
*             : char chrData[] - the char array to be filled with data
* RETURNS     : the number of bytes read from the serialized data or kError in case of failure
*/
int32_t deserializeData(unsigned char serData[], char* packetType, int32_t* intData, char chrData[]) {
	if (serData == nullptr || chrData == nullptr) {
		return kError;
	}

	// Start at the beginning of the serialized data
	unsigned char* pDataPoint = serData;

	// Deserialize the char
	memcpy(packetType, pDataPoint, sizeof(char)); //copy the char data from the serData to packetType
	pDataPoint += sizeof(char); // Move the pointer past the char data

	// Deserialize the int32_t
	memcpy(intData, pDataPoint, sizeof(int32_t)); //copy the int data from the serData to dataInt
	pDataPoint += sizeof(int32_t); // Move the pointer past the int data

	// Deserialize the char data
	memcpy(chrData, pDataPoint, kPayloadSize);

	// Return the total number of bytes deserialized
	return sizeof(int32_t) + kPayloadSize;
}

/*
* FUNCTION    : deserializeData64()
* DESCRIPTION : This function takes a serialized data array of char and
			  : deserializes the int and char from it
* PARAMETERS  : unsigned char serData[] - the serialized data array
*			  : char* packetType the type of packet that is being deserialized
*			  : int64_t& intData - variable containing reference to the int data to be extracted
*             : char chrData[] - the char array to be filled with data
* RETURNS     : the number of bytes read from the serialized data or kError in case of failure
*/
int32_t deserializeData64(unsigned char serData[], char* packetType, int64_t* intData, char chrData[]) {
	if (serData == nullptr || chrData == nullptr) {
		return kError;
	}

	// Start at the beginning of the serialized data
	unsigned char* pDataPoint = serData;

	// Deserialize the char
	memcpy(packetType, pDataPoint, sizeof(char)); //copy the char data from the serData to packetType
	pDataPoint += sizeof(char); // Move the pointer past the char data

	// Deserialize the int64_t
	memcpy(intData, pDataPoint, sizeof(int64_t)); //copy the int data from the serData to dataInt
	pDataPoint += sizeof(int64_t); // Move the pointer past the int data

	// Deserialize the char data
	memcpy(chrData, pDataPoint, kFileNameSize);

	// Return the total number of bytes deserialized
	return sizeof(int64_t) + kFileNameSize;
}


/*
* FUNCTION    : fileWriter()
* DESCRIPTION : This function takes a file and reads one chunk of it
* PARAMETERS  : ifstream* file - a pointer to the file to read
* RETURNS     : int with the status of end of file
*/
int32_t fileWriter(ofstream* file, char buffer[])
{
	if (file->is_open() == true)
	{
		
		//memset(buffer, 0, 256); //this allows us to copy non text files!

		file->write(buffer, 256); //write the buffer to file


		cout << "file written\n";
		 
	}
	return kSuccess; // putting this here since without it the program would not compile but we can change it later
}
