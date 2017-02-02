

#include "std_include.h"
#include "VCO.h"
#include "GC/gcgraph.hpp"
#include <limits>


#define VERBOSE_COMP_VES_RADII


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

/*
This is implementation of image segmentation algorithm GrabCut described in
"GrabCut ?Interactive Foreground Extraction using Iterated Graph Cuts".
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
	//cv::imwrite("unary_bg.png", unary_bg * 50);
	//cv::imwrite("unary_fg.png", unary_fg * 50);
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
void MakeRegionMask_NKJ(
	CVesSegm &vecPts, cv::Mat &frangiScale,
	cv::Mat &mask, cv::Scalar col)
{
	cv::Mat convScale(1, vecPts.size(), CV_64FC1);
	for (int k = 0; k < vecPts.size(); k++)
	{
		int cur_x = vecPts[k].pt.x;
		int cur_y = vecPts[k].pt.y;
		double s = frangiScale.at<double>(cur_y, cur_x);

		convScale.at<double>(0, k) = s;
	}

	cv::GaussianBlur(convScale, convScale, cv::Size(23, 1), 4.4f);

	for (int k = 0; k < vecPts.size(); k++)
	{
		int cur_x = vecPts[k].pt.x;
		int cur_y = vecPts[k].pt.y;
		//test.at<uchar>(cur_y,cur_x*3+0)
		//double s = FrangiScale[i + 1].at<double>(cur_y, cur_x);
		cv::circle(mask, cv::Point(cur_x, cur_y), convScale.at<double>(0, k), col, -1);
	}
	convScale.release();
}


// generate vessel segmentation mask from vessel centerline points 
// and estimated vessel radius
// - by Soochahn Lee, 201701
void MakeRegionMask_VesRadii(CVesSegm &vesSegm, cv::Mat &mask, cv::Scalar col)
{
	for (int k = 0; k < vesSegm.size(); k++)
	{
		cv::circle(mask, vesSegm[k].pt, vesSegm[k].rad, col, -1);
	}
}


cv::Mat Convert2OnlyFGBG(cv::Mat &gcm, uchar BG, uchar FG)
{
	cv::Mat om(gcm.size(), CV_8UC1);
	for (int y = 0; y < gcm.rows; y++) {
		for (int x = 0; x < gcm.cols; x++) {
			uchar p = gcm.at<uchar>(y, x);
			if (p == cv::GC_BGD || p == cv::GC_PR_BGD)
				om.at<uchar>(y, x) = BG;
			if (p == cv::GC_FGD || p == cv::GC_PR_FGD)
				om.at<uchar>(y, x) = FG;
		}
	}
	return om;
}

// generate vessel segmentation mask from vessel centerline points 
// using graph cuts
// - by Soochahn Lee, 201612
// method: 
// - centerline points are seed points
// - histograms are used to define appearance (unary) probabilities
// - histograms defined by 
//			 draw filled circles at the corresponding point with radius scale
cv::Mat MakeRegionMask_GraphCut(
	CVesSegm &vecPts,
	cv::Mat &frangiScale,
	cv::Mat &img, 
	bool bPR_BGD_is_NarrowBand, int bgd_nb_sz, int fgd_nb_sz)
{
	int w = img.rows;
	int h = img.cols;
	// construct mask
	cv::Mat gc_mask(img.size(), CV_8UC1);
	cv::Rect gc_roi;
	// initialize to GC_BGD
	gc_mask.setTo(cv::Scalar(cv::GC_BGD));

	// if probably background is defined as narrowband surrounding vessel region
	if (bPR_BGD_is_NarrowBand)
	{
		// set mask by frangi-max-response-scale region as PROBABLY_FGD 
		cv::Mat v_pbg = cv::Mat::zeros(gc_mask.size(), CV_8UC1);
		MakeRegionMask_NKJ(vecPts, frangiScale, v_pbg, cv::Scalar(cv::GC_PR_BGD));
		cv::Mat se_bg = cv::getStructuringElement(cv::MORPH_ELLIPSE,
			cv::Size(2 * bgd_nb_sz + 1, 2 * bgd_nb_sz + 1));
		cv::dilate(v_pbg, v_pbg, se_bg);
		cv::Mat v_pfg = cv::Mat::zeros(gc_mask.size(), CV_8UC1);
		MakeRegionMask_NKJ(vecPts, frangiScale, v_pfg, cv::Scalar(cv::GC_PR_FGD));
		if (fgd_nb_sz > 0) {
			cv::Mat se_fg = cv::getStructuringElement(cv::MORPH_ELLIPSE,
				cv::Size(2 * fgd_nb_sz + 1, 2 * fgd_nb_sz + 1),
				cv::Point(fgd_nb_sz, fgd_nb_sz));
			cv::erode(v_pfg, v_pfg, se_fg);
		}
		gc_mask = cv::max(cv::max(gc_mask, v_pbg), v_pfg);
	}
	// else, probably background is defined by vessel point bounding box
	else
	{
		// make ROI based on current vessel points
		int minvx, maxvx, minvy, maxvy;
		minvx = minvy = INT_MAX;
		maxvx = maxvy = INT_MIN;
		for (int k = 0; k < vecPts.size(); k++) {
			if (vecPts[k].pt.x < minvx) minvx = vecPts[k].pt.x;
			if (vecPts[k].pt.y < minvy) minvy = vecPts[k].pt.y;
			if (vecPts[k].pt.x > maxvx) maxvx = vecPts[k].pt.x;
			if (vecPts[k].pt.y > maxvy) maxvy = vecPts[k].pt.y;
		}
		// add buffer for roi
		int roi_buf = 10;
		minvx = cv::max(0, minvx - roi_buf);
		minvy = cv::max(0, minvy - roi_buf);
		maxvx = cv::min(w, maxvx + roi_buf);
		maxvy = cv::min(h, maxvy + roi_buf);
		gc_roi = cv::Rect(minvx, minvy, maxvx - minvx, maxvy - minvy);
		cv::Mat gc_mask_roi = gc_mask(gc_roi);
		gc_mask_roi.setTo(cv::Scalar(cv::GC_PR_BGD));
		MakeRegionMask_NKJ(vecPts, frangiScale, gc_mask, cv::Scalar(cv::GC_PR_FGD));
	}
	// set boundary as DEFINITELY_BGD
	cv::line(gc_mask, cv::Point(0, 0), cv::Point(gc_mask.cols - 1, 0), cv::Scalar(cv::GC_BGD));
	cv::line(gc_mask, cv::Point(0, 0), cv::Point(0, gc_mask.rows - 1), cv::Scalar(cv::GC_BGD));
	cv::line(gc_mask, cv::Point(gc_mask.cols - 1, gc_mask.rows - 1), cv::Point(gc_mask.cols - 1, 0), cv::Scalar(cv::GC_BGD));
	cv::line(gc_mask, cv::Point(gc_mask.cols - 1, gc_mask.rows - 1), cv::Point(0, gc_mask.rows - 1), cv::Scalar(cv::GC_BGD));
	// set centerline points as DEFINITLY_FGD
	for (int k = 0; k < vecPts.size(); k++)
		gc_mask.at<char>(vecPts[k].pt) = cv::GC_FGD;

#ifdef VERBOSE_COMP_VES_RADII
	cv::imwrite("gc_seeds.png", gc_mask*60);
#endif
	cv::Mat bgdModel, fgdModel;
	int iterCount = 1;
	grabCut_mod(img, gc_mask, gc_roi, bgdModel, fgdModel, iterCount,
		cv::GC_INIT_WITH_MASK);

	return Convert2OnlyFGBG(gc_mask, 0, 255);
}

/*
cv::Mat GetDCFromMaskBd(cv::Mat mask)
{
	// perform distance transform from mask 
	// *** important that mask has non-zero values for inside vessel and zero for outside vessel
	cv::Mat mask_dist;
	cv::distanceTransform(mask, mask_dist, CV_DIST_L2, cv::DIST_MASK_PRECISE);

	// determine ridges on computed DT, based on method from 
	// http://haralick.org/journals/ridges_and_valleys.pdf
	return cv::Mat();
}

void SmoothRegionMask(cv::Mat src, cv::Mat dst, int niter, int se_sz)
{
	cv::Mat se = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(se_sz * 2 + 1, se_sz * 2 + 1), cv::Point(se_sz, se_sz));
	for (int i = 0; i < niter; i++)
	{
		cv::morphologyEx(src, dst, cv::MORPH_CLOSE, se);
		cv::morphologyEx(src, dst, cv::MORPH_OPEN, se);
	}
}
*/



