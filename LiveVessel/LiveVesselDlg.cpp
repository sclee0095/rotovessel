
// LiveVesselDlg.cpp : 구현 파일
//





#include "stdafx.h"
#include "LiveVessel.h"
#include "LiveVesselDlg.h"
#include "afxdialogex.h"
#include "SegmTree.h"
#include "Feature.h"
//#include "vld.h"

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
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CLiveVesselDlg::OnBnClickedButtonLoad)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_ZOOM, &CLiveVesselDlg::OnBnClickedButtonZoom)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ZOOM_RATIO, &CLiveVesselDlg::OnNMCustomdrawSliderZoomRatio)
	ON_BN_CLICKED(IDC_RADIO_NORMAL, &CLiveVesselDlg::OnBnClickedRadioNormal)
	ON_BN_CLICKED(IDC_RADIO_DRAW, &CLiveVesselDlg::OnBnClickedRadioDraw)
	ON_BN_CLICKED(IDC_RADIO_MOVE, &CLiveVesselDlg::OnBnClickedRadioMove)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_BUTTON_VCO_FRAME, &CLiveVesselDlg::OnBnClickedButtonVcoFrame)
	ON_BN_CLICKED(IDC_BUTTON_FINISH, &CLiveVesselDlg::OnBnClickedButtonFinish)
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
END_MESSAGE_MAP()


// CLiveVesselDlg 메시지 처리기

BOOL CLiveVesselDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	
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
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLiveVesselDlg::OnPaint()
{
	
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
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
						std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
						for (int j = 0; j < tmp_Segm.size(); j++)
						{
							WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");

							int pt_x = tmp_Segm[j].x;
							int pt_y = tmp_Segm[j].y;

							//// added by kjNoh 161006
							//int R = 150;
							//int G = vesselImg.at<uchar>(pt_y, pt_x * 3 + 1);
							//int B = vesselImg.at<uchar>(pt_y, pt_x * 3 + 0);
							//cv::circle(disp, cv::Point(pt_x, pt_y), FrangiScale.at<double>(pt_y, pt_x), CV_RGB(R, G, B), -1);

							disp.at<uchar>(pt_y, pt_x * 3 + 0) = 255;
							disp.at<uchar>(pt_y, pt_x * 3 + 1) = 0;
							disp.at<uchar>(pt_y, pt_x * 3 + 2) = 0;
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
							std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
							for (int j = 0; j < tmp_Segm.size(); j++)
							{
								WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnPaint()");;

								int pt_x = tmp_Segm[j].x;
								int pt_y = tmp_Segm[j].y;

								//// added by kjNoh 161006
								//int R = 150;
								//int G = vesselImg.at<uchar>(pt_y, pt_x * 3 + 1);
								//int B = vesselImg.at<uchar>(pt_y, pt_x * 3 + 0);
								//cv::circle(disp, cv::Point(pt_x, pt_y), FrangiScale.at<double>(pt_y, pt_x), CV_RGB(R, G, B), -1);

								disp.at<uchar>(pt_y, pt_x * 3 + 0) = 255;
								disp.at<uchar>(pt_y, pt_x * 3 + 1) = 0;
								disp.at<uchar>(pt_y, pt_x * 3 + 2) = 0;
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
					std::vector<cv::Point> selectedSegm = SegmTree.get(iLineIndex);
					for (int i = 0; i < selectedSegm.size(); i++)
					{

						for (int y = -1; y <= 1; y++)
						for (int x = -1; x <= 1; x++)
						{
							int pt_x = selectedSegm[i].x;
							int pt_y = selectedSegm[i].y;
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

						std::vector<cv::Point> curSegm = SegmTree.get(iLineIndex);
						for (int i = 0; i < curSegm.size(); i++)
						{
							disp.at<uchar>(curSegm[i].y, curSegm[i].x * 3 + 0) = 255;
							disp.at<uchar>(curSegm[i].y, curSegm[i].x * 3 + 1) = 0;
							disp.at<uchar>(curSegm[i].y, curSegm[i].x * 3 + 2) = 0;
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

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLiveVesselDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLiveVesselDlg::OnBnClickedButtonImageLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//폴더 열기
	//vesselImg.clear();
	//SegmTree.clear();
	//SegmTree->clear();
	//vecPoints.clear();

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

		tStart = clock();
	}
	else
		return;
	// *** end: launch UI to get image sequence directory *** //
	
	// *** generate list of files of image sequence directory *** //
	CFileFind finder;
	CString str = L"\\*.bmp";
	CString  strFolderPath = m_pszPathName + str;
	//CString  strFolderPath = L"S1/" + str;
	BOOL bWorking = finder.FindFile(strFolderPath);
	FILE *f;

	ptStart = ptEnd = cv::Point(-1, -1);
	iSliderPos = 0;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		CString strA;
		strA = finder.GetFileName();

		if ((strA == L".") || (strA == L".."));
		else
		{
			strFilePath = finder.GetFilePath();
			std::string filePath = CT2CA(strFilePath.operator LPCWSTR());
			vecFname.push_back(filePath);
		}
	}
	// *** end: generate list of files of image sequence directory *** //


	// *** for all vessel sequence frames: (pre-process & save) or (load pre-processed data) *** //
	for (int i = 0; i < vecFname.size(); i++)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");

		vesselImg = cv::imread(vecFname[i]);
		std::string ffPath = vecFname[i];
		ffPath.pop_back(); ffPath.pop_back(); ffPath.pop_back();
		// try to load pre-saved pre-processing data
		ffPath += "pre";
		int check = _access(ffPath.data(), 0);
		// if successful, load saved data
		if (!check)
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");

			double *ffData = new double[vesselImg.rows*vesselImg.cols];
			double *ffSacleData = new double[vesselImg.rows*vesselImg.cols];
			f = fopen(ffPath.data(), "rb");
			fread(ffData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
			fread(ffSacleData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
			cv::Mat tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffData);
			tmp.copyTo(frangiImg);
			tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffSacleData);
			tmp.copyTo(FrangiScale);
			/*FrangiScale.convertTo(FrangiScale, CV_8UC1);
			cv::imshow("FrangiScale", FrangiScale);
			cv::waitKey();*/
			delete[] ffData;
			delete[] ffSacleData;
			fclose(f);
		}
		// if not, do pre-processing
		else
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");

			cv::Mat gray_vessel;
			cv::cvtColor(vesselImg, gray_vessel, CV_BGR2GRAY);
			frangiImg = frfilt.frangi(gray_vessel, &FrangiScale);
			frangiImg.convertTo(frangiImg, CV_64F);
			FrangiScale.convertTo(FrangiScale, CV_64F);

			f = fopen(ffPath.data(), "wb+");
			fwrite(((double*)frangiImg.data), sizeof(double), frangiImg.rows*frangiImg.cols, f);

			fwrite(((double*)FrangiScale.data), sizeof(double), FrangiScale.rows*FrangiScale.cols, f);
			fclose(f);
		}
	}
	// *** end: for all vessel sequence frames: (pre-process & save) or (load pre-processed data) *** //

	std::string segmTree = vecFname[0];
	segmTree.pop_back(); segmTree.pop_back(); segmTree.pop_back();
	segmTree += "vsc";
	int check = _access(segmTree.data(), 0);

	if (!check)
	{
		int nSegm;
		f = fopen(segmTree.data(), "rb");
		fread(&nSegm, sizeof(int), 1, f);

		for (int j = 0; j < nSegm; j++)
		{
			std::vector<cv::Point> curSegm;
			int nPts;
			fread(&nPts, sizeof(int), 1, f);

			for (int k = 0; k < nPts; k++)
			{
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");
				int x; int y;
				fread(&x, sizeof(int), 1, f);
				fread(&y, sizeof(int), 1, f);
				cv::Point pt;
				pt.x = x;
				pt.y = y;
				curSegm.push_back(pt);
			}

			if (!curSegm.size())
			{
				continue;
			}
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");
			SegmTree.addSegm(curSegm);

			int spType;
			int epType;
			fread(&spType, sizeof(int), 1, f);
			fread(&epType, sizeof(int), 1, f);

			featureInfo feat;
			feat.sp = curSegm[0];
			feat.spType = spType;
			feat.ep = curSegm.back();
			feat.epType = epType;

			featureTree.addFeat(feat);

		}
		fclose(f);
	}

	vesselImg = cv::imread(vecFname[0]);

	std::string ffPath = vecFname[0];
	ffPath.pop_back(); ffPath.pop_back(); ffPath.pop_back();
	ffPath += "pre";

	double *ffData = new double[vesselImg.rows*vesselImg.cols];
	double *ffSacleData = new double[vesselImg.rows*vesselImg.cols];
	f = fopen(ffPath.data(), "rb");
	fread(ffData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
	fread(ffSacleData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
	cv::Mat tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffData);
	tmp.copyTo(frangiImg);
	tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffSacleData);
	tmp.copyTo(FrangiScale);
	/*FrangiScale.convertTo(FrangiScale, CV_8UC1);
	cv::imshow("FrangiScale", FrangiScale);
	cv::waitKey();*/
	delete[] ffData;
	delete[] ffSacleData;
	fclose(f);
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");

	std::string maskPath = vecFname[0];
	maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back();
	maskPath += "_mask.png";
	m_mask = cv::imread(maskPath, CV_LOAD_IMAGE_GRAYSCALE);
	if (m_mask.empty())
	{
		m_mask = cv::Mat(vesselImg.size(), CV_8UC1);
		m_mask = 0;
	}

	m_bLoad = true;

	m_ctrlSlider.SetRange(0, vecFname.size() - 1);
	m_ctrlSlider.SetPos(0);

	m_fRatio = vesselImg.cols / (float)m_rcPic.Width();

	m_width = vesselImg.cols;
	m_height = vesselImg.rows;

	updateSegm();

	OnPaint();

	str.Empty();
	strFolderPath.Empty();

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonImageLoad()");

	//CString fname;
	//fname.Format("%s",);
	//
	//m_ctrl_FileName.SetWindowTextW(fname);
}


void CLiveVesselDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (vecFname.size() == NULL)
	{
		OnBnClickedButtonImageLoad();
		return;
	}

	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");

	if (0 <= center.x && center.x < m_width&& 0 <= center.y && center.y < m_height)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
		m_Picture.SetFocus();
		if (ptStart == cv::Point(-1, -1))
		{
			// edited by kjNoh 160922
			//ptStart = center;
			//ptEnd = center;
			if (iFeatSelected_state == SELECT_FEATURE)
			{
				ptStart = selectedFeat;
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
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

				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
			}
				
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
			ptEnd = center;

			//m_iMouseState = SP_CLICK_DOWN;
		}
		else
		{
			// edited by kjNoh 160922
			//ptEnd = center;
			if (iFeatSelected_state == SELECT_FEATURE)
			{
				ptEnd = selectedFeat;
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
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
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
			}
				

			//PointsOfLine();

			fmm.compute_discrete_geodesic(frangiDist, ptEnd, &cur_path);
			std::reverse(cur_path.begin(), cur_path.end()); // coeded by kjNoh 160922
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");

			std::vector<cv::Point> vecPts;
			//vecPts.push_back(ptStart);
			for (int i = 0; i < cur_path.size(); i++)
				vecPts.push_back(cur_path[i]);
			//vecPts.push_back(ptEnd);
			SegmTree.addSegm(vecPts);

			MakeRegionMask_NKJ(vesselImg,vecPts);
			/*
			cv::Mat convScale(1, vecPts.size(), CV_64FC1);
			for (int k = 0; k < vecPts.size(); k++)
			{
				int cur_x = vecPts[k].x;
				int cur_y = vecPts[k].y;
				double s = FrangiScale.at<double>(cur_y, cur_x);

				convScale.at<double>(0, k) = s;
			}

			cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

			for (int k = 0; k < vecPts.size(); k++)
			{
				int cur_x = vecPts[k].x;
				int cur_y = vecPts[k].y;
				//test.at<uchar>(cur_y,cur_x*3+0)
				//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);

				cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
			}
			convScale.release();
			*/


			//////////////////////////
			//coded by kjNoh 160922 
			featureInfo cur_spepInfor;
			cur_spepInfor.sp = ptStart;
			cur_spepInfor.ep = ptEnd;
			cur_spepInfor.spType = 0;
			cur_spepInfor.epType = 0;

			featureTree.addFeat(cur_spepInfor);

			for (int i = 0; i < vecPts.size(); i++)
				SegLabelImg.at<int>(vecPts[i]) = SegmTree.nSegm;

			FeatrueImg.at<int>(vecPts[0]) = SegmTree.nSegm;
			FeatrueImg.at<int>(vecPts.back()) = SegmTree.nSegm;
			////////////////////////


			ptStart = ptEnd;
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");

		}
		
		if (m_bManualEdit)
		{
			mouseL_state = true;
			cur_path.clear();
			m_prePt = ptStart;
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
		}
		else
		{

			double pfm_start_points[] = { ptStart.y, ptStart.x };
			double nb_iter_max = std::min(pram.pfm_nb_iter_max, (1.2*std::max(frangiImg.rows, frangiImg.cols)*std::max(frangiImg.rows, frangiImg.cols)));

			cv::Mat transPose_frangiImg;
			cv::transpose(frangiImg, transPose_frangiImg);

			transPose_frangiImg.setTo(1e-10, transPose_frangiImg < 1e-10);

			double *S;

			fmm.fast_marching(transPose_frangiImg, frangiImg.cols, frangiImg.rows, pfm_start_points, 1, 0, 0, nb_iter_max,
				&frangiDist, &S);
			frangiDist = frangiDist.t();
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");

		}
	}

	if (m_bZoom)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
		if (0 <= center.x && center.x < m_rcPic.Width() && 0 <= center.y && center.y < m_rcPic.Height())
		if (m_iMode_zoomWnd != 1)
			m_ptZoom = center;

		if (0 <= point.x - m_rcZoom.left && point.x - m_rcZoom.left < m_rcZoom.Width() && 0 <= point.y - m_rcZoom.top && point.y - m_rcZoom.top < m_rcZoom.Height())
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
			if (!m_bMove)
			{
				WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnLButtonDown()");
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

					std::vector<cv::Point> vecPts;
					vecPts.push_back(ptStart);
					vecPts.push_back(ptEnd);
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	iSliderPos -= 1;

	if (iSliderPos < 0)
		iSliderPos = 0;

	////vesselImg[iSliderPos].copyTo(vesselImg);
	//vesselImg = vesselImg[iSliderPos];
	//frangiImg = frangiImg[iSliderPos];
	//segLabelImg = vec_segLabelImg[iSliderPos];
	//featrueImage = vec_featrueImage[iSliderPos];
	////SegmTree = SegmTree[iSliderPos];

	//OnPaint();
	//m_ctrlSlider.SetPos(iSliderPos);

	dispUpdate();
	updateSegm();
}


void CLiveVesselDlg::OnBnClickedButtonRightSlide()
{
	iSliderPos += 1;

	if (iSliderPos > vecFname.size() - 1)
		iSliderPos = vecFname.size() - 1;

	////vesselImg[iSliderPos].copyTo(vesselImg);
	//vesselImg = vesselImg[iSliderPos];
	//frangiImg = frangiImg[iSliderPos];
	//segLabelImg = vec_segLabelImg[iSliderPos];
	//featrueImage = vec_featrueImage[iSliderPos];
	////SegmTree = SegmTree[iSliderPos];

	//OnPaint();
	//m_ctrlSlider.SetPos(iSliderPos);

	dispUpdate();
	updateSegm();
}


