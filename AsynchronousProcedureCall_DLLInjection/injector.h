//�t�@�C�����Finjector.h
//�����F�C���W�F�N�^�[�ŕK�v�Ȋ֐��Ȃǂ�錾���Ă��܂�

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>
#include<thread>
#include<chrono>

BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& dwProcId);
std::string PrintAddress(LPVOID address);
BOOL InjectUsingQueueUserAPC(DWORD& processId, LPVOID apcFunction, LPVOID apcParam);