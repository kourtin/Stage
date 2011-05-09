#include "kinectivision.h"

#include "kinect.h"
#include "objet_store.h"

void miniaturiser2(cv::Mat original, cv::Mat reduit, unsigned int facteur = 2) {
	int y = 0;
	for(int i = 0; i < original.rows; i += facteur) {
		int x = 0;
		for(int j = 0; j < original.cols; j += facteur) {
			unsigned char v = original.at<unsigned char>(i,j);
			reduit.at<unsigned char>(y,x) = v;
			++x;
		}
		++y;
	}
}

kinectivision::kinectivision(cv::Mat& img) : image_couleur_(img), image_nb_(cv::Size(640, 480), CV_8UC1)/*, image_nb2_(cv::Size(320, 240), CV_8UC1)*/, image_processed_(cv::Size(640, 480), CV_8UC1) {
	sprintf(config_.file,"none");
	readSettings(&config_);
	
	server_ = 0;
	// server_ = new TuioServer(config_.host,config_.port);
	// server_->setInversion(config_.invert_x, config_.invert_y, config_.invert_a);
	
	if (!config_.dtouch) {
		equalizer_ = new FrameEqualizer();
		thresholder_ = new FrameThresholder(config_.gradient_gate);
		if (config_.amoeba) fiducialfinder_ = new FidtrackFinder(server_, config_.tree_config, config_.grid_config, config_.finger_size, config_.finger_sensitivity);
		else if (config_.classic) fiducialfinder_ = new FidtrackFinderClassic(server_, config_.grid_config);
		processors_.push_back(equalizer_);
		processors_.push_back(thresholder_);
	} else {
		fiducialfinder_ = new DtouchFinder(server_, config_.grid_config);
	}
	
	processors_.push_back(fiducialfinder_);
	
	calibrator_ = new CalibrationEngine(config_.grid_config);
	processors_.push_back(calibrator_);
	
	// Initialise les processeurs
	for(std::vector<FrameProcessor*>::iterator it = processors_.begin(); it != processors_.end();) {
		bool success = (*it)->init(640, 480, 1, 1);
		if(!success) {	
			processors_.erase(it);
		} else it++;
	}
}

kinectivision::~kinectivision() {
	// Finit les processors
	for(std::vector<FrameProcessor*>::iterator it = processors_.begin(); it != processors_.end(); ++it)
		(*it)->finish();
	
	if(calibrator_)
	delete calibrator_;
	if(thresholder_)
	delete thresholder_;
	if(equalizer_)
	delete equalizer_;
	if(fiducialfinder_)
	delete fiducialfinder_;
	if(server_)
	delete server_;
}

void kinectivision::operator()(objet_store* store) {
	cv::cvtColor(image_couleur_, image_nb_, CV_BGR2GRAY);
	// unsigned char* src = image_couleur_.data;
	// 		unsigned char* dest = image_nb_.data;
	// 		for(int i=0; i<640*480; ++i) {
	// 			float r = *src++ * 1.0 / 255;
	// 			float g = *src++ * 1.0 / 255;
	// 			float b = *src++ * 1.0 / 255;
	// 			float gr = 0.2989 * r + 0.5870 * g + 0.1140 * b;
	// 			*dest++ = static_cast<unsigned char>(gr / 1.0 * 255);
	// 		}
	// miniaturiser2(image_nb_, image_nb2_);
	// std::copy(image_nb2_.data, image_nb2_.data + image_nb2_.cols * image_nb2_.rows, image_processed_.data);
	// Processe
	for(std::vector<FrameProcessor*>::iterator it = processors_.begin(); it != processors_.end(); ++it) {
		(*it)->process(image_nb_.data,image_processed_.data);
	}
	
	// Envoie les infos au store
	if(store)
		if(fiducialfinder_)
			((FiducialFinder*)fiducialfinder_)->update(store);
}