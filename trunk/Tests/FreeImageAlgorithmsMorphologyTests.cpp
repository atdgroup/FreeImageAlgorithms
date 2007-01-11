#include "CuTest.h"

#include "FreeImage.h"
#include "FreeImageAlgorithms.h"
#include "FreeImageAlgorithms_IO.h"
#include "FreeImageAlgorithms_Palettes.h"
#include "FreeImageAlgorithms_Testing.h"
#include "FreeImageAlgorithms_Utilities.h"
#include "FreeImageAlgorithms_Morphology.h"

static double kernel_values[] = {1.0, 1.0, 1.0,
								 1.0, 1.0, 1.0,
								 1.0, 1.0, 1.0};


static void
TestFreeImageAlgorithms_DilationTest(CuTest* tc)
{
	char *file = IMAGE_DIR "\\morpholology_test.jpg";

	FIBITMAP *dib1 = FreeImageAlgorithms_LoadFIBFromFile(file);
	
	CuAssertTrue(tc, dib1 != NULL);
	
	FIBITMAP *threshold_dib = FreeImage_Threshold(dib1, 20);

	CuAssertTrue(tc, threshold_dib != NULL);

	FIBITMAP *threshold_8bit_dib = FreeImage_ConvertTo8Bits(threshold_dib);

	CuAssertTrue(tc, threshold_8bit_dib != NULL);

	//FreeImageAlgorithms_SetGreyLevelOverLoadPalette(threshold_8bit_dib);

	FIABITMAP *border_dib = FreeImageAlgorithms_SetBorder(threshold_8bit_dib, 1, 1);

	ProfileStart("DilationFilter");

	FilterKernel kernel = FreeImageAlgorithms_NewKernel(10, 10, kernel_values, 1.0);

	FIBITMAP* result_dib = FreeImageAlgorithms_BinaryDilation(border_dib, kernel);

	CuAssertTrue(tc, result_dib != NULL);

	ProfileStop("DilationFilter");

	FreeImageAlgorithms_SaveFIBToFile(threshold_dib, TEMP_DIR "\\dilation_threshold.jpg", BIT24);
	FreeImageAlgorithms_SaveFIBToFile(result_dib, TEMP_DIR "\\dilation_result.jpg", BIT24);

	FreeImage_Unload(dib1);
	FreeImage_Unload(threshold_dib);
	FreeImage_Unload(threshold_8bit_dib);
	FreeImageAlgorithms_Unload(border_dib);
	FreeImage_Unload(result_dib);
}

/*
static void
TestFreeImageAlgorithms_ErosionTest(CuTest* tc)
{
	char *file = IMAGE_DIR "\\morpholology_test.jpg";

	FIBITMAP *dib1 = FreeImageAlgorithms_LoadFIBFromFile(file);
	
	CuAssertTrue(tc, dib1 != NULL);
	
	FIBITMAP *threshold_dib = FreeImage_Threshold(dib1, 20);

	CuAssertTrue(tc, threshold_dib != NULL);

	FIBITMAP *threshold_8bit_dib = FreeImage_ConvertTo8Bits(threshold_dib);

	CuAssertTrue(tc, threshold_8bit_dib != NULL);

	//FreeImageAlgorithms_SetGreyLevelOverLoadPalette(threshold_8bit_dib);
	
	FIABITMAP* border_dib = FreeImageAlgorithms_SetBorder(threshold_8bit_dib, 1, 1);

	ProfileStart("ErosionFilter");

	FIBITMAP* result_dib = FreeImageAlgorithms_Erosion(border_dib);

	CuAssertTrue(tc, result_dib != NULL);

	ProfileStop("ErosionFilter");

	FreeImageAlgorithms_SaveFIBToFile(threshold_dib, TEMP_DIR "\\erosion_threshold.jpg", BIT24);
	FreeImageAlgorithms_SaveFIBToFile(result_dib, TEMP_DIR "\\erosion_result.jpg", BIT24);

	FreeImage_Unload(dib1);
	FreeImage_Unload(threshold_dib);
	FreeImage_Unload(threshold_8bit_dib);
	FreeImageAlgorithms_Unload(border_dib);
	FreeImage_Unload(result_dib);
}
*/


CuSuite* DLL_CALLCONV
CuGetFreeImageAlgorithmsMorphologySuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestFreeImageAlgorithms_DilationTest);
	//SUITE_ADD_TEST(suite, TestFreeImageAlgorithms_ErosionTest);

	return suite;
}