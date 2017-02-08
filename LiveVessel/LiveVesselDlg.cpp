
// LiveVesselDlg.cpp : 구현 파일
//





#include "stdafx.h"
#include "LiveVessel.h"
#include "LiveVesselDlg.h"
#include "afxdialogex.h"

#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#endif

#define SP_CLICK_DOWN 1
#define MOUSE_MOVE 2
#define EP_CLICK_DOWN 3
#define ZOOM_MODE 4
#define ZOOMWND_CLICK_DOWN 5

#define SELECT_LINE 1
#define CLEAR_LINE 2

// coded by kjNoh 160922
enum{
	SELECT_FEATURE = 2,
	CLEAR_FEATURE = 1
};


std::string ChangeFilenameExtension(std::string frm_path, std::string ext)
{
	frm_path.erase(frm_path.length() - 3, 3);
	return frm_path + ext;
}

std::string GetFileExtension(std::string path)
{
	return path.substr(path.find_last_of(".") + 1);
}

std::string RemoveAndAppendFilepath(std::string path, std::string app)
{
	size_t lastindex = path.find_last_of(".");
	return path.substr(0, lastindex) + app;
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLiveVesselDlg 대화 상자



CLiveVesselDlg::CLiveVesselDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CLiveVesselDlg::IDD, pParent)
, m_bPostProc(false)
, m_bOverlay(false)
, m_bManualEdit(false)
, m_bDrawManualSegment(false)
, m_ctrlRadioNavi(0)
, m_bCancel(false)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLiveVesselDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Control(pDX, IDC_SLIDER_PICTURE_CONTROL, m_ctrlSlider);
	DDX_Control(pDX, IDC_PICTURE_ZOOM, m_picZoom);
	DDX_Control(pDX, IDC_EDIT_ZOOM_RATIO, m_editZoomRatio);
	DDX_Control(pDX, IDC_SLIDER_ZOOM_RATIO, m_ctrlSliderZoomRatio);
	DDX_Control(pDX, IDC_RADIO_NORMAL, m_ctrlRadioNormal);
	DDX_Control(pDX, IDC_RADIO_DRAW, m_ctrlRadioDraw);
	DDX_Control(pDX, IDC_RADIO_MOVE, m_ctrlRadioMove);
	//DDX_Control(pDX, IDC_CHECK_POST_PROCESSING, m_bPostProc);
	//DDX_Control(pDX, IDC_CHECK_OVERLAY, m_bOverlay);
	DDX_Control(pDX, IDC_CHECK_MANUAL_EDIT, m_ButtManual);
	DDX_Control(pDX, IDC_STATIC_FILE_NAME, m_ctrl_FileName);
	DDX_Radio(pDX, IDC_RADIO_MASK, m_ctrlRadioNavi);
	DDX_Control(pDX, ID_PROGRESS, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CLiveVesselDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_LOAD, &CLiveVesselDlg::OnBnClickedButtonImageLoad)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PICTURE_CONTROL, &CLiveVesselDlg::OnNMCustomdrawSlider1)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_LEFT_SLIDE, &CLiveVesselDlg::OnBnClickedButtonLeftSlide)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_SLIDE, &CLiveVesselDlg::OnBnClickedButtonRightSlide)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_ZOOM, &CLiveVesselDlg::OnBnClickedButtonZoom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ZOOM_RATIO, &CLiveVesselDlg::OnNMCustomdrawSliderZoomRatio)
	ON_BN_CLICKED(IDC_RADIO_NORMAL, &CLiveVesselDlg::OnBnClickedRadioNormal)
	ON_BN_CLICKED(IDC_RADIO_DRAW, &CLiveVesselDlg::OnBnClickedRadioDraw)
	ON_BN_CLICKED(IDC_RADIO_MOVE, &CLiveVesselDlg::OnBnClickedRadioMove)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_BUTTON_VCO_FRAME, &CLiveVesselDlg::OnBnClickedButtonVcoFrame)
	ON_BN_CLICKED(IDC_BUTTON_VCO_SEQUENCE, &CLiveVesselDlg::OnBnClickedButtonVcoSequence)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_THREAD_PAUSE, &CLiveVesselDlg::OnBnClickedButtonThreadPause)
	ON_BN_CLICKED(IDC_CHECK_POST_PROCESSING, &CLiveVesselDlg::OnBnClickedCheckPostProcessing)
	ON_BN_CLICKED(IDC_CHECK_OVERLAY, &CLiveVesselDlg::OnBnClickedCheckOverlay)
	ON_BN_CLICKED(IDC_CHECK_MANUAL_EDIT, &CLiveVesselDlg::OnBnClickedCheckManualEdit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_DATA, &CLiveVesselDlg::OnBnClickedButtonConvertData)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CLiveVesselDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()

int CALLBACK BrowseCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);


// CLiveVesselDlg 메시지 처리기

BOOL CLiveVesselDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	
	// to write log file
	char log_file_path[200];
	sprintf(log_file_path,"RotoVessel.log");
	InitLogStream(log_file_path);

	vesselImg = NULL;
	m_ctrlSlider.SetPos(0);
	m_curVesSegmStartPt = cv::Point(-1, -1);
	m_curVesSegmEndPt = cv::Point(-1, -1);
	m_bLoad = false;
	m_iMouseState = 0;
	m_iszDrawCircle = 1;
	m_bZoom = false;
	m_zoomImg = NULL;
	m_editZoomRatio.SetWindowTextW(L"100");
	strEdit = "100";
	m_ptZoom = cv::Point(-1, -1);
	m_ctrlSliderZoomRatio.SetRange(0, 1000);
	m_ctrlSliderZoomRatio.SetPos(100);
	m_ptCenterZoomWnd = cv::Point(-1, -1);
	m_fNull_width = 0;
	m_fNull_height = 0;

	m_picZoom.GetWindowRect(&m_rcNavi);
	ScreenToClient(&m_rcNavi);
	m_Picture.GetWindowRect(&m_rcPic);
	ScreenToClient(&m_rcPic);
	m_iMode_zoomWnd = 0;
	m_ctrlRadioNormal.SetCheck(BST_CHECKED);
	m_bMove = false;
	iLineIndex = -1;
	iLineSelected_state = CLEAR_LINE;
	//m_nLine = 0;

	///////////////////////
	// coded by kjNoh 160922
	selectedFeat = cv::Point(-1, -1);
	featSearchRange = 5;
	lineSearchRange = 2;
	mouseR_state = 0;
	dragIdx = -1;
	dragSpEp = -1;

	///////////////////////

	m_nFrame = 0;
	iFeatSelected_state = CLEAR_FEATURE;

	//161010_daseul
	m_bPostProc = false;
	m_bOverlay = true;
	CheckDlgButton(IDC_CHECK_POST_PROCESSING, FALSE);
	CheckDlgButton(IDC_CHECK_OVERLAY, TRUE);
	m_ButtManual.SetCheck(false);

	//2017.01.05_daseul
	bCtrl = false;
	bWheel = false;
	m_fZoomRatio = 1;

	// 2017. 02. 07 - SJKOH PROGRESS
	m_ctrlProgress.SetRange(0, 100);
	m_ctrlProgress.SetPos(0);
	return TRUE;
}

void CLiveVesselDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CLiveVesselDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_bLoad)
		{	
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			cv::Mat disp;
			vesselImg.copyTo(disp);

			//if (SegmTree.size() != 0)
			if (m_bOverlay)
			{
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
				for (int y = 0; y < m_mask.rows; y++)
				for (int x = 0; x < m_mask.cols; x++)
				{
					disp.at<cv::Vec3b>(y, x) = vesselImg.at<cv::Vec3b>(y, x);
					if (m_mask.at<uchar>(y, x))
					{
						disp.at<uchar>(y, x * 3 + 2) = 150;
					}
				}
				
				for (int i = 0; i < SegmTree.nSegm; i++)
				{
					if (SegmTree.vecSegmTree[i].size() != 0)
					{
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						//std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
						CVesSegm tmp_Segm = SegmTree.get(i);
						for (int j = 0; j < tmp_Segm.size(); j++)
						{
							WriteLog(__FILE__, __LINE__, __FUNCTION__);

							int pt_x = tmp_Segm[j].pt.x;
							int pt_y = tmp_Segm[j].pt.y;

							//// added by kjNoh 161006
							//int R = 150;
							//int G = vesselImg.at<uchar>(pt_y, pt_x * 3 + 1);
							//int B = vesselImg.at<uchar>(pt_y, pt_x * 3 + 0);
							//cv::circle(disp, cv::Point(pt_x, pt_y), FrangiScale.at<double>(pt_y, pt_x), CV_RGB(R, G, B), -1);

							//2016.12.27_daseul
							if ((pt_x < disp.cols) && (pt_y < disp.rows))
							{
								disp.at<uchar>(pt_y, pt_x * 3 + 0) = 255;
								disp.at<uchar>(pt_y, pt_x * 3 + 1) = 0;
								disp.at<uchar>(pt_y, pt_x * 3 + 2) = 0;
							}
						}
					}

					if (featureTree.getSize())
					{
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						featureInfo cur_feat = featureTree.get(i);
						if (cur_feat.spType)
							cv::circle(disp, cur_feat.sp, 3, CV_RGB(0, 255, 0)); 
						else
							cv::circle(disp, cur_feat.sp, 3, CV_RGB(255, 0, 0));

						if (cur_feat.epType)
							cv::circle(disp, cur_feat.ep, 3, CV_RGB(0, 255, 0));
						else
							cv::circle(disp, cur_feat.ep, 3, CV_RGB(255, 0, 0));
					}

				}
				//}
				//else
				{
					for (int i = 0; i < SegmTree.nSegm; i++)
					{
						if (SegmTree.vecSegmTree[i].size() != 0)
						{
							WriteLog(__FILE__, __LINE__, __FUNCTION__);
							//std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
							CVesSegm tmp_Segm = SegmTree.get(i);
							for (int j = 0; j < tmp_Segm.size(); j++)
							{
								WriteLog(__FILE__, __LINE__, __FUNCTION__);

								int pt_x = tmp_Segm[j].pt.x;
								int pt_y = tmp_Segm[j].pt.y;

								//// added by kjNoh 161006
								//int R = 150;
								//int G = vesselImg.at<uchar>(pt_y, pt_x * 3 + 1);
								//int B = vesselImg.at<uchar>(pt_y, pt_x * 3 + 0);
								//cv::circle(disp, cv::Point(pt_x, pt_y), FrangiScale.at<double>(pt_y, pt_x), CV_RGB(R, G, B), -1);

								//2016.12.27_dasuel
								if ((pt_x < disp.cols) && (pt_y < disp.rows))
								{
									disp.at<uchar>(pt_y, pt_x * 3 + 0) = 255;
									disp.at<uchar>(pt_y, pt_x * 3 + 1) = 0;
									disp.at<uchar>(pt_y, pt_x * 3 + 2) = 0;
								}
							}
						}

						if (featureTree.getSize())
						{
							WriteLog(__FILE__, __LINE__, __FUNCTION__);
							featureInfo cur_feat = featureTree.get(i);
							if (cur_feat.spType)
								cv::circle(disp, cur_feat.sp, 3, CV_RGB(0, 255, 0));
							else
								cv::circle(disp, cur_feat.sp, 3, CV_RGB(255, 0, 0));

							if (cur_feat.epType)
								cv::circle(disp, cur_feat.ep, 3, CV_RGB(0, 255, 0));
							else
								cv::circle(disp, cur_feat.ep, 3, CV_RGB(255, 0, 0));
						}

					}
				}
				//DrawPicture(disp);

				// if currently the mouse hovering above predetermined line, 
				// i.e., the user is selecting a specific vessel segment line, highlight that line
				if (iLineSelected_state == SELECT_LINE)
				{
					assert(0 <= iLineIndex && iLineIndex < SegmTree.nSegm);
					WriteLog(__FILE__, __LINE__, __FUNCTION__);
					CVesSegm curSegm = SegmTree.get(iLineIndex);
					for (int i = 0; i < curSegm.size(); i++)
						cv::circle(disp, curSegm[i].pt, 1, cv::Scalar(255, 0, 255), -1);
				}
				else if (iFeatSelected_state == CLEAR_LINE)
				{
					if (iLineIndex != -1)
					{
						assert(SegmTree.nSegm > iLineIndex);
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						CVesSegm curSegm = SegmTree.get(iLineIndex);
						for (int i = 0; i < curSegm.size(); i++)
						{
							disp.at<uchar>(curSegm[i].pt.y, curSegm[i].pt.x * 3 + 0) = 255;
							disp.at<uchar>(curSegm[i].pt.y, curSegm[i].pt.x * 3 + 1) = 0;
							disp.at<uchar>(curSegm[i].pt.y, curSegm[i].pt.x * 3 + 2) = 0;
						}
					}
				}
				/////////////////////////////////////
				// codeby kjNoh 160922
				if (iFeatSelected_state == SELECT_FEATURE)
				{
					WriteLog(__FILE__, __LINE__, __FUNCTION__);;
					int label = FeatrueImg.at<int>(selectedFeat)-1;
					assert(featureTree.nFeature > label);
					if (featureTree.getSize() && label >= 0)
					{
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						featureInfo tmp_feat = featureTree.get(label);
						if (tmp_feat.sp == selectedFeat)
						{
							WriteLog(__FILE__, __LINE__, __FUNCTION__);

							if (tmp_feat.spType)
								cv::circle(disp, tmp_feat.sp, 3, CV_RGB(0, 255, 0), -1);
							else
								cv::circle(disp, tmp_feat.sp, 3, CV_RGB(255, 0, 0), -1);
						}
						else
						{
							WriteLog(__FILE__, __LINE__, __FUNCTION__);
							if (tmp_feat.epType)
								cv::circle(disp, tmp_feat.ep, 3, CV_RGB(0, 255, 0), -1);
							else
								cv::circle(disp, tmp_feat.ep, 3, CV_RGB(255, 0, 0), -1);
						}
					}
				}
				////////////////////////////////////

				if (m_ptCur.x >= 0 && m_ptCur.y >= 0 && m_ptCur.x < vesselImg.cols && m_ptCur.y < vesselImg.rows)
				{
					WriteLog(__FILE__, __LINE__, __FUNCTION__);
					if (m_curVesSegmStartPt != cv::Point(-1, -1)) {
						//cv::line(disp, m_curVesSegmStartPt, m_curVesSegmEndPt, CV_RGB(255, 0, 0));
						// *** LiveVessel method (modified) - find gradient-based geodesic between two points
						for (int i = 0; i < m_curVesSegmPath.size(); i++)
						{
							WriteLog(__FILE__, __LINE__, __FUNCTION__);
							disp.at<uchar>(m_curVesSegmPath[i].y, m_curVesSegmPath[i].x * 3 + 0) = 0;
							disp.at<uchar>(m_curVesSegmPath[i].y, m_curVesSegmPath[i].x * 3 + 1) = 0;
							disp.at<uchar>(m_curVesSegmPath[i].y, m_curVesSegmPath[i].x * 3 + 2) = 255;
						}
						cv::circle(disp, m_ptCur, m_iszDrawCircle, CV_RGB(0, 255, 255), -1);
					}

					else
					{
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						cv::circle(disp, m_ptCur, m_iszDrawCircle, CV_RGB(0, 255, 255), -1);
					}
					disp.copyTo(m_disp_tmp);
				}
			}

			//2017.02.07_daseul _annotate
			//if (m_iMouseState == ZOOM_MODE || m_iMouseState == ZOOMWND_CLICK_DOWN) {
			//	WriteLog(__FILE__, __LINE__, __FUNCTION__);
			//	printf("onpaint_zoom\n");
			//	//cv::circle(m_zoomImg, cv::Point((m_ptCur_Zoom.x / m_fZoomRatio), (m_ptCur_Zoom.y / m_fZoomRatio)), 1, cv::Scalar(0, 0, 255), 1);

			//	cv::Mat dispZoom;
			//	m_zoomImg.copyTo(dispZoom);
			//	DrawNavi2window(dispZoom);
			//}

			//2017.01.05_daseul _zoom in&out
			if (bCtrl & bWheel)
			{
				//image resize to zoom ratio
				cv::resize(m_disp_tmp, m_zoomImg, cv::Size(m_disp_tmp.rows * m_fZoomRatio, m_disp_tmp.cols * m_fZoomRatio));

				cv::Point zoomPt = m_ptCur * m_fZoomRatio;
				cv::Point letfTop, rigthBottom;

				//compute the view range from resized image
				letfTop.x = (zoomPt.x - m_ptCur.x) * m_fRatio;
				letfTop.y = (zoomPt.y - m_ptCur.y) * m_fRatio;
				rigthBottom.x = (zoomPt.x + (m_rcPic.Width() - m_ptCur.x)) * m_fRatio;
				rigthBottom.y = (zoomPt.y + (m_rcPic.Height() - m_ptCur.y)) * m_fRatio;

				m_zoom_tmp = m_zoomImg(cv::Rect(letfTop.x, letfTop.y, (rigthBottom.x - letfTop.x), (rigthBottom.y - letfTop.y)));
				m_zoom_tmp.copyTo(disp);

				m_iMouseState = ZOOM_MODE;
				//bWheel = false;
			}
			DrawPicture(disp);

			cv::resize(m_disp_tmp, disp, cv::Size(disp.rows, disp.cols));
		}
		CDialogEx::OnPaint();
	}
}


