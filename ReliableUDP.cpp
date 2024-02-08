/*
	Reliability and Flow Control Example
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Net.h"
#include "FileTransfer.h"

//#define SHOW_ACKS

using namespace std;
using namespace net;

const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x11223344;
const float DeltaTime = 1.0f / 30.0f;
const float SendRate = 1.0f / 30.0f;
const float TimeOut = 10.0f;
const int PacketSize = 256;
const int messageSize = 100;
const int convertSize = 32;

class FlowControl
{
public:

	FlowControl()
	{
		printf("flow control initialized\n");
		Reset();
	}

	void Reset()
	{
		mode = Bad;
		penalty_time = 4.0f;
		good_conditions_time = 0.0f;
		penalty_reduction_accumulator = 0.0f;
	}

	void Update(float deltaTime, float rtt)
	{
		const float RTT_Threshold = 250.0f;

		if (mode == Good)
		{
			if (rtt > RTT_Threshold)
			{
				printf("*** dropping to bad mode ***\n");
				mode = Bad;
				if (good_conditions_time < 10.0f && penalty_time < 60.0f)
				{
					penalty_time *= 2.0f;
					if (penalty_time > 60.0f)
						penalty_time = 60.0f;
					printf("penalty time increased to %.1f\n", penalty_time);
				}
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				return;
			}

			good_conditions_time += deltaTime;
			penalty_reduction_accumulator += deltaTime;

			if (penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f)
			{
				penalty_time /= 2.0f;
				if (penalty_time < 1.0f)
					penalty_time = 1.0f;
				printf("penalty time reduced to %.1f\n", penalty_time);
				penalty_reduction_accumulator = 0.0f;
			}
		}

		if (mode == Bad)
		{
			if (rtt <= RTT_Threshold)
				good_conditions_time += deltaTime;
			else
				good_conditions_time = 0.0f;

			if (good_conditions_time > penalty_time)
			{
				printf("*** upgrading to good mode ***\n");
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				mode = Good;
				return;
			}
		}
	}

	float GetSendRate()
	{
		return mode == Good ? 30.0f : 10.0f;
	}

private:

	enum Mode
	{
		Good,
		Bad
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;
};

// ----------------------------------------------

int main(int argc, char* argv[])
{
	// client vars
	bool sendFile = false;
	bool checksumSend = false;
	bool fileInfoSend = true;
	string filePath = "";
	ifstream inputFile;
	int32_t fileSize = 0;
	double numOfReads = 0;
	int currentNumOfReads = 0;
	int32_t finalBytes = 0;

	//server vars
	bool receivedFileInfo = false;
	bool receivedChecksum = false;
	ofstream outputFile; // used to write into file 


	//client and server vars
	string fileName = "";
	int32_t finalFileSize = 0;
	int32_t currentFileSize = 0;


	bool isFileClosed = false;


	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;

	//Run as client if argc >= 2
	if (argc >= 2)
	{
		int a, b, c, d;
		
		#pragma warning(suppress : 4996)
		if (sscanf(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d))
		{
			mode = Client;
			address = Address(a, b, c, d, ServerPort);
		}

		//check for file name in argv[2]
		if (argc == 3)
		{
			filePath = argv[2]; //copy name to string
			fileName = fileNameExtractor(filePath); //ensure we just have the file name
			if (fileName.length() <= kFileNameSize)
			{
				sendFile = true; //we're sending a file
				inputFile.open(filePath, ifstream::binary); //open file
				if (inputFile.is_open() == false)
				{
					printf("File could not be opened\n");
					sendFile = false;
				}
			}
			else
			{
				printf("File name is too long.\n");
			}

		}
	}

	// initialize

	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return 1;
	}

	ReliableConnection connection(ProtocolId, TimeOut);

	const int port = mode == Server ? ServerPort : ClientPort;

	if (!connection.Start(port))
	{
		printf("could not start connection on port %d\n", port);
		return 1;
	}

	if (mode == Client)
		connection.Connect(address);
	else
		connection.Listen();

	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	FlowControl flowControl;
	unsigned int packetCounter = 0;

	while (true)
	{
		// update flow control

		if (connection.IsConnected())
			flowControl.Update(DeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if (mode == Server && connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf("reset flow control\n");
			connected = false;
		}

		if (!connected && connection.IsConnected())
		{
			printf("client connected to server\n");
			connected = true;
		}

		if (!connected && connection.ConnectFailed())
		{
			printf("connection failed\n");
			break;
		}

		// send and receive packets

		sendAccumulator += DeltaTime;

		//First packet sent will be the file info packet. We will track when our first packet was sent with a bool
		//All subsequent packets will contain the file data.
		
		while (sendAccumulator > 1.0f / sendRate)
		{	
			unsigned char packet[PacketSize];
			memset(packet, 0, sizeof(packet));

			if (connected == true)
			{			
				char packetType = 0;

				if (sendFile) //we have a file to send
				{
					if (inputFile.is_open() == true)
					{
						if (fileInfoSend) //we haven't sent the first chunk of data yet
						{
							//send our file info
							packetType = kFileInfoPacket;

							char fileNameChar[kFileNameSize] = { "\0" };
							#pragma warning(disable:4996);
							strcpy(fileNameChar, fileName.c_str());

							int64_t currentTime = getTime();

							serializeData64(packetType, currentTime, fileNameChar, packet); //serialize the data
							//done our first send
							checksumSend = true;
							fileInfoSend = false;
						}
						else if (checksumSend)//first send has been done. Now we do file data
						{
							checksumSend = false; // No longer in checksum 
							char fileChecksum[kPayloadSize];

							
							fileSize = fileSizeReader(&inputFile);
							generateChecksum(fileName, fileChecksum, &inputFile); // Generate checksum
							serializeData(kChecksumPacket, fileSize, fileChecksum, packet); // Sending checksum packet

							// preparing for data transmission
							double totalPackets = ceil(static_cast<double>(fileSize) / kPayloadSize);
							int currentPacket = 0;

							// reopen file to reset pointer
							inputFile.close();
							inputFile.open(filePath, std::ifstream::binary);

							
							while (currentPacket < totalPackets) {
								char buffer[kPayloadSize] = { '\0' };		// creating file buffer
								int32_t readSize = kPayloadSize;

								// Adjusting read size for last packet
								if (currentPacket == totalPackets - 1) {
									int lastPacketSize = fileSize % kPayloadSize;
									if (lastPacketSize > 0) {
										readSize = lastPacketSize;
									}
								}

								// Read file data into buffer
								inputFile.read(buffer, readSize);
								if (!inputFile.good() && !inputFile.eof()) {
									printf("Error during data transmission\n");
									
									break; // out of the while loop
								}

								// serialize the data and send the packet
								serializeData(kFileDataPacket, readSize, buffer, packet);
								currentPacket++;
							}

							
							if (fileSize % kPayloadSize == 0) { // If remainder exists then send EOF indicator in form of packet
								char buffer[kPayloadSize] = { 0 }; // Create an empty buffer 
								int eofIndicatorSize = 0; // create payload to indicate EOF (can change later if needed) 
								serializeData(kEndOfFile, eofIndicatorSize, buffer, packet); //Serializing Data
							}

							inputFile.close(); // close File
						}
						else //otherwise send file data
						{
							packetType = kFileDataPacket;
							char fileBuffer[kPayloadSize] = { '\0' }; //the file data buffer						


							currentNumOfReads++; //increment number of reads
							int32_t dataSize = fileReader(&inputFile, fileBuffer); //read the data
							
							if (currentNumOfReads >= numOfReads)
							{
								dataSize = finalBytes; //this array will be smaller because it's our last
							}


							if (dataSize != kEndOfFile) //check if end of file
							{
								serializeData(packetType, dataSize, fileBuffer, packet); //serialize the data and send it							
							}
							else //end of file close it
							{
								char emptyBuffer[kPayloadSize] = { '\0' }; //no data to send so just send an empty array
								serializeData(packetType, dataSize, fileBuffer, packet); //serialize the data and send it
								sendFile = false;
								inputFile.close();//close file
							}
						}

					}
					else
					{
						printf("File could not be read.\n");
						sendFile = false;
					}

				}
			}
			
			
			connection.SendPacket(packet, sizeof(packet));
			sendAccumulator -= 1.0f / sendRate;
		}


		//Receive packets here. Also tracked will the same bool but determined when a packet is 
		while (true)
		{
			//check if the received packet is file info. If it is, switch to file reading mode
			//When in file reading mode, check if packet contains file data or packet to say we are done.
			// If the packet contains data send the data to be read and written to the disk
			//When done reading file verify checksum and then swtich back to default mode.
			unsigned char packet[PacketSize];
			char packetType = 0;
			int bytes_read = connection.ReceivePacket(packet, sizeof(packet));

			char receivedChecksumValue[kPayloadSize + 1];
			if (bytes_read == 0) //empty packet
			{
				break; //don't touch this is from original code
			}
			else //data in packet
			{
				int32_t intData = 0;
				int64_t timeData = 0;
				char charData[kPayloadSize] = { '\0' };
				//first check for file name
				if (packet[0] != kFileInfoPacket)
				{
					deserializeData(packet, &packetType, &intData, charData);
				}
				else
				{
					deserializeData64(packet, &packetType, &timeData, charData);
				}				

				if (packetType == kFileInfoPacket) //check if we have not started receiving a file
				{
					receivedFileInfo = true; //we have now received a file

					fileName = charData; //copy the file name over

					printf("%s", fileName.c_str());

					finalFileSize = intData; //store the file size

				}
				else if (packetType == kChecksumPacket)
				{
					memcpy(receivedChecksumValue, charData, kPayloadSize);
					receivedChecksumValue[kPayloadSize] = '\0';
					receivedChecksum = true;
				}
				else if ((packetType == kFileDataPacket) && !isFileClosed)//otherwise we're reciving file data
				{

					if (intData < kEndOfFile){
						printf("\nno data to print\n");
					}
					else {
						printf("\nthere is data to print\n");
					}
					//create file with the given name
					if (!outputFile.is_open())
					{
						// Open the file for writing while ensuring directory exists and I have writting permissions 
						outputFile.open(fileName, std::ofstream::binary);
						if (!outputFile)
						{

							printf("File could not be opened for writing %s\n", fileName.c_str());

						}
					}
					if (intData > 0) { // If there's data to write in the output file 
						
						/*currentFileSize += intData;*/

						//if (currentFileSize > finalFileSize)
						//{
						//	int32_t subtractEnd = currentFileSize - finalFileSize; //get difference of the useless data
						//	intData -= subtractEnd; //we only want to write the good data
						//}
						outputFile.write(charData, intData);
						
					}
					else if (intData == kEndOfFile || currentFileSize >= finalFileSize) { // Check if end of file was reached 
						outputFile.close();
						isFileClosed = true;
						printf("File Transfer Done\n");

						//handle final data
					}

					
				}


			}
				
		}

		// show packets that were acked this frame

