PROJECT(ImageCalculator)

ADD_EXECUTABLE(ImageCalculator  ImageCalculator.cxx ImageCalculatorProcess2D.cxx ImageCalculatorProcess3D.cxx ImageCalculatorUtils.cxx)
TARGET_LINK_LIBRARIES(ImageCalculator ITKCommon ITKIO ITKAlgorithms ITKBasicFilters )

ADD_EXECUTABLE(ImageGenerate ImageGenerate.cxx )
TARGET_LINK_LIBRARIES(ImageGenerate ITKCommon ITKIO)

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
  #
  # Test add
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

  #
  # Test SUB
  ADD_TEST(ImageCalculator2DTest3 ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllZeroes.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
        -intype UCHAR -outtype USHORT -sub
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )
  #
  # Test Mult
  ADD_TEST(ImageCalc2DMult ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllTens.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllTens.png 
        -intype UCHAR -outtype UCHAR -mul
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )
  #
  # Test Mult
  ADD_TEST(ImageCalc2DDiv ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllTens.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllTens.png 
        -intype UCHAR -outtype UCHAR -div
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )

  #
  # Test Avg
  ADD_TEST(ImageCalc2DAvg ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllFifties.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/LowerHalfHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/UpperHalfHundreds.png 
        -intype UCHAR -outtype UCHAR -avg
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )


  #
  # Test Input Square
  ADD_TEST(ImageCalc2DTestSquare ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllTens.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllZeroes.png 
        -ifsqr -intype UCHAR -outtype UCHAR -sub
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )
  #
  # Test Input Square Root
  ADD_TEST(ImageCalc2DTestSquareRoot ${IMAGE_CALCULATOR_TESTS} 
    --compare
        ${ImageCalculator_BINARY_DIR}/Test.png 
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllTens.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png 
            ${ImageCalculator_SOURCE_DIR}/TestImages/AllZeroes.png 
        -ifsqrt -intype UCHAR -outtype USHORT -sub
        -out ${ImageCalculator_BINARY_DIR}/Test.png 
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM  )

  # Test for 3d Images and division
  #
  # test Divide
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
  # test multiply
  ADD_TEST(ImageCalc3DTestOMult ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.hdr
        ${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.hdr
        -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ofmulc 2  )

  #A simple scalar multiplcation test
  # test multiply
  ADD_TEST(ImageCalc3DTestIMult ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.hdr
        ${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.hdr
        -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ifmulc 2  )

  #A simple scalar division test
  # test 
  ADD_TEST(ImageCalc3DTestODiv ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.hdr
        ${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.hdr
        -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ofdivc 2  )

  #A simple scalar divide test
  # test diviply
  ADD_TEST(ImageCalc3DTestIDiv ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.hdr
        ${ImageCalculator_SOURCE_DIR}/TestImages/Hundred.hdr
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/TwoHundred.hdr 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.hdr
        -d 3 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ifdivc 2  )

  #A simple scalar addition test
  # test 
  ADD_TEST(ImageCalc2DTestIAdd ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.png
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllFifties.png 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.png
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ifaddc 50 )
  ADD_TEST(ImageCalc2DTestOAdd ${IMAGE_CALCULATOR_TESTS} 
    --compare 
        ${ImageCalculator_BINARY_DIR}/Test.png
        ${ImageCalculator_SOURCE_DIR}/TestImages/AllHundreds.png
    ImageCalculatorTest 
        -in ${ImageCalculator_SOURCE_DIR}/TestImages/AllFifties.png 
        -intype UCHAR -outtype UCHAR 
        -out ${ImageCalculator_BINARY_DIR}/Test.png
        -d 2 -statNPX -statMIN -statAVG -statMAX -statVAR -statSUM -ofaddc 50 )


  ADD_EXECUTABLE(ImageCalculatorTests ImageCalculatorTests.cxx ImageCalculatorUtils.cxx ImageCalculatorProcess2D.cxx ImageCalculatorProcess3D.cxx)
  TARGET_LINK_LIBRARIES(ImageCalculatorTests ITKCommon ITKIO ITKAlgorithms ITKBasicFilters)
  
ENDIF( BUILD_TESTING )

