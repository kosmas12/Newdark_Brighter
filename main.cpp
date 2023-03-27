/*
Newdark_Brighter - An open source reimplementation of Newdark
Copyright (C) 2023 Kosmas Raptis

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <SDL2/SDL.h>
#include <iostream>
#if defined(__WIN32__)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#endif

SDL_bool hasMMX = SDL_FALSE;
SDL_bool has3DNow = SDL_TRUE;
int SSEVersion = 0;

inline long getPID() {
#if defined(__WIN32__)
    return GetCurrentProcessId();
#else
    return getppid();
#endif
}

std::string makeFormattedString(std::string format, ...) {
    va_list parameters;
    va_start(parameters, format);

    char buffer[0x7FF];
    vsnprintf(buffer, 0x7FF, format.c_str(), parameters);
    return std::string(buffer);
}

inline void checkForx86Extensions() {
    if (!SDL_HasSSE3()) {
        if (!SDL_HasSSE2()) {
            if (SDL_HasSSE()) {
                SSEVersion = 1;
            }
        }
        else {
            SSEVersion = 2;
        }
    }
    else {
        SSEVersion = 3;
    }
    hasMMX = SDL_HasMMX();
    has3DNow = SDL_Has3DNow();
}

inline void printOSVersion() {
    std::string osVersionString;
    bool readSuccessful;
#if defined(__WIN32__)
    OSVERSIONINFOA osVersion;
    memset(&osVersion.dwMajorVersion,0,0x90);
    osVersion.dwOSVersionInfoSize = 0x94;
    readSuccessful = GetVersionExA(&osVersion);
    if (readSuccessful) {
      osVersionString = makeFormattedString("OS Version: %d.%d.%d %s",osVersion.dwMajorVersion,
                                            osVersion.dwMinorVersion,osVersion.dwBuildNumber,
                                            osVersion.szCSDVersion);
    }
#else
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        readSuccessful = true;
        osVersionString = makeFormattedString("OS version: %s\n", buffer.release);
    }

#endif
    if (readSuccessful) {
        std::cout << osVersionString << std::endl;
    }
}

inline void printDesktopAndVirtualResolutions() {
    // Originally named debugString in the Ghidra project
    std::string resolutionString;

    int height, width;
#if defined(__WIN32__)
    height = GetSystemMetrics(SM_CYSCREEN);
    width = GetSystemMetrics(SM_CXSCREEN);
    resolutionString = makeFormattedString("Desktop resolution: %d x %d", width, height);
    std::cout << resolutionString << std::endl;

    height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    resolutionString = makeFormattedString("Virtual desktop resolution: %d x %d",width,height);
    std::cout << resolutionString << std::endl;
#else
    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode);
    height = displayMode.h;
    width = displayMode.w;
    resolutionString = makeFormattedString("Desktop resolution: %d x %d", width, height);
    std::cout << resolutionString << std::endl;

    height = 0;
    width = 0;
    int numDisplays = SDL_GetNumVideoDisplays();
    for (int i = 0; i < numDisplays; ++i) {
        SDL_GetDesktopDisplayMode(i, &displayMode);
        height += displayMode.h;
        width += displayMode.w;
    }
    resolutionString = makeFormattedString("Virtual desktop resolution: %d x %d", width, height);
    std::cout << resolutionString << std::endl;

#endif
}

void printCommandLine(int argc, char **argv) {
    char *commandLine;
#if defined(__WIN32__)
    commandLine = GetCommandLineA();
#else
    // Max command line length is usually 4096 characters
    commandLine = (char *) calloc(4096, sizeof(char));

    for (int i = 0; i < argc; ++i) {
        strcat(commandLine, argv[i]);
    }

#endif
    // Originally, the command line is assigned to `debugString` using makeFormattedString()
    printf("Command line: %s", commandLine);
    std::cout << std::endl;
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);

    long PID = getPID();
    std::string PIDString = makeFormattedString("PID: %d", PID);
    std::cout << PIDString << std::endl;

    /* TODO: Figure out how to get CPU info in a cross-platform way
        Precise Ghidra decompiler output (from Ghidra 10.2.3):
            SYSTEM_INFO systemInfo;
            ...
            [Address 0x006644b4]
            GetSystemInfo(&systemInfo);
              if ((systemInfo.u.dwOemId & 0xffff) < 0xb) {
                // WARNING: Could not find normalized switch variable to match jumptable
                // WARNING: This code block may not be properly labeled as switch case
                debugString = "x86-64";
              }
              else {
                sprintf(archString,(char *)&DAT_006f81b8,systemInfo.u.dwOemId & 0xffff);
                debugString = archString;
              }
              ...
                debugString = makeFormattedString("CPU: arch=%s , fam=%d , cpus=%d",debugString,
                                    systemInfo.dwProcessorType,systemInfo.dwNumberOfProcessors);

               NOTE: DAT_006f81b8's type and size are not clear, Ghidra reports no writes to it.
     */

    checkForx86Extensions();

    std::string SIMDInfoString = makeFormattedString("SIMD: SSE=%d , 3DNow=%d , MMX=%d", SSEVersion, has3DNow, hasMMX);
    std::cout << SIMDInfoString << std::endl;

    /* TODO: Figure out how to check for Large Address Aware (What kind of sorcery is this???)
        Precise Ghidra decompiler output (from Ghidra 10.2.3):
            HMODULE programHandle;
            ...
            [Address 0x006645cc]
              programHandle = GetModuleHandleA((LPCSTR)0x0);
              if (((*(short *)&programHandle->unused == 0x5a4d) &&
                  (*(int *)((int)&programHandle->unused + programHandle[0xf].unused) == 0x4550)) &&
                 ((*(byte *)((int)&programHandle[5].unused + programHandle[0xf].unused + 2) & 0x20) != 0)) {
                debugPrintWrapper("Large Address Aware is enabled.");
              }

         NOTE: Ghidra seems to think HMODULE is a struct, when in reality it's a typedef that eventually
         becomes a PVOID (void pointer), and access to this pointer is done here by referring to the member
         of the struct that Ghidra calls "unused". This PVOID is meant to point to the base address of the
         loaded executable.

         NOTE: This might look different when analyzed after parsing the Win32 header files.

         NOTE: According to Microsoft's documentation, all you need to know
         LAA is enabled is to check the Characteristics field (offset 18 or 0x12) in the COFF image header
         (which starts at offset 0x40 of the file) for the flag 0x20, yet, according to both
         disassembler and decompiler output, this code seems to be doing a bit more:

         Check if this is indeed running off of a valid PE file, in 2 steps:
            Compare the first 2 bytes of the file to 0x5a4d ("MZ" with reversed byte order, probably due to endianness)

            Go to the beginning of the PE header (Which is pointed to by offset 0x3c of the PE file itself)
            Compare its first 2 bytes to 0x4550 ("PE" with reversed byte order)

         Then, once it makes sure this is a valid PE, it checks the Characteristics field for the LAA flag.
    */

    printOSVersion();
    printDesktopAndVirtualResolutions();

    std::cout << "-----------------------------------------------------------" << std::endl;
    printCommandLine(argc, argv);
    std::cout << "-----------------------------------------------------------" << std::endl;

    SDL_Quit();
    return 0;
}
