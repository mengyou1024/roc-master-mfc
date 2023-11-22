#pragma once

#include "duckx.hpp"
#include <Model/SystemConfig.h>

// 点到线段距离
float PointToSegDist(float x, float y, float x1, float y1, float x2, float y2);
// 去基线
void RemoveBaseline(USHORT* pInput, float* pOutput, int iSize);
// 线段是否相交
float LineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, glm::vec2& pt);
void  GenVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO);
void  GenVAO(GLuint& VAO, GLuint& VBO);
void  DeleteVAO(GLuint& VAO, GLuint& VBO, GLuint& VEO);
void  DeleteVAO(GLuint& VAO, GLuint& VBO);
void  DeleteTexture(GLuint& Texture);
void  DeleteBuffers(GLuint& Buffer);
void  GenFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX, GLsizei width, GLsizei height);
void  DeleteFBO(GLuint& FBO, GLuint& FBOTEX, GLuint& RBO, GLuint& PBO, GLuint& OUTTEX);
void  AddQuadsStrip(GLuint* pEBO, int iSize);

glm::vec3 NormalLine(float* p0, float* p1, float* p2);

std::string  StringFromWString(std::wstring str);
std::wstring WStringFromString(std::string str);

std::string GB2312ToUtf8(std::string gb2312);
std::string Utf8ToGB2312(std::string utf8);

int GB2312ToUtf8(const char* gb2312, char* utf8);
int Utf8ToGB2312(const char* utf8, char* gb2312);

/**
 * @brief 判断是否包含中文
 * @param str
 * @return bool
 */
bool IncChinese(std::wstring str);

/**
 * @brief 创建多级目录
 * @param szPath 路径
 * @return bool
 */
bool CreateMultipleDirectory(LPCTSTR szPath);

/**
 * @brief 点是否在矩形框内
 * @param rc 矩形
 * @param pt 点
 * @return bool
 */
bool PointInRect(RECT rc, ::CPoint pt);

/**
 * @brief 获取版本的整形码
 * @param version 版本号字符串
 * @return versionCode
 */
int GetVersionCode(std::string version);

/**
 * @brief 检查更新
 * @param currentVersion 当前版本
 * @param newVersion 新版本
 * @return 是否蛋更新可用
 */
bool Check4Update(std::string currentVersion, std::string newVersion);

/**
 * @brief 获取最新的发布信息
 * @param github_api_url github api地址
 * @return [tag, body, download_url}
 */
std::tuple<string, string, string> GetLatestReleaseNote(std::string github_api_url);

/**
 * @brief word模板变量替换
 * @param templateName Word模板名称
 * @param fileName 保存后的文件名
 * @param var 变量映射表
 */
void WordTemplateRender(std::wstring templateName, std::wstring fileName, std::map<string, string> var);

/**
 * @brief 获取系统配置
 * @return 系统配置结构体
 */
ORM_Model::SystemConfig GetSystemConfig();

/**
 * @brief 更新系统配置
 * @param config 系统配置结构体
 */
void UpdateSystemConfig(ORM_Model::SystemConfig& config);

/**
 * @brief 获取工作组
 * @return 工作中名称
 */
std::string GetJobGroup();
