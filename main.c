#include <stdio.h>
#include <windows.h>
#include <msdelta.h>

void ReportLastError(LPWSTR pFunctionName) {
    DWORD dwLastError = GetLastError();

    WCHAR Message[4096] = {};
    DWORD dwLength = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        Message,
        sizeof(Message) / sizeof(WCHAR),
        NULL
    );
    fprintf(stderr, "%S: (0x%08lx) %S\n", pFunctionName, dwLastError, Message);
}

BOOL ReadDeltaInput(wchar_t *filename, LPDELTA_INPUT lpDeltaInput) {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LPVOID lpData = NULL;

    hFile = CreateFileW(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        goto fail;

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
        goto fail;

    lpData = HeapAlloc(GetProcessHeap(), 0, dwFileSize);
    if (lpData == NULL)
        goto fail;

    DWORD dwBytesRead = 0;
    ReadFile(hFile, lpData, dwFileSize, &dwBytesRead, NULL);
    if (dwBytesRead != dwFileSize)
        goto fail;

    lpDeltaInput->lpcStart = lpData;
    lpDeltaInput->uSize = dwFileSize;
    lpDeltaInput->Editable = FALSE;

    CloseHandle(hFile);
    return TRUE;

fail:
    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

    if (lpData != NULL)
        HeapFree(GetProcessHeap(), 0, lpData);

    return FALSE;
}

BOOL WriteDeltaOutput(wchar_t *filename, LPDELTA_OUTPUT lpDeltaOutput) {
    HANDLE hFile = INVALID_HANDLE_VALUE;

    hFile = CreateFileW(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        goto fail;

    DWORD dwBytesWritten = 0;
    WriteFile(hFile, lpDeltaOutput->lpStart, (DWORD) lpDeltaOutput->uSize, &dwBytesWritten, NULL);
    if (dwBytesWritten != lpDeltaOutput->uSize)
        goto fail;

    CloseHandle(hFile);
    return TRUE;

fail:
    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

    return FALSE;
}

int wmain(int argc, wchar_t *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %S [Source] [Delta] [Target]\n", argv[0]);
        return 1;
    }

    DELTA_INPUT Source = {};
    if (ReadDeltaInput(argv[1], &Source) == FALSE) {
        ReportLastError(L"ReadDeltaInput(Source)");
        return 1;
    }

    DELTA_INPUT Delta = {};
    if (ReadDeltaInput(argv[2], &Delta) == FALSE) {
        ReportLastError(L"ReadDeltaInput(Delta)");
        return 1;
    }

    DELTA_INPUT DeltaPatch = Delta;
    if (DeltaPatch.uSize >= 4 && RtlEqualMemory("PA", DeltaPatch.lpcStart, 2) == FALSE) {
        // The first four bytes are either a header PA\d{2} or a CRC32.
        DeltaPatch.lpcStart = (LPVOID)((uintptr_t)DeltaPatch.lpcStart + 4);
        DeltaPatch.uSize -= 4;
    }

    DELTA_OUTPUT Target = {};
    if (ApplyDeltaB(DELTA_APPLY_FLAG_ALLOW_PA19, Source, DeltaPatch, &Target) == FALSE) {
        ReportLastError(L"ApplyDeltaB");
        return 1;
    }

    if (WriteDeltaOutput(argv[3], &Target) == FALSE) {
        ReportLastError(L"WriteDeltaOutput(Target)");
        return 1;
    }

    HeapFree(GetProcessHeap(), 0, Source.lpStart);
    HeapFree(GetProcessHeap(), 0, Target.lpStart);
    DeltaFree(Target.lpStart);

    return 0;
}