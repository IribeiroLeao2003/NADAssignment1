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
	bool doneFile = false;
	string filePath = "";
	ifstream inputFile;
	int32_t fileSize = 0;
	double totalPackets = 0;
	int currentPacket = 0;
	bool badMode = false;


	//server vars
	bool receivedFileInfo = false;
	bool receivedChecksum = false;
	ofstream outputFile; // used to write into file 
	int64_t startTime = 0;
	int64_t endTime = 0;

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
		if (argc >= 3)
		{
			filePath = argv[2]; //copy name to string
			fileName = fileNameExtractor(filePath); //ensure we just have the file name
			if (fileName.length() <= kFileNameSize)
			{
				sendFile = true; //we're sending a file
				inputFile.open(filePath, ifstream::binary); //open file

				// check for bad file transfer
				if (argc == 4 && string(argv[3]) == "bad") {
					badMode = true;
				}

				if (inputFile.is_open() == false)
				{
					printf("File could not be opened\n");
					sendFile = false;
					badMode = false;
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

			if (connection.IsConnected() == true && mode == Client)
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

							printf("\nSending File: %s \n\n", fileNameChar);

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
							generateChecksum("", fileChecksum, &inputFile); // Generate checksum


							serializeData(kChecksumPacket, fileSize, fileChecksum, packet); // Sending checksum packet

							// preparing for data transmission
							totalPackets = ceil(static_cast<double>(fileSize) / kPayloadSize);

							// reopen file to reset pointer
							inputFile.close();
							inputFile.open(filePath, std::ifstream::binary);
							if (!inputFile) {
								printf("\nFailed to reopen file for data transmission.\n");
								sendFile = false;
							}

						}
						else //otherwise send file data
						{
							//printf("\n Sending file data\n");
							if (currentPacket < totalPackets) {
								char buffer[kPayloadSize] = { '\0' };		// creating file buffer
								int32_t readSize = kPayloadSize;

								// Adjusting read size for last packet
								if (currentPacket == totalPackets - 1) {
									int lastPacketSize = fileSize % kPayloadSize;
									if (lastPacketSize > 0) {
										readSize = lastPacketSize;
										doneFile = true;
									}
								}

								// Read file data into buffer
								inputFile.read(buffer, readSize);
								if (!inputFile.good() && !inputFile.eof()) {
									printf("\nError during data transmission\n");
								}

								if (badMode) //if in bad mode corrupt the data
								{
									corruptData(buffer, readSize); //corrupt the data that is being sent
									badMode = false; //only do it once
								}

								// serialize the data and send the packet
								serializeData(kFileDataPacket, readSize, buffer, packet);
								currentPacket++;

								if (doneFile)
								{
									inputFile.close();
									sendFile = false;
									printf("\nFile Copy complete.\n\n");
								}
							}
							
						}

					}
					else
					{
						printf("\nFile could not be read.\n");
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
			else if (mode == Server) //data in packet
			{
				int32_t intData = 0;
				
				char charData[kPayloadSize] = { '\0' };
				//first check for file name
				if (packet[0] != kFileInfoPacket && packet[0] != 0)
				{
					deserializeData(packet, &packetType, &intData, charData);				
				}
				else
				{
					deserializeData64(packet, &packetType, &startTime, charData);
				}				

				if (packetType == kFileInfoPacket) //check if we have not started receiving a file
				{
					receivedFileInfo = true; //we have now received a file

					fileName = charData; //copy the file name over

					printf("\nWriting File: %s\n\n", fileName.c_str()); //print our file that we're writing

				}
				else if (packetType == kChecksumPacket)
				{
					memcpy(receivedChecksumValue, charData, kPayloadSize);
					receivedChecksumValue[kPayloadSize] = '\0';
					finalFileSize = intData;
					receivedChecksum = true;
				}
				else if ((packetType == kFileDataPacket) && !isFileClosed)//otherwise we're reciving file data
				{
					//printf("\nGot file data packet %d\n", intData);

					if (intData < kEndOfFile){
						printf("\nno data to print\n");
					}
					else {
						//printf("\nthere is data to print\n");
					}
					//create file with the given name
					if (!outputFile.is_open())
					{
						// Open the file for writing while ensuring directory exists and I have writting permissions 
						outputFile.open(fileName, std::ofstream::binary);
						if (!outputFile)
						{

							printf("\nFile could not be opened for writing %s\n", fileName.c_str());

						}
					}
					if (intData == kPayloadSize && outputFile.good()) { // If there's data to write in the output file 
						
						outputFile.write(charData, intData);
						
						outputFile.flush();
						if (!outputFile.good()) {
							// Handle write error
							printf("\nError writing into file for output\n");
						}
						
					}
					else if (intData < kPayloadSize || currentFileSize >= finalFileSize) { // Check if end of file was reached 
						
						//final write
						outputFile.write(charData, intData);
						outputFile.flush();
						
						outputFile.flush();
						outputFile.close();
						isFileClosed = true;
						
						printf("\nFile Copy Complete\n");
						endTime = getTime();
						int64_t difference = endTime - startTime;
						double sec = ((difference + 500) / 1000);
						double timeinSeconds = calculateTransferSpeed(finalFileSize, sec);
						printf("Transfer Speed: %.2f Mbps\n", timeinSeconds);

						//handle final data
					}
					// checksum comparson
					if (isFileClosed && receivedChecksum) {
						ifstream receivedFilechecksum(fileName, ifstream::binary);
						if (!receivedFilechecksum) {
							printf("\nFailed to open for checksum verification.\n");
						}

						char calculatedFileChecksum[kPayloadSize];
						// Call generateChecksum
						generateChecksum(fileName, calculatedFileChecksum, nullptr);
						

						// Close the file after calculation
						receivedFilechecksum.close();

						printf("Value after checksum2\n\n%s \t\n %s \t\n", receivedChecksumValue, calculatedFileChecksum);
						if (memcmp(receivedChecksumValue, calculatedFileChecksum, sizeof(calculatedFileChecksum)) == 0) {
							printf("\nChecksum validated.\n\n");
						}
						else {
							printf("\nChecksum invalid. File is Likely Corrupt.\n\n");
						}
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

