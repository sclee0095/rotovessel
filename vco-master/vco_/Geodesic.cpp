

#include "Geodesic.h"
//#include <vld.h> 
#if !defined(_AFXDLL)
#include <windows.h>
#include <crtdbg.h>
#if defined(DEBUG) | defined(_DEBUG)
#if !defined(DEBUG_NEW)
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif
#endif 

cGeodesic::cGeodesic()
{
}


cGeodesic::~cGeodesic()
{
}


//Node::Node(int to, float w) : to(to), w(w) {
//}
//bool Node::operator<(const Node &o) const {
//	return w < o.w;
//}
//bool Node::operator>(const Node &o) const {
//	return w > o.w;
//}
//Node2::Node2(int to, int from, float w) : to(to), from(from), w(w) {
//}
//bool Node2::operator<(const Node2 &o) const {
//	return w < o.w;
//}
//bool Node2::operator>(const Node2 &o) const {
//	return w > o.w;
//}
//GeodesicDistance::GeodesicDistance(const OverSegmentation & os) : GeodesicDistance(os.edges(), os.edgeWeights()) {
//}
//GeodesicDistance::GeodesicDistance(const Edges &edges, const VectorXf &edge_weights) : N_(getN(edges)) {
//	adj_list_.resize(N_);
//	for (int i = 0; i<edges.size(); i++) {
//		adj_list_[edges[i].a].push_back(Node(edges[i].b, edge_weights[i]));
//		adj_list_[edges[i].b].push_back(Node(edges[i].a, edge_weights[i]));
//	}
//	reset();
//}
//GeodesicDistance& GeodesicDistance::reset(float v) {
//	d_ = v * VectorXf::Ones(N_);
//	return *this;
//}
//GeodesicDistance& GeodesicDistance::update(int nd, float v) {
//	PQ q;
//	q.push(Node(nd, v));
//	updatePQ(d_, q);
//	return *this;
//}
//GeodesicDistance& GeodesicDistance::update(const VectorXf &new_min) {
//	PQ q;
//	for (int i = 0; i<N_; i++)
//	if (new_min[i] < d_[i])
//		q.push(Node(i, new_min[i]));
//	updatePQ(d_, q);
//	return *this;
//}
//void GeodesicDistance::updatePQ(VectorXf & d, GeodesicDistance::PQ &q) const {
//	const float EPS = 1e-6;
//	while (!q.empty()) {
//		Node n = q.top();
//		q.pop();
//
//		if (d[n.to] <= n.w)
//			continue;
//
//		d[n.to] = n.w;
//		for (const Node & i : adj_list_[n.to]) {
//			float w = n.w + i.w;
//			if (w < d[i.to]) {
//				q.push(Node(i.to, w));
//				d[i.to] = w*(1 + EPS) + 1e-20;
//			}
//		}
//	}
//}
//int GeodesicDistance::N() const {
//	return N_;
//}
//const VectorXf &GeodesicDistance::d() const {
//	return d_;
//}
//VectorXf GeodesicDistance::compute(int nd) const {
//	PQ q;
//	q.push(Node(nd, 0));
//	VectorXf d = 1e10*VectorXf::Ones(N_);
//	updatePQ(d, q);
//	return d;
//}
//VectorXf GeodesicDistance::compute(const VectorXf &start) const {
//	PQ q;
//	for (int i = 0; i<N_; i++)
//	if (start[i] < 1e5)
//		q.push(Node(i, start[i]));
//	VectorXf d = 1e10*VectorXf::Ones(N_);
//	updatePQ(d, q);
//	return d;
//}
//RMatrixXf GeodesicDistance::compute(const RMatrixXf &start) const {
//	RMatrixXf r = 0 * start;
//	for (int i = 0; i<start.cols(); i++)
//		r.col(i) = compute((VectorXf)start.col(i));
//	return r;
//}
//VectorXf GeodesicDistance::backPropGradient(int nd, const VectorXf &g) const {
//	PQ2 q;
//	q.push(Node2(nd, nd, 0));
//	return backPropGradientPQ(q, g);
//}
//VectorXf GeodesicDistance::backPropGradient(const VectorXf &start, const VectorXf &g) const {
//	PQ2 q;
//	for (int i = 0; i<N_; i++)
//	if (start[i] < 1e5)
//		q.push(Node2(i, i, start[i]));
//	return backPropGradientPQ(q, g);
//}
//VectorXf GeodesicDistance::backPropGradientPQ(PQ2 & q, const VectorXf &g) const {
//	// Compute the shortest path
//	const float EPS = 1e-6;
//	VectorXi p = -VectorXi::Ones(N_);
//	VectorXf d = 1e10*VectorXf::Ones(N_);
//	while (!q.empty()) {
//		Node2 n = q.top();
//		q.pop();
//
//		if (p[n.to] != -1)
//			continue;
//
//		p[n.to] = n.from;
//		d[n.to] = n.w;
//
//		for (const Node & i : adj_list_[n.to]) {
//			float w = n.w + i.w;
//			if (w < d[i.to]) {
//				q.push(Node2(i.to, n.from, w));
//				d[i.to] = w*(1 + EPS) + 1e-20;
//			}
//		}
//	}
//	// And backprop
//	VectorXf r = VectorXf::Zero(N_);
//	for (int i = 0; i<N_; i++)
//		r[p[i]] += g[i];
//	return r;
//}
//
//int geodesicCenter(const Edges &edges, const VectorXf &edge_weights) {
//	// Find the geodesic center of a graph
//	GeodesicDistance gdist(edges, edge_weights);
//	// First find a few corners of the graph (a point with the maximal distance to any other point)
//	ArrayXf max_geodesic = ArrayXf::Zero(gdist.N());
//	// 3 iterations should be enough, but you can never be paranoid enough
//	int seed = 0;
//	for (int it = 0; it<3; it++) {
//		// Update the geodesic from the new seed
//		ArrayXf d = gdist.compute(seed);
//		max_geodesic = d.max(max_geodesic);
//		d.maxCoeff(&seed);
//	}
//	// The center is the point with the minimal max_geodesic (distance to the side of the graph)
//	int center;
//	max_geodesic.minCoeff(&center);
//	return center;
//}
//