#ifdef SHOW_ACKS
		unsigned int* acks = NULL;
		int ack_count = 0;
		connection.GetReliabilitySystem().GetAcks(&acks, ack_count);
		if (ack_count > 0)
		{
			printf("acks: %d", acks[0]);
			for (int i = 1; i < ack_count; ++i)
				printf(",%d", acks[i]);
			printf("\n");
		}
#endif

		// update connection

		connection.Update(DeltaTime);

		// show connection stats

		statsAccumulator += DeltaTime;

		while (statsAccumulator >= 0.25f && connection.IsConnected())
		{
			float rtt = connection.GetReliabilitySystem().GetRoundTripTime();

			unsigned int sent_packets = connection.GetReliabilitySystem().GetSentPackets();
			unsigned int acked_packets = connection.GetReliabilitySystem().GetAckedPackets();
			unsigned int lost_packets = connection.GetReliabilitySystem().GetLostPackets();

			float sent_bandwidth = connection.GetReliabilitySystem().GetSentBandwidth();
			float acked_bandwidth = connection.GetReliabilitySystem().GetAckedBandwidth();

			printf("rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n",
				rtt * 1000.0f, sent_packets, acked_packets, lost_packets,
				sent_packets > 0.0f ? (float)lost_packets / (float)sent_packets * 100.0f : 0.0f,
				sent_bandwidth, acked_bandwidth);

			statsAccumulator -= 0.25f;
		}

		net::wait(DeltaTime);
	}

	ShutdownSockets();

	return 0;
}

