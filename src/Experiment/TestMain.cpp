//
// Created by yche on 11/2/2015.
//

#include "TestMain.h"
#include "stdlib.h"
#include "direct.h"
#include "string.h"
#include "string"

#include "stdio.h"
#include <vector>
#include "iostream"
using namespace std;

class CBrowseDir
{
protected:
    //?????????????'\'??
    char m_szInitDir[_MAX_PATH];

public:
    //?????
    CBrowseDir();

    //???????dir?????false????????
    bool SetInitDir(const char *dir);

    //???????????????filespec???????
    //filespec??????? * ?????????
    //????false????????????
    bool BeginBrowse(const char *filespec);
    vector<string> BeginBrowseFilenames(const char *filespec);

protected:
    //????dir??filespec?????
    //?????,???????
    //????false,????????
    bool BrowseDir(const char *dir,const char *filespec);
    vector<string> GetDirFilenames(const char *dir,const char *filespec);
    //??BrowseDir???????,???ProcessFile
    //?????????????
    //????false,????????
    //?????????,?????????
    virtual bool ProcessFile(const char *filename);

    //??BrowseDir???????,???ProcessDir
    //?????????????????????????
    //????????????,?parentdir=NULL
    //?????????,?????????
    //?????????????????
    virtual void ProcessDir(const char *currentdir,const char *parentdir);
};

CBrowseDir::CBrowseDir()
{
    //????????m_szInitDir
    getcwd(m_szInitDir,_MAX_PATH);

    //?????????????'\',????????'\'
    int len=strlen(m_szInitDir);
    if (m_szInitDir[len-1] != '\\')
        strcat(m_szInitDir,"\\");
}

bool CBrowseDir::SetInitDir(const char *dir)
{
    //??dir???????
    if (_fullpath(m_szInitDir,dir,_MAX_PATH) == NULL)
        return false;

    //????????
    if (_chdir(m_szInitDir) != 0)
        return false;

    //?????????????'\',????????'\'
    int len=strlen(m_szInitDir);
    if (m_szInitDir[len-1] != '\\')
        strcat(m_szInitDir,"\\");

    return true;
}

vector<string> CBrowseDir::BeginBrowseFilenames(const char *filespec)
{
    ProcessDir(m_szInitDir,NULL);
    return GetDirFilenames(m_szInitDir,filespec);
}

bool CBrowseDir::BeginBrowse(const char *filespec)
{
    ProcessDir(m_szInitDir,NULL);
    return BrowseDir(m_szInitDir,filespec);
}

bool CBrowseDir::BrowseDir(const char *dir,const char *filespec)
{
    _chdir(dir);

    //????dir????????
    long hFile;
    _finddata_t fileinfo;
    if ((hFile=_findfirst(filespec,&fileinfo)) != -1)
    {
        do
        {
            //???????
            //????,?????
            if (!(fileinfo.attrib & _A_SUBDIR))
            {
                char filename[_MAX_PATH];
                strcpy(filename,dir);
                strcat(filename,fileinfo.name);
                cout << filename << endl;
                if (!ProcessFile(filename))
                    return false;
            }
        } while (_findnext(hFile,&fileinfo) == 0);
        _findclose(hFile);
    }
    //??dir?????
    //?????dir??????????ProcessFile??????
    //??????????????????dir?
    //???_findfirst?????????????????????
    //?_findnext?????
    _chdir(dir);
    if ((hFile=_findfirst("*.*",&fileinfo)) != -1)
    {
        do
        {
            //???????
            //???,?????? . ? ..
            //????,????
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name,".") != 0 && strcmp
                                                              (fileinfo.name,"..") != 0)
                {
                    char subdir[_MAX_PATH];
                    strcpy(subdir,dir);
                    strcat(subdir,fileinfo.name);
                    strcat(subdir,"\\");
                    ProcessDir(subdir,dir);
                    if (!BrowseDir(subdir,filespec))
                        return false;
                }
            }
        } while (_findnext(hFile,&fileinfo) == 0);
        _findclose(hFile);
    }
    return true;
}

vector<string> CBrowseDir::GetDirFilenames(const char *dir,const char *filespec)
{
    _chdir(dir);
    vector<string>filename_vector;
    filename_vector.clear();

    //????dir????????
    long hFile;
    _finddata_t fileinfo;
    if ((hFile=_findfirst(filespec,&fileinfo)) != -1)
    {
        do
        {
            //???????
            //????,?????
            if (!(fileinfo.attrib & _A_SUBDIR))
            {
                char filename[_MAX_PATH];
                strcpy(filename,dir);
                strcat(filename,fileinfo.name);
                filename_vector.push_back(filename);
            }
        } while (_findnext(hFile,&fileinfo) == 0);
        _findclose(hFile);
    }
    //??dir?????
    //?????dir??????????ProcessFile??????
    //??????????????????dir?
    //???_findfirst?????????????????????
    //?_findnext?????
    _chdir(dir);
    if ((hFile=_findfirst("*.*",&fileinfo)) != -1)
    {
        do
        {
            //???????
            //???,?????? . ? ..
            //????,????
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name,".") != 0 && strcmp
                                                              (fileinfo.name,"..") != 0)
                {
                    char subdir[_MAX_PATH];
                    strcpy(subdir,dir);
                    strcat(subdir,fileinfo.name);
                    strcat(subdir,"\\");
                    ProcessDir(subdir,dir);
                    vector<string>tmp= GetDirFilenames(subdir,filespec);
                    for (vector<string>::iterator it=tmp.begin();it<tmp.end();it++)
                    {
                        filename_vector.push_back(*it);
                    }
                }
            }
        } while (_findnext(hFile,&fileinfo) == 0);
        _findclose(hFile);
    }
    return filename_vector;
}

bool CBrowseDir::ProcessFile(const char *filename)
{
    return true;
}

void CBrowseDir::ProcessDir(const char
                            *currentdir,const char *parentdir)
{
}

//?CBrowseDir???????????????????????
class CStatDir:public CBrowseDir
{
protected:
    int m_nFileCount;   //??????
    int m_nSubdirCount; //???????

public:
    //?????
    CStatDir()
    {
        //???????m_nFileCount?m_nSubdirCount
        m_nFileCount=m_nSubdirCount=0;
    }

    //??????
    int GetFileCount()
    {
        return m_nFileCount;
    }

    //???????
    int GetSubdirCount()
    {
        //????????????????ProcessDir?
        //???1????????????
        return m_nSubdirCount-1;
    }

protected:
    //?????ProcessFile????????????1
    virtual bool ProcessFile(const char *filename)
    {
        m_nFileCount++;
        return CBrowseDir::ProcessFile(filename);
    }

    //?????ProcessDir?????????????1
    virtual void ProcessDir
            (const char *currentdir,const char *parentdir)
    {
        m_nSubdirCount++;
        CBrowseDir::ProcessDir(currentdir,parentdir);
    }
};

void main()
{
    //?????
    char buf[256];
    printf("??????????:");
    gets(buf);

    //?????
    CStatDir statdir;

    //????????
    if (!statdir.SetInitDir(buf))
    {
        puts("??????");
        return;
    }

    //????

    vector<string>file_vec = statdir.BeginBrowseFilenames("*.*");
    for(vector<string>::const_iterator it = file_vec.begin(); it < file_vec.end(); ++it)
        std::cout<<*it<<std::endl;

    printf("????: %d\n",file_vec.size());
    system("pause");
}