#include <windows.h>
#include <wchar.h>

bool GetExecutablePath(wchar_t* outpath, int outpath_length)
{
    // This path is longer than MAX_PATH, do not use it with
    // windows api functions as you might get buffer overflows.
    wchar_t shortpath[1024];
    wchar_t longpath[1024];
    if (!GetModuleFileName(NULL, shortpath, 1024)) 
        return false;
    if (!GetLongPathName(shortpath, longpath, 1024))
        return false;
    swprintf_s(outpath, outpath_length, L"%s", longpath);
    return true;
}

bool GetExecutablePathQuoted(wchar_t* outpath, int outpath_length)
{
    wchar_t* temppath = new wchar_t[outpath_length];
    GetExecutablePath(temppath, outpath_length);
    int result = swprintf_s(outpath, outpath_length, L"\"%s\"", temppath);
    delete[] temppath;
    temppath = 0;
    if (result == -1)
        return false;
    return true;
}

bool GetExecutableFilename(wchar_t* outdir, int outdir_length)
{
    // Filename with extension.
    wchar_t longpath[1024];
    if (!GetExecutablePath(longpath, _countof(longpath)))
        return false;
    wchar_t drive[3];
    wchar_t dir[768];
    wchar_t fname[256];
    wchar_t ext[32];
    errno_t result = _wsplitpath_s(longpath, drive, _countof(drive), dir, 
            _countof(dir), fname, _countof(fname), ext, _countof(ext));
    if (result != 0)
        return false;
    if (swprintf_s(outdir, outdir_length, L"%s%s", fname, ext) == -1)
        return false;
    return true;
}

bool GetExecutableName(wchar_t* outdir, int outdir_length)
{
    wchar_t longpath[1024];
    if (!GetExecutablePath(longpath, _countof(longpath)))
        return false;
    wchar_t drive[3];
    wchar_t dir[768];
    wchar_t fname[256];
    wchar_t ext[32];
    errno_t result = _wsplitpath_s(longpath, drive, _countof(drive), dir, 
            _countof(dir), fname, _countof(fname), ext, _countof(ext));
    if (result != 0)
        return false;
    if (swprintf_s(outdir, outdir_length, L"%s", fname) == -1)
        return false;
    return true;
}

bool GetExecutableDir(wchar_t* outdir, int outdir_length)
{
    // Returns directory without slash at the end.
    // This path is longer than MAX_PATH, do not use it with
    // windows api functions as you might get buffer overflows.
    wchar_t longpath[1024];
    GetExecutablePath(longpath, _countof(longpath));
    wchar_t drive[3];
    wchar_t dir[768];
    wchar_t fname[256];
    wchar_t ext[32];
    errno_t result = _wsplitpath_s(longpath, drive, _countof(drive), dir, 
            _countof(dir), fname, _countof(fname), ext, _countof(ext));
    if (result != 0)
        return false;
    if (swprintf_s(outdir, outdir_length, L"%s%s", drive, dir) == -1)
        return false;
    int len = wcslen(outdir);
    // Remove slash at the end.
    if (outdir[len-1] == '\\' || outdir[len-1] == '/') {
        outdir[len-1] = 0;
    }
    return true;
}