BOOL CLiveVesselDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.	

	if (!m_bLoad)
		return 0;

	bool bWorking = false;
	if (pMsg->message == WM_KEYDOWN)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::PreTranslateMessage()");
		if (!bWorking)
		{

			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::PreTranslateMessage()");
			bWorking = true;
			switch (pMsg->wParam)
			{
			case VK_LEFT:
				iSliderPos -= 1;

				if (iSliderPos < 0)
					iSliderPos = 0;

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
				if (m_bLoad != 0 && iLineIndex != -1)
				{
					//CSegmTree &curSegTree = SegmTree;
					SegmTree.rmSegm(iLineIndex);
					//SegmTree.rmSegm(iLineIndex);
					//(*SegmTree).erase((*SegmTree).begin() + iLineIndex);

					iLineSelected_state = CLEAR_LINE;

					//////////////////////////////////////////161007 daseul


					//cv::imshow("mask_before_edit", m_mask);
					//cv::waitKey();
					m_mask = 0;
					for (int i = 0; i < SegmTree.nSegm; i++)
					{
						std::vector<cv::Point> vecPts;
						vecPts = SegmTree.get(i);
						MakeRegionMask_NKJ(vesselImg,vecPts);
						/*
						cv::Mat convScale(1, vecPts.size(), CV_64FC1);
						for (int k = 0; k < vecPts.size(); k++)
						{
							int cur_x = vecPts[k].x;
							int cur_y = vecPts[k].y;
							double s = FrangiScale.at<double>(cur_y, cur_x);

							convScale.at<double>(0, k) = s;
						}

						cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

						for (int k = 0; k < vecPts.size(); k++)
						{
							int cur_x = vecPts[k].x;
							int cur_y = vecPts[k].y;

							cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
						}
						convScale.release();
						*/
					}
					//cv::imshow("mask_after_edit", m_mask);
					//cv::waitKey();
					////////////////////////////////////////


					//////////////////////////////////
					// coded by kjNoh 160922
					updateSegm();
					iLineIndex = -1;
					//dispUpdate();
					////////////////////////////////////

					OnPaint();
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");

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
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
			for (int y = -featSearchRagne; y <= featSearchRagne; y++)
			{
				for (int x = -featSearchRagne; x <= featSearchRagne; x++)
				{
					if (m_ptCur.y + y >= 0 && m_ptCur.y + y < m_height &&m_ptCur.x + x >= 0 && m_ptCur.x + x < m_width)
					if (FeatrueImg.at<int>(m_ptCur.y + y, m_ptCur.x + x))
					{
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
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
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
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
						WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
						iLineSelected_state = SELECT_LINE;
						iLineIndex = SegLabelImg.at<int>(m_ptCur.y + y, m_ptCur.x + x) - 1;
						find_pt = m_ptCur + cv::Point(x, y);

						findNearSeg = true;
						break;
					}
				}
				if (findNearSeg)
					break; WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
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

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
	if (vecFname.size() == NULL)
		return;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
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
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
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
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
		}
		else if (ptStart != cv::Point(-1, -1) && ptStart != m_ptCur && !frangiDist.empty())
		{
			fmm.compute_discrete_geodesic(frangiDist, m_ptCur, &cur_path);
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
		}
	}

	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));

	/*if (0 <= center.x && center.x < m_rcPic.Width() && 0 <= center.y && center.y < m_rcPic.Height())
	{
	OnPaint();
	}*/

	if (m_bZoom)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");

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
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");
	}
	if (bPaint)
		OnPaint(); 
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnMouseMove()");

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

		SegmTree.addSegm(cur_path);

		MakeRegionMask_NKJ(vesselImg,cur_path);
		/*
		cv::Mat convScale(1, cur_path.size(), CV_64FC1);
		for (int k = 0; k < cur_path.size(); k++)
		{
			int cur_x = cur_path[k].x;
			int cur_y = cur_path[k].y;
			double s = FrangiScale.at<double>(cur_y, cur_x);

			convScale.at<double>(0, k) = s;
		}

		cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

		for (int k = 0; k < cur_path.size(); k++)
		{
			int cur_x = cur_path[k].x;
			int cur_y = cur_path[k].y;
			//test.at<uchar>(cur_y,cur_x*3+0)
			//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);

			cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
		}
		convScale.release();
		*/

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

//임시 버튼
void CLiveVesselDlg::OnBnClickedButtonLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//vesselImg.clear();
	//vecPoints.clear();

	//SegmTree->clear();
	//ptStart = ptEnd = cv::Point(-1, -1);

	//CFileFind finder;
	//CString str = L"\\*.bmp";
	//CString strFolderPath;
	////strFolderPath = L"E:\\50_RotoVessel\\RotoVessel\\IMG\\voc_test" + str;
	//strFolderPath = L"S1/" + str;
	//BOOL bWorking = finder.FindFile(strFolderPath);
	//FILE *f;

	//while (bWorking)
	//{
	//	bWorking = finder.FindNextFile();

	//	CString strA;
	//	strA = finder.GetFileName();

	//	if ((strA == L".") || (strA == L".."));
	//	else
	//	{
	//		CString fileName = finder.GetFileTitle();
	//		m_fileName.push_back(fileName);

	//		strFilePath = finder.GetFilePath();

	//		std::string a = CT2CA(strFilePath.operator LPCWSTR());
	//		//vesselImg = cv::imread(a,CV_LOAD_IMAGE_GRAYSCALE);
	//		vesselImg = cv::imread(a);
	//		vesselImg.push_back(vesselImg);
	//		cv::Mat gray_vessel;
	//		cv::cvtColor(vesselImg, gray_vessel, CV_RGB2GRAY);

	//		std::string ffResPath = a;
	//		ffResPath.pop_back(); ffResPath.pop_back(); ffResPath.pop_back();
	//		ffResPath += "pre";
	//		
	//		BOOL bWorking2;
	//		int check = _access(ffResPath.data(), 0);

	//		cv::Mat ffres;
	//		if (!check)
	//		{
	//			double *ffData = new double[512 * 512];
	//			f = fopen(ffResPath.data(), "rb");
	//			fread(ffData, sizeof(double), 512 * 512, f);
	//			ffres = cv::Mat(512, 512, CV_64FC1, ffData);
	//			fclose(f);
	//		}
	//		else
	//		{
	//			ffres = frfilt.frangi(gray_vessel);
	//			ffres.convertTo(ffres, CV_64F);

	//			f = fopen(ffResPath.data(), "wb+");
	//			fwrite(((double*)ffres.data), sizeof(double), 512 * 512, f);
	//			fclose(f);
	//		}

	//		frangiImg.push_back(ffres);
	//	}
	//}
	////vesselImg[0].copyTo(vesselImg);
	//vesselImg = vesselImg[0];
	//frangiImg = frangiImg[0];

	//m_bLoad = true;
	//OnPaint();

	//m_ctrlSlider.SetRange(0, vesselImg.size() - 1);
	//m_ctrlSlider.SetPos(0);

	//m_fRatio = vesselImg.cols / (float)m_rcPic.Width();
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
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::ZoomProcessing()");
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMode_zoomWnd = 0;
	m_bMove = false;
}


void CLiveVesselDlg::OnBnClickedRadioDraw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMode_zoomWnd = 1;
	m_bMove = false;
}


void CLiveVesselDlg::OnBnClickedRadioMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMode_zoomWnd = 2;
	m_bMove = true;
}


