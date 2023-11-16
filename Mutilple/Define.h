#pragma once

#pragma region ����
#include <pch.h>

#pragma warning(disable :26495)

// �ļ��ؼ��� ���
#define FK_NEWFILE            0x55545041 // ��ʼλ
#define FK_DATA_SYSFILE       0x55545042 // ϵͳ����λ System
#define FK_DATA_CHFILE        0x55545043 // ͨ������λ  Channel*10
#define FK_DATA_SPECIFILE     0x55545044 // ��������/���ֲ���
#define FK_DATA_PLCFILE       0x55545045 // PLC�·��Ĳ���
#define FK_DATA_USERFILE      0x55545046 // ��ǰ̽�˹���Ϣ
#define FK_DATA_STDDETECTFILE 0x55545047 // ��׼̽������
#define FK_DATA_SCANFILE      0x55545048 // ɨ������
#define FK_DATA_DEFECTFILE    0x55545049 // ȱ������
#define FK_DATA_PLCSPEED      0x55545051 // PLC�ٶȲ��� ����
#define FK_DATA_END           0x55545050 // ���ݽ���

const INT   HD_CHANNEL_NUM_12 = 12;                // Ӳ��ͨ����
const INT   HD_CHANNEL_NUM    = HD_CHANNEL_NUM_12; // �ϲ�ͨ����
const int   MAX_GATE_NUM      = 3;                 // �������
const int   MAX_ML_NUM        = 2;                 // ������
const int   COLOR_LIST_NUM    = 6;                 // ɫ�ʱ�
const float MAX_AMP           = 255.0F;

const float ANGLE_LEFT  = 120;
const float ANGLE_RIGHT = 240;

const float MAX_SAMPLEDEPTH  = 1700.0f;
const float MIN_RANGE        = 5.0F;
const float MIN_GAIN         = 0.0F;
const float MAX_GAIN         = 110.0F;
const int   MIN_VELOCITY     = 500;
const int   MAX_VELOCITY     = 20000;
const float MIN_DELAY        = 0.0F; //-10.24F; 10ͨ���忨��ʱΪ��ֵ�����壿
const float MAX_DELAY        = 1700.0F;
const int   MIN_FREQUENCY    = 50;
const int   MAX_FREQUENCY    = 5000;
const int   MIN_PULSEWIDTH   = 30;
const int   MAX_PULSEWIDTH   = 500;
const int   MIN_WHEEL        = 0;
const int   MAX_WHEEL        = 1000;
const int   MIN_WHEEL_OFFSER = -100;
const int   MAX_WHEEL_OFFSER = 100;
const int   MIN_DAC_RL       = -40;
const int   MAX_DAC_RL       = 40;
const int   MIN_DAC_Hole     = 10;
const int   MAX_DAC_Hole     = 500;
const int   MIN_DAC_AMP      = 0;
const int   MAX_DAC_AMP      = 100;
// PLC���Ʒ���
const int MIN_X_POS = 0;
const int MAX_X_POS = 1000;

const int MIN_Y_POS = 0;
const int MAX_Y_POS = 1000;

const int MIN_Y_Speed = 0;
const int MAX_Y_Speed = 100;
//
const INT DAC_HOLE_NUM = 12; // DAC �ɼ��׵ĸ���
//
const int TIMER_TIME  = 100;
const int TIMER_SCAN  = 111;
const int MESSURE_NUM = 4; // ��������list���� �и���
//
// ɨ����ͼ
const DWORD A_SCAN = 0x00000001;
const DWORD C_SCAN = 0x00000008;

