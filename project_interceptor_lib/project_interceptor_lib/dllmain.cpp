// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <winspool.h>
#include <vector>
#include <thread>
#include <chrono>

#define DLL_EXPORT __declspec(dllexport)
struct JobInfo {
	DWORD JobId;
	std::wstring Document;
	DWORD Status;
};

std::vector<JobInfo> GetCurrentJobs(const std::wstring& printerName) {
	std::vector<JobInfo> jobs;
	PRINTER_INFO_2* pPrinterInfo = NULL;
	DWORD dwNeeded = 0;
	DWORD deReturned = 0;
	HANDLE hPrinter = NULL;
	JOB_INFO_2* pJobInfo = NULL;
	DWORD cByteUsed = 0;
	DWORD cJobs = 0;
	//std::wstring printerName = GetDefaultPrinterName();

	//Getting the printer name
	//DWORD cchprinterName = 0;
	//this is the length of the printerName
	//GetDefaultPrinter(NULL, &cchprinterName);
	//This is to get the length of the name of the printer
	//std::wstring printerName(cchprinterName, L'\0');
	//Adding null to the buffer to acquire the printer name
	//GetDefaultPrinter(&printerName[0], &cchprinterName);
	//Now this call of getDefaultPrinter is to acquire the name of the printer inside the given buffer.


	//Open the handle to the printer
	if (!OpenPrinter(const_cast<LPWSTR>(printerName.c_str()), &hPrinter, NULL)) {
		std::cerr << "Error opening the handle to the printer" << std::endl;
		return jobs;
	}


	//Then we have to enumaerate or iterate through all the jobs inside the print queue
	EnumJobs(hPrinter, 0, 0xFFFFFFFF, 2, NULL, 0, &cByteUsed, &cJobs);
	if (cJobs == 0) {
		ClosePrinter(hPrinter);
		//Close the handle of the printer of default of the system
		return jobs;
	}

	pJobInfo = (JOB_INFO_2*)malloc(cByteUsed);
	if (!pJobInfo) {
		std::cerr << "Failed to allocate memory for job info" << std::endl;
		ClosePrinter(hPrinter);
		return jobs;
	}

	if (!EnumJobs(hPrinter, 0, 0xFFFFFFFF, 2, (LPBYTE)pJobInfo, cByteUsed, &dwNeeded, &cJobs)) {
		std::cerr << "Failed during enumeration of the print jobs" << std::endl;
		free(pJobInfo);
		ClosePrinter(hPrinter);
		return jobs;
	}

	//display job information
	/*for (DWORD i = 0; i < cJobs; i++) {
		std::cout << "Job ID: " << pJobInfo[i].JobId
			<< ", Document: " << pJobInfo[i].pDocument
			<< ", Status: " << pJobInfo[i].Status << std::endl;
	}*/
	for (DWORD i = 0; i < cJobs; i++) {
		jobs.push_back({ pJobInfo[i].JobId, pJobInfo[i].pDocument, pJobInfo[i].Status });
	}
	free(pJobInfo);
	ClosePrinter(hPrinter);
	return jobs;
}

std::wstring GetDefaultPrinterName() {
	DWORD cchPrinterName = 0;
	GetDefaultPrinter(NULL, &cchPrinterName);
	std::wstring printerName(cchPrinterName, L'\0');
	GetDefaultPrinter(&printerName[0], &cchPrinterName);
	return printerName;
}

extern "C" {
	DLL_EXPORT void startMonitoring();
	DLL_EXPORT void stopMonitoring();
	DLL_EXPORT void ListJobs();
	DLL_EXPORT void cancelJobs();
	DLL_EXPORT void MonitorPrintQueue();
	/*DLL_EXPORT std::vector<JobInfo> GetCurrentJobs(const std::wstring& printerName);*/
	//This is the fn that callbacks the alert inside the system

	/*void(*alertCallback)(const char*) = nullptr;*/
	//this is the char array dynamically having the message that can be sent to the 

	//We will be using OpenPrinter function to obtain the handle of the default pruinter used by the user on the system.
	
	void MonitorPrintQueue() {
		std::cout << "Function monitor called" << std::endl;
		//DWORD cchprinterName = 0;
		//this is the length of the printerName
		//GetDefaultPrinter(NULL, &cchprinterName);
		//This is to get the length of the name of the printer
		std::wstring printerName = GetDefaultPrinterName();
		//Adding null to the buffer to acquire the printer name
		//GetDefaultPrinter(&printerName[0], &cchprinterName);

		std::wcout << "Monitoring print queue for printer: " << printerName << std::endl;

		std::vector<JobInfo> prevJobs;
		while (true) {
			std::vector<JobInfo> currentJobs = GetCurrentJobs(printerName);


			for (const auto& job:currentJobs) {
				auto it = std::find_if(prevJobs.begin(), prevJobs.end(), [&job](const JobInfo& prevJob) { return prevJob.JobId == job.JobId; });
				//Lambda fn is trying to find if the job alrady exists inside the vector jobs
				if (it == prevJobs.end()) {
					std::wcout << L"New print job detected: ID: " << job.JobId
						<< L", Document: " << job.Document
						<< L", Status: " << job.Status << std::endl;
				}
				else if (it->Status != job.Status) {
					std::wcout << L"Job ID: " << job.JobId
						<< L"Status changed to : " << job.Status
						<< L"For the Document: " << job.Document;
				}
			}
			for (const auto& prevJob : prevJobs) {
				//Lambda fn to check if any of the oprevjobs have finished
				auto it = std::find_if(currentJobs.begin(), currentJobs.end(),
					[&prevJob](const JobInfo& job) {return job.JobId == prevJob.JobId; });

				if (it == currentJobs.end()) {
					std::wcout << L"Job completed ID: " << prevJob.JobId
						<< L", Document: " << prevJob.Document << std::endl;
				}
			}

			prevJobs = currentJobs;

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}


//These events will be called on their own on some debuig events
//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}

