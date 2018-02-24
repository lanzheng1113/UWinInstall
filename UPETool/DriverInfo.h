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
	// WIN7,���ϵͳ�̣����հ� ����������̣� 
	wstring m_Types;
	// ���ش��̣�C:�������ݣ�D:��
	wstring m_volName;
	// ���ÿռ�
	unsigned long long m_partionFreeSize;
	// ��������
	unsigned long long m_partionTotalSize;
	// diskType GetDriveType
	int diskType;
};

