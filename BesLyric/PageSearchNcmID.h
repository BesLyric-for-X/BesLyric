/*
	BesLyric  一款 操作简单、功能实用的 专门用于制作网易云音乐滚动歌词的 歌词制作软件。
    Copyright (C) 2017  
	Author: BensonLaur <BensonLaur@163.com>
	Author:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       PageSearchNcmID.h
* 
* Describe    CPageSearchNcmID类，搜索文件对应的网易云音乐ID
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"
#include <helper/SAdapterBase.h>


//MC ListView 适配器类，用于处理 MCListView 的逻辑
class CSongIDMcAdapterFix : public SMcAdapterBase
{
private:
    struct SONG_ID_INFO
    {
        wstring strMusicName;		//歌名
        wstring strMusicArtist;		//歌手
        wstring strSongID;		//歌词ID
    };

    SArray<SONG_ID_INFO> m_IDInfo;

	SHostWnd*	m_mainWnd;

public:
    CSongIDMcAdapterFix(){}

	void AttachMainDlgPointer(SHostWnd* swindow){m_mainWnd = swindow;}

    virtual int getCount()
	{
        return m_IDInfo.GetCount();
    }   
    
    virtual void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
    {
        if(pItem->GetChildrenCount()==0)
        {
            pItem->InitFromXml(xmlTemplate);
        }
        SONG_ID_INFO *pli =m_IDInfo.GetData()+position;

		pItem->FindChildByName(L"txt_mclv_ncm_id_num")->SetWindowText(SStringW().Format(L"%02d",position + 1));
		pItem->FindChildByName(L"txt_mclv_music_name")->SetWindowText(SStringW().Format(L"%s",pli->strMusicName.c_str()));
		pItem->FindChildByName(L"txt_mclv_music_artist")->SetWindowText(SStringW().Format(L"%s",pli->strMusicArtist.c_str()));
		pItem->FindChildByName(L"txt_mclv_ncm_id")->SetWindowText(SStringW().Format(L"%s",pli->strSongID.c_str()));
		
        SButton *pBtnSelect = pItem->FindChildByName2<SButton>(L"btn_mclv_select");
        pBtnSelect->SetUserData(position);
        pBtnSelect->GetEventSet()->subscribeEvent(EVT_CMD,Subscriber(&CSongIDMcAdapterFix::OnButtonSelectClick,this));
        
    }

    bool OnButtonSelectClick(EventArgs *pEvt)
    {
        SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
        int iItem = pBtn->GetUserData();
        SONG_ID_INFO *pli =m_IDInfo.GetData()+iItem;
        
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_ncm_id)->SetWindowTextW( pli->strSongID.c_str());
        return true;
    }
	
	
    //删除一行，提供外部调用。
    void DeleteItem(int iPosition)
    {
        if(iPosition>=0 && iPosition<getCount())
        {
            m_IDInfo.RemoveAt(iPosition);
            notifyDataSetChanged();
        }
    }
	   
	//删除所有行，提供外部调用。
    void DeleteAllItem()
    {
		m_IDInfo.RemoveAll();
        notifyDataSetChanged();
    }
	
    //添加一行，提供外部调用。
	void AddItem(wstring strMusicName, wstring strMusicArtist, wstring strID)
	{
		SONG_ID_INFO IDInfo;
		IDInfo.strMusicName = strMusicName;
		IDInfo.strMusicArtist = strMusicArtist;
		IDInfo.strSongID = strID;
		m_IDInfo.Add(IDInfo);
	}

    virtual SStringW GetColumnName(int iCol) const{
        return SStringW().Format(L"col%d",iCol+1);
    }
    
    struct SORTCTX
    {
        int iCol;
        SHDSORTFLAG stFlag;
    };
    
    bool OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols)
    {
        if(iCol==0 || iCol==4) //第1列序号和最后一列 “操作”不支持排序
            return false;
        
        SHDSORTFLAG stFlag = stFlags[iCol];
        switch(stFlag)
        {
            case ST_NULL:stFlag = ST_UP;break;
            case ST_DOWN:stFlag = ST_UP;break;
            case ST_UP:stFlag = ST_DOWN;break;
        }
        for(int i=0;i<nCols;i++)
        {
            stFlags[i]=ST_NULL;
        }
        stFlags[iCol]=stFlag;
        
        SORTCTX ctx={iCol,stFlag};
        qsort_s(m_IDInfo.GetData(),m_IDInfo.GetCount(),sizeof(SONG_ID_INFO),SortCmp,&ctx);
        return true;
    }
    
    static int __cdecl SortCmp(void *context,const void * p1,const void * p2)
    {
        SORTCTX *pctx = (SORTCTX*)context;
        const SONG_ID_INFO *pLI1=(const SONG_ID_INFO*)p1;
        const SONG_ID_INFO *pLI2=(const SONG_ID_INFO*)p2;
        int nRet =0;
        switch(pctx->iCol)
        {
            case 0://序号
                break;
            case 1://strMusicName
				nRet = wcscmp(pLI1->strMusicName.c_str(), pLI2->strMusicName.c_str());
                break;
            case 2://strMusicArtist
				nRet = wcscmp(pLI1->strMusicArtist.c_str(), pLI2->strMusicArtist.c_str());
                break;
            case 3://strSongID
				nRet = wcscmp(pLI1->strSongID.c_str(), pLI2->strSongID.c_str());
                break;

            case 4://操作
                break;

        }
        if(pctx->stFlag == ST_UP)
            nRet = -nRet;
        return nRet;
    }
};


class CMainDlg;							//嵌套定义，先声明
class CPageSearchNcmID
{
	friend class CMainDlg;	

public:
	CPageSearchNcmID();
	~CPageSearchNcmID(){};

	void Init(SHostWnd *pMainWnd);		//初始化设置页面
	CMainDlg* M();					//获得主窗口类实例对象
	
	void OnBtnReturnMain();			//返回主要窗口页面

	//使用文件路径的文件名填充歌曲文件名编辑控件
	void SetSongFileNameEditWithPath(wstring strFilePath);

	//选择ID作为当前歌名的ncm ID
	void OnBtnSelectID();


	//搜索歌ID
	void OnBtnSearch();
	void ShowIDResult(IDSearchResult* pResult);
//	
//
	//根据猜测结果自动填充搜索关键词并开始搜索
	void OnSearchWithGuess(SongInfoGuessResult* pGuessRes);
//
//	
//
private:
	//获得界面上填写的歌曲名和歌手名
	bool GetMusicAndArtist(SStringW &strMusicName, SStringW &strMusicArtist);


	//消息
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_return_to_main_page , OnBtnReturnMain)
		EVENT_ID_COMMAND(R.id.btn_ncm_id_select , OnBtnSelectID)
		
		EVENT_ID_COMMAND(R.id.btn_search_ncm_id , OnBtnSearch)

		//EVENT_ID_COMMAND(R.id.btn_search_lyric_using_program , OnBtnSearchInProgram)
		//
		//EVENT_ID_COMMAND(R.id.btn_select_origin_lyric_path , OnBtnSelectOriginLyricPath);
		//EVENT_ID_COMMAND(R.id.btn_select_lrc_lyric_path , OnBtnSelectLrcLyricPath);
		//EVENT_ID_COMMAND(R.id.btn_save_origin_lyric_path , OnBtnSaveOriginLyricPath);
		//EVENT_ID_COMMAND(R.id.btn_save_lrc_lyric_path , OnBtnSaveLrcLyricPath);
		//
		//EVENT_ID_HANDLER(R.id.edit_search_lyric_name, EVT_RE_NOTIFY, OnEditChanged);
		//EVENT_ID_HANDLER(R.id.edit_search_lyric_artist, EVT_RE_NOTIFY, OnEditChanged);
	EVENT_MAP_BREAK()

	
private:

	//控件指针
	SWindow *m_window_search_ncm_id_tip;
	SEdit	*m_editSongFileName; 
	SEdit	*m_editNcmID; 
	
	SEdit	*m_editSearchName;
	SEdit	*m_editSearchArtist;

	SStatic *m_txtSearchNameTip;
	SStatic *m_txtSearchArtistTip;
	SWindow *m_wndSearchArtistTip;
	SStatic *m_txtSearchResultTip;
	SWindow *m_txtIsSearchingIDTip;


	SEdit	*m_editLrcLyricPath;
	
	SButton *m_btnSelectOriginLyricPath;
	SButton *m_btnSelectLrcLyricPath;
	
	SEdit	*m_editOriginLyricName;
	SEdit	*m_editOriginLyricArtist;
	SEdit	*m_editLrcLyricName;
	SEdit	*m_editLrcLyricArtist;
	
	SEdit	*m_editOriginLyricContent;
	SEdit	*m_editLrcLyricContent;

	CSongIDMcAdapterFix*	m_IDListAdapter;

	
	SHostWnd *m_pMainWnd;		/* 主窗口指针 */
};