// Ĭ��ɫ�ʱ�
const DWORD COLOR_TAB[3][256] = {
    {0xFCFDFE, 0x1F7FBFE, 0x2F2F8FD, 0x3EDF5FD, 0x4E7F2FC, 0x5E2EFFC, 0x6DDECFB, 0x7D8EAFB, 0x8D2E7FA, 0x9CDE4FA, 0xAC8E1F9,
     0xBC3DEF9, 0xCBDDBF8, 0xDB8D8F8, 0xEB3D6F7, 0xFAED3F7, 0x10A8D0F6, 0x11A3CDF6, 0x129ECAF5, 0x1399C7F5, 0x1485BCF2, 0x1581BAF1,
     0x167CB7EF, 0x1778B5EE, 0x1873B2EC, 0x196FB0EB, 0x1A6AADE9, 0x1B66ABE8, 0x1C61A8E6, 0x1D5DA6E5, 0x1E58A3E3, 0x1F54A1E1, 0x204F9EE0,
     0x214B9CDE, 0x224699DD, 0x234297DB, 0x243D94DA, 0x253992D8, 0x26348FD7, 0x27308DD5, 0x282989CE, 0x292A89CC, 0x2A2B8ACA, 0x2B2C8BC8,
     0x2C2D8CC6, 0x2D2E8DC4, 0x2E2F8EC2, 0x2F318FC0, 0x303290BE, 0x313391BC, 0x323492B9, 0x333592B7, 0x343693B5, 0x353794B3, 0x363995B1,
     0x373A96AF, 0x383B97AD, 0x393C98AB, 0x3A3D99A9, 0x3B3E9AA7, 0x3C469F9F, 0x3D49A19D, 0x3E4CA39A, 0x3F4FA597, 0x4052A794, 0x4155A991,
     0x4258AB8F, 0x435BAD8C, 0x445EAF89, 0x4561B186, 0x4664B483, 0x4767B681, 0x486AB87E, 0x496DBA7B, 0x4A70BC78, 0x4B73BE75, 0x4C76C073,
     0x4D79C270, 0x4E7CC46D, 0x4F7FC66A, 0x5089CE61, 0x518CCF5F, 0x528FD15C, 0x5393D359, 0x5496D556, 0x5599D754, 0x569DD951, 0x57A0DB4E,
     0x58A3DD4B, 0x59A7DF49, 0x5AAAE146, 0x5BADE343, 0x5CB1E540, 0x5DB4E73E, 0x5EB7E93B, 0x5FBBEB38, 0x60BEED35, 0x61C1EF33, 0x62C5F130,
     0x63C8F32D, 0x64D3F825, 0x65D5F824, 0x66D7F822, 0x67D9F820, 0x68DBF91E, 0x69DEF91C, 0x6AE0F91A, 0x6BE2F919, 0x6CE4FA17, 0x6DE6FA15,
     0x6EE9FA13, 0x6FEBFA11, 0x70EDFB0F, 0x71EFFB0D, 0x72F1FB0C, 0x73F4FB0A, 0x74F6FC08, 0x75F8FC06, 0x76FAFC04, 0x77FCFC02, 0x78FFFD00,
     0x79FFFB00, 0x7AFFF900, 0x7BFFF700, 0x7CFFF500, 0x7DFFF300, 0x7EFFF100, 0x7FFFEF00, 0x80FFED00, 0x81FFEB00, 0x82FFE900, 0x83FFE700,
     0x84FFE500, 0x85FFE300, 0x86FFE100, 0x87FFDF00, 0x88FFDD00, 0x89FFDB00, 0x8AFFD900, 0x8BFFD700, 0x8CFFCE00, 0x8DFFCB00, 0x8EFFC800,
     0x8FFFC400, 0x90FFC100, 0x91FFBD00, 0x92FFBA00, 0x93FFB600, 0x94FFB300, 0x95FFAF00, 0x96FFAC00, 0x97FFA900, 0x98FFA500, 0x99FFA200,
     0x9AFF9E00, 0x9BFF9B00, 0x9CFF9700, 0x9DFF9400, 0x9EFF9000, 0x9FFF8D00, 0xA0FF8100, 0xA1FF7E00, 0xA2FF7A00, 0xA3FF7700, 0xA4FF7300,
     0xA5FF7000, 0xA6FF6C00, 0xA7FF6900, 0xA8FF6500, 0xA9FF6200, 0xAAFF5E00, 0xABFF5A00, 0xACFF5700, 0xADFF5300, 0xAEFF5000, 0xAFFF4C00,
     0xB0FF4900, 0xB1FF4500, 0xB2FF4200, 0xB3FF3E00, 0xB4FF3300, 0xB5FF3100, 0xB6FF2E00, 0xB7FF2C00, 0xB8FF2900, 0xB9FF2700, 0xBAFF2400,
     0xBBFF2200, 0xBCFF1F00, 0xBDFF1D00, 0xBEFF1A00, 0xBFFF1700, 0xC0FF1500, 0xC1FF1200, 0xC2FF1000, 0xC3FF0D00, 0xC4FF0B00, 0xC5FF0800,
     0xC6FF0600, 0xC7FF0300, 0xC8FF0000, 0xC9FF0000, 0xCAFF0000, 0xCBFF0000, 0xCCFF0000, 0xCDFF0000, 0xCEFF0000, 0xCFFF0000, 0xD0FF0000,
     0xD1FF0000, 0xD2FF0000, 0xD3FF0000, 0xD4FF0000, 0xD5FF0000, 0xD6FF0000, 0xD7FF0000, 0xD8FF0000, 0xD9FF0000, 0xDAFF0000, 0xDBFF0000,
     0xDCFF0000, 0xDDFF0000, 0xDEFF0000, 0xDFFF0000, 0xE0FF0000, 0xE1FF0000, 0xE2FF0000, 0xE3FF0000, 0xE4FF0000, 0xE5FF0000, 0xE6FF0000,
     0xE7FF0000, 0xE8FF0000, 0xE9FF0000, 0xEAFF0000, 0xEBFF0000, 0xECFF0000, 0xEDFF0000, 0xEEFF0000, 0xEFFF0000, 0xF0FF0000, 0xF1FF0000,
     0xF2FF0000, 0xF3FF0000, 0xF4FF0000, 0xF5FF0000, 0xF6FF0000, 0xF7FF0000, 0xF8FF0000, 0xF9FF0000, 0xFAFF0000, 0xFBFF0000, 0xFCFF0000,
     0xFDFF0000, 0xFEFF0000, 0xFFFF0000},
    {0xFAFCFE, 0x1F6FAFD, 0x2F2F7FD, 0x3EEF5FC, 0x4E6F0FB, 0x5E1EDFA, 0x6DDEBF9, 0x7D9E8F9, 0x8D1E3F7, 0x9CDE1F7, 0xAC8DEF6,
     0xBC0D9F5, 0xCBCD7F4, 0xDB8D4F4, 0xEB4D2F3, 0xFABCDF2, 0x10A7CBF1, 0x11A3C8F0, 0x129FC6F0, 0x1396C1EF, 0x1492BEEE, 0x158EBCED,
     0x1686B7EC, 0x1782B4EB, 0x187DB2EB, 0x1979AFEA, 0x1A71AAE9, 0x1B6DA8E8, 0x1C69A6E8, 0x1D66A1E5, 0x1E6099DF, 0x1F5D95DC, 0x205A91DA,
     0x215589D4, 0x225285D1, 0x234F81CF, 0x244C7DCC, 0x254775C6, 0x264471C3, 0x27416DC1, 0x283B65BB, 0x293861B8, 0x2A365DB6, 0x2B3359B3,
     0x2C2D51AD, 0x2D2A4DAA, 0x2E2749A8, 0x2F2545A5, 0x301F3D9F, 0x311C399D, 0x3219359A, 0x33142D94, 0x34112991, 0x350E258F, 0x360B218C,
     0x37051986, 0x38031584, 0x39041883, 0x3A061C83, 0x3B0A2483, 0x3C0C2883, 0x3D0E2C83, 0x3E123482, 0x3F143882, 0x40153C82, 0x41174082,
     0x421B4881, 0x431D4C81, 0x441F5081, 0x45215481, 0x46255C81, 0x47266080, 0x48286480, 0x492C6C80, 0x4A2E7080, 0x4B307480, 0x4C32787F,
     0x4D36807F, 0x4E37847F, 0x4F39887F, 0x503D907E, 0x513F947E, 0x5241987E, 0x53439C7E, 0x5447A47E, 0x554CA67B, 0x5651A878, 0x5756AA75,
     0x5860AE6F, 0x5965B06C, 0x5A6AB269, 0x5B74B764, 0x5C79B961, 0x5D7EBB5E, 0x5E83BD5B, 0x5F8DC155, 0x6092C353, 0x6197C550, 0x629CC74D,
     0x63A6CC47, 0x64ABCE44, 0x65B0D041, 0x66BAD43C, 0x67BFD639, 0x68C4D836, 0x69C9DA33, 0x6AD3DF2D, 0x6BD8E12B, 0x6CDDE328, 0x6DE2E525,
     0x6EECE91F, 0x6FF1EB1C, 0x70F6ED19, 0x71FAEC19, 0x72F9E91B, 0x73F8E61D, 0x74F7E31F, 0x75F5DC24, 0x76F4D926, 0x77F2D628, 0x78F0CF2D,
     0x79EFCC2F, 0x7AEEC931, 0x7BEDC633, 0x7CEABF38, 0x7DE9BC3A, 0x7EE8B93C, 0x7FE7B63F, 0x80E5AF43, 0x81E4AC45, 0x82E2A947, 0x83E0A24C,
     0x84DF9F4E, 0x85DE9C50, 0x86DD9953, 0x87DA9257, 0x88D98F59, 0x89D88C5B, 0x8AD7885E, 0x8BD58262, 0x8CD47F64, 0x8DD37C67, 0x8ED27B62,
     0x8FD27B60, 0x90D27A5E, 0x91D17A5B, 0x92D17957, 0x93D17955, 0x94D07953, 0x95D07850, 0x96D0784C, 0x97CF774A, 0x98CF7747, 0x99CF7643,
     0x9ACE7641, 0x9BCE763F, 0x9CCE753C, 0x9DCD7538, 0x9ECD7436, 0x9FCD7433, 0xA0CC732F, 0xA1CC732D, 0xA2CC732B, 0xA3CC7228, 0xA4CB7224,
     0xA5CB7122, 0xA6CB711F, 0xA7CA711D, 0xA8CA7019, 0xA9CA7017, 0xAAC86D17, 0xABC56717, 0xACC36417, 0xADC26217, 0xAEC05F18, 0xAFBD5918,
     0xB0BB5718, 0xB1BA5418, 0xB2B85118, 0xB3B54B19, 0xB4B34919, 0xB5B24619, 0xB6AF4019, 0xB7AD3E1A, 0xB8AB3B1A, 0xB9AA381A, 0xBAA7321A,
     0xBBA5301A, 0xBCA42D1B, 0xBDA22A1B, 0xBE9F251B, 0xBF9D221B, 0xC09C1F1B, 0xC198191C, 0xC297171C, 0xC395141C, 0xC494111C, 0xC5910C1D,
     0xC68F091D, 0xC78E061D, 0xC88C031D, 0xC98C031D, 0xCA8C031D, 0xCB8C031D, 0xCC8C031D, 0xCD8C031D, 0xCE8C031D, 0xCF8C031D, 0xD08C031D,
     0xD18C031D, 0xD28C031D, 0xD38C031D, 0xD48C031D, 0xD58C031D, 0xD68C031D, 0xD78C031D, 0xD88C031D, 0xD98C031D, 0xDA8C031D, 0xDB8C031D,
     0xDC8C031D, 0xDD8C031D, 0xDE8C031D, 0xDF8C031D, 0xE08C031D, 0xE18C031D, 0xE28C031D, 0xE38C031D, 0xE48C031D, 0xE58C031D, 0xE68C031D,
     0xE78C031D, 0xE88C031D, 0xE98C031D, 0xEA8C031D, 0xEB8C031D, 0xEC8C031D, 0xED8C031D, 0xEE8C031D, 0xEF8C031D, 0xF08C031D, 0xF18C031D,
     0xF28C031D, 0xF38C031D, 0xF48C031D, 0xF58C031D, 0xF68C031D, 0xF78C031D, 0xF88C031D, 0xF98C031D, 0xFA8C031D, 0xFB8C031D, 0xFC8C031D,
     0xFD8C031D, 0xFE8C031D, 0xFF8C031D},
    {0x000000, 0x1000003, 0x2000007, 0x300000B, 0x4000013, 0x5000017, 0x600001B, 0x700001F, 0x8000027, 0x900002B, 0xA00002F,
     0xB000037, 0xC00003B, 0xD00003F, 0xE000043, 0xF00004B, 0x1000004F, 0x11000053, 0x12000057, 0x1300005F, 0x14000063, 0x15000067,
     0x1600006F, 0x17000073, 0x18000077, 0x1900007B, 0x1A000083, 0x1B000087, 0x1C00008B, 0x1D00008F, 0x1E000097, 0x1F00009B, 0x2000009F,
     0x210000A7, 0x220000AB, 0x230000AF, 0x240000B3, 0x250000BB, 0x260000BF, 0x270000C3, 0x280000CB, 0x290000CF, 0x2A0000D3, 0x2B0000D7,
     0x2C0000DF, 0x2D0000E3, 0x2E0000E7, 0x2F0000EB, 0x300000F3, 0x310000F7, 0x320000FB, 0x330003FD, 0x340007FA, 0x35000BF7, 0x36000FF4,
     0x370017EE, 0x38001BEB, 0x39001FE8, 0x3A0023E5, 0x3B002BDF, 0x3C002FDC, 0x3D0033D9, 0x3E003BD3, 0x3F003FD0, 0x400043CD, 0x410047CA,
     0x42004FC4, 0x430053C1, 0x440057BE, 0x45005BBB, 0x460063B5, 0x470067B2, 0x48006BAF, 0x490073A9, 0x4A0077A6, 0x4B007BA3, 0x4C007FA0,
     0x4D00879A, 0x4E008B97, 0x4F008F94, 0x5000978E, 0x51009B8B, 0x52009F88, 0x5300A385, 0x5400AB7F, 0x5500AF7C, 0x5600B379, 0x5700B776,
     0x5800BF70, 0x5900C36D, 0x5A00C76A, 0x5B00CF64, 0x5C00D361, 0x5D00D75E, 0x5E00DB5B, 0x5F00E355, 0x6000E752, 0x6100EB4F, 0x6200EF4C,
     0x6300F746, 0x6400FB43, 0x6500FF40, 0x6607FF3E, 0x670BFF3D, 0x680FFF3C, 0x6913FF3B, 0x6A1BFF39, 0x6B1FFF38, 0x6C23FF37, 0x6D27FF36,
     0x6E2FFF34, 0x6F33FF33, 0x7037FF32, 0x713FFF30, 0x7243FF2F, 0x7347FF2E, 0x744BFF2D, 0x7553FF2B, 0x7657FF2A, 0x775BFF29, 0x7863FF27,
     0x7967FF26, 0x7A6BFF25, 0x7B6FFF24, 0x7C77FF22, 0x7D7BFF21, 0x7E7FFF20, 0x7F83FF1F, 0x808BFF1D, 0x818FFF1C, 0x8293FF1B, 0x839BFF19,
     0x849FFF18, 0x85A3FF17, 0x86A7FF16, 0x87AFFF14, 0x88B3FF13, 0x89B7FF12, 0x8ABBFF11, 0x8BC3FF0F, 0x8CC7FF0E, 0x8DCBFF0D, 0x8ED3FF0B,
     0x8FD7FF0A, 0x90DBFF09, 0x91DFFF08, 0x92E7FF06, 0x93EBFF05, 0x94EFFF04, 0x95F3FF03, 0x96FBFF01, 0x97FFFF00, 0x98FFFC00, 0x99FFF400,
     0x9AFFF000, 0x9BFFEC00, 0x9CFFE800, 0x9DFFE000, 0x9EFFDC00, 0x9FFFD800, 0xA0FFD000, 0xA1FFCC00, 0xA2FFC800, 0xA3FFC400, 0xA4FFBC00,
     0xA5FFB800, 0xA6FFB400, 0xA7FFB000, 0xA8FFA800, 0xA9FFA400, 0xAAFFA000, 0xABFF9800, 0xACFF9400, 0xADFF9000, 0xAEFF8C00, 0xAFFF8400,
     0xB0FF8000, 0xB1FF7C00, 0xB2FF7800, 0xB3FF7000, 0xB4FF6C00, 0xB5FF6800, 0xB6FF6000, 0xB7FF5C00, 0xB8FF5800, 0xB9FF5400, 0xBAFF4C00,
     0xBBFF4800, 0xBCFF4400, 0xBDFF4000, 0xBEFF3800, 0xBFFF3400, 0xC0FF3000, 0xC1FF2800, 0xC2FF2400, 0xC3FF2000, 0xC4FF1C00, 0xC5FF1400,
     0xC6FF1000, 0xC7FF0C00, 0xC8FF0400, 0xC9FF0400, 0xCAFF0400, 0xCBFF0400, 0xCCFF0400, 0xCDFF0400, 0xCEFF0400, 0xCFFF0400, 0xD0FF0400,
     0xD1FF0400, 0xD2FF0400, 0xD3FF0400, 0xD4FF0400, 0xD5FF0400, 0xD6FF0400, 0xD7FF0400, 0xD8FF0400, 0xD9FF0400, 0xDAFF0400, 0xDBFF0400,
     0xDCFF0400, 0xDDFF0400, 0xDEFF0400, 0xDFFF0400, 0xE0FF0400, 0xE1FF0400, 0xE2FF0400, 0xE3FF0400, 0xE4FF0400, 0xE5FF0400, 0xE6FF0400,
     0xE7FF0400, 0xE8FF0400, 0xE9FF0400, 0xEAFF0400, 0xEBFF0400, 0xECFF0400, 0xEDFF0400, 0xEEFF0400, 0xEFFF0400, 0xF0FF0400, 0xF1FF0400,
     0xF2FF0400, 0xF3FF0400, 0xF4FF0400, 0xF5FF0400, 0xF6FF0400, 0xF7FF0400, 0xF8FF0400, 0xF9FF0400, 0xFAFF0400, 0xFBFF0400, 0xFCFF0400,
     0xFDFF0400, 0xFEFF0400, 0xFFFF0400}
};
// ����10ͨ����ɫ  ����� ��ɫ 0 �����ȱ�� ����11      ��ȱ�� 1-10 ͨ����
const DWORD COLOR_TAB_CH[12] = {
    0xFF000000, // ��ɫ 0
    0xFFDC143C, // �ɺ� 1
    0xFFC71585, // ���е���������ɫ2
    0xFF800080, // ��ɫ 2
    0xFF483D8B, // ���Ұ�����ɫ3
    0xFF0000FF, // ���� 4
    0xFF00BFFF, // ������6
    0xFF00FFFF, // ˮ��ɫ7
    0xFF2E8B57, // ������8
    0xFF00FF00, // ���ɫ9
    0xFFDAA520, // ������ 10
    0x01505050  // ����
};

