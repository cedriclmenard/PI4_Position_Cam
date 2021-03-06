//
//  Views.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "Views.hpp"

static bool sdlIsInit = false;
static unsigned int currentNumberOfSDLOGLUsers = 0;

static GLuint texBGR = 0;
static bool texBGRisGen = false;

static GLuint texGray = 0;
static bool texGrayisGen = false;

static GLuint texSingle = 0;
static bool texSingleisGen = false;

void InitializeSDLOGLForViews() {
    if (~sdlIsInit) {
        // Setup SDL and OpenGL
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        }
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        
        
        glGenTextures(1, &texGray);
        glGenTextures(1, &texSingle);
        
        sdlIsInit = true;
    }
    currentNumberOfSDLOGLUsers++;
}

void QuitSDLOGLForViews() {
    currentNumberOfSDLOGLUsers--;
    if (sdlIsInit && currentNumberOfSDLOGLUsers == 0) {
        glDeleteTextures(1, &texBGR);
        glDeleteTextures(1, &texGray);
        SDL_Quit();
        sdlIsInit = false;
    }
}




// Grayscale
//static SDL_Color grayscalePalette[256];
//static bool grayscalePaletteBuilt = false;




GLuint bgrImageToTexture(unsigned char *bgr, unsigned int width, unsigned int height) {
    static unsigned int prevWidth = 0;
    static unsigned int prevHeight = 0;
    
    if (!texBGRisGen) {
        glGenTextures(1, &texBGR);
        texBGRisGen = true;
    }
    
    glBindTexture(GL_TEXTURE_2D, texBGR);
    if (prevWidth == width && prevHeight == height) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, bgr);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, bgr);
        prevHeight = height;
        prevWidth = width;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texBGR;
}

GLuint grayscaleImageToTexture(unsigned char *gray, unsigned int width, unsigned int height) {
    static unsigned int prevWidth = 0;
    static unsigned int prevHeight = 0;
    
    if (!texGrayisGen) {
        glGenTextures(1, &texGray);
        texGrayisGen = true;
    }
    
    glBindTexture(GL_TEXTURE_2D, texGray);
    if (prevWidth == width && prevHeight == height) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, gray);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gray);
        prevHeight = height;
        prevWidth = width;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texGray;
}

GLuint singleColorRGBAToTexture(uint32_t rgba) {
    
    if (!texSingleisGen) {
        glGenTextures(1, &texSingle);
        texSingleisGen = true;
    }
    
    glBindTexture(GL_TEXTURE_2D, texSingle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &rgba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    return texSingle;
}






// MARK: File access
/* Returns a list of files in a directory (except the ones that begin with a dot) */

void GetFilesFullNameInDirectory(std::vector<std::string> &out, const std::string &directory, const std::string fileExt)
{
#ifdef WINDOWS
    HANDLE dir;
    WIN32_FIND_DATA file_data;
    std::string searchStr;
    if (fileExt == "") {
        searchStr = "/*";
    } else {
        searchStr = "/*." + fileExt;
    }
    
    if ((dir = FindFirstFile((directory + searchStr).c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */
    
    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        
        if (file_name[0] == '.')
            continue;
        
        if (is_directory)
            continue;
        
        out.push_back(full_file_name);
    } while (FindNextFile(dir, &file_data));
    
    FindClose(dir);
#else
    DIR *dir;
    class dirent *ent;
    class stat st;
    
    dir = opendir(directory.c_str());
    while ((ent = readdir(dir)) != NULL) {
        const std::string file_name = ent->d_name;
        const std::string full_file_name = directory + "/" + file_name;
        
        if (file_name[0] == '.')
            continue;
        
        if (stat(full_file_name.c_str(), &st) == -1)
            continue;
        
        const bool is_directory = (st.st_mode & S_IFDIR) != 0;
        
        if (is_directory)
            continue;
        
        if (file_name.substr(file_name.length(),fileExt.length()) != fileExt)
            continue;
        
        out.push_back(full_file_name);
    }
    closedir(dir);
#endif
} // GetFilesInDirectory

void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory, const std::string fileExt)
{
#ifdef WINDOWS
    HANDLE dir;
    WIN32_FIND_DATA file_data;
    std::string searchStr;
    if (fileExt == "") {
        searchStr = "/*";
    } else {
        searchStr = "/*." + fileExt;
    }
    
    if ((dir = FindFirstFile((directory + searchStr).c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */
    
    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        
        if (file_name[0] == '.')
            continue;
        
        if (is_directory)
            continue;
        
        out.push_back(full_file_name);
    } while (FindNextFile(dir, &file_data));
    
    FindClose(dir);
#else
    DIR *dir;
    class dirent *ent;
    class stat st;
    
    dir = opendir(directory.c_str());
    while ((ent = readdir(dir)) != NULL) {
        const std::string file_name = ent->d_name;
        const std::string full_file_name = directory + "/" + file_name;
        
        if (file_name[0] == '.')
            continue;
        
        if (stat(full_file_name.c_str(), &st) == -1)
            continue;
        
        const bool is_directory = (st.st_mode & S_IFDIR) != 0;
        
        if (is_directory)
            continue;
        
        if (file_name.substr(file_name.length(),fileExt.length()) != fileExt)
            continue;
        
        out.push_back(file_name);
    }
    closedir(dir);
#endif
} // GetFilesInDirectory

