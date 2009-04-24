#include "CuTest.h"

#include "FreeImage.h"
#include "FreeImageAlgorithms_IO.h"
#include "FreeImageAlgorithms_Drawing.h"
#include "FreeImageAlgorithms_Palettes.h"
#include "FreeImageAlgorithms_Utilities.h"

#include "FreeImageAlgorithms_Testing.h"

static void
TestFIA_GreyscaleElipseTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "drone-bee-greyscale.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);
	CuAssertTrue(tc, src != NULL);

    FIARECT rect;
	rect.left = 50;
	rect.top = 100;
	rect.bottom = 200;
	rect.right = 200;

    FIA_DrawSolidGreyscaleEllipse (src, rect, 200, 1);
    FIA_DrawGreyscaleRect (src, rect, 200, 2);

	FIA_SaveFIBToFile(src, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_GreyscaleElipseTest.jpg", BIT8);

	FreeImage_Unload(src);
}

static void
TestFIA_ConvexHullTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "particle.bmp";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);
	CuAssertTrue(tc, src != NULL);
	
    FIBITMAP *hull_dib = FreeImage_ConvexHull (src);

    CuAssertTrue(tc, hull_dib != NULL);

	FIA_SaveFIBToFile(hull_dib, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_ConvexHullTest.jpg", BIT8);

	FreeImage_Unload(src);
    FreeImage_Unload(hull_dib);
}

static void
TestFIA_GSLineTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "drone-bee-greyscale.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);
	CuAssertTrue(tc, src != NULL);

	FIAPOINT p1, p2;

	p1.x = 10;
	p1.y = 10;
    p2.x = p1.x;
    p2.y = 200;

    FIA_DrawGreyscaleLine (src, p1, p2, 150, 3, 0);
 
	FIA_SaveFIBToFile(src, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_GSLineTest.jpg", BIT8);

	FreeImage_Unload(src);
}

static void
TestFIA_ColourLineTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "bumblebee.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);

	FIBITMAP *src32 = FreeImage_ConvertTo32Bits(src);

	CuAssertTrue(tc, src != NULL);

	FIAPOINT p1, p2, p3;

	p1.x = 10;
	p1.y = 10;
	p2.x = 200;
	p2.y = 300;
    p3.x = 200;
	p3.y = 300;
    
	FIA_DrawColourLine (src32, p1, p2, FIA_RGBQUAD(255, 0, 0), 5, 0);
    FIA_DrawColourLine (src32, p1, p3, FIA_RGBQUAD(255, 0, 0), 5, 1);

	FIA_SaveFIBToFile(src32, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_ColourLineTest.jpg", BIT24);

	FreeImage_Unload(src);
	FreeImage_Unload(src32);
}


static void
TestFIA_Rect24bitTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "drone-bee.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);

	FIBITMAP *src24 = FreeImage_ConvertTo24Bits(src);

	CuAssertTrue(tc, src != NULL);

	FIARECT rect;
	rect.left = 165;
	rect.top = 118;
	rect.bottom = 156;
	rect.right = 200;
	
	FIA_DrawColourRect (src24, rect, FIA_RGBQUAD(255,0,0), 1);

	FIA_SaveFIBToFile(src24, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_Rect24bitTest.jpg", BIT24);

	FreeImage_Unload(src);
	FreeImage_Unload(src24);
}


static void
TestFIA_Rect32bitTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "bumblebee.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);

	FIBITMAP *src32 = FreeImage_ConvertTo32Bits(src);

	CuAssertTrue(tc, src != NULL);

	FIARECT rect;
	rect.left = 100;
	rect.top = 100;
	rect.bottom = 200;
	rect.right = 200;
	
	FIA_DrawColourRect (src32, rect, FIA_RGBQUAD(255,0,0), 7);

	FIA_SaveFIBToFile(src32, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_Rect32bitTest.jpg", BIT24);

	FreeImage_Unload(src);
	FreeImage_Unload(src32);
}