HCURSOR CLiveVesselDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLiveVesselDlg::FindFilesInDir(CString dir, CString ext, 
	std::vector<std::string> &path_list)
{
	CFileFind finder;
	CString str = L"\\*." + ext;
	CString  strFolderPath = dir + str;
	BOOL bWorking = finder.FindFile(strFolderPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strA = finder.GetFileName();
		if ((strA == L".") || (strA == L".."));
		else
		{
			strFilePath = finder.GetFilePath();
			std::string filePath = CT2CA(strFilePath.operator LPCWSTR());
			path_list.push_back(filePath);
		}
	}
}


void CLiveVesselDlg::OnBnClickedButtonImageLoad()
{
	// *** launch UI to get image sequence directory *** //
	ITEMIDLIST *pidlBrowse=NULL;
	BROWSEINFO brInfo;
	brInfo.hwndOwner = GetSafeHwnd();
	brInfo.pidlRoot = NULL;
	memset(&brInfo, 0, sizeof(brInfo));
	brInfo.pszDisplayName = m_pszPathName;
	brInfo.lpszTitle = _T("Select Directory");
	brInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&brInfo);
	if (pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, m_pszPathName);
		vecFname.clear();
		SegmTree.rmAll();
		featureTree.rmAll();
		vecPoints.clear();
		m_vecPtZoom.clear();
		m_curVesSegmPath.clear();
		//2016.12.22_daseul
		m_tmpLine.clear();
		tStart = clock();
	}
	else
		return;
	// *** end: launch UI to get image sequence directory *** //
	m_ctrlProgress.SetPos(40); //2017.02.07 - SJKOH - PROGRESS
	
	// *** generate list of files of image sequence directory *** //
	FindFilesInDir(CString(m_pszPathName), L"bmp", vecFname);
	// *** end: generate list of files of image sequence directory *** //

	// minor initializations
	m_curVesSegmStartPt = m_curVesSegmEndPt = cv::Point(-1, -1);
	iSliderPos = 0;

	// *** for all vessel sequence frames: (pre-process & save) or (load pre-processed data) *** //
	for (int i = 0; i < vecFname.size(); i++)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		vesselImg = cv::imread(vecFname[i]); // load original frame
		cv::cvtColor(vesselImg, vesselImgG, CV_BGR2GRAY);
		std::string pre_fn = ChangeFilenameExtension(vecFname[i], "pre");
		// if frangi filtering results are not stored, perform frangi filtering and save
		if (!LoadFrangi(pre_fn))
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			SaveFrangi(pre_fn);
		}
	}
	// *** end: (pre-process & save) or (load pre-processed data) *** //
	m_ctrlProgress.SetPos(60); //2017.02.07 - SJKOH - PROGRESS

	// *** for initial frame, load frame and preprocessed info *** //
	// load frame
	vesselImg = cv::imread(vecFname[0]);
	cv::cvtColor(vesselImg, vesselImgG, CV_BGR2GRAY);
	// load frangi filter results
	std::string pre_fn = ChangeFilenameExtension(vecFname[0], "pre");
	LoadFrangi(pre_fn);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// load vessel centerline information
#ifdef NEW_VSC_TYPE
	std::string vsc_fn = ChangeFilenameExtension(vecFname[0], "vs2");
	LoadVesselCenterlineInfo(vsc_fn);
#else
	std::string vsc_fn = ChangeFilenameExtension(vecFname[0], "vsc");
	LoadVesselCenterlinePoints(vsc_fn);
	SegmTree.SetVesselRadiiFromFraigiScale(FrangiScale);
#endif
	// load region mask
	std::string maskPath = vecFname[0];
	maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back();
	maskPath += "_mask.png";
	m_mask = cv::imread(maskPath, CV_LOAD_IMAGE_GRAYSCALE);
	if (m_mask.empty())
		m_mask = cv::Mat::zeros(vesselImg.size(), CV_8UC1);
	// *** END loading frame and preprocessed info for initial frame *** //
	m_ctrlProgress.SetPos(80); //2017.02.07 - SJKOH - PROGRESS

	m_fRatio = vesselImg.cols / (float)m_rcPic.Width();
	m_width = vesselImg.cols;
	m_height = vesselImg.rows;

	m_bLoad = true;
	m_ctrlSlider.SetRange(0, vecFname.size() - 1);
	m_ctrlSlider.SetPos(0);

	updateSegm();
	OnPaint();
	m_ctrlProgress.SetPos(100); //2017.02.07 - SJKOH - PROGRESS
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// show sequence directory name above viewing window
	m_ctrl_FileName.SetWindowText(m_pszPathName);
	m_ctrlProgress.SetPos(0); //2017.02.07 - SJKOH - PROGRESS
}


void CLiveVesselDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	if (vecFname.size() != 0) {
		//iSliderPos = m_ctrlSlider.GetPos();
		////vesselImg[iSliderPos].copyTo(vesselImg);
		//vesselImg = vesselImg[iSliderPos];
		//frangiImg = frangiImg[iSliderPos];
		//
		//OnPaint();

		dispUpdate();
		updateSegm();
	}
	//*pResult = 0;
}


void CLiveVesselDlg::DrawPicture(cv::Mat disp)
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	CRect rc;
	m_Picture.GetClientRect(rc);

	
	cv::Mat view_vesselImg;
	disp.copyTo(view_vesselImg);
	cv::resize(view_vesselImg, view_vesselImg, cv::Size(rc.Width(), rc.Height()));
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	//cv::flip(view_vesselImg, view_vesselImg, 0);
	int nX = view_vesselImg.cols;
	int nY = view_vesselImg.rows;

	BITMAPINFOHEADER bitmaphaeader;
	bitmaphaeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmaphaeader.biWidth = nX;
	bitmaphaeader.biHeight = -nY;
	bitmaphaeader.biPlanes = 1;
	bitmaphaeader.biBitCount = 24;
	bitmaphaeader.biCompression = BI_RGB;
	bitmaphaeader.biSizeImage = nX*nY * 3;
	bitmaphaeader.biXPelsPerMeter = 0;
	bitmaphaeader.biYPelsPerMeter = 0;
	bitmaphaeader.biClrUsed = 0;
	bitmaphaeader.biClrImportant = 0;

	CDC *pdc = m_Picture.GetDC();
	HDC hdc = pdc->m_hDC;

	char log[260];
	sprintf_s(log, "nX: %d, nY: %d", nX, nY);
	WriteLog(__FILE__, __LINE__, __FUNCTION__, log);
	::StretchDIBits(hdc, 0, 0, nX, nY, 0, 0, nX, nY, view_vesselImg.data, (BITMAPINFO*)&bitmaphaeader, DIB_RGB_COLORS, SRCCOPY);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	m_Picture.ReleaseDC(pdc);
}




void CLiveVesselDlg::OnBnClickedButtonLeftSlide()
{
	iSliderPos -= 1;

	if (iSliderPos < 0)
		iSliderPos = 0;

	//2016.12.22_daseul
	m_tmpLine.clear();

	dispUpdate();
	updateSegm();
	UpdateData();
}

void CLiveVesselDlg::OpenEndedFMM()
{
	double pfm_start_points[] = { m_curVesSegmStartPt.x, m_curVesSegmStartPt.y };
	double nb_iter_max = std::min(pram.pfm_nb_iter_max,
		(1.2*std::max(frangiImg.rows, frangiImg.cols)*
		std::max(frangiImg.rows, frangiImg.cols)));
	cv::Mat copy_frangiImg;
	copy_frangiImg = frangiImg.clone();;
	copy_frangiImg.setTo(1e-10, copy_frangiImg < 1e-10);
	double *S;
	fmm.fast_marching(copy_frangiImg, frangiImg.cols, frangiImg.rows,
		pfm_start_points, 1, 0, 0, nb_iter_max,
		&frangiDist, &S);
}

