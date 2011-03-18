#if !defined(OBJECT_TRACKER_H_997320YD)
#define OBJECT_TRACKER_H_997320YD

#include "objet.h"
#include "blobby.h"
#include <opencv/cv.h>
#include <cvblob.h>
#define KINECT_MAX_DEPTH 2048

struct object_tracker {
	enum { max_segments = 15 };
	object_tracker(cv::Size);
	virtual ~object_tracker();
	void process(cv::Mat& m);
	
	size_t taille_median() { return taille_median_; }
	void taille_median(size_t t);
	double& bandwidth() { return bandwidth_; }
	size_t increment() { return increment_; }
	void increment(size_t);
	cv::Mat& m_tmp1() { return m_tmp1_; }
	cv::Mat& m_8bit() { return m_8bit_; }
	std::vector<blobby>& blobs() { return blobs_; }
	std::vector<float>& kde() { return kde_; }
	std::vector<float>& histogramme() { return h_; }
	std::vector<int>& segments() { return segments_; }
	std::vector<cv::Mat>& segments_nb() { return segments_nb_; }
	std::vector<objet> objets_;
private:
	cv::Mat m_8bit_, m_tmp1_, blobby_;
	size_t taille_median_, increment_;
	double bandwidth_;
	std::vector<float> kde_, h_;
	std::vector<int> segments_;
	std::vector<cv::Mat> segments_nb_;
	std::vector<blobby> blobs_;
};

#endif /* end of include guard: OBJECT_TRACKER_H_997320YD */
