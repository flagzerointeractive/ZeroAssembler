#include "ZSMData.hpp"

int MatchRegister(const string& Reg) {
    if (Reg.size() < 2 || Reg[0] != 'r') return -1; 

    string NumPart = Reg.substr(1);
    for (char C : NumPart) {
        if (!isdigit(C)) return -1;
    }

    int RegNum = stoi(NumPart);
    if (RegNum < 0 || RegNum > 31) return -1;

    return RegNum;
}

uint64_t NormalizeAddress(string Address) {
    if (Address.empty()) {
        cout << "\033[31mERROR: Empty address string\033[0m\n";
        return 0;
    }
    
    if (Address.rfind("0b", 0) == 0 || Address.rfind("0B", 0) == 0) {
        if (Address.length() <= 2) {
            cout << "\033[31mERROR: Invalid binary address format\033[0m\n";
            return 0;
        }
        return stoull(Address.substr(2), nullptr, 2);
    }
    else if (Address.rfind("0x", 0) == 0 || Address.rfind("0X", 0) == 0) {
        if (Address.length() <= 2) {
            cout << "\033[31mERROR: Invalid hex address format\033[0m\n";
            return 0;
        }
        return stoull(Address.substr(2), nullptr, 16);
    }
    else {
        return stoull(Address, nullptr, 10);
    }
}

uint64_t StringToBinaryBits(const string &Input) {
    if (Input.empty()) {
        cout << "\033[31mERROR: Empty value string\033[0m\n";
        return 0;
    }
    
    string S = Input;

    // HEX (starts with 0x)
    if (S.rfind("0x", 0) == 0 || S.rfind("0X", 0) == 0) {
        if (S.length() <= 2) {
            cout << "\033[31mERROR: Invalid hex format\033[0m\n";
            return 0;
        }
        uint64_t Value = stoull(S, nullptr, 16);
        return Value;
    }

    // BINARY (starts with 0b)
    if (S.rfind("0b", 0) == 0 || S.rfind("0B", 0) == 0) {
        if (S.length() <= 2) {
            cout << "\033[31mERROR: Invalid binary format\033[0m\n";
            return 0;
        }
        return stoull(S.substr(2), nullptr, 2);
    }

    // FLOAT (ends with 'f')
    if (!S.empty() && S.back() == 'f') {
        S.pop_back();
        if (S.empty()) {
            cout << "\033[31mERROR: Invalid float format\033[0m\n";
            return 0;
        }
        float F = stof(S);
        uint32_t Bits;
        memcpy(&Bits, &F, sizeof(Bits));
        return static_cast<uint64_t>(Bits);
    }

    // DOUBLE (contains '.')
    if (S.find('.') != string::npos) {
        double D = stod(S);
        uint64_t Bits;
        memcpy(&Bits, &D, sizeof(Bits));
        return Bits;
    }

    // INTEGER (default)
    return stoull(S, nullptr, 10);
}

uint64_t PackBytes(uint8_t B0, uint8_t B1, uint8_t B2, uint8_t B3,
                   uint8_t B4, uint8_t B5, uint8_t B6, uint8_t B7) {
    uint64_t Result = 0;

    Result |= (static_cast<uint64_t>(B0) << 56);
    Result |= (static_cast<uint64_t>(B1) << 48);
    Result |= (static_cast<uint64_t>(B2) << 40);
    Result |= (static_cast<uint64_t>(B3) << 32);
    Result |= (static_cast<uint64_t>(B4) << 24);
    Result |= (static_cast<uint64_t>(B5) << 16);
    Result |= (static_cast<uint64_t>(B6) << 8);
    Result |= (static_cast<uint64_t>(B7) << 0);

    return Result;
}

