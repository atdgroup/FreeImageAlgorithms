import sys

import FreeImagePy
import FreeImageAlgorithmsPy as FIA

F = FIA.FIAImage()  
F.load("C:\\lena_gray.bmp")
ggg = 0
print F.monoImageFindWhiteArea(ggg)
F.save("C:\\lena_result.bmp")