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
		return kNotEndOfFile;
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
int32_t serializeData(int32_t intData, char charData[], unsigned char serializedData[])
{
	if (sizeof(intData) + sizeof(charData) < kPayloadSize) //make sure our data isn't too big to fit
	{
		//mark our location
		unsigned char* pDataPoint = serializedData;

		//copy int
		memcpy(pDataPoint, &intData, sizeof(intData));
		//move pointer ahead
		pDataPoint += sizeof(intData);

		//copy charData
		memcpy(pDataPoint, charData, sizeof(charData));
		return kSuccess;
	}
	else
	{
		return kFailure;
	}


}