BOOL CLiveVesselDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCursor(LoadCursor(NULL, (m_iMode_zoomWnd == 2) ? IDC_HAND : IDC_ARROW));

	return TRUE;

	//return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CLiveVesselDlg::OnBnClickedButtonVcoFrame()
{
	if (!m_bLoad)
		return;

	int nY = vesselImg.rows;
	int nX = vesselImg.cols;
	int cur_frame = iSliderPos;

	//cv::Mat d_tVesselImg;
	//cv::cvtColor(vesselImg, d_tVesselImg, CV_BGR2GRAY);
	//d_tVesselImg.convertTo(d_tVesselImg, CV_64FC1);
	//cv::Mat d_tp1VesselImg = cv::imread(vecFname[cur_frame+1],CV_LOAD_IMAGE_GRAYSCALE);
	//d_tp1VesselImg.convertTo(d_tp1VesselImg, CV_64FC1);

	cv::Mat d_tVc(nY, nX, CV_8UC1);
	d_tVc = 0;

	for (int i = 0; i < SegmTree.nSegm; i++)
	{
		std::vector<cv::Point> tmp_Seg = SegmTree.get(i);
		for (int j = 0; j < tmp_Seg.size(); j++)
		{
			d_tVc.at<uchar>(tmp_Seg[j]) = 255;
		}

	}

	CString aa = L"\\";

	FILE *vscFile;

	printf("start %dth frame\n", cur_frame);
	cv::Mat d_tVesselImg = cv::imread(vecFname[cur_frame], CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat d_tp1VesselImg = cv::imread(vecFname[cur_frame + 1], CV_LOAD_IMAGE_GRAYSCALE);

	std::string FScalePath = vecFname[cur_frame + 1];
	FScalePath.pop_back(); FScalePath.pop_back(); FScalePath.pop_back();
	FScalePath += "pre";

	FILE *f;
	f = fopen(FScalePath.data(), "rb+");
	double *tmp_V = new double[d_tp1VesselImg.rows*d_tp1VesselImg.cols];
	fread(tmp_V, sizeof(double), d_tp1VesselImg.rows*d_tp1VesselImg.cols, f);
	delete[] tmp_V;
	for (int y = 0; y < d_tp1VesselImg.rows; y++)
	for (int x = 0; x < d_tp1VesselImg.cols; x++)
	{
		double v;
		fread(&v, sizeof(double), 1, f);
		FrangiScale.at<double>(y, x) = v;
	}
	fclose(f);


	//cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, cur_frame + 1, nX, nY, false, "result/");
	bool bPostProc = true;
	// ***** EXCEPTION THROWN @2016.12.19-02:38:00 ***** //
	WriteLog(__FILE__, __LINE__, __FUNCTION__, "START OF VCO");
	char LOG[128];
	sprintf_s(LOG, "d_tVesselImg.(nrow, ncol):(%d, %d)", d_tVesselImg.rows, d_tVesselImg.cols);	WriteLog(LOG);
	sprintf_s(LOG, "d_tVc.(nrow, ncol):(%d, %d)", d_tVc.rows, d_tVc.cols);	WriteLog(LOG);
	sprintf_s(LOG, "d_tp1VesselImg.(nrow, ncol):(%d, %d)", d_tp1VesselImg.rows, d_tp1VesselImg.cols);	WriteLog(LOG);
	sprintf_s(LOG, "cur_frame, nX, nY: %d, %d, %d", cur_frame, nX, nY);	WriteLog(LOG);
	cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, cur_frame + 1, nX, nY, m_bPostProc, true, "../result/");
	vco.VesselCorrespondenceOptimization();
	WriteLog(__FILE__, __LINE__, __FUNCTION__, "END OF VCO");

	cv::Mat tp1_mask_8u = vco.get_tp1_adjusted_vescl_mask_pp();
	tp1_mask_8u.copyTo(d_tVc);


	std::vector<cv::Point> J, End;
	std::vector<std::vector < cv::Point >> tp1_2d_vec_vescl = vco.get_adjust_VsegVpts2dArr_pp(&J, &End);

	m_mask = 0;
	//cv::Mat gaussKernel = cv::getGaussianKernel(23, 4.4f);
	for (int j = 0; j < tp1_2d_vec_vescl.size(); j++)
	{
		MakeRegionMask_NKJ(vesselImg,tp1_2d_vec_vescl[j]);
		/*
		cv::Mat convScale(1, tp1_2d_vec_vescl[j].size(), CV_64FC1);
		for (int k = 0; k < tp1_2d_vec_vescl[j].size(); k++)
		{
			int cur_x = tp1_2d_vec_vescl[j][k].x;
			int cur_y = tp1_2d_vec_vescl[j][k].y;
			double s = FrangiScale.at<double>(cur_y, cur_x);

			convScale.at<double>(0, k) = s;
		}

		cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

		for (int k = 0; k < tp1_2d_vec_vescl[j].size(); k++)
		{
			int cur_x = tp1_2d_vec_vescl[j][k].x;
			int cur_y = tp1_2d_vec_vescl[j][k].y;
			//test.at<uchar>(cur_y,cur_x*3+0)
			//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);

			cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
		}
		convScale.release();
		*/
	}

	char maskSavePath[200];
	std::string cvtFname = vecFname[cur_frame + 1];
	cvtFname.pop_back(); cvtFname.pop_back(); cvtFname.pop_back(); cvtFname.pop_back();
	sprintf(maskSavePath, "%s_mask.png", cvtFname.data());
	cv::imwrite(maskSavePath, m_mask);

	cv::String str = cvtFname + "_vsc_mask" + ".png";
	//str = m_pszPathName + aa + m_fileName[cur_frame + 1] + "_vsc_mask" + ".png";
	cv::imwrite(str, tp1_mask_8u);

	//SegmTree[cur_frame + i+1].rmAll();
	//SegmTree[cur_frame + i+1].vecSegmTree = tp1_2d_vec_vescl;
	SegmTree.newSetTree(tp1_2d_vec_vescl);

	featureTree.rmAll();
	for (int j = 0; j < SegmTree.nSegm; j++)
	{

		std::vector<cv::Point> tmp_Segm = SegmTree.get(j);
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
		featureTree.addFeat(cur_feat);
		tmp_Segm.clear();
	}

	//vscFile = fopen(m_pszPathName + aa + m_fileName[cur_frame + 1] + ".vsc", "wb+");
	//161010_daseul
	//vecFname[cur_frame + 1].pop_back(); vecFname[cur_frame + 1].pop_back(); vecFname[cur_frame + 1].pop_back(); vecFname[cur_frame + 1].pop_back();
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

	iSliderPos = cur_frame + 1;

	iLineIndex = -1; // coded by kjNoh 160922
	selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

	iLineSelected_state = CLEAR_LINE;
	iFeatSelected_state = CLEAR_FEATURE;

	updateSegm();
	dispUpdate(false);

	tp1_2d_vec_vescl.clear();
	J.clear();
	str.clear();

	//double *d_tImgArr = ((double*)d_tVesselImg.data);
	//double *d_tp1ImgArr = ((double*)d_tp1VesselImg.data);
	//double *d_tVcArr = ((double*)d_tVc.data);

	//cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, cur_frame, nX, nY);
	//vco.VesselCorrespondenceOptimization();

	//std::vector<cv::Point> J, End;
	//cv::Mat tp1_mask_8u = vco.get_tp1_adjusted_vescl_mask_pp();
	//cv::Mat tp1_mask_8u = cv::Mat(512,512,CV_8UC1);
	//tp1_mask_8u = 0;

	//std::vector<std::vector < cv::Point >> tp1_2d_vec_vescl = vco.get_adjust_VsegVpts2dArr_pp(&J,&End);

	//cv::String str;
	//CString aa = L"\\";
	//str = m_pszPathName + aa + m_fileName[cur_frame + 1] +"_vsc_mask"+ ".png";
	//cv::imwrite(str, tp1_mask_8u);


	/*featureTree.rmAll();
	for (int j = 0; j < tp1_2d_vec_vescl.size(); j++)
	{

	std::vector<cv::Point> tmp_Segm = tp1_2d_vec_vescl[j];
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
	featureTree.addFeat(cur_feat);
	tmp_Segm.clear();
	}

	FILE *vscFile = fopen(m_pszPathName + aa + m_fileName[cur_frame + 1] + ".vsc", "wb+");
	int nCurFrmaeSegm = (int)tp1_2d_vec_vescl.size();
	fwrite(&nCurFrmaeSegm, sizeof(int), 1, vscFile);

	for (int j = 0; j < nCurFrmaeSegm; j++)
	{

	int nCurFrmaeSegmPt = (int)tp1_2d_vec_vescl[j].size();
	fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, vscFile);

	for (int k = 0; k < nCurFrmaeSegmPt; k++)
	{
	int x = tp1_2d_vec_vescl[j][k].x;
	int y = tp1_2d_vec_vescl[j][k].y;
	fwrite(&x, sizeof(int), 1, vscFile);
	fwrite(&y, sizeof(int), 1, vscFile);
	}
	featureInfo spepInfo = featureTree.get(j);
	fwrite(&spepInfo.spType, sizeof(int), 1, vscFile);
	fwrite(&spepInfo.epType, sizeof(int), 1, vscFile);
	}

	fclose(vscFile);

	iLineIndex = -1;

	iSliderPos = cur_frame + 1;

	updateSegm();
	dispUpdate();*/

	printf("VCO_frame done!!\n\n");

}