void CLiveVesselDlg::OpenEndedFMM(cv::Point end_pt)
{
	double pfm_start_points[] = { m_curVesSegmStartPt.x, m_curVesSegmStartPt.y };
	double pfm_end_points[] = { end_pt.x, end_pt.y };
	double nb_iter_max = std::min(pram.pfm_nb_iter_max,
		(1.2*std::max(frangiImg.rows, frangiImg.cols)*
		std::max(frangiImg.rows, frangiImg.cols)));
	cv::Mat copy_frangiImg;
	copy_frangiImg = frangiImg.clone();
	copy_frangiImg.setTo(1e-10, copy_frangiImg < 1e-10);
	double *S;
	fmm.fast_marching(copy_frangiImg, frangiImg.cols, frangiImg.rows,
		pfm_start_points, 1, pfm_end_points, 1, nb_iter_max,
		&frangiDist, &S);
}

void CLiveVesselDlg::OpenEndedFMMwithUserPath(cv::Point end_pt)
{
	double pfm_start_points[] = { m_curVesSegmStartPt.x, m_curVesSegmStartPt.y };
	double pfm_end_points[] = { end_pt.x, end_pt.y };
	double nb_iter_max = std::min(pram.pfm_nb_iter_max,
		(1.2*std::max(frangiImg.rows, frangiImg.cols)*
		std::max(frangiImg.rows, frangiImg.cols)));
	cv::Mat copy_frangiImg;
	copy_frangiImg = frangiImg.clone();
	copy_frangiImg.setTo(1e-10, copy_frangiImg < 1e-10);

	// incorporating user path proximity
	if (m_userPath.size() > 1) {
		// draw user path
		cv::Mat user_path = cv::Mat::ones(frangiImg.size(), CV_8UC1) * 255;
		for (int i = 0; i < m_userPath.size() - 1; i++) {
			cv::line(user_path, m_userPath[i], m_userPath[i + 1], cv::Scalar(0));
		}
		// compute distance transform to user path
		cv::Mat user_path_dist, user_path_dist_64F;
		cv::distanceTransform(user_path, user_path_dist, CV_DIST_L2, cv::DIST_MASK_PRECISE);
		// combine with frangi based speed 
		user_path_dist.convertTo(user_path_dist_64F, CV_64FC1);
		user_path_dist_64F = 0.1 / (1 + user_path_dist_64F);
		copy_frangiImg = copy_frangiImg + user_path_dist_64F;
	}
	double *S;
	fmm.fast_marching(copy_frangiImg, frangiImg.cols, frangiImg.rows,
		pfm_start_points, 1, pfm_end_points, 1, nb_iter_max,
		&frangiDist, &S);
}

void CLiveVesselDlg::OnBnClickedButtonRightSlide()
{
	iSliderPos += 1;

	if (iSliderPos > vecFname.size() - 1)
		iSliderPos = vecFname.size() - 1;

	//2016.12.22_daseul
	m_tmpLine.clear();
	dispUpdate();
	updateSegm();
	
	UpdateData();
}


BOOL CLiveVesselDlg::PreTranslateMessage(MSG* pMsg)
{
	if (!m_bLoad)
		return 0;

	bool bWorking = false;
	if (pMsg->message == WM_KEYDOWN)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		if (!bWorking)
		{
			//2016.12.21_daseul _UNDO
			if ((GetKeyState(VK_CONTROL) & 0x8000) /*&& pMsg->wParam == 'Z'*/)
			{
				if (pMsg->wParam == 'Z')
				{
					//Store the last path of SegmTree to m_tmpLine and Remove the last path of SegmTree
					if (SegmTree.nSegm != 0)
					{
						m_tmpLine.push_back(SegmTree.get(SegmTree.nSegm - 1));
						SegmTree.rmSegm(SegmTree.nSegm - 1);
					}
					//initialize selected line
					iLineSelected_state = CLEAR_LINE;
					//iLineIndex = -1;

					//re-draw vessel mask
					ReDrawMask();
					iLineIndex = -1;
				}

				//2017.02.07_daseul _zoom in&out using +, - key
				else if (GetKeyState(VK_OEM_PLUS) & 0x8000)
				{
					bWheel = true;
					m_fZoomRatio += 0.3;
					OnPaint();
				}

				else if (GetKeyState(VK_OEM_MINUS) & 0x8000)
				{
					bWheel = true;
					m_fZoomRatio -= 0.3;

					if (m_fZoomRatio < 1)
						m_fZoomRatio = 1;

					OnPaint();
				}

				//2017.01.05_daseul _zoom in&out function
				else
					bCtrl = true;
			}

			//2016.12.21_daseul _REDO
			else if ((GetKeyState(VK_SHIFT) & 0x8000) && pMsg->wParam == 'Z')
			{
				if (!m_tmpLine.empty())
				{
					//Add the last line of m_tmpLine to SegmTree and Remove the last line of m_tmpLine
					SegmTree.addSegm(m_tmpLine.back());
					//m_tmpLine.pop_back();

					//2016.12.27_daseul _add feature point
					featureInfo cur_feat;
					cur_feat.sp = m_tmpLine.back().front().pt;
					cur_feat.spType = 0;
					cur_feat.ep = m_tmpLine.back().back().pt;
					cur_feat.epType = 0;

					featureTree.addFeat(cur_feat);

					m_tmpLine.pop_back();
				}

				//initialize selected line
				iLineSelected_state = CLEAR_LINE;
				iLineIndex = -1;

				//re-draw vessel mask
				ReDrawMask();
			}

			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			bWorking = true;
			switch (pMsg->wParam)
			{
			case VK_LEFT:
				iSliderPos -= 1;

				if (iSliderPos < 0)
					iSliderPos = 0;

				//2016.12.22_daseul
				m_tmpLine.clear();

				////vesselImg[iSliderPos].copyTo(vesselImg);
				//vesselImg = vesselImg[iSliderPos];
				//frangiImg = frangiImg[iSliderPos];
				//segLabelImg = vec_segLabelImg[iSliderPos];
				//featrueImage = vec_featrueImage[iSliderPos];

				//

				//m_ctrlSlider.SetPos(iSliderPos);
				//m_picZoom.SetFocus();

				//iLineSelected_state = 0;

				//OnPaint();
				////m_ctrlSlider.SetFocus();

				dispUpdate();
				updateSegm();
				break;

			case VK_RIGHT:
				iSliderPos += 1;
				if (iSliderPos > vecFname.size() - 1)
					iSliderPos = vecFname.size() - 1;

				//2016.12.22_daseul
				m_tmpLine.clear();

				////vesselImg[iSliderPos].copyTo(vesselImg);
				//vesselImg = vesselImg[iSliderPos];
				//frangiImg = frangiImg[iSliderPos];
				//segLabelImg = vec_segLabelImg[iSliderPos];
				//featrueImage = vec_featrueImage[iSliderPos];

				//

				//m_ctrlSlider.SetPos(iSliderPos);
				//m_picZoom.SetFocus();

				//iLineSelected_state = 0;

				//OnPaint();
				////m_ctrlSlider.SetFocus();

				dispUpdate();
				updateSegm();
				break;

			case VK_SPACE:	
				m_curVesSegmStartPt = m_curVesSegmEndPt = cv::Point(-1, -1);
				// end drawing mode
				m_bDrawFMMSegment = false;
				m_bDrawManualSegment = false;
				if (m_iMouseState == ZOOMWND_CLICK_DOWN || m_iMouseState == ZOOM_MODE) {
					printf("space");
				}
				OnPaint();
				break;

				/*case 'A':
					printf("a down");
					m_iszDrawCircle -= 1;

					if (m_iszDrawCircle < 5)
					m_iszDrawCircle = 5;
					OnPaint();
					break;

					case 'D':
					printf("d down");
					m_iszDrawCircle += 1;

					if (m_iszDrawCircle > 15)
					m_iszDrawCircle = 15;
					OnPaint();
					break;*/

			case VK_RETURN:
				m_editZoomRatio.GetWindowTextW(strEdit);
				ZoomProcessing();
				OnPaint();
				break;

			case 'Q':
				m_bMove = false;
				m_iMode_zoomWnd = 0;
				m_ctrlRadioNormal.SetCheck(BST_CHECKED);
				m_ctrlRadioDraw.SetCheck(BST_UNCHECKED);
				m_ctrlRadioMove.SetCheck(BST_UNCHECKED);
				break;

			case 'W':
				m_bMove = false;
				m_iMode_zoomWnd = 1;
				m_ctrlRadioDraw.SetCheck(BST_CHECKED);
				m_ctrlRadioNormal.SetCheck(BST_UNCHECKED);
				m_ctrlRadioMove.SetCheck(BST_UNCHECKED);
				break;

			case 'E':
				m_bMove = true;
				m_iMode_zoomWnd = 2;
				m_ctrlRadioMove.SetCheck(BST_CHECKED);
				m_ctrlRadioNormal.SetCheck(BST_UNCHECKED);
				m_ctrlRadioDraw.SetCheck(BST_UNCHECKED);
				break;

			case 'Z':
				if (!(GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_CONTROL) & 0x8000))
				{
					if (m_bLoad != 0 && iLineIndex != -1)
					{
						//CSegmTree &curSegTree = SegmTree;
						//2016.12.22_daseul
						if (SegmTree.nSegm != 0)
						{
							m_tmpLine.push_back(SegmTree.get(iLineIndex));
							SegmTree.rmSegm(iLineIndex);
						}
						//SegmTree.rmSegm(iLineIndex);
						//SegmTree.rmSegm(iLineIndex);
						//(*SegmTree).erase((*SegmTree).begin() + iLineIndex);

						iLineSelected_state = CLEAR_LINE;

						//////////////////////////////////////////161007 daseul

						//////////////////////////////////
						// coded by kjNoh 160922
						//2016.12.22_daseul _modified code : code->function(ReDrawMask())
						//updateSegm();
						//iLineIndex = -1;
						//dispUpdate();
						////////////////////////////////////

						//OnPaint();

						//2016.12.22_daseul
						ReDrawMask();
						iLineIndex = -1;
						FinishAndSave2File();
					}
				}
				break;

			case 'P':
				if (m_bPostProc)
				{
					m_bPostProc = false;
					CheckDlgButton(IDC_CHECK_POST_PROCESSING, FALSE);
				}
				else
				{
					m_bPostProc = true;
					CheckDlgButton(IDC_CHECK_POST_PROCESSING, TRUE);
				}
				break;

			case 'O':
				if (m_bOverlay)
				{
					m_bOverlay = false;
					CheckDlgButton(IDC_CHECK_OVERLAY, FALSE);
				}
				else
				{
					m_bOverlay = true;
					CheckDlgButton(IDC_CHECK_OVERLAY, TRUE);
				}
				OnPaint();
				break;
			case '1':
				if (m_bManualEdit)
				{
					m_bManualEdit = false;
					CheckDlgButton(IDC_CHECK_MANUAL_EDIT, FALSE);
				}
				else
				{
					m_bManualEdit = true;
					CheckDlgButton(IDC_CHECK_MANUAL_EDIT, TRUE);
				}
				ClearCurPath();
				m_bDrawManualSegment = false;
				m_bDrawFMMSegment = false;
				OnPaint();
				break;
			}

		}
	}
	// 2017. 02. 07 SJKOH - NAVIGATOR
	UpdateData(TRUE);
	if (m_ctrlRadioNavi == 0)
	{
		std::string tmp_mask_path = vecFname[iSliderPos];
		tmp_mask_path.pop_back(); tmp_mask_path.pop_back(); tmp_mask_path.pop_back(); tmp_mask_path.pop_back();

		cv::Mat tmp_mask_1;
		cv::String cur_name = tmp_mask_path + "_vsc_mask" + ".png";
		tmp_mask_1 = cv::imread(cur_name, CV_LOAD_IMAGE_GRAYSCALE);

		// ADDTIONAL kjnoh 170208
		if (tmp_mask_1.empty())
			tmp_mask_1 = cv::Mat::zeros(m_height, m_width,CV_8UC1);
		
		DrawNavi2window(tmp_mask_1);
	}
	else if (m_ctrlRadioNavi == 1)
	{
		std::string vessel_path = vecFname[iSliderPos];
		vessel_path.pop_back(); vessel_path.pop_back(); vessel_path.pop_back(); vessel_path.pop_back();

		cv::Mat tmp_vessel;
		cv::String cur_name = vessel_path + ".bmp";
		tmp_vessel = cv::imread(cur_name, CV_LOAD_IMAGE_GRAYSCALE);

		DrawNavi2window(tmp_vessel);
	}
	UpdateData(FALSE);
	//return CDialogEx::PreTranslateMessage(pMsg);
	return 0;
}

