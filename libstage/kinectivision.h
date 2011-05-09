#if !defined(KINECTIVISION_H_367XGTMJ)
#define KINECTIVISION_H_367XGTMJ

#include "sonde_environnementale.h"
#include "reactivision.h"
#include <opencv/cv.h>
#include <vector>

struct kinectivision : public sonde_environnementale {
	kinectivision(cv::Mat&);
	virtual ~kinectivision();
	void operator()(objet_store* store = 0);
	cv::Mat& image() { return image_processed_; }
private:
	cv::Mat image_couleur_, image_nb_/*, image_nb2_*/, image_processed_;
	reactivision_settings config_;
	std::vector<FrameProcessor*> processors_;
	MessageServer* server_;
	FrameProcessor* fiducialfinder_, *thresholder_, *equalizer_, *calibrator_;
};

#endif /* end of include guard: KINECTIVISION_H_367XGTMJ */
