#ifndef __FREEIMAGE_ALGORITHMS_FFT__
#define __FREEIMAGE_ALGORITHMS_FFT__

#include "FreeImageAlgorithms.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file 
*	Provides FFT methods.
*/ 

/** \brief Creates a HBITMAP
 *	
 *  \param src FIBITMAP 
 *	\param inverse Whether to perform a inverse fourier transform.
 *	\param shift Shift the Zero frequency to the center of the image.
 *  \return FIBITMAP* on success and NULL on error.
*/
DLL_API FIBITMAP* DLL_CALLCONV
FIA_FFT(FIBITMAP *src);

DLL_API FIBITMAP* DLL_CALLCONV
FIA_IFFT(FIBITMAP *src);

/** \brief Creates a FIT_DOUBLE absolute image from a complex image.
 *	
 *  \param src FIBITMAP complex image.
 *  \return FIBITMAP* on success and NULL on error.
*/
DLL_API FIBITMAP* DLL_CALLCONV
FIA_ConvertComplexImageToAbsoluteValued(FIBITMAP *src);

/** \brief Creates a FIT_DOUBLE absolute image from a complex image.
 *	
 *  \param src FIBITMAP complex image.
 *  \return FIBITMAP* on success and NULL on error.
*/
DLL_API FIBITMAP* DLL_CALLCONV
FIA_ConvertComplexImageToAbsoluteValuedSquared(FIBITMAP *src);

DLL_API FIBITMAP* DLL_CALLCONV
FIA_ComplexImageToRealValued(FIBITMAP *src);

DLL_API FIBITMAP* DLL_CALLCONV
FIA_ShiftImageEdgeToCenter(FIBITMAP *src);

DLL_API int DLL_CALLCONV
FIA_InPlaceShiftImageEdgeToCenter(FIBITMAP **src);

#ifdef __cplusplus
}
#endif

#endif
