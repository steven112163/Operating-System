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

// MF
void MF(vector<process>& processes, const int& highTimeQuantum, const int& lowTimeQuantum);

// highRR
void highRR(vector<process>& processes, const int& timeQuantum, int& time, int& number, vector<process>& highQueue, vector<process>& lowQueue);

// lowRR
void lowRR(vector<process>& processes, const int& timeQuantum, int& quantum, int& time, int& number, vector<process>& highQueue, vector<process>& lowQueue, vector<process>& SJFQueue);

// SJF
void SJF(vector<process>& processes, int& time, int& number, bool& removed, vector<process>& highQueue, vector<process>& SJFQueue);

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
	ofstream outputFile("ans4.txt");
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

	getline(inputFile, inputNumbers);
	ss.str(inputNumbers);
	ss >> input;
	int highTimeQuantum = atoi(input.c_str());
	ss >> input;
	int lowTimeQuantum = atoi(input.c_str());

	inputFile.close();

	// Sort processes' arrivalTime in natural order
	sort(processes.begin(), processes.end(), arrival_order);

	// Assign orderID
	for(int i = 0; i < processes.size(); i++)
		processes[i].orderID = i;

	// Run MF
	MF(processes, highTimeQuantum, lowTimeQuantum);

	// Sort processes' processID in natural order
	sort(processes.begin(), processes.end(), process_order);

	// Compute average waitingTime and turnaroundTime;
	int totalWaitingTime = 0, totalTurnaroundTime = 0;
	for(int i = 0; i < numberOfProcesses; i++) {
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

// MF
void MF(vector<process>& processes, const int& highTimeQuantum, const int& lowTimeQuantum) {
	// Burst time
	int time = processes[0].arrivalTime;
	// Number of unfinished processes
	int number = processes.size();

	// (Low) time quantum
	int lowQuantum = 0;

	// High process queue
	vector<process> highQueue;
	highQueue.push_back(processes[0]);
	// Low process queue
	vector<process> lowQueue;
	// SJF process queue
	vector<process> SJFQueue;

	// (SJF) Remember removed process
	bool removed = false;

	// Check whether SJFQueue is empty at the beginning of lowRR
	bool empty = false;

	while(number > 0) {
		if(highQueue.size() > 0)
			highRR(processes, highTimeQuantum, time, number, highQueue, lowQueue);
		else if(lowQueue.size() > 0) {
			if(SJFQueue.size() == 0)
				empty = true;
			lowRR(processes, lowTimeQuantum, lowQuantum, time, number, highQueue, lowQueue, SJFQueue);
			if(empty || removed) {
				sort(SJFQueue.begin(), SJFQueue.end(), burst_order);
				empty = false;
				removed = false;
			} else
				sort(SJFQueue.begin()+1, SJFQueue.end(), burst_order);
		} else {
			SJF(processes, time, number, removed, highQueue, SJFQueue);
		}
	}

	return;
}

// highRR
void highRR(vector<process>& processes, const int& timeQuantum, int& time, int& number, vector<process>& highQueue, vector<process>& lowQueue) {
	// Time quantum
	int quantum = 0;
	// Remember removed process
	int removed = -1;

	while(highQueue.size() > 0) {
		time++;
		processes[highQueue.front().orderID].runTime++;
		quantum++;

		// Remove finished or quantum-elapsed process
		if(processes[highQueue.front().orderID].runTime == processes[highQueue.front().orderID].burstTime) {
			processes[highQueue.front().orderID].turnaroundTime = time - processes[highQueue.front().orderID].arrivalTime;
			processes[highQueue.front().orderID].waitingTime = processes[highQueue.front().orderID].turnaroundTime - processes[highQueue.front().orderID].burstTime;
			number--;
			highQueue.erase(highQueue.begin());
			quantum = 0;
		} else if(quantum == timeQuantum) {
			removed = highQueue.front().orderID;
			highQueue.erase(highQueue.begin());
			quantum = 0;
		}

		// Add unfinished process
		if(removed != -1) {
			lowQueue.push_back(processes[removed]);
			removed = -1;
		}
		// Add next arrival process
		for(int i = processes.size()-number; i < processes.size(); i++)
			if(time == processes[i].arrivalTime)
				highQueue.push_back(processes[i]);
	}
	
	return;
}

// lowRR
void lowRR(vector<process>& processes, const int& timeQuantum, int& quantum, int& time, int& number, vector<process>& highQueue, vector<process>& lowQueue, vector<process>& SJFQueue) {
	// Remember removed process
	int removed = -1;

	while(lowQueue.size() > 0) {
		time++;
		processes[lowQueue.front().orderID].runTime++;
		quantum++;

		// Remove finished or quantum-elapsed process
		if(processes[lowQueue.front().orderID].runTime == processes[lowQueue.front().orderID].burstTime) {
			processes[lowQueue.front().orderID].turnaroundTime = time - processes[lowQueue.front().orderID].arrivalTime;
			processes[lowQueue.front().orderID].waitingTime = processes[lowQueue.front().orderID].turnaroundTime - processes[lowQueue.front().orderID].burstTime;
			number--;
			lowQueue.erase(lowQueue.begin());
			quantum = 0;
		} else if(quantum == timeQuantum) {
			removed = lowQueue.front().orderID;
			lowQueue.erase(lowQueue.begin());
			quantum = 0;
		}

		// Add unfinished process
		if(removed != -1) {
			SJFQueue.push_back(processes[removed]);
			removed = -1;
		}
		// Check next arrival process
		for(int i = processes.size()-number; i < processes.size(); i++)
			if(time == processes[i].arrivalTime)
				highQueue.push_back(processes[i]);
		if(highQueue.size() > 0)
			return;
	}
	
	return;
}

// SJF
void SJF(vector<process>& processes, int& time, int& number, bool& removed, vector<process>& highQueue, vector<process>& SJFQueue) {
	while(number > 0) {
		// Remember last number
		int oldNumber = number;
		if(SJFQueue.size() > 0) {
			time++;
			processes[SJFQueue.front().orderID].runTime++;
			
			// Remove finished process
			if(processes[SJFQueue.front().orderID].runTime == processes[SJFQueue.front().orderID].burstTime) {
				processes[SJFQueue.front().orderID].turnaroundTime = time - processes[SJFQueue.front().orderID].arrivalTime;
				processes[SJFQueue.front().orderID].waitingTime = processes[SJFQueue.front().orderID].turnaroundTime - processes[SJFQueue.front().orderID].burstTime;
				SJFQueue.erase(SJFQueue.begin());
				number--;
			}
		} else
			time++;

		// Check next arrival process
		for(int i = processes.size()-number; i < processes.size(); i++)
			if(time == processes[i].arrivalTime)
				highQueue.push_back(processes[i]);
		if(highQueue.size() > 0) {
			if(oldNumber < number)
				removed = true;
			return;
		}
	}
	
	return;
}
