#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <mutex>
#include <windows.h>
#include <time.h>
#include <synchapi.h>

#define MAXTHREADS 10

using namespace std;

typedef struct
{
	int id;
	int sleepTime;
}threadParam;

HANDLE hmutex = NULL;

int userInputThread()
{
	int numThread = 0;
	while (numThread < 2 || numThread > 10)
	{
		cout << "Enter the number of threads[2 to 10] > ";
		cin >> numThread;
	}
	return numThread;
}

int userInputTime(int threadID)
{
	int interval = 0;
	while (interval > 20 || interval < 1)
	{
		cout << "Enter the delay interval for thread #" << threadID << " [1 to 20] seconds > ";
		cin >> interval;
	}
	return interval;
}

void userInputTimes(int numThread, int *sleepTime)
{
	for (int i = 0; i < numThread; i++)
	{
		sleepTime[i] = userInputTime(i + 1);
	}
}

DWORD WINAPI MyThreadFunction(void *Param)
{
	threadParam tp = *(threadParam*)Param;
	
	struct tm * timeinfo;
	int threadCount = 1;

	while (threadCount <= 30)
	{
		WaitForSingleObject(hmutex, INFINITE);
		time_t currentTime = time(NULL);
		timeinfo = localtime(&currentTime);
		cout << "Thread #" << tp.id << ": [" << threadCount << "]: " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << endl;
		ReleaseMutex(hmutex);
		Sleep(tp.sleepTime * 1000);
		threadCount++;
	}

	return 0;
}

void launchThreads(int numThread, int *sleepTime)
{
	HANDLE hThread[MAXTHREADS];
	threadParam tp[MAXTHREADS];
	for (int i = 0; i < numThread; i++)
	{
		tp[i].id = i + 1;
		tp[i].sleepTime = sleepTime[i];
		WaitForSingleObject(hmutex, INFINITE);
		cout << "Launching thread number: " << tp[i].id << " with sleepTime = " << tp[i].sleepTime << endl;
		ReleaseMutex(hmutex);
		hThread[i] = CreateThread(NULL, 0, MyThreadFunction, &tp[i], 0, NULL);
	}
	WaitForMultipleObjects(numThread, hThread, TRUE, INFINITE);
}

int main()
{
	hmutex = CreateMutex(NULL, FALSE, "myMutex");
	int numThread = userInputThread();
	int sleepTimes[MAXTHREADS] = { 0 };
	userInputTimes(numThread, sleepTimes);
	launchThreads(numThread, sleepTimes);

	system("pause");
	return 0;
}

