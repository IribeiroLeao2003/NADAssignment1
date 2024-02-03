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
* PARAMETERS  : const unsigned char serData[] - the serialized data array
*			  : int32_t& dataInt - variable containing reference to the int data to be extracted 
*             : char dataCh[] - the char array to be filled with data
*             : size_t dataChSize - the size of the dataCh array
* RETURNS     : the number of bytes read from the serialized data or kFailure in case of 
*/
int32_t deserializeData(const unsigned char serData[], int32_t& dataInt, char dataCh[], size_t dataChSize) {
    if (serData == nullptr || dataCh == nullptr) {
        return kFailure;
    }

    // Start at the beginning of the serialized data
    const unsigned char* pDataPoint = serData;

    // Deserialize the int32_t
    memcpy(&dataInt, pDataPoint, sizeof(int32_t));
    pDataPoint += sizeof(int32_t); // Move the pointer past the int data

    // Calculate the size of charData we expect to deserialize
    size_t expectedCharDataSize = dataChSize;

    // Deserialize the char data
    memcpy(dataCh, pDataPoint, expectedCharDataSize);

    // Return the total number of bytes deserialized
    return sizeof(int32_t) + expectedCharDataSize;
}