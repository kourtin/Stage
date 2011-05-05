#include "reactivision.h"
#include "tinyxml.h"

void readSettings(reactivision_settings *config) {
	
	config->port = 3333;
	sprintf(config->host,"localhost");
	sprintf(config->tree_config,"none");
	sprintf(config->grid_config,"none");
	sprintf(config->midi_config,"none");
	sprintf(config->camera_config,"none");
	config->invert_x = false;
	config->invert_y = false;
	config->invert_a = false;
	config->midi = false;
	config->amoeba = true;
	config->classic = false;
	config->dtouch = false;
	config->background = false;
	config->finger_size = 0;
	config->finger_sensitivity = 100;	
	config->gradient_gate = 32;	
	config->display_mode = 2;
	
	if (strcmp( config->file, "none" ) == 0) {
	char app_path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	CFStringGetCString( cfStringRef, app_path, 1024, kCFStringEncodingASCII);	
	CFRelease( mainBundleURL);
	CFRelease( cfStringRef);
	sprintf(config->file,"%s/Contents/Resources/reacTIVision.xml",app_path);
	}

	TiXml::TiXmlDocument xml_settings( config->file );
	xml_settings.LoadFile();
	if( xml_settings.Error() )
	{
		std::cout << "Error loading configuration file: " << config->file << std::endl;
		return;
	}
	
	TiXml::TiXmlHandle docHandle( &xml_settings );
	TiXml::TiXmlHandle config_root = docHandle.FirstChild("reactivision");

	TiXml::TiXmlElement* tuio_element = config_root.FirstChild("tuio").Element();
	if( tuio_element!=NULL )
	{
		if(tuio_element->Attribute("host")!=NULL) sprintf(config->host,"%s",tuio_element->Attribute("host"));
		if(tuio_element->Attribute("port")!=NULL) config->port = atoi(tuio_element->Attribute("port"));
	}

	TiXml::TiXmlElement* camera_element = config_root.FirstChild("camera").Element();
	if( camera_element!=NULL )
	{
		if(camera_element->Attribute("config")!=NULL) sprintf(config->camera_config,"%s",camera_element->Attribute("config"));
	}
	
	TiXml::TiXmlElement* midi_element = config_root.FirstChild("midi").Element();
	if( midi_element!=NULL )
	{
		if(midi_element->Attribute("config")!=NULL) {
			sprintf(config->midi_config,"%s",midi_element->Attribute("config"));
			config->midi=true;
		}
	}

	TiXml::TiXmlElement* finger_element = config_root.FirstChild("finger").Element();
	if( finger_element!=NULL )
	{
		if(finger_element->Attribute("size")!=NULL) config->finger_size = atoi(finger_element->Attribute("size"));
		if(finger_element->Attribute("sensitivity")!=NULL) config->finger_sensitivity = atoi(finger_element->Attribute("sensitivity"));
	}
	
	TiXml::TiXmlElement* image_element = config_root.FirstChild("image").Element();
	if( image_element!=NULL )
	{
		if(image_element->Attribute("gradient")!=NULL) config->gradient_gate = atoi(image_element->Attribute("gradient"));
		if(image_element->Attribute("display")!=NULL)  {
			if ( strcmp( image_element->Attribute("display"), "none" ) == 0 ) config->display_mode = 0;
			else if ( strcmp( image_element->Attribute("display"), "src" ) == 0 )  config->display_mode = 1;
			else if ( strcmp( image_element->Attribute("display"), "dest" ) == 0 )  config->display_mode = 2;
		}
		if(image_element->Attribute("equalize")!=NULL) {
			if ((strcmp( image_element->Attribute("equalize"), "true" ) == 0) ||  atoi(image_element->Attribute("equalize"))==1) config->background = true;
		}
	}

	TiXml::TiXmlElement* fiducial_element = config_root.FirstChild("fiducial").Element();
	if( fiducial_element!=NULL )
	{
		if(fiducial_element->Attribute("engine")!=NULL)  {
			if ( strcmp( fiducial_element->Attribute("engine"), "amoeba" ) == 0 ) config->amoeba = true;
			else if ( strcmp( fiducial_element->Attribute("engine"), "classic" ) == 0 ) { config->classic = true; config->amoeba = false; }
			else if ( strcmp( fiducial_element->Attribute("engine"), "dtouch" ) == 0 )  { config->dtouch = true;  config->amoeba = false; }
		}
		if(fiducial_element->Attribute("tree")!=NULL) sprintf(config->tree_config,"%s",fiducial_element->Attribute("tree"));
	}

	TiXml::TiXmlElement* calibration_element = config_root.FirstChild("calibration").Element();
	if( calibration_element!=NULL )
	{
		if(calibration_element->Attribute("invert")!=NULL)  {
			if (strstr(calibration_element->Attribute("invert"),"x")>0) config->invert_x = true;
			if (strstr(calibration_element->Attribute("invert"),"y")>0) config->invert_y = true; 
			if (strstr(calibration_element->Attribute("invert"),"a")>0) config->invert_a = true; 			
		}
		if(calibration_element->Attribute("grid")!=NULL) sprintf(config->grid_config,"%s",calibration_element->Attribute("grid"));
	}
	
}


