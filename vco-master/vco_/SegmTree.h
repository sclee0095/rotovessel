#pragma once

//#include "stdafx.h"
#include "std_include.h"

class CVesSegmPt
{
public:
	CVesSegmPt() : pt(-1, -1), normal(0.f, 0.f), rad(0.f) {}
	CVesSegmPt(cv::Point _pt) : pt(_pt), normal(0.f, 0.f), rad(0.f) {}
	CVesSegmPt(int x, int y) : pt(x, y), normal(0.f, 0.f), rad(0.f) {}
	CVesSegmPt(int x, int y, float nx, float ny, float r) : pt(x, y), normal(nx, ny), rad(r) {}
	CVesSegmPt(const CVesSegmPt &vsp) : pt(vsp.pt), normal(vsp.normal), rad(vsp.rad) {}
	~CVesSegmPt() {};
	void setrad(double r) { rad = (float)r; }
	void setrad(float r) { rad = r; }
	cv::Point pt;
	cv::Point2f normal;
	float rad;
};

class CVesSegm
{
public:
	std::vector<CVesSegmPt> vesSegm;
	bool bCompRadii;
	bool bCompNormals;

	CVesSegm() : bCompRadii(false), bCompNormals(false) {}
	CVesSegm(std::vector<cv::Point> &vs) : bCompRadii(false), bCompNormals(false)   {
		init_from_ptarr(vs);
	}
	CVesSegm(const CVesSegm &vs) : bCompRadii(vs.bCompRadii), bCompNormals(vs.bCompNormals) {
		for (int i = 0; i < (int)vs.vesSegm.size(); i++)
			vesSegm.push_back(CVesSegmPt(vs.vesSegm[i]));
	}
	~CVesSegm() {
		clear();
	}

	void init_from_ptarr(std::vector<cv::Point> &vs) {
		for (int i = 0; i < (int)vs.size(); i++)
			vesSegm.push_back(CVesSegmPt(vs[i]));
	}
	void init_from_ptarr_rev(std::vector<cv::Point> &vs) {
		for (int i = (int)vs.size() - 1; i >= 0; i--)
			vesSegm.push_back(CVesSegmPt(vs[i]));
	}

	void push_back(const CVesSegmPt vp) {
		vesSegm.push_back(vp);
	}
	void clear() { 
		vesSegm.clear(); 
		bCompRadii = false;
		bCompNormals = false;
	}
	size_t size() { return vesSegm.size(); }
	CVesSegmPt &operator[](const int i) { return vesSegm[i]; }
	CVesSegmPt &front() { return vesSegm.front(); }
	CVesSegmPt &back() { return vesSegm.back(); }
	void draw2mask(cv::Mat &mask, cv::Scalar col) 
	{
		for (int i = 0; i < (int)vesSegm.size(); i++) {
			int rad = cvRound(vesSegm[i].rad);
			cv::circle(mask, vesSegm[i].pt, rad, col, -1);
		}
	}
	void drawline_8UC1(cv::Mat &mask, uchar col) 
	{
		for (int i = 0; i < (int)vesSegm.size(); i++) {
			mask.at<uchar>(vesSegm[i].pt) = col;
		}
	}
	void drawline_8UC3(cv::Mat &mask, cv::Scalar col)
	{
		for (int i = 0; i < (int)vesSegm.size(); i++) {
			mask.at<cv::Vec3b>(vesSegm[i].pt) = cv::Vec3b(col.val[0], col.val[1], col.val[2]);
		}
	}
	void set_radii(cv::Mat dtmap)
	{
		for (int i = 0; i < (int)size(); i++) {
			vesSegm[i].setrad(dtmap.at<float>(vesSegm[i].pt));
		}
		bCompRadii = true;
	}
	void smooth_radii(int n, int iter)//, bool weighted)
	{
		if (bCompRadii) {
			int nh = n / 2;
			for (int it = 0; it < iter; it++) {
				for (int i = 0; i < (int)vesSegm.size(); i++) {
					float mr = 0;
					int mr_count = 0;
					for (int k = -nh; k <= nh; k++) {
						if (i + k >= 0 && i + k < (int)vesSegm.size()) {
							mr += vesSegm[i + k].rad;
							mr_count++;
						}
					}
					vesSegm[i].rad = mr / (float)mr_count;
				}
			}
		}
	}
	void compute_normals(int nwid)
	{
		int nh = nwid / 2;
		for (int i = 0; i < (int)vesSegm.size(); i++) {
			int st = std::max(i - nh, 0);
			int ed = std::min(i + nh, (int)vesSegm.size()-1);
			cv::Point tv = vesSegm[ed].pt - vesSegm[st].pt;
			cv::Point2f nv(-tv.y, tv.x);
			vesSegm[i].normal = nv / cv::norm(nv);
		}
		bCompNormals = true;
	}
	void draw_normals(cv::Mat img, int intv, float length, cv::Scalar col, int thickness)
	{
		if (bCompNormals) {
			for (int i = 0; i < (int)vesSegm.size(); i += intv) {
				cv::Point2f arrend(vesSegm[i].pt);
				arrend = arrend + vesSegm[i].normal*length;
				cv::Point ae(std::min(std::max(cvRound(arrend.x), 0), img.cols - 1),
					std::min(std::max(cvRound(arrend.y), 0), img.rows - 1));
				cv::arrowedLine(img, vesSegm[i].pt, ae, col, thickness);
			}
		}
	}
	void copyFrom(const CVesSegm &vs) {
		clear();
		for (int i = 0; i < (int)vs.vesSegm.size(); i++)
			vesSegm.push_back(CVesSegmPt(vs.vesSegm[i]));
		bCompRadii = vs.bCompRadii;
		bCompNormals = vs.bCompNormals;
	}
	void SetVesselRadiiFromFraigiScale(cv::Mat frangi_scale)
	{
		for (int i = 0; i < (int)vesSegm.size(); i++) {
			vesSegm[i].setrad(frangi_scale.at<double>(vesSegm[i].pt));
		}
	}
};

class VCO_EXPORTS CSegmTree
{
public:
	CSegmTree();
	~CSegmTree();

	//std::vector<std::vector<cv::Point>> vecSegmTree;
	//std::vector<std::vector<CVesSegmPt>> vecSegmTree;
	std::vector<CVesSegm> vecSegmTree;

	void addSegm(CVesSegm &segm);
	void addSegm(std::vector<cv::Point> &segm_pt);
	void rmSegm(int i);
	void rmAll();
	void insert(int i, CVesSegm &segm);
	void insert(int i, std::vector<cv::Point> &segm_pt);
	void newSetTree(std::vector<std::vector<cv::Point>> &tree_pt);
	void newSetTree(std::vector<CVesSegm> &tree);

	CVesSegm &get(int i);
	CVesSegm &back();

	void draw2mask(cv::Mat mask, cv::Scalar col=cv::Scalar(255,255,255));
	void drawlines_8UC1(cv::Mat mask, uchar col = 255);
	void SetVesselRadiiFromFraigiScale(cv::Mat frangi_scale);
	int nSegm;
};

