
#include "Feature.h"


#ifdef _DEBUG
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
	//WriteLog(__FILE__, __LINE__, "CFeatureTree::getSize()");

	return (int)vecFeature.size();
}
void CFeatureTree::addFeat(featureInfo feat)
{
	//WriteLog(__FILE__, __LINE__, "CFeatureTree::addFeat()");

	vecFeature.push_back(feat);
	nFeature = getSize();
}
void CFeatureTree::rmFeat(int i)
{
	if (!vecFeature.size())
		return;

	//WriteLog(__FILE__, __LINE__, "CFeatureTree::rmFeat()");

	vecFeature.erase(vecFeature.begin()+i);
	nFeature = getSize();
}
void CFeatureTree::rmAll()
{
	if (!vecFeature.size())
		return;

	//WriteLog(__FILE__, __LINE__, "CFeatureTree::rmAll");

	vecFeature.clear();
	nFeature = 0;
}
featureInfo CFeatureTree::get(int i)
{
	if (!vecFeature.size())
		return featureInfo();

	//WriteLog(__FILE__, __LINE__, "CFeatureTree::get()");

	return vecFeature[i];
}

void CFeatureTree::insert(int i, featureInfo feat)
{
	//WriteLog(__FILE__, __LINE__, "CFeatureTree::insert()");

	vecFeature.insert(vecFeature.begin() + i, feat);
	nFeature = getSize();
}


void CFeatureTree::init(CSegmTree &SegmTree, std::vector<cv::Point> &J)
{
	rmAll();
	for (int j = 0; j < SegmTree.nSegm; j++)
	{
		CVesSegm tmp_Segm = SegmTree.get(j);
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
		addFeat(cur_feat);
		tmp_Segm.clear();
	}
}