#include <Windows.h>
#include <stdio.h>
#include <sstream>
#include <vector>
using namespace std;
// Device type
#define SIOCTL_TYPE 40000

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define IOCTL_HELLO\
 CTL_CODE( SIOCTL_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)


vector<string> __cdecl GetProcesses(){
    HANDLE hDevice;
    const char* welcome = "Give me processes";
    DWORD dwBytesRead = 0;
    char ReadBuffer[50] = { 0 };
    std::vector<string> vect;
    while (vect.size() == 0)
    {
        hDevice = CreateFile(L"\\\\.\\DriverBlockerLink", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        DeviceIoControl(hDevice, IOCTL_HELLO, (LPVOID)welcome, strlen(welcome), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
        //printf("Current length: %d", strlen(ReadBuffer));
        CloseHandle(hDevice);
        //printf("Failed\n");
        if (strlen(ReadBuffer) != 0) {
            printf("Got message: %s\n", ReadBuffer);
            //std::stringstream ss((string)ReadBuffer);
            //while (ss.good()) {
            //    string substr;
            //    std::getline(ss, substr, ',');
            //    vect.push_back(substr);
            //}
            vect.push_back(ReadBuffer);
        }
    }
    return vect;
}

int inject(int pid)
{
	HANDLE processHandle;
	PVOID remoteBuffer;
	wchar_t dllPath[] = TEXT("C:\\Users\\User\\Desktop\\DriverHooker.dll");

	printf("Injecting DLL to PID: %i\n", pid);
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));
	remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof dllPath, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, sizeof dllPath, NULL);
	PTHREAD_START_ROUTINE threatStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
	CreateRemoteThread(processHandle, NULL, 0, threatStartRoutineAddress, remoteBuffer, 0, NULL);
	CloseHandle(processHandle);

	return 0;
}

int main(int argc, char* argv[]) {
    while (true) {
        vector<string> processes = GetProcesses();
        //if (processes.size() > 0)
        //{
         //   for (int i = 0; i < processes.size(); i++) {
          //          printf("Got target process: %s\n", processes[i]);
           //         inject(stoi(processes[i]));
            //}
        //}
        for (int i = 0; i < processes.size(); i++)
        {
            inject(stoi(processes[i]));
       }
        Sleep(100);
    }
}


