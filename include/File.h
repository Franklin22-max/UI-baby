#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED


#include "Administrator.h"
#include <string>
#include <commdlg.h>
#include "common.h"
#include <fstream>


class File
{
    std::string path;
    std::vector<std::string> lines;
public:
    File(std::string path)
    : path(path)
    {
        std::fstream file;
        std::stringstream ss;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // Open files
            file.open(path);
            std::stringstream vShaderStream, fShaderStream;
            // Read file’s buffer contents into streams
            ss << file.rdbuf();
            // close file handlers
            file.close();

            // Convert stream into GLchar array
            //vertexCode = vShaderStream.str();
            while(ss.peek() != EOF)
            {
                std::string linedata;
                std::getline(ss,linedata);
                lines.push_back(linedata.c_str());
            }
        }
        catch(std::ifstream::failure e)
        {
            Error::write_error("Couldn't open file");
        }
    }



    uint32_t getFileSize()
    {
        uint32_t size = 0;
        for(int i = 0; i < lines.size(); i++)
            size += lines[i].size();
        return size;
    }


    std::string readLine(uint32_t line)
    {
        line = line - 1;
        if(line >= 0 && line < lines.size())
            return lines[line];
        else
            return "";
    }



};



#ifdef WIN32// Functions below are windows specific

// Returns an empty string if dialog is canceled
inline std::string savefilename(char *filter = (char*)"All Files (*.*)\0*.*\0", HWND owner = Administrator::get_instance()->GET_WINDOW_HANDLER())
{
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = (LPCWSTR)filter;
    ofn.lpstrFile = (LPWSTR)fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = (LPCWSTR)"";

    std::string fileNameStr;

    if (GetSaveFileName(&ofn))
        fileNameStr = fileName;

    return fileNameStr;
}


// Returns an empty string if dialog is canceled
inline std::string openfilename(char* filter = (char*)"All Files (*.*)\0*.*\0", HWND owner = Administrator::get_instance()->GET_WINDOW_HANDLER())
{
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = (LPCWSTR)filter;
    ofn.lpstrFile = (LPWSTR)fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = (LPCWSTR)"";

    std::string fileNameStr;

    if( GetOpenFileName(&ofn) )
        fileNameStr = fileName;

    return fileNameStr;
}


/** \brief  Opens a Color chooser dialog and let user pick colors dynamically
    *
    * \return  SDL_Color  filled with color data
    * \note the alpha value for return color is always 255
    */

inline SDL_Color ChooseColor()
{
    CHOOSECOLOR cc;                 // common dialog box structure
    static COLORREF acrCustClr[16]; // array of custom colors
    //HBRUSH hbrush;                  // brush handle
    static DWORD rgbCurrent;        // initial color selection

    // Initialize CHOOSECOLOR
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner =  Administrator::get_instance()->GET_WINDOW_HANDLER();
    cc.lpCustColors = (LPDWORD) acrCustClr;
    cc.rgbResult = rgbCurrent;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc)==TRUE)
    {
        //hbrush = CreateSolidBrush(cc.rgbResult);
        rgbCurrent = cc.rgbResult;

        SDL_Color cl; cl.a = 255;/*(BYTE)(rgbCurrent >> 24);*/ cl.b = (BYTE)(rgbCurrent >> 16) & 255; cl.g = (BYTE)(rgbCurrent >> 8) & 255; cl.r = (BYTE)(rgbCurrent & 255);
        return cl;
    }
}

#endif



#endif // FILE_H_INCLUDED
