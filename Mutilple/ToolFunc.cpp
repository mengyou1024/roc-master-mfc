#include "pch.h"

#include "ToolFunc.h"

float PointToSegDist(float x, float y, float x1, float y1, float x2, float y2) {
    float cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
    if (cross <= 0)
        return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

    float d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (cross >= d2)
        return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

    float r  = cross / d2;
    float px = x1 + (x2 - x1) * r;
    float py = y1 + (y2 - y1) * r;
    return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}

void RemoveBaseline(USHORT* pInput, float* pOutput, int iSize) {
    const INT iSmooth        = 32;
    USHORT    pLast[iSmooth] = {0};

    float fSum = 0;
    for (int i = 0; i < iSmooth; i++) {
        fSum += pInput[i];
        pLast[i] = pInput[i];
    }

    for (int i = 0; i < iSize; i++) {
        int iLast    = i % iSmooth;
        fSum         = fSum + pInput[i] - pLast[iLast];
        pLast[iLast] = pInput[i];

        float fBase = fSum / iSmooth;
        pOutput[i]  = pInput[i] - fBase;
    }
}

// 线段相交
float LineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, glm::vec2& pt) {
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
        return -1;
    bool denomPositive = denom > 0;

    s02_x   = p0_x - p2_x;
    s02_y   = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return -1;

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return -1;

    if (abs(s_numer) > abs(denom) || abs(t_numer) > abs(denom))
        return -1;

    t = t_numer / denom;

    pt.x = p0_x + (t * s10_x);
    pt.y = p0_y + (t * s10_y);
    return t;
}

void GenVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO) {
    if (VAO == 0)
        glGenVertexArrays(1, &VAO);
    if (VBO == 0)
        glGenBuffers(1, &VBO);
    if (VEO == 0)
        glGenBuffers(1, &VEO);
}

void GenVAO(GLuint& VAO, GLuint& VBO) {
    if (VAO == 0)
        glGenVertexArrays(1, &VAO);
    if (VBO == 0)
        glGenBuffers(1, &VBO);
}

void DeleteVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO) {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VEO != 0) {
        glDeleteBuffers(1, &VEO);
        VEO = 0;
    }
}

void DeleteVAO(GLuint& VAO, GLuint& VBO) {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}

void DeleteTexture(GLuint& Texture) {
    if (Texture != 0) {
        glDeleteTextures(1, &Texture);
        Texture = 0;
    }
}

void DeleteBuffers(GLuint& Buffer) {
    if (Buffer != 0) {
        glDeleteBuffers(1, &Buffer);
        Buffer = 0;
    }
}

void GenFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX, GLsizei width, GLsizei height) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &FBOTEX);
    glBindTexture(GL_TEXTURE_2D, FBOTEX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTEX, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &PBO);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_PACK_BUFFER, width * height * sizeof(DWORD), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    glGenTextures(1, &OUTTEX);
    glBindTexture(GL_TEXTURE_2D, OUTTEX);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DeleteFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX) {
    if (FBO != 0) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }

    if (FBOTEX != 0) {
        glDeleteTextures(1, &FBOTEX);
        FBOTEX = 0;
    }

    if (RBO != 0) {
        glDeleteRenderbuffers(1, &RBO);
        RBO = 0;
    }

    if (PBO != 0) {
        glDeleteBuffers(1, &PBO);
        PBO = 0;
    }

    if (OUTTEX != 0) {
        glDeleteTextures(1, &OUTTEX);
        OUTTEX = 0;
    }
}

void AddQuadsStrip(GLuint* pEBO, int iSize) {
    int iIndex = 0;
    for (int i = 0; i < iSize - 2; i++) {
        pEBO[iIndex++] = i;
        pEBO[iIndex++] = i + 1;
        pEBO[iIndex++] = i + 2;
    }
}

glm::vec3 NormalLine(float* p0, float* p1, float* p2) {
    glm::vec3 a(p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2]);
    glm::vec3 b(p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2]);

    return glm::normalize(glm::cross(a, b));
}

std::string StringFromLPCTSTR(LPCTSTR str) {
#ifdef _UNICODE
    int   size_str        = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, NULL, NULL);
    char* point_new_array = new char[size_str];
    WideCharToMultiByte(CP_UTF8, 0, str, -1, point_new_array, size_str, NULL, NULL);
    std::string return_string(point_new_array);
    delete[] point_new_array;
    point_new_array = NULL;
    return return_string;
#else
    return std::string(str);
#endif
}

// GB2312到UTF-8的转换
int GB2312ToUtf8(const char* gb2312, char* utf8) {
    int      len  = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, static_cast<size_t>(len + 1));
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);
    if (wstr)
        delete[] wstr;
    return len;
}

// UTF-8到GB2312的转换
int Utf8ToGB2312(const char* utf8, char* gb2312) {
    int      len  = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, static_cast<size_t>(len + 1));
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, gb2312, len, NULL, NULL);
    if (wstr)
        delete[] wstr;
    return len;
}

bool CreateMultipleDirectory(LPCTSTR szPath) {
    CString strDir(szPath);                               // 存放要创建的目录字符串
    if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\')) // 确保以'\'结尾以创建最后一个目录
    {
        strDir.AppendChar(_T('\\'));
    }
    vector<CString> vPath;            // 存放每一层目录字符串
    CString         strTemp;          // 一个临时变量,存放目录字符串
    bool            bSuccess = false; // 成功标志
    // 遍历要创建的字符串
    for (int i = 0; i < strDir.GetLength(); ++i) {
        if (strDir.GetAt(i) != _T('\\')) { // 如果当前字符不是'\\'
            strTemp.AppendChar(strDir.GetAt(i));
        } else {                      // 如果当前字符是'\\'
            vPath.push_back(strTemp); // 将当前层的字符串添加到数组中
            strTemp.AppendChar(_T('\\'));
        }
    }
    // 遍历存放目录的数组,创建每层目录
    vector<CString>::const_iterator vIter;
    for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) {
        if (!PathIsDirectory(*vIter)) // 判断当前目录时候存在，不存在就创建
        {
            // 如果CreateDirectory执行成功,返回true,否则返回false
            bSuccess = CreateDirectory(*vIter, NULL) ? true : false;
        }
    }
    return bSuccess;
}

bool IncChinese(CString str) {
    int     nLen = str.GetLength();
    wchar_t ch;
    for (int i = 0; i != nLen; ++i) {
        ch = str[i];
        if (ch >= 0xA0) {
            return true;
        } else {
            continue;
        }
    }
    return false;
}
