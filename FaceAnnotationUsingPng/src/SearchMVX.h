#pragma once
#include<vector>

using namespace std;
class CSearchMVX
{
public:
    CSearchMVX();
    ~CSearchMVX();
    int GetFilePaths(const std::string& strPath, std::vector<std::string>& vecFilePaths);

private:
    int getAllFiles(string path, vector<string>& files);
    bool CheckMVX(char* pFileName);
};

