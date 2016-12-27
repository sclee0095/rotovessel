#include "stdafx.h"
#include "SegmTree.h"

#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif


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
	WriteLog(__FILE__, __LINE__, __FUNCTION__);
	if (!vecSegmTree.size())
	{
		nSegm = 0;
		return;
	}

	vecSegmTree.erase(vecSegmTree.begin()+i);
	nSegm = vecSegmTree.size();

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

}
void CSegmTree::rmAll()
{
	if (!vecSegmTree.size())
	{
		WriteLog(__FILE__, __LINE__, __FUNCTION__);

		nSegm = 0;
		return;
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	vecSegmTree.clear();
	nSegm = 0;
}
void CSegmTree::addSegm(std::vector<cv::Point> segm)
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	vecSegmTree.push_back(segm);
	nSegm = vecSegmTree.size();
}

std::vector<cv::Point> CSegmTree::get(int i)
{
	if (!vecSegmTree.size() || i >= vecSegmTree.size())
		return std::vector<cv::Point>();

	WriteLog(__FILE__, __LINE__, __FUNCTION__);


	return vecSegmTree[i];
}

void CSegmTree::insert(int i,std::vector<cv::Point> segm)
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	vecSegmTree.insert(vecSegmTree.begin() + i, segm);
	nSegm = vecSegmTree.size();
}
void CSegmTree::newSetTree(std::vector<std::vector<cv::Point>> tree)
{
	WriteLog(__FILE__, __LINE__, __FUNCTION__);

	rmAll();
	for (int i = 0; i < tree.size(); i++)
	{
		addSegm(tree[i]);
	}

	WriteLog(__FILE__, __LINE__, __FUNCTION__);
}

	