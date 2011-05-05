#include "kinect.h"
#include "object_tracker.h"

#include "CinderOpenCv.h"

#include <iostream>
#include <opencv/cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <sstream>
#include <string>

#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/params/Params.h>
#include <cinder/ip/Fill.h>
#include <cinder/Camera.h>


using namespace ci;
using namespace ci::app;

struct tracking : public AppBasic {
	tracking();
	void setup();
	void update();
	void draw();
	void prepareSettings(Settings*);
	void mouseWheel(MouseEvent);
	void mouseDown(MouseEvent);
	void mouseUp(MouseEvent);
	void mouseDrag(MouseEvent);
private:
	gl::Texture image_;
	Surface r_;
	cv::Mat dipah_;
	Freenect::Freenect freenect_;
	kinect kinect_;
	object_tracker tracker_;
	float mouse_;
	int seg_;
	CameraPersp cam_;
	Quatf rotation_;
	Vec3f eye_, centre_, up_;
	params::InterfaceGl params_;
	float distance_cam_;
};

tracking::tracking() : dipah_(cv::Size(640, 480), CV_16UC1), tracker_(cv::Size(640, 480)), mouse_(-1), seg_(-1), centre_(Vec3f::zero()), up_(Vec3f::yAxis()) {}

void tracking::prepareSettings(Settings* settings) {
	settings->setWindowSize(640, 480);
	settings->setFrameRate(60.f);
	settings->setTitle("Coucou");
}

