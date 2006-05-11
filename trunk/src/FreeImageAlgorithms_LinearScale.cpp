#include "FreeImageAlgorithms_LinearScale.h"
#include "FreeImageAlgorithms_Utilities.h"
#include "FreeImageAlgorithms_Utils.h"

#include <iostream>
#include "assert.h"

/** Convert a greyscale image of type Tsrc to a 8-bit grayscale dib.
	Conversion is done using either a linear scaling from [min, max] to [0, 255]
	or a rounding from src_pixel to (BYTE) MIN(255, MAX(0, q)) where int q = int(src_pixel + 0.5); 
*/
template<class Tsrc>
class LINEAR_SCALE
{
public:
	FIBITMAP* convert(FIBITMAP *src, double min, double max, double *min_with_image, double *max_within_image);
};

template<class Tsrc> FIBITMAP* 
LINEAR_SCALE<Tsrc>::convert(FIBITMAP *src, double min, double max, double *min_within_image, double *max_within_image)
{
	FIBITMAP *dst = NULL;
	unsigned x, y;
						
	unsigned width	= FreeImage_GetWidth(src);
	unsigned height = FreeImage_GetHeight(src);

	// allocate a 8-bit dib
	dst = FreeImage_AllocateT(FIT_BITMAP, width, height, 8, 0, 0, 0);
	
	if(!dst)
		return NULL;

	double scale;

	FreeImageAlgorithms_FindMinMax(src, min_within_image, max_within_image);

	// If the user has not specifed min & max use the min and max pixels in the image.
	if(max == 0.0 && min == 0.0) {
		min = *min_within_image;
		max = *max_within_image;
	}
	
	if(*min_within_image == *max_within_image) {

		FreeImage_Unload(dst);
		return FreeImage_ConvertToStandardType(src, 1);
	}
	
	// compute the scaling factor
	scale = 255 / (double)(max - min);

	Tsrc *src_bits;
	BYTE *dst_bits;
	long tmp;

	// scale to 8-bit
	for(y = 0; y < height; y++) {

		src_bits = reinterpret_cast<Tsrc*>(FreeImage_GetScanLine(src, y));
		dst_bits = FreeImage_GetScanLine(dst, y);

		for(x = 0; x < width; x++) {

			if(src_bits[x] < min) {

				dst_bits[x] = 0;
				continue;
			}

			tmp = static_cast<long>(scale * (src_bits[x] - min));

			if(tmp >= 255) {
				tmp = 255;
			}

			dst_bits[x] = (BYTE) tmp;	
		}
	}
	
	return dst;
}


// Convert from type X to type BYTE
LINEAR_SCALE<unsigned char>		scaleUCharImage;
LINEAR_SCALE<unsigned short>	scaleUShortImage;
LINEAR_SCALE<short>				scaleShortImage;
LINEAR_SCALE<unsigned long>		scaleULongImage;
LINEAR_SCALE<long>				scaleLongImage;
LINEAR_SCALE<float>				scaleFloatImage;
LINEAR_SCALE<double>			scaleDoubleImage;


// ----------------------------------------------------------
//   smart convert X to standard FIBITMAP
// ----------------------------------------------------------

/** Convert image of any type to a standard 8-bit greyscale image.
For standard images, a clone of the input image is returned.
When the scale_linear parameter is TRUE, conversion is done by scaling linearly 
each pixel to an integer value between [0..255]. When it is FALSE, conversion is done 
by rounding each float pixel to an integer between [0..255]
@param image Image to convert
@param scale_linear Linear scaling / rounding switch
*/
FIBITMAP* DLL_CALLCONV
FreeImageAlgorithms_LinearScaleToStandardType(FIBITMAP *src, double min, double max, double *min_within_image, double *max_within_image) {
	FIBITMAP *dst = NULL;

	if(!src) return NULL;

	// convert from src_type to FIT_BITMAP

	FREE_IMAGE_TYPE src_type = FreeImage_GetImageType(src);

	switch(src_type) {
		case FIT_BITMAP:	// standard image: 1-, 4-, 8-, 16-, 24-, 32-bit
			if(FreeImage_GetBPP(src) == 8)
				dst = scaleUCharImage.convert(src, min, max, min_within_image, max_within_image);
			break;
		case FIT_UINT16:	// array of unsigned short: unsigned 16-bit
			dst = scaleUShortImage.convert(src, min, max, min_within_image, max_within_image);
			break;
		case FIT_INT16:		// array of short: signed 16-bit
			dst = scaleShortImage.convert(src, min, max, min_within_image, max_within_image);
			break;
		case FIT_UINT32:	// array of unsigned long: unsigned 32-bit
			dst = scaleULongImage.convert(src, min, max, min_within_image, max_within_image);
			break;
		case FIT_INT32:		// array of long: signed 32-bit
			dst = scaleLongImage.convert(src, min, max, min_within_image, max_within_image);
			break;
		case FIT_FLOAT:		// array of float: 32-bit
			dst = scaleFloatImage.convert(src, min, max,  min_within_image,  max_within_image);
			break;
		case FIT_DOUBLE:	// array of double: 64-bit
			dst = scaleDoubleImage.convert(src, min, max, min_within_image, max_within_image);
			break;
		case FIT_COMPLEX:	// array of FICOMPLEX: 2 x 64-bit
			break;
	}

	if(NULL == dst) {
		FreeImage_OutputMessageProc(FIF_UNKNOWN, "FREE_IMAGE_TYPE: Unable to convert from type %d to type %d.\n No such conversion exists.", src_type, FIT_BITMAP);
	}

	return dst;
}
