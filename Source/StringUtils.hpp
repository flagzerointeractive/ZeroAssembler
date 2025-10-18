#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

bool IsZSMFile(string Filename) {
    string Extension = ".zsm";
    if (Filename.size() >= Extension.size()) {
        return Filename.compare(Filename.size() - Extension.size(), Extension.size(), Extension) == 0;
    }
    return false;
}

vector<uint64_t> StringToWords(const string& Str) {
    vector<uint64_t> Words(8, 0);
    size_t Len = min(Str.size(), size_t(64));
    memcpy(Words.data(), Str.data(), Len);
    return Words;
}

string CleanString(string Input) {
    string Result = "";
    bool LastWasSpace = true;
    for (int i = 0; i < Input.length(); i++) {
        char C = Input[i];
        if (C == ' ') {
            if (!LastWasSpace) {
                Result += C;
                LastWasSpace = true;
            }
        } else {
            Result += C;
            LastWasSpace = false;
        }
    }
    if (Result.length() > 0 && Result[Result.length() - 1] == ' ') {
        Result = Result.substr(0, Result.length() - 1);
    }
    return Result;
}

string RemoveComments(string Input) {
    string Result = "";
    for (int i = 0; i < Input.length(); i++) {
        if (Input[i] == '#') break;
        Result += Input[i];
    }
    return Result;
}

string StripCommas(const string& Line) {
    string Result;
    Result.reserve(Line.size());
    for (char C : Line) {
        if (C != ',') Result += C;
    }
    return Result;
}

vector<string> LoadFileLines(const string& FilePath) {
    vector<string> Lines;
    ifstream File(FilePath);
    if (!File.is_open()) return Lines;
    string Line;
    while (getline(File, Line)) Lines.push_back(Line);
    return Lines;
}

string LoadFile(const char* Path) {
    ifstream InFile(Path);
    if (!InFile.is_open()) return {};
    ostringstream Buffer;
    Buffer << InFile.rdbuf();
    return Buffer.str();
}

string ReadValue(const string& Data, int Start, int End) {
    string Out;
    for (int i = Start; i <= End; i++) Out += Data[i];
    return Out;
}

vector<string> SplitString(const string& Data, char Delimiter = ':') {
    vector<string> Parts;
    stringstream SS(Data);
    string Item;
    while (getline(SS, Item, Delimiter)) Parts.push_back(Item);
    return Parts;
}

string SplitValue(const string& Data, int Index, char Delimiter = ' ') {
    vector<string> Parts = SplitString(Data, Delimiter);
    if (Index >= 0 && Index < static_cast<int>(Parts.size())) return Parts[Index];
    return "";
}

int SplitSize(const string& Data) {
    int SplitCount = 0;
    for (int i = 0; i <= Data.size(); i++) {
        if (Data[i] == ' ') SplitCount++;
    }
    return SplitCount;
}

bool IsNumber(const string& S) {
    if (S.empty()) return false;
    size_t Start = 0;
    if (S[0] == '-' || S[0] == '+') {
        if (S.length() == 1) return false;
        Start = 1;
    }
    if (Start + 2 <= S.length() && S[Start] == '0' && (S[Start + 1] == 'x' || S[Start + 1] == 'X')) {
        if (S.length() <= Start + 2) return false;
        for (size_t i = Start + 2; i < S.length(); i++) {
            if (!isxdigit(static_cast<unsigned char>(S[i]))) return false;
        }
        return true;
    }
    if (Start + 2 <= S.length() && S[Start] == '0' && (S[Start + 1] == 'b' || S[Start + 1] == 'B')) {
        if (S.length() <= Start + 2) return false;
        for (size_t i = Start + 2; i < S.length(); i++) {
            if (S[i] != '0' && S[i] != '1') return false;
        }
        return true;
    }
    if (Start + 2 <= S.length() && S[Start] == '0' && (S[Start + 1] == 'o' || S[Start + 1] == 'O')) {
        if (S.length() <= Start + 2) return false;
        for (size_t i = Start + 2; i < S.length(); i++) {
            if (S[i] < '0' || S[i] > '7') return false;
        }
        return true;
    }
    bool HasDecimal = false;
    bool HasExponent = false;
    for (size_t i = Start; i < S.length(); i++) {
        char C = S[i];
        if (isdigit(static_cast<unsigned char>(C))) continue;
        else if (C == '.' && !HasDecimal && !HasExponent) {
            HasDecimal = true;
            if (i == Start && i + 1 >= S.length()) return false;
            if (i > Start && i + 1 < S.length()) continue;
            if (i == Start && i + 1 < S.length() && isdigit(static_cast<unsigned char>(S[i + 1]))) continue;
            if (i + 1 >= S.length() && i > Start && isdigit(static_cast<unsigned char>(S[i - 1]))) continue;
            return false;
        }
        else if ((C == 'e' || C == 'E') && !HasExponent && i > Start) {
            HasExponent = true;
            if (i + 1 >= S.length()) return false;
            if (S[i + 1] == '+' || S[i + 1] == '-') {
                i++;
                if (i + 1 >= S.length()) return false;
            }
            continue;
        }
        else return false;
    }
    return true;
}

int StringToInt(const string& Data) {
    int Out = 0;
    int Place = 1;
    int I = Data.size() - 1;
    while (I >= 0) {
        char C = Data[I];
        if (C == '-') Out = -Out;
        else if (C >= '0' && C <= '9') {
            int Digit = C - '0';
            Out += Digit * Place;
            Place *= 10;
        }
        I--;
    }
    return Out;
}

float StringToFloat(const string& Data) {
    float Out = 0.0f;
    float Place = 1.0f;
    int I = Data.size() - 1;
    bool IsNegative = false;
    while (I >= 0) {
        char C = Data[I];
        if (C == '-') IsNegative = true;
        else if (C == '.') {
            Out /= Place;
            Place = 1.0f;
        } else if (C >= '0' && C <= '9') {
            int Digit = C - '0';
            Out += Digit * Place;
            Place *= 10.0f;
        }
        I--;
    }
    return IsNegative ? -Out : Out;
}
