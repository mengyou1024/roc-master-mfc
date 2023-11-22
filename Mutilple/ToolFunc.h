#pragma once

#include "duckx.hpp"
#include <Model/SystemConfig.h>

// �㵽�߶ξ���
float PointToSegDist(float x, float y, float x1, float y1, float x2, float y2);
// ȥ����
void RemoveBaseline(USHORT* pInput, float* pOutput, int iSize);
// �߶��Ƿ��ཻ
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
 * @brief �ж��Ƿ��������
 * @param str
 * @return bool
 */
bool IncChinese(std::wstring str);

/**
 * @brief �����༶Ŀ¼
 * @param szPath ·��
 * @return bool
 */
bool CreateMultipleDirectory(LPCTSTR szPath);

/**
 * @brief ���Ƿ��ھ��ο���
 * @param rc ����
 * @param pt ��
 * @return bool
 */
bool PointInRect(RECT rc, ::CPoint pt);

/**
 * @brief ��ȡ�汾��������
 * @param version �汾���ַ���
 * @return versionCode
 */
int GetVersionCode(std::string version);

/**
 * @brief ������
 * @param currentVersion ��ǰ�汾
 * @param newVersion �°汾
 * @return �Ƿ񵰸��¿���
 */
bool Check4Update(std::string currentVersion, std::string newVersion);

/**
 * @brief ��ȡ���µķ�����Ϣ
 * @param github_api_url github api��ַ
 * @return [tag, body, download_url}
 */
std::tuple<string, string, string> GetLatestReleaseNote(std::string github_api_url);

/**
 * @brief wordģ������滻
 * @param templateName Wordģ������
 * @param fileName �������ļ���
 * @param var ����ӳ���
 */
void WordTemplateRender(std::wstring templateName, std::wstring fileName, std::map<string, string> var);

/**
 * @brief ��ȡϵͳ����
 * @return ϵͳ���ýṹ��
 */
ORM_Model::SystemConfig GetSystemConfig();

/**
 * @brief ����ϵͳ����
 * @param config ϵͳ���ýṹ��
 */
void UpdateSystemConfig(ORM_Model::SystemConfig& config);

/**
 * @brief ��ȡ������
 * @return ����������
 */
std::string GetJobGroup();
