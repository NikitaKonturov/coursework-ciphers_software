#include "fileMapping.hpp"

// Функция для получения названия ошибки
std::string getLastErrorCodeMessage(DWORD _dwErrCode)
{
    LPSTR messageBuffer = nullptr;
 
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        _dwErrCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Язык по умолчанию
        (LPTSTR)&messageBuffer,
        0, NULL);
 
 
    std::string resCode(std::to_string(_dwErrCode));
    std::string resMess(messageBuffer, size);
    LocalFree(messageBuffer); // Освобождение буффера
    if (resMess.empty())
    {
        return "(" + resCode + ")";
    }
    else
    {
        return "(" + resCode + "): " + resMess;
    }
}
 
// Функция для разбиения файла на части посредстом FileMapping и вывода результата в res
// ВАЖНО! файл должен быть в кодировке windows 1251
bool read_and_markup_big_file(const std::string strFilePath, std::string& res)
{
    setlocale(LC_ALL, ".1251");

    SYSTEM_INFO sysInfo = { 0 };
    GetSystemInfo(&sysInfo);
    DWORD dwSysGran = sysInfo.dwAllocationGranularity; // Получение значения свободной памяти буффера
 
    HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);// Создание файла
    if (hFile == INVALID_HANDLE_VALUE) // Файл не создан
    {
        res = "ERROR - CreateFileA " + getLastErrorCodeMessage(GetLastError()) + "\n";
        return false;
    }
 
    LARGE_INTEGER liFileSize = { 0 }; // Временное хранилище размера большого файла
    if (!GetFileSizeEx(hFile, &liFileSize)) // Проверка на недопустимый размер файла
    {
        res = "ERROR - GetFileSizeEx " + getLastErrorCodeMessage(GetLastError()) + "\n";
        return false;
    }
    const uint64_t cbFile = static_cast<uint64_t>(liFileSize.QuadPart);
 
    if (cbFile == 0) // Проверка на пустоту файла
    {
        res = "Size of File os ZERO";
        CloseHandle(hFile); // Закрываем файл
        return true;
    }
 
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL); // Создаём файл разметки
    CloseHandle(hFile); // К самому файлу больше не нужен доступ
    if (hMap == NULL) 
    {
        res = "ERROR - CreateFileMapping " + getLastErrorCodeMessage(GetLastError()) + "\n";
        return false;
    }
 
    uint64_t ok = 0; // Количество успешно прочтённых фрагментов
    uint64_t no = 0; // Количество повреждённых объъектов
    uint64_t all = 0; // Колличество всех фрагментов
    for (uint64_t offset = 0; offset < cbFile; offset += dwSysGran)
    {
        DWORD high = static_cast<DWORD>((offset >> 32) & 0xFFFFFFFFul);
        DWORD low = static_cast<DWORD>(offset & 0xFFFFFFFFul);
        
        //Последний читаемый фрагмент можеть быть короче
        if (offset + dwSysGran > cbFile)
        {
            dwSysGran = static_cast<int>(cbFile - offset);
        }
 
        //const char *pView = static_cast<const char *>(MapViewOfFile(hMap, FILE_MAP_READ, high, low, dwSysGran));
        PBYTE pView = static_cast<PBYTE>(MapViewOfFile(hMap, FILE_MAP_READ, high, low, dwSysGran)); //  LPVOID lpMapAddress;  // pointer to the base address of the memory-mapped region
 
 
        if (pView) //if (pView != NULL)
        {
            ok++;
        }
        else // if pView == NULL
        {
            no++;
            res = "ERROR - MapViewOfFile " + getLastErrorCodeMessage(GetLastError()) + "\n";
 
            UnmapViewOfFile(pView);
            CloseHandle(hMap);
            return false;
        }
 
        if (pView)
        {
            UnmapViewOfFile(pView); //Освобождение памяти
        }
 
        all++;
    }
 
    CloseHandle(hMap);
 
    res = "SUCCES - Mapped all segments: " + std::to_string(all) + "\n"; //Вывод количества фрагментов в res
    return true;
}
