#include "PETools.h"
int main() { return Align(0x201, 0x200) == 0x400 ? 0 : 1; }