void CLiveVesselDlg::OnBnClickedButtonFinish()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (SegmTree.nSegm == 0)
		return;

	cv::Mat result_frame(m_height, m_width, CV_8UC1);
	result_frame = 0;

	for (int i = 0; i < SegmTree.nSegm; i++)
	{
		std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
		for (int j = 0; j < tmp_Segm.size(); j++)
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonFinish()");
			int x, y;
			x = tmp_Segm[j].x;
			y = tmp_Segm[j].y;

			result_frame.at<uchar>(y, x) = 255;
		}
	}

	/*cv::String str;
	CString aa = L"\\";
	CString root_path = L"S1";
	str = m_pszPathName + aa + m_fileName[iSliderPos] + ".png";*/
	int cur_frame = iSliderPos;
	std::string str = vecFname[cur_frame];
	str.pop_back(); str.pop_back(); str.pop_back(); str.pop_back();
	str += "_vsc_mask";
	str += ".png";
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonFinish()");
	cv::imwrite(str, result_frame);

	FILE *vscFile;
	str = vecFname[cur_frame];
	str.pop_back(); str.pop_back(); str.pop_back();
	str += "vsc";
	vscFile = fopen(str.data(), "wb");

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonFinish()");
	if (!vscFile)
		return;

	/*char buffer[1024];
	sprintf(buffer, "%d\r\n", (*SegmTree).size());
	fwrite(buffer, strlen(buffer), 1, vscFile);
	for (int i = 0; i < (*SegmTree).size(); i++)
	{
	char buffer1[1024];
	sprintf(buffer1, "%d %d ", i, (*SegmTree)[i].size());
	fwrite(buffer1, strlen(buffer1), 1, vscFile);
	for (int j = 0; j < (*SegmTree)[i].size(); j++)
	{
	char buffer2[1024];
	float x, y;
	x = (*SegmTree)[i][j].x;
	y = (*SegmTree)[i][j].y;

	sprintf(buffer2, "%f %f ", x, y);
	fwrite(buffer2, strlen(buffer2), 1, vscFile);
	}
	char buffer3[1024];
	sprintf(buffer3, "\r\n\r\n");
	fwrite(buffer3, strlen(buffer3), 1, vscFile);
	}
	fclose(vscFile);*/

	// aa = L"\\";

	//FILE *vscFile;
	//vscFile = fopen(m_pszPathName + aa + m_fileName[iSliderPos] + ".vsc", "wb+");
	int nCurFrmaeSegm = (int)SegmTree.nSegm;
	fwrite(&nCurFrmaeSegm, sizeof(int), 1, vscFile);

	for (int j = 0; j < SegmTree.nSegm; j++)
	{
		std::vector<cv::Point> tmp_Segm = SegmTree.get(j);
		int nCurFrmaeSegmPt = (int)tmp_Segm.size();
		fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, vscFile);
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonFinish()");

		for (int k = 0; k < nCurFrmaeSegmPt; k++)
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonFinish()");
			int x = tmp_Segm[k].x;
			int y = tmp_Segm[k].y;
			fwrite(&x, sizeof(int), 1, vscFile);
			fwrite(&y, sizeof(int), 1, vscFile);
		}

		//fwrite(0, sizeof(int), 1, vscFile);
		//fwrite(0, sizeof(int), 1, vscFile);
		featureInfo spepInfo = featureTree.get(j);
		fwrite(&spepInfo.spType, sizeof(int), 1, vscFile);
		fwrite(&spepInfo.epType, sizeof(int), 1, vscFile);
	}
	fclose(vscFile);

	std::string maskPath = vecFname[cur_frame];
	maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back();
	maskPath += "_mask.png";

	cv::imwrite(maskPath, m_mask);
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnBnClickedButtonFinish()");
	//FILE *fScale;
	//std::string ffPath = vecFname[cur_frame];
	//ffPath.pop_back(); ffPath.pop_back(); ffPath.pop_back();
	//ffPath += "pre";
	//int check = _access(ffPath.data(), 0);

	//if (!check)
	//{
	//	double *ffData = new double[vesselImg.rows*vesselImg.cols];
	//	double *ffSacleData = new double[vesselImg.rows*vesselImg.cols];
	//	fScale = fopen(ffPath.data(), "rb");
	//	fread(ffData, sizeof(double), vesselImg.rows*vesselImg.cols, fScale);
	//	fread(ffSacleData, sizeof(double), vesselImg.rows*vesselImg.cols, fScale);
	//	cv::Mat tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffSacleData);
	//	tmp.copyTo(FrangiScale);
	//	/*FrangiScale.convertTo(FrangiScale, CV_8UC1);
	//	cv::imshow("FrangiScale", FrangiScale);
	//	cv::waitKey();*/
	//	delete[] ffData;
	//	delete[] ffSacleData;
	//	fclose(fScale);
	//}
}


void CLiveVesselDlg::OnBnClickedButtonVcoSequence()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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

	cv::Mat d_tVc(nY, nX, CV_8UC1);
	d_tVc = 0;

	for (int i = 0; i < SegmTree.nSegm; i++)
	{
		std::vector<cv::Point> tmp_Segm = SegmTree.get(i);
		for (int j = 0; j < tmp_Segm.size(); j++)
		{
			d_tVc.at<uchar>(tmp_Segm[j]) = 255;
		}
	}

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
		std::vector<std::vector < cv::Point >> tp1_2d_vec_vescl = vco.get_adjust_VsegVpts2dArr_pp(&J, &End);

		m_mask = 0;
		//cv::Mat gaussKernel = cv::getGaussianKernel(23, 4.4f);
		for (int j = 0; j < tp1_2d_vec_vescl.size(); j++)
		{
			MakeRegionMask_NKJ(vesselImg,tp1_2d_vec_vescl[j]);
			/*
			cv::Mat convScale(1, tp1_2d_vec_vescl[j].size(), CV_64FC1);
			for (int k = 0; k < tp1_2d_vec_vescl[j].size(); k++)
			{
				int cur_x = tp1_2d_vec_vescl[j][k].x;
				int cur_y = tp1_2d_vec_vescl[j][k].y;
				double s = FrangiScale.at<double>(cur_y, cur_x);

				convScale.at<double>(0, k) = s;
			}

			cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

			for (int k = 0; k < tp1_2d_vec_vescl[j].size(); k++)
			{
				int cur_x = tp1_2d_vec_vescl[j][k].x;
				int cur_y = tp1_2d_vec_vescl[j][k].y;
				//test.at<uchar>(cur_y,cur_x*3+0)
				//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);

				cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
			}
			convScale.release();
			*/
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

		//SegmTree[cur_frame + i+1].rmAll();
		//SegmTree[cur_frame + i+1].vecSegmTree = tp1_2d_vec_vescl;
		SegmTree.newSetTree(tp1_2d_vec_vescl);

		featureTree.rmAll();
		for (int j = 0; j < SegmTree.nSegm; j++)
		{

			std::vector<cv::Point> tmp_Segm = SegmTree.get(j);
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
			featureTree.addFeat(cur_feat);
			tmp_Segm.clear();
		}

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (selectedFeat == cv::Point(-1, -1))
		return;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");

	iLineSelected_state = CLEAR_LINE;
	mouseR_state = 1;

	for (int i = 0; i < featureTree.getSize(); i++)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");

		featureInfo tmp_feat = featureTree.get(i);
		if (tmp_feat.sp == selectedFeat && !tmp_feat.spType)
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");

			dragIdx = i;
			dragSpEp = 0;
			ptStart = tmp_feat.ep;
			break;
		}

		if (tmp_feat.ep == selectedFeat && !tmp_feat.epType)
		{
			WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");

			dragIdx = i;
			dragSpEp = 1;
			ptStart = tmp_feat.sp;
			break;
		}
	}

	if (dragSpEp == -1 || dragIdx == -1)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");

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

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");

	double pfm_start_points[] = { ptStart.y, ptStart.x };
	double nb_iter_max = std::min(pram.pfm_nb_iter_max, (1.2*std::max(frangiImg.rows, frangiImg.cols)*std::max(frangiImg.rows, frangiImg.cols)));


	cv::Mat transPose_frangiImg;
	cv::transpose(frangiImg, transPose_frangiImg);

	transPose_frangiImg.setTo(1e-10, transPose_frangiImg < 1e-10);


	double *S;

	fmm.fast_marching(transPose_frangiImg, frangiImg.cols, frangiImg.rows, pfm_start_points, 1, 0, 0, nb_iter_max,
		&frangiDist, &S);


	frangiDist = frangiDist.t();

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonDown()");


	OnPaint();

	CDialogEx::OnRButtonDown(nFlags, point);
}
/////////////////////////////////////////

