
// LiveVesselDlg.h : 헤더 파일
//

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../vco-master/vco_/FastMarching.h"
#include "../vco-master/vco_/FrangiFilter.h"
#include "../vco-master/vco_/VCOParams.h"

#include "../vco-master/vco_/VCO.h"

#include "SegmTree.h"
#include "Feature.h"

#ifdef _DEBUG
#pragma comment(lib,"vco64d.lib")
#else
#pragma comment(lib,"vco64.lib")
#endif

/////////////////////////
//coded by kjNoh 160922
//struct featureInfo
//{
//	cv::Point sp;
//	cv::Point ep;
//
//	// point type, 0= end(non junction point) and 1= junction
//	int spType;
//	int epType;
//};
///////////////////////
enum ThreadWorkingType
{
	THREAD_STOP = 0,
	THREAD_RUNNING = 1,
	TRHEAD_PAUSE = 2, 
};



// CLiveVesselDlg 대화 상자
class CLiveVesselDlg : public CDialogEx
{
// 생성입니다.
public:
	CLiveVesselDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIVEVESSEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//std::vector<cv::Mat> vesselImg;
	cv::Mat vesselImg;
	//cv::Mat vesselImg;
	cv::Mat m_zoomImg;
	cv::Mat m_disp_tmp;
	cv::Mat m_cropImg;
	cv::Mat m_cropTmp;

	CString strFilePath;

	cv::Point ptStart, ptEnd;
	cv::Point m_ptCur;
	cv::Point m_ptCenterZoomWnd;
	cv::Point m_ptCur_Zoom;
	cv::Point m_leftTop, m_rightBottom;
	cv::Point m_ptZoom;
	cv::Point m_ptZoom_tmp;

	std::vector<std::vector<cv::Point>> bivecPointsOfLine;
	std::vector<cv::Point> vecPoints;
	//std::vector<std::vector<cv::Point>> *SegmTree;
	//CSegmTree &SegmTree;
	CSegmTree SegmTree;
	std::vector<std::vector<cv::Point>> vecSpEpLists_Zoom;
	std::vector<cv::Point> m_vecPtZoom;

	// to perform fast-marching method, for computing geodesic between points
	cFastMarching fmm;
	// to perform frangi-filtering
	cFrangiFilter frfilt;
	//std::vector<cv::Mat> frangiImg;
	cv::Mat frangiImg;
	//cv::Mat frangiImg;
	cv::Mat frangiDist;
	
	cVCOParams pram;

	int iSliderPos;
	int m_iMouseState; // 0:init, 1:sp click down, 2:mouse move, 3:ep click down, 4:zoom mode
	int m_iszDrawCircle;
	int m_iMode_zoomWnd;		// 0:normal, 1:draw

	bool m_bLoad;
	bool m_bZoom;
	bool m_bMove;
	bool m_bPtSave;
	
	float m_fNull_width, m_fNull_height;
	float m_fRatio;
	float m_fZoomRatio;
	
	void DrawPicture(cv::Mat disp);
	void DrawPictureZoom(cv::Mat disp_zoom);

	afx_msg void OnBnClickedButtonImageLoad();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonLeftSlide();
	afx_msg void OnBnClickedButtonRightSlide();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButtonZoom();
	afx_msg void OnNMCustomdrawSliderZoomRatio(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioNormal();

	void PointsOfLine();
	void ZoomProcessing();

	CRect m_rcPic, m_rcZoom;
	CStatic m_Picture;
	CSliderCtrl m_ctrlSlider;
	CStatic m_picZoom;
	CEdit m_editZoomRatio;
	CSliderCtrl m_ctrlSliderZoomRatio;
	CString strEdit;
	CButton m_ctrlRadioNormal;
	CButton m_ctrlRadioDraw;
	afx_msg void OnBnClickedRadioDraw();
	afx_msg void OnBnClickedRadioMove();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	CButton m_ctrlRadioMove;

	//fast marching
	std::vector<cv::Point> cur_path;
	int iLineIndex;
	int iLineSelected_state;
	afx_msg void OnBnClickedButtonVcoFrame();
	afx_msg void OnBnClickedButtonFinish();

	//std::vector<CString> m_fileName;
	WCHAR m_pszPathName[2000];

	//int m_nLine;
	afx_msg void OnBnClickedButtonVcoSequence();

	///////////////////////
	//coded by kjNoh 160922
	//std::vector<featureInfo> vecSpEpJp;
	
	int featSearchRagne;
	cv::Point selectedFeat;
	int mouseR_state; // 0=non, 1 = click
	int dragIdx;
	bool dragSpEp; // if seleted drag point is start point, drag point is dragSpEp=0, another dragSpEp=1
	int m_width;
	int m_height;
	void updateSegm();
	void updateSegm(int n);

	featureInfo oldFeat;
	///////////////////////

	std::vector<std::string> vecFname;
	void dispUpdate(bool bPaint = true);

	int m_nFrame;

	bool m_bThreadStart;
	CWinThread *m_pThread;
	ThreadWorkingType m_eThreadWork;

	static UINT ThreadFunction(LPVOID _mothod);

	int iFeatSelected_state;
	cv::Mat FrangiScale;

	cv::Mat m_mask;

	CFeatureTree featureTree;

	cv::Mat FeatrueImg;
	cv::Mat SegLabelImg;



	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonThreadPause();

	//20161004 _daseul
	//cv::Mat m_frangiImg;
	//cv::Mat m_frangiScale;
	cv::Mat m_cur_mask;

	//time test
	float fFull_Time;
	time_t tStart, tEnd;
	bool m_bPostProc;
	bool m_bOverlay;
	afx_msg void OnBnClickedCheckPostProcessing();
	afx_msg void OnBnClickedCheckOverlay();
	afx_msg void OnBnClickedCheckManualEdit();
	CButton m_ButtManual;

	bool m_bManualEdit;

	int mouseL_state; // 0=non, 1 = click
	cv::Point m_prePt;
	std::vector<cv::Point> getLine(cv::Point sp, cv::Point ep);
	CStatic m_ctrl_FileName;	
	afx_msg void OnClose();
};
