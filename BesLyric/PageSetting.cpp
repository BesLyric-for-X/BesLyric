#include "stdafx.h"
#include "PageSetting.h"
#include <fstream>
#include "utility\WinDialog.h"
#include "utility\WinFile.h"
#include "lib\xml\tinyxml2.h"
#include <sstream>
#include "entity\AutoUpdateThread.h"
#include "entity\CheckIntegrityThread.h"
#include "entity\SendLoginThread.h"
using namespace std;
using namespace tinyxml2;

static const wstring XML_TRUE = L"1";
static const wstring XML_FALSE = L"0";
static const wstring XML_SHIFT_TIME = L"shiftTime";							//时间轴偏移量
static const wstring XML_DEFAULF_MUSIC_PATH = L"defaultMusicPath";			//默认音乐路径
static const wstring XML_DEFAULF_LYRIC_PATH = L"defaultLyricPath";			//默认歌词路径
static const wstring XML_DEFAULF_OUTPUT_PATH = L"defaultOutputPath";		//默认输出路径
static const wstring XML_AUTO_UPDATE = L"autoUpdate";						//是否自动升级
static const wstring XML_ANONYMITY = L"anonymity";							//是否匿名


void CSettingPage::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//保存主窗口对象
	m_pMainWnd = pMainWnd;

	//获取控件指针
	btn_modify_shift_time = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_modify_setting_shift_time);
	edit_time_shift = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_time_shift);

	text_default_music_path = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_default_music_path);	
	text_default_lyric_path = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_default_lyric_path);	
	text_default_output_path = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_default_output_path);		
	check_auto_update = m_pMainWnd->FindChildByID2<SCheckBox>(R.id.check_auto_update);	
	check_anonymity = m_pMainWnd->FindChildByID2<SCheckBox>(R.id.check_anonymity);

	SASSERT( btn_modify_shift_time != NULL);
	SASSERT( edit_time_shift != NULL);
	SASSERT( text_default_music_path != NULL);
	SASSERT( text_default_lyric_path != NULL);
	SASSERT( text_default_output_path != NULL);
	SASSERT( check_auto_update != NULL);
	SASSERT( check_anonymity != NULL);

	
	//初始化界面数据
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));
		
	//初始禁用修改按钮
	btn_modify_shift_time->EnableWindow(FALSE, FALSE);
	
	text_default_music_path->SetWindowTextW(SStringW(m_default_music_path.c_str()));
	text_default_lyric_path->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));
	text_default_output_path->SetWindowTextW(SStringW(m_default_output_path.c_str()));

	//初始化界面check 和 生效其作用
	check_auto_update->SetCheck(m_check_auto_update);
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);

	check_anonymity->SetCheck(m_check_anonymity);
	SendLoginThread::getSingleton().Start(m_check_anonymity);
}


//保存设置
void CSettingPage::SaveSetting()
{
	wstring a;
	wstringstream ss(a);  //创建一个流
	ss << m_nTimeShift;  //把值传递如流中

	wstring xmlStr = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xmlStr += L"<setting>\n";
	xmlStr += L"\t<"+XML_SHIFT_TIME+L" value=\""+	ss.str() +L"\"/>\n";
	xmlStr += L"\t<"+XML_DEFAULF_MUSIC_PATH+L" value=\""+	m_default_music_path +L"\"/>\n";
	xmlStr += L"\t<"+XML_DEFAULF_LYRIC_PATH+L" value=\""+	m_default_lyric_path +L"\"/>\n";
	xmlStr += L"\t<"+XML_DEFAULF_OUTPUT_PATH+L" value=\""+ m_default_output_path +L"\"/>\n";
	xmlStr += L"\t<"+XML_AUTO_UPDATE+		L" value=\""+ (m_check_auto_update? XML_TRUE : XML_FALSE) +L"\"/>\n";
	xmlStr += L"\t<"+XML_ANONYMITY+		L" value=\""+ (m_check_anonymity? XML_TRUE : XML_FALSE) +L"\"/>\n";
	
	xmlStr += L"</setting>\n";
	
	wstring wstrPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" +SETTING_FILE_NAME;
	FileOperator::WriteToUtf8File( wstrPath,xmlStr);
}

