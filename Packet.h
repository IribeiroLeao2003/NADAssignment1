#ifndef PACKET_H
#define PACKET 
#include <string>
using namespace std;

class Packet {
	string header;
	string payload;
	// assuming we are just placing in string and parsing for values later 

	// packet header 
	// packet payload 
public:
	Packet(string sentHeader, string sentpayLoad) : header{ sentHeader }, payload{ sentpayLoad } { }// constructor  


	//method to print packet contents (in case we needed it)
	void PrintPacket() {
		printf("This packets header: %s\n\t This packets payload %s\n", header.c_str(), payload.c_str());
	}

	// get methods 
	string getHeader() const { return header; }
	string getPayload() const { return payload; } 

	//set methods 
	string setPacketHeader(string newHeader) { newHeader = header; }
	string setPacketPayload(string newPayload) { newPayload = payload; }


	// writing/reading methods (or use the files you created) 
};



#endif // !PACKET_H
