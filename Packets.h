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

const int kPayloadSize = 256; //arbitrary we can change it later
const int kIntSize = 4; //4 bytes
const int kError = -1;
const int kEndOfFile = 1;
const int kNotEndOfFile = -1;

int fileSizeReader(ifstream* file);
string generateChecksum(string fileName);
int fileReader(ifstream* file, unsigned char buffer[]);

/*
* Name: FilePacket
* This packet contains the payload of the filebuffer data we are sending.
*/
class FilePacket {

public:
	bool endFile = false;
	char payload[kPayloadSize];
};


/*
* Name: FilePacket
* This packet contains the payload of the checksum data we are sending.
*/
class ChecksumPacket {

public:
	bool endChecksum = false;
	char checksum[kPayloadSize];
};


/*
* Name: FilePacket
* This packet contains the file name, file size, and checksum of the file we are sending. It will be send first.
*/
class FileInfoPacket {

public:
	string fileName;
	int fileSize;

	FileInfoPacket(string nameInput, int sizeInput)
	{
		fileName = nameInput;
		fileSize = sizeInput;
	}
};


#endif // !PACKET_H
