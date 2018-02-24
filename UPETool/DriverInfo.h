#pragma once
#include<string>
using std::wstring;

class CDriverInfo
{
public:
	CDriverInfo(void);
	~CDriverInfo(void);
public:
	// C: D: ... etc.
	wstring m_PartionName;
	// 1:1 (disk 1,partion 1),1:2(disk 1, partion 2),2:1(disk2,partion1)
	//
    // The number of this device
    //
    DWORD       m_deviceNumber;
    //
    // If the device is partitionable, the partition number of the device.
    // Otherwise -1
    //
    DWORD       m_partitionNumber;
	// WIN7,活动（系统盘）、空白 、活动（启动盘） 
	wstring m_Types;
	// 本地磁盘（C:）、数据（D:）
	wstring m_volName;
	// 可用空间
	unsigned long long m_partionFreeSize;
	// 分区容量
	unsigned long long m_partionTotalSize;
	// diskType GetDriveType
	int diskType;
};

