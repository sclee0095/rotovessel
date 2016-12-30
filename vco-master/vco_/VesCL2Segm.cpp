
/*
Original OpenCV code grabcut.cpp modified by SCLEE, 20161228
Modifications:
- class cGMMg: modified GMM class so that 
  number of Gaussians can be modified by constructor parameter and
  input image is 1 channel greyscale image
- grabCut: parameter lambda (and gamma) can be modified by function parameter
*/

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
#include "std_include.h"
#include "VCO.h"
#include "GC/gcgraph.hpp"
#include <limits>


/*
This is implementation of image segmentation algorithm GrabCut described in
"GrabCut — Interactive Foreground Extraction using Iterated Graph Cuts".
Carsten Rother, Vladimir Kolmogorov, Andrew Blake.
*/



/*
cGMMg - Gaussian Mixture Model for GREYSCALE input
 by SOOCHAHN LEE, 20161229
*/
class cGMMg
{
public:
	int componentsCount;

	cGMMg(cv::Mat& _model, int cc = 5);
	~cGMMg();
	double operator()(const double i) const;
	double operator()(int ci, const double i) const;
	int whichComponent(const double i) const;

	void initLearning();
	void addSample(int ci, const uchar i);
	void endLearning();

private:
	void calcInverseCovAndDeterm(int ci);
	cv::Mat model;
	double* coefs;
	double* mean;
	double* cov;

	double *inverseCovs;
	double *sums;
	double *prods;
	int *sampleCounts;

	int totalSampleCount;
};

cGMMg::cGMMg(cv::Mat& _model, int cc) : componentsCount(cc)
{
	const int modelSize = 1/*mean*/ + 1/*variance*/ + 1/*component weight*/;
	if (_model.empty())
	{
		_model.create(1, modelSize*componentsCount, CV_64FC1);
		_model.setTo(cv::Scalar(0));
	}
	else if ((_model.type() != CV_64FC1) || (_model.rows != 1) ||
		(_model.cols != modelSize*componentsCount))
		CV_Error(CV_StsBadArg, 
			"_model must have CV_64FC1 type, rows == 1 and cols == 13*componentsCount");

	model = _model;

	coefs = model.ptr<double>(0);
	mean = coefs + componentsCount;
	cov = mean + componentsCount;

	inverseCovs = new double[componentsCount];
	sums = new double[componentsCount];
	prods = new double[componentsCount];
	sampleCounts = new int[componentsCount];

	for (int ci = 0; ci < componentsCount; ci++)
	if (coefs[ci] > 0)
		inverseCovs[ci] = 1.0 / cov[ci];
}

cGMMg::~cGMMg()
{
	delete[] inverseCovs;
	delete[] sums;
	delete[] prods;
	delete[] sampleCounts;
}

double cGMMg::operator()(const double i) const
{
	double res = 0;
	for (int ci = 0; ci < componentsCount; ci++)
		res += coefs[ci] * (*this)(ci, i);
	return res;
}

double cGMMg::operator()(int ci, const double i) const
{
	double res = 0;
	if (coefs[ci] > 0)
	{
		CV_Assert(cov[ci] > std::numeric_limits<double>::epsilon());
		double diff = i;
		diff -= mean[ci];
		double mult = diff*diff*inverseCovs[ci];
		res = 1.0f / sqrt(cov[ci]) * exp(-0.5f*mult);
	}
	return res;
}

int cGMMg::whichComponent(const double i) const
{
	int k = 0;
	double max = 0;

	for (int ci = 0; ci < componentsCount; ci++)
	{
		double p = (*this)(ci, i);
		if (p > max)
		{
			k = ci;
			max = p;
		}
	}
	return k;
}

void cGMMg::initLearning()
{
	for (int i = 0; i < componentsCount; i++) {
		inverseCovs[i] = 0;
		sums[i] = 0;
		prods[i] = 0;
		sampleCounts[i] = 0;
	}
	totalSampleCount = 0;
}

void cGMMg::addSample(int ci, const uchar i)
{
	sums[ci] += i;
	prods[ci] += i*i;
	sampleCounts[ci]++;
	totalSampleCount++;
}

void cGMMg::endLearning()
{
	const double variance = 0.01;
	for (int ci = 0; ci < componentsCount; ci++)
	{
		int n = sampleCounts[ci];
		if (n == 0)
			coefs[ci] = 0;
		else
		{
			coefs[ci] = (double)n / totalSampleCount;
			mean[ci] = sums[ci] / n;
			cov[ci] = prods[ci] / n - mean[ci] * mean[ci];
			inverseCovs[ci] = 1 / cov[ci];
		}
	}
}
// end cGMMg - greyscale GMM



