#include <XnCppWrapper.h>
#include <iostream>

int main() {
	xn::Context contexte;
	
	XnStatus etat_erreur = XN_STATUS_OK;
	etat_erreur = contexte.Init();
	if(etat_erreur != XN_STATUS_OK) {
		std::cerr << "Erreur: " << xnGetStatusString(etat_erreur) << std::endl;
		exit(etat_erreur);
	}
	
	xn::DepthGenerator depth;
	etat_erreur = depth.Create(contexte);
	if(etat_erreur != XN_STATUS_OK) {
		std::cerr << "Erreur: " << xnGetStatusString(etat_erreur) << std::endl;
		exit(etat_erreur);
	}
	
	XnMapOutputMode mode_map;
	mode_map.nXRes = XN_VGA_X_RES;
	mode_map.nYRes = XN_VGA_Y_RES;
	mode_map.nFPS = 30;
	etat_erreur = depth.SetMapOutputMode(mode_map);
	if(etat_erreur != XN_STATUS_OK) {
		std::cerr << "Erreur: " << xnGetStatusString(etat_erreur) << std::endl;
		exit(etat_erreur);
	}
	
	etat_erreur = contexte.StartGeneratingAll();
	if(etat_erreur != XN_STATUS_OK) {
		std::cerr << "Erreur: " << xnGetStatusString(etat_erreur) << std::endl;
		exit(etat_erreur);
	}
	
	XnUInt32 nMiddleIndex =
	XN_VGA_X_RES * XN_VGA_Y_RES/2 + // start of middle line
	XN_VGA_X_RES/2;
	
	bool executer = true;
	while(executer) {
		etat_erreur = contexte.WaitOneUpdateAll(depth);
		if(etat_erreur != XN_STATUS_OK) {
			std::cerr << "Erreur: " << xnGetStatusString(etat_erreur) << std::endl;
			continue;
		}
		
		const XnDepthPixel* depth_map = depth.GetDepthMap();
		printf("Middle pixel is %u millimeters away\n",
			depth_map[nMiddleIndex]);
	}
	
	contexte.Shutdown();
}