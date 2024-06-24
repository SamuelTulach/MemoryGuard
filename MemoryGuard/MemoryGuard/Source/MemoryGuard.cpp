#include "MemoryGuard.h"

std::vector<GrdMem*> GrdMem::m_List;

void GrdMem::UpdateProtection()
{
    for (const auto instance : m_List)
        instance->Protect();
}

LONG GrdMem::VectoredExceptionHandler(EXCEPTION_POINTERS* exceptionInfo)
{
    if (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
    {
        const PVOID pageAddress = reinterpret_cast<PVOID>(exceptionInfo->ExceptionRecord->ExceptionInformation[1]);

        for (const auto instance : m_List)
        {
            if (!(pageAddress >= instance->m_Buffer && pageAddress < static_cast<PBYTE>(instance->m_Buffer) + instance->m_Size))
                continue;

            //printf("[GrdMem] Access to 0x%p from 0x%p\n", pageAddress, reinterpret_cast<PVOID>(exceptionInfo->ContextRecord->Rip));

            exceptionInfo->ContextRecord->EFlags |= 0x100;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    if (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
    {
        UpdateProtection();
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void GrdMem::Init()
{
    const PVOID value = AddVectoredExceptionHandler(1, &VectoredExceptionHandler);
    if (!value)
        throw std::exception("Exception handler not set");
}

GrdMem::GrdMem(const SIZE_T size) : m_Size(size)
{
    m_Buffer = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD);
    if (!m_Buffer)
        throw std::exception("Memory allocation fail");

    m_List.push_back(this);
}

GrdMem::~GrdMem()
{
    memset(m_Buffer, 0, m_Size);
    VirtualFree(m_Buffer, 0, MEM_RELEASE);
    m_List.erase(std::remove(m_List.begin(), m_List.end(), this), m_List.end());
}

void GrdMem::Protect() const
{
    DWORD oldProtect;
    if (!VirtualProtect(m_Buffer, m_Size, PAGE_READWRITE | PAGE_GUARD, &oldProtect))
        throw std::exception("Failed to set page protection");
}

PVOID GrdMem::Data() const
{
    return m_Buffer;
}