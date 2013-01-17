#include <windows.h>
#include <wchar.h>

bool GetExecutablePath(wchar_t* outPath, int outPathSize) {
    wchar_t path[4096];
    if (!GetModuleFileName(NULL, path, _countof(path)))
        return false;
    if (!GetLongPathName(path, path, _countof(path)))
        return false;
    swprintf_s(outPath, outPathSize, L"%s", path);
    return true;
}
bool GetExecutablePathQuoted(wchar_t* outPath, int outPathSize) {
    wchar_t* tempPath = new wchar_t[outPathSize];
    GetExecutablePath(tempPath, outPathSize);
    int result = swprintf_s(outPath, outPathSize, L"\"%s\"", tempPath);
    delete[] tempPath;
    tempPath = 0;
    if (result == -1)
        return false;
    return true;
}
bool GetExecutableFilename(wchar_t* outFilename, int outFilenameSize) {
    wchar_t path[4096];
    if (!GetExecutablePath(path, _countof(path)))
        return false;
    wchar_t drive[3];
    wchar_t directory[2941];
    wchar_t filename[1024];
    wchar_t extension[128];
    errno_t result = _wsplitpath_s(path, 
            drive, _countof(drive), 
            directory, _countof(directory), 
            filename, _countof(filename), 
            extension, _countof(extension));
    if (result != 0)
        return false;
    if (swprintf_s(outFilename, outFilenameSize, L"%s%s", filename, 
            extension) == -1) {
        return false;
    }
    return true;
}
bool GetExecutableName(wchar_t* outName, int outNameSize) {
    wchar_t path[4096];
    if (!GetExecutablePath(path, _countof(path)))
        return false;
    wchar_t drive[3];
    wchar_t directory[2941];
    wchar_t filename[1024];
    wchar_t extension[128];
    errno_t result = _wsplitpath_s(path,
            drive, _countof(drive), 
            directory, _countof(directory), 
            filename, _countof(filename), 
            extension, _countof(extension));
    if (result != 0)
        return false;
    if (swprintf_s(outName, outNameSize, L"%s", filename) == -1)
        return false;
    return true;
}
bool GetExecutableDirectory(wchar_t* outDirectory, int outDirectorySize) {
    /* Returns directory without slash at the end. */
    wchar_t path[4096];
    GetExecutablePath(path, _countof(path));
    wchar_t drive[3];
    wchar_t directory[2941];
    wchar_t filename[1024];
    wchar_t extension[128];
    errno_t result = _wsplitpath_s(path, 
            drive, _countof(drive),
            directory, _countof(directory), 
            filename, _countof(filename),
            extension, _countof(extension));
    if (result != 0)
        return false;
    if (swprintf_s(outDirectory, outDirectorySize, L"%s%s", drive, 
            directory) == -1) {
        return false;
    }
    int len = wcslen(outDirectory);
    if (outDirectory[len-1] == '\\' || outDirectory[len-1] == '/')
        outDirectory[len-1] = 0;
    return true;
}