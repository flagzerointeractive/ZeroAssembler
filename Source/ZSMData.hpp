#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iomanip>
#include <cstdint>

namespace fs = std::filesystem;

#include "StringUtils.hpp"

using namespace std;

static int ProgramMaxSize = 0x01000000;
static int ProgramStartAddress = 0x00002864;

class ZSMFunction{
    public:
        int ID;
        int MemoryOffset = 0;
        string Label;
        vector<string> Lines;
        vector<uint64_t> Program;
};

class ZSMFile {
    int Depth;
    vector<string> Lines;
};

class ZSMProgram {
    public:
        vector<string> Lines;
};

class ZSMDefinition {
    public:
        string Label;
        string Value;
};

class ZSMCore{
    public:
        bool GenDebugSymbols = false;
        int MaxMemoryOffset = 0;

        vector<uint64_t> FinalProgram;

        vector<ZSMFunction> ZSMFunctions;
        vector<ZSMDefinition> ZSMDefinitions;

        ZSMProgram MasterProgram;

        vector<ZSMFile> ZSMFiles;
};

static ZSMCore ZSMCoreData;