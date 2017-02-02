#pragma once

#include "std_include.h"
#include "SegmTree.h"

struct VCO_EXPORTS featureInfo
{
	cv::Point sp;
	cv::Point ep;

	// point type, 0= end(non junction point) and 1= junction
	int spType;
	int epType;
};

class VCO_EXPORTS CFeatureTree
{
public:
	CFeatureTree();
	~CFeatureTree();

	std::vector<featureInfo> vecFeature;

	int nFeature;
	
	int getSize();

	void addFeat(featureInfo feat);
	void rmFeat(int i);
	void rmAll();
	featureInfo get(int i);
	void insert(int i, featureInfo feat);
	
	void init(CSegmTree &SegmTree, std::vector<cv::Point> &J);
};

