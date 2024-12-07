/**
 * by lingview 
 * https://github.com/lingview/windows_ls
 */

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
    WIN32_FIND_DATA data;
} FileEntry;

int CompareBySize(const void *a, const void *b);
int CompareByTime(const void *a, const void *b);

void ListFilesInDirectory(LPCTSTR lpPath, int listLong, int showAll, int recursive, int horizontal, int sortBySize, int sortByTime, int showDirsOnly);
void PrintLongFormat(const WIN32_FIND_DATA *findFileData);

int _tmain(int argc, TCHAR *argv[])
{
    int i = 1;
    int listLong = 0;
    int showAll = 0;
    int recursive = 0;
    int horizontal = 1;
    int sortBySize = 0;
    int sortByTime = 0;
    int showDirsOnly = 0;

    while (i < argc && argv[i][0] == _T('-'))
    {
        for (LPCTSTR p = &argv[i][1]; *p != _T('\0'); ++p)
        {
            switch (*p)
            {
            case _T('l'):
                listLong = 1;
                break;
            case _T('a'):
                showAll = 1;
                break;
            case _T('R'):
                recursive = 1;
                break;
            case _T('x'):
                horizontal = 1;
                break;
            case _T('s'):
                sortBySize = 1;
                break;
            case _T('d'):
                showDirsOnly = 1;
                break;
            case _T('t'): 
                sortByTime = 1;
                break;
            default:
                _ftprintf(stderr, _T("Unknown option: -%c\n"), *p);
                return 1;
            }
        }
        ++i;
    }

    if (sortBySize && sortByTime)
    {
        _ftprintf(stderr, _T("Cannot use both -s and -t options simultaneously.\n"));
        return 1;
    }

    if (i >= argc)
    {
        TCHAR currentDir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, currentDir);
        ListFilesInDirectory(currentDir, listLong, showAll, recursive, horizontal, sortBySize, sortByTime, showDirsOnly);
    }
    else
    {
        for (; i < argc; ++i)
        {
            ListFilesInDirectory(argv[i], listLong, showAll, recursive, horizontal, sortBySize, sortByTime, showDirsOnly);
        }
    }

    return 0;
}

void ListFilesInDirectory(LPCTSTR lpPath, int listLong, int showAll, int recursive, int horizontal, int sortBySize, int sortByTime, int showDirsOnly)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    TCHAR szDir[MAX_PATH];
    _stprintf_s(szDir, MAX_PATH, _T("%s\\*"), lpPath);

    hFind = FindFirstFile(szDir, &findFileData);

    if (INVALID_HANDLE_VALUE == hFind)
    {
        _tprintf(_T("FindFirstFile failed (%d)\n"), GetLastError());
        return;
    }
    else
    {
        FileEntry *files = NULL;
        int fileCount = 0;

        do
        {
            if (!showAll)
            {
                if (_tcscmp(findFileData.cFileName, _T(".")) == 0 ||
                    _tcscmp(findFileData.cFileName, _T("..")) == 0)
                {
                    continue;
                }
            }

            if (showDirsOnly && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                continue;
            }

            files = (FileEntry *)realloc(files, (fileCount + 1) * sizeof(FileEntry));
            files[fileCount].data = findFileData;
            fileCount++;
        } while (FindNextFile(hFind, &findFileData) != 0);

        if (GetLastError() != ERROR_NO_MORE_FILES)
        {
            _tprintf(_T("FindNextFile failed (%d)\n"), GetLastError());
        }

        if (sortBySize)
        {
            qsort(files, fileCount, sizeof(FileEntry), CompareBySize);
        }
        else if (sortByTime)
        {
            qsort(files, fileCount, sizeof(FileEntry), CompareByTime);
        }

        int printedCount = 0;
        for (int j = 0; j < fileCount; ++j)
        {
            if (listLong)
            {
                PrintLongFormat(&files[j].data);
            }
            else
            {
                if (horizontal)
                {
                    _tprintf(_T("%s\t"), files[j].data.cFileName);
                    printedCount++;
                    if (printedCount % 5 == 0) { 
                        _tprintf(_T("\n"));
                    }
                }
                else
                {
                    _tprintf(_T("%s\n"), files[j].data.cFileName);
                }
            }

            if (recursive && (files[j].data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                _tcscmp(files[j].data.cFileName, _T(".")) != 0 &&
                _tcscmp(files[j].data.cFileName, _T("..")) != 0)
            {
                _tprintf(_T("\n%s:\n"), files[j].data.cFileName);
                TCHAR subDir[MAX_PATH];
                _stprintf_s(subDir, MAX_PATH, _T("%s\\%s"), lpPath, files[j].data.cFileName);
                ListFilesInDirectory(subDir, listLong, showAll, recursive, horizontal, sortBySize, sortByTime, showDirsOnly);
            }
        }

        if (horizontal && printedCount % 5 != 0) { 
            _tprintf(_T("\n"));
        }

        free(files);
        FindClose(hFind);
    }
}

void PrintLongFormat(const WIN32_FIND_DATA *findFileData)
{
    TCHAR attrs[6]; 
    _stprintf_s(attrs, sizeof(attrs) / sizeof(TCHAR), _T("%c%c%c%c%c"),
                (findFileData->dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? 'r' : '-',
                (findFileData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? 'h' : '-',
                (findFileData->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? 's' : '-',
                (findFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 'd' : '-',
                (findFileData->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? 'a' : '-');

    ULARGE_INTEGER fileSize;
    fileSize.LowPart = findFileData->nFileSizeLow;
    fileSize.HighPart = findFileData->nFileSizeHigh;

    SYSTEMTIME st;
    FileTimeToSystemTime(&(findFileData->ftLastWriteTime), &st);
    _tprintf(_T("%s %I64u %02d/%02d/%04d %02d:%02d %s\n"),
            attrs,
            fileSize.QuadPart,
            st.wMonth, st.wDay, st.wYear,
            st.wHour, st.wMinute,
            findFileData->cFileName);
}

int CompareBySize(const void *a, const void *b)
{
    ULARGE_INTEGER sizeA, sizeB;
    sizeA.LowPart = ((FileEntry *)a)->data.nFileSizeLow;
    sizeA.HighPart = ((FileEntry *)a)->data.nFileSizeHigh;
    sizeB.LowPart = ((FileEntry *)b)->data.nFileSizeLow;
    sizeB.HighPart = ((FileEntry *)b)->data.nFileSizeHigh;

    if (sizeA.QuadPart < sizeB.QuadPart)
        return -1;
    if (sizeA.QuadPart > sizeB.QuadPart)
        return 1;
    return 0;
}

int CompareByTime(const void *a, const void *b)
{
    FILETIME timeA = ((FileEntry *)a)->data.ftLastWriteTime;
    FILETIME timeB = ((FileEntry *)b)->data.ftLastWriteTime;

    if (CompareFileTime(&timeA, &timeB) < 0)
        return -1;
    if (CompareFileTime(&timeA, &timeB) > 0)
        return 1;
    return 0;
}



/**
 * 参数指南
 * -l	以长格式列出文件信息，包括权限、大小、修改日期和时间等
 * -a	显示所有文件，包括隐藏文件（. 和 ..）
 * -R	递归地列出指定目录及其子目录中的文件
 * -x	横向排布文件，一行一行地列出文件
 * -s	按文件大小排序
 * -d	仅显示目录
 * -t	按最后修改时间排序
 */