/*
Calculate beta - parameter of GrabCut algorithm.
beta = 1/(2*avg(sqr(||color[i] - color[j]||)))
*/
static double calcBeta(const cv::Mat& img)
{
	double beta = 0;
	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			double color = (double)img.at<uchar>(y, x);
			if (x>0) // left
			{
				double diff = color - (double)img.at<uchar>(y, x - 1);
				beta += diff*diff;
			}
			if (y>0 && x>0) // upleft
			{
				double diff = color - (double)img.at<uchar>(y - 1, x - 1);
				beta += diff*diff;
			}
			if (y>0) // up
			{
				double diff = color - (double)img.at<uchar>(y - 1, x);
				beta += diff*diff;
			}
			if (y>0 && x<img.cols - 1) // upright
			{
				double diff = color - (double)img.at<uchar>(y - 1, x + 1);
				beta += diff*diff;
			}
		}
	}
	if (beta <= std::numeric_limits<double>::epsilon())
		beta = 0;
	else
		beta = 1.f / (2 * beta / (4 * img.cols*img.rows - 3 * img.cols - 3 * img.rows + 2));

	return beta;
}

/*
Calculate weights of noterminal vertices of graph.
beta and gamma - parameters of GrabCut algorithm.
*/
static void calcNWeights(const cv::Mat& img, 
						 cv::Mat& leftW, cv::Mat& upleftW, 
						 cv::Mat& upW, cv::Mat& uprightW, 
						 double beta, double gamma)
{
	const double gammaDivSqrt2 = gamma / std::sqrt(2.0f);
	leftW.create(img.rows, img.cols, CV_64FC1);
	upleftW.create(img.rows, img.cols, CV_64FC1);
	upW.create(img.rows, img.cols, CV_64FC1);
	uprightW.create(img.rows, img.cols, CV_64FC1);
	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			double color = (double)img.at<uchar>(y, x);
			if (x - 1 >= 0) // left
			{
				double diff = color - (double)img.at<uchar>(y, x - 1);
				leftW.at<double>(y, x) = gamma * exp(-beta*diff*diff);
			}
			else
				leftW.at<double>(y, x) = 0;
			if (x - 1 >= 0 && y - 1 >= 0) // upleft
			{
				double diff = color - (double)img.at<uchar>(y - 1, x - 1);
				upleftW.at<double>(y, x) = gammaDivSqrt2 * exp(-beta*diff*diff);
			}
			else
				upleftW.at<double>(y, x) = 0;
			if (y - 1 >= 0) // up
			{
				double diff = color - (double)img.at<uchar>(y - 1, x);
				upW.at<double>(y, x) = gamma * exp(-beta*diff*diff);
			}
			else
				upW.at<double>(y, x) = 0;
			if (x + 1<img.cols && y - 1 >= 0) // upright
			{
				double diff = color - (double)img.at<uchar>(y - 1, x + 1);
				uprightW.at<double>(y, x) = gammaDivSqrt2 * exp(-beta*diff*diff);
			}
			else
				uprightW.at<double>(y, x) = 0;
		}
	}
}

/*
Check size, type and element values of mask matrix.
*/
static void checkMask(const cv::Mat& img, const cv::Mat& mask)
{
	if (mask.empty())
		CV_Error(CV_StsBadArg, "mask is empty");
	if (mask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "mask must have CV_8UC1 type");
	if (mask.cols != img.cols || mask.rows != img.rows)
		CV_Error(CV_StsBadArg, "mask must have as many rows and cols as img");
	for (int y = 0; y < mask.rows; y++)
	{
		for (int x = 0; x < mask.cols; x++)
		{
			uchar val = mask.at<uchar>(y, x);
			if (val != cv::GC_BGD && val != cv::GC_FGD && 
				val != cv::GC_PR_BGD && val != cv::GC_PR_FGD)
				CV_Error(CV_StsBadArg, "mask element value must be equal "
				"cv::GC_BGD or cv::GC_FGD or cv::GC_PR_BGD or cv::GC_PR_FGD");
		}
	}
}

/*
Initialize mask using rectangular.
*/
static void initMaskWithRect(cv::Mat& mask, cv::Size imgSize, cv::Rect rect)
{
	mask.create(imgSize, CV_8UC1);
	mask.setTo(cv::GC_BGD);

	rect.x = std::max(0, rect.x);
	rect.y = std::max(0, rect.y);
	rect.width = std::min(rect.width, imgSize.width - rect.x);
	rect.height = std::min(rect.height, imgSize.height - rect.y);

	(mask(rect)).setTo(cv::Scalar(cv::GC_PR_FGD));
}

