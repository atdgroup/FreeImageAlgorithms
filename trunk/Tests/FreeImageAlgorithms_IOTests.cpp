#include "CuTest.h"

#include "Constants.h"
#include "FreeImageAlgorithms.h"
#include "FreeImageAlgorithms_IO.h"
#include "FreeImageAlgorithms_Utilities.h"

#include "FreeImageAlgorithms_Testing.h"

static void
TestFIA_IO(CuTest* tc)
{
	FIBITMAP *dib1 = NULL, *dib2 = NULL;
	FREE_IMAGE_TYPE type;
	int bpp, err;
    
    const char *file = TEST_DATA_DIR "drone-bee.jpg";
	dib1 = FIA_LoadFIBFromFile(file);

	CuAssertTrue(tc, dib1 != NULL);

	bpp = FreeImage_GetBPP(dib1);
	type = FreeImage_GetImageType(dib1);

	CuAssertTrue(tc, bpp == 24);
	CuAssertTrue(tc, type == FIT_BITMAP);
	
	err = FIA_SaveFIBToFile (dib1, TEST_DATA_OUTPUT_DIR "/IO/save-colour-test.bmp", BIT8);

	CuAssertTrue(tc, err == FIA_SUCCESS);

	dib2 = FIA_LoadFIBFromFile(TEST_DATA_OUTPUT_DIR "/IO/save-colour-test.bmp");

	err = FIA_BitwiseCompare(dib1, dib2);

	FreeImage_Unload(dib1);
	FreeImage_Unload(dib2);
}

static void
TestFIA_IOLoadColourArrayData(CuTest* tc)
{
	FIBITMAP *dib1 = NULL, *dib2 = NULL;
	FREE_IMAGE_TYPE type;
	int bpp, err;
    
    const char *file = TEST_DATA_DIR "lucy_pinder.jpg";
	dib1 = FIA_LoadFIBFromFile(file);

	CuAssertTrue(tc, dib1 != NULL);

	dib2 = FreeImage_AllocateT (FIT_BITMAP, FreeImage_GetWidth(dib1), FreeImage_GetHeight(dib1), 24, 0, 0, 0);

	PROFILE_START("CopyColourBytesToFIBitmap");

	for(int i=0; i < 500; i++) {

		CopyColourBytesToFIBitmap (dib2, FreeImage_GetBits(dib1), 0, 1, COLOUR_ORDER_RGB);
	}

	PROFILE_STOP("CopyColourBytesToFIBitmap");

	FIA_SaveFIBToFile (dib2, TEST_DATA_OUTPUT_DIR "/IO/save-colour-test.bmp", BIT24);

	FreeImage_Unload(dib1);
	FreeImage_Unload(dib2);
}

CuSuite* DLL_CALLCONV
CuGetFreeImageAlgorithmsIOSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	MkDir(TEST_DATA_OUTPUT_DIR "/IO");

	SUITE_ADD_TEST(suite, TestFIA_IO);
	SUITE_ADD_TEST(suite, TestFIA_IOLoadColourArrayData);

	return suite;
}