#define WM_DEFECTLISTITEMSELECT WM_USER + 30

#define PI                      3.141592653f
#define ANG2RAD(x)              (0.017453292f * (x)) // 1��=��/180��0.01745����
#define RAD2ANG(x)              ((x) / 0.017453292f)
#define _Tr                     __T

#define STR_LEN                 64

#define AXIS_X                  0
#define AXIS_Y                  1
#define AXIS_Z                  2
#define GATE_A                  0
#define GATE_B                  1
#define EPS                     0.00001 // �жϸ�������ȵ�΢С���
// ����
enum class ITEM_KEYDOWN_TYPE {
    ITEM_KEYDOWN_NONE,
    ITEM_KEYDOWN_NORMAL,
    ITEM_KEYDOWN_X_POS,
    ITEM_KEYDOWN_X_WIDTH,
    ITEM_KEYDOWN_Y_POS,
    ITEM_KEYDOWN_Y_WIDTH,
    ITEM_KEYDOWN_MOVE,
    ITEM_KEYDOWN_ZOOM
};
enum ITEM_VIEW_TYPE {
    ITEM_VIEW_OUT    = 0,
    ITEM_VIEW_WORK   = 1,
    ITEM_VIEW_AXIS_X = 2,
    ITEM_VIEW_AXIS_Y = 3,
};
enum SCAN_TYPE { SCAN_NORMAL = 0, SCAN_SCAN, SCAN_REPLAY };
// ����״̬  ROCESS_PARAMSET ��������   ROCESS_SCAN ɨ��  ��Ҫ���� �·������������Դ��
enum MAINPROCESS_TYPE { ROCESS_PARAMSET, ROCESS_SCAN };

