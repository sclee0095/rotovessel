#pragma once

#include "stdafx.h"

struct featureInfo
{
	cv::Point sp ;
	cv::Point ep;

	// point type, 0= end(non junction point) and 1= junction
	int spType;
	int epType;
};

class CFeatureTree
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
	
};

