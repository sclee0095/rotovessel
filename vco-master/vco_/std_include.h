#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <iostream>
#include <direct.h> 
#include <Windows.h>
#include <string>
#include <vector>
#include <time.h>

#include <deque>
#include <queue>

#include "opencv2/opencv.hpp"

#include <limits>

#ifdef VCOAPI_EXPORTS
#define VCO_EXPORTS __declspec(dllexport)
#else
#define VCO_EXPORTS __declspec(dllimport)
#endif

#include <stdarg.h>

#include <cstdlib>
#include <algorithm>
#include <functional>

//std::greater

//#include "mex.h"