/////////////////////////////////////////
// coded by kjNoh 160922
void CLiveVesselDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (!mouseR_state)
		return;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");
	cv::Point center(std::round((point.x - m_rcPic.left) * m_fRatio), std::round((point.y - m_rcPic.top) * m_fRatio));

	mouseR_state = 0;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");
	// edited by kjNoh 160922
	if (iFeatSelected_state == SELECT_FEATURE)
		ptEnd = selectedFeat;
	else
		ptEnd = center;

	//PointsOfLine();

	fmm.compute_discrete_geodesic(frangiDist, ptEnd, &cur_path);

	std::reverse(cur_path.begin(), cur_path.end()); // coeded by kjNoh 160922

	std::vector<cv::Point> vecPts;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");

	//vecPts.push_back(ptStart);
	for (int i = 0; i < cur_path.size(); i++)
		vecPts.push_back(cur_path[i]);
	//vecPts.push_back(ptEnd);

	// coeded by kjNoh 160922
	if (!dragSpEp)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");

		std::reverse(vecPts.begin(), vecPts.end());
		cv::Point tmp = ptStart;
		ptStart = ptEnd;
		ptEnd = tmp;
	}

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");


	//SegmTree->insert(SegmTree->begin() + dragIdx, vecPts);
	//(*SegmTree).insert((*SegmTree).begin() + dragIdx, vecPts);
	SegmTree.insert(dragIdx, vecPts);


	//////////////////////////
	//coded by kjNoh 160922 
	featureInfo cur_spepInfor;
	cur_spepInfor.sp = ptStart;
	cur_spepInfor.ep = ptEnd;
	if (!dragSpEp)
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");

		cur_spepInfor.spType = 0;
		cur_spepInfor.epType = oldFeat.epType;
	}
	else
	{
		WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");

		cur_spepInfor.spType = oldFeat.spType;
		cur_spepInfor.epType = 0;
	}
	featureTree.insert(dragIdx, cur_spepInfor);
	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");


	for (int i = 0; i < vecPts.size(); i++)
		SegLabelImg.at<int>(vecPts[i]) = dragIdx + 1;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");


	FeatrueImg.at<int>(vecPts[0]) = dragIdx + 1;
	FeatrueImg.at<int>(vecPts.back()) = dragIdx + 1;

	WriteLog(__FILE__, __LINE__, "CLiveVesselDlg::OnRButtonUp()");


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

		std::vector<cv::Point> vecPts;
		vecPts = SegmTree.get(i);

		cv::Mat convScale(1, vecPts.size(), CV_64FC1);
		for (int k = 0; k < vecPts.size(); k++)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);

			int cur_x = vecPts[k].x;
			int cur_y = vecPts[k].y;
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
		convScale.release();
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

			std::vector<cv::Point> segm = SegmTree.get(i);
			for (int j = 0; j < segm.size(); j++)
				SegLabelImg.at<int>(segm[j]) = i + 1;

			FeatrueImg.at<int>(segm.front()) = i + 1;
			FeatrueImg.at<int>(segm.back()) = i + 1;
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

void CLiveVesselDlg::updateSegm(int n)
{


}


void CLiveVesselDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	CloseLogStream();

	bivecPointsOfLine.clear();
	vecPoints.clear();

	vecSpEpLists_Zoom.clear();
	m_vecPtZoom.clear();

	cv::Mat* vecMask;
	//m_fileName.clear();
	vecFname.clear();


	//_CrtDumpMemoryLeaks();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CLiveVesselDlg::dispUpdate(bool bPaint)
{
	iLineSelected_state = CLEAR_LINE;
	iLineIndex = -1;

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	iFeatSelected_state = CLEAR_FEATURE ;
	selectedFeat = cv::Point(-1, -1);

	std::string vessPath = vecFname[iSliderPos];
	std::string frangiPath = vecFname[iSliderPos];
	frangiPath.pop_back(); frangiPath.pop_back(); frangiPath.pop_back();
	frangiPath += "pre";
	vesselImg = cv::imread(vessPath);
	SegmTree.rmAll();
	featureTree.rmAll();

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	FILE* f;

	int check = _access(frangiPath.data(), 0);

	if (!check)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		double *ffData = new double[vesselImg.rows*vesselImg.cols];
		double *ffSacleData = new double[vesselImg.rows*vesselImg.cols];
		f = fopen(frangiPath.data(), "rb");
		fread(ffData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
		fread(ffSacleData, sizeof(double), vesselImg.rows*vesselImg.cols, f);
		cv::Mat tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffData);
		tmp.copyTo(frangiImg);
		tmp = cv::Mat(vesselImg.size(), CV_64FC1, ffSacleData);
		tmp.copyTo(FrangiScale);
		delete[] ffData;
		delete[] ffSacleData;
		fclose(f);
	}
	else
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		cv::Mat gray_vessel;
		cv::cvtColor(vesselImg, gray_vessel, CV_BGR2GRAY);
		frangiImg = frfilt.frangi(gray_vessel, &FrangiScale);
		frangiImg.convertTo(frangiImg, CV_64F);
		FrangiScale.convertTo(FrangiScale, CV_64F);

		f = fopen(frangiPath.data(), "wb+");
		fwrite(((double*)frangiImg.data), sizeof(double), frangiImg.rows*frangiImg.cols, f);

		fwrite(((double*)FrangiScale.data), sizeof(double), FrangiScale.rows*FrangiScale.cols, f);
		fclose(f);
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	std::string segmTree = vecFname[iSliderPos];
	segmTree.pop_back(); segmTree.pop_back(); segmTree.pop_back();
	segmTree += "vsc";
	check = _access(segmTree.data(), 0);


	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	if (!check)
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		int nSegm;
		f = fopen(segmTree.data(), "rb");
		fread(&nSegm, sizeof(int), 1, f);

		for (int j = 0; j < nSegm; j++)
		{
			WriteLog(__FILE__, __LINE__, __FUNCTION__);

			std::vector<cv::Point> curSegm;
			int nPts;
			fread(&nPts, sizeof(int), 1, f);

			for (int k = 0; k < nPts; k++)
			{
				int x; int y;
				fread(&x, sizeof(int), 1, f);
				fread(&y, sizeof(int), 1, f);
				cv::Point pt;
				pt.x = x;
				pt.y = y;
				curSegm.push_back(pt);
			}

			if (!curSegm.size())
			{
				continue;
			}

			SegmTree.addSegm(curSegm);

			int spType;
			int epType;
			fread(&spType, sizeof(int), 1, f);
			fread(&epType, sizeof(int), 1, f);

			featureInfo feat;
			feat.sp = curSegm[0];
			feat.spType = spType;
			feat.ep = curSegm.back();
			feat.epType = epType;

			featureTree.addFeat(feat);

		}

		fclose(f);

		WriteLog(__FILE__, __LINE__, __FUNCTION__);
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	std::string maskPath = vecFname[iSliderPos];
	maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back(); maskPath.pop_back();
	maskPath += "_mask.png";
	m_mask = cv::imread(maskPath, CV_LOAD_IMAGE_GRAYSCALE);
	if (m_mask.empty())
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		m_mask = cv::Mat(vesselImg.size(), CV_8UC1);
		m_mask = 0;
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
		std::vector<cv::Point> tmp_Segm = pDlg->SegmTree.get(i);
		for (int j = 0; j < tmp_Segm.size(); j++)
		{
			d_tVc.at<uchar>(tmp_Segm[j]) = 255;
		}
	}

	CString aa = L"\\";

	//161010_daseul
	std::string cvtFname = pDlg->vecFname[cur_frame];
	cvtFname.pop_back(); cvtFname.pop_back(); cvtFname.pop_back(); cvtFname.pop_back();
	std::string str = cvtFname + ".vsc";
	const char*path = str.c_str();

	FILE *vscFile;
	//vscFile = fopen(pDlg->m_pszPathName + aa + pDlg->m_fileName[cur_frame] + ".vsc", "wb+");
	vscFile = fopen(path, "wb+");
	int nCurFrmaeSegm = (int)pDlg->SegmTree.nSegm;
	fwrite(&nCurFrmaeSegm, sizeof(int), 1, vscFile);

	for (int j = 0; j < nCurFrmaeSegm; j++)
	{
		std::vector<cv::Point> tmp_Segm = pDlg->SegmTree.get(j);
		int nCurFrmaeSegmPt = (int)tmp_Segm.size();
		fwrite(&nCurFrmaeSegmPt, sizeof(int), 1, vscFile);

		for (int k = 0; k < nCurFrmaeSegmPt; k++)
		{
			int x = tmp_Segm[k].x;
			int y = tmp_Segm[k].y;
			fwrite(&x, sizeof(int), 1, vscFile);
			fwrite(&y, sizeof(int), 1, vscFile);
		}

		featureInfo spepInfo = pDlg->featureTree.get(j);
		fwrite(&spepInfo.spType, sizeof(int), 1, vscFile);
		fwrite(&spepInfo.epType, sizeof(int), 1, vscFile);

		tmp_Segm.clear();
	}
	fclose(vscFile);


	for (int i = cur_frame; i < end_frame; i++)
	{
		printf("start %dth frame\n", i);
		cv::Mat d_tVesselImg = cv::imread(pDlg->vecFname[i], CV_LOAD_IMAGE_GRAYSCALE);

		cv::Mat d_tp1VesselImg = cv::imread(pDlg->vecFname[i + 1], CV_LOAD_IMAGE_GRAYSCALE);;

		cVCO vco(d_tVesselImg, d_tVc, d_tp1VesselImg, i + 1, nX, nY, false, "result/");
		vco.VesselCorrespondenceOptimization();

		cv::Mat tp1_mask_8u = vco.get_tp1_adjusted_vescl_mask_pp();
		tp1_mask_8u.copyTo(d_tVc);


		std::vector<cv::Point> J, End;
		std::vector<std::vector < cv::Point >> tp1_2d_vec_vescl = 
			vco.get_adjust_VsegVpts2dArr_pp(&J, &End);


		//cv::Mat gaussKernel = cv::getGaussianKernel(23, 4.4f);
		for (int j = 0; j < tp1_2d_vec_vescl.size(); j++)
		{
			pDlg->MakeRegionMask_NKJ(pDlg->vesselImg,tp1_2d_vec_vescl[j]);
			/*
			cv::Mat convScale(1, tp1_2d_vec_vescl[j].size(), CV_64FC1);
			for (int k = 0; k < tp1_2d_vec_vescl[j].size(); k++)
			{
				int cur_x = tp1_2d_vec_vescl[j][k].x;
				int cur_y = tp1_2d_vec_vescl[j][k].y;
				double s = pDlg->FrangiScale.at<double>(cur_y, cur_x);

				convScale.at<double>(0, k) = s;
			}

			cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

			for (int k = 0; k < tp1_2d_vec_vescl[j].size(); k++)
			{
				int cur_x = tp1_2d_vec_vescl[j][k].x;
				int cur_y = tp1_2d_vec_vescl[j][k].y;
				//test.at<uchar>(cur_y,cur_x*3+0)
				//double s = pDlg->FrangiScale[i + 1].at<double>(cur_y, cur_x);

				cv::circle(pDlg->m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
			}
			convScale.release();
			*/
		}


		cv::String str;
		std::string cvtFname2 = pDlg->vecFname[cur_frame + i + 1];
		cvtFname2.pop_back(); cvtFname2.pop_back(); cvtFname2.pop_back(); cvtFname2.pop_back();
		str = cvtFname2 + "_vsc_mask" + ".png";
		cv::imwrite(str, tp1_mask_8u);

		//SegmTree[cur_frame + i+1].rmAll();
		//SegmTree[cur_frame + i+1].vecSegmTree = tp1_2d_vec_vescl;
		pDlg->SegmTree.newSetTree(tp1_2d_vec_vescl);

		pDlg->featureTree.rmAll();
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
		}

		//161010_daseul
		//vscFile = fopen(pDlg->m_pszPathName + aa + pDlg->m_fileName[cur_frame + i + 1] + ".vsc", "wb+");
		str = pDlg->vecFname[cur_frame + i + 1] + ".vsc";
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
			featureInfo spepInfo = pDlg->featureTree.get(j);
			fwrite(&spepInfo.spType, sizeof(int), 1, vscFile);
			fwrite(&spepInfo.epType, sizeof(int), 1, vscFile);
			tmp_Segm.clear();
		}

		fclose(vscFile);

		pDlg->iSliderPos = i + 1;

		pDlg->iLineIndex = -1; // coded by kjNoh 160922
		pDlg->selectedFeat = cv::Point(-1, -1);// coded by kjNoh 160922

		pDlg->iLineSelected_state = CLEAR_LINE;
		pDlg->iFeatSelected_state = CLEAR_FEATURE;

		pDlg->updateSegm();
		pDlg->dispUpdate(false);

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iState = IsDlgButtonChecked(IDC_CHECK_POST_PROCESSING);

	if (iState)
		m_bPostProc = true;
	else
		m_bPostProc = false;
}


void CLiveVesselDlg::OnBnClickedCheckOverlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iState = IsDlgButtonChecked(IDC_CHECK_OVERLAY);

	if (iState)
		m_bOverlay = true;
	else
		m_bOverlay = false;

	OnPaint();
}


void CLiveVesselDlg::OnBnClickedCheckManualEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CloseLogStream();

	CDialogEx::OnClose();
}



