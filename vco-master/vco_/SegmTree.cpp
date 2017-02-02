//#include "stdafx.h"
#include "SegmTree.h"


CSegmTree::CSegmTree()
{
	nSegm = 0;
}


CSegmTree::~CSegmTree()
{

	vecSegmTree.clear();
}


void CSegmTree::rmSegm(int i)
{
	//WriteLog(__FILE__, __LINE__, __FUNCTION__);
	if (!vecSegmTree.size())
	{
		nSegm = 0;
		return;
	}

	vecSegmTree.erase(vecSegmTree.begin()+i);
	nSegm = vecSegmTree.size();

	//WriteLog(__FILE__, __LINE__, __FUNCTION__);

}
void CSegmTree::rmAll()
{
	if (!vecSegmTree.size())
	{
		//WriteLog(__FILE__, __LINE__, __FUNCTION__);

		nSegm = 0;
		return;
	}

	//WriteLog(__FILE__, __LINE__, __FUNCTION__);

	vecSegmTree.clear();
	nSegm = 0;
}
void CSegmTree::addSegm(CVesSegm &segm)
{
	//WriteLog(__FILE__, __LINE__, __FUNCTION__);
	vecSegmTree.push_back(segm);
	nSegm = vecSegmTree.size();
}
void CSegmTree::addSegm(std::vector<cv::Point> &segm_pt)
{
	addSegm(CVesSegm(segm_pt));
}
CVesSegm &CSegmTree::get(int i)
{
	if (!vecSegmTree.size() || i >= vecSegmTree.size())
		//return std::vector<cv::Point>();
		return CVesSegm();

	return vecSegmTree[i];
}
CVesSegm &CSegmTree::back()
{
	return vecSegmTree.back();
}

void CSegmTree::insert(int i, CVesSegm &segm)
{
	vecSegmTree.insert(vecSegmTree.begin() + i, segm);
	nSegm = vecSegmTree.size();
}
void CSegmTree::insert(int i, std::vector<cv::Point> &segm_pt)
{
	insert(i, CVesSegm(segm_pt));
}

void CSegmTree::newSetTree(std::vector<CVesSegm> &tree)
{
	rmAll();
	for (int i = 0; i < tree.size(); i++)
	{
		addSegm(tree[i]);
	}
}
void CSegmTree::newSetTree(std::vector<std::vector<cv::Point>> &tree_pt)
{
	rmAll();
	for (int i = 0; i < tree_pt.size(); i++)
	{
		addSegm(tree_pt[i]);
	}
}
void CSegmTree::draw2mask(cv::Mat mask, cv::Scalar col)
{
	for (int i = 0; i < vecSegmTree.size(); i++)
		vecSegmTree[i].draw2mask(mask, col);
}
void CSegmTree::drawlines_8UC1(cv::Mat mask, uchar col)
{
	for (int i = 0; i < vecSegmTree.size(); i++)
		vecSegmTree[i].drawline_8UC1(mask, col);
}
void CSegmTree::SetVesselRadiiFromFraigiScale(cv::Mat frangi_scale)
{
	for (int i = 0; i < vecSegmTree.size(); i++)
		vecSegmTree[i].SetVesselRadiiFromFraigiScale(frangi_scale);
}
