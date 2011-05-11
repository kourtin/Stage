#if !defined(KINECT_SEGMENTOR_H_9R4T99LF)
#define KINECT_SEGMENTOR_H_9R4T99LF

#include "sondes/sonde_environnementale.h"
#include <opencv/cv.h>
#include <cinder/Rect.h>

#include "objets/objet.h"

struct blobbos {
	ci::Rectf rect;
	int id;
};

struct depth_segmentor : public sonde_secondaire {
	depth_segmentor(cv::Mat&);
	virtual ~depth_segmentor();
	void init_segments();
	void operator()(objet_store* store = 0);
	cv::Mat& image() { *segments_[0]; return petite_image_; }
	typedef std::vector<cv::Mat*> segment_liste;
	unsigned int downscale_kde_, downscale_;
	cv::Mat& depth_map_;
	cv::Mat image_depth_/*, image_depth2_*/, petite_image_/*, image_tmp_*/;
	// IplImage image_tmp_ipl_, *label_img_;
	float median_size_;
	float kernel_bandwidth_;
	segment_liste segments_;
	std::vector<blobbos> blobs_;
private:
};

#endif /* end of include guard: KINECT_SEGMENTOR_H_9R4T99LF */
