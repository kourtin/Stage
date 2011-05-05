#if !defined(REACTIVISION_H_GDR14SYB)
#define REACTIVISION_H_GDR14SYB

#include "FrameEqualizer.h"
#include "FrameThresholder.h"
#include "FidtrackFinder.h"
#include "FidtrackFinderClassic.h"
#include "DtouchFinder.h"
#include "CalibrationEngine.h"
#include <opencv/cv.h>

struct reactivision_settings {
	char file[1024];
	int port;
	char host[1024];
	char tree_config[1024];
	char grid_config[1024];
	char midi_config[1024];
	char camera_config[1024];
	bool invert_x;
	bool invert_y;
	bool invert_a;
	bool midi;
	bool amoeba;
	bool classic;
	bool dtouch;
	bool background;
	int finger_size;
	int finger_sensitivity;	
	int gradient_gate;	
	int display_mode;
};

void readSettings(reactivision_settings *config);
void writeSettings(reactivision_settings *config);

#endif /* end of include guard: REACTIVISION_H_GDR14SYB */
