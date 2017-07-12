#include "stdafx.h"
#include "PageSetting.h"
#include <fstream>
#include "FileHelper.h"
#include "WinFile.h"
#include "lib\tinyxml2.h"
#include <sstream>
#include "AutoUpdateThread.h"
using namespace std;
using namespace tinyxml2;

static const string XML_TRUE = "1";
static const string XML_FALSE = "0";
static const string XML_SHIFT_TIME = "shiftTime";							//时间轴偏移量
static const string XML_DEFAULF_MUSIC_PATH = "defaultMusicPath";			//默认音乐路径
static const string XML_DEFAULF_LYRIC_PATH = "defaultLyricPath";			//默认歌词路径
static const string XML_DEFAULF_OUTPUT_PATH = "defaultOutputPath";			//默认输出路径
static const string XML_AUTO_UPDATE = "autoUpdate";							//是否自动升级


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
	
	SASSERT( btn_modify_shift_time != NULL);
	SASSERT( edit_time_shift != NULL);
	SASSERT( text_default_music_path != NULL);
	SASSERT( text_default_lyric_path != NULL);
	SASSERT( text_default_output_path != NULL);
	SASSERT( check_auto_update != NULL);

	
	//初始化界面数据
	edit_time_shift->SetWindowTextW(SStringW(L"").Format(L"%d",m_nTimeShift));
		
	//初始禁用修改按钮
	btn_modify_shift_time->EnableWindow(FALSE, FALSE);
	
	text_default_music_path->SetWindowTextW(S_CA2W(SStringA(m_default_music_path.c_str())));
	text_default_lyric_path->SetWindowTextW(S_CA2W(SStringA(m_default_lyric_path.c_str())));
	text_default_output_path->SetWindowTextW(S_CA2W(SStringA(m_default_output_path.c_str())));

	//初始化界面check 和 生效其作用
	check_auto_update->SetCheck(m_check_auto_update);
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);
}


//保存设置
void CSettingPage::SaveSetting()
{
	string a;
	stringstream ss(a);  //创建一个流
	ss << m_nTimeShift;  //把值传递如流中

	string xmlStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xmlStr += "<setting>\n";
	xmlStr += "\t<"+XML_SHIFT_TIME+" value=\""+	ss.str() +"\"/>\n";
	xmlStr += "\t<"+XML_DEFAULF_MUSIC_PATH+" value=\""+	m_default_music_path +"\"/>\n";
	xmlStr += "\t<"+XML_DEFAULF_LYRIC_PATH+" value=\""+	m_default_lyric_path +"\"/>\n";
	xmlStr += "\t<"+XML_DEFAULF_OUTPUT_PATH+" value=\""+ m_default_output_path +"\"/>\n";
	xmlStr += "\t<"+XML_AUTO_UPDATE+		" value=\""+ (m_check_auto_update? XML_TRUE : XML_FALSE) +"\"/>\n";
	xmlStr += "</setting>\n";

	FileOperator::SaveToFile(FileHelper::GetCurrentDirectoryStr() + SETTING_FILE_NAME, xmlStr);
}

//加载设置
void CSettingPage::LoadSetting()
{
	//先进行默认初始化，初始化默认设置
	m_nTimeShift = 300;			
	m_default_music_path = "";
	m_default_lyric_path = "";
	m_default_output_path = "";
	m_check_auto_update = TRUE;	

	//从文件加载数据
	string strSettingPath = FileHelper::GetCurrentDirectoryStr() + SETTING_FILE_NAME;
	if(FileHelper::CheckFileExist(strSettingPath))
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
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		string strName = ("");
		while(ele)
		{
			strName = ele->Name();
			string value = ele->Attribute("value");
			if (XML_SHIFT_TIME == strName)
			{
				//时间轴偏移量
				m_nTimeShift = atoi(value.c_str());
			}
			else if (XML_DEFAULF_MUSIC_PATH == strName)
			{
				//默认音乐路径
				m_default_music_path = value;
			}
			else if ( XML_DEFAULF_LYRIC_PATH == strName)
			{
				//默认歌词路径
				m_default_lyric_path = value;
			}
			else if (XML_DEFAULF_OUTPUT_PATH == strName)
			{
				//默认输出路径
				m_default_output_path = value;
			}
			else if (XML_AUTO_UPDATE == strName)
			{
				//是否自动升级
				m_check_auto_update = (atoi(value.c_str())? TRUE: FALSE);
			}

			//下一兄弟结点
			ele = ele->NextSiblingElement();
		}
	}
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
}

//选择默认音乐路径
void CSettingPage::OnBtnSelectDefaultMusicPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【音乐文件】 默认路径"), TRUE);
	if(bRet == TRUE)
	{
		m_default_music_path = string(Browser.GetDirPath());
		m_pMainWnd->FindChildByID(R.id.text_default_music_path)->SetWindowTextW(S_CA2W(SStringA(m_default_music_path.c_str())));
	}
}

//选择默认歌词路径
void CSettingPage::OnBtnSelectDefaultLyricPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【歌词文件】 默认路径"), TRUE);
	if(bRet == TRUE)
	{
		m_default_lyric_path = string(Browser.GetDirPath());
		m_pMainWnd->FindChildByID(R.id.text_default_lyric_path)->SetWindowTextW(S_CA2W(SStringA(m_default_lyric_path.c_str())));
	}
	
}
	
//选择默认LRC歌词输出路径
void CSettingPage::OnBtnSelectDefaultOutputPath()
{
	CBrowseDlg Browser;
	BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【LRC歌词文件】 默认输出路径"), TRUE);
	if(bRet == TRUE)
	{
		m_default_output_path = string(Browser.GetDirPath());
		m_pMainWnd->FindChildByID(R.id.text_default_output_path)->SetWindowTextW(S_CA2W(SStringA(m_default_output_path.c_str())));
	}
}
			
//自动升级check 改变状态时
void CSettingPage::OnCheckAutoUpdateChanged()
{
	m_check_auto_update = check_auto_update->IsChecked();
	AutoUpdateThread::getSingleton().SetBKeepUpdate(m_check_auto_update);
}