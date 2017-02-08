
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

#include "../vco-master/vco_/SegmTree.h"
#include "../vco-master/vco_/Feature.h"

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


std::string ChangeFilenameExtension(std::string frm_path, std::string ext);
std::string GetFileExtension(std::string path);
std::string RemoveAndAppendFilepath(std::string path, std::string app);


class CLiveVesselDlg : public CDialogEx
{

public:
	CLiveVesselDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_LIVEVESSEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonImageLoad();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonLeftSlide();
	afx_msg void OnBnClickedButtonRightSlide();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButtonZoom();
	afx_msg void OnNMCustomdrawSliderZoomRatio(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioNormal();
	afx_msg void OnBnClickedButtonVcoFrame();
	afx_msg void FinishAndSave2File();
	afx_msg void OnBnClickedButtonVcoSequence();
	afx_msg void OnBnClickedRadioDraw();
	afx_msg void OnBnClickedRadioMove();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonThreadPause();
	afx_msg void OnBnClickedCheckPostProcessing();
	afx_msg void OnBnClickedCheckOverlay();
	afx_msg void OnBnClickedCheckManualEdit();
	afx_msg void OnClose();
	// 2016.12.27 SCLEE - FOR XELIS INTEGRATION
	afx_msg void OnBnClickedButtonConvertData();
	DECLARE_MESSAGE_MAP()

	static UINT ThreadFunction(LPVOID _mothod);

public:
	//std::vector<cv::Mat> vesselImg;
	cv::Mat vesselImg;
	cv::Mat vesselImgG;
	cv::Mat m_disp_tmp;

	CSegmTree SegmTree;

	cv::Mat m_cropImg;
	cv::Mat m_cropTmp;

	CString strFilePath;

	bool m_bDrawFMMSegment;
	cv::Point m_curVesSegmStartPt, m_curVesSegmEndPt;
	cv::Point m_ptCur;
	cv::Point m_leftTop, m_rightBottom;
	std::vector<cv::Point> m_userPath;
	std::vector<cv::Point> m_curVesSegmPath;
	int iLineIndex;
	int iLineSelected_state;

	cv::Mat m_zoomImg;
	cv::Point m_ptCenterZoomWnd;
	cv::Point m_ptCur_Zoom;
	cv::Point m_ptZoom;
	cv::Point m_ptZoom_tmp;
	std::vector<cv::Point> m_vecPtZoom;
	std::vector<cv::Point> vecPoints;

	// to perform fast-marching method, for computing geodesic between points
	cFastMarching fmm;
	// to perform frangi-filtering
	cFrangiFilter frfilt;
	cv::Mat frangiImg;
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
	

	CRect m_rcPic, m_rcNavi;
	CStatic m_Picture;
	CSliderCtrl m_ctrlSlider;
	CStatic m_picZoom;
	CEdit m_editZoomRatio;
	CSliderCtrl m_ctrlSliderZoomRatio;
	CString strEdit;
	CButton m_ctrlRadioNormal;
	CButton m_ctrlRadioDraw;
	CButton m_ctrlRadioMove;

	//fast marching

	//std::vector<CString> m_fileName;
	WCHAR m_pszPathName[2000];


	///////////////////////
	//coded by kjNoh 160922
	//std::vector<featureInfo> vecSpEpJp;
	
	int featSearchRange;
	int lineSearchRange;
	cv::Point selectedFeat;
	cv::Point selectedLinePoint;
	int mouseR_state; // 0=non, 1 = click
	int dragIdx;
	bool dragSpEp; // if seleted drag point is start point, drag point is dragSpEp=0, another dragSpEp=1
	int m_width;
	int m_height;
	void updateSegm();
	featureInfo oldFeat;
	///////////////////////

	std::vector<std::string> vecFname;

	int m_nFrame;

	bool m_bThreadStart;
	CWinThread *m_pThread;
	ThreadWorkingType m_eThreadWork;

	int iFeatSelected_state;
	cv::Mat FrangiScale;
	cv::Mat m_mask;

	CFeatureTree featureTree;

	cv::Mat FeatrueImg;
	cv::Mat SegLabelImg;

	//20161004 _daseul
	cv::Mat m_cur_mask;

	//time test
	float fFull_Time;
	time_t tStart, tEnd;
	bool m_bPostProc;
	bool m_bOverlay;
	CButton m_ButtManual;

	bool m_bManualEdit;

	int m_bDrawManualSegment; // 0=non, 1 = click
	cv::Point m_prePt;
	CStatic m_ctrl_FileName;
	std::vector<CVesSegm> m_tmpLine;


	//2016.12.21_daseul _UNDO&REDO
	void DrawPicture(cv::Mat disp);
	void DrawNavi2window(cv::Mat disp);
	void ZoomProcessing();
	void ReDrawMask();
	std::vector<cv::Point> getLine(cv::Point sp, cv::Point ep);
	void dispUpdate(bool bPaint = true);

	// 2016.12.29 SCLEE - CLEAN CODE
	void SaveVesCenterlineMask(std::string str);
	// ADDITIONAL 2017.01.25
	void OpenEndedFMM(); 
	void OpenEndedFMM(cv::Point end_pt); // ADDED 2017.02.02 kjnoh
	void OpenEndedFMMwithUserPath(cv::Point end_pt); // ADDED 2017.02.07 sclee
	bool LoadFrangi(std::string ffPath);
	void SaveFrangi(std::string ffpath);
	bool LoadVesselCenterlineInfo(std::string vsc_fn);
	void SaveVesselCenterlineInfo(std::string vsc_fn);
	bool LoadVesselCenterlinePoints(std::string vsc_fn);
	void SaveVesselCenterlinePoints(std::string vsc_fn);
	void SaveVesselCenterlinePoints(std::string vsc_fn,
		std::vector<std::vector<cv::Point>> &segm_tree);
	void FindFilesInDir(CString dir, CString ext, std::vector<std::string> &path_list);
	// 2017.01.09 SCLEE - CONSTRUCT CSV FILE
	void ConvertPerFrameData(CString dir);
	void ConstructSeqCSVDataFile(CString dir);
	// 2017.02.06 SCLEE - REVISION OF MOUSE RESPONSE FUNCTIONS
	bool checkIfPointIsNearFeat(cv::Point pt);
	bool checkIfPointIsNearLine(cv::Point pt);
	void FinalizeVesselSegment(std::vector<cv::Point> vpath);
	void ClearCurPath();

	//2017.01.05_daseul _Zoom in&out(ctrl+mouse wheel)
	bool bCtrl;
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	bool bWheel;
	cv::Mat m_zoom_tmp;
	cv::Point m_zoomCenter;

	//2017.02.07 SJKOH - NAVI & PROGRESS & CANCEL
	int m_ctrlRadioNavi;
	CProgressCtrl m_ctrlProgress;
	BOOL m_bCancel;
	afx_msg void OnBnClickedButtonCancel();
	void ProcessWindowMessage();
};