#pragma endregion

#pragma region �ṹ��

// ���ݶ�ȡ
typedef struct _tagREAD_DATA {
    float fAngle; // ����Ƕ�

    __int32      pAxle[HD_CHANNEL_NUM]       = {}; // ���
    __int32      pChannel[HD_CHANNEL_NUM]    = {}; // ͨ����
    __int32      pPackage[HD_CHANNEL_NUM]    = {}; // ������
    vector<BYTE> pAscan[HD_CHANNEL_NUM]      = {}; // Aɨ����
    __int32      pCoder[HD_CHANNEL_NUM][2]   = {}; // ������ֵ
    __int32      pGatePos[HD_CHANNEL_NUM][2] = {}; // ����λ��
    __int32      pGateAmp[HD_CHANNEL_NUM][2] = {}; // ���Ų���
    __int32      pAlarm[HD_CHANNEL_NUM][2]   = {}; // ���ű���
} READ_DATA, *P_READ_DATA;

// ����
typedef struct _tagDRAW_BUFFER {
    HANDLE           hReady                      = {};
    int              pCoder[HD_CHANNEL_NUM][2]   = {}; // ������ֵ
    __int32          pGatePos[HD_CHANNEL_NUM][2] = {}; // ����λ��
    __int32          pGateAmp[HD_CHANNEL_NUM][2] = {}; // ���Ų���
    vector<float>    pData[HD_CHANNEL_NUM]       = {}; // ʵʱ�ɼ�ʱ �Ѿ� /MAX_AMP ��0.1-1.0f��
    _tagDRAW_BUFFER* pNext;
} DRAW_BUFFER, *P_DRAW_BUFFER;

