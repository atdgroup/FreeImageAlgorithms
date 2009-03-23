#!/usr/bin/env python
# -*- coding: utf-8 -*-

# FreeImageAlgorithms wrapper inherits types from the FreeImage wrapper.
# Written by Glenn Pierce glennpierce@gmail.com

import sys
import ctypes as C
from ctypes.util import find_library
from warnings import warn

#Load the constants
from FreeImagePy.constants import *
import FreeImagePy.FreeImagePy as FI

def LoadLibrary(libraryName):
    """ Initialize the library and set the self.init value
        to error if an exception is raise
    """
    # Add current directory
    os.environ['PATH'] = os.environ['PATH'] + ';' + os.path.abspath(os.path.dirname(__file__))
        
    if sys.platform == 'win32':
        functForLoad = C.windll
    else:
        functForLoad = C.cdll
        
    if not libraryName:
        libraryName = "freeimagealgorithms"
        
    libName = find_library(libraryName)
        
    if not libName:
        raise FreeImagePy_LibraryNotFound, \
        "I cannot find the library at %s. Did you pass me a vaid path?" % libraryName
        
    try:
        lib = getattr(functForLoad, libName)
        if sys.platform != "win32":
            lib.FreeImage_Initialise(False)
    except Exception, ex:
        print "Exception on load dll %s :%s"  % (libraryName, ex)
            
    return lib
            
 
# A ctype void_p but I define a new type so I can compare on the type and take the approiate action as this is a out parameter
class c_double_out(C.c_void_p):
    pass
 
FIA_FUNCTION_LIST = {
    
    # Name, Return Type, (Param1 type, Param1 type2) ....... etc
    # Defaulr param of BITMAP gets translated to self.bitmap
    'setRainBowPalette' : ('FIA_SetRainBowPalette', C.c_int, (C.c_void_p,)), 
    'histEq' : ('FIA_HistEq', C.c_int, (C.c_void_p,)), 
    'getGreyLevelAverage' : ('FIA_GetGreyLevelAverage', C.c_double, (C.c_void_p,)), 
    'monoImageFindWhiteArea' : ('FIA_MonoImageFindWhiteArea', C.c_int, (C.c_void_p, c_double_out)), 
     
	}
	   
	   
class WrappedMethod(object):
       
    def __init__(self, lib, function_details, dib):
        self.__lib = lib
        self.__function_details = function_details
        self.__dib = dib
        
        function_name = function_details[0]
        function_return_type = function_details[1]
        function_arg_types = function_details[2]
        
        self.__function = getattr(lib, function_name)
        self.__function.restype = function_return_type
        self.__function.argtypes = function_arg_types
       
    def __call__(self, *args):
    
        required_args = len(self.__function.argtypes) 
    
        # Check correct number of args
       # if args != len(self.__function.argtypes) - 1:
       #     msg = "%s takes %d arguments" % (self.__function_details[0], len(self.__function.argtypes) - 1)
        #    raise TypeError, msg
    
        param_count = 1
        params = [None] * required_args
        params[0] = self.__dib
        
        for arg in self.__function.argtypes[1:]:
            
            # ctype can automatically wrap ints, strings and unicode so we dont wrap these args in the ctype type object
            if arg == C.c_int or arg == C.c_char_p:
                params[param_count] = arg
        
            if arg == C.c_double:
                params[param_count] = C.c_double(arg)
                
            if arg == c_double_out:
                params[param_count] = C.c_double()
                params[param_count] = C.byref(params[param_count])            
            
            param_count = param_count + 1
    
        print params
    
        ret = self.__function(params[0], params[1])
         
        return (ret, params[1].value)
         
        # We must make sure the arguments passed are valid for the ctype function parameters
        #if len(args) == 0:
        #    return self.__function(self.__dib)
        #else:
        #    return self.__function(self.__dib, args)
        
       
class FIAImage(FI.Image):
    
    """ 
    FreeImageAlgorithms class wrapper
    
    The method with the first line uppercase are internal library methods
    the other one are internal method for help the user
        
    @author: Glenn Pierce
    """
    
    __lib = LoadLibrary("freeimagealgorithms")
    
    # Enable the message output
    if sys.platform == 'win32':
        MessageOutput = C.WINFUNCTYPE(VOID, C.c_char_p)
    else:
        MessageOutput = C.CFUNCTYPE(VOID, C.c_char_p)

    def __init__(self, f=None):
        """
        Init method for the class
        
        @param libraryName: library path, otherwise I'll look for it into the
                            standard path
        @type libraryName: string
        """
        
        super(FIAImage, self).__init__(f, None)      
        self.initCalled = 0
    
    def wrappedFunction(self, func):
    
        func_props = FIA_FUNCTION_LIST.get(funct.__name__)
        
        if func_props == None:
            raise AttributeError, attr
            
        function_name = func_props[0]
        function_return_type = func_props[1]
        function_arg_types = func_props[2]
        
        function = getattr(lib, function_name)
        function.restype = function_return_type
        function.argtypes = function_arg_types
        
        return function

    @wrappedFunction
    def setRainBowPalette(self):
        """ Set a rainbow palette for the bitmap.
        """
        return self.setRainBowPalette(self.getBitmap()))
       
    def getHistogram(self, min, max, bins):
        "Get the histogram of a greylevel image"
        DW_array = DWORD * bins # type
        red_histo = DW_array()
        green_histo = DW_array()
        blue_histo = DW_array()
                
        bitmap = self.getBitmap()
            
        if self.getBPP() >= 24 and self.GetImageType(bitmap) == FIT_BITMAP:
            FIAImage.__lib.FIA_RGBHistogram(bitmap, C.c_byte(min),
                C.c_byte(max), bins, C.byref(red_histo),  C.byref(green_histo),
                C.byref(blue_histo))
            return ([int(x) for x in red_histo], [int(x) for x in green_histo], [int(x) for x in blue_histo])    
        else: 
            FIAImage.__lib.FIA_Histogram(bitmap, C.c_double(min), C.c_double(max), bins, C.byref(red_histo))
            return ([int(x) for x in red_histo])
    
        return None
    
    
    
