#include "blobby.h"
#include <cmath>

blobby::blobby(unsigned int section, unsigned int label, cvb::CvBlob& blob, IplImage* img_lbl, cv::Mat& img_ref) : id_(-1), section_(section), label_(label), aire_(blob.area), rect_(blob.minx, blob.miny, blob.maxx, blob.maxy), centroide_(blob.centroid.x, blob.centroid.y), angle_(cvb::cvAngle(&blob)), pdf_x_(img_lbl->width), pdf_y_(img_lbl->height), pdf_z_(255), zmin_(5000.0f), zmax_(0.0f) {
	// Calcul de la pdf des positions en x, y et z.
	int n = 0;
	for(size_t i=0; i<pdf_y_.size(); ++i) {
		for(size_t j=0; j<pdf_x_.size(); ++j) {
			if(cvb::cvGetLabel(img_lbl, j, i) == blob.label) {
				size_t z = img_ref.at<unsigned char>(i, j);
				if(z == 255) continue;
				++n;
				++pdf_x_[j];
				++pdf_y_[i];
				++pdf_z_[z];
				float zf = z * 1.0f/255;
				if(zf > zmax_)
					zmax_ = zf;
				if(zf < zmin_)
					zmin_ = zf;
			}
		}
	}
	if(zmax_ > 1)
		zmax_ = 1;
	if(zmin_ < 0)
		zmin_ = 0;
	if(zmin_ > 1.0f)
		return;
	// Normalisation des pdf
	for(size_t i=0; i<std::max(pdf_x_.size(), std::max(pdf_y_.size(), pdf_z_.size())); ++i) {
		if(i < pdf_x_.size())
			pdf_x_[i] /= n;
		if(i < pdf_y_.size())
			pdf_y_[i] /= n;
		if(i < pdf_z_.size())
			pdf_z_[i] /= n;
	}
}

blobby::~blobby() {}

bool blobby::valide() {
	bool aire_minimale = aire_ > 5 && aire_ < 100;
	bool ratio_correct = true;//rect_.getAspectRatio() > 2 || rect_.getAspectRatio() > 1.0/2.0;
	return zmin_ <= 1.0f && aire_minimale && ratio_correct;
}

float blobby::distance(blobby& b) {
	float d_x = 0, d_y = 0, d_z = 0;
	for(size_t i=0; i<pdf_x_.size(); ++i)
		d_x += (pdf_x_[i] - b.pdf_x_[i]) * (pdf_x_[i] - b.pdf_x_[i]);
	for(size_t i=0; i<pdf_y_.size(); ++i)
		d_y += (pdf_y_[i] - b.pdf_y_[i]) * (pdf_y_[i] - b.pdf_y_[i]);
	for(size_t i=0; i<pdf_z_.size(); ++i)
		d_z += (pdf_z_[i] - b.pdf_z_[i]) * (pdf_z_[i] - b.pdf_z_[i]);
	return sqrt(d_x + d_y + d_z);
}