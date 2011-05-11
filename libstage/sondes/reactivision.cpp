#if defined(COMPILES_WITH_LIB_reactivision)

#include "sondes/reactivision.h"
#include "objets/objet_store.h"

reactivision::reactivision(cv::Mat& img) : image_couleur_(img), image_nb_(cv::Size(640, 480), CV_8UC1), image_processed_(cv::Size(640, 480), CV_8UC1) {
	sprintf(config_.file,"none");
	readSettings(&config_);
	
	server_ = 0;
	
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

reactivision::~reactivision() {
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

void reactivision::operator()(objet_store* store) {
	if(!store) return;
	
	cv::cvtColor(image_couleur_, image_nb_, CV_BGR2GRAY);
	
	// Processe
	for(std::vector<FrameProcessor*>::iterator it = processors_.begin(); it != processors_.end(); ++it) {
		(*it)->process(image_nb_.data,image_processed_.data);
	}
	
	// Envoie les infos au store
	if(store)
		if(fiducialfinder_)
			((FiducialFinder*)fiducialfinder_)->update(store);
}

#endif