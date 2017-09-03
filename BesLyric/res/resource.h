//stamp:0af19fd316ed9943
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
				xml_messagebox = _T("LAYOUT:xml_messagebox");
				xml_check_integrity = _T("LAYOUT:xml_check_integrity");
			}
			const TCHAR * XML_MAINWND;
			const TCHAR * xml_messagebox;
			const TCHAR * xml_check_integrity;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMGX{
			public:
			_IMGX(){
				png_tab_main = _T("IMGX:png_tab_main");
				png_page_icons = _T("IMGX:png_page_icons");
				png_key_up = _T("IMGX:png_key_up");
				png_key_left = _T("IMGX:png_key_left");
				png_key_right = _T("IMGX:png_key_right");
				png_key_back = _T("IMGX:png_key_back");
				png_key_space = _T("IMGX:png_key_space");
				png_key_return = _T("IMGX:png_key_return");
				png_netease_button = _T("IMGX:png_netease_button");
				png_blue_button = _T("IMGX:png_blue_button");
				png_progress_frontground = _T("IMGX:png_progress_frontground");
				png_progress_background = _T("IMGX:png_progress_background");
				png_slider_frontground = _T("IMGX:png_slider_frontground");
				png_slider_background = _T("IMGX:png_slider_background");
				png_slider_thumb = _T("IMGX:png_slider_thumb");
				png_sound_open = _T("IMGX:png_sound_open");
				png_sound_close = _T("IMGX:png_sound_close");
				img_information = _T("IMGX:img_information");
				img_question = _T("IMGX:img_question");
				img_exclamation = _T("IMGX:img_exclamation");
				png_check = _T("IMGX:png_check");
			}
			const TCHAR * png_tab_main;
			const TCHAR * png_page_icons;
			const TCHAR * png_key_up;
			const TCHAR * png_key_left;
			const TCHAR * png_key_right;
			const TCHAR * png_key_back;
			const TCHAR * png_key_space;
			const TCHAR * png_key_return;
			const TCHAR * png_netease_button;
			const TCHAR * png_blue_button;
			const TCHAR * png_progress_frontground;
			const TCHAR * png_progress_background;
			const TCHAR * png_slider_frontground;
			const TCHAR * png_slider_background;
			const TCHAR * png_slider_thumb;
			const TCHAR * png_sound_open;
			const TCHAR * png_sound_close;
			const TCHAR * img_information;
			const TCHAR * img_question;
			const TCHAR * img_exclamation;
			const TCHAR * png_check;
		}IMGX;
		class _BackGround{
			public:
			_BackGround(){
				background_pic = _T("BackGround:background_pic");
				msg_background_1 = _T("BackGround:msg_background_1");
				msg_background_2 = _T("BackGround:msg_background_2");
			}
			const TCHAR * background_pic;
			const TCHAR * msg_background_1;
			const TCHAR * msg_background_2;
		}BackGround;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"1_line_1",65553},
		{L"1_line_2",65000},
		{L"1_line_3",65554},
		{L"2_line_1",65574},
		{L"2_line_2",65001},
		{L"2_line_3",65575},
		{L"btnSwitch",65602},
		{L"btn_cancel_intigrity_check",65609},
		{L"btn_check_integrity",65594},
		{L"btn_check_integrity_close",65606},
		{L"btn_close",65537},
		{L"btn_edit_origin_lyric",65545},
		{L"btn_end_preview",65581},
		{L"btn_load_1",65549},
		{L"btn_load_2",65569},
		{L"btn_manual_adjust",65579},
		{L"btn_max",65538},
		{L"btn_min",65540},
		{L"btn_modify_setting_shift_time",65586},
		{L"btn_open_output",65559},
		{L"btn_preview",65558},
		{L"btn_restart",65561},
		{L"btn_restore",65539},
		{L"btn_select_default_lyric_path",65589},
		{L"btn_select_default_music_path",65587},
		{L"btn_select_default_output_path",65591},
		{L"btn_select_lyric_1",65546},
		{L"btn_select_lyric_2",65568},
		{L"btn_select_music_1",65543},
		{L"btn_select_music_2",65566},
		{L"btn_select_output",65548},
		{L"btn_sound_close_1",65563},
		{L"btn_sound_close_2",65583},
		{L"btn_sound_open_1",65562},
		{L"btn_sound_open_2",65582},
		{L"btn_start_making",65560},
		{L"btn_start_playing",65580},
		{L"button1st",65603},
		{L"button2nd",65604},
		{L"button3rd",65605},
		{L"check_auto_update",65593},
		{L"edit_lyric_1",65544},
		{L"edit_lyric_2",65567},
		{L"edit_music_1",65542},
		{L"edit_music_2",65565},
		{L"edit_output",65547},
		{L"edit_time_shift",65585},
		{L"empty_tip_block",65556},
		{L"empty_tip_block_2",65577},
		{L"highlight_bkgnd",65552},
		{L"highlight_bkgnd_2",65573},
		{L"img_icon_exclamation",65600},
		{L"img_icon_information",65598},
		{L"img_icon_question",65599},
		{L"img_skin_layer",65536},
		{L"line_lyric_1",65551},
		{L"line_lyric_2",65571},
		{L"line_music_1",65550},
		{L"line_music_2",65570},
		{L"msgicon",65597},
		{L"msgtext",65601},
		{L"msgtitle",65596},
		{L"progress_check_integrity",65608},
		{L"progress_music_1",65557},
		{L"progress_music_2",65578},
		{L"slider_sound_1",65564},
		{L"slider_sound_2",65584},
		{L"tab_main",65541},
		{L"text_default_lyric_path",65590},
		{L"text_default_music_path",65588},
		{L"text_default_output_path",65592},
		{L"text_status_intigrity_check",65607},
		{L"text_time_1",65555},
		{L"text_time_2",65576},
		{L"txt_version_number",65595},
		{L"wnd_lyric_display",65572}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			name_1_line_1 = namedXmlID[0].strName;
			name_1_line_2 = namedXmlID[1].strName;
			name_1_line_3 = namedXmlID[2].strName;
			name_2_line_1 = namedXmlID[3].strName;
			name_2_line_2 = namedXmlID[4].strName;
			name_2_line_3 = namedXmlID[5].strName;
			btnSwitch = namedXmlID[6].strName;
			btn_cancel_intigrity_check = namedXmlID[7].strName;
			btn_check_integrity = namedXmlID[8].strName;
			btn_check_integrity_close = namedXmlID[9].strName;
			btn_close = namedXmlID[10].strName;
			btn_edit_origin_lyric = namedXmlID[11].strName;
			btn_end_preview = namedXmlID[12].strName;
			btn_load_1 = namedXmlID[13].strName;
			btn_load_2 = namedXmlID[14].strName;
			btn_manual_adjust = namedXmlID[15].strName;
			btn_max = namedXmlID[16].strName;
			btn_min = namedXmlID[17].strName;
			btn_modify_setting_shift_time = namedXmlID[18].strName;
			btn_open_output = namedXmlID[19].strName;
			btn_preview = namedXmlID[20].strName;
			btn_restart = namedXmlID[21].strName;
			btn_restore = namedXmlID[22].strName;
			btn_select_default_lyric_path = namedXmlID[23].strName;
			btn_select_default_music_path = namedXmlID[24].strName;
			btn_select_default_output_path = namedXmlID[25].strName;
			btn_select_lyric_1 = namedXmlID[26].strName;
			btn_select_lyric_2 = namedXmlID[27].strName;
			btn_select_music_1 = namedXmlID[28].strName;
			btn_select_music_2 = namedXmlID[29].strName;
			btn_select_output = namedXmlID[30].strName;
			btn_sound_close_1 = namedXmlID[31].strName;
			btn_sound_close_2 = namedXmlID[32].strName;
			btn_sound_open_1 = namedXmlID[33].strName;
			btn_sound_open_2 = namedXmlID[34].strName;
			btn_start_making = namedXmlID[35].strName;
			btn_start_playing = namedXmlID[36].strName;
			button1st = namedXmlID[37].strName;
			button2nd = namedXmlID[38].strName;
			button3rd = namedXmlID[39].strName;
			check_auto_update = namedXmlID[40].strName;
			edit_lyric_1 = namedXmlID[41].strName;
			edit_lyric_2 = namedXmlID[42].strName;
			edit_music_1 = namedXmlID[43].strName;
			edit_music_2 = namedXmlID[44].strName;
			edit_output = namedXmlID[45].strName;
			edit_time_shift = namedXmlID[46].strName;
			empty_tip_block = namedXmlID[47].strName;
			empty_tip_block_2 = namedXmlID[48].strName;
			highlight_bkgnd = namedXmlID[49].strName;
			highlight_bkgnd_2 = namedXmlID[50].strName;
			img_icon_exclamation = namedXmlID[51].strName;
			img_icon_information = namedXmlID[52].strName;
			img_icon_question = namedXmlID[53].strName;
			img_skin_layer = namedXmlID[54].strName;
			line_lyric_1 = namedXmlID[55].strName;
			line_lyric_2 = namedXmlID[56].strName;
			line_music_1 = namedXmlID[57].strName;
			line_music_2 = namedXmlID[58].strName;
			msgicon = namedXmlID[59].strName;
			msgtext = namedXmlID[60].strName;
			msgtitle = namedXmlID[61].strName;
			progress_check_integrity = namedXmlID[62].strName;
			progress_music_1 = namedXmlID[63].strName;
			progress_music_2 = namedXmlID[64].strName;
			slider_sound_1 = namedXmlID[65].strName;
			slider_sound_2 = namedXmlID[66].strName;
			tab_main = namedXmlID[67].strName;
			text_default_lyric_path = namedXmlID[68].strName;
			text_default_music_path = namedXmlID[69].strName;
			text_default_output_path = namedXmlID[70].strName;
			text_status_intigrity_check = namedXmlID[71].strName;
			text_time_1 = namedXmlID[72].strName;
			text_time_2 = namedXmlID[73].strName;
			txt_version_number = namedXmlID[74].strName;
			wnd_lyric_display = namedXmlID[75].strName;
		}
		 const wchar_t * name_1_line_1;
		 const wchar_t * name_1_line_2;
		 const wchar_t * name_1_line_3;
		 const wchar_t * name_2_line_1;
		 const wchar_t * name_2_line_2;
		 const wchar_t * name_2_line_3;
		 const wchar_t * btnSwitch;
		 const wchar_t * btn_cancel_intigrity_check;
		 const wchar_t * btn_check_integrity;
		 const wchar_t * btn_check_integrity_close;
		 const wchar_t * btn_close;
		 const wchar_t * btn_edit_origin_lyric;
		 const wchar_t * btn_end_preview;
		 const wchar_t * btn_load_1;
		 const wchar_t * btn_load_2;
		 const wchar_t * btn_manual_adjust;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_modify_setting_shift_time;
		 const wchar_t * btn_open_output;
		 const wchar_t * btn_preview;
		 const wchar_t * btn_restart;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_select_default_lyric_path;
		 const wchar_t * btn_select_default_music_path;
		 const wchar_t * btn_select_default_output_path;
		 const wchar_t * btn_select_lyric_1;
		 const wchar_t * btn_select_lyric_2;
		 const wchar_t * btn_select_music_1;
		 const wchar_t * btn_select_music_2;
		 const wchar_t * btn_select_output;
		 const wchar_t * btn_sound_close_1;
		 const wchar_t * btn_sound_close_2;
		 const wchar_t * btn_sound_open_1;
		 const wchar_t * btn_sound_open_2;
		 const wchar_t * btn_start_making;
		 const wchar_t * btn_start_playing;
		 const wchar_t * button1st;
		 const wchar_t * button2nd;
		 const wchar_t * button3rd;
		 const wchar_t * check_auto_update;
		 const wchar_t * edit_lyric_1;
		 const wchar_t * edit_lyric_2;
		 const wchar_t * edit_music_1;
		 const wchar_t * edit_music_2;
		 const wchar_t * edit_output;
		 const wchar_t * edit_time_shift;
		 const wchar_t * empty_tip_block;
		 const wchar_t * empty_tip_block_2;
		 const wchar_t * highlight_bkgnd;
		 const wchar_t * highlight_bkgnd_2;
		 const wchar_t * img_icon_exclamation;
		 const wchar_t * img_icon_information;
		 const wchar_t * img_icon_question;
		 const wchar_t * img_skin_layer;
		 const wchar_t * line_lyric_1;
		 const wchar_t * line_lyric_2;
		 const wchar_t * line_music_1;
		 const wchar_t * line_music_2;
		 const wchar_t * msgicon;
		 const wchar_t * msgtext;
		 const wchar_t * msgtitle;
		 const wchar_t * progress_check_integrity;
		 const wchar_t * progress_music_1;
		 const wchar_t * progress_music_2;
		 const wchar_t * slider_sound_1;
		 const wchar_t * slider_sound_2;
		 const wchar_t * tab_main;
		 const wchar_t * text_default_lyric_path;
		 const wchar_t * text_default_music_path;
		 const wchar_t * text_default_output_path;
		 const wchar_t * text_status_intigrity_check;
		 const wchar_t * text_time_1;
		 const wchar_t * text_time_2;
		 const wchar_t * txt_version_number;
		 const wchar_t * wnd_lyric_display;
		}name;

		class _id{
		public:
		const static int name_1_line_1	=	65553;
		const static int name_1_line_2	=	65000;
		const static int name_1_line_3	=	65554;
		const static int name_2_line_1	=	65574;
		const static int name_2_line_2	=	65001;
		const static int name_2_line_3	=	65575;
		const static int btnSwitch	=	65602;
		const static int btn_cancel_intigrity_check	=	65609;
		const static int btn_check_integrity	=	65594;
		const static int btn_check_integrity_close	=	65606;
		const static int btn_close	=	65537;
		const static int btn_edit_origin_lyric	=	65545;
		const static int btn_end_preview	=	65581;
		const static int btn_load_1	=	65549;
		const static int btn_load_2	=	65569;
		const static int btn_manual_adjust	=	65579;
		const static int btn_max	=	65538;
		const static int btn_min	=	65540;
		const static int btn_modify_setting_shift_time	=	65586;
		const static int btn_open_output	=	65559;
		const static int btn_preview	=	65558;
		const static int btn_restart	=	65561;
		const static int btn_restore	=	65539;
		const static int btn_select_default_lyric_path	=	65589;
		const static int btn_select_default_music_path	=	65587;
		const static int btn_select_default_output_path	=	65591;
		const static int btn_select_lyric_1	=	65546;
		const static int btn_select_lyric_2	=	65568;
		const static int btn_select_music_1	=	65543;
		const static int btn_select_music_2	=	65566;
		const static int btn_select_output	=	65548;
		const static int btn_sound_close_1	=	65563;
		const static int btn_sound_close_2	=	65583;
		const static int btn_sound_open_1	=	65562;
		const static int btn_sound_open_2	=	65582;
		const static int btn_start_making	=	65560;
		const static int btn_start_playing	=	65580;
		const static int button1st	=	65603;
		const static int button2nd	=	65604;
		const static int button3rd	=	65605;
		const static int check_auto_update	=	65593;
		const static int edit_lyric_1	=	65544;
		const static int edit_lyric_2	=	65567;
		const static int edit_music_1	=	65542;
		const static int edit_music_2	=	65565;
		const static int edit_output	=	65547;
		const static int edit_time_shift	=	65585;
		const static int empty_tip_block	=	65556;
		const static int empty_tip_block_2	=	65577;
		const static int highlight_bkgnd	=	65552;
		const static int highlight_bkgnd_2	=	65573;
		const static int img_icon_exclamation	=	65600;
		const static int img_icon_information	=	65598;
		const static int img_icon_question	=	65599;
		const static int img_skin_layer	=	65536;
		const static int line_lyric_1	=	65551;
		const static int line_lyric_2	=	65571;
		const static int line_music_1	=	65550;
		const static int line_music_2	=	65570;
		const static int msgicon	=	65597;
		const static int msgtext	=	65601;
		const static int msgtitle	=	65596;
		const static int progress_check_integrity	=	65608;
		const static int progress_music_1	=	65557;
		const static int progress_music_2	=	65578;
		const static int slider_sound_1	=	65564;
		const static int slider_sound_2	=	65584;
		const static int tab_main	=	65541;
		const static int text_default_lyric_path	=	65590;
		const static int text_default_music_path	=	65588;
		const static int text_default_output_path	=	65592;
		const static int text_status_intigrity_check	=	65607;
		const static int text_time_1	=	65555;
		const static int text_time_2	=	65576;
		const static int txt_version_number	=	65595;
		const static int wnd_lyric_display	=	65572;
		}id;

		class _string{
		public:
		const static int title	=	0;
		const static int ver	=	1;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int gray	=	1;
		const static int green	=	2;
		const static int lightGray	=	3;
		const static int lightGrayTrans	=	4;
		const static int red	=	5;
		const static int white	=	6;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
