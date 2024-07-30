// �t�@�C�����Finjector.cpp
// �����F�C���W�F�N�^�[�Ŏg�p����֐��Ȃǂ��`���Ă��܂�

#include"injector.h"

// �v���Z�X������v���Z�XID���擾����֐�
// processName�F��������v���Z�X��
// processId�F�v���Z�XID�i�Q�ƂƂ��Ď󂯎��j
// �߂�l�F���������ꍇ��TRUE�A���s�����ꍇ��FALSE��Ԃ�
BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId) {
	// �v���Z�X�̃X�i�b�v�V���b�g���쐬
	HANDLE processSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);
	if (processSnapshotHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "[-] CreateToolhelp32Snapshot�����s���܂����BError code: " << GetLastError() << std::endl;
		return FALSE;
	}

	// �v���Z�X���̏�����
	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };

	// �ŏ��̃v���Z�X���擾
	BOOL hasNextProcess = Process32First(processSnapshotHandle, &processEntry);

	while (hasNextProcess) {
		if (wcsstr(processEntry.szExeFile, processName) != NULL) {
			processId = processEntry.th32ProcessID;
			return TRUE;
		}
		// ���̃v���Z�X���擾
		hasNextProcess = Process32Next(processSnapshotHandle, &processEntry);
	}

	CloseHandle(processSnapshotHandle);
	return FALSE;
}

// �A�h���X��16�i���̕�����Ƃ��ĕ\������
// address�F�������A�h���X
// �߂�l�F�������A�h���X��16�i�����܂ޕ�����
std::string PrintAddress(LPVOID address) {
	std::ostringstream stream;
	stream << std::showbase << std::hex << reinterpret_cast<uintptr_t>(address);
	return stream.str();
}

// �^�[�Q�b�g�v���Z�X�̂��ׂẴX���b�h��APC�֐����L���[����֐�
// processId�F�v���Z�XID�i�Q�ƂƂ��Ď󂯎��j
// apcFunction: APC�֐��̃|�C���^
// apcParam: APC�֐��ɓn�������̃|�C���^
// �߂�l�F���������ꍇ��TRUE�A���s�����ꍇ��FALSE
BOOL InjectUsingQueueUserAPC(DWORD& processId, LPVOID apcFunction, LPVOID apcParam) {

	// �S�v���Z�X�̃X���b�h�̃X�i�b�v�V���b�g���쐬
	HANDLE threadSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (threadSnapshotHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "[-] CreateToolhelp32Snapshot�����s���܂����B�G���[�R�[�h: " << GetLastError() << std::endl;
		return FALSE;
	}

	// �X���b�h���̏�����
	THREADENTRY32 threadEntry = { sizeof(THREADENTRY32) };

	//�@�ŏ��̃X���b�h���擾
	BOOL hasNextThread = Thread32First(threadSnapshotHandle, &threadEntry);

	while (hasNextThread) {
		if (threadEntry.th32OwnerProcessID == processId) {

			// �X���b�h�n���h�����擾
			HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			if (threadHandle == NULL) {
				std::cerr << "[-] OpenThread�����s���܂����B�G���[�R�[�h: " << GetLastError() << std::endl;
				CloseHandle(threadSnapshotHandle);
				return FALSE;
			}

			// QueueUserAPC��APC���L���[�C���O
			DWORD success = QueueUserAPC(static_cast<PAPCFUNC>(apcFunction), threadHandle, reinterpret_cast<ULONG_PTR>(apcParam));
			if (success == NULL) {
				std::cerr << "[-] QueueUserAPC�����s���܂����B�G���[�R�[�h: " << GetLastError() << std::endl;
				CloseHandle(threadSnapshotHandle);
				CloseHandle(threadHandle);
				return FALSE;
			}
			std::cout << "[+] �X���b�h" << threadEntry.th32ThreadID << "��APC���L���[���܂���" << std::endl;
				
			CloseHandle(threadHandle);
		}
		hasNextThread = Thread32Next(threadSnapshotHandle, &threadEntry);
	}
	CloseHandle(threadSnapshotHandle);
	return TRUE;
}