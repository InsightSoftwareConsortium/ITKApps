# This project is intended to be built outside the Insight source tree
PROJECT(ConvertBetweenFileFormats)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(ConvertBetweenFileFormats castconvert.cxx  castconvertDicomScalar.cxx castconvertScalar.cxx  castconvertScalar2D.cxx castconvertScalar3D.cxx castconvertScalar4D.cxx)
ADD_EXECUTABLE(ImageCompare ImageCompare.cxx)

TARGET_LINK_LIBRARIES(ConvertBetweenFileFormats  ITKIO ITKCommon)
TARGET_LINK_LIBRARIES(ImageCompare ITKIO ITKCommon)

IF( BUILD_TESTING )
  # Set the path to the images
  SET(IMAGE_PATH ${ConvertBetweenFileFormats_SOURCE_DIR}/Testing/Data)
  SET(IMAGE_PATH_OUTPUTS ${ConvertBetweenFileFormats_BINARY_DIR}/Testing/Output)
  MAKE_DIRECTORY( ${IMAGE_PATH_OUTPUTS} )

  # Convert from grayscale mhd to other image file formats
  ADD_TEST(ccvnt_to_png                   ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.png)
  ADD_TEST(ccvnt_to_jpg_unsigned_int      ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.jpg unsigned_int)
  ADD_TEST(ccvnt_to_tiff                  ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.tiff)
  ADD_TEST(ccvnt_to_analyze75             ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.img)
  ADD_TEST(ccvnt_to_bmp_unsigned_char     ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.bmp unsigned_char)
  ADD_TEST(ccvnt_to_nrrd_float            ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.nrrd float)
  ADD_TEST(ccvnt_to_vtk_long              ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.vtk long)
  ADD_TEST(ccvnt_to_gipl                  ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.gipl)
  #only 3D currently ADD_TEST(ccvnt_to_nifti_test                 ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.nii.gz)

  # Convert back to mhd
  ADD_TEST(ccvnt_from_png                 ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.png    ${IMAGE_PATH_OUTPUTS}/image_in1.mhd)
  ADD_TEST(ccvnt_from_jpg_unsigned_int    ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.jpg    ${IMAGE_PATH_OUTPUTS}/image_in2.mhd unsigned_short)
  ADD_TEST(ccvnt_from_tiff                ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.tiff   ${IMAGE_PATH_OUTPUTS}/image_in3.mhd)
  ADD_TEST(ccvnt_from_analyze75           ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.hdr    ${IMAGE_PATH_OUTPUTS}/image_in4.mhd)
  ADD_TEST(ccvnt_from_bmp_unsigned_char   ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.bmp    ${IMAGE_PATH_OUTPUTS}/image_in5.mhd unsigned_short)
  ADD_TEST(ccvnt_from_nrrd_float          ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.nrrd   ${IMAGE_PATH_OUTPUTS}/image_in6.mhd unsigned_short)
  ADD_TEST(ccvnt_from_vtk_long            ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.vtk    ${IMAGE_PATH_OUTPUTS}/image_in7.mhd unsigned_short)
  ADD_TEST(ccvnt_from_gipl                ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.gipl   ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
  #only 3D currently ADD_TEST(ccvnt_from_nifti               ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.nii.gz ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)

  # Compare with original png
  ADD_TEST(compare_png              ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in1.mhd)
  #ADD_TEST(compare_jpg_unsigned_int ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in2.mhd)
  ADD_TEST(compare_tiff             ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in3.mhd)
  ADD_TEST(compare_analyze75        ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in4.mhd)
  ADD_TEST(compare_bmp_unsigned_char ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in5.mhd)
  ADD_TEST(compare_nrrd_float       ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in6.mhd)
  ADD_TEST(compare_vtk_long         ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in7.mhd)
  ADD_TEST(compare_gipl             ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
  #only 3D currently ADD_TEST(ccvnt_compare_nifti            ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/. ConvertBetweenFileFormats)
INSTALL_TARGETS(/. ImageCompare)
