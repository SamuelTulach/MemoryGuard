#ifndef MEMORY_GUARD_H
#define MEMORY_GUARD_H

#include <Windows.h>
#include <iostream>
#include <exception>
#include <vector>

class GrdMem
{
private:
    static std::vector<GrdMem*> m_List;

    PVOID m_Buffer;
    SIZE_T m_Size;

public:
    static void UpdateProtection();
    static LONG VectoredExceptionHandler(EXCEPTION_POINTERS* exceptionInfo);
    static void Init();

    GrdMem(SIZE_T size);
    ~GrdMem();

    void Protect() const;
    PVOID Data() const;
};

#endif