// compute vessel radius for all centerline points 
// which automaticall defines the vessel segmentation mask
//
// - by Soochahn Lee, 201612
// method: 
// - initially compute vessel region mask from graph cut using centerline points as seed points
// - apply thinning to computed region mask to get revised vessel centerline
// - vessel radius at centerline point is defined from value of 
//	 distance transform from region mask boundary
// - compute vessel centerline normal vectors by very simple approximated differential
// - vessel radii and normal vectors are stored within CVesSegm object
void ComputeVesselRadii(
	CVesSegm &vesSegm, // input (vessel centerline points) & output (radii)
	cv::Mat &frangiScale,
	cv::Mat &img, 
	bool bPR_BGD_is_NarrowBand, int bgd_nb_sz, int fgd_nb_sz)
{
	if (!vesSegm.bCompRadii) {
		// *** get vessel region mask *** //
		cv::Mat gc_mask = MakeRegionMask_GraphCut(vesSegm, frangiScale, img,
			bPR_BGD_is_NarrowBand, bgd_nb_sz, fgd_nb_sz);
		// compute distance transform within vessel region
		// *** important that mask has non-zero values for inside vessel and zero for outside vessel
		cv::Mat mask_dist, mask_dist_64f;
		cv::distanceTransform(gc_mask, mask_dist, CV_DIST_L2, cv::DIST_MASK_PRECISE);

		// get shortest path, on distance transform of obtained region boundary
		// update minimum values to avoid INF in FMM
		mask_dist.convertTo(mask_dist_64f, CV_64FC1);
		mask_dist_64f.setTo(1e-10, mask_dist_64f < 1e-10);
		// set starting and ending points
		cv::Point spt = vesSegm.front().pt;	// original start point
		cv::Point ept = vesSegm.back().pt;	// original end point
		double sptd[2]; sptd[0] = spt.x; sptd[1] = spt.y;
		double eptd[2]; eptd[0] = ept.x; eptd[1] = ept.y;
		double *S;
		cv::Mat D_mat;
		// perform fast-marching method on computed distance transform
		cFastMarching fmm;
		fmm.fast_marching(mask_dist_64f, mask_dist_64f.cols, mask_dist_64f.rows,
			sptd, 1, eptd, 1, 1000000, &D_mat, &S);
		// get shortest path
		std::vector<cv::Point> geo_path;
		fmm.compute_discrete_geodesic(D_mat, ept, &geo_path);
		// store shortest path in ves_pts_ref
		CVesSegm ves_pts_ref;
		ves_pts_ref.init_from_ptarr_rev(geo_path);
		// set vessel radii based on distance transform
		ves_pts_ref.set_radii(mask_dist);
		// smooth vessel radii
		ves_pts_ref.smooth_radii(5, 3);
#ifdef VERBOSE_COMP_VES_RADII
		// for verification
		cv::imwrite("gc_mask.png", gc_mask);
		cv::imwrite("gc_mask_dist.png", mask_dist);
		cv::Mat vsr_mask = cv::Mat::zeros(gc_mask.size(), CV_8UC3);
		ves_pts_ref.draw2mask(vsr_mask, cv::Scalar(255, 255, 255));
		cv::imwrite("vessegm_res_mask.png", vsr_mask);
		cv::Mat vsr_line = cv::Mat::zeros(gc_mask.size(), CV_8UC3);
		ves_pts_ref.drawline_8UC3(vsr_line, cv::Scalar(0, 0, 255));
		vesSegm.drawline_8UC3(vsr_line, cv::Scalar(0, 255, 0));
		cv::imwrite("vessegm_res_line.png", vsr_line);
#endif
		// store revised vessel contour
		vesSegm.copyFrom(ves_pts_ref);
	}
	// compute normal vector for all centerline points
	if (!vesSegm.bCompNormals)
		vesSegm.compute_normals(5);
#ifdef VERBOSE_COMP_VES_RADII
	// for verification
	cv::Mat vsr_mask = cv::Mat::zeros(img.size(), CV_8UC3);
	vesSegm.draw_normals(vsr_mask, 5, 20, cv::Scalar(255, 255, 0), 1);
	cv::imwrite("vessegm_nvecs.png", vsr_mask);
#endif

	/*
	if (!vesSegm.bCompRadii) {
		// *** get vessel region mask *** //
		cv::Mat gc_mask = MakeRegionMask_GraphCut(vesSegm, frangiScale, img,
			bPR_BGD_is_NarrowBand, bgd_nb_sz, fgd_nb_sz);
		cv::imwrite("grabcut_vessel_extraction.png", gc_mask);


		// *** get skeleton by morphological thinning *** //
		cP2pMatching proc_obj;
		cv::Mat thin_gc_mask;
		proc_obj.thin(gc_mask, thin_gc_mask);

		// *** refine vessel centerline based on vessel region segmentation 
		//	   effectively some smoothing results from this refinement *** //
		// first, construct vessel branch graph
		std::vector<cv::Point> junc_pts, end_pts; // junc_pts: array of junction points, end_pts: array of end points
		std::vector<std::vector<cv::Point>> vsegm2d; // vsegm2d: 2d array of all vessel segment points 
		cv::Mat ftype_flag_arr; // ftype_flag_arr: flag array for all n_feat feature (junction+end) points, true for junction
		cv::Mat vsegm_conn_mat;	// vsegm_conn_mat: n_segm * n_segm matrix representing connectivity between vessel segments
		proc_obj.MakeGraphFromImage(thin_gc_mask, junc_pts, end_pts,
			ftype_flag_arr, vsegm2d, vsegm_conn_mat);
		// next, examine branches and end points
		// if only 1 segment, use that segment directly
		CVesSegm ves_pts_ref;
		if (vsegm2d.size() == 1) {
			ves_pts_ref.init_from_ptarr(vsegm2d[0]);
		}
		// if more than 1 segment, 
		// extract single segment that starts and ends at (or close to) the original points
		else {
			// determine 2 end points that are closest to the original end points
			cv::Point ept_s = vesSegm.front().pt;	// original start point 1
			cv::Point ept_e = vesSegm.back().pt;		// original end point 1
			cv::Point ept_s_ref, ept_e_ref;			// refined end points
			// get points closest to original start and end points = refined start & end point
			double dsmin = FLT_MAX, demin = FLT_MAX;
			for (int i = 0; i < (int)end_pts.size(); i++) {
				double ds = cv::norm(end_pts[i] - ept_s);
				if (ds < dsmin) {
					dsmin = ds;
					ept_s_ref = end_pts[i];
				}
				double de = cv::norm(end_pts[i] - ept_e);
				if (de < demin) {
					demin = de;
					ept_e_ref = end_pts[i];
				}
			}
			// get shortest path, on current thinned mask, between two end points
			cFastMarching fmm;
			double s_pt[2]; s_pt[0] = ept_s_ref.x; s_pt[1] = ept_s_ref.y;
			double e_pt[2]; e_pt[0] = ept_e_ref.x; e_pt[1] = ept_e_ref.y;
			double *S;
			cv::Mat D_mat, thin_gc_mask_64f;
			thin_gc_mask.convertTo(thin_gc_mask_64f, CV_64FC1, 1.0 / 255.0);
			// *must dilate thin path because FMM is 4-neighbor based
			for (int n = 0; n < (int)vsegm2d.size(); n++)
			for (int i = 0; i < (int)vsegm2d[n].size(); i++)
				cv::circle(thin_gc_mask_64f, vsegm2d[n][i], 3, cv::Scalar(1.0), -1);
			// update minimum values to avoid INF in FMM
			thin_gc_mask_64f.setTo(1e-10, thin_gc_mask_64f < 1e-10);
			// perform fast-marching method
			fmm.fast_marching(thin_gc_mask_64f, thin_gc_mask.cols, thin_gc_mask.rows,
				s_pt, 1, e_pt, 1, 1000000, &D_mat, &S);
			// get shortest path
			std::vector<cv::Point> geo_path;
			fmm.compute_discrete_geodesic(D_mat, ept_e_ref, &geo_path);
			// store shortest path in ves_pts_ref
			ves_pts_ref.init_from_ptarr_rev(geo_path);
		}
		// determine vessel radii based on distance transform
		// *** important that mask has non-zero values for inside vessel and zero for outside vessel
		cv::Mat mask_dist;
		cv::distanceTransform(gc_mask, mask_dist, CV_DIST_L2, cv::DIST_MASK_PRECISE);
		ves_pts_ref.set_radii(mask_dist);
		// smooth vessel radii
		ves_pts_ref.smooth_radii(5, 3);
		vesSegm.copyFrom(ves_pts_ref);
	}
	// compute normal vector for all centerline points
	if (!vesSegm.bCompNormals)
		vesSegm.compute_normals(5);
	*/
}