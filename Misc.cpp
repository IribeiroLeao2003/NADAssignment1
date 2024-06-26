#include "FileTransfer.h"

const int kIgnoreSlash = 1;



/*
* FUNCTION    : fileNameExtractor()
* DESCRIPTION : This function takes a file path and extracts the file name from it
* PARAMETERS  : void
* RETURNS     : string containing the file name
*/
string fileNameExtractor(string filePath)
{
	int slashLoc = filePath.find_last_of("/");
	int length = filePath.length();
	string substring = "";

	if (slashLoc + kIgnoreSlash == filePath.length()) //check if what we found was actually at the end of the string
	{
		//filePath.erase(slashLoc); //remove end character
		slashLoc = filePath.find_last_of("/"); //find again
		substring = filePath.substr(slashLoc + kIgnoreSlash); //add one so we ignore the /
	}
	else //name doesn't end with /
	{
		substring = filePath.substr(slashLoc + kIgnoreSlash); //add one so we ignore the /
	}

	return substring;
}


/*
* FUNCTION    : getTime()
* DESCRIPTION : This returns the current time since epoch in milliseconds
* PARAMETERS  : void
* RETURNS     : int64_t time in milliseconds sinse epoch
*/
int64_t getTime()
{
	// using correct namespace
	using namespace std::chrono;
	
	auto Timenow = system_clock::now();	// Get the current time 
	
	auto since_epoch = Timenow.time_since_epoch();	// Convert to a duration since epoch
	
	auto millis = duration_cast<milliseconds>(since_epoch).count();		//Convert to mileseconds
	
	return millis;		
	

} 

/*
* FUNCTION    : calculateTransferSpeed()
* DESCRIPTION : This returns the current transfer speed in mbps
* PARAMETERS  : int fileSize
*			  : double seconds
* RETURNS     : double mbps which should be the transfer speed
*/
double calculateTransferSpeed(int fileSize, double seconds) {
	
	int fileSizeinBits = fileSize * 8;		//calculatefile size in bits


	double bps = fileSizeinBits / seconds;	//calculate bits per seconds


	double mbps = bps / 1000000.0;			// ensuring number accuracy using point division

	return mbps;
}
