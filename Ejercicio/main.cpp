#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define SIZE 4096

int _tmain(int argc, _TCHAR* argv[]) {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        SIZE,
        _T("SharedMemory"));

    if (hMapFile == NULL) {
        _tprintf(_T("Could not create file mapping object (%d).\n"), GetLastError());
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        SIZE);

    if (pBuf == NULL) {
        _tprintf(_T("Could not map view of file (%d).\n"), GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    _stprintf((TCHAR*)pBuf, _T("Hello, child process!"));

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create child process
    if (!CreateProcess(NULL,
                       _T("ChildProcess.exe"),
                       NULL,
                       NULL,
                       FALSE,
                       0,
                       NULL,
                       NULL,
                       &si,
                       &pi))
    {
        _tprintf(_T("CreateProcess failed (%d).\n"), GetLastError());
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    _tprintf(_T("Child reads: %s\n"), pBuf);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