//加载设置
void CSettingPage::LoadSetting()
{
	//先进行默认初始化，初始化默认设置
	m_nTimeShift = 300;			
	m_default_music_path = L"";
	m_default_lyric_path = L"";
	m_default_output_path = L"";
	m_check_auto_update = TRUE;	
	m_check_anonymity = FALSE;	

	//从文件加载数据
	wstring wstrSettingPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + SETTING_FILE_NAME;
	string strSettingPath = S_CW2A(SStringW(wstrSettingPath.c_str()));
	if(FileHelper::CheckFileExist(wstrSettingPath))
	{
		//读取XML文件
		tinyxml2::XMLDocument doc;
		doc.LoadFile(strSettingPath.c_str());
		if(doc.Error())
		{
			return;
		}

		//根
		XMLElement *pRoot = doc.RootElement();
		if(pRoot)
		{
			XMLElement* ele = pRoot->FirstChildElement();
			while(ele)
			{
				const char* szName = ele->Name();
				const char* szValue = ele->Attribute("value");
			
				wstring wStrName = S_CA2W(SStringA(szName));
				WCHAR wszValue[MAX_BUFFER_SIZE] = {0};
				if(szValue)
				{
					//由于文件保存是以utf-8保存，需要转换
					::MultiByteToWideChar(CP_UTF8,0,szValue, MAX_BUFFER_SIZE/2, wszValue, MAX_BUFFER_SIZE);
				}

				if (XML_SHIFT_TIME == wStrName)
				{
					//时间轴偏移量
					m_nTimeShift = _wtoi(wszValue);
				}
				else if (XML_DEFAULF_MUSIC_PATH == wStrName)
				{
					//默认音乐路径
					m_default_music_path = wszValue;
				}
				else if ( XML_DEFAULF_LYRIC_PATH == wStrName)
				{
					//默认歌词路径
					m_default_lyric_path = wszValue;
				}
				else if (XML_DEFAULF_OUTPUT_PATH == wStrName)
				{
					//默认输出路径
					m_default_output_path = wszValue;
				}
				else if (XML_AUTO_UPDATE == wStrName)
				{
					//是否自动升级
					m_check_auto_update = (_wtoi(wszValue)? TRUE: FALSE);
				}
				else if(XML_ANONYMITY == wStrName)
				{
					//是否匿名
					m_check_anonymity = (_wtoi(wszValue)? TRUE: FALSE);
				}

				//下一兄弟结点
				ele = ele->NextSiblingElement();
			}
		}
		else
			return;
	}
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
}

//选择默认音乐路径
void CSettingPage::OnBtnSelectDefaultMusicPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【音乐文件】 默认路径"), TRUE, m_default_music_path.c_str());
	if(bRet == TRUE)
	{
		m_default_music_path = SStringW(Browser.GetDirPath()).GetBuffer(1);
		m_pMainWnd->FindChildByID(R.id.text_default_music_path)->SetWindowTextW(SStringW(m_default_music_path.c_str()));
	}
}

//选择默认歌词路径
void CSettingPage::OnBtnSelectDefaultLyricPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【歌词文件】 默认路径"), TRUE, m_default_lyric_path.c_str());
	if(bRet == TRUE)
	{
		m_default_lyric_path = SStringW(Browser.GetDirPath()).GetBuffer(1);
		m_pMainWnd->FindChildByID(R.id.text_default_lyric_path)->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));//填到对应的显示text
		
		m_pMainWnd->FindChildByID(R.id.edit_origin_lyric_path)->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));//填到歌词选择页面的源歌词保存路径
		
	}
	
}
	
//选择默认LRC歌词输出路径
void CSettingPage::OnBtnSelectDefaultOutputPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【LRC歌词文件】 默认输出路径"), TRUE, m_default_output_path.c_str());
	if(bRet == TRUE)
	{
		m_default_output_path = SStringW(Browser.GetDirPath()).GetBuffer(1);
		m_pMainWnd->FindChildByID(R.id.text_default_output_path)->SetWindowTextW(SStringW(m_default_output_path.c_str()));//填到对应的显示text
		
		m_pMainWnd->FindChildByID(R.id.edit_lrc_lyric_path)->SetWindowTextW(SStringW(m_default_lyric_path.c_str()));//填到歌词选择页面的源歌词保存路径
	}
}
			
//自动升级check 改变状态时
void CSettingPage::OnCheckAutoUpdateChanged()
{
	m_check_auto_update = check_auto_update->IsChecked();
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);
}
		
//匿名登录选项 改变状态时
void CSettingPage::OnCheckAnonymityChanged()
{
	m_check_anonymity = check_anonymity->IsChecked();
}


//检测程序完整性
void CSettingPage::OnBtnCheckIntegrity()
{
	CCheckIntegrityThread::getSingleton().Start(true);
}