#include "stdafx.h"
#include "Feature.h"

#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif


CFeatureTree::CFeatureTree()
{
	nFeature = 0;
}


CFeatureTree::~CFeatureTree()
{
}
int CFeatureTree::getSize()
{
	WriteLog(__FILE__, __LINE__, "CFeatureTree::getSize()");

	return (int)vecFeature.size();
}
void CFeatureTree::addFeat(featureInfo feat)
{
	WriteLog(__FILE__, __LINE__, "CFeatureTree::addFeat()");

	vecFeature.push_back(feat);
	nFeature = getSize();
}
void CFeatureTree::rmFeat(int i)
{
	if (!vecFeature.size())
		return;

	WriteLog(__FILE__, __LINE__, "CFeatureTree::rmFeat()");

	vecFeature.erase(vecFeature.begin()+i);
	nFeature = getSize();
}
void CFeatureTree::rmAll()
{
	if (!vecFeature.size())
		return;

	WriteLog(__FILE__, __LINE__, "CFeatureTree::rmAll");

	vecFeature.clear();
	nFeature = 0;
}
featureInfo CFeatureTree::get(int i)
{
	if (!vecFeature.size())
		return featureInfo();

	WriteLog(__FILE__, __LINE__, "CFeatureTree::get()");


	return vecFeature[i];
}

void CFeatureTree::insert(int i, featureInfo feat)
{
	WriteLog(__FILE__, __LINE__, "CFeatureTree::insert()");

	vecFeature.insert(vecFeature.begin() + i, feat);
	nFeature = getSize();
}