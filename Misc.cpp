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
	system_clock::time_point time = system_clock::now(); //get time
	system_clock::duration sinceEpoc = time.time_since_epoch(); //get time since epoch
	system_clock::duration milliSecondsSinceEpoc = duration_cast<milliseconds>(sinceEpoc); //convert to miliseconds;

	 return milliSecondsSinceEpoc.count(); //return milliseconds

}