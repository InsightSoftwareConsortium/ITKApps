PROJECT(ImageCalculator)

ADD_EXECUTABLE(ImageCalculator  ImageCalculator.cxx ImageCalculatorProcess2D.cxx ImageCalculatorProcess3D.cxx ImageCalculatorUtils.cxx)
TARGET_LINK_LIBRARIES(ImageCalculator ITKCommon ITKIO ITKAlgorithms ITKBasicFilters )

INSTALL_FILES(/include/ImageCalculator "(\\.h)$")
INSTALL_TARGETS(/bin ImageCalculator )

IF( BUILD_TESTING )
 
  SET(IMAGE_CALCULATOR_TESTS ${CXX_TEST_PATH}/ImageCalculatorTests)
  
  #Test to show arithmetic operation and printing the statistics.
  ADD_TEST(ImageCalculator2DTest1 ${IMAGE_CALCULATOR_TESTS} 
    --compare 
       ${ImageCalculator_BINARY_DIR}/Test.png
       ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png
    ImageCalculatorTest 
       -in ${ImageCalculator_SOURCE_DIR}/TestImages/UpperHalfHundreds.png
           ${ImageCalculator_SOURCE_DIR}/TestImages/LowerHalfHundreds.png 
       -intype UCHAR -outtype UCHAR -add 
       -out ${ImageCalculator_BINARY_DIR}/Test.png 
       -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )

  #Test for more than 2 images different data types for Input image and output image

  ADD_TEST(ImageCalculator2DTest2 ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllFiveHundreds.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
        -intype UCHAR -outtype USHORT -add 
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )

  # Test for 3d Images and division
  ADD_TEST(ImageCalculator3DTest1 ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.hdr 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllTwos.hdr
    ImageCalculatorTest  
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr
            ${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr 
        -intype UCHAR -outtype UCHAR -div 
        -out ${ImageCalculator_BINARY_DIR}/Test.hdr 
        -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )

  #A simple scalar multiplcation test
  ADD_TEST(ImageCalculator3DTest2 ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.hdr
        ${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.hdr
        -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ofmulc 2  )

  ADD_EXECUTABLE(ImageCalculatorTests ImageCalculatorTests.cxx ImageCalculatorUtils.cxx ImageCalculatorProcess2D.cxx ImageCalculatorProcess3D.cxx)
  TARGET_LINK_LIBRARIES(ImageCalculatorTests ITKCommon ITKIO ITKAlgorithms ITKBasicFilters)
  
ENDIF( BUILD_TESTING )