void tracking::setup() {
	r_ = Surface(getWindowWidth(), getWindowHeight(), false);
	image_ = ci::gl::Texture(r_);
	// gl::enableAlphaBlending();
	
	rotation_ = Quatf(0.0, 0.5, 0.0, 1.0);
	distance_cam_ = 400.0f;
	eye_ = Vec3f(0.0f, 0.0f, -distance_cam_);
	cam_.setPerspective(75.0f, getWindowAspectRatio(), 5.0f, 2000.0f);
	params_ = params::InterfaceGl("Tracking", Vec2i(225, 200));
	params_.addParam("Rotation", &rotation_);
	params_.addSeparator();
	params_.addParam("Distance", &distance_cam_, "min=50.0 max=1500.0 step=50.0 keyIncr=s keyDecr=w");
	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void tracking::update() {
	if(kinect_.depth_to(dipah_)) {
		// cv::flip(dipah_, dipah_, 1);
		tracker_.process(dipah_);
		if(seg_ == -1) {
			image_ = ci::gl::Texture(fromOcv(tracker_.m_tmp1()));
		}
		else
			image_ = ci::gl::Texture(fromOcv(tracker_.segments_nb()[seg_]));
	}
}

void tracking::draw() {
	gl::clear(Color(0.f, 0.f, 0.f));
	gl::color(Color(1.0, 1.0, 1.0));
	gl::disableDepthWrite();
	gl::disableDepthRead();
	// if(seg_ == -1)
	gl::draw(image_, getWindowBounds());
	eye_ = Vec3f(0.0f, 0.0f, distance_cam_);
	// cam_.lookAt(eye_, centre_, up_);
	// gl::setMatrices(cam_);
	// gl::rotate(Vec3f(0.0, 180.0, 0.0));
	// gl::rotate(rotation_);
	// gl::translate(Vec3f(-getWindowWidth() / 2, -getWindowHeight() / 2, 0));
	// else
		// gl::draw(image_, Area(0,0,getWindowWidth() * 1.0 / tracker_.increment(), getWindowHeight() * 1.0 / tracker_.increment()));
		
	if(seg_ >= 0) {
		std::ostringstream oss;
		oss << "Segment " << seg_ << " / " << tracker_.segments().size();
		gl::drawString(oss.str(), Vec2f(500, 10));
	}
		
	
	// Affichage infos
	std::ostringstream oss;
	oss << "Decimation : " << tracker_.increment() << " - KDE bandwidth : " << tracker_.bandwidth() << " - Filtre median : " << tracker_.taille_median() << "px - " << getAverageFps() << "fps";
	gl::drawString(oss.str(), Vec2f(10, 10));
	
	// Affichage histogramme
	for(size_t i=0; i<tracker_.kde().size(); ++i) {
		float x = i * 1.0 / tracker_.kde().size() * getWindowWidth();
		float y = 480 * 10 * (tracker_.histogramme()[i]);
		gl::color(ColorA(0.0, 0.0, 1.0, 0.3));
		gl::drawLine(Vec2f(x, 480), Vec2f(x, 480 - y));
		gl::color(ColorA(1.0, 0.0, 0.0, 0.8));
		gl::drawSolidCircle(Vec2f(x, 480 - 480*10*(tracker_.kde()[i])), 2.0f);
		if(mouse_ != -1 && i == static_cast<size_t>(mouse_ * 255)) {
			std::ostringstream oss;
			oss << i;
			gl::drawString(oss.str(), Vec2f(x, 400));
		}
	}
	
	// 
	// Affichage segments
	gl::color(ColorA(1.0, 0.0, 1.0, 0.5));
	for(std::vector<int>::iterator it = tracker_.segments().begin(); it != tracker_.segments().end(); ++it) {
		int i = *it;
		float x = i * 1.0 / tracker_.kde().size() * getWindowWidth();
		gl::drawLine(Vec2f(x, 480), Vec2f(x, 400));
	}
	// return;
	// Affichage blobs
	int i=0;
	for (std::vector<blobby>::iterator it = tracker_.blobs().begin(); it != tracker_.blobs().end(); ++it) {
		if(seg_ == -1 || (seg_ >= 0 && it->section() == seg_)) {
			float c = (i * 1.0 / tracker_.blobs().size());
			gl::color(ColorA(1.0 - c, 0.0, 0.0, 1.0));
			std::ostringstream oss2;
			// oss2 << "" << (int)((it->zmax() - it->zmin())*255);
			oss2 << "" << (long int)(it->id_);
			Vec2f centroide = it->centroide() * tracker_.increment();
			// gl::drawSolidCircle(centroide, 4.0f);
			gl::drawString(oss2.str(), centroide);
			const Rectf r = it->rect().scaled(tracker_.increment());
			gl::drawLine(r.getUpperLeft(), r.getUpperRight());
			gl::drawLine(r.getLowerLeft(), r.getLowerRight());
			gl::drawLine(r.getUpperLeft(), r.getLowerLeft());
			gl::drawLine(r.getUpperRight(), r.getLowerRight());
			
			// // gl::drawSphere(Vec3f(centroide.x, centroide.y, 1.0 / it->zmin() * -10), 10.0);
			// gl::pushMatrices();
			// gl::translate(Vec3f(centroide.x, centroide.y, 1.0 / it->zmin() *-10.0));
			// gl::rotate(Quatf(1.0, 0.0, 0.0, it->angle()));
			// gl::drawCube(Vec3f(0,0,0), Vec3f(r.getWidth(), r.getHeight(), (it->zmax() - it->zmin())*100));
			// gl::drawCube(Vec3f(centroide.x, centroide.y, 1.0 / it->zmin() *-10.0), Vec3f(r.getWidth(), r.getHeight(), (it->zmax() - it->zmin())*100));
			// gl::popMatrices();
			// gl::color(Color(1.0 - c, 1.0, c));
			// float l = std::max(r.x2 - r.x1, r.y2 - r.y1) / 2;
			// gl::drawLine(Vec2f(centroide.x + l*cos(it->angle()), centroide.y + l*sin(it->angle())), Vec2f(centroide.x - l*cos(it->angle()), centroide.y - l*sin(it->angle())));
			++i;
		}
	}
	
	// Affichage objets
	for(std::vector<objet>::iterator it = tracker_.objets_.begin(); it != tracker_.objets_.end(); ++it) {
		blobby& b = it->blob();
		Vec2f centroide = b.centroide() * tracker_.increment();
		const Rectf r = b.rect().scaled(tracker_.increment());
		if(it->present())
			gl::color(ColorA(0.0, 0.5, 0.0, 0.5));
		else
			gl::color(ColorA(1.0, 0.0, 0.0, 0.5));
		gl::drawSolidCircle(centroide, 10.0f);
		gl::drawString(it->nom(), centroide);
		gl::pushMatrices();
		gl::translate(Vec3f(centroide.x, centroide.y, 1.0 / b.zmin() *-10.0));
		// std::cout << (b.zmax() - b.zmin())*500 << std::endl;
		// gl::rotate(Quatf(1.0, 0.0, 0.0, it->angle()));
		gl::drawCube(Vec3f(0,0,0), Vec3f(r.getWidth(), r.getHeight(), (b.zmax() - b.zmin())*500));
		gl::popMatrices();
	}
	
	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	
	// params::InterfaceGl::draw();
}

void tracking::mouseWheel(MouseEvent e) {
	if(e.isShiftDown()) {
		size_t t = tracker_.taille_median();
		if(e.getWheelIncrement() > 0)
			tracker_.taille_median(t + 2);
		else if(e.getWheelIncrement() < 0)
			tracker_.taille_median(t - 2);
	} else if(e.isMetaDown()) {
		if(e.getWheelIncrement() > 0)
			tracker_.increment(tracker_.increment()+1);
		else if(e.getWheelIncrement() < 0)
			tracker_.increment(tracker_.increment()-1);
	} else if(e.isAltDown()) {
		if(e.getWheelIncrement() > 0 && seg_ < tracker_.max_segments)
			++seg_;
		else if(e.getWheelIncrement() < 0 && seg_ > -1)
			--seg_;
	} else {
		if(e.getWheelIncrement() > 0)
			tracker_.bandwidth()=std::min(20.0, tracker_.bandwidth()+0.1);
		else if(e.getWheelIncrement() < 0)
			tracker_.bandwidth()=std::max(0.1, tracker_.bandwidth()-0.1);
	}
}

void tracking::mouseDown(MouseEvent e) {
	mouse_ = e.getX() * 1.0 / getWindowWidth();
}

void tracking::mouseDrag(MouseEvent e) {
	mouse_ = e.getX() * 1.0 / getWindowWidth();
}

void tracking::mouseUp(MouseEvent e) {
	mouse_ = -1.0;
	blobby* b = 0;
	for(size_t i=0; i<tracker_.blobs().size(); ++i) {
		if(tracker_.blobs()[i].rect().scaled(tracker_.increment()).contains(e.getPos())) {
			b = &tracker_.blobs()[i];
			break;
		}
	}
	if(b != 0) {
		tracker_.objets_.push_back(objet(*b));
	}
}

// int main() {
// 	bool meurs(false);
// 	cv::Mat depth(cv::Size(640, 480), CV_16UC1), depth_noiretblanc(cv::Size(640, 480), CV_8UC3);
// 	
// 	Freenect::Freenect freenect;
// 	kinect& k = freenect.createDevice<kinect>(0);
// 	
// 	cv::namedWindow("dipah", CV_WINDOW_AUTOSIZE);
// 	k.startDepth();
// 	
// 	std::clock_t c = std::clock();
// 	// for(int i=0; i<30;) {
// 	double fps = 0;
// 	int i = 0;
// 	while(!meurs) {
// 		if(k.prochaine_depth_map(depth)) {
// 			depth.convertTo(depth_noiretblanc, CV_8UC1, 255.0/2048.0);
// 			cv::medianBlur(depth_noiretblanc, depth, 101);
// 			cv::imshow("dipah", depth);
// 			++i;
// 			fps += (std::clock() - c) * 1000.0 / CLOCKS_PER_SEC;
// 			c = std::clock();
// 			std::cout << (fps / i) << "fps" << std::endl;
// 		}	
// 		if(cvWaitKey(5) == 27)
// 			meurs = true;
// 	}
// 	std::cout << (fps / i) << "fps" << std::endl;
// 	// std::cout << (std::clock() - c) << std::endl;
// 	cvDestroyWindow("dipah");
// 	k.stopDepth();
// 	return 0;
// }

CINDER_APP_BASIC(tracking, RendererGl)
