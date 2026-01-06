# kstrings - fork of strings2
This is fork of 'strings2' and is intended to be cross-platform.

Uses CMake for building.

## Build Status

|Compiler|Passing|Notes|
|--------|-------|-----|
|Apple Clang|✅||
|Apple GCC|❔||
|Visual Studio 2022|✅|
|Visual Studio 2026|✅|
|Clang (Windows)|✅|
|MinGW GCC|⚠️|Do not define ``AAO_USE_SANITIZERS`` to ``ON``, sanitizer doesn't work on MinGW??|
|MinGW Clang|❔||

## Documentation

```kstrings (options) file_pattern```

* `file_pattern` can be a folder or file. Wildcards (`*`) are supported in the filename parts - eg `.\files\*.dat`.

|Option|Description|
|--|--|
|-r|Recursively process subdirectories.|
|-f|Prints the filename/processname for each string.|
|-F|Prints the full path and filename for each string.|
|-s|Prints the file offset or memory address span of each string.|
|-t|Prints the string type for each string. UTF8, or WIDE_STRING.|
|-wide|Prints only WIDE_STRING strings that are encoded as two bytes per character.|
|-utf|Prints only UTF8 encoded strings.|
|-a|Prints both interesting and not interesting strings. Default only prints interesting non-junk strings.|
|-ni|Prints only not interesting strings. Default only prints interesting non-junk strings.|
|-e|Escape new line characters.|
|-l [num_chars]|Minimum number of characters that is a valid string. Default is 4.|
|-b [start]\(:[end]\)|Scan only the specified byte range for strings. Optionally specify an end offset as well.|
|-pid [pid]|The strings from the process address space for the specified PID will be dumped. Use a '0x' prefix to specify a hex PID.|
|-system|Dumps strings from all accessible processes on the system. This takes awhile.|
|-json|Writes output as json. Many flags are ignored in this mode.|