bool CLiveVesselDlg::checkIfPointIsNearFeat(cv::Point pt)
{
	bool findNearFeat = false;
	for (int y = -featSearchRange; y <= featSearchRange; y++)
	{
		for (int x = -featSearchRange; x <= featSearchRange; x++)
		{
			if (pt.y + y >= 0 && pt.y + y < m_height &&
				pt.x + x >= 0 && pt.x + x < m_width &&
				FeatrueImg.at<int>(pt.y + y, pt.x + x))
			{
				iFeatSelected_state = SELECT_FEATURE;
				selectedFeat = cv::Point(pt.x + x, pt.y + y);
				findNearFeat = true;
				break;
			}
		}
		if (findNearFeat)
			break;
	}
	return findNearFeat;
}

bool CLiveVesselDlg::checkIfPointIsNearLine(cv::Point pt)
{
	bool findNearSeg = false;
	for (int y = -lineSearchRange; y <= lineSearchRange; y++)
	{
		for (int x = -lineSearchRange; x <= lineSearchRange; x++)
		{
			if (pt.y + y >= 0 && pt.y + y < m_height &&
				pt.x + x >= 0 && pt.x + x < m_width &&
				SegLabelImg.at<int>(pt.y + y, pt.x + x))
			{
				iLineSelected_state = SELECT_LINE;
				iLineIndex = SegLabelImg.at<int>(pt.y + y, pt.x + x) - 1;
				selectedLinePoint = cv::Point(pt.x + x, pt.y + y);
				findNearSeg = true;
				break;
			}
		}
		if (findNearSeg)
			break;
	}
	return findNearSeg;
}

void CLiveVesselDlg::FinalizeVesselSegment(std::vector<cv::Point> vpath)
{
	CVesSegm vesSegm(vpath);
	ComputeVesselRadii(vesSegm, FrangiScale, vesselImgG);
	MakeRegionMask_VesRadii(vesSegm, m_mask);
	//MakeRegionMask_NKJ(m_curVesSegmPath, FrangiScale, m_mask);
	SegmTree.addSegm(vesSegm);

	//////////////////////////
	//coded by kjNoh 160922 
	featureInfo cur_spepInfor;
	cur_spepInfor.sp = m_curVesSegmStartPt;
	cur_spepInfor.ep = m_curVesSegmEndPt;
	cur_spepInfor.spType = 0;
	cur_spepInfor.epType = 0;

	featureTree.addFeat(cur_spepInfor);

	for (int i = 0; i < m_curVesSegmPath.size(); i++)
		SegLabelImg.at<int>(m_curVesSegmPath[i]) = SegmTree.nSegm;
	FeatrueImg.at<int>(m_curVesSegmPath[0]) = SegmTree.nSegm;
	FeatrueImg.at<int>(m_curVesSegmPath.back()) = SegmTree.nSegm;
}

void CLiveVesselDlg::ClearCurPath()
{
	m_curVesSegmPath.clear();
	m_curVesSegmStartPt = m_curVesSegmEndPt = cv::Point(-1, -1);
	m_userPath.clear();
}

void CLiveVesselDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// if no image, do nothing
	if (vecFname.size() == NULL)
		return;

	// get image coordinate for mouse input coordinate
	cv::Point pt_img(std::round((point.x - m_rcPic.left) * m_fRatio),
					 std::round((point.y - m_rcPic.top) * m_fRatio));

	// if image coordinate is valid
	if (0 <= pt_img.x && pt_img.x < m_width && 0 <= pt_img.y && pt_img.y < m_height)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		m_Picture.SetFocus();

		// *** determine "CLICK POINT" - to snap clicked point to nearby feature or line point
		cv::Point click_pt;
		// if the mouse curser is (was) above a predetermined feature point, 
		// set click point to that feature point
		if (iFeatSelected_state == SELECT_FEATURE)
		{
			click_pt = selectedFeat;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		// if the mouse curser is (was) above a predetermined line point, 
		// set click point to that line point
		else if (iLineSelected_state == SELECT_LINE)
		{
			click_pt = selectedLinePoint;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		// if none of the above, set starting point to current image coordinate
		else
		{
			click_pt = pt_img;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		// *** end of determining "CLICK POINT" 

		// if manual edit, simple, just toggle drawing mode, do no shortest path computations
		if (m_bManualEdit)
		{
			m_bDrawManualSegment = true;
			m_curVesSegmPath.clear();
			m_curVesSegmStartPt = click_pt;
			m_prePt = m_curVesSegmStartPt;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		// if non-manual = LiveVessel mode
		else {
			// *** if currently not drawing (and thus this left click is to set start point),
			// begin draw segment mode *** //
			if (!m_bDrawFMMSegment)
			{
				// set to drawing mode
				m_bDrawFMMSegment = true;
				// initialize starting point to click point and end point as current point
				m_curVesSegmStartPt = click_pt;
				m_curVesSegmEndPt = pt_img;
				m_userPath.clear();
				m_userPath.push_back(m_curVesSegmStartPt);
				m_curVesSegmPath.clear();
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
			// if this left click is to set end point
			else
			{
				// set end point and compute path
				m_curVesSegmEndPt = click_pt;
				m_userPath.push_back(m_curVesSegmEndPt);
				fmm.compute_discrete_geodesic(frangiDist, m_curVesSegmEndPt, &m_curVesSegmPath);
				std::reverse(m_curVesSegmPath.begin(), m_curVesSegmPath.end()); // coeded by kjNoh 160922
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
				// compute region mask, refine path, and store
				FinalizeVesselSegment(m_curVesSegmPath);
				// reinitialize starting point for next vessel segment
				m_curVesSegmStartPt = m_curVesSegmEndPt;
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
		}

		//2016.12.22_daseul
		FinishAndSave2File();
	}

	// ZOOM MODE
	/*if (m_bZoom)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		if (0 <= pt_img.x && pt_img.x < m_rcPic.Width() && 0 <= pt_img.y && pt_img.y < m_rcPic.Height())
		if (m_iMode_zoomWnd != 1)
			m_ptZoom = pt_img;

		if (0 <= point.x - m_rcNavi.left && point.x - m_rcNavi.left < m_rcNavi.Width() && 0 <= point.y - m_rcNavi.top && point.y - m_rcNavi.top < m_rcNavi.Height())
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			if (!m_bMove)
			{
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
				m_iMouseState = ZOOMWND_CLICK_DOWN;
				m_picZoom.SetFocus();

				m_ptCur_Zoom.x = (point.x - m_rcNavi.left) * m_fZoomRatio;
				m_ptCur_Zoom.y = (point.y - m_rcNavi.top) * m_fZoomRatio;

				cv::circle(m_cropImg, m_ptCur_Zoom, 1, cv::Scalar(255, 255, 255), 2);
				//cv::imshow("circle_crop", m_cropImg);

				cv::Point pt;
				if (m_leftTop.x == 0) {
					m_fNull_width = m_zoomImg.cols - m_cropImg.cols;
					pt.x = m_ptCur_Zoom.x - m_fNull_width;
				}
				else
					pt.x = m_ptCur_Zoom.x;
				if (m_leftTop.y == 0) {
					m_fNull_height = m_zoomImg.rows - m_cropImg.rows;
					pt.y = m_ptCur_Zoom.y - m_fNull_height;
				}
				else
					pt.y = m_ptCur_Zoom.y;

				if (m_curVesSegmStartPt == cv::Point(-1, -1))
				{
					m_curVesSegmStartPt = pt + m_leftTop;
					m_curVesSegmEndPt = pt + m_leftTop;
				}
				else
				{
					m_curVesSegmEndPt = pt + m_leftTop;
					//PointsOfLine();

					CVesSegm vecPts;
					vecPts.push_back(CVesSegmPt(m_curVesSegmStartPt));
					vecPts.push_back(CVesSegmPt(m_curVesSegmEndPt));
					SegmTree.addSegm(vecPts);

					m_curVesSegmStartPt = m_curVesSegmEndPt;
					OnPaint();
				}
			}
			else
				m_vecPtZoom.push_back(m_curVesSegmEndPt);
		}
		ZoomProcessing();
	}*/

	OnPaint();
	//ZoomProcessing(); // annoated by noh 160928

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLiveVesselDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bLoad)
		return;

	// if no files have been loaded, do nothing
	if (vecFname.size() == NULL) {
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		return;
	}

	// set default line/feature selection mode as CLEAR
	iLineSelected_state = CLEAR_LINE;
	iFeatSelected_state = CLEAR_FEATURE;

	iLineIndex = -1; // coded by kjNoh 160922
	selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

	// compute image coordinate of current mouse point
	cv::Point pt_img(std::round((point.x - m_rcPic.left) * m_fRatio),
					 std::round((point.y - m_rcPic.top) * m_fRatio));

	bool bPaint = false;
	// if image coordinate of mouse point valid
	if (0 <= pt_img.x && pt_img.x < m_width && 0 <= pt_img.y && pt_img.y < m_height)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		// store image point as current point
		m_ptCur = pt_img;
		// check if current point is close to feature (end/branching point),
		// if so, set iFeatSelected_state as SELECT_FEATURE and selectedFeat
		checkIfPointIsNearFeat(m_ptCur);
		// check if current point is close to vessel segment 
		// if so, set iLineSelected_state as SELECT_LINE
		checkIfPointIsNearLine(m_ptCur);
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		bPaint = true;

		// if drawing mode has been set by left mouse click, 
		// compute and show real-time path from starting point
		if (m_bDrawFMMSegment && m_curVesSegmStartPt != m_ptCur)
		{
			m_userPath.push_back(m_ptCur);
			// update fast marching method distance if you move mouse pointer
			m_curVesSegmEndPt = m_ptCur;
			//OpenEndedFMM(m_curVesSegmEndPt);
			OpenEndedFMMwithUserPath(m_curVesSegmEndPt);
			fmm.compute_discrete_geodesic(frangiDist, m_ptCur, &m_curVesSegmPath);
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		else if (m_bManualEdit && m_bDrawManualSegment)
		{
			std::vector<cv::Point> pts;
			pts = getLine(m_prePt, m_ptCur);
			for (int i = 0; i < pts.size(); i++)
				m_curVesSegmPath.push_back(pts[i]);

			m_prePt = m_ptCur;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
	}
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	// FOR ZOOM MODE - PROBABLY OBSOLETE
	/*if (m_bZoom)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		if (!m_zoomImg.empty()) {
			m_ptCur_Zoom.x = (point.x - m_rcNavi.left) * m_fZoomRatio;
			m_ptCur_Zoom.y = (point.y - m_rcNavi.top) * m_fZoomRatio;

			if (0 <= point.x - m_rcNavi.left && point.x - m_rcNavi.left < m_rcNavi.Width() && 0 <= point.y - m_rcNavi.top && point.y - m_rcNavi.top < m_rcNavi.Height())
			{
				cv::Point pt;
				if (m_leftTop.x == 0) {
					m_fNull_width = m_zoomImg.cols - m_cropImg.cols;
					pt.x = m_ptCur_Zoom.x - m_fNull_width;
				}
				else
					pt.x = m_ptCur_Zoom.x;
				if (m_leftTop.y == 0) {
					m_fNull_height = m_zoomImg.rows - m_cropImg.rows;
					pt.y = m_ptCur_Zoom.y - m_fNull_height;
				}
				else
					pt.y = m_ptCur_Zoom.y;

				m_picZoom.SetFocus();

				m_ptCur = pt + m_leftTop;
				m_curVesSegmEndPt = pt + m_leftTop;

				if (m_bMove)
				{
					if (m_vecPtZoom.size() != 0)
					{
						cv::Point ptDiff = m_vecPtZoom[0] - m_ptCur;
						m_ptZoom += ptDiff;
						if (0 <= m_ptZoom.x && m_ptZoom.x <= vesselImg.cols && 0 <= m_ptZoom.y && m_ptZoom.y <= vesselImg.rows)
							ZoomProcessing();
					}
				}
				//OnPaint();
			}
		}
		bPaint = true;
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
	}*/
	if (bPaint)
		OnPaint(); 
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	CDialogEx::OnMouseMove(nFlags, point);
}

// OnLButtonUp() : function effective only for MANUAL DRAWING MODE
//	drawing is done while holding the Left Button in MANUAL MODE
//	when Left Button is released, must finalize manually drawn segment
void CLiveVesselDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_iMode_zoomWnd == 2)
		m_vecPtZoom.clear();

	else if (m_bManualEdit && m_bDrawManualSegment)
	{
		// compute image coordinate of current mouse point
		cv::Point pt_img(std::round((point.x - m_rcPic.left) * m_fRatio),
						 std::round((point.y - m_rcPic.top) * m_fRatio));

		if (pt_img.x < 0 || pt_img.x >= vesselImg.cols || 
			pt_img.y < 0 || pt_img.y >= vesselImg.rows)
			return;

		if (iFeatSelected_state == SELECT_FEATURE)
			m_curVesSegmEndPt = selectedFeat;
		else if (iLineSelected_state == SELECT_LINE)
			m_curVesSegmEndPt = selectedLinePoint;
		else
			m_curVesSegmEndPt = pt_img;

		if (m_curVesSegmEndPt == m_curVesSegmStartPt) {
			ClearCurPath();
			return;
		}

		std::vector<cv::Point> pts = getLine(m_prePt, m_curVesSegmEndPt);
		for (int i = 0; i < pts.size(); i++)
		{
			m_curVesSegmPath.push_back(pts[i]);
		}
		if (m_curVesSegmPath.size() < 4)
		{
			ClearCurPath();
			return;
		}

		FinalizeVesselSegment(m_curVesSegmPath);
		
		m_bDrawManualSegment = false;
		ClearCurPath();
	}
	//m_ptZoom = m_curVesSegmEndPt;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CLiveVesselDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CLiveVesselDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	int a = 5;
}


void CLiveVesselDlg::OnBnClickedButtonZoom()
{
	m_bZoom = true;
}


void CLiveVesselDlg::DrawNavi2window(cv::Mat disp)
{
	CRect rc;
	m_picZoom.GetClientRect(rc);

	cv::Mat view_zoomImg;
	disp.copyTo(view_zoomImg);
	cv::resize(view_zoomImg, view_zoomImg, cv::Size(rc.Width(), rc.Height()));

	cv::flip(view_zoomImg, view_zoomImg, 0);
	int nX = view_zoomImg.cols;
	int nY = view_zoomImg.rows;

	// 2017. 02. 07 - SJKOH - NAVIGATOR
	cv::cvtColor(view_zoomImg, view_zoomImg, CV_GRAY2RGB);

	BITMAPINFOHEADER bitmaphaeader;
	bitmaphaeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmaphaeader.biWidth = nX;
	bitmaphaeader.biHeight = nY;
	bitmaphaeader.biPlanes = 1;
	bitmaphaeader.biBitCount = 24;
	bitmaphaeader.biCompression = BI_RGB;
	bitmaphaeader.biSizeImage = nX*nY * 3;
	bitmaphaeader.biXPelsPerMeter = 0;
	bitmaphaeader.biYPelsPerMeter = 0;
	bitmaphaeader.biClrUsed = 0;
	bitmaphaeader.biClrImportant = 0;

	CDC *pdc = m_picZoom.GetDC();
	HDC hdc = pdc->m_hDC;

	::StretchDIBits(hdc, 0, 0, nX, nY, 0, 0, nX, nY, view_zoomImg.data, (BITMAPINFO*)&bitmaphaeader, DIB_RGB_COLORS, SRCCOPY);
	m_picZoom.ReleaseDC(pdc);
}


void CLiveVesselDlg::ZoomProcessing()
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	float iZoom_width = m_rcNavi.Width();
	float iZoom_height = m_rcNavi.Height();

	float iLeft_width, iLeft_height, iRight_width, iRight_height;
	iLeft_width = 0;
	iLeft_height = 0;
	iRight_width = iZoom_width;
	iRight_height = iZoom_height;

	float fZoomRatio = _ttof(strEdit) / 100;

	if (m_ptZoom.x - (iZoom_width / (fZoomRatio * 2)) < 0) {
		m_leftTop.x = 0;
		iLeft_width = (iZoom_width / (fZoomRatio * 2)) - m_ptZoom.x;
	}
	else
		m_leftTop.x = m_ptZoom.x - (iZoom_width / (fZoomRatio * 2));

	if (m_ptZoom.y - (iZoom_height / (fZoomRatio * 2)) < 0) {
		m_leftTop.y = 0;
		iLeft_height = (iZoom_height / (fZoomRatio * 2)) - m_ptZoom.y;
	}
	else
		m_leftTop.y = m_ptZoom.y - (iZoom_height / (fZoomRatio * 2));

	if (m_ptZoom.x + (iZoom_width / (fZoomRatio * 2)) > vesselImg.cols) {
		m_rightBottom.x = vesselImg.cols;
		iRight_width = (iZoom_width / (fZoomRatio * 2)) + (vesselImg.cols - m_ptZoom.x);
	}
	else
		m_rightBottom.x = m_ptZoom.x + (iZoom_width / (fZoomRatio * 2));

	if (m_ptZoom.y + (iZoom_height / (fZoomRatio * 2)) > vesselImg.rows) {
		m_rightBottom.y = vesselImg.rows;
		iRight_height = (iZoom_height / (fZoomRatio * 2)) + (vesselImg.rows - m_ptZoom.y);
	}
	else
		m_rightBottom.y = m_ptZoom.y + (iZoom_height / (fZoomRatio * 2));

	float iCrop_width = m_rightBottom.x - m_leftTop.x;
	float iCrop_height = m_rightBottom.y - m_leftTop.y;

	m_cropImg = m_disp_tmp(cv::Rect(m_leftTop.x, m_leftTop.y, iCrop_width, iCrop_height));
	float width = iCrop_width * fZoomRatio;
	float height = iCrop_height * fZoomRatio;

	if (width > iZoom_width)
		width = iZoom_width;
	if (height > iZoom_height)
		height = iZoom_height;

	if (width <= 20 || height <= 20)
		return;

	cv::resize(m_cropImg, m_cropTmp, cv::Size(width, height), 0, BPAS_LINEAR);

	cv::Mat tmp(iZoom_width, iZoom_height, CV_8UC3, cv::Scalar::all(0));
	cv::Mat	tmpROI;

	float fZoom_Left_width = iLeft_width * fZoomRatio;
	float fZoom_Left_height = iLeft_height *fZoomRatio;

	if (fZoom_Left_width < 0)
		fZoom_Left_width = 0;
	if (fZoom_Left_height < 0)
		fZoom_Left_height = 0;

	tmpROI = tmp(cv::Rect(fZoom_Left_width, fZoom_Left_height, width, height));

	if (iCrop_width != iZoom_width || iCrop_height != iZoom_height)
	{
		cv::addWeighted(tmpROI, 1, m_cropTmp, 1, 0, tmpROI);
		tmp.copyTo(m_zoomImg);
	}
	else
		m_cropImg.copyTo(m_zoomImg);

	m_iMouseState = ZOOM_MODE;

	m_ctrlSliderZoomRatio.SetPos(_ttof(strEdit));

	m_fZoomRatio = m_cropImg.cols / (float)m_cropTmp.cols;
	//cv::imshow("cropImg", m_cropImg);
}


void CLiveVesselDlg::OnNMCustomdrawSliderZoomRatio(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	if (m_bZoom)
	{
		int iCurPos;

		iCurPos = m_ctrlSliderZoomRatio.GetPos();
		strEdit.Format(_T("%d"), iCurPos);
		m_editZoomRatio.SetWindowTextW(strEdit);

		ZoomProcessing();
		OnPaint();
	}
	*pResult = 0;
}


void CLiveVesselDlg::OnBnClickedRadioNormal()
{
	m_iMode_zoomWnd = 0;
	m_bMove = false;
}


void CLiveVesselDlg::OnBnClickedRadioDraw()
{
	m_iMode_zoomWnd = 1;
	m_bMove = false;
}


void CLiveVesselDlg::OnBnClickedRadioMove()
{
	m_iMode_zoomWnd = 2;
	m_bMove = true;
}


BOOL CLiveVesselDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(LoadCursor(NULL, (m_iMode_zoomWnd == 2) ? IDC_HAND : IDC_ARROW));

	return TRUE;

	//return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CLiveVesselDlg::OnBnClickedButtonVcoFrame()
{
	if (!m_bLoad)
		return;

	//2016.12.22_daseul
	m_tmpLine.clear();

	int nY = vesselImg.rows;
	int nX = vesselImg.cols;
	int cur_frame = iSliderPos;

	cv::Mat d_tVc = cv::Mat::zeros(nY, nX, CV_8UC1);
	SegmTree.drawlines_8UC1(d_tVc);
	CString aa = L"\\";

	// load frames
	printf("start %dth frame\n", cur_frame);
	m_ctrlProgress.SetPos(20); //2017.02.17 SJKOH -PROGRESS
	cv::Mat d_tVesselImg = cv::imread(vecFname[cur_frame], CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat d_tp1VesselImg = cv::imread(vecFname[cur_frame + 1], CV_LOAD_IMAGE_GRAYSCALE);

	//cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, cur_frame + 1, nX, nY, false, "result/");
	bool bPostProc = true;
	// ***** EXCEPTION THROWN @2016.12.19-02:38:00 ***** //
	WriteLog(__FILE__, __LINE__, __FUNCTION__, "START OF VCO");
	char LOG[128];
	sprintf_s(LOG, "d_tVesselImg.(nrow, ncol):(%d, %d)", d_tVesselImg.rows, d_tVesselImg.cols);	WriteLog(LOG);
	sprintf_s(LOG, "d_tVc.(nrow, ncol):(%d, %d)", d_tVc.rows, d_tVc.cols);	WriteLog(LOG);
	sprintf_s(LOG, "d_tp1VesselImg.(nrow, ncol):(%d, %d)", d_tp1VesselImg.rows, d_tp1VesselImg.cols);	WriteLog(LOG);
	sprintf_s(LOG, "cur_frame, nX, nY: %d, %d, %d", cur_frame, nX, nY);	WriteLog(LOG);
	cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, cur_frame + 1, nX, nY, 
		m_bPostProc, true, "../result/");
	vco.VesselCorrespondenceOptimization();
	std::vector<std::vector < cv::Point >> tp1_2d_ves_pts;
	if (!m_bPostProc)
		tp1_2d_ves_pts = vco.getVsegVpts2dArr();
	else
		tp1_2d_ves_pts = vco.getVsegVpts2dArr_pp();
	WriteLog(__FILE__, __LINE__, __FUNCTION__, "END OF VCO");
	m_ctrlProgress.SetPos(40); // 2017.02.07 SJKOH - PRGORESS 
	// initialize vessel-segmentation tree from vco results
	SegmTree.newSetTree(tp1_2d_ves_pts);
	// load pre-computed data - required to compute region mask
	std::string pre_fn = ChangeFilenameExtension(vecFname[cur_frame + 1], "pre");
	LoadFrangi(pre_fn);
	// clear mask
	m_mask = 0;
	// compute region mask and refine centerline from vco results
	for (int j = 0; j < SegmTree.nSegm; j++)
	{
		ComputeVesselRadii(SegmTree.get(j), FrangiScale, d_tp1VesselImg);
		MakeRegionMask_VesRadii(SegmTree.get(j), m_mask);
	}
	// draw updated vessel centerline to d_tVc
	d_tVc = cv::Mat::zeros(nY, nX, CV_8UC1); // initilize
	SegmTree.drawlines_8UC1(d_tVc);
	// make graph structure from mask
	cP2pMatching p2p;
	cv::Mat bJ, mapMat;
	std::vector<cv::Point> J, End;
	std::vector<std::vector<cv::Point>> E;
	p2p.MakeGraphFromImage(d_tVc, J, End, bJ, E, mapMat);
	// construct feature tree
	featureTree.init(SegmTree, J);

	// *** save results *** //
	// save region mask (=m_mask)

	m_ctrlProgress.SetPos(60); //2017.02.07 SJKOH - PROGRESS
	char maskSavePath[200];
	std::string cvtFname = vecFname[cur_frame + 1];
	cvtFname.erase(cvtFname.length() - 4, 4);
	sprintf(maskSavePath, "%s_mask.png", cvtFname.data());
	cv::imwrite(maskSavePath, m_mask);
	// save vessel centerline mask (=d_tVc)
	cv::String str = cvtFname + "_vsc_mask.png";
	cv::imwrite(str, d_tVc);
	// save vessel feature map - 161010_daseul
#ifdef NEW_VSC_TYPE
	std::string vsc_fn = ChangeFilenameExtension(vecFname[cur_frame + 1], "vs2");
	SaveVesselCenterlineInfo(vsc_fn);
#else
	std::string vsc_fn = ChangeFilenameExtension(vecFname[cur_frame + 1], "vsc");
	SaveVesselCenterlinePoints(vsc_fn);
	m_ctrlProgress.SetPos(80); //2017.02.07 SJKOH - PROGRESS
#endif
	// *** end saving results *** //

	// 2017.02.07 SJKOH - CANCEL
	ProcessWindowMessage();
	if (m_bCancel)
		iSliderPos = cur_frame;
	else
		iSliderPos = cur_frame + 1;

	iLineIndex = -1; // coded by kjNoh 160922
	selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922
	iLineSelected_state = CLEAR_LINE;
	iFeatSelected_state = CLEAR_FEATURE;

	updateSegm();
	dispUpdate(false);
	m_ctrlProgress.SetPos(100); //2017.02.07 SJKOH - PROGRESS
	printf("VCO_frame done!!\n\n");
	m_ctrlProgress.SetPos(0); //2017.02.07 SJKOH - PROGRESS
}


void CLiveVesselDlg::SaveFrangi(std::string ffpath)
{
	frangiImg = frfilt.frangi(vesselImgG, &FrangiScale);
	frangiImg.convertTo(frangiImg, CV_64F);
	FrangiScale.convertTo(FrangiScale, CV_64F);

	FILE *f = fopen(ffpath.data(), "wb+");
	fwrite(((double*)frangiImg.data), sizeof(double), frangiImg.rows*frangiImg.cols, f);
	fwrite(((double*)FrangiScale.data), sizeof(double), FrangiScale.rows*FrangiScale.cols, f);
	fclose(f);
}

bool CLiveVesselDlg::LoadFrangi(std::string ffPath)
{
	// try to load pre-saved pre-processing data
	int check = _access(ffPath.data(), 0);
	// if successful, load saved data
	if (!check)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		double *ffData = new double[vesselImg.rows*vesselImg.cols];
		double *ffSacleData = new double[vesselImg.rows*vesselImg.cols];
		FILE *f = fopen(ffPath.data(), "rb");
		fread(ffData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
		fread(ffSacleData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
		cv::Mat tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffData);
		tmp.copyTo(frangiImg);
		tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffSacleData);
		tmp.copyTo(FrangiScale);
		delete[] ffData;
		delete[] ffSacleData;
		fclose(f);
		return true;
	}
	return false;
}

