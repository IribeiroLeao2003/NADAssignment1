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
#include <chrono>
#include <cmath>
using namespace std;
using namespace chrono;

//Packet sizing
const int32_t kPayloadSize = 251; //256 - 4 for ints we include, - 1 for the char type
const int32_t kFileNameSize = 247; //256 - 8 for the long and -1 for char type
const int32_t kEndOfFile = 0;

//Error and success
const int32_t kError = -1;
const int32_t kSuccess = 1;
const int32_t kFailure = -1;

//checkum constants
const int32_t kChecksumGenBufferSize = 1024; //used for checksum generation. NOT FOR PACKETS

//packet types
const char kFileInfoPacket = 1;
const char kChecksumPacket = 2;
const char kFileDataPacket = 3;



const int32_t Path_Max = 260;

//serialization
int32_t serializeData(char packetType, int32_t intData, char charData[], unsigned char serializedData[]);
int32_t serializeData64(char packetType, int64_t intData, char charData[], unsigned char serializedData[]);
int32_t deserializeData(unsigned char serData[], char* packetType, int32_t* dataInt, char dataCh[]);
int32_t deserializeData64(unsigned char serData[], char* packetType, int64_t* intData, char chrData[]);

//File related
int32_t fileSizeReader(ifstream* file);
int32_t fileReader(ifstream* file, char buffer[]);
int32_t fileWriter(ofstream* file, char buffer[]);
string fileNameExtractor(string filePath);

//checksum related
void generateChecksum(const string& fileName, char checksumStr[kPayloadSize], ifstream* userFile);
void corruptData(char data[], int32_t dataSize);
void generateChecksum2(const string& fileName, char checksumStr[kPayloadSize]);



//misc
int64_t getTime();

//const int32_t kNotEndOfFile = -1;
//const int32_t kTrue = 1;
//const int32_t kFalse = 0;
//const int32_t kIntSize = 4; //4 bytes

#endif // !PACKET_H
