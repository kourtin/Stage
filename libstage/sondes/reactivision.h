#if !defined(reactivision_H_367XGTMJ)
#define reactivision_H_367XGTMJ

#include "sondes/sonde_environnementale.h"
#include "sondes/reactivision_settings.h"
#include <opencv/cv.h>
#include <vector>

struct reactivision : public sonde_primaire {
	reactivision(cv::Mat&);
	virtual ~reactivision();
	void operator()(objet_store* store = 0);
	cv::Mat& image() { return image_processed_; }
private:
	cv::Mat& image_couleur_;
	cv::Mat image_nb_/*, image_nb2_*/, image_processed_;
	reactivision_settings config_;
	std::vector<FrameProcessor*> processors_;
	MessageServer* server_;
	FrameProcessor* fiducialfinder_, *thresholder_, *equalizer_, *calibrator_;
};

#endif /* end of include guard: reactivision_H_367XGTMJ */
