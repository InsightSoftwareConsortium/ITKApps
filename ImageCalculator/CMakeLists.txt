PROJECT(ImageCalculator)

ADD_EXECUTABLE(ImageCalculator.exe  ImageCalculator.cc ImageCalculatorProcess2D.cxx ImageCalculatorProcess3D.cxx ImageCalculatorUtils.cxx)

TARGET_LINK_LIBRARIES(ImageCalculator.exe ITKCommon ITKIO ITKAlgorithms ITKBasicFilters )

INSTALL_FILES(/include/ImageCalculator "(\\.h)$")
INSTALL_TARGETS(/bin ImageCalculator.exe )

ADD_EXECUTABLE(ImageCompare.exe ImageCompare.cxx)
TARGET_LINK_LIBRARIES(ImageCompare.exe ITKCommon ITKIO )
INSTALL_TARGETS(/bin ImageCompare.exe )


#Test to show arithmetic operation and printing the statistics.
ADD_TEST(ImageCalculator2DTest1 ${EXECUTABLE_OUTPUT_PATH}/ImageCalculator.exe -in
${ImageCalculator_SOURCE_DIR}/TestImages/UpperHalfHundreds.png
${ImageCalculator_SOURCE_DIR}/TestImages/LowerHalfHundreds.png -intype UCHAR
-outtype UCHAR -add -out ${ImageCalculator_BINARY_DIR}/Test.png -d 2 -statNPX -statMIN -statAVG
-statMAX -statVAR -statSUM  )

ADD_TEST(ValidateImageCalculator2DTest1 ${EXECUTABLE_OUTPUT_PATH}/ImageCompare.exe ${ImageCalculator_BINARY_DIR}/Test.png
${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png)

#Test for more than 2 images different data types for Input image and output image

ADD_TEST(ImageCalculator2DTest2 ${EXECUTABLE_OUTPUT_PATH}/ImageCalculator.exe -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png -intype UCHAR -outtype USHORT -add -out ${ImageCalculator_BINARY_DIR}/Test.png -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )

ADD_TEST(ValidateImageCalculator2DTest2 ${EXECUTABLE_OUTPUT_PATH}/ImageCompare.exe ${ImageCalculator_BINARY_DIR}/Test.png ${ImageCalculator_SOURCE_DIR}/TestImages/AllFiveHundreds.png)


# Test for 3d Images and division
ADD_TEST(ImageCalculator3DTest1 ${EXECUTABLE_OUTPUT_PATH}/ImageCalculator.exe  -in
${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr
${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr -intype UCHAR -outtype
UCHAR -div -out
${ImageCalculator_BINARY_DIR}/Test.hdr -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM
)

ADD_TEST(ValidateImageCalculator3DTest1 ${EXECUTABLE_OUTPUT_PATH}/ImageCompare.exe ${ImageCalculator_BINARY_DIR}/Test.hdr ${ImageCalculator_SOURCE_DIR}/TestImages/AllTwos.hdr)


#A simple scalar multiplcation test
ADD_TEST(ImageCalculator3DTest2 ${EXECUTABLE_OUTPUT_PATH}/ImageCalculator.exe  -in
${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr -intype UCHAR -outtype
UCHAR -out
${ImageCalculator_BINARY_DIR}/Test.hdr
-d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ofmulc 2
)

ADD_TEST(ValidateImageCalculator3DTest2 ${EXECUTABLE_OUTPUT_PATH}/ImageCompare.exe ${ImageCalculator_BINARY_DIR}/Test.hdr
${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr)




