#include "ZSMAssembler.hpp"

struct FilePath {
    int Depth;
    string Path;
    int ProcessedIncludes = 0;
};

void CreateMasterProgram(const char *InitialFile){
    vector<FilePath> FilePaths;
    int DepthCounter = 0;

    FilePath IFFilePath;
    IFFilePath.Depth = DepthCounter;
    IFFilePath.Path = InitialFile;
    IFFilePath.ProcessedIncludes = 0;

    FilePaths.push_back(IFFilePath);
    DepthCounter++;

    bool Scanning = true;
    int Generation = 0;

    while (Scanning){
        bool FoundNewFile = false;
        
        for (int i = 0; i < FilePaths.size(); i++){
            vector<string> NFLines = LoadFileLines(FilePaths.at(i).Path);
            
            int TotalIncludes = 0;

            for (int x = 0; x < NFLines.size(); x++){
                if (SplitValue(NFLines.at(x), 0) == ".include"){
                    TotalIncludes++;
                }
            }
            
            if (FilePaths.at(i).ProcessedIncludes < TotalIncludes){
                FoundNewFile = true;
                int IncludeCounter = 0;

                for (int x = 0; x < NFLines.size(); x++){
                    string Line = NFLines.at(x);

                    if (SplitValue(Line, 0) == ".include"){
                        if (IncludeCounter == FilePaths.at(i).ProcessedIncludes){
                            FilePath NFilePath;
                            NFilePath.Depth = DepthCounter;

                            string IncludeName = SplitValue(Line, 1);
                            fs::path ParentDir = fs::path(FilePaths.at(i).Path).parent_path();
                            fs::path ResolvedPath = fs::weakly_canonical(ParentDir / IncludeName);
                            NFilePath.Path = ResolvedPath.string();

                            NFilePath.ProcessedIncludes = 0;
                            FilePaths.push_back(NFilePath);
                            DepthCounter++;
                            
                            FilePaths.at(i).ProcessedIncludes++;
                            break;
                        }
                        IncludeCounter++;
                    }
                }
                
                break; 
            }
        }

        Generation++;

        if (FoundNewFile == false){
            Scanning = false;
        }
    }

    // Create Master Program 

    int TopDepth = 0;

    for (int i = 0; i < FilePaths.size(); i++){
        if (FilePaths.at(i).Depth > TopDepth){
            TopDepth = FilePaths.at(i).Depth;
        }
    }

    for (int i = TopDepth; i > -1; i--){
        int FPIndex = 0;

        for (int x = 0; x < FilePaths.size(); x++){
            if (FilePaths.at(x).Depth == i){
                FPIndex = x;
            }
        }

        vector<string> NFLines = LoadFileLines(FilePaths.at(FPIndex).Path);

        for (int x = 0; x < NFLines.size(); x++){
            string NLine = NFLines.at(x);

            NLine = CleanString(NLine);
            NLine = RemoveComments(NLine);
            NLine = StripCommas(NLine);

            bool WriteLine = true;

            if (SplitValue(NLine, 0) == ".include"){
                WriteLine = false;
            }

            if (SplitValue(NLine, 0) == ".define"){
                ZSMDefinition NZSMDefinition;
                NZSMDefinition.Label = SplitValue(NLine, 1);
                NZSMDefinition.Value = SplitValue(NLine, 2);
                ZSMCoreData.ZSMDefinitions.push_back(NZSMDefinition);
                WriteLine = false;
            }

            if (NLine.empty()){
                WriteLine = false;
            }

            if (WriteLine){
                ZSMCoreData.MasterProgram.Lines.push_back(NLine);
            }
        }
    }
}

void CreateFunctionTable(){
    int FunctionIndex = 0;

    for (int i = 0; i < ZSMCoreData.MasterProgram.Lines.size(); i ++){
        string Line = ZSMCoreData.MasterProgram.Lines.at(i);
        bool IsIntruction = true;
        
        for (int x = 0; x < Line.size(); x++){
            if (Line[x] == ':'){
                if (Line.size() > 64){
                    cout << "ERROR LABEL TOO LARGE: " << ReadValue(Line, 0, Line.size() - 2) << "\n";
                    exit(0);
                }
                else {
                    ZSMFunction NZSMFunction;
                    NZSMFunction.Label = ReadValue(Line, 0, Line.size() - 2);
                    NZSMFunction.ID = FunctionIndex;
                    ZSMCoreData.ZSMFunctions.push_back(NZSMFunction);
                    IsIntruction = false;
                }
            }
        }


        if (IsIntruction){
            ZSMCoreData.ZSMFunctions.at(FunctionIndex).Lines.push_back(Line);
        }

        if (SplitValue(Line, 0) == "ret"){
            FunctionIndex ++;
        }
    }
}

