#define _CRT_SECURE_NO_DEPRECATE // so yeah, no fopen_s (it's not portable)

#include "svo_convert.h"

using namespace std;
using namespace trimesh;

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// (GLOBAL) Program version
string version = "1.0";

// (GLOBAL) Program parameters
string filename = "";
size_t gridsize = 512;
bool verbose = true;
ColorMode color_mode = COLOR_FROM_MODEL;
vec3 FIXED_COLOR = vec3(0.0f, 0.0f, 0.0f);

void printInfo(){
	cout << "-------------------------------------------------------------" << endl;
	cout << "Model to .SVO converter " << version << endl;
#ifdef _WIN32 || _WIN64
	cout << "Windows version" << endl;
#endif
#ifdef __LINUX__
	cout << "Linux version" << endl;
#endif
#ifdef __APPLE__
	cout << "OSX version" << endl;
#endif
#ifdef ENVIRONMENT64
	cout << "64-bit version" << endl;
#endif
#ifdef ENVIRONMENT32
	cout << "32-bit version" << endl;
#endif
	cout << "Jeroen Baert - jeroen.baert@cs.kuleuven.be - www.forceflow.be" << endl;
	cout << "-------------------------------------------------------------" << endl << endl;
}

void printHelp(){
        cout << "Available program parameters:" << endl;
	cout << "	-f (filename)	: Path to model file (.obj, .ply, .3ds, .ray)" << endl;
        cout << "	-s (gridsize)	: Grid size, shouldd be power of 2 (1,2,4,8,16,32,64,128,256,512 or 1024)" << endl;
	cout << "";
	cout << "Example: "<< endl;
        cout << "	svo_convert -f bunny.ply -s 256" << endl;
}

void printInvalid(){
	std::cout << "Not enough or invalid arguments, please try again.\n" << endl; 
	printHelp();
}

void parseProgramParameters(int argc, char* argv[]){
	// Input argument validation
	if(argc<3){ // not enough arguments
		printInvalid(); exit(0);
	} 
	for (int i = 1; i < argc; i++) {
		if (string(argv[i]) == "-f") {
			filename = argv[i + 1]; 
			i++;
		} else if (string(argv[i]) == "-s") {
			gridsize = atoi(argv[i + 1]);
			if (!isPowerOf2(gridsize)) {
				cout << "Requested gridsize is not a power of 2" << endl; printInvalid(); exit(0);
			}
			i++;
		} else if (string(argv[i]) == "-c") {
			string color_input = string(argv[i + 1]);
			if (color_input == "model") {
				color_mode = COLOR_FROM_MODEL;
			} else if (color_input == "normal") {
				color_mode = COLOR_NORMAL;
			} else if (color_input == "fixed") {
				color_mode = COLOR_FIXED;
			} else {
				cout << "Unrecognized color switch: " << color_input << ", so defaulting to colors from model." << endl;
			}
			i++;
		} else {
			printInvalid(); exit(0);
		}
	}
	if(verbose){
		cout << "  filename: " << filename << endl;
		cout << "  gridsize: " << gridsize << endl;
	}
}

int main(int argc, char *argv[]){
	// Parse parameters

	printInfo();
	cout << "Reading program parameters ..." << endl;
	parseProgramParameters(argc,argv);

	convert2svo(filename,gridsize, color_mode);
}