bool ValidateOperandCount(const string& Instruction, const string& Line, int Expected, int LineNumber, const string& FunctionLabel) {
    int Actual = SplitSize(Line);
    if (Actual != Expected) {
        cout << "\033[31mERROR: Instruction '\033[32m" << Instruction 
             << "\033[31m' expects " << Expected << " operands but got " << Actual 
             << " in function '\033[35;32m" << FunctionLabel << "\033[31m' at line " << LineNumber << "\n";
        cout << "    \033[1;31m" << Line << "\033[0m\n";
        return false;
    }
    return true;
}

bool ValidateRegister(const string& RegStr, const string& Instruction, const string& Line, int LineNumber, const string& FunctionLabel) {
    int RegNum = MatchRegister(RegStr);
    if (RegNum == -1) {
        cout << "\033[31mERROR: Invalid register '\033[33m" << RegStr 
             << "\033[31m' in instruction '\033[32m" << Instruction 
             << "\033[31m' in function '\033[35;32m" << FunctionLabel 
             << "\033[31m' at line " << LineNumber << "\n";
        cout << "    \033[1;31m" << Line << "\033[0m\n";
        cout << "    Expected register format: r0-r31\n";
        return false;
    }
    return true;
}

bool ValidateBank(const string& BankStr, const string& Instruction, const string& Line, int LineNumber, const string& FunctionLabel) {
    if (!IsNumber(BankStr)) {
        cout << "\033[31mERROR: Invalid bank number '\033[33m" << BankStr 
             << "\033[31m' in instruction '\033[32m" << Instruction 
             << "\033[31m' in function '\033[35;32m" << FunctionLabel 
             << "\033[31m' at line " << LineNumber << "\n";
        cout << "    \033[1;31m" << Line << "\033[0m\n";
        return false;
    }
    
    int BankNum = StringToInt(BankStr);
    if (BankNum < 0 || BankNum > 255) {
        cout << "\033[31mERROR: Bank number " << BankNum 
             << " out of range (0-255) in instruction '\033[32m" << Instruction 
             << "\033[31m' in function '\033[35;32m" << FunctionLabel 
             << "\033[31m' at line " << LineNumber << "\n";
        cout << "    \033[1;31m" << Line << "\033[0m\n";
        return false;
    }
    return true;
}

