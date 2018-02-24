#include "StdAfx.h"
#include "PartionInfo.h"
#include <Windows.h>
#include "DriverInfo.h"
#include <WinIoCtl.h>
#include "util.h"

CPartionInfo::CPartionInfo(void)
{
}


CPartionInfo::~CPartionInfo(void)
{
}

BOOL CPartionInfo::GetPartionsInfo(std::vector<CDriverInfo>& arrDrives)
{
	char szPartionName[] = "X:\\";
	for(char PartionNameChar = 'A';PartionNameChar <= 'Z'; PartionNameChar ++)
	{
		szPartionName[0] = PartionNameChar;
		int driverType = GetDriveTypeA(szPartionName);
		if(driverType == DRIVE_REMOVABLE || driverType == DRIVE_FIXED)
		{
			HANDLE hDevice;
			char devName[MAX_PATH] = {0};
			sprintf(devName, "\\\\?\\%c:", PartionNameChar);
			hDevice = CreateFileA(devName,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,NULL,NULL);
			if(hDevice != INVALID_HANDLE_VALUE)
			{
				DWORD   dwReturned = 0;
				STORAGE_DEVICE_NUMBER sdn;
				BOOL bResult = DeviceIoControl(hDevice,  
                                IOCTL_STORAGE_GET_DEVICE_NUMBER,  
                                NULL,  
                                0,  
                                &sdn,  
                                sizeof(sdn),  
                                &dwReturned,  
                                NULL); 
				if(bResult)
				{
					CDriverInfo drv;
					drv.diskType = driverType;
					drv.m_deviceNumber = sdn.DeviceNumber + 1;
					drv.m_partitionNumber = sdn.PartitionNumber;
					drv.m_PartionName =  CUtil::StringToWString(szPartionName);
					unsigned long long available,total,free;
					if(GetDiskFreeSpaceExA(szPartionName,(ULARGE_INTEGER*)&available,(ULARGE_INTEGER*)&total,(ULARGE_INTEGER*)&free)){
						drv.m_partionFreeSize = available;
						drv.m_partionTotalSize = total;
					}
					else
					{
						drv.m_partionFreeSize = 0;
						drv.m_partionTotalSize = 0;
					}
					DWORD   VolumeSerialNumber; 
					char   VolumeName[256]; 
					GetVolumeInformationA(szPartionName,VolumeName,12,&VolumeSerialNumber,NULL,NULL,NULL,10); 
					drv.m_volName = CUtil::StringToWString(VolumeName);
					arrDrives.push_back(drv);
					//printf("c盘的卷标：%s\n ",VolumeName);
					//printf("c盘的序列号：%lu\n ",VolumeSerialNumber);
					// 判断是不是USB设备盘
					// SetupDiGetClassDevsW
					// SetupDiEnumDeviceInterfaces
					// SetupDiGetDeviceInterfaceDetailW
					// SetupDiDestoryDeviceInfoList

				}
				CloseHandle(hDevice);
			}
			else
			{
				MessageBox(NULL,CUtil::ErrorMessageW(L"CreateFileA").c_str(),NULL,MB_OK);
			}
		}
	}
    return TRUE;  
}