// ���ݼ�¼
typedef struct _tagRECORD_DATA {
    __int32 pGatePos[HD_CHANNEL_NUM][2] = {}; // ����λ��
    __int32 pGateAmp[HD_CHANNEL_NUM][2] = {}; // ���Ų���

    __int64      iAscanSize[HD_CHANNEL_NUM] = {}; // pAscan ��С
    vector<BYTE> pAscan[HD_CHANNEL_NUM]     = {}; // ����ͨ�����ݶ���¼
} RECORD_DATA, *P_RECORD_DATA;

typedef struct _tagRECORD_DATA_RES {
    __int32 pGatePos[HD_CHANNEL_NUM][2]{}; // ����λ��
    __int32 pGateAmp[HD_CHANNEL_NUM][2]{}; // ���Ų���

    __int64      iAscanSize[HD_CHANNEL_NUM]{};  // pAscan ��С
    vector<BYTE> pAscan[HD_CHANNEL_NUM]{};      // ����ͨ�����ݶ���¼
    float        fDb[HD_CHANNEL_NUM]{};         // ȱ�ݴ�С
    int          bDefectType[HD_CHANNEL_NUM]{}; // ȱ������ 0 ȱ�� 1. ͸������
} RECORD_DATA_RES, *P_RECORD_DATA_RES;