void GenerateDebugSymbols(){
    for (int i = 0; i < ZSMCoreData.ZSMFunctions.size(); i++){
        vector<uint64_t> Label = StringToWords(ZSMCoreData.ZSMFunctions.at(i).Label);

        for (int x = 0; x < Label.size(); x ++){
            ZSMCoreData.FinalProgram.push_back(Label.at(x));
        }

        ZSMCoreData.FinalProgram.push_back(ZSMCoreData.ZSMFunctions.at(i).MemoryOffset + ProgramStartAddress);
    }
}

void FixLabels(){

    vector<int> MarkersBefore(ZSMCoreData.FinalProgram.size(), 0);
    int MarkerCount = 0;

    for (int i = 0; i < ZSMCoreData.FinalProgram.size(); i++){
        MarkersBefore[i] = MarkerCount;
        if (ZSMCoreData.FinalProgram[i] == -1){
            MarkerCount++; 
        }
    }
    
    for (int i = ZSMCoreData.FinalProgram.size() - 1; i >= 0; i--){
        if (ZSMCoreData.FinalProgram[i] == -1){
            int MemoryOffset = 0;
            for (int x = 0; x < ZSMCoreData.ZSMFunctions.size(); x++){
                if (ZSMCoreData.ZSMFunctions[x].ID == ZSMCoreData.FinalProgram[i + 1]){
                    int OriginalOffset = ZSMCoreData.ZSMFunctions[x].MemoryOffset;
                    int AdjustedOffset = OriginalOffset - MarkersBefore[OriginalOffset];
                    MemoryOffset = AdjustedOffset + ProgramStartAddress;
                    break;
                }
            }

            ZSMCoreData.FinalProgram[i] = MemoryOffset;
            ZSMCoreData.FinalProgram.erase(ZSMCoreData.FinalProgram.begin() + i + 1);
        }
    }
}
void CreateBinary(const char *OutFile){
    cout << "SIZING BINARY\n";

    int BinarySize = 0;

    for (int x = 0; x < ZSMCoreData.ZSMFunctions.size(); x++){
        BinarySize += ZSMCoreData.ZSMFunctions.at(x).Program.size();
    }

    for (int i = 0; i < BinarySize; i++){
        ZSMCoreData.FinalProgram.push_back(0);
    }

    cout << "WRITING FUNCTIONS\n";

    int MemoryOffset = 0;

    for (int x = 0; x < ZSMCoreData.ZSMFunctions.size(); x++){
        if (ZSMCoreData.ZSMFunctions.at(x).Label == "PRG_MAIN"){
            ZSMCoreData.ZSMFunctions.at(x).MemoryOffset = MemoryOffset;

            for (int i = 0; i < ZSMCoreData.ZSMFunctions.at(x).Program.size(); i ++){
                ZSMCoreData.FinalProgram.at(MemoryOffset) = ZSMCoreData.ZSMFunctions.at(x).Program.at(i);
                MemoryOffset ++;
            }
        }
    }

    for (int x = 0; x < ZSMCoreData.ZSMFunctions.size(); x++){
        if (ZSMCoreData.ZSMFunctions.at(x).Label != "PRG_MAIN"){
            ZSMCoreData.ZSMFunctions.at(x).MemoryOffset = MemoryOffset;
            for (int i = 0; i < ZSMCoreData.ZSMFunctions.at(x).Program.size(); i ++){
                ZSMCoreData.FinalProgram.at(MemoryOffset) = ZSMCoreData.ZSMFunctions.at(x).Program.at(i);
                MemoryOffset ++;
            }
        }
    }

    cout << "FIXING JUMPS\n";

    FixLabels();

    if (ZSMCoreData.GenDebugSymbols){
        cout << "WRITING DEBUG SYMBOLS\n";
        GenerateDebugSymbols();
    }

    std::ofstream outFile(OutFile, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(ZSMCoreData.FinalProgram.data()), ZSMCoreData.FinalProgram.size() * sizeof(uint64_t));
    outFile.close();
}

