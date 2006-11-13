#ifndef __FREEIMAGE_ALGORITHMS_TESTING__
#define __FREEIMAGE_ALGORITHMS_TESTING__

#include "FreeImageAlgorithms.h"

#define IMAGE_DIR "..\\..\\..\\..\\Test Images"
#define TEMP_DIR "C:\\Temp\\FreeImageAlgorithms_Tests"

#ifdef __cplusplus
extern "C" {
#endif

void
ShowImage(FIBITMAP *src);

void
ShowImageFromFile(char *filepath);


#ifdef __cplusplus
}
#endif

#endif