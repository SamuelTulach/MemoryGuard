#include "MemoryGuard.h"
#include "StackText.h"

int main()
{
    GrdMem::Init();

    PVOID data1;
    {
        printf("Writing text to unprotected memory...\n");
        data1 = malloc(512);
        char text[] = MAKESTR("TESTDATA_UNPROTECTED", 20);
        strcpy_s(static_cast<char*>(data1), 512, text);
        memset(text, 0, 20);
    }

    PVOID data2;
    {
        printf("Writing text to protected memory...\n");
        GrdMem* buffer = new GrdMem(512);
        data2 = buffer->Data();
        char text[] = MAKESTR("TESTDATA_MEMORY_GUARD_PROTECTED", 31);
        strcpy_s(static_cast<char*>(data2), 512, text);
        memset(text, 0, 20);
    }

    printf("String is now saved in the memory, press any key to print it\n");
    printf("Press key to continue...\n");
    getchar();

    printf("Unprotected: %s\n", static_cast<char*>(data1));
    printf("Protected:   %s\n", static_cast<char*>(data2));

    printf("Press key to exit...\n");
    getchar();
    return 0;
}