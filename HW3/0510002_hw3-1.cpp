#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iomanip>

using namespace std;

// Process structure
typedef struct{
	int processID;
	int arrivalTime;
	int burstTime;
	int runTime;
	int waitingTime;
	int turnaroundTime;
	int orderID;
} process;

// Sort processes' arrivalTime in natural order
bool arrival_order(process i, process j) {
	if(i.arrivalTime < j.arrivalTime)
		return true;
	else if(i.arrivalTime == j.arrivalTime)
		return i.burstTime < j.burstTime;
	else
		return false;
}

// Sort processes' burstTime in natural order
bool burst_order(process i, process j) { return i.burstTime < j.burstTime; }

// Sort processes' processID in natural order
bool process_order(process i, process j) { return i.processID < j.processID; }

// SJF
void SJF(vector<process>& processes);

int main(int argc, char* argv[]) {

	// Warn invalid input
	if(argc != 2) {
		cout << "Invalid input" << endl;
		return 0;
	}

	// Check whether successfully open input file
	ifstream inputFile(argv[1]);
	if(inputFile.fail()) {
		perror("Failed to open input file");
		return 0;
	}

	// Check whether successfully open output file
	ofstream outputFile("ans1.txt");
	if(outputFile.fail()) {
		perror("Failed to open output file");
		return 0;
	}

	// Get the number of processes
	string inputNumbers;
	getline(inputFile, inputNumbers);
	stringstream ss(inputNumbers);
	string input;
	ss >> input;
	int numberOfProcesses = atoi(input.c_str());
	vector<process> processes(numberOfProcesses);

	// Get the information of every process
	getline(inputFile, inputNumbers);
	ss.str(inputNumbers);

	for(int number = 0; number < numberOfProcesses; number++) {
		processes[number].processID = number;
		processes[number].runTime = 0;
		ss >> input;
		processes[number].arrivalTime = atoi(input.c_str());
	}

	getline(inputFile, inputNumbers);
	ss.str(inputNumbers);
	for(int number = 0; number < numberOfProcesses; number++) {
		ss >> input;
		processes[number].burstTime = atoi(input.c_str());
	}

	inputFile.close();

	// Sort processes' arrivalTime in natural order
	sort(processes.begin(), processes.end(), arrival_order);

	// Assign orderID
	for(int i = 0; i < processes.size(); i++)
		processes[i].orderID = i;

	// Run SJF
	SJF(processes);

	// Sort processes' processID in natural order
	sort(processes.begin(), processes.end(), process_order);

	// Compute average waitingTime and turnaroundTime;
	int totalWaitingTime = 0, totalTurnaroundTime = 0;
	for(int i = 0; i < processes.size(); i++) {
		outputFile << processes[i].waitingTime << " " << processes[i].turnaroundTime << endl;
		totalWaitingTime += processes[i].waitingTime;
		totalTurnaroundTime += processes[i].turnaroundTime;
	}
	float aveWaitingTime = (float)totalWaitingTime / numberOfProcesses;
	float aveTurnaroundTime = (float)totalTurnaroundTime / numberOfProcesses;

	int digits = 0;
	int n = (int)aveWaitingTime;
	do {
		digits++;
		n /= 10;
	} while(n > 0);
	outputFile << setprecision(digits+5) << aveWaitingTime << endl;
	digits = 0;
	n = (int)aveTurnaroundTime;
	do {
		digits++;
		n /= 10;
	} while(n > 0);
	outputFile << setprecision(digits+5) << aveTurnaroundTime << endl;

	outputFile.close();
		
	return 0;
}

// SJF
void SJF(vector<process>& processes) {
	// Burst time
	int time = processes[0].arrivalTime;
	// Number of unfinished processes
	int number = processes.size();
	// Process queue
	vector<process> processQueue;
	processQueue.push_back(processes[0]);
	// Check whether the process is removed
	bool removed = false;

	while(number > 0) {
		if(processQueue.size() > 0) {
			time++;
			processes[processQueue.front().orderID].runTime++;
			
			// Remove finished process
			if(processes[processQueue.front().orderID].runTime == processes[processQueue.front().orderID].burstTime) {
				processes[processQueue.front().orderID].turnaroundTime = time - processes[processQueue.front().orderID].arrivalTime;
				processes[processQueue.front().orderID].waitingTime = processes[processQueue.front().orderID].turnaroundTime - processes[processQueue.front().orderID].burstTime;
				processQueue.erase(processQueue.begin());
				number--;
				removed = true;
			}
		} else
			time++;

		// Add next process
		for(int i = processes.size()-number; i < processes.size(); i++)
			if(time == processes[i].arrivalTime)
				processQueue.push_back(processes[i]);
		if(removed) {
			sort(processQueue.begin(), processQueue.end(), burst_order);
			removed = false;
		} else
			sort(processQueue.begin()+1, processQueue.end(), burst_order);
	}

	return;
}