typedef struct _tagFILE_INFO {
    bool  bSelected         = {};
    TCHAR pName[_MAX_FNAME] = {}; // �ļ���
    TCHAR pPath[_MAX_FNAME] = {}; // ·��
    TCHAR pUser[128]        = {}; // ɨ����Ա
    TCHAR pDate[32]         = {}; // ɨ������
} FILE_INFO, *P_FILE_INFO;
// ����
typedef struct _tagGL_RECT {
    // ��������
    int left{};
    int right{};
    int top{};
    int bottom{};

    // ��ͼ����
    int vleft{};
    int vright{};
    int vtop{};
    int vbottom{};

    int iAxisWidth{};

    int Width() {
        return right - left;
    }
    int Height() {
        return bottom - top;
    }
    int vWidth() {
        return vright - vleft;
    }
    int vHeight() {
        return vbottom - vtop;
    }

    int pt[9][2]{0};

    void set(int l, int r, int t, int b, int w = 25) {
        left   = l;
        right  = r;
        top    = t;
        bottom = b;

        iAxisWidth = w;

        pt[0][0] = left;
        pt[0][1] = top;

        pt[1][0] = left + iAxisWidth;
        pt[1][1] = top;

        pt[2][0] = right;
        pt[2][1] = top;

        pt[3][0] = left;
        pt[3][1] = top + iAxisWidth;

        pt[4][0] = vleft = left + iAxisWidth;
        pt[4][1] = vtop = top + iAxisWidth;

        pt[5][0] = right;
        pt[5][1] = top + iAxisWidth;

        pt[6][0] = left;
        pt[6][1] = bottom;

        pt[7][0] = left + iAxisWidth;
        pt[7][1] = bottom;

        pt[8][0] = vright = right;
        pt[8][1] = vbottom = bottom;
    }

    int Contain(int x, int y) {
        if (x > left && x < right && y > top && y < bottom) {
            if (x > vleft && y < vtop)
                return ITEM_VIEW_AXIS_X;
            else if (x < vleft && y > vtop)
                return ITEM_VIEW_AXIS_Y;
            return ITEM_VIEW_WORK;
        }

        return ITEM_VIEW_OUT;
    }

} GL_RECT, *P_GL_RECT;