void writeSettings(reactivision_settings *config) {
	
	TiXml::TiXmlDocument xml_settings( config->file );
	xml_settings.LoadFile();
	if( xml_settings.Error() )
	{
		std::cout << "Error loading configuration file: " << config->file << std::endl;
		return;
	}
	
	char config_value[64];
	
	TiXml::TiXmlHandle docHandle( &xml_settings );
	TiXml::TiXmlHandle config_root = docHandle.FirstChild("reactivision");
	
	TiXml::TiXmlElement* tuio_element = config_root.FirstChild("tuio").Element();
	if( tuio_element!=NULL )
	{
		if(tuio_element->Attribute("host")!=NULL) tuio_element->SetAttribute("host",config->host);
		if(tuio_element->Attribute("port")!=NULL) {
			sprintf(config_value,"%d",config->port);
			tuio_element->SetAttribute("port",config_value);
		}
	}
	
	TiXml::TiXmlElement* camera_element = config_root.FirstChild("camera").Element();
	if( camera_element!=NULL )
	{
		if(camera_element->Attribute("config")!=NULL) camera_element->SetAttribute("config",config->camera_config);
	}
	
	TiXml::TiXmlElement* midi_element = config_root.FirstChild("midi").Element();
	if( midi_element!=NULL )
	{
		if(midi_element->Attribute("config")!=NULL) midi_element->SetAttribute("config",config->midi_config);
	}
	
	TiXml::TiXmlElement* finger_element = config_root.FirstChild("finger").Element();
	if( finger_element!=NULL )
	{
		if(finger_element->Attribute("size")!=NULL) {
			sprintf(config_value,"%d",config->finger_size);
			finger_element->SetAttribute("size",config_value);
		}
		if(finger_element->Attribute("sensitivity")!=NULL) {
			sprintf(config_value,"%d",config->finger_sensitivity);
			finger_element->SetAttribute("sensitivity",config_value);
		}
	}
	
	TiXml::TiXmlElement* image_element = config_root.FirstChild("image").Element();
	if( image_element!=NULL )
	{
		if(image_element->Attribute("gradient")!=NULL) {
			sprintf(config_value,"%d",config->gradient_gate);
			image_element->SetAttribute("gradient",config_value);
		}
		if(image_element->Attribute("display")!=NULL)  {
			if (config->display_mode == 0) image_element->SetAttribute("display","none");
			else if (config->display_mode == 1) image_element->SetAttribute("display","src");
			else if (config->display_mode == 2) image_element->SetAttribute("display","dest");  
		}
		if(image_element->Attribute("equalize")!=NULL) {
			if (config->background) image_element->SetAttribute("equalize","true");
			else image_element->SetAttribute("equalize","false");
		}
	}
	
	TiXml::TiXmlElement* fiducial_element = config_root.FirstChild("fiducial").Element();
	if( fiducial_element!=NULL )
	{
		if(fiducial_element->Attribute("engine")!=NULL)  {
			if (config->amoeba) fiducial_element->SetAttribute("engine", "amoeba"); 
			else if (config->classic) fiducial_element->SetAttribute("engine","classic"); 
			else if (config->dtouch) fiducial_element->SetAttribute("engine","dtouch"); 
		}
		if(fiducial_element->Attribute("tree")!=NULL) fiducial_element->SetAttribute("tree",config->tree_config);
	}
	
	TiXml::TiXmlElement* calibration_element = config_root.FirstChild("calibration").Element();
	if( calibration_element!=NULL )
	{
		sprintf(config_value," ");
		if(calibration_element->Attribute("invert")!=NULL)  {
			if (config->invert_x) strcat(config_value,"x");
			if (config->invert_y) strcat(config_value,"y");
			if (config->invert_a) strcat(config_value,"a");
			calibration_element->SetAttribute("invert",config_value);
		}
		if(calibration_element->Attribute("grid")!=NULL) calibration_element->SetAttribute("grid",config->grid_config);
	}
	
	xml_settings.SaveFile();
	if( xml_settings.Error() ) std::cout << "Error saving configuration file: "  << config->file << std::endl;
	
}