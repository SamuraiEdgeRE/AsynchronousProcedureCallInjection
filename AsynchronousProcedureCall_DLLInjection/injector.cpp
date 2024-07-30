// ファイル名：injector.cpp
// 説明：インジェクターで使用する関数などを定義しています

#include"injector.h"

// プロセス名からプロセスIDを取得する関数
// processName：検索するプロセス名
// processId：プロセスID（参照として受け取る）
// 戻り値：成功した場合はTRUE、失敗した場合はFALSEを返す
BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId) {
	// プロセスのスナップショットを作成
	HANDLE processSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);
	if (processSnapshotHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "[-] CreateToolhelp32Snapshotが失敗しました。Error code: " << GetLastError() << std::endl;
		return FALSE;
	}

	// プロセス情報の初期化
	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };

	// 最初のプロセスを取得
	BOOL hasNextProcess = Process32First(processSnapshotHandle, &processEntry);

	while (hasNextProcess) {
		if (wcsstr(processEntry.szExeFile, processName) != NULL) {
			processId = processEntry.th32ProcessID;
			return TRUE;
		}
		// 次のプロセスを取得
		hasNextProcess = Process32Next(processSnapshotHandle, &processEntry);
	}

	CloseHandle(processSnapshotHandle);
	return FALSE;
}

// アドレスを16進数の文字列として表示する
// address：メモリアドレス
// 戻り値：メモリアドレスの16進数を含む文字列
std::string PrintAddress(LPVOID address) {
	std::ostringstream stream;
	stream << std::showbase << std::hex << reinterpret_cast<uintptr_t>(address);
	return stream.str();
}

// ターゲットプロセスのすべてのスレッドにAPC関数をキューする関数
// processId：プロセスID（参照として受け取る）
// apcFunction: APC関数のポインタ
// apcParam: APC関数に渡す引数のポインタ
// 戻り値：成功した場合はTRUE、失敗した場合はFALSE
BOOL InjectUsingQueueUserAPC(DWORD& processId, LPVOID apcFunction, LPVOID apcParam) {

	// 全プロセスのスレッドのスナップショットを作成
	HANDLE threadSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (threadSnapshotHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "[-] CreateToolhelp32Snapshotが失敗しました。エラーコード: " << GetLastError() << std::endl;
		return FALSE;
	}

	// スレッド情報の初期化
	THREADENTRY32 threadEntry = { sizeof(THREADENTRY32) };

	//　最初のスレッドを取得
	BOOL hasNextThread = Thread32First(threadSnapshotHandle, &threadEntry);

	while (hasNextThread) {
		if (threadEntry.th32OwnerProcessID == processId) {

			// スレッドハンドルを取得
			HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			if (threadHandle == NULL) {
				std::cerr << "[-] OpenThreadが失敗しました。エラーコード: " << GetLastError() << std::endl;
				CloseHandle(threadSnapshotHandle);
				return FALSE;
			}

			// QueueUserAPCでAPCをキューイング
			DWORD success = QueueUserAPC(static_cast<PAPCFUNC>(apcFunction), threadHandle, reinterpret_cast<ULONG_PTR>(apcParam));
			if (success == NULL) {
				std::cerr << "[-] QueueUserAPCが失敗しました。エラーコード: " << GetLastError() << std::endl;
				CloseHandle(threadSnapshotHandle);
				CloseHandle(threadHandle);
				return FALSE;
			}
			std::cout << "[+] スレッド" << threadEntry.th32ThreadID << "にAPCをキューしました" << std::endl;
				
			CloseHandle(threadHandle);
		}
		hasNextThread = Thread32Next(threadSnapshotHandle, &threadEntry);
	}
	CloseHandle(threadSnapshotHandle);
	return TRUE;
}