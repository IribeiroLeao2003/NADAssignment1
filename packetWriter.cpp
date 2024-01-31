/*
 * File: packetWriter.cpp
 * Project: SENG 2040 A1
 * Programmer: Vivian Morton & Isaac
 * First Version: 23/01/2024 (dd/mm/yyyy)
 * Purpose: This file contains the logic for sending packets and preparing the data for them.
 */

#include "Packets.h"

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
int32_t fileReader(ifstream* file, unsigned char buffer[])
{
	if (!file->eof()) //while not at end of file
	{
		file->read((char*)buffer, kPayloadSize); //read the buffer
		return kNotEndOfFile;
	}
	else
	{
		return kEndOfFile;
	}
}