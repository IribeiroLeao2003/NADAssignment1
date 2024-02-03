/*
 * File: Packets.h
 * Project: SENG 2040 A1
 * Programmer: Vivian Morton & Isaac
 * First Version: 23/01/2024 (dd/mm/yyyy)
 * Purpose: This file contains all the packet class types used in this assignment.
 */


#ifndef PACKET_H
#define PACKET 
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

const int32_t kPayloadSize = 252; //256 - 4 for ints we include
const int32_t kIntSize = 4; //4 bytes
const int32_t kError = -1;
const int32_t kEndOfFile = 1;
const int32_t kNotEndOfFile = -1;
const int32_t kTrue = 1;
const int32_t kFalse = 0;
const int32_t kSuccess = 1;
const int32_t kFailure = -1;
const int32_t kBufferSize = 1024; //setting it up as a test, we can change the number as much as nessesary 

int32_t fileSizeReader(ifstream* file);
string generateChecksum(const string& fileName);
int32_t fileReader(ifstream* file, unsigned char buffer[]);
int32_t serializeData(int32_t intData, char charData[], unsigned char serializedData[]);
string fileNameExtractor(string filePath);

/*
* Name: FilePacket
* This packet contains the payload of the filebuffer data we are sending.
*/
class FilePacket {

public:
	int32_t endFile = kFalse;
	char payload[kPayloadSize];
};


/*
* Name: FilePacket
* This packet contains the payload of the checksum data we are sending.
*/
class ChecksumPacket {

public:
	int32_t endChecksum = kFalse;
	char checksum[kPayloadSize];
};


/*
* Name: FilePacket
* This packet contains the file name, file size, and checksum of the file we are sending. It will be send first.
*/
class FileInfoPacket {

public:
	int32_t fileSize;
	string fileName;

	FileInfoPacket(string nameInput, int sizeInput)
	{
		fileName = nameInput;
		fileSize = sizeInput;
	}
};


#endif // !PACKET_H
