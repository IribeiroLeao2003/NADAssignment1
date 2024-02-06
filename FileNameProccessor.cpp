#include "FileTransfer.h"

const int kIgnoreSlash = 1;


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