typedef struct _tagRECTF {
    float left{};
    float right{};
    float top{};
    float bottom{};
} RECTF, *P_RECTF;

typedef struct _tagARGB {
    union {
        DWORD color;
        UCHAR bgra[4];
    };
} ARGB, *P_ARGB;

struct PT_V3F_T2F {
    float x{};
    float y{};
    float z{};

    float s{};
    float t{};
};

struct PT_V2F_C4F {
    float x{};
    float y{};

    float r{};
    float g{};
    float b{};
    float a{};
};

struct float4_2 {
    glm::vec2 lt{}, rt{}, rb{}, lb{};
};

// �Զ�������صĲ��� ��Ҫ�·�plc
struct PLC_SCAN_PAPA {
    /**
    ɨ�������� �Զ�������� ��ֵ
    Y������ ��������Ϊԭ�� ����λ�� ����Ϊ��
    Y������̽ͷ��0λ��Ϊԭ�㣬����Ϊ����
    */

    // �������
    float fSideYPos{}; // ����̽ͷѹ����������ɨ���ƶ�

    float fSideXStart{}; // ����̽ͷ��X��ʼ��
    float fSideXEnd{};   // ����̽ͷ��X������
    float fSideXStep{};  // ����̽ͷ�Ĳ���ֵ
    float fSideParam1{}; // ��������
    float fSideParam2{}; // ��������

    // ̤�����
    float fTreadXPos{}; // ̤��̽ͷ���λ�ã����������ƶ� ��

    float fTreadYStart{}; // ̤��̽ͷ��Y��ʼ��
    float fTreadYEnd{};   // ̤��̽ͷ��Y������
    float fTreadYStep{};  // ̤��̽ͷ�Ĳ���ֵ
    int   nBestCirSize{}; // ���Ȧ��
    float fTreadLyXPos{}; // ��Ե��λ��
};
struct PLC_SPeed_PAPA {
    /**
    ɨ������ٶȲ���
    */

    // ��������  �ֶ�/�Զ�
    float fSideXSpeed1{};
    float fSideXSpeed2{};
    float fSideYSpeed1{};
    float fSideYSpeed2{};

    // ��̤��
    float fTreadXSpeed1{};
    float fTreadXSpeed2{};
    float fTreadYSpeed1{};
    float fTreadYSpeed2{};

    // ת��
    float fRotateSpeed1{};
};
// ���ֲ���  ������Ҫ���� ���� �Լ� ��Ҫ�ĳߴ����
struct WHEEL_PAPA {
    TCHAR szWheelUseModel[20]{};    // ���ó���  ������
    TCHAR szWheelType[20]{};        // ��������  �ֶ�
    TCHAR szWheelName[20]{};        // ��������
    TCHAR szWheelModel[20]{};       // �����ͺ�
    TCHAR szWheelMaterial[20]{};    // ���ֲ���  ����
    TCHAR szWheelNumber[20]{};      // ���ֱ��
    TCHAR szHeatNumber[20]{};       // ¯��
    TCHAR szDetectionStd[20]{};     // ̽�˱�׼ TB/T2995-200
    TCHAR szDetectionContent[50]{}; // ̽������   ȱ��С�ڧ�2mmƽ�ؿ׵������ڲ�����ʱ�Ͳ�˥��������4dB
    TCHAR szDetectionArea[20]{};    // ̽������  ̤�� �ڲ���
    TCHAR szDetectionFact[50]{};    // ��ⵥλ
    TCHAR szDeviceName[50]{};       // �豸����
    // ���ֳߴ�
    FLOAT fWheelHub{};               // ��챺��  1
    FLOAT fWheelRim{};               // ������� 2
    FLOAT fWheelInnerSideOffset{};   // ����ڲ����������ڲ���߶Ȳ-n~+n�� 3
    FLOAT fWheelHubInnerDiameter{};  // �����ھ� 4
    FLOAT fWheelHubOuterDiameter{};  // ����⾶ 5
    FLOAT fWheelRimlInnerDiameter{}; // �����ھ� 6
    FLOAT fWheelRimOuterDiameter{};  // �����⾶ 7
    // ������������
    FLOAT fTreadWidth{}; // ̤��̽ͷ���ļ�ࣨ3�� ������ļ�࣬���ڼ���̽ͷ�ֶθ��ǣ�
    FLOAT fSideWidth{};  // ����̽ͷ���ļ�� ��2�� ������ļ�࣬���ڼ���̽ͷ�ֶθ��ǣ�

    FLOAT fWheelFlangeHeight{}; // ��Ե��� ������ ̤���ⷶΧ ������ȼ�ȥ��Ե��ȣ�
    FLOAT fWheelParam4{};
};

