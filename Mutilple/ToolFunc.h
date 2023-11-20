#pragma once

// 点到线段距离
float PointToSegDist(float x, float y, float x1, float y1, float x2, float y2);

// 去基线
void RemoveBaseline(USHORT* pInput, float* pOutput, int iSize);

// 线段是否相交
float LineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, glm::vec2& pt);

void GenVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO);
void GenVAO(GLuint& VAO, GLuint& VBO);
void DeleteVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO);
void DeleteVAO(GLuint& VAO, GLuint& VBO);

void DeleteTexture(GLuint& Texture);
void DeleteBuffers(GLuint& Buffer);

void GenFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX, GLsizei width, GLsizei height);
void DeleteFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX);

void AddQuadsStrip(GLuint* pEBO, int iSize);

glm::vec3 NormalLine(float* p0, float* p1, float* p2);

std::string StringFromLPCTSTR(LPCTSTR str);
std::string StringFromWString(std::wstring str);
std::wstring WStringFromString(std::string str);
int         GB2312ToUtf8(const char* gb2312, char* utf8);
int         Utf8ToGB2312(const char* utf8, char* gb2312);
bool         IncChinese(CString str);

bool CreateMultipleDirectory(LPCTSTR szPath);
bool pointInRect(RECT rc, ::CPoint pt);

int  GetVersionCode(std::string version);
bool Check4Update(std::string currentVersion, std::string newVersion);

/**
 * @brief 获取最新的发布信息
 * @param github_api_url github api地址
 * @return [tag, body, download_url}
 */
std::tuple<string, string, string> GetLatestReleaseNote(std::string github_api_url);