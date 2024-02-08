/*
 * File: packetWriter.cpp
 * Project: SENG 2040 A1
 * Programmer: Vivian Morton & Isaac
 * First Version: 23/01/2024 (dd/mm/yyyy)
 * Purpose: This file contains the logic for sending packets and preparing the data for them.
 */

#include "FileTransfer.h"

/*
* FUNCTION    : fileSizeReader()
* DESCRIPTION : This function takes a file and checks the size
* PARAMETERS  : ifstream* file - a pointer to the file to check the size of
* RETURNS     : int fileSize
*/
int32_t fileSizeReader(ifstream* file)
{
	bool error = false;
	int32_t fileSize = 0; //filesize in bytes

	file->seekg(0, file->end);  //move to end of file to get size
	fileSize = file->tellg(); //get file size
	file->seekg(0, file->beg); //go back to beginning of file

	return fileSize;
	
}



/*
* FUNCTION    : fileSizeReader()
* DESCRIPTION : This function takes a file and reads one chunk of it
* PARAMETERS  : ifstream* file - a pointer to the file to read
* RETURNS     : int with the status of end of file
*/
int32_t fileReader(ifstream* file, char buffer[])
{
	if (!file->eof()) //while not at end of file
	{
		file->read(buffer, kPayloadSize); //read the buffer
		return kPayloadSize;
	}
	else
	{
		return kEndOfFile;
	}
}


/*
* FUNCTION    : serializeData()
* DESCRIPTION : This function takes an int32_t, char array, and unsigned char array 
			  : and serializes the int and char array and saves it to the unsigned char array
* PARAMETERS  : int32_t intData - the int data to save
*			  : char charData[] - the char data to save
*			  : unsigned char serializedData[] - the array to save the serialized data to
* RETURNS     : int32_t kSuccess on success and kFailure on failure
*/
int32_t serializeData(char packetType, int32_t intData, char charData[], unsigned char serializedData[])
{
	if (sizeof(intData) + sizeof(charData) < kPayloadSize) //make sure our data isn't too big to fit
	{
		//mark our location
		unsigned char* pDataPoint = serializedData;

		//copy char
		memcpy(pDataPoint, &packetType, sizeof(char));
		//move pointer ahead
		pDataPoint += sizeof(char);

		//copy int
		memcpy(pDataPoint, &intData, sizeof(int32_t));
		//move pointer ahead
		pDataPoint += sizeof(int32_t);

		//copy charData
		memcpy(pDataPoint, charData, kPayloadSize);
		return kSuccess;
	}
	else
	{
		return kFailure;
	}


}

/*
* FUNCTION    : corruptData()
* DESCRIPTION : This function takes a char, int64_t, char array, and unsigned char array
			  : and serializes the int and char array and saves it to the unsigned char array
* PARAMETERS  : char* data - the data to be corrupted
*			  : size_t dataSize - the size of that data
*			  :
* RETURNS     : Nothinig
*/

void corruptData(char* data, size_t dataSize) {
	srand(time(NULL));

	// choose a random position to corrupt
	int positionToCorrupt = rand() % dataSize;

	//inverting the bits
	data[positionToCorrupt] = ~data[positionToCorrupt];

}


/*
* FUNCTION    : serializeData64()
* DESCRIPTION : This function takes a char, int64_t, char array, and unsigned char array
			  : and serializes the int and char array and saves it to the unsigned char array
* PARAMETERS  : char packetType - the type of packet this is
*			  : int64_t intData - the int data to save
*			  : char charData[] - the char data to save
*			  : unsigned char serializedData[] - the array to save the serialized data to
* RETURNS     : int32_t kSuccess on success and kFailure on failure
*/
int32_t serializeData64(char packetType, int64_t intData, char charData[], unsigned char serializedData[])
{
	if (sizeof(intData) + sizeof(charData) < kPayloadSize) //make sure our data isn't too big to fit
	{
		//mark our location
		unsigned char* pDataPoint = serializedData;

		//copy char
		memcpy(pDataPoint, &packetType, sizeof(char));
		//move pointer ahead
		pDataPoint += sizeof(char);

		//copy int64
		memcpy(pDataPoint, &intData, sizeof(int64_t));
		//move pointer ahead
		pDataPoint += sizeof(int64_t);

		//copy charData
		memcpy(pDataPoint, charData, kFileNameSize);
		return kSuccess;
	}
	else
	{
		return kFailure;
	}


}