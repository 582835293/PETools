#include "PETools.h"
#include <stdlib.h>
#include <string.h>

static int failures;
static int checks;
#define CHECK(expr) do { ++checks; if (!(expr)) { ++failures; printf("FAIL line %d: %s\n", __LINE__, #expr); } } while (0)

int main(void) {
    unsigned char file[0x400] = {0};
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)file;
    IMAGE_NT_HEADERS32* nt = (IMAGE_NT_HEADERS32*)(file + 0x80);
    IMAGE_SECTION_HEADER* section;
    PEHeaderInfo info = {0};
    void* image = NULL;
    void* roundtrip = NULL;
    void* code = NULL;
    dos->e_magic = IMAGE_DOS_SIGNATURE;
    dos->e_lfanew = 0x80;
    nt->Signature = IMAGE_NT_SIGNATURE;
    nt->FileHeader.Machine = IMAGE_FILE_MACHINE_I386;
    nt->FileHeader.NumberOfSections = 1;
    nt->FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER32);
    nt->OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
    nt->OptionalHeader.ImageBase = 0x400000;
    nt->OptionalHeader.AddressOfEntryPoint = 0x1000;
    nt->OptionalHeader.SizeOfHeaders = 0x200;
    nt->OptionalHeader.SizeOfImage = 0x2000;
    nt->OptionalHeader.SectionAlignment = 0x1000;
    nt->OptionalHeader.FileAlignment = 0x200;
    section = IMAGE_FIRST_SECTION(nt);
    memcpy(section->Name, ".text", 5);
    section->VirtualAddress = 0x1000;
    section->Misc.VirtualSize = 0x200;
    section->PointerToRawData = 0x200;
    section->SizeOfRawData = 0x200;
    section->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;
    file[0x200] = 0xB8;
    file[0x201] = 42;
    CHECK(Align(0x201, 0x200) == 0x400);
    CHECK(Align(0x400, 0x200) == 0x400);
    CHECK(!ValidatePEFile(NULL));
    CHECK(ValidatePEFile(file));
    CHECK(RvaToFileOffset(file, 0x40) == 0x40);
    CHECK(RvaToFileOffset(file, 0x1020) == 0x220);
    CHECK(RvaToFileOffset(file, 0x3000) == 0);
    ParsePEHeaders(file, &info);
    CHECK(info.dwEntryPoint == 0x1000);
    CHECK(info.dwImageBase == 0x400000);
    CHECK(info.wNumberOfSections == 1);
    CHECK(CopyFileBufferToImageBuffer(file, &image) == 0x2000);
    CHECK(image != NULL);
    if (image) {
        CHECK(memcmp((unsigned char*)image + 0x1000, file + 0x200, 0x200) == 0);
        CHECK(QueryCodeSection(image, &code) == 0x200);
        CHECK(code == (unsigned char*)image + 0x1000);
        CHECK(CopyImageBufferToFileBuffer(image, &roundtrip) == sizeof(file));
        CHECK(roundtrip != NULL);
        if (roundtrip) CHECK(memcmp(roundtrip, file, sizeof(file)) == 0);
    }
    free(image);
    free(roundtrip);
    printf("PETools: %d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
