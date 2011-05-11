#if !defined(ARKINECTOOLKITPLUS_H_49QQK61C)
#define ARKINECTOOLKITPLUS_H_49QQK61C

#include "sondes/sonde_environnementale.h"
#include <opencv/cv.h>
#include <vector>

class kinect;
namespace ARToolKitPlus {
	class TrackerSingleMarker;
}

struct artoolkitplus : public sonde_primaire {
	artoolkitplus(cv::Mat&);
	virtual ~artoolkitplus();
	void operator()(objet_store* store = 0);
	cv::Mat& image() { return image_nb_; }
private:
	cv::Mat& image_couleur_;
	cv::Mat image_nb_, image_processed_;
	ARToolKitPlus::TrackerSingleMarker *tracker_;
};

#endif /* end of include guard: ARKINECTOOLKITPLUS_H_49QQK61C */