// �ճ�����  ��Ч�ر���ʹ�����־���ɨ�� �г�ɨ��ȱ�ݵ�db ���ֵ��

// ������Ϣ
struct DB_QUARTER_DATA {
    char  m_dtTime[11]{};             // ����ʱ��
    TCHAR m_szPerson[20]{};           // ������Ա�����򹤺�
    float m_nHorLinearity[10]{};      // ˮƽ����  �������Ϊ������ܣ������Ƿ�ϸ�ͨ��5������ֵ��Χ�ж�
    float m_nVerLinearity[10]{};      // ��ֱ����
    float m_nDistinguishValuel[10]{}; // �ֱ���
    float m_nDynamicRange[10]{};      // ��̬��Χ
    float m_nSensitivityMargin[10]{}; // ��̬��Χ����������

    TCHAR szProbeType[10]{}; // ̽ͷ����
    INT32 nProbeFrequency{}; // ̽ͷƵ��
    int   nPreProbeNum{};    // ̽ͷ���
    TCHAR szProbeMake[10]{}; // ̽ͷ��ʶ A1 B1֮��

    TCHAR m_szStdTestBlock[20]{}; // ��׼�Կ��ͺ�

    //	 ������������
    int   nParam1{};
    int   nParam2{};
    TCHAR szParam3[20]{};
    TCHAR szParam4[20]{};
};

// ����TB/T2995_200 ��������Ϣ
struct DETECTION_PARAM2995_200 {
    FLOAT fScanGain{};     // ̽��������
    FLOAT fScanCompensate; // ̽�˲���������

    FLOAT fScanTrUTGain{};       // ͸��������
    FLOAT fScanTrUTCompensate{}; // ͸������

    FLOAT fAperture{}; // �׾�
    int   nUseHole{};  // ��������

    FLOAT fDAC_HoleDepth[DAC_HOLE_NUM]{}; // Dac �Ŀ��� λ��  10 - 30 -50 -100mm
    FLOAT fDAC_HolebAmp[DAC_HOLE_NUM]{};  // Dac�� �׵Ĳ��� �ٷֱ�   0-1

    FLOAT fDAC_BaseGain{};  // ����Dac �Ļ�׼���� ĸ������ ��һ�׵�����
    FLOAT fDAC_LineRD{};    // ��¼�� ��ĸ�ߵ�dB
    FLOAT fDAC_LineRL{};    // �з���
    FLOAT nDACGatePos[2]{}; // ������λ �ٷֱ�  0-1 ȡֵ
    FLOAT nDACWidth[2]{};   // ���ſ�� �ٷֱ�  0-1 ȡֵ

    //	 ������������
    float fGateAAmpPos{}; // ��̬DAC ��¼����DAC ��һ�����A����߲�  ���泵��̤������Ҫ��̬DAC
    int   nParam2{};
};

// ȱ����Ϣ һ����������ж���ȱ����Ϣ
struct DB_DEFECT_DATA {
    int   nIndex{};              // ȱ�������� ��ǰͨ����
    int   nCircleIndex{};        // ȱ������߲�ʱ����Ȧ��
    int   nScanIndex{};          // ȱ������߲�ʱɨ�����ݵ�����
    int   nCH{};                 // ����ȱ�ݵ�ͨ��
    TCHAR szProbeMake[10]{};     // ̽������� ̽ͷ��ʶ A1 B1֮��
    TCHAR szDetectionArea[20]{}; // ȱ�ݲ�λ ̤�� /����
    float fProbleYPos{};         // ����ȱ��ʱ̽ͷλ��Y        nCH 0-5 Ϊ̤��λ��     6-9Ϊ����λ��
    float fProbleXPos{};         // ����ȱ��ʱ̽ͷλ��X
    float nRadialDistance{};     // �������  //̤��̽ͷΪ ȱ�����       ����̽ͷΪ̽ͷλ��
    float nAxialDepth{};         // �������    //̤��̽ͷΪ ̽ͷλ��      ����̽ͷΪȱ�����
    float nDefectAngle{};        // ȱ�ݽǶ�
    int   nWaveHeight{};         // ���� 0-255
    float nDBOffset{};           // dB��
    float nSensitivity{};        // ̽��������
    float nTranslucency{};       // ͸��������
    int   bDefectType{};         // 0 ȱ�� 1. ͸������

    //	 ������������
    int   nParam1{}; // �״η���ȱ�ݵ�����
    int   nParam2{}; // ȱ�ݽ���������
    TCHAR szParam3[20]{};
    TCHAR szParam4[20]{};
};

// ��Ա��Ϣ
struct DB_USER_DATA {
    long  lSerial{};       // �Զ����
    TCHAR strName[21]{};   // �����Ա
    TCHAR strNumber[21]{}; // ����
    TCHAR strPwd[21]{};    // ����
    long  lLevel{};        // Ȩ��
    TCHAR strRemark[41]{}; // ��ע
};

#pragma endregion
