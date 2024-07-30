//ファイル名：injector.h
//説明：インジェクターで必要な関数などを宣言しています

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>
#include<thread>
#include<chrono>

BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& dwProcId);
std::string PrintAddress(LPVOID address);
BOOL InjectUsingQueueUserAPC(DWORD& processId, LPVOID apcFunction, LPVOID apcParam);