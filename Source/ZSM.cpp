#include "ZSMCore.hpp"

int main(int argc, char* argv[]) {

    string BinaryName = "OUT.zx";
    string ZSMFile = "";

    for (int i = 0; i < argc; i ++){
        if (strcmp(argv[i], "-d") == 0){
            ZSMCoreData.GenDebugSymbols = true;
        }

        if (strcmp(argv[i], "-o") == 0){
            BinaryName = argv[i + 1];
        }

        if (IsZSMFile(argv[i])){
            ZSMFile = argv[i];
        }
    }

    if (ZSMFile.empty()){
        cout << "No ZSM File Found.\n";
        return 0;
    }
    else {
        CreateMasterProgram(ZSMFile.c_str());
        CreateFunctionTable();
        AssembleFunctions();
        CreateBinary(BinaryName.c_str());
    }
}