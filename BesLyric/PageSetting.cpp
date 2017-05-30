#include "stdafx.h"
#include "PageSetting.h"
#include <fstream>
using namespace std;

void CSettingPage::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//保存主窗口对象
	m_pMainWnd = pMainWnd;

	//获取控件指针
	btn_modify_shift_time = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_modify_setting_shift_time);
	SASSERT( btn_modify_shift_time != NULL);
	edit_time_shift = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_time_shift);
	SASSERT( edit_time_shift != NULL);
		
	//初始禁用修改按钮
	if(btn_modify_shift_time)
		btn_modify_shift_time->EnableWindow(FALSE, FALSE);

	//从文件获取获取时间，显示到edit_time_shift 编辑框中
	LoadShiftTime();
}

//从文件获取获取时间，显示到edit_time_shift 编辑框中
void CSettingPage::LoadShiftTime()
{
	//获得exe文件目录
	wchar_t exeFullPath[MAX_PATH]; // Full path   
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	string strPath(S_CW2A(SStringW(exeFullPath)).GetBuffer(1));
	strPath = strPath.substr(0, strPath.find_last_of("\\")+1);

	//得到储存路径
	strPath += SETTING_FILE_NAME;

	fstream storageFile;
	storageFile.open(strPath, ios::in);
	if(storageFile)//成功打开文件
	{
		char msTime[100] = {'\0'};
		storageFile.getline(msTime, 100-1);
	
		m_nTimeShift = atoi(msTime);
	}
	else
	{
		m_nTimeShift = 0;
		//创建新的文件，写入偏移时间
		storageFile.close();
		storageFile.open(strPath, ios::out);
		if(storageFile){
			storageFile << m_nTimeShift;
			storageFile.close();
		}
	}
	
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));
}

//偏移时间编辑框通知响应
void CSettingPage::OnTimeShiftEditNotify(EventArgs *pEvt)
{
	EventRENotify* Evt = sobj_cast<EventRENotify>(pEvt);
	if(Evt)
	{
		if(Evt->iNotify==EN_CHANGE )
		{
			string strTime = SStringA(S_CW2A(edit_time_shift->GetWindowTextW())).GetBuffer(1);
			int nTime = atoi(strTime.c_str());
			if(nTime != m_nTimeShift) //不同于先前保存的值
			{
				//启用用修改按钮
				if(btn_modify_shift_time)
					btn_modify_shift_time->EnableWindow(TRUE, TRUE);
			}
			else
			{
				//禁用修改按钮
				if(btn_modify_shift_time)
					btn_modify_shift_time->EnableWindow(FALSE, TRUE);
			}
		}

	}
}

//修改偏移时间
void CSettingPage::OnBtnModifyShiftTime()
{
	string strTime = SStringA(S_CW2A(edit_time_shift->GetWindowTextW())).GetBuffer(1);
	int nTime = atoi(strTime.c_str());
	m_nTimeShift = nTime;
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));

	//保存到设置文件中
	//获得exe文件目录
	wchar_t exeFullPath[MAX_PATH]; // Full path   
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	string strPath(S_CW2A(SStringW(exeFullPath)).GetBuffer(1));
	strPath = strPath.substr(0, strPath.find_last_of("\\")+1);

	//得到储存路径
	strPath += SETTING_FILE_NAME;
	
	fstream storageFile;
	storageFile.open(strPath, ios::out);
	if(storageFile){
			storageFile << m_nTimeShift;
			storageFile.close();
	}
}