#ifndef TAPDATACOLLECTAPI_H
#define TAPDATACOLLECTAPI_H

//! �ɹ�
const int TAPIERROR_DataCollect_SUCCEED                                = 0;
//! ��ȡIPʧ��
const int TAPIERROR_IP                                                 = -1;
//! ��ȡMACʧ��
const int TAPIERROR_MAC                                                = -2;
//! ��ȡ�豸����ʧ��
const int TAPIERROR_ComputerName                                       = -3;
//! ��ȡ����ϵͳ�汾ʧ��
const int TAPIERROR_OSVersion                                          = -4;
//! ��ȡӲ�����к�ʧ��
const int TAPIERROR_HDID                                               = -5;
//! ��ȡCPU���к�ʧ��
const int TAPIERROR_CPUSerialID                                        = -6;
//! ��ȡBIOS���к�ʧ��
const int TAPIERROR_BIOSSerailID                                       = -7;
//! ��ȡϵͳ������Ϣʧ��
const int TAPIERROR_SysDiskInfo                                        = -8;
//! ��ȡMacOS�豸���к�ʧ��
const int TAPIERROR_MacOsSerialID                                      = -9;

#if defined WIN32 || defined _WIN64
#define TAP_CDECL __cdecl
#define TAP_DLLEXPORT __declspec(dllexport)
#else
#define TAP_CDECL
#define TAP_DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  
	
TAP_DLLEXPORT int TAP_CDECL esunny_getsysteminfo(char* pSystemInfo, int* nLen, int* nVer);
	

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* TAPDATACOLLECTAPI_H */

