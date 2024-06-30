#include "Revision.h"
#include <iostream>
#include <fstream>
#include <sstream>  
#include <string>
#include <cassert>
#include <iterator>
#include <algorithm>
using namespace std;

//given functions
bool getInt(istream& inf, int& n)
{
	char ch;
	if (!inf.get(ch) || !isascii(ch) || !isdigit(ch))
		return false;
	inf.unget();
	inf >> n;
	return true;
}

bool getCommand(istream& inf, char& cmd, char& delim, int& length, int& offset)
{
	if (!inf.get(cmd))
	{
		cmd = 'x';  // signals end of file
		return true;
	}
	switch (cmd)
	{
	case '+':
		return inf.get(delim).good();
	case '#':
	{
		char ch;
		return getInt(inf, offset) && inf.get(ch) && ch == ',' && getInt(inf, length);
	}
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

//create revision function
void createRevision(istream& fold, istream& fnew, ostream& frevision)
{
	//copy the old and new files into strings
	char temp;
	string oldState = "";
	while (fold.get(temp))
	{
		oldState += temp;
	}

	string newState = "";
	while (fnew.get(temp))
	{
		newState += temp;
	}


	//determine the size of the hash sequence
	size_t hashSize = 8;

	if (newState.length() > 60000)
		hashSize = 16;

	HashTable hashTable;

	size_t N = hashSize;
	//add to the hashtable
	for (size_t i = 0; i <= oldState.length() - N; ++i)
	{
		string sequence = oldState.substr(i, N);
		hashTable.insert(sequence, i);
	}

	for (size_t j = 0; j < newState.length();) {
		string check = newState.substr(j, N);
		//copy function
		if (hashTable.contains(check)) {
			int bestIndex = -1;
			unsigned long bestlen = 0;

			// Retrieve the bucket index for the given string
			size_t index = hashTable.calculateIndex(check);

			// Iterate over all possible entries in the bucket to find the best match
			const auto& bucketEntries = hashTable.getBucketEntries(index);
			for (const auto& entry : bucketEntries) {
				if (entry.first == check) {
					size_t findIndex = entry.second;
					size_t currentMatchLength = N;

					// Extend the match as far as possible
					while (j + currentMatchLength < newState.length() && findIndex + currentMatchLength < oldState.length() &&
						newState[j + currentMatchLength] == oldState[findIndex + currentMatchLength]) {
						++currentMatchLength;
					}

					// Update the best match found
					if (currentMatchLength > bestlen) {
						bestlen = currentMatchLength;
						bestIndex = findIndex;
					}
				}
			}

			// If a match was found, write the copy instruction to the revision file
			if (bestIndex != -1) {
				frevision << '#' << bestIndex << ',' << bestlen;
				j += bestlen; // Move j to the end of the matched sequence
			}
		}
		//add function
		else {
			// Find the longest non-matching sequence starting from the current position
			size_t nextChar = j + 1;
			bool containsNext = false;
			while (nextChar < newState.length()) {
				if (hashTable.contains(newState.substr(nextChar, N))) {
					containsNext = true;
					break;
				}
				nextChar++;
			}

			// Adjust nextChar when there is a match
			if (containsNext) {
				nextChar--;
			}

			// get the length of the sequence to be added
			size_t addNumChar = nextChar - j + 1;

			//find an appropriate delimiter
			bool foundDelim = true;
			char delimiter = findDelimiter(newState, foundDelim);

			// Write the add instruction to the revision file
			frevision << "+" << delimiter << newState.substr(j, addNumChar) << delimiter;

			j = nextChar + 1; // Move j to the next position after the sequence is added
		}
	}

}
bool revise(istream& fold, istream& frevision, ostream& fnew)
{
	//add the old file into a string
	char oLine;
	string appendOld;
	while (fold.get(oLine))
	{
		appendOld += oLine;
	}

	string outputNew;

	char cmd;
	char delim;
	int length = 0;
	int offset = 0;

	while (true)
	{
		if (!getCommand(frevision, cmd, delim, length, offset)) {
			return false;
		}
		switch (cmd)
		{
		//copy case
		case '#':
		{
			if (length < 0 || offset < 0 || offset + length > appendOld.size())
				return false;
			//add the substring that is meant to be copied
			string addSequence = appendOld.substr(offset, length);
			fnew << addSequence;
			break;
		}
		//add case
		case '+':
		{
			if (length < 0)
			{
				return false;
			}
			//add until you hit another delimiter
			string add; 
			getline(frevision, add, delim);
			int addSize = add.size();
			fnew.write(add.c_str(), addSize);
			break;
		}
		//do nothing cases
		case '\n':
		case '\r':
			break;
			
		case 'x':
			return true;
		default:
			return false;
		}
	}
	return true;
}