static void
TestFIA_GsRectTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "drone-bee-greyscale.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);

	CuAssertTrue(tc, src != NULL);

	FIARECT rect;
	rect.left = 100;
	rect.top = 100;
	rect.bottom = 200;
	rect.right = 200;
	
	FIA_DrawGreyscaleRect (src, rect, 200, 5);

	FIA_SaveFIBToFile(src, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_GsRectTest.jpg", BIT8);

	FreeImage_Unload(src);
}

static void
TestFIA_SolidRectTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "bumblebee.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);

	CuAssertTrue(tc, src != NULL);

	FIARECT rect;
	rect.left = 100;
	rect.top = 100;
	rect.bottom = 200;
	rect.right = 200;
	
	FIA_DrawColourSolidRect (src, rect, FIA_RGBQUAD(255,0,0));

	FIA_SaveFIBToFile(src, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_SolidRectTest.jpg", BIT24);

	FreeImage_Unload(src);
}

static void
TestFIA_SolidGSRectTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "drone-bee-greyscale.jpg";

	FIBITMAP *src = FIA_LoadFIBFromFile(file);

	CuAssertTrue(tc, src != NULL);

	FIBITMAP *gs_src = FreeImage_ConvertTo8Bits(src);

	CuAssertTrue(tc, gs_src != NULL);

	FIARECT rect;
	rect.left = 10;
	rect.top = 10;
	rect.bottom = 200;
	rect.right = 200;

	FIA_DrawSolidGreyscaleRect (gs_src, rect, 100);

	FIA_SaveFIBToFile(gs_src, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_SolidGSRectTest.jpg", BIT8);
  
	FreeImage_Unload(src);
	FreeImage_Unload(gs_src);
}

static void
TestFIA_FloodFillTest(CuTest* tc)
{
	const char *file = TEST_DATA_DIR "particle.bmp";

	FIBITMAP *dib1 = FIA_LoadFIBFromFile(file);

	CuAssertTrue(tc, dib1 != NULL);

	FIBITMAP *dib2 = FreeImage_ConvertTo8Bits(dib1);

	CuAssertTrue(tc, dib2 != NULL);

	FIBITMAP *dib3 = FIA_FloodFill(dib2, 20, 1, 255);

    FIA_SetGreyLevelPalette(dib3);
    
	CuAssertTrue(tc, dib3 != NULL);

	FIA_SaveFIBToFile(dib3, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_FloodFillTest.jpg", BIT24);

	FreeImage_Unload(dib1);
	FreeImage_Unload(dib2);
	FreeImage_Unload(dib3);
}


static void
TestFIA_CheckerboardTest(CuTest* tc)
{
	FIBITMAP *dib = FreeImage_Allocate(500, 500, 8, 0, 0, 0);

	CuAssertTrue(tc, dib != NULL);

	FIA_SetGreyLevelPalette(dib);
	FIA_DrawGreyScaleCheckerBoard (dib, 50);
  
	FIA_SaveFIBToFile(dib, TEST_DATA_OUTPUT_DIR "Drawing/TestFIA_CheckerboardTest.jpg", BIT24);

	FreeImage_Unload(dib);
}





CuSuite* DLL_CALLCONV
CuGetFreeImageAlgorithmsDrawingSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	MkDir(TEST_DATA_OUTPUT_DIR "/Drawing");

	/*
	SUITE_ADD_TEST(suite, TestFIA_GSLineTest);
	SUITE_ADD_TEST(suite, TestFIA_ColourLineTest);
	SUITE_ADD_TEST(suite, TestFIA_Rect24bitTest);
    SUITE_ADD_TEST(suite, TestFIA_Rect32bitTest);
    SUITE_ADD_TEST(suite, TestFIA_GsRectTest);
	SUITE_ADD_TEST(suite, TestFIA_SolidRectTest);
	SUITE_ADD_TEST(suite, TestFIA_SolidGSRectTest);
	SUITE_ADD_TEST(suite, TestFIA_FloodFillTest);
    SUITE_ADD_TEST(suite, TestFIA_ConvexHullTest);
    SUITE_ADD_TEST(suite, TestFIA_GreyscaleElipseTest);
	*/

	SUITE_ADD_TEST(suite, TestFIA_CheckerboardTest);

	return suite;
}