/*
Initialize cGMMg background and foreground models using kmeans algorithm.
*/
static void initGMMs(const cv::Mat& img, const cv::Mat& mask, cGMMg& bgdGMM, cGMMg& fgdGMM, int nGMMcc)
{
	const int kMeansItCount = 10;
	const int kMeansType = cv::KMEANS_PP_CENTERS;

	cv::Mat bgdLabels, fgdLabels;
	std::vector<float> bgdSamples, fgdSamples;
	cv::Point p;
	for (p.y = 0; p.y < img.rows; p.y++)
	{
		for (p.x = 0; p.x < img.cols; p.x++)
		{
			if (mask.at<uchar>(p) == cv::GC_BGD || mask.at<uchar>(p) == cv::GC_PR_BGD)
				bgdSamples.push_back((float)img.at<uchar>(p));
			else // cv::GC_FGD | cv::GC_PR_FGD
				fgdSamples.push_back((float)img.at<uchar>(p));
		}
	}
	CV_Assert(!bgdSamples.empty() && !fgdSamples.empty());
	cv::Mat _bgdSamples((int)bgdSamples.size(), 1, CV_32FC1, &bgdSamples[0]);
	cv::kmeans(_bgdSamples, nGMMcc, bgdLabels,
		cv::TermCriteria(CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType);
	cv::Mat _fgdSamples((int)fgdSamples.size(), 1, CV_32FC1, &fgdSamples[0]);
	cv::kmeans(_fgdSamples, nGMMcc, fgdLabels,
		cv::TermCriteria(CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType);

	bgdGMM.initLearning();
	for (int i = 0; i < (int)bgdSamples.size(); i++)
		bgdGMM.addSample(bgdLabels.at<int>(i, 0), bgdSamples[i]);
	bgdGMM.endLearning();

	fgdGMM.initLearning();
	for (int i = 0; i < (int)fgdSamples.size(); i++)
		fgdGMM.addSample(fgdLabels.at<int>(i, 0), fgdSamples[i]);
	fgdGMM.endLearning();
}

/*
Assign GMMs components for each pixel.
*/
static void assignGMMsComponents(const cv::Mat& img, const cv::Mat& mask, 
	const cGMMg& bgdGMM, const cGMMg& fgdGMM, cv::Mat& compIdxs)
{
	cv::Point p;
	for (p.y = 0; p.y < img.rows; p.y++)
	{
		for (p.x = 0; p.x < img.cols; p.x++)
		{
			double color = (double)img.at<uchar>(p);
			compIdxs.at<int>(p) = 
				mask.at<uchar>(p) == cv::GC_BGD || mask.at<uchar>(p) == cv::GC_PR_BGD ?
				bgdGMM.whichComponent(color) : fgdGMM.whichComponent(color);
		}
	}
}

/*
Learn GMMs parameters.
*/
static void learnGMMs(const cv::Mat& img, const cv::Mat& mask, const cv::Mat& compIdxs, 
	cGMMg& bgdGMM, cGMMg& fgdGMM, int nGMMcc)
{
	bgdGMM.initLearning();
	fgdGMM.initLearning();
	cv::Point p;
	for (int ci = 0; ci < nGMMcc; ci++)
	{
		for (p.y = 0; p.y < img.rows; p.y++)
		{
			for (p.x = 0; p.x < img.cols; p.x++)
			{
				if (compIdxs.at<int>(p) == ci)
				{
					if (mask.at<uchar>(p) == cv::GC_BGD || 
						mask.at<uchar>(p) == cv::GC_PR_BGD)
						bgdGMM.addSample(ci, (double)img.at<uchar>(p));
					else
						fgdGMM.addSample(ci, (double)img.at<uchar>(p));
				}
			}
		}
	}
	bgdGMM.endLearning();
	fgdGMM.endLearning();
}

/*
Construct GCGraph
*/
static void constructGCGraph(const cv::Mat& img, const cv::Mat& mask, 
	const cGMMg& bgdGMM, const cGMMg& fgdGMM, double lambda,
	const cv::Mat& leftW, const cv::Mat& upleftW, const cv::Mat& upW, const cv::Mat& uprightW,
	GCGraph<double>& graph)
{
	int vtxCount = img.cols*img.rows,
		edgeCount = 2 * (4 * img.cols*img.rows - 3 * (img.cols + img.rows) + 2);
	graph.create(vtxCount, edgeCount);
	cv::Point p;
	cv::Mat unary_bg(img.rows, img.cols, CV_32F);
	cv::Mat unary_fg(img.rows, img.cols, CV_32F);
	for (p.y = 0; p.y < img.rows; p.y++)
	{
		for (p.x = 0; p.x < img.cols; p.x++)
		{
			// add node
			int vtxIdx = graph.addVtx();
			double color = (double)img.at<uchar>(p);

			// set t-weights
			double fromSource, toSink;
			if (mask.at<uchar>(p) == cv::GC_PR_BGD || mask.at<uchar>(p) == cv::GC_PR_FGD)
			{
				fromSource = -log(bgdGMM(color));
				toSink = -log(fgdGMM(color));
			}
			else if (mask.at<uchar>(p) == cv::GC_BGD)
			{
				fromSource = 0;
				toSink = lambda;
			}
			else // cv::GC_FGD
			{
				fromSource = lambda;
				toSink = 0;
			}
			graph.addTermWeights(vtxIdx, fromSource, toSink);
			unary_bg.at<float>(p) = fromSource;
			unary_fg.at<float>(p) = toSink;

			// set n-weights
			if (p.x>0)
			{
				double w = leftW.at<double>(p);
				graph.addEdges(vtxIdx, vtxIdx - 1, w, w);
			}
			if (p.x>0 && p.y>0)
			{
				double w = upleftW.at<double>(p);
				graph.addEdges(vtxIdx, vtxIdx - img.cols - 1, w, w);
			}
			if (p.y>0)
			{
				double w = upW.at<double>(p);
				graph.addEdges(vtxIdx, vtxIdx - img.cols, w, w);
			}
			if (p.x<img.cols - 1 && p.y>0)
			{
				double w = uprightW.at<double>(p);
				graph.addEdges(vtxIdx, vtxIdx - img.cols + 1, w, w);
			}
		}
	}
	cv::imwrite("unary_bg.png", unary_bg * 50);
	cv::imwrite("unary_fg.png", unary_fg * 50);
}

/*
Estimate segmentation using MaxFlow algorithm
*/
static void estimateSegmentation(GCGraph<double>& graph, cv::Mat& mask)
{
	graph.maxFlow();
	cv::Point p;
	for (p.y = 0; p.y < mask.rows; p.y++)
	{
		for (p.x = 0; p.x < mask.cols; p.x++)
		{
			if (mask.at<uchar>(p) == cv::GC_PR_BGD || mask.at<uchar>(p) == cv::GC_PR_FGD)
			{
				if (graph.inSourceSegment(p.y*mask.cols + p.x /*vertex index*/))
					mask.at<uchar>(p) = cv::GC_PR_FGD;
				else
					mask.at<uchar>(p) = cv::GC_PR_BGD;
			}
		}
	}
}

void grabCut_mod(cv::InputArray _img, cv::InputOutputArray _mask, cv::Rect rect,
	cv::InputOutputArray _bgdModel, cv::InputOutputArray _fgdModel,
	int iterCount, int mode, int nGMMcc, double lambda, double u_max)
{
	//int nGMMcc = 3;

	cv::Mat img = _img.getMat();
	cv::Mat& mask = _mask.getMatRef();
	cv::Mat& bgdModel = _bgdModel.getMatRef();
	cv::Mat& fgdModel = _fgdModel.getMatRef();

	if (img.empty())
		CV_Error(CV_StsBadArg, "image is empty");
	if (img.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "image must have CV_8UC1 type");

	cGMMg bgdGMM(bgdModel, nGMMcc), fgdGMM(fgdModel, nGMMcc);
	cv::Mat compIdxs(img.size(), CV_32SC1);

	if (mode == cv::GC_INIT_WITH_RECT || mode == cv::GC_INIT_WITH_MASK)
	{
		if (mode == cv::GC_INIT_WITH_RECT)
			initMaskWithRect(mask, img.size(), rect);
		else // flag == GC_INIT_WITH_MASK
			checkMask(img, mask);
		initGMMs(img, mask, bgdGMM, fgdGMM, nGMMcc);
	}

	if (iterCount <= 0)
		return;

	if (mode == cv::GC_EVAL)
		checkMask(img, mask);

	//const double gamma = 50;
	//const double lambda = 9 * gamma;
	const double beta = calcBeta(img);

	cv::Mat leftW, upleftW, upW, uprightW;
	calcNWeights(img, leftW, upleftW, upW, uprightW, beta, lambda);

	for (int i = 0; i < iterCount; i++)
	{
		GCGraph<double> graph;
		assignGMMsComponents(img, mask, bgdGMM, fgdGMM, compIdxs);
		learnGMMs(img, mask, compIdxs, bgdGMM, fgdGMM, nGMMcc);
		constructGCGraph(img, mask, bgdGMM, fgdGMM, u_max, 
			leftW, upleftW, upW, uprightW, graph);
		estimateSegmentation(graph, mask);
	}
}


// generate vessel segmentation mask from vessel centerline points 
// using Frangi max response scales
// - by Kyoungjin Noh, 201610
//   method: get max response scale at each vessel centerline point from Frangi filtering results,
//			 draw filled circles at the corresponding point with radius scale
void MakeRegionMask_NKJ(std::vector<cv::Point> &vecPts, cv::Mat &frangiScale, 
	cv::Mat &mask, cv::Scalar col)
{
	cv::Mat convScale(1, vecPts.size(), CV_64FC1);
	for (int k = 0; k < vecPts.size(); k++)
	{
		int cur_x = vecPts[k].x;
		int cur_y = vecPts[k].y;
		double s = frangiScale.at<double>(cur_y, cur_x);

		convScale.at<double>(0, k) = s;
	}

	cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

	for (int k = 0; k < vecPts.size(); k++)
	{
		int cur_x = vecPts[k].x;
		int cur_y = vecPts[k].y;
		//test.at<uchar>(cur_y,cur_x*3+0)
		//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);

		cv::circle(mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), col, -1);
	}
	convScale.release();
}


// generate vessel segmentation mask from vessel centerline points 
// using graph cuts
// - by Soochahn Lee, 201612
// method: 
// - centerline points are seed points
// - histograms are used to define appearance (unary) probabilities
// - histograms defined by 
//			 draw filled circles at the corresponding point with radius scale
cv::Mat MakeRegionMask_GraphCut(std::vector<cv::Point> &vecPts,
	cv::Mat &frangiScale,
	cv::Mat &img, cv::Mat &mask)
{
	int w = mask.rows;
	int h = mask.cols;
	// construct mask
	cv::Mat gc_mask(mask.size(), CV_8UC1);

	// initialize to PROBABLY_BGD
	gc_mask.setTo(cv::Scalar(cv::GC_BGD));

	// make ROI based on current vessel points
	int minvx, maxvx, minvy, maxvy;
	minvx = minvy = INT_MAX;
	maxvx = maxvy = INT_MIN;
	for (int k = 0; k < vecPts.size(); k++) {
		if (vecPts[k].x < minvx) minvx = vecPts[k].x;
		if (vecPts[k].y < minvy) minvy = vecPts[k].y;
		if (vecPts[k].x > maxvx) maxvx = vecPts[k].x;
		if (vecPts[k].y > maxvy) maxvy = vecPts[k].y;
	}
	// add buffer for roi
	int roi_buf = 10;
	minvx = cv::max(0, minvx - roi_buf);
	minvy = cv::max(0, minvy - roi_buf);
	maxvx = cv::min(w, maxvx + roi_buf);
	maxvy = cv::min(h, maxvy + roi_buf);
	cv::Rect gc_roi(minvx, minvy, maxvx - minvx, maxvy - minvy);
	cv::Mat gc_mask_roi = gc_mask(gc_roi);
	gc_mask_roi.setTo(cv::Scalar(cv::GC_PR_BGD));

	// set mask by frangi-max-response-scale region as PROBABLY_FGD 
	MakeRegionMask_NKJ(vecPts, frangiScale, gc_mask, cv::Scalar(cv::GC_PR_FGD));
	// set centerline points as DEFINITLY_FGD
	for (int k = 0; k < vecPts.size(); k++)
		gc_mask.at<char>(vecPts[k]) = cv::GC_FGD;
	//cv::imshow("grabcut input mask", gc_mask*63);
	cv::imwrite("grabcut_input_mask.png", gc_mask * 63);
	printf("ROI: (%d, %d, %d, %d)", gc_roi.x, gc_roi.y, gc_roi.width, gc_roi.height);
	//cv::imwrite("grabcut_input_mask.png", gc_mask*63);
	cv::Mat bgdModel, fgdModel;
	int iterCount = 1;
	grabCut_mod(img, gc_mask, gc_roi, bgdModel, fgdModel, iterCount,
		cv::GC_INIT_WITH_MASK);
	//cv::imshow("grabcut vessel extraction", m_mask*63);
	cv::imwrite("grabcut_vessel_extraction.png", gc_mask * 63);
	cv::imwrite("grabcut_input.png", gc_mask * 63);
	return gc_mask;
}