void AssembleFunctions(){
    for (int i = 0; i < ZSMCoreData.ZSMFunctions.size(); i ++){
        for (int x = 0; x < ZSMCoreData.ZSMFunctions.at(i).Lines.size(); x ++){
            string Line = ZSMCoreData.ZSMFunctions.at(i).Lines.at(x);
            string Instruction = SplitValue(Line, 0);

            bool FoundInstruction = false;

            string FormattedInstruction;
            bool InstructionChanged = false;

            for (int y = 0; y < SplitSize(Line) + 1; y++) {
                string LToken = SplitValue(Line, y);
                bool Matched = false;

                for (int z = 0; z < ZSMCoreData.ZSMDefinitions.size(); z++) {
                    if (LToken == ZSMCoreData.ZSMDefinitions.at(z).Label) {
                        FormattedInstruction += ZSMCoreData.ZSMDefinitions.at(z).Value + " ";
                        InstructionChanged = true;
                        Matched = true;
                        break;
                    }
                }

                if (!Matched) {
                    FormattedInstruction += LToken + " ";
                }
            }

            Line = FormattedInstruction;

            if (Instruction == "ldr"){
                // Example ldr r0, 255

                // First Word
                uint8_t OPCODE = 40;
                uint8_t REG = MatchRegister(SplitValue(Line, 1));

                // Second Word 
                uint64_t VALUE = StringToBinaryBits(SplitValue(Line, 2));

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, REG, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(VALUE);

                FoundInstruction = true;
            }

            if (Instruction == "lda"){
                // Example lda 0, r0, 0x000001

                // First Word
                uint8_t OPCODE = 12;
                uint8_t BANK = StringToInt(SplitValue(Line, 1));
                uint8_t REG = MatchRegister(SplitValue(Line, 2));

                // Second Word
                uint64_t ADDRESS = StringToBinaryBits(SplitValue(Line, 3));

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, BANK, REG, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                FoundInstruction = true;
            }

            if (Instruction == "ldar"){
                // Example ldar 0, r0, r1

                // First Word
                uint8_t OPCODE = 19;
                uint8_t BANK = StringToInt(SplitValue(Line, 1));
                uint8_t REG1 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 3));

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, BANK, REG1, REG2, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "sta"){
                // Example sta 0, r0, 0x000001

                // First Word 
                uint8_t OPCODE = 13;
                uint8_t BANK = StringToInt(SplitValue(Line, 1));
                uint8_t REG = MatchRegister(SplitValue(Line, 2));

                // Second Word
                uint64_t ADDRESS = NormalizeAddress(SplitValue(Line, 3));

                uint64_t INSTURCTION = PackBytes(OPCODE, BANK, REG, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                FoundInstruction = true;
            }

            if (Instruction == "star"){
                // Example star 0, r0, r1

                // First Word 
                uint8_t OPCODE = 20;
                uint8_t BANK = StringToInt(SplitValue(Line, 1));
                uint8_t REG1 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 3));

                uint64_t INSTURCTION = PackBytes(OPCODE, BANK, REG1, REG2, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "cpr"){
                // Example cpr r1, r2

                // First Word 
                uint8_t OPCODE = 23;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "swapr"){
                // Example swapr r1, r2

                // First Word 
                uint8_t OPCODE = 27;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "fconv"){
                // Example fconv r1

                // First Word 
                uint8_t OPCODE = 28;
                uint8_t REG = MatchRegister(SplitValue(Line, 1));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "iconv"){
                // Example iconv r1

                // First Word 
                uint8_t OPCODE = 29;
                uint8_t REG = MatchRegister(SplitValue(Line, 1));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "pushr"){
                // Example pushr r1

                // First Word 
                uint8_t OPCODE = 14;
                uint8_t REG = MatchRegister(SplitValue(Line, 1));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "pullr"){
                // Example pullr r1

                // First Word 
                uint8_t OPCODE = 15;
                uint8_t REG = MatchRegister(SplitValue(Line, 1));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "pop"){
                // Example pop

                // First Word 
                uint8_t OPCODE = 16;

                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "iaddr"){
                // Example iaddr r1, r2

                // First Word 
                uint8_t OPCODE = 17;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "isubr"){
                // Example isubr r1, r2

                // First Word 
                uint8_t OPCODE = 18;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "imulr"){
                // Example imulr r1, r2

                // First Word 
                uint8_t OPCODE = 57;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "idivr"){
                // Example idivr r1, r2

                // First Word 
                uint8_t OPCODE = 21;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "faddr"){
                // Example faddr r1, r2

                // First Word 
                uint8_t OPCODE = 22;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "fsubr"){
                // Example fsubr r1, r2

                // First Word 
                uint8_t OPCODE = 24;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "fmulr"){
                // Example fmulr r1, r2

                // First Word 
                uint8_t OPCODE = 25;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "fdivr"){
                // Example fdivr r1, r2

                // First Word 
                uint8_t OPCODE = 26;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "ivaddr"){
                // Example ivaddr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 30;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "ivsubr"){
                // Example ivsubr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 31;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "ivmulr"){
                // Example ivmulr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 32;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "ivdivr"){
                // Example ivdivr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 33;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "fvaddr"){
                // Example fvaddr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 34;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "fvsubr"){
                // Example fvsubr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 35;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "fvmulr"){
                // Example fvmulr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 36;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "fvdivr"){
                // Example fvdivr r1, r2, r3, r4, r5, r6

                // First Word 
                uint8_t OPCODE = 37;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 3));
                uint8_t REG4 = MatchRegister(SplitValue(Line, 4));
                uint8_t REG5 = MatchRegister(SplitValue(Line, 5));
                uint8_t REG6 = MatchRegister(SplitValue(Line, 6));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, REG4, REG5, REG6, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "jmp"){
                // Example jmp 0x0000001

                // First Word
                uint8_t OPCODE = 38;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }
            if (Instruction == "jmpr"){
                // Example jmpr r1

                // First Word 
                uint8_t OPCODE = 39;
                uint8_t REG = MatchRegister(SplitValue(Line, 1));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "ret"){
                // Example ret
                
                // First Word 
                uint8_t OPCODE = 41;

                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "call"){
                // Example call 0x0000001

                // First Word
                uint8_t OPCODE = 42;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }

            if (Instruction == "cmpr"){
                // Example cmpr r1, r2

                uint8_t OPCODE = 43;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                FoundInstruction = true;
            }

            if (Instruction == "bie"){
                // Example bie 0x0000001

                // First Word
                uint8_t OPCODE = 44;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }

            if (Instruction == "bin"){
                // Example bin 0x0000001

                // First Word
                uint8_t OPCODE = 45;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }

            if (Instruction == "big"){
                // Example big 0x0000001

                // First Word
                uint8_t OPCODE = 46;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }

            if (Instruction == "bil"){
                // Example bil 0x0000001

                // First Word
                uint8_t OPCODE = 47;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }
            if (Instruction == "bige"){
                // Example bige 0x0000001

                // First Word
                uint8_t OPCODE = 48;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }
            if (Instruction == "bile"){
                // Example bile 0x0000001

                // First Word
                uint8_t OPCODE = 49;
                uint64_t ADDRESS = 0;

                if (IsNumber(SplitValue(Line, 1))){
                    ADDRESS = StringToBinaryBits(SplitValue(Line, 1));
                }
                else {
                    ADDRESS = -1;
                }

                // Encoding 
                uint64_t INSTURCTION = PackBytes(OPCODE, 0, 0, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);
                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(ADDRESS);

                int FunctionID = -1;

                if (ADDRESS == -1){

                    for (int y = 0; y < ZSMCoreData.ZSMFunctions.size(); y++){
                        if (ZSMCoreData.ZSMFunctions.at(y).Label == SplitValue(Line, 1)){
                            FunctionID = ZSMCoreData.ZSMFunctions.at(y).ID;
                        }
                    }

                    if (FunctionID == -1){
                        cout << "ERROR LABEL " << SplitValue(Line, 1) << " NOT FOUND\n";
                    }
                    else {
                        ZSMCoreData.ZSMFunctions.at(i).Program.push_back(FunctionID);
                    }
                }

                FoundInstruction = true;
            }

            if (Instruction == "andr"){
                // Example andr r1, r2, r3

                uint8_t OPCODE = 50;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "orr"){
                // Example orr r1, r2, r3

                uint8_t OPCODE = 51;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }

            if (Instruction == "xorr"){
                // Example xorr r1, r2, r3

                uint8_t OPCODE = 52;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "notr"){
                // Example notr r1, r2

                uint8_t OPCODE = 53;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, 0, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "shlr"){
                // Example shlr r1, r2, r3

                uint8_t OPCODE = 54;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "shrr"){
                // Example shrr r1, r2, r3

                uint8_t OPCODE = 55;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (Instruction == "sarr"){
                // Example sarr r1, r2, r3

                uint8_t OPCODE = 56;
                uint8_t REG1 = MatchRegister(SplitValue(Line, 1));
                uint8_t REG2 = MatchRegister(SplitValue(Line, 2));
                uint8_t REG3 = MatchRegister(SplitValue(Line, 2));

                uint64_t INSTURCTION = PackBytes(OPCODE, REG1, REG2, REG3, 0, 0, 0, 0);

                ZSMCoreData.ZSMFunctions.at(i).Program.push_back(INSTURCTION);

                FoundInstruction = true;
            }
            if (!FoundInstruction){
                cout << "\033[31mERROR INSTRUCTION (\033[32m" << Instruction << "\033[31m) IN \033[35;32m" << ZSMCoreData.ZSMFunctions.at(i).Label << " \033[31mNOT FOUND. \n";
                cout << "    \033[1;31m" << Line << "\n";
                exit(0);
            }
        }
    }
}