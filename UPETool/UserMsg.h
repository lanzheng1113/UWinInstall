#pragma once
//寻找，校验ISO
#define WM_MYMSG_FindISOEnd (WM_USER + 100)
//GHOST备份
#define WM_MYMSG_GHOST_BACKUP_END (WM_USER + 101)
//GHOST恢复
#define WM_MYMSG_GHOST_RESTORE_END	(WM_USER + 102)
//ImageEx备份
#define WM_MYMSG_IMAGEX_BACKUP_END (WM_USER +103)

// 在Imagex还原时发生了错误
#define WM_UNEXCEPT_IMAGEX_RESTORE_ERROR (WM_USER + 104)

// 更新ImageX还原时的进度信息（百分比，剩余时间）
#define WM_PROGRESS_IMAGEX_RESTORE (WM_USER + 105)

// 报告IMAGEX安装对话框的总体进度（格式化、IMAGEX和添加引导）
#define WM_MYMSG_IMAGEX_INSTALL_TOTAL_PROGRESS (WM_USER + 106)

// 开始取消安装系统的操作
//#define WM_ENDING_TASK_START (WM_USER + 107)

// 取消系统安装的操作已经完成
//#define WM_ENDING_TASK_FINISH (WM_USER + 108)

#define WM_INSTALL_XP_RESULT (WM_USER+109)