void CLiveVesselDlg::MakeRegionMask_NKJ(cv::Mat img, std::vector<cv::Point> &vecPts)
{
	////for 모든(또는 현재 vessel segment의) vessel point
	////	kernel_response(window_size * kernel 스케일 3d array) 초기화
	////  for (x, y) = 사이즈[w h]의 local window 내 점들마다
	////		for s = 모든 kernel 스케일
	////			kernel_response(x, y, s) = kernel 컨벌루션 출력
	////		end for
	////	end for
	////	최대의 response 값의 x, y, s 값 적용
	////end for

	//// convert image scale
	//cv::Mat grayscale_img;
	//if (img.channels() == 3)
	//	cv::cvtColor(img, grayscale_img, CV_BGR2GRAY);
	//else
	//	img.copyTo(grayscale_img);

	//// set sigma
	//double start_kernel_sigma = 1;
	//double end_kernel_sigma = 7;
	//double sigma_inetval = 1.0f;
	//
	//// set searching window size, kernal size, rotation dgree
	//int window_size = 3;
	//int kernel_size = 40;
	//int rotation_degree = 10;

	//// make kernels, signmas and stored
	//std::vector<std::vector<cv::Mat>> vec_2D_kernels;
	//std::vector<double> vec_sigma;
	//int selected_kernel = 2; // 0 = 2d line kenel, 1 = 2d circle kernel, 2 = non-gauss kernel
	//for (double i = start_kernel_sigma; i <= end_kernel_sigma; i += sigma_inetval)
	//{
	//	vec_sigma.push_back(i);


	//	std::vector<cv::Mat> vec_kernels;
	//	if (selected_kernel == 0)
	//	{
	//		cv::Mat gauss1 = cv::getGaussianKernel(kernel_size*sqrt(2), i);
	//		cv::Mat gauss2 = cv::getGaussianKernel(kernel_size*sqrt(2), i * 2);
	//		cv::Mat DoG = gauss2 - gauss1;

	//		cv::Mat kernel_2D = cv::repeat(DoG, 1, kernel_size*sqrt(2));
	//		for (int j = 0; j < 180; j += rotation_degree)
	//		{
	//			cv::Mat rotation_matrix = cv::getRotationMatrix2D(cv::Point2f(kernel_size*sqrt(2) / 2, kernel_size*sqrt(2) / 2), j, 1);

	//			cv::Mat rotation_kernel;
	//			cv::warpAffine(kernel_2D, rotation_kernel, rotation_matrix, cv::Size(kernel_size*sqrt(2), kernel_size*sqrt(2)));

	//			cv::Mat crop_rotation_kernel = rotation_kernel(
	//				cv::Rect(
	//				kernel_size*sqrt(2) / 2 - kernel_size / 2,
	//				kernel_size*sqrt(2) / 2 - kernel_size / 2,
	//				kernel_size,
	//				kernel_size)).clone();

	//			//// check rotation kernel
	//			//double minv, maxv;
	//			//cv::minMaxIdx(rotation_kernel, &minv, &maxv);
	//			//cv::Mat check_rotation = (rotation_kernel - minv) / (maxv - minv) * 255;
	//			//check_rotation.convertTo(check_rotation, CV_8UC1);
	//			//cv::imshow("check_rotation", check_rotation);
	//			//cv::waitKey();
	//			crop_rotation_kernel.convertTo(crop_rotation_kernel, CV_64FC1);
	//			vec_kernels.push_back(crop_rotation_kernel);
	//		}
	//	}
	//	else if (selected_kernel == 1)
	//	{
	//		cv::Mat gauss1 = cv::getGaussianKernel(kernel_size, i);
	//		cv::Mat gauss2 = cv::getGaussianKernel(kernel_size, i * 2);
	//		//cv::Mat DoG = gauss2 - gauss1;

	//		cv::Mat gauss1_X = cv::repeat(gauss1, 1, kernel_size);
	//		cv::Mat gauss1_Y = gauss1_X.t();
	//		cv::Mat gauss2_X = cv::repeat(gauss2, 1, kernel_size);
	//		cv::Mat gauss2_Y = gauss2_X.t();

	//		//cv::Mat kernel_2D_X = cv::repeat(DoG, 1, kernel_size);
	//		//cv::Mat kernel_2D_Y = kernel_2D_X.t();

	//		//cv::Mat kernel_2D =  kernel_2D_X*kernel_2D_Y;
	//		cv::Mat kernel_2D(kernel_size, kernel_size,CV_64FC1);
	//		kernel_2D = 0;

	//		for (int y = 0; y < kernel_2D.rows; y++)
	//		for (int x = 0; x < kernel_2D.cols; x++)
	//		{
	//			kernel_2D.at<double>(y, x) = 
	//				gauss2_X.at<double>(y, x)*gauss2_Y.at<double>(y, x)
	//				-gauss1_X.at<double>(y, x)*gauss1_Y.at<double>(y, x);
	//		}

	//		//// check circle kernel
	//		//double minv, maxv;
	//		//cv::minMaxIdx(kernel_2D, &minv, &maxv);
	//		//cv::Mat check_circle = (kernel_2D - minv) / (maxv - minv) * 255;
	//		//check_circle.convertTo(check_circle, CV_8UC1);
	//		//cv::imshow("check_circle", check_circle);
	//		//cv::waitKey();
	//		kernel_2D.convertTo(kernel_2D, CV_64FC1);
	//		vec_kernels.push_back(kernel_2D);
	//	}
	//	else if (selected_kernel == 2)
	//	{
	//		cv::Mat kernel_2D(kernel_size,kernel_size,CV_64FC1);
	//		kernel_2D = -1;
	//		cv::circle(kernel_2D, cv::Point(kernel_size / 2, kernel_size / 2), vec_sigma.back() * 2, 1, -1);
	//		
	//		kernel_2D.convertTo(kernel_2D, CV_64FC1);
	//		//kernel_2D /= 255;
	//		vec_kernels.push_back(kernel_2D);
	//	}

	//	//// for check the kernel
	//	//for (int j = 0; j < DoG.rows; j++)
	//	//{
	//	//	printf("%lf\n", DoG.at<double>(j, 0));
	//	//}
	//	//cv::Mat check_kernel= cv::repeat(DoG, 1, 100);
	//	//double minv, maxv;
	//	//cv::minMaxIdx(check_kernel, &minv, &maxv);
	//	//check_kernel = (check_kernel - minv) / (maxv - minv) * 255;
	//	//check_kernel.convertTo(check_kernel, CV_8UC1);
	//	//cv::imshow("check_kernel",check_kernel);
	//	//cv::waitKey();
	//	
	//	vec_2D_kernels.push_back(vec_kernels);
	//}

	//// init kernel response sp
	//int nKernels = (int)vec_2D_kernels.size();
	//int nPts = (int)vecPts.size();
	//cv::Mat maximun_response_sigmas = cv::Mat::zeros(1, nPts, CV_64FC1);
	//cv::Mat maximun_response_pts = cv::Mat::zeros(1, nPts, CV_32SC2);
	//cv::Mat region_mask(m_height, m_width, CV_8UC1);
	//region_mask = 0;
	//for (int i = 0; i < nPts; i++)
	//{
	//	double max_response = 0;
	//	int max_response_sigma_idx = -1;
	//	int max_response_angle_idx = -1;
	//	cv::Point max_response_pt(-1,-1);

	//	for (int sy = -window_size / 2; sy < window_size / 2; sy++)
	//	for (int sx = -window_size / 2; sx < window_size / 2; sx++)
	//	{
	//		cv::Mat crop = grayscale_img(
	//			cv::Rect(vecPts[i].x + sx - kernel_size / 2,
	//			vecPts[i].y + sy - kernel_size / 2,
	//			kernel_size,
	//			kernel_size)).clone();
	//		crop.convertTo(crop, CV_64FC1);
	//		
	//		for (int j = 0; j < nKernels; j++)
	//		{
	//			for (int k = 0; k < (int)vec_2D_kernels[j].size(); k++)
	//			{
	//				cv::Mat kernel = vec_2D_kernels[j][k].clone();
	//				cv::Mat mul_mat = crop.mul(kernel);
	//				cv::Scalar val = cv::sum(mul_mat);
	//				
	//				if (val[0]>max_response)
	//				{
	//					max_response = val[0];
	//					max_response_sigma_idx = j;
	//					max_response_angle_idx = k;
	//					max_response_pt = cv::Point(vecPts[i] + cv::Point(sx, sy));
	//				}
	//			}
	//		}
	//	}
	//	cv::circle(region_mask, max_response_pt, vec_sigma[max_response_sigma_idx]*2, cv::Scalar(255),-1);
	//	maximun_response_sigmas.at<double>(0, i) = vec_sigma[max_response_sigma_idx] * 2;
	//	maximun_response_pts.at<cv::Point>(0, i) = max_response_pt;
	//}

	////// non-smoothing region mask visualization
	////cv::Mat draw_region = img.clone();
	////
	////for (int y = 0; y < m_height; y++)
	////for (int x = 0; x < m_width; x++)
	////{
	////	if (region_mask.at<uchar>(y, x))
	////	{
	////		draw_region.at<uchar>(y, x * 3 + 0) = 255;
	////		//draw_region.at<uchar>(y, x * 3 + 0) = 0;
	////		//draw_region.at<uchar>(y, x * 3 + 0) = 255;
	////	}
	////}
	////cv::imshow("region_mask", region_mask);
	////cv::imshow("draw_region", draw_region);
	////cv::waitKey();

	//bool bSmoothing = false;

	//// smoothing and drawing region mask
	//cv::Mat smoothing_response_sigmas;
	//if (bSmoothing)
	//	cv::GaussianBlur(maximun_response_sigmas, smoothing_response_sigmas, cv::Size(23, 1), 4.4f);
	//else
	//	smoothing_response_sigmas = maximun_response_sigmas.clone();

	//for (int i = 0; i < maximun_response_pts.cols; i++)
	//{
	//	cv::circle(m_mask, maximun_response_pts.at<cv::Point>(0, i), smoothing_response_sigmas.at<double>(0, i), 255, -1);
	//}

	// original smoothing before using convolutional max response scale
	cv::Mat convScale(1, vecPts.size(), CV_64FC1);
	for (int k = 0; k < vecPts.size(); k++)
	{
		int cur_x = vecPts[k].x;
		int cur_y = vecPts[k].y;
		double s = FrangiScale.at<double>(cur_y, cur_x);

		convScale.at<double>(0, k) = s;
	}

	cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

	for (int k = 0; k < vecPts.size(); k++)
	{
		int cur_x = vecPts[k].x;
		int cur_y = vecPts[k].y;
		//test.at<uchar>(cur_y,cur_x*3+0)
		//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);

		cv::circle(m_mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), 255, -1);
	}
	convScale.release();
}