bool CLiveVesselDlg::LoadVesselCenterlineInfo(std::string vsc_fn)
{
	// check file extension - must be "vs2"
	std::string ext = GetFileExtension(vsc_fn);
	assert(ext.compare("vs2") == 0);
	// 
	if (!_access(vsc_fn.data(), 0))
	{
		int nSegm;
		FILE *f = fopen(vsc_fn.data(), "rb");
		fread(&nSegm, sizeof(int), 1, f);
		for (int j = 0; j < nSegm; j++)
		{
			//std::vector<cv::Point> curSegm;
			CVesSegm curSegm;
			int nPts;
			fread(&nPts, sizeof(int), 1, f);
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			for (int k = 0; k < nPts; k++)
			{
				int x, y;
				float nx, ny, r;
				fread(&x, sizeof(int), 1, f);
				fread(&y, sizeof(int), 1, f);
				fread(&nx, sizeof(float), 1, f);
				fread(&ny, sizeof(float), 1, f);
				fread(&r, sizeof(float), 1, f);
				CVesSegmPt pt(x, y, nx, ny, r);
				curSegm.push_back(pt);
			}
			// if empty vessel segment, move on
			if (!curSegm.size())
				continue;
			
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			SegmTree.addSegm(curSegm);

			int spType, epType;
			fread(&spType, sizeof(int), 1, f);
			fread(&epType, sizeof(int), 1, f);

			featureInfo feat;
			feat.sp = curSegm[0].pt;
			feat.spType = spType;
			feat.ep = curSegm.back().pt;
			feat.epType = epType;
			featureTree.addFeat(feat);
		}
		fclose(f);
		return true;
	}
	return false;
}

bool CLiveVesselDlg::LoadVesselCenterlinePoints(std::string vsc_fn)
{
	// check file extension - must be "vsc"
	std::string ext = GetFileExtension(vsc_fn);
	assert(ext.compare("vsc") == 0);
	// 
	if (!_access(vsc_fn.data(), 0))
	{
		int nSegm;
		FILE *f = fopen(vsc_fn.data(), "rb");
		fread(&nSegm, sizeof(int), 1, f);
		SegmTree.rmAll();
		for (int j = 0; j < nSegm; j++)
		{
			//std::vector<cv::Point> curSegm;
			CVesSegm curSegm;
			int nPts;
			fread(&nPts, sizeof(int), 1, f);
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			for (int k = 0; k < nPts; k++)
			{
				int x, y;
				float nx, ny, r;
				fread(&x, sizeof(int), 1, f);
				fread(&y, sizeof(int), 1, f);
				CVesSegmPt pt(x, y);
				curSegm.push_back(pt);
			}
			// if empty vessel segment, move on
			if (!curSegm.size())
				continue;

			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			SegmTree.addSegm(curSegm);

			int spType, epType;
			fread(&spType, sizeof(int), 1, f);
			fread(&epType, sizeof(int), 1, f);

			featureInfo feat;
			feat.sp = curSegm[0].pt;
			feat.spType = spType;
			feat.ep = curSegm.back().pt;
			feat.epType = epType;
			featureTree.addFeat(feat);
		}
		fclose(f);
		return true;
	}
	return false;
}

void CLiveVesselDlg::SaveVesselCenterlineInfo(std::string vsc_fn)
{
	FILE *fs = fopen(vsc_fn.c_str(), "wb+");
	int nSegm = (int)SegmTree.nSegm;
	fwrite(&nSegm, sizeof(int), 1, fs);

	for (int j = 0; j < nSegm; j++)
	{
		CVesSegm tmp_Segm = SegmTree.get(j);
		int nCurFrmaeSegmPt = (int)tmp_Segm.size();
		fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, fs);

		for (int k = 0; k < nCurFrmaeSegmPt; k++)
		{
			fwrite(&tmp_Segm[k].pt.x, sizeof(int), 1, fs);
			fwrite(&tmp_Segm[k].pt.y, sizeof(int), 1, fs);
			fwrite(&tmp_Segm[k].normal.x, sizeof(float), 1, fs);
			fwrite(&tmp_Segm[k].normal.y, sizeof(float), 1, fs);
			fwrite(&tmp_Segm[k].rad, sizeof(float), 1, fs);
		}

		featureInfo spepInfo = featureTree.get(j);
		fwrite(&spepInfo.spType, sizeof(int), 1, fs);
		fwrite(&spepInfo.epType, sizeof(int), 1, fs);
		tmp_Segm.clear();
	}
	fclose(fs);
}

void CLiveVesselDlg::SaveVesselCenterlinePoints(std::string vsc_fn)
{
	FILE *fs = fopen(vsc_fn.c_str(), "wb+");
	int nSegm = (int)SegmTree.nSegm;
	fwrite(&nSegm, sizeof(int), 1, fs);

	for (int j = 0; j < nSegm; j++)
	{
		//std::vector<cv::Point> tmp_Segm = pDlg->SegmTree.get(j);
		CVesSegm tmp_Segm = SegmTree.get(j);
		int nCurFrmaeSegmPt = (int)tmp_Segm.size();
		fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, fs);

		for (int k = 0; k < nCurFrmaeSegmPt; k++)
		{
			fwrite(&tmp_Segm[k].pt.x, sizeof(int), 1, fs);
			fwrite(&tmp_Segm[k].pt.y, sizeof(int), 1, fs);
		}

		featureInfo spepInfo = featureTree.get(j);
		fwrite(&spepInfo.spType, sizeof(int), 1, fs);
		fwrite(&spepInfo.epType, sizeof(int), 1, fs);
		tmp_Segm.clear();
	}
	fclose(fs);
}

void CLiveVesselDlg::SaveVesselCenterlinePoints(std::string vsc_fn, 
	std::vector<std::vector<cv::Point>> &segm_tree)
{
	FILE *fs = fopen(vsc_fn.c_str(), "wb+");
	int nSegm = (int)segm_tree.size();
	fwrite(&nSegm, sizeof(int), 1, fs);

	for (int j = 0; j < nSegm; j++)
	{
		//std::vector<cv::Point> tmp_Segm = pDlg->SegmTree.get(j);
		std::vector<cv::Point> tmp_Segm = segm_tree[j];
		int nCurFrmaeSegmPt = (int)tmp_Segm.size();
		fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, fs);

		for (int k = 0; k < nCurFrmaeSegmPt; k++)
		{
			fwrite(&tmp_Segm[k].x, sizeof(int), 1, fs);
			fwrite(&tmp_Segm[k].y, sizeof(int), 1, fs);
		}

		featureInfo spepInfo = featureTree.get(j);
		fwrite(&spepInfo.spType, sizeof(int), 1, fs);
		fwrite(&spepInfo.epType, sizeof(int), 1, fs);
		tmp_Segm.clear();
	}
	fclose(fs);
}


// save vessel centerline mask to file
void CLiveVesselDlg::SaveVesCenterlineMask(std::string str)
{
	cv::Mat vclmask = cv::Mat::zeros(vesselImg.size(), CV_8UC1);
	SegmTree.drawlines_8UC1(vclmask);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	cv::imwrite(str, vclmask);
}


// save current frame results
void CLiveVesselDlg::FinishAndSave2File()
{
	// if there is no vessel segments in vessel tree (= no vessels extracted), do nothing
	if (SegmTree.nSegm == 0)
		return;

	// save vessel centerline coordinates to file
#ifdef NEW_VSC_TYPE
	std::string vsc_fn = ChangeFilenameExtension(vecFname[iSliderPos], "vs2");
	SaveVesselCenterlineInfo(vsc_fn);
#else
	std::string vsc_fn = ChangeFilenameExtension(vecFname[iSliderPos], "vsc");
	SaveVesselCenterlinePoints(vsc_fn);
#endif
	// save vessel centerline mask to file
	std::string maskPath = RemoveAndAppendFilepath(vecFname[iSliderPos], "_mask.png");
	cv::imwrite(maskPath, m_mask);
	// save vessel segmenation mask to file
	std::string vclPath = RemoveAndAppendFilepath(vecFname[iSliderPos], "_vsc_mask.png");
	SaveVesCenterlineMask(vclPath);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
}


