#if !defined(BLOBBY_H_MO5KH1HV)
#define BLOBBY_H_MO5KH1HV

#include <cinder/Rect.h>
#include <cinder/Vector.h>
#include <opencv/cv.h>
#include <cvblob.h>

struct blobby {
	blobby(unsigned int section, unsigned int label, cvb::CvBlob& blob, IplImage* img_lbl, cv::Mat& img_ref);
	virtual ~blobby();
	
	bool valide();
	float aire() { return aire_; }
	ci::Rectf rect() { return rect_; }
	ci::Vec2f centroide() { return centroide_; }
	unsigned int section() { return section_; }
	unsigned int label() { return label_; }
	void label(unsigned int i) { label_ = i; }
	float angle() { return angle_; }
	float distance(blobby&);
	float zmin() { return zmin_; }
	float zmax() { return zmax_; }
	
	static long int next_id() {
		static long int id = 0;
		return id++;
	}
	
	long int id_;
private:
	unsigned int section_;
	unsigned int label_;
	float aire_;
	ci::Rectf rect_;
	ci::Vec2f centroide_;
	float angle_;
	std::vector<float> pdf_x_, pdf_y_, pdf_z_;
	float zmin_, zmax_;
};

#endif /* end of include guard: BLOBBY_H_MO5KH1HV */
