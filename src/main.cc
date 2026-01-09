//===------------------------------------------------------------------------------------------===//
// main.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "strings2/core.hh"
#include "strings2.hh"              // IWYU pragma: keep

// Standard headers
#include <cstring>

#if AAO_PLATFORM_WINDOWS
#include <fcntl.h>
#include <io.h>
// Define a helper to set the console output to UTF-8 on windows.
void SetConsoleUTF8 () { SetConsoleOutputCP (65001); }
#else
#include <unistd.h>
// Standard terminals usually handle this for us.
void SetConsoleUTF8 () {}
#endif

int main(int argc, char *argv[])
{
    SetConsoleUTF8();

    strings2::sStringOptions Options;
    std::string szFilterArg = "";

    bool bFlagHelp = false;
    bool bFlagDumpPid = false;
    bool bFlagDumpSystem = false;
    bool bFlagRecursive = false;

    // Check for piped input
#if AAO_PLATFORM_WINDOWS
    bool bPipedInput = !_isatty (_fileno (stdin));
#else
    bool bPipedInput = !isatty (fileno (stdin));
#endif

    if (argc <= 1 && !bPipedInput)
        bFlagHelp = true;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-help" || arg == "-h")
            bFlagHelp = true;
        else if (arg == "-f")
            Options.bPrintFilename = true;
        else if (arg == "-F")
            Options.bPrintFilepath = true;
        else if (arg == "-r")
            bFlagRecursive = true;
        else if (arg == "-t")
            Options.bPrintStringType = true;
        else if (arg == "-s")
            Options.bPrintSpan = true;
        else if (arg == "-e")
            Options.bEscapeNewLines = true;
        else if (arg == "-json")
            Options.bPrintJson = true;
        else if (arg == "-a")
        {
            Options.bPrintInteresting = true;
            Options.bPrintNotInteresting = true;
        }
        else if (arg == "-ni")
        {
            Options.bPrintInteresting = false;
            Options.bPrintNotInteresting = true;
        }
        else if (arg == "-utf" || arg == "-utf8")
        {
            Options.bPrintUTF8 = true;
            Options.bPrintWideString = false;
        }
        else if (arg == "-w" || arg == "-wide")
        {
            Options.bPrintUTF8 = false;
            Options.bPrintWideString = true;
        }
        else if (arg == "-pid")
            bFlagDumpPid = true;
        else if (arg == "-system")
            bFlagDumpSystem = true;
        else if (arg == "-l")
        {
            if (i + 1 < argc)
            {
                try
                {
                    Options.iMinChars = std::stoi(argv[i + 1]);
                    i++;
                }
                catch (...)
                {
                    std::cerr << "Warning: Invalid minimum characters value specified: " << argv[i + 1] << std::endl;
                }
            }
        }
        else if (arg == "-b")
        {
            if (i + 1 < argc)
            {
                std::string szRange = argv[i + 1];
                i++;    // Consume

                try
                {
                    strings2::usize iColonPos = szRange.find(':');

                    if (iColonPos == std::string::npos)
                    {
                        // No colon was found, treat entire string as offset
                        //
                        // Base 0 allows 0x prefix for hex automatically.
                        Options.iOffsetStart = std::stoull(szRange, nullptr, 0);
                    }
                    else
                    {
                        // Colon was found so we parse "start:end"
                        std::string szStartPart = szRange.substr(0, iColonPos);
                        std::string szEndPart = szRange.substr(iColonPos + 1);

                        // Parse start if it's not empty
                        if (!szStartPart.empty())
                        {
                            Options.iOffsetStart = std::stoull(szStartPart, nullptr, 0);
                        }

                        // Parse end if it's not empty
                        if (!szEndPart.empty())
                        {
                            Options.iOffsetEnd = std::stoull(szEndPart, nullptr, 0);
                        }
                    }
                }
                catch (...)
                {
                    std::cerr << "Warning: Invalid byte range format specified: " << szRange << std::endl;
                }
            }
        }
        else
        {
            if (szFilterArg.empty())
                szFilterArg = arg;
            else
            {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return 1;
            }
        }
    }

    if (bFlagHelp)
    {
        printf (
            "strings2 extracts all unicode/ascii strings from binary data. On top of the classical "
            "strings approach, this version decodes multilingual strings (eg Chinese, Russian, "
            "etc) and uses a ML model to suppress noisy uninteresting strings.\n\n");
        printf ("Example Usage:\n");
        printf ("\tstrings2 malware.exe\n");
        printf ("\tstrings2 *.exe > strings.txt\n");
        printf ("\tstrings2 ./files/*.exe > strings.txt\n");
        printf ("\tstrings2 -pid 419 > process_strings.txt\n");
        printf ("\tstrings2 -f -s -pid 0x1a3 > process_strings.txt\n");
        printf ("\tstrings2 -system > all_process_strings.txt\n");
#if AAO_PLATFORM_WINDOWS
        printf ("\ttype abcd.exe | strings2 > out.txt\n\n");
#else
        printf ("\tcat abcd.exe | strings2 > out.txt\n\n");
#endif
        printf ("\tstrings2 malware.exe -json > strings.json\n");
        printf ("Flags:\n");
        printf (" -r\n\tRecursively process subdirectories.\n");
        printf (" -f\n\tPrints the filename/processname for each string.\n");
        printf (" -F\n\tPrints the full path and filename for each string.\n");
        printf (" -s\n\tPrints the file offset or memory address span\n\tof each string.\n");
        printf (" -t\n\tPrints the string type for each string. UTF8,\n\tor WIDE_STRING.\n");
        printf (" -wide\n\tPrints only WIDE_STRING strings that are encoded\n\tas two bytes per "
                "character.\n");
        printf (" -utf\n\tPrints only UTF8 encoded strings.\n");
        printf (" -a\n\tPrints both interesting and not interesting strings.\n\tDefault only "
                "prints interesting non-junk strings.\n");
        printf (" -ni\n\tPrints only not interesting strings. Default only\n\tprints interesting "
                "non-junk strings.\n");
        printf (" -e\n\tEscape new line characters.\n");
        printf (" -l [numchars]\n\tMinimum number of characters that is a valid string.\n\tDefault "
                "is 4.\n");
        printf (" -b [start](:[end])\n\tScan only the specified byte range for strings.\n");
        printf (" -pid [pid]\n\tThe strings from the process address space for the\n\tspecified "
                "PID will be dumped. Use a '0x' prefix to\n\tspecify a hex PID. (Windows Only)\n");
        printf (" -system\n\tDumps strings from all accessible processes on the\n\tsystem. This "
                "takes awhile. (Windows Only)\n");
        printf (" -json\n\tWrites output as json. Many flags are ignored in this mode.\n");
        return 0;
    }

    strings2::CStringParser *pParser = new strings2::CStringParser(Options);

    if (bFlagDumpPid || bFlagDumpSystem)
    {
#if AAO_PLATFORM_WINDOWS
        // Existing Windows Logic
        if (strings2::IsWin64() && sizeof (void *) == 4)
        {
            std::cerr << "WARNING: Running 32-bit binary on 64-bit OS. Process dumping may fail."
                      << std::endl;
        }
        strings2::GetPrivileges(GetCurrentProcess ());

        // CMemoryStrings is Windows-specific
        strings2::CMemoryStrings *pProcess = new strings2::CMemoryStrings(pParser);

        if (bFlagDumpPid)
        {
            try
            {
                DWORD pid = std::stoul (szFilterArg);
                pProcess->DumpProcess (pid);
            }
            catch (...)
            {
                std::cerr << "Invalid PID specified." << std::endl;
            }
        }
        else if (bFlagDumpSystem)
        {
            pProcess->DumpSystem ();
        }
        delete pProcess;
#else
        std::cerr
            << "Error: Process dumping (-pid / -system) is currently only supported on Windows."
            << std::endl;
#endif
    }
    else if (bPipedInput)
    {
#if AAO_PLATFORM_WINDOWS
        _setmode (_fileno (stdin), _O_BINARY);
#endif
// Use fdopen to get a FILE* from the stdin file descriptor
#if AAO_PLATFORM_WINDOWS
        FILE *fh = _fdopen (_fileno (stdin), "rb");
#else
        FILE *fh = fdopen (fileno (stdin), "rb");
#endif
        if (fh)
        {
            pParser->ParseStream (fh, "piped data", "piped data");
            fclose (fh);
        }
        else
        {
            std::cerr << "Error reading piped input." << std::endl;
        }
    }
    else if (!szFilterArg.empty ())
    {
        // Handle Paths vs Filters
        std::filesystem::path p (szFilterArg);

        if (std::filesystem::exists (p))
        {
            // It is a real path (file or dir)
            ProcessPath (p, "", bFlagRecursive, pParser);
        }
        else
        {
            // It might be a wildcard pattern like "*.exe" or "C:/bin/*.exe"
            std::filesystem::path parent = p.parent_path ();
            if (parent.empty ())
                parent = ".";

            std::string szFilePattern = p.filename ().string ();
            ProcessPath (parent, szFilePattern, bFlagRecursive, pParser);
        }
    }

    delete pParser;
    return 0;
}
