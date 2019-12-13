#include "SearchMVX.h"
#include <iostream>
#include <map>
#include <io.h>
using namespace std;

CSearchMVX::CSearchMVX()
{

}

CSearchMVX::~CSearchMVX()
{

}

int CSearchMVX::GetFilePaths(const std::string& strPath, std::vector<std::string>& vecFilePaths)
{
    if (strPath.size() == 0)
    {
        return -1;
    }

    int nResult = 0;
    nResult = getAllFiles(strPath, vecFilePaths);
    if (nResult != 0)
    {
        return nResult;
    }

    return 0;
}

int CSearchMVX::getAllFiles(string path, vector<string>& files)
{
    intptr_t hFile = 0;
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    if (CheckMVX(fileinfo.name) == true)
                    {
                        files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    }
                    getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
                }
            }
            else
            {
                if (CheckMVX(fileinfo.name) == true)
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                }
            }

        } while (_findnext(hFile, &fileinfo) == 0); 
    }

    _findclose(hFile);

    return 0;
}

bool CSearchMVX::CheckMVX(char* pFileName)
{
    if (pFileName == nullptr)
    {
        return false;
    }

    std::string strFileName = pFileName;
    int nStartPos = strFileName.rfind(".");
    if (nStartPos == -1)
    {
        return false;
    }

    std::string strPostFix = strFileName.substr(strFileName.rfind("."));

    if (strPostFix.find(".png") == std::string::npos)
    {
        return false;
    }

    return true;
}