void CLiveVesselDlg::OnBnClickedButtonVcoSequence()
{
	if (!m_bLoad)
		return;

	// this is VCO sqeunce using thread function
	//if (m_pThread==NULL)
	//{
	//	m_pThread = AfxBeginThread(ThreadFunction, this);

	//	if (m_pThread == NULL)
	//	{
	//		AfxMessageBox(L"Error");
	//	}

	//	m_pThread->m_bAutoDelete = TRUE;
	//	m_eThreadWork = THREAD_RUNNING;
	//}
	//else
	//{
	//	if (m_eThreadWork == TRHEAD_PAUSE)
	//	{
	//		m_pThread->ResumeThread();
	//		m_eThreadWork = THREAD_RUNNING;
	//	}
	//}

	int nY = vesselImg.rows;
	int nX = vesselImg.cols;
	int cur_frame = iSliderPos;

	int end_frame = (vecFname.size() - 1);

	cv::Mat d_tVc = cv::Mat::zeros(nY, nX, CV_8UC1);
	SegmTree.drawlines_8UC1(d_tVc);

	CString aa = L"\\";

	FILE *vscFile;
	for (int i = cur_frame; i < end_frame; i++)
	{
		printf("start %dth frame\n", i);
		cv::Mat d_tVesselImg = cv::imread(vecFname[i], CV_LOAD_IMAGE_GRAYSCALE);
		cv::Mat d_tp1VesselImg = cv::imread(vecFname[i + 1], CV_LOAD_IMAGE_GRAYSCALE);;

		cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, i + 1, nX, nY, m_bPostProc, "result/");
		vco.VesselCorrespondenceOptimization();

		cv::Mat tp1_mask_8u = vco.get_tp1_adjusted_vescl_mask_pp();
		tp1_mask_8u.copyTo(d_tVc);

		std::vector<cv::Point> J, End;
		std::vector<std::vector < cv::Point >> tp1_2d_vec_vescl = 
			vco.get_adjust_VsegVpts2dArr_pp(&J, &End);
		SegmTree.newSetTree(tp1_2d_vec_vescl);

		//cv::Mat gaussKernel = cv::getGaussianKernel(23, 4.4f);
		m_mask = 0;
		for (int j = 0; j < tp1_2d_vec_vescl.size(); j++)
		{
			ComputeVesselRadii(SegmTree.get(j), FrangiScale, d_tp1VesselImg);
			MakeRegionMask_VesRadii(SegmTree.get(j), m_mask);
			//MakeRegionMask_NKJ(tp1_2d_vec_vescl[j], FrangiScale, m_mask);
		}

		char maskSavePath[200];
		std::string cvtFname = vecFname[i + 1];
		cvtFname.pop_back(); cvtFname.pop_back(); cvtFname.pop_back(); cvtFname.pop_back();
		sprintf(maskSavePath, "%s_mask.png", cvtFname.data());
		cv::imwrite(maskSavePath, m_mask);

		cv::String str;

		//str = m_pszPathName + aa + m_fileName[i + 1] + "_vsc_mask" + ".png";
		str = cvtFname + "_vsc_mask" + ".png";
		cv::imwrite(str, tp1_mask_8u);

		featureTree.init(SegmTree, J);
		/*featureTree.rmAll();
		for (int j = 0; j < SegmTree.nSegm; j++)
		{
			std::vector<CVesSegmPt> tmp_Segm = SegmTree.get(j);
			//std::vector<cv::Point> tmp_Segm = SegmTree.get(j);
			featureInfo cur_feat;
			cur_feat.sp = tmp_Segm.front().pt;
			cur_feat.spType = 0;
			cur_feat.ep = tmp_Segm.back().pt;
			cur_feat.epType = 0;
			for (int k = 0; k < J.size(); k++)
			{
				if (cur_feat.sp == J[k])
				{
					cur_feat.spType = 1;
				}
				if (cur_feat.ep == J[k])
				{
					cur_feat.epType = 1;
				}

			}
			featureTree.addFeat(cur_feat);
			tmp_Segm.clear();
		}*/

		//vscFile = fopen(m_pszPathName + aa + m_fileName[i + 1] + ".vsc", "wb+");

		//161010_daseul
		str = cvtFname + ".vsc";
		const char*path = str.c_str();
		vscFile = fopen(path, "wb+");

		int nCurFrmaeSegm = (int)tp1_2d_vec_vescl.size();
		fwrite(&nCurFrmaeSegm, sizeof(int), 1, vscFile);

		for (int j = 0; j < nCurFrmaeSegm; j++)
		{
			std::vector<cv::Point> tmp_Segm = tp1_2d_vec_vescl[j];
			int nCurFrmaeSegmPt = (int)tmp_Segm.size();
			fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, vscFile);

			for (int k = 0; k < nCurFrmaeSegmPt; k++)
			{
				int x = tmp_Segm[k].x;
				int y = tmp_Segm[k].y;
				fwrite(&x, sizeof(int), 1, vscFile);
				fwrite(&y, sizeof(int), 1, vscFile);
			}
			featureInfo spepInfo = featureTree.get(j);
			fwrite(&spepInfo.spType, sizeof(int), 1, vscFile);
			fwrite(&spepInfo.epType, sizeof(int), 1, vscFile);
			tmp_Segm.clear();
		}

		fclose(vscFile);

		iSliderPos = i + 1;

		iLineIndex = -1; // coded by kjNoh 160922
		selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

		iLineSelected_state = CLEAR_LINE;
		iFeatSelected_state = CLEAR_FEATURE;

		updateSegm();
		dispUpdate(false);

		tp1_2d_vec_vescl.clear();
		J.clear();
		str.clear();

	}

	aa.Empty();

	tEnd = clock();
	fFull_Time = (float)(tEnd - tStart) / (CLOCKS_PER_SEC);
	printf("측정 시간 : %f\n\n", fFull_Time);
}

/////////////////////////////////////////
// coded by kjNoh 160922
void CLiveVesselDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));

	if (selectedFeat != cv::Point(-1, -1))
	{
		for (int i = 0; i < featureTree.getSize(); i++)
		{
			featureInfo feat = featureTree.get(i);
			if (feat.sp == selectedFeat)
			{
				if (feat.spType)
					featureTree.vecFeature[i].spType = 0;
				else
					featureTree.vecFeature[i].spType = 1;
			}
			else if (feat.ep == selectedFeat)
			{
				if (feat.epType)
					featureTree.vecFeature[i].epType = 0;
				else
					featureTree.vecFeature[i].epType = 1;
			}
		}
	}

	m_curVesSegmStartPt = cv::Point(-1, -1);
	OnPaint();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
/////////////////////////////////////////

/////////////////////////////////////////
// coded by kjNoh 160922
void CLiveVesselDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (selectedFeat == cv::Point(-1, -1))
		return;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	iLineSelected_state = CLEAR_LINE;
	mouseR_state = 1;

	for (int i = 0; i < featureTree.getSize(); i++)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		featureInfo tmp_feat = featureTree.get(i);
		if (tmp_feat.sp == selectedFeat && !tmp_feat.spType)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			dragIdx = i;
			dragSpEp = 0;
			m_curVesSegmStartPt = tmp_feat.ep;
			break;
		}

		if (tmp_feat.ep == selectedFeat && !tmp_feat.epType)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			dragIdx = i;
			dragSpEp = 1;
			m_curVesSegmStartPt = tmp_feat.sp;
			break;
		}
	}

	if (dragSpEp == -1 || dragIdx == -1)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		mouseR_state = 0;
		return;
	}

	//for (int i = 0; i < (*SegmTree)[dragIdx].size(); i++)
	//	segLabelImg.at<int>((*SegmTree)[dragIdx][i]) = 0;
	//featrueImage.at<int>((*SegmTree)[dragIdx][0]) = 0;
	//featrueImage.at<int>((*SegmTree)[dragIdx].back()) = 0;

	oldFeat = featureTree.get(dragIdx);
	//(*SegmTree).erase((*SegmTree).begin() + dragIdx);
	SegmTree.rmSegm(dragIdx);
	featureTree.rmFeat(dragIdx);
	//vecSpEpJp[iSliderPos].erase(vecSpEpJp[iSliderPos].begin() + dragIdx);

	iLineIndex = -1;
	updateSegm();

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	OpenEndedFMM(m_curVesSegmEndPt);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	OnPaint();
	CDialogEx::OnRButtonDown(nFlags, point);
}
/////////////////////////////////////////

/////////////////////////////////////////
// coded by kjNoh 160922
void CLiveVesselDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (!mouseR_state)
		return;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));

	mouseR_state = 0;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// edited by kjNoh 160922
	if (iFeatSelected_state == SELECT_FEATURE)
		m_curVesSegmEndPt = selectedFeat;
	else
		m_curVesSegmEndPt = center;
	//PointsOfLine();

	fmm.compute_discrete_geodesic(frangiDist, m_curVesSegmEndPt, &m_curVesSegmPath);
	std::reverse(m_curVesSegmPath.begin(), m_curVesSegmPath.end()); // coeded by kjNoh 160922

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	std::vector<cv::Point> vecPts;
	//vecPts.push_back(m_curVesSegmStartPt);
	for (int i = 0; i < m_curVesSegmPath.size(); i++)
		vecPts.push_back(m_curVesSegmPath[i]);
	//vecPts.push_back(m_curVesSegmEndPt);

	// coeded by kjNoh 160922
	if (!dragSpEp)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		std::reverse(vecPts.begin(), vecPts.end());
		cv::Point tmp = m_curVesSegmStartPt;
		m_curVesSegmStartPt = m_curVesSegmEndPt;
		m_curVesSegmEndPt = tmp;
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	SegmTree.insert(dragIdx, vecPts);

	//////////////////////////
	//coded by kjNoh 160922 
	featureInfo cur_spepInfor;
	cur_spepInfor.sp = m_curVesSegmStartPt;
	cur_spepInfor.ep = m_curVesSegmEndPt;
	if (!dragSpEp)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		cur_spepInfor.spType = 0;
		cur_spepInfor.epType = oldFeat.epType;
	}
	else
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		cur_spepInfor.spType = oldFeat.spType;
		cur_spepInfor.epType = 0;
	}
	featureTree.insert(dragIdx, cur_spepInfor);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);


	for (int i = 0; i < vecPts.size(); i++)
		SegLabelImg.at<int>(vecPts[i]) = dragIdx + 1;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	FeatrueImg.at<int>(vecPts[0]) = dragIdx + 1;
	FeatrueImg.at<int>(vecPts.back()) = dragIdx + 1;
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	//////////////////////////

	m_curVesSegmStartPt = cv::Point(-1, -1);
	m_curVesSegmEndPt = cv::Point(-1, -1);

	dragIdx = -1;
	dragSpEp = -1;

	iLineIndex = -1;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	m_mask = 0;
	for (int i = 0; i < SegmTree.nSegm; i++)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		/*
		//std::vector<cv::Point> vecPts = SegmTree.get(i);
		std::vector<CVesSegmPt> vecPts = SegmTree.get(i);
		cv::Mat convScale(1, vecPts.size(), CV_64FC1);
		for (int k = 0; k < vecPts.size(); k++)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);

			int cur_x = vecPts[k].pt.x;
			int cur_y = vecPts[k].pt.y;
			double s = FrangiScale.at<double>(cur_y, cur_x);

			convScale.at<double>(0, k) = s;
		}

		cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

		for (int k = 0; k < vecPts.size(); k++)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			int cur_x = vecPts[k].x;
			int cur_y = vecPts[k].y;
			cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
		}
		convScale.release();*/
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	updateSegm();

	OnPaint();
	CDialogEx::OnRButtonUp(nFlags, point);
}
/////////////////////////////////////////

/////////////////////////////////////////
// coded by kjNoh 160922
void CLiveVesselDlg::updateSegm()
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	if (SegLabelImg.empty() || FeatrueImg.empty())
	{
		SegLabelImg = cv::Mat::zeros(vesselImg.size(), CV_32SC1);
		FeatrueImg = cv::Mat::zeros(vesselImg.size(), CV_32SC1);
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
	}
	else
	{
		SegLabelImg = 0;
		FeatrueImg = 0;
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
	}

	for (int i = 0; i < SegmTree.nSegm; i++)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		if (SegmTree.nSegm)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			//std::vector<cv::Point> segm = SegmTree.get(i);
			CVesSegm segm = SegmTree.get(i);
			for (int j = 0; j < segm.size(); j++)
				SegLabelImg.at<int>(segm[j].pt) = i + 1;

			FeatrueImg.at<int>(segm.front().pt) = i + 1;
			FeatrueImg.at<int>(segm.back().pt) = i + 1;
		}
	}

	if (iLineIndex != -1)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		featureTree.rmFeat(iLineIndex);
	}
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
}
/////////////////////////////////////////


void CLiveVesselDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	CloseLogStream();

	vecPoints.clear();
	m_vecPtZoom.clear();
	vecFname.clear();
}

void CLiveVesselDlg::dispUpdate(bool bPaint)
{
	// minor initializations
	iLineSelected_state = CLEAR_LINE;
	iLineIndex = -1;
	iFeatSelected_state = CLEAR_FEATURE;
	selectedFeat = cv::Point(-1, -1);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// init objects to load
	SegmTree.rmAll();
	featureTree.rmAll();

	// load current frame
	vesselImg = cv::imread(vecFname[iSliderPos]);
	cv::cvtColor(vesselImg, vesselImgG, CV_BGR2GRAY);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// load frangi filter results
	std::string fr_fn = ChangeFilenameExtension(vecFname[iSliderPos], "pre");
	if ( !LoadFrangi(fr_fn) )
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		SaveFrangi(fr_fn);
	}
	// load vessel centerline information
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
#ifdef NEW_VSC_TYPE
	std::string ststr = ChangeFilenameExtension(vecFname[iSliderPos], "vs2");
	LoadVesselCenterlineInfo(ststr);
#else
	std::string ststr = ChangeFilenameExtension(vecFname[iSliderPos], "vsc");
	LoadVesselCenterlinePoints(ststr);
	SegmTree.SetVesselRadiiFromFraigiScale(FrangiScale);
#endif
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// load vessel centerline mask
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	std::string maskPath = RemoveAndAppendFilepath(vecFname[iSliderPos], "_mask.png");
	m_mask = cv::imread(maskPath, CV_LOAD_IMAGE_GRAYSCALE);
	if (m_mask.empty())	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		m_mask = cv::Mat::zeros(vesselImg.size(), CV_8UC1);
	}

	m_ctrlSlider.SetPos(iSliderPos);
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	if (bPaint)
		OnPaint(); WriteLog(__FILE__, __LINE__, __FUNCTION__);
}

