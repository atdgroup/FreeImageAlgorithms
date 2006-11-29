#ifndef __FREEIMAGE_ALGORITHMS_PALETTES__
#define __FREEIMAGE_ALGORITHMS_PALETTES__

#include "FreeImageAlgorithms.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file 
	Provides various palette modification methods.
*/ 

/** \brief Copies the palette for an image to an array of RGBQUAD.
 *
 *  \param src Image containing palette.
 *  \param palette  RGBQUAD Array to copy palette to.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_CopyPaletteToRGBQUAD(FIBITMAP *src, RGBQUAD *palette);

/** \brief Copies the palette from an array of RGBQUAD to a FIBITMAP.
 *
 *  \param dst Image to copy palette to.
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV		   
FreeImageAlgorithms_CopyPaletteFromRGBQUAD(FIBITMAP *dst, RGBQUAD *palette);

/** \brief Copies the palette from one FIBITMAP to another.
 *
 *  \param src Image containing palette.
 *  \param dst Image to copy palette to.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV		   
FreeImageAlgorithms_CopyPalette(FIBITMAP *src, FIBITMAP *dst);

/** \brief Reverses a section or all of an array of RGBQUAD elements.
 *
 *  \param  palette RGBQUAD* array.
 *  \param  start Point to start from in array.
 *  \param  size Number of elements you wish to reverse.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_ReversePaletteEntries(RGBQUAD *palette, unsigned char start, unsigned char size);

/** \brief Set the pallete of a FIBITMAP image to a greylevel palette.
 *
 *  \param src Image to set palette to greyscale.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetGreyLevelPalette(FIBITMAP *src);

/** \brief Set the pallete of a FIBITMAP image to a log palette.
 *
 *  \param src Image to set palette to log.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetLogColourPalette(FIBITMAP *src);

/** \brief Set the pallete of a FIBITMAP image to a rainbow palette.
 *
 *  \param src Image to set palette to rainbow.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetRainBowPalette(FIBITMAP *src);

/** \brief Set the pallete of a FIBITMAP image to a temperature palette.
 *
 *  \param src Image to set palette to temperature.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetTemperaturePalette(FIBITMAP *src);

/** \brief Set the pallete of a FIBITMAP image to a false colour palette.
 *
 *  \param src Image to set palette to false colour.
 *  \param wavelength Wavelength of the false colour.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetFalseColourPalette(FIBITMAP *src, double wavelength);

/** \brief Set the pallete of a FIBITMAP image to a greyscale overload palette.
 *
 *  \param src Image to set palette to greyscale overload.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetGreyLevelOverLoadPalette(FIBITMAP *src);

/** \brief Set the pallete of a FIBITMAP image to a seismic palette.
 *
 *  \param src Image to set palette to seismic.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_SetSeismicColourPalette(FIBITMAP *src);

/** \brief Get a greyscale pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetGreyLevelPalette(RGBQUAD *palette);

/** \brief Get a greyscale overload pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetGreyLevelOverLoadPalette(RGBQUAD *palette);

/** \brief Get a rainbow pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetRainBowPalette(RGBQUAD *palette);

/** \brief Get a log pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetLogColourPalette(RGBQUAD *palette);

/** \brief Get a false colour pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \param wavelength of the false colour palette.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetFalseColourPalette(RGBQUAD *palette, double wavelength);

/** \brief Get a temperature pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetTemperaturePalette(RGBQUAD *palette);

/** \brief Get a seismic pallete into a RGBQUAD array.
 *
 *  \param  palette RGBQUAD* array.
 *  \return int FREEIMAGE_ALGORITHMS_SUCCESS on success or FREEIMAGE_ALGORITHMS_ERROR on error.
*/
DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetSeismicColourPalette(RGBQUAD *palette);

DLL_API int DLL_CALLCONV
FreeImageAlgorithms_GetOpticalDensityPalette(RGBQUAD *palette, unsigned char red,
	unsigned char green, unsigned char blue, int contrast, int entries);

#ifdef __cplusplus
}
#endif

#endif