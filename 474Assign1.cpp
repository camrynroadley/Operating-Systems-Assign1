#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <string>
using namespace std;

int main()
{
	int numberOfValues;
	string fileName;
	clock_t start;
	clock_t end;

	// Ask user what file they would like to use
	int fileNumber;
	cout << "Please enter the data file number that you'd like to use (1, 2, 3 or 4): ";
	cin >> fileNumber;

	// Ask user how many children they would like to use
	int numOfChildren;
	cout << "Please enter the number of child processes that you'd like to use (1, 2, or 4): ";
	cin >> numOfChildren;

	// Set fileName and numberOfValues based on the user's answers
	if (fileNumber == 1) {
		numberOfValues = 1000;
		fileName = "file1.dat";
	} else if (fileNumber == 2) {
		numberOfValues = 10000;
		fileName = "file2.dat";
	} else if (fileNumber == 3) {
		numberOfValues = 100000;
		fileName = "file3.dat";
	} else {
		numberOfValues = 1000000;
		fileName = "file4.dat";
	}

	const char* name = fileName.c_str();
	int valuesPerChild = numberOfValues / numOfChildren;

	// Set up fork and pipe 
	int CToP[numOfChildren][2];
	int PToC[numOfChildren][2];

	// Get start time
	start = clock();

	// Initialize pipes
	for (int i = 0; i < numOfChildren; i++)
	{
		pipe(CToP[i]); 
		pipe(PToC[i]);
	}

	// For loop through each of the child processes
	for (int i = 0; i < numOfChildren; i++)
	{
		int sum = 0;
		int range[2]; // holds the range that the child will fetch
		range[0] = (i * valuesPerChild);
		range[1] = range[0] + valuesPerChild;

		// Send the range to child
		write(PToC[i][1], range, 2 * sizeof(int));

		// Child Process
		if (fork() == 0) 
		{
			int childRange[2]; // array to hold range from parent

			read(PToC[i][0], childRange, 2 * sizeof(int)); // read range from parent

			FILE * pFile = fopen(name, "r");

			for (int j = childRange[0]; j < childRange[1]; j++) 
			{
				fseek(pFile, (j * 4), SEEK_SET);

				char line[4];
				fgets(line, 4, pFile);

				string strValue = "";
				for (int k = 0; k < 3; k++) {
					strValue += line[k];
				}
				
				int value = atoi(strValue.c_str());
				sum += value;
			}
			fclose(pFile);

			// Send sum back to parent
			write(CToP[i][1], &sum, sizeof(int));

			exit(0);
		}
	}

	// Get the sum from each of the child processes and add to the totalSum
	int totalSum = 0;
	for (int i = 0; i < numOfChildren; i++) 
	{
		int buf;
		read(CToP[i][0], &buf, sizeof(int));
		totalSum += buf; // add the sums together
	}

	// Get end time
	end = clock();

	cout << "Sum: " << totalSum << endl;
	cout << "Execution Time: " << (end - start)/(double)(CLOCKS_PER_SEC)*1000 << " milliseconds" << endl;
	
	return 0;
}
