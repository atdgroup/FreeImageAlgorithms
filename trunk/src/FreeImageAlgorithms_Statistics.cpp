#include "FreeImageAlgorithms.h"
#include "FreeImageAlgorithms_Statistics.h"
#include "FreeImageAlgorithms_Palettes.h"
#include "FreeImageAlgorithms_Utilities.h"
#include "FreeImageAlgorithms_Utils.h"

FIBITMAP* DLL_CALLCONV
FreeImageAlgorithms_HistEq_Random_Additions(FIBITMAP *src)
{
	FIBITMAP *dib;
	int i, R;
	const int number_grey_levels = 256;
	int type, bpp, image_width, image_height, *left, *width;
	unsigned char *src_bits, *dib_bits;
	long total, Hsum, Havg, *histogram;

	bpp = FreeImage_GetBPP(src);
	type = FreeImage_GetImageType(src);
	
	// The image is not 8bit greyscale return.
	if( bpp != 8 || type != FIT_BITMAP )
		return NULL;

	image_width = FreeImage_GetWidth(src);
	image_height = FreeImage_GetHeight(src);

	// Allocate a 8-bit dib
	dib = FreeImage_AllocateT(FIT_BITMAP, image_width, image_height, 8, 0, 0, 0);
	FreeImageAlgorithms_CopyPalette(src, dib);

	total = (long) image_width * image_height;
	
	left =  (int *) malloc (sizeof(int) * number_grey_levels);
	width = (int *) malloc (sizeof(int) * number_grey_levels);
	histogram = (long *) malloc (sizeof(long) * number_grey_levels);
	
	/* Clear histogram */ 
	memset(histogram, 0, number_grey_levels); 

	FreeImage_GetHistogram(src, (DWORD *)histogram, FICC_BLACK );  

	/* right end of interval */ 
	R    = 0;		
	
	/* cumulative value for interval */ 
	Hsum = 0;		
	
	/* interval value for uniform histogram	*/
	Havg = total / number_grey_levels;	

	/* evaluate remapping of all input gray levels;
	 * Each input gray value maps to an interval of valid output values.
	 * The endpoints of the intervals are left[] and left[]+width[].
	 */
	for(i=0; i < number_grey_levels; i++) {
	
		left[i] = R;			/* left end of interval	*/
		
		Hsum += histogram[i];
		
		while( Hsum > Havg && R < (number_grey_levels - 1) ) { /* make interval wider	*/
		
			Hsum -= Havg;		/* adjust Hsum		*/
			R++;			/* update right end	*/
		}
		
		width[i] = R - left[i] + 1;	/* width of interval	*/
	}

	src_bits = (unsigned char *) FreeImage_GetBits(src);   
	dib_bits = (unsigned char *) FreeImage_GetBits(dib); 

	/* visit all input pixels and remap intensities */
	for(i=0; i < total; i++) {
	
		const unsigned char pixel = *src_bits++;
	
		if( width[pixel] == 1) {
		
			*dib_bits++ = left[pixel];
		}
		else {
		
			/* pixel spills over into width[] possible values */
			/* randomly pick from 0 to width[i] */
			
			/* 0 <= R < width */ 
			R = ( (rand() & 0x7fff) * width[pixel] ) >> 15;	
			
			*dib_bits++ = left[ pixel ] + R;
		}
	}
	
	free(histogram);
	
	return dib;
}


FIBITMAP* DLL_CALLCONV
FreeImageAlgorithms_HistEq(FIBITMAP *src)
{
	FIBITMAP *dst;
	int i;
	const int number_grey_levels = 256;
	int type, bpp, image_width, image_height;
	unsigned char *src_bits, *dst_bits;
	long total, Hsum;
	DWORD *histogram;
	double Havg;
	long *mapping;

	bpp = FreeImage_GetBPP(src);
	type = FreeImage_GetImageType(src);
	
	// The image is not 8bit greyscale return.
	if( bpp != 8 || type != FIT_BITMAP )
		return NULL;

	image_width = FreeImage_GetWidth(src);
	image_height = FreeImage_GetHeight(src);

	// Allocate a 8-bit dib
	dst = FreeImage_AllocateT(FIT_BITMAP, image_width, image_height, 8, 0, 0, 0);

	FreeImageAlgorithms_SetGreyLevelPalette(dst);

	total = image_width * image_height;
	
	mapping =  (long *) malloc (sizeof(long) * number_grey_levels);
	histogram = (DWORD *) malloc (sizeof(long) * number_grey_levels);
	
	/* Clear histogram */ 
	memset(histogram, 0, number_grey_levels); 
	memset(mapping, 0, number_grey_levels); 

	FreeImage_GetHistogram(src, histogram, FICC_BLACK );  
	
	/* cumulative value for interval */ 
	Hsum = 0;		
	
	/* interval value for uniform histogram	*/
	Havg = (double) total / (number_grey_levels - 1);	

	for(i=0; i < number_grey_levels; i++) {   
	
		Hsum += histogram[i];
		
		mapping[i] = Hsum;
	}

	src_bits = (unsigned char *) FreeImage_GetBits(src);   
	dst_bits = (unsigned char *) FreeImage_GetBits(dst); 

	/* visit all input pixels and remap intensities */
	for(i=0; i < total; i++) {
	
		const unsigned char pixel = *src_bits++;
	
		*dst_bits++ = (const unsigned char) (int) ((mapping[pixel] / Havg) + 0.5);
	}
	
	free(histogram);
	
	return dst;
}


double DLL_CALLCONV
FreeImageAlgorithms_GetGreyLevelAverage(FIBITMAP *src)
{
	if(!FreeImageAlgorithms_IsGreyScale(src))
		return 0,0;

	int bpp = FreeImage_GetBPP(src);
	int width = FreeImage_GetWidth(src);
	int height = FreeImage_GetHeight(src);
	long size = width * height;  	
   	
	FREE_IMAGE_TYPE type = FreeImage_GetImageType(src); 

	switch(type)
	{
		case FIT_BITMAP:
		{
			if(bpp == 8)
			{
				unsigned char *bits = (unsigned char *) FreeImage_GetBits(src);
				return MeanAverage(bits, size);
			}
		}

		case FIT_INT16:
		{
			short *bits = (short *) FreeImage_GetBits(src);
			return MeanAverage(bits, size);
		}
		
		case FIT_UINT16:
		{
			unsigned short *bits = (unsigned short *) FreeImage_GetBits(src);
			return MeanAverage(bits, size);
		}

		case FIT_FLOAT:
		{
			float *bits = (float *) FreeImage_GetBits(src);
			return MeanAverage(bits, size);
		}
	}

	return 0.0;
}

