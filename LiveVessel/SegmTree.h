#pragma once

#include "stdafx.h"

class CSegmTree
{
public:
	CSegmTree();
	~CSegmTree();

	std::vector<std::vector<cv::Point>> vecSegmTree;

	void addSegm(std::vector<cv::Point> segm);
	void rmSegm(int i);
	void rmAll();
	void insert(int i, std::vector<cv::Point> segm);
	void newSetTree(std::vector<std::vector<cv::Point>> tree);

	std::vector<cv::Point> get(int i);

	int nSegm;
	
};