UINT CLiveVesselDlg::ThreadFunction(LPVOID _mothod)
{
	CLiveVesselDlg *pDlg = (CLiveVesselDlg*)_mothod;

	int nY = pDlg->vesselImg.rows;
	int nX = pDlg->vesselImg.cols;
	int cur_frame = pDlg->iSliderPos;

	int end_frame = (pDlg->vecFname.size() - 1);

	cv::Mat d_tVc(nY, nX, CV_8UC1);
	d_tVc = 0;

	for (int i = 0; i < pDlg->SegmTree.nSegm; i++)
	{
		//std::vector<cv::Point> tmp_Segm = pDlg->SegmTree.get(i);
		CVesSegm tmp_Segm = pDlg->SegmTree.get(i);
		for (int j = 0; j < tmp_Segm.size(); j++)
		{
			d_tVc.at<uchar>(tmp_Segm[j].pt) = 255;
		}
	}

	CString aa = L"\\";

	//161010_daseul
	// *** save vessel centerline result file FOR CURRENT FILE *** // 
#ifdef NEW_VSC_TYPE
	std::string vsc_fn = ChangeFilenameExtension(pDlg->vecFname[cur_frame], "vs2");
	pDlg->SaveVesselCenterlineInfo(vsc_fn);
#else
	std::string vsc_fn = ChangeFilenameExtension(pDlg->vecFname[cur_frame], "vsc");
	pDlg->SaveVesselCenterlinePoints(vsc_fn);
#endif

	// *** FOR ALL SEQUENCE FILES *** // 
	for (int i = cur_frame; i < end_frame; i++)
	{		
		printf("start %dth frame\n", i);
		// load frames
		cv::Mat d_tVesselImg = cv::imread(pDlg->vecFname[i], CV_LOAD_IMAGE_GRAYSCALE);
		cv::Mat d_tp1VesselImg = cv::imread(pDlg->vecFname[i + 1], CV_LOAD_IMAGE_GRAYSCALE);;
		// RUN VCO
		cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, i + 1, nX, nY, false, "result/");
		vco.VesselCorrespondenceOptimization();
		// GET VCO RESULTS
		// ***** TODO : CLEAN THIS PROCESS UP ***** // 
		cv::Mat tp1_mask_8u = vco.get_tp1_adjusted_vescl_mask_pp();
		tp1_mask_8u.copyTo(d_tVc);
		std::vector<cv::Point> J, End;
		std::vector<std::vector < cv::Point >> tp1_2d_vec_vescl = 
			vco.get_adjust_VsegVpts2dArr_pp(&J, &End);
		pDlg->SegmTree.newSetTree(tp1_2d_vec_vescl);
		// compute post processing: compute vessel radius / normal vectors from region mask
		for (int j = 0; j < tp1_2d_vec_vescl.size(); j++)
		{
			ComputeVesselRadii(pDlg->SegmTree.get(j), pDlg->FrangiScale, d_tp1VesselImg);
			MakeRegionMask_VesRadii(pDlg->SegmTree.get(j), pDlg->m_mask);
			//MakeRegionMask_NKJ(tp1_2d_vec_vescl[j], pDlg->FrangiScale, pDlg->m_mask);
		}
		// ***** TODO : CLEAN THIS PROCESS UP ***** // 

		// SAVE RESULTS FOR FOLLOWING FRAME
		cv::String str;
		std::string cvtFname2 = pDlg->vecFname[cur_frame + i + 1];
		cvtFname2.pop_back(); cvtFname2.pop_back(); cvtFname2.pop_back(); cvtFname2.pop_back();
		str = cvtFname2 + "_vsc_mask" + ".png";
		cv::imwrite(str, tp1_mask_8u);

		//SegmTree[cur_frame + i+1].rmAll();
		//SegmTree[cur_frame + i+1].vecSegmTree = tp1_2d_vec_vescl;
		pDlg->featureTree.init(pDlg->SegmTree, J);
		/*pDlg->featureTree.rmAll();
		for (int j = 0; j < pDlg->SegmTree.nSegm; j++)
		{
			std::vector<cv::Point> tmp_Segm = pDlg->SegmTree.get(j);
			featureInfo cur_feat;
			cur_feat.sp = tmp_Segm.front();
			cur_feat.spType = 0;
			cur_feat.ep = tmp_Segm.back();
			cur_feat.epType = 0;
			for (int k = 0; k < J.size(); k++)
			{
				if (cur_feat.sp == J[k])
				{
					cur_feat.spType = 1;
				}
				if (cur_feat.ep == J[k])
				{
					cur_feat.epType = 1;
				}
			}
			pDlg->featureTree.addFeat(cur_feat);
			tmp_Segm.clear();
		}*/

		//161010_daseul
#ifdef NEW_VSC_TYPE
		std::string vsc_fn = ChangeFilenameExtension(pDlg->vecFname[cur_frame + i + 1], "vs2");
		pDlg->SaveVesselCenterlineInfo(vsc_fn);
#else
		std::string vsc_fn = ChangeFilenameExtension(pDlg->vecFname[cur_frame + i + 1], "vsc");
		pDlg->SaveVesselCenterlinePoints(vsc_fn);
#endif

		pDlg->iSliderPos = i + 1;
		pDlg->iLineIndex = -1; // coded by kjNoh 160922
		pDlg->selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922
		pDlg->iLineSelected_state = CLEAR_LINE;
		pDlg->iFeatSelected_state = CLEAR_FEATURE;

		pDlg->updateSegm();
		pDlg->dispUpdate(false);

		// CLEAR
		tp1_2d_vec_vescl.clear();
		J.clear();
		str.clear();
	}

	aa.Empty();

	//pDlg->m_pThread->SuspendThread();
	//DWORD dwResult;
	//::GetExitCodeThread(pDlg->m_pThread->m_hThread, &dwResult);

	//delete pDlg->m_pThread;
	//pDlg->m_pThread = NULL;

	//pDlg->m_eThreadWork = THREAD_STOP;

	//pDlg->iLineIndex = -1; // coded by kjNoh 160922
	//pDlg->selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

	//pDlg->iLineSelected_state = CLEAR_LINE;
	//pDlg->iFeatSelected_state = CLEAR_FEATURE;

	//pDlg->updateSegm();
	//pDlg->dispUpdate();

	//printf("VCO_sequence done!!\n\n");

	return 0;
}

void CLiveVesselDlg::OnBnClickedButtonThreadPause()
{
	if (m_pThread == NULL)
	{
		AfxMessageBox(L"Thread Not Start!!");
	}
	else
	{
		m_pThread->SuspendThread();
		DWORD dwResult;
		::GetExitCodeThread(m_pThread->m_hThread, &dwResult);

		delete m_pThread;
		m_pThread = NULL;

		m_eThreadWork = THREAD_STOP;

		iLineIndex = -1; // coded by kjNoh 160922
		selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

		iLineSelected_state = CLEAR_LINE;
		iFeatSelected_state = CLEAR_FEATURE;

		updateSegm();
		dispUpdate();
	}
}


void CLiveVesselDlg::OnBnClickedCheckPostProcessing()
{
	int iState = IsDlgButtonChecked(IDC_CHECK_POST_PROCESSING);

	if (iState)
		m_bPostProc = true;
	else
		m_bPostProc = false;
}


void CLiveVesselDlg::OnBnClickedCheckOverlay()
{
	int iState = IsDlgButtonChecked(IDC_CHECK_OVERLAY);

	if (iState)
		m_bOverlay = true;
	else
		m_bOverlay = false;

	OnPaint();
}


void CLiveVesselDlg::OnBnClickedCheckManualEdit()
{
	int iState = IsDlgButtonChecked(IDC_CHECK_MANUAL_EDIT);

	if (iState)
		m_bManualEdit = true;
	else
		m_bManualEdit = false;

	ClearCurPath();
	m_bDrawManualSegment = false;
	m_bDrawFMMSegment = false;
	OnPaint();
}

std::vector<cv::Point> CLiveVesselDlg::getLine(cv::Point sp, cv::Point ep)
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	int subX = ep.x - sp.x;
	int subY = ep.y - sp.y;

	double divX = subX / 1000.f;
	double divY = subY / 1000.f;

	std::vector<double> xs;
	std::vector<double> ys;
	std::vector<cv::Point> pts;

	xs.push_back(sp.x);
	ys.push_back(sp.y);

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	for (int i = 1; i < 1000+1; i++)
	{
		xs.push_back(xs[i - 1] + divX);
	}
	for (int i = 1; i < 1000+1; i++)
	{
		ys.push_back(ys[i - 1] + divY);
	}

	for (int i = 0; i < xs.size(); i++)
	{
		pts.push_back(cv::Point(std::floor(xs[i] + 0.5), std::floor(ys[i]+0.5)));
	}

	std::vector<cv::Point>::iterator pos;
	pos = std::unique(pts.begin(), pts.end());
	pts.erase(pos, pts.end());

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	return pts;
}


void CLiveVesselDlg::OnClose()
{
	CloseLogStream();
	CDialogEx::OnClose();
}


int CALLBACK BrowseCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CHAR lpszDir[255];
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	case BFFM_SELCHANGED:
		if (SHGetPathFromIDList((LPITEMIDLIST)lParam, (LPWSTR)lpszDir))
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)lpszDir);
		break;
	}
	return 0;
}

void CLiveVesselDlg::ReDrawMask()
{
	m_mask = 0;
	for (int i = 0; i < SegmTree.nSegm; i++)
	{
		CVesSegm vesSegm = SegmTree.get(i);
		MakeRegionMask_VesRadii(vesSegm, m_mask);
		//ComputeVesselRadii(vesSegm, FrangiScale, vesselImgG);
		//MakeRegionMask_NKJ(vecPts, FrangiScale, m_mask);
	}
	updateSegm();
	OnPaint();
}

void CLiveVesselDlg::OnBnClickedButtonConvertData()
{
	// get sequence directory
	CString dir = L"F:\\0000_DATA\\001_VESSEL\\201609-YSCS-IITP_Y4-CTO_DEMO\\8196659-S4_MASK_TEST-20161227\\";
	// convert per-frame data
	ConvertPerFrameData(dir);
	// construct sequence CSV data file
	ConstructSeqCSVDataFile(dir);
}

// convert per-frame data without vessel normal and radius
// compute normal for each vessel point, (re)compute vessel region as required
// save new per-frame files 
void CLiveVesselDlg::ConvertPerFrameData(CString dir)
{
	// make list of files to convert
	std::vector<std::string> flistfrm;
	FindFilesInDir(dir, L"bmp", flistfrm);
	//assert(flistvsc.size() == flistpre.size());
	// convert all files
	for (int i = 0; i < flistfrm.size(); i++) {
		std::string vsc_fn = ChangeFilenameExtension(flistfrm[i], "vsc");
		// load saved vsc file - OLD FORMAT
		if (LoadVesselCenterlinePoints(vsc_fn)) {
			// load frame
			vesselImg = cv::imread(flistfrm[i], CV_LOAD_IMAGE_GRAYSCALE);
			// load saved pre file
			std::string pre_fn = ChangeFilenameExtension(flistfrm[i], "pre");
			if (LoadFrangi(pre_fn)) {
				// compute region mask, revise vessel centerline, 
				//	compute vessel radius and normal vectors
				for (int j = 0; j < SegmTree.nSegm; j++)
				{
					ComputeVesselRadii(SegmTree.get(j), FrangiScale, vesselImg);
					MakeRegionMask_VesRadii(SegmTree.get(j), m_mask);
				}
				// save vsc file - NEW FORMAT
				std::string fn_vsc2 = ChangeFilenameExtension(flistfrm[i], "vs2");
				SaveVesselCenterlineInfo(fn_vsc2);
				// redraw and save vessel centerline mask
				std::string vclpath = RemoveAndAppendFilepath(flistfrm[i], "_vsc_mask.png");
				SaveVesCenterlineMask(vclpath);
				// redraw and save vessel region mask 
				cv::Mat vcregmask = cv::Mat::zeros(vesselImg.size(), CV_8UC1);
				SegmTree.draw2mask(vcregmask);
				std::string vregpath = RemoveAndAppendFilepath(flistfrm[i], "_mask.png");
				cv::imwrite(vregpath, vcregmask);
			}
		}
	}
}

// construct *.csv file corresponding to whole sequence .dicom file
void CLiveVesselDlg::ConstructSeqCSVDataFile(CString dir)
{
	   
}


//zoom in&out function using ctrl + mouse wheel
BOOL CLiveVesselDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//2017.01.05_daseul _zoom in&out fuction
	//if bCtrl is true, start zoom function
	bWheel = true;

	if (bCtrl)
	{
		if (zDelta >= 0)	//up scroll, zoom in
			m_fZoomRatio += 0.3;

		else				//down scroll, zoom out
		{
			m_fZoomRatio -= 0.3;
			if (m_fZoomRatio < 1)
				m_fZoomRatio = 1;
		}
		OnPaint();
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

//2017.02.07 - SJKOH - CANCEL BUTTON
void CLiveVesselDlg::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bCancel = true;
}


//2017.02.07 - SJKOH - PROGRESSWINDOWMESSAGE
void CLiveVesselDlg::ProcessWindowMessage()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		::SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
	}
}
