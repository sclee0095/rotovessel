
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
, mouseL_state(false)
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
	//ON_BN_CLICKED(IDC_BUTTON_FINISH, &CLiveVesselDlg::FinishAndSave2File)
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
END_MESSAGE_MAP()

int CALLBACK BrowseCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);


// CLiveVesselDlg 메시지 처리기

BOOL CLiveVesselDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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


	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	
	// to write log file
	char log_file_path[200];
	sprintf(log_file_path,"RotoVessel.log");
	InitLogStream(log_file_path);

	vesselImg = NULL;
	m_ctrlSlider.SetPos(0);
	ptStart = cv::Point(-1, -1);
	ptEnd = cv::Point(-1, -1);
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

	m_picZoom.GetWindowRect(&m_rcZoom);
	ScreenToClient(&m_rcZoom);
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
	featSearchRagne = 5;
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
			
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
			cv::Mat disp;
			vesselImg.copyTo(disp);

			//if (SegmTree.size() != 0)
			if (m_bOverlay)
			{
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
				for (int y = 0; y < m_mask.rows; y++)
				for (int x = 0; x < m_mask.cols; x++)
				{
					if (m_mask.at<uchar>(y, x))
					{
						int R = 150;
						int G = vesselImg.at<uchar>(y, x * 3 + 1);
						int B = vesselImg.at<uchar>(y, x * 3 + 0);

						disp.at<uchar>(y, x * 3 + 0) = B;
						disp.at<uchar>(y, x * 3 + 1) = G;
						disp.at<uchar>(y, x * 3 + 2) = R;
					}
					else
					{
						int R = vesselImg.at<uchar>(y, x * 3 + 2);
						int G = vesselImg.at<uchar>(y, x * 3 + 1);
						int B = vesselImg.at<uchar>(y, x * 3 + 0);

						disp.at<uchar>(y, x * 3 + 0) = B;
						disp.at<uchar>(y, x * 3 + 1) = G;
						disp.at<uchar>(y, x * 3 + 2) = R;
					}
				}
				
				for (int i = 0; i < SegmTree.nSegm; i++)
				{
					if (SegmTree.vecSegmTree[i].size() != 0)
					{
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
						//std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
						CVesSegm tmp_Segm = SegmTree.get(i);
						for (int j = 0; j < tmp_Segm.size(); j++)
						{
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");

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
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
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
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
							//std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
							CVesSegm tmp_Segm = SegmTree.get(i);
							for (int j = 0; j < tmp_Segm.size(); j++)
							{
								WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;

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
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;
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



				//for (int y = 0; y < FrangiScale.rows; y++)
				//for (int x = 0; x < FrangiScale.cols; x++)
				//{
				//	if (FrangiScale.at<double>(y, x))
				//	{
				//		double B = FrangiScale.at<double>(y, x * 3 + 0);
				//		double G = FrangiScale.at<double>(y, x * 3 + 1);
				//		double R = 200;

				//		disp.at<uchar>(y, x * 3 + 0) = B;
				//		disp.at<uchar>(y, x * 3 + 1) = G;
				//		disp.at<uchar>(y, x * 3 + 2) = R;
				//	}
				//}

				if (iLineSelected_state == SELECT_LINE)
				{
					assert(SegmTree.nSegm > iLineIndex);
					WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
					//std::vector<cv::Point> selectedSegm = SegmTree.get(iLineIndex);
					CVesSegm selectedSegm = SegmTree.get(iLineIndex);
					for (int i = 0; i < selectedSegm.size(); i++)
					{

						for (int y = -1; y <= 1; y++)
						for (int x = -1; x <= 1; x++)
						{
							int pt_x = selectedSegm[i].pt.x;
							int pt_y = selectedSegm[i].pt.y;
							disp.at<uchar>(pt_y, pt_x * 3 + 0) = 255;
							disp.at<uchar>(pt_y, pt_x * 3 + 1) = 0;
							disp.at<uchar>(pt_y, pt_x * 3 + 2) = 255;
						}
					}


				}

				else if (iFeatSelected_state == CLEAR_LINE)
				{
					if (iLineIndex != -1)
					{
						assert(SegmTree.nSegm > iLineIndex);
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;

						//std::vector<cv::Point> curSegm = SegmTree.get(iLineIndex);
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
					WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;
					int label = FeatrueImg.at<int>(selectedFeat)-1;

					assert(featureTree.nFeature > label);

					if (featureTree.getSize() && label >= 0)
					{
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
						featureInfo tmp_feat = featureTree.get(label);
						if (tmp_feat.sp == selectedFeat)
						{
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;

							if (tmp_feat.spType)
								cv::circle(disp, tmp_feat.sp, 3, CV_RGB(0, 255, 0), -1);
							else
								cv::circle(disp, tmp_feat.sp, 3, CV_RGB(255, 0, 0), -1);
						}
						else
						{
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;
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
					WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
					if (ptStart != cv::Point(-1, -1)) {
						//cv::line(disp, ptStart, ptEnd, CV_RGB(255, 0, 0));
						// *** LiveVessel method (modified) - find gradient-based geodesic between two points
						for (int i = 0; i < cur_path.size(); i++)
						{
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
							disp.at<uchar>(cur_path[i].y, cur_path[i].x * 3 + 0) = 0;
							disp.at<uchar>(cur_path[i].y, cur_path[i].x * 3 + 1) = 0;
							disp.at<uchar>(cur_path[i].y, cur_path[i].x * 3 + 2) = 255;
						}
						cv::circle(disp, m_ptCur, m_iszDrawCircle, CV_RGB(0, 255, 255), -1);
					}

					else
					{
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
						cv::circle(disp, m_ptCur, m_iszDrawCircle, CV_RGB(0, 255, 255), -1);
					}
					disp.copyTo(m_disp_tmp);
				}
			}


			if (m_iMouseState == ZOOM_MODE || m_iMouseState == ZOOMWND_CLICK_DOWN) {
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");
				printf("onpaint_zoom\n");
				//cv::circle(m_zoomImg, cv::Point((m_ptCur_Zoom.x / m_fZoomRatio), (m_ptCur_Zoom.y / m_fZoomRatio)), 1, cv::Scalar(0, 0, 255), 1);

				cv::Mat dispZoom;
				m_zoomImg.copyTo(dispZoom);
				DrawPictureZoom(dispZoom);
			}

			DrawPicture(disp);
		}

		CDialogEx::OnPaint();
	}
}


HCURSOR CLiveVesselDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLiveVesselDlg::FindFilesInDir(CString dir, CString ext, std::vector<std::string> &path_list)
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
		//m_fileName.clear();
		bivecPointsOfLine.clear();
		vecPoints.clear();
		vecSpEpLists_Zoom.clear();
		m_vecPtZoom.clear();
		cur_path.clear();
		//2016.12.22_daseul
		m_tmpLine.clear();
		tStart = clock();
	}
	else
		return;
	// *** end: launch UI to get image sequence directory *** //
	
	// *** generate list of files of image sequence directory *** //
	FindFilesInDir(CString(m_pszPathName), L"bmp", vecFname);
	// *** end: generate list of files of image sequence directory *** //

	// minor initializations
	ptStart = ptEnd = cv::Point(-1, -1);
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
	// *** end: for all vessel sequence frames: (pre-process & save) or (load pre-processed data) *** //

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

	m_fRatio = vesselImg.cols / (float)m_rcPic.Width();
	m_width = vesselImg.cols;
	m_height = vesselImg.rows;

	m_bLoad = true;
	m_ctrlSlider.SetRange(0, vecFname.size() - 1);
	m_ctrlSlider.SetPos(0);

	updateSegm();
	OnPaint();

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	// show sequence directory name above viewing window
	m_ctrl_FileName.SetWindowText(m_pszPathName);
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
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::DrawPicture()");
	CRect rc;
	m_Picture.GetClientRect(rc);

	
	cv::Mat view_vesselImg;
	disp.copyTo(view_vesselImg);
	cv::resize(view_vesselImg, view_vesselImg, cv::Size(rc.Width(), rc.Height()));
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::DrawPicture()");

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
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::DrawPicture()", log);
	::StretchDIBits(hdc, 0, 0, nX, nY, 0, 0, nX, nY, view_vesselImg.data, (BITMAPINFO*)&bitmaphaeader, DIB_RGB_COLORS, SRCCOPY);
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::DrawPicture()");
	m_Picture.ReleaseDC(pdc);
}


void CLiveVesselDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (vecFname.size() == NULL)
	{
		OnBnClickedButtonImageLoad();
		return;
	}

	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), 
					 std::round((point.y - m_rcPic.top) * m_fRatio));

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	if (0 <= center.x && center.x < m_width&& 0 <= center.y && center.y < m_height)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		m_Picture.SetFocus();
		// if this left click is to set start point,
		// set starting point, initialize
		if (ptStart == cv::Point(-1, -1))
		{
			// edited by kjNoh 160922
			if (iFeatSelected_state == SELECT_FEATURE)
			{
				ptStart = selectedFeat;
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
			else
			{
				float minD = FLT_MAX;
				cv::Point minPt(-1,-1);
				for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
				{
					// DEBUGGED: SOOCHAHN LEE, 20161219-11:49:00
					// changed AND && to OR ||
					if ((y == 0 && x == 0) || 
						center.x + x < 0 || center.y + y < 0 ||
						center.x + x >= vesselImg.cols || center.y + y >= vesselImg.rows)
						continue;

					if (SegLabelImg.at<int>(center.y + y, center.x + x))
					{
						if (minD > std::sqrt(y*y + x*x))
						{
							minD = std::sqrt(y*y + x*x);
							minPt = center + cv::Point(x,y);
						}
					}
				}

				if (minPt != cv::Point(-1, -1))
				{
					ptStart = minPt;
				}
				else
					ptStart = center;

				WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
				
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			ptEnd = center;
			//m_iMouseState = SP_CLICK_DOWN;
		}
		// if this left click is to set end point
		else
		{
			// edited by kjNoh 160922
			if (iFeatSelected_state == SELECT_FEATURE)
			{
				ptEnd = selectedFeat;
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
			else
			{
				float minD = FLT_MAX;
				cv::Point minPt(-1, -1);
				
				for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
				{
					if (y == x && 
						center.x + x <0 && center.y + y <0 && 
						center.x + x >=vesselImg.cols&&center.y + y >=vesselImg.rows)
						continue;

					if (SegLabelImg.at<int>(center.y + y, center.x + x))
					{
						if (minD > std::sqrt(y*y + x*x))
						{
							minD = std::sqrt(y*y + x*x);
							minPt = center + cv::Point(x, y);
						}
					}
				}

				if (minPt != cv::Point(-1, -1))
				{
					ptEnd = minPt;
				}
				else
					ptEnd = center;
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
			// 	
			fmm.compute_discrete_geodesic(frangiDist, ptEnd, &cur_path);
			std::reverse(cur_path.begin(), cur_path.end()); // coeded by kjNoh 160922
			WriteLog(__FILE__, __LINE__, __FUNCTION__);

			CVesSegm vesSegm(cur_path);
			//MakeRegionMask_NKJ(vecPts, FrangiScale, m_mask);
			ComputeVesselRadii(vesSegm, FrangiScale, vesselImgG);
			MakeRegionMask_VesRadii(vesSegm, m_mask);

			SegmTree.addSegm(vesSegm);

			//////////////////////////
			//coded by kjNoh 160922 
			featureInfo cur_spepInfor;
			cur_spepInfor.sp = ptStart;
			cur_spepInfor.ep = ptEnd;
			cur_spepInfor.spType = 0;
			cur_spepInfor.epType = 0;

			featureTree.addFeat(cur_spepInfor);

			for (int i = 0; i < vesSegm.size(); i++)
				SegLabelImg.at<int>(vesSegm[i].pt) = SegmTree.nSegm;

			FeatrueImg.at<int>(vesSegm[0].pt) = SegmTree.nSegm;
			FeatrueImg.at<int>(vesSegm.back().pt) = SegmTree.nSegm;
			////////////////////////

			ptStart = ptEnd;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		
		// if manual edit, do no shortest path computations
		if (m_bManualEdit)
		{
			mouseL_state = true;
			cur_path.clear();
			m_prePt = ptStart;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		// if LiveVessel mode, initialize shortest path computations by fast marching method
		else
		{
			// perform fast-marching method, get frangiDist
			OpenEndedFMM();
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}

		//2016.12.22_daseul
		FinishAndSave2File();
	}

	if (m_bZoom)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		if (0 <= center.x && center.x < m_rcPic.Width() && 0 <= center.y && center.y < m_rcPic.Height())
		if (m_iMode_zoomWnd != 1)
			m_ptZoom = center;

		if (0 <= point.x - m_rcZoom.left && point.x - m_rcZoom.left < m_rcZoom.Width() && 0 <= point.y - m_rcZoom.top && point.y - m_rcZoom.top < m_rcZoom.Height())
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			if (!m_bMove)
			{
				WriteLog(__FILE__, __LINE__, __FUNCTION__);
				m_iMouseState = ZOOMWND_CLICK_DOWN;
				m_picZoom.SetFocus();

				m_ptCur_Zoom.x = (point.x - m_rcZoom.left) * m_fZoomRatio;
				m_ptCur_Zoom.y = (point.y - m_rcZoom.top) * m_fZoomRatio;

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

				if (ptStart == cv::Point(-1, -1))
				{
					ptStart = pt + m_leftTop;
					ptEnd = pt + m_leftTop;
				}
				else
				{
					ptEnd = pt + m_leftTop;
					//PointsOfLine();

					CVesSegm vecPts;
					vecPts.push_back(CVesSegmPt(ptStart));
					vecPts.push_back(CVesSegmPt(ptEnd));
					SegmTree.addSegm(vecPts);

					ptStart = ptEnd;
					OnPaint();
				}
			}
			else
				m_vecPtZoom.push_back(ptEnd);
		}
		ZoomProcessing();
	}

	OnPaint();
	//ZoomProcessing(); // annoated by noh 160928

	CDialogEx::OnLButtonDown(nFlags, point);
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
	double pfm_start_points[] = { ptStart.y, ptStart.x };
	double nb_iter_max = std::min(pram.pfm_nb_iter_max,
		(1.2*std::max(frangiImg.rows, frangiImg.cols)*
		std::max(frangiImg.rows, frangiImg.cols)));
	cv::Mat transPose_frangiImg;
	cv::transpose(frangiImg, transPose_frangiImg);
	transPose_frangiImg.setTo(1e-10, transPose_frangiImg < 1e-10);
	double *S;
	fmm.fast_marching(transPose_frangiImg, frangiImg.cols, frangiImg.rows,
		pfm_start_points, 1, 0, 0, nb_iter_max,
		&frangiDist, &S);
	frangiDist = frangiDist.t();
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.	

	if (!m_bLoad)
		return 0;

	bool bWorking = false;
	if (pMsg->message == WM_KEYDOWN)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		if (!bWorking)
		{
			//2016.12.21_daseul _UNDO
			if ((GetKeyState(VK_CONTROL) & 0x8000) && pMsg->wParam == 'Z')
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

			case VK_SPACE:	//Q. 포커스가 Button에 있을때 스페이스 누르면 영상 로드 등 버튼 기능 수행 _해결방법찾아보기
				ptStart = ptEnd = cv::Point(-1, -1);
				mouseL_state = false;
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

				break;
			
			}

		}
	}
	//return CDialogEx::PreTranslateMessage(pMsg);
	return 0;
}


void CLiveVesselDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bLoad)
		return;

	iLineSelected_state = CLEAR_LINE;
	iFeatSelected_state = CLEAR_FEATURE;

	iLineIndex = -1; // coded by kjNoh 160922
	selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

	bool bPaint = false;

	if (0 <= point.x - m_rcPic.left && point.x - m_rcPic.left < m_rcPic.Width() &&
		0 <= point.y - m_rcPic.top && point.y - m_rcPic.top < m_rcPic.Height())
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		assert(0 <= point.x - m_rcPic.left && point.x - m_rcPic.left < m_rcPic.Width() &&
			0 <= point.y - m_rcPic.top && point.y - m_rcPic.top < m_rcPic.Height());
		//m_Picture.SetFocus();

		//////////////////////////////////////
		//edited by kjNoh 160922
		//m_ptCur.x = point.x - m_rcPic.left;
		//m_ptCur.y = point.y - m_rcPic.top;
		m_ptCur.x = std::round((point.x - m_rcPic.left)*m_fRatio);
		m_ptCur.y = std::round((point.y - m_rcPic.top)*m_fRatio);
		///////////////////////////////////////

		cv::Point find_pt;
		//if (ptStart == cv::Point(-1, -1) && ptEnd == cv::Point(-1, -1))
		{

			////////////////////////////////////
			// annotated ny kjNoh 160922
			//for (int i = 0; i < SegmTree[iSliderPos].size(); i++)
			//{
			//	if (SegmTree[iSliderPos][i].end() != (std::find(SegmTree[iSliderPos][i].begin(), SegmTree[iSliderPos][i].end(), m_ptCur)))
			//	{
			//		iLineSelected_state = SELECT_LINE;
			//		iLineIndex = i;
			//		find_pt = m_ptCur;
			//	}
			//}
			////////////////////////////////////
			//////////////////////////////
			// coded by kjNoh 160922
			bool findNearFeat = false;
			if (mouseR_state)
				int afdf = 0;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			for (int y = -featSearchRagne; y <= featSearchRagne; y++)
			{
				for (int x = -featSearchRagne; x <= featSearchRagne; x++)
				{
					if (m_ptCur.y + y >= 0 && m_ptCur.y + y < m_height &&m_ptCur.x + x >= 0 && m_ptCur.x + x < m_width)
					if (FeatrueImg.at<int>(m_ptCur.y + y, m_ptCur.x + x))
					{
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						iFeatSelected_state = SELECT_FEATURE;
						selectedFeat = cv::Point(m_ptCur.x + x, m_ptCur.y + y);
						findNearFeat = true;
						break;
					}
				}
				if (findNearFeat)
					break;
			}
			//if (!findNearFeat)
			//{
			bool findNearSeg = false;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			for (int y = -2; y <= 2; y++)
			{
				for (int x = -2; x <= 2; x++)
				{
					if (m_ptCur.y + y >= 0 && m_ptCur.y + y < m_height &&m_ptCur.x + x >= 0 && m_ptCur.x + x < m_width)
						/*{
							for (int i = 0; i < SegLabelImg.cols * SegLabelImg.rows; i++)
							{
							if (SegLabelImg.at<int>(m_ptCur.y + y, m_ptCur.x + x) == )
							}
							}*/
					if (SegLabelImg.at<int>(m_ptCur.y + y, m_ptCur.x + x))
					{
						WriteLog(__FILE__, __LINE__, __FUNCTION__);
						iLineSelected_state = SELECT_LINE;
						iLineIndex = SegLabelImg.at<int>(m_ptCur.y + y, m_ptCur.x + x) - 1;
						find_pt = m_ptCur + cv::Point(x, y);

						findNearSeg = true;
						break;
					}
				}
				if (findNearSeg)
					break; WriteLog(__FILE__, __LINE__, __FUNCTION__);
			}
			//}
			//////////////////////////////


			//}
			//////////////////////////////

			////////////////////////////////////
			// annotated ny kjNoh 160922
			//if (m_ptCur.x < (find_pt.x - 5) || m_ptCur.y < (find_pt.y - 5) || (find_pt.x + 5) < m_ptCur.x || (find_pt.y + 5) < m_ptCur.y)
			//	iLineSelected_state = CLEAR_LINE;
			///////////////////////////////////

		}
		bPaint = true;
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	if (vecFname.size() == NULL)
		return;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	if (ptStart == cv::Point(-1, -1)) {
		//edited by kjNoh 160922
		//m_ptCur.x = point.x - m_rcPic.left;
		//m_ptCur.y = point.y - m_rcPic.top;
		m_ptCur.x = std::round((point.x - m_rcPic.left)*m_fRatio);
		m_ptCur.y = std::round((point.y - m_rcPic.top)*m_fRatio);
	}
	else if (ptStart != cv::Point(-1, -1))
	{
		//edited by kjNoh 160922
		//ptEnd.x = point.x - m_rcPic.left;
		//ptEnd.y = point.y - m_rcPic.top;
		ptEnd.x = std::round((point.x - m_rcPic.left)*m_fRatio);
		ptEnd.y = std::round((point.y - m_rcPic.top)*m_fRatio);

		//PointsOfLine();
	}

	//compute function
	if (m_ptCur.x >= 0 && m_ptCur.y >= 0 && m_ptCur.x < vesselImg.cols && m_ptCur.y < vesselImg.rows)
	{
		assert(0 <= point.x - m_rcPic.left && point.x - m_rcPic.left < m_rcPic.Width() &&
			0 <= point.y - m_rcPic.top && point.y - m_rcPic.top < m_rcPic.Height());
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		if (mouseL_state&&m_bManualEdit)
		{
			std::vector<cv::Point> pts;
			//cv::line(pts, m_prePt, m_ptCur, 255, 1);
			pts = getLine(m_prePt, m_ptCur);
			for (int i = 0; i < pts.size(); i++)
			{
				cur_path.push_back(pts[i]);
			}
			//cur_path.push_back(m_ptCur);
			m_prePt = m_ptCur;
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
		else if (ptStart != cv::Point(-1, -1) && ptStart != m_ptCur && !frangiDist.empty())
		{
			fmm.compute_discrete_geodesic(frangiDist, m_ptCur, &cur_path);
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
		}
	}

	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));

	/*if (0 <= center.x && center.x < m_rcPic.Width() && 0 <= center.y && center.y < m_rcPic.Height())
	{
	OnPaint();
	}*/

	if (m_bZoom)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		if (!m_zoomImg.empty()) {
			m_ptCur_Zoom.x = (point.x - m_rcZoom.left) * m_fZoomRatio;
			m_ptCur_Zoom.y = (point.y - m_rcZoom.top) * m_fZoomRatio;

			if (0 <= point.x - m_rcZoom.left && point.x - m_rcZoom.left < m_rcZoom.Width() && 0 <= point.y - m_rcZoom.top && point.y - m_rcZoom.top < m_rcZoom.Height())
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
				ptEnd = pt + m_leftTop;

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
	}
	if (bPaint)
		OnPaint(); 
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CLiveVesselDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_iMode_zoomWnd == 2)
		m_vecPtZoom.clear();
	else if (mouseL_state&&m_bManualEdit)
	{
		cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));
		
		if (center.x < 0 || center.y < 0 || center.x >= vesselImg.cols || center.y >= vesselImg.rows)
			return;

		if (iFeatSelected_state == SELECT_FEATURE)
			ptEnd = selectedFeat;
		else
			ptEnd = center;

		mouseL_state = false;

		if (ptEnd == ptStart)
		{
			cur_path.clear();
			ptStart = cv::Point(-1, -1);
			return;
		}

		std::vector<cv::Point> pts = getLine(m_prePt, ptEnd);
		for (int i = 0; i < pts.size(); i++)
		{
			cur_path.push_back(pts[i]);
		}
		
		if (cur_path.size() < 4)
		{
			cur_path.clear();
			ptStart = cv::Point(-1, -1);
			return;
		}

		CVesSegm vesSegm(cur_path);
		ComputeVesselRadii(vesSegm, FrangiScale, vesselImgG);
		MakeRegionMask_VesRadii(vesSegm, m_mask);
		//MakeRegionMask_NKJ(cur_path, FrangiScale, m_mask);
		SegmTree.addSegm(vesSegm);

		//////////////////////////
		//coded by kjNoh 160922 
		featureInfo cur_spepInfor;
		cur_spepInfor.sp = ptStart;
		cur_spepInfor.ep = ptEnd;
		cur_spepInfor.spType = 0;
		cur_spepInfor.epType = 0;

		featureTree.addFeat(cur_spepInfor);

		for (int i = 0; i < cur_path.size(); i++)
			SegLabelImg.at<int>(cur_path[i]) = SegmTree.nSegm;
		FeatrueImg.at<int>(cur_path[0]) = SegmTree.nSegm;
		FeatrueImg.at<int>(cur_path.back()) = SegmTree.nSegm;

		cur_path.clear();

		ptStart = cv::Point(-1, -1);
	}
	//m_ptZoom = ptEnd;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CLiveVesselDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	/*if (nChar == VK_SPACE)
	{
	ptStart = ptEnd = cv::Point(-1, -1);
	}*/

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CLiveVesselDlg::PointsOfLine()	//Bresenham algorithm
{
	int dx = abs(ptEnd.x - ptStart.x);
	int dy = abs(ptEnd.y - ptStart.y);

	if (dx >= dy)
	{
		int p = 2 * (dy - dx);
		int inc_x, inc_y;
		int y = ptStart.y;

		if (ptStart.x <= ptEnd.x) inc_x = 1;
		else inc_x = -1;

		if (ptStart.x <= ptEnd.y) inc_y = 1;
		else inc_y = -1;

		for (int x = ptStart.x; ptStart.x < ptEnd.x ? (x <= ptEnd.x) : (x >= ptEnd.x); x += inc_x) {
			if (p >= 0) {
				p += 2 * (dy - dx);
				y += inc_y;
			}
			else
				p += 2 * dy;

			vecPoints.push_back(cv::Point(x, y));
			printf("%d %d\n", x, y);
		}
		bivecPointsOfLine.push_back(vecPoints);
	}

	else
	{
		int p = 2 * (dx - dy);
		int inc_x, inc_y;
		int x = ptStart.x;

		if (ptStart.x < ptEnd.x) inc_x = 1;
		else inc_x = -1;

		if (ptStart.y < ptEnd.y) inc_y = 1;
		else inc_y = -1;

		for (int y = ptStart.y; ptStart.y < ptEnd.y ? (y <= ptEnd.y) : (y >= ptEnd.y); y += inc_y) {
			if (p >= 0) {
				p += 2 * (dx - dy);
				x += inc_x;
			}
			else
				p += 2 * dx;

			vecPoints.push_back(cv::Point(x, y));
			printf("%d %d\n", x, y);
		}
		bivecPointsOfLine.push_back(vecPoints);
	}
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


void CLiveVesselDlg::DrawPictureZoom(cv::Mat disp_zoom)
{
	CRect rcZoom;
	m_picZoom.GetClientRect(rcZoom);

	cv::Mat view_zoomImg;
	disp_zoom.copyTo(view_zoomImg);
	cv::resize(view_zoomImg, view_zoomImg, cv::Size(rcZoom.Width(), rcZoom.Height()));

	cv::flip(view_zoomImg, view_zoomImg, 0);
	int nX = view_zoomImg.cols;
	int nY = view_zoomImg.rows;

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
	float iZoom_width = m_rcZoom.Width();
	float iZoom_height = m_rcZoom.Height();

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
	std::vector<std::vector < cv::Point >> tp1_2d_ves_pts = vco.getVsegVpts2dArr();
	WriteLog(__FILE__, __LINE__, __FUNCTION__, "END OF VCO");

	// initialize vessel-segmentation tree from vco results
	SegmTree.newSetTree(tp1_2d_ves_pts);
	// load pre-computed data - required to compute region mask
	std::string pre_fn = ChangeFilenameExtension(vecFname[cur_frame + 1], "pre");
	LoadFrangi(pre_fn);
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
#endif
	// *** end saving results *** //

	iSliderPos = cur_frame + 1;
	iLineIndex = -1; // coded by kjNoh 160922
	selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922
	iLineSelected_state = CLEAR_LINE;
	iFeatSelected_state = CLEAR_FEATURE;

	updateSegm();
	dispUpdate(false);

	printf("VCO_frame done!!\n\n");
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
	//20161009 _daseul
	/*vscFile = fopen(m_pszPathName + aa + m_fileName[cur_frame+1] + ".vsc", "wb+");
	int nCurFrmaeSegm = (int)SegmTree.nSegm;
	fwrite(&nCurFrmaeSegm, sizeof(int), 1, vscFile);

	for (int j = 0; j < nCurFrmaeSegm; j++)
	{
	std::vector<cv::Point> tmp_Segm = SegmTree.get(j);
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
	fclose(vscFile);*/



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

	ptStart = cv::Point(-1, -1);
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
			ptStart = tmp_feat.ep;
			break;
		}

		if (tmp_feat.ep == selectedFeat && !tmp_feat.epType)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);
			dragIdx = i;
			dragSpEp = 1;
			ptStart = tmp_feat.sp;
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
	OpenEndedFMM();
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
		ptEnd = selectedFeat;
	else
		ptEnd = center;
	//PointsOfLine();

	fmm.compute_discrete_geodesic(frangiDist, ptEnd, &cur_path);
	std::reverse(cur_path.begin(), cur_path.end()); // coeded by kjNoh 160922

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	std::vector<cv::Point> vecPts;
	//vecPts.push_back(ptStart);
	for (int i = 0; i < cur_path.size(); i++)
		vecPts.push_back(cur_path[i]);
	//vecPts.push_back(ptEnd);

	// coeded by kjNoh 160922
	if (!dragSpEp)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);
		std::reverse(vecPts.begin(), vecPts.end());
		cv::Point tmp = ptStart;
		ptStart = ptEnd;
		ptEnd = tmp;
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	SegmTree.insert(dragIdx, vecPts);

	//////////////////////////
	//coded by kjNoh 160922 
	featureInfo cur_spepInfor;
	cur_spepInfor.sp = ptStart;
	cur_spepInfor.ep = ptEnd;
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

	ptStart = cv::Point(-1, -1);
	ptEnd = cv::Point(-1, -1);

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

	bivecPointsOfLine.clear();
	vecPoints.clear();

	vecSpEpLists_Zoom.clear();
	m_vecPtZoom.clear();

	//m_fileName.clear();
	vecFname.clear();

	//_CrtDumpMemoryLeaks();
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
