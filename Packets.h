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
#include <windows.data.json.h>
#include <iostream>
#include <fstream>
using namespace std;

const int payloadSize = 255; //arbitrary we can change it later

/*
* Name: FilePacket
* This packet contains the payload of the filebuffer data we are sending.
*/
class FilePacket {

public:
	char Payload[payloadSize];
};


/*
* Name: FilePacket
* This packet contains the file name, file size, and checksum of the file we are sending. It will be send first.
*/
class FileInfoPacket {
	string FileName;
	string FileSize;
	string CheckSum;
};


#endif // !PACKET_H
