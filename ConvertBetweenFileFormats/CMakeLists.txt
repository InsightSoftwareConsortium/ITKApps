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

ADD_EXECUTABLE(ConvertBetweenFileFormats castconvert.cxx  castconvertDicomScalar.cxx castconvertScalar.cxx  castconvertScalar2D.cxx castconvertScalar3D.cxx castconvertScalar4D.cxx castconvertScalar4DA.cxx castconvertScalar2DA.cxx castconvertScalar3DA.cxx castconvertDicomScalarA.cxx)

TARGET_LINK_LIBRARIES(ConvertBetweenFileFormats  ITKIO ITKCommon)

INSTALL_TARGETS(/bin ConvertBetweenFileFormats)

IF( BUILD_TESTING )

  ADD_EXECUTABLE(ImageCompare ImageCompare.cxx)
  TARGET_LINK_LIBRARIES(ImageCompare  ITKIO ITKCommon)

  SET(IMAGE_PATH ${ConvertBetweenFileFormats_SOURCE_DIR}/Testing/Data)
  SET(IMAGE_PATH_OUTPUTS ${ConvertBetweenFileFormats_BINARY_DIR})
  # Convert from grayscale mhd to other image file formats
  ADD_TEST(ccvnt_to_png                   ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.png)
  ADD_TEST(ccvnt_to_jpg_unsigned_int      ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.jpg unsigned_int)
  ADD_TEST(ccvnt_to_tiff                  ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.tiff)
  ADD_TEST(ccvnt_to_analyze75             ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.img)
  ADD_TEST(ccvnt_to_bmp_unsigned_char     ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.bmp unsigned_char)
  ADD_TEST(ccvnt_to_nrrd_float            ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.nrrd float)
  ADD_TEST(ccvnt_to_vtk_long              ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.vtk long)
  ADD_TEST(ccvnt_to_gipl                  ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.gipl)
  ADD_TEST(ccvnt_to_nifti_test            ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.nii.gz)

  # Convert back to mhd
  ADD_TEST(ccvnt_from_png                 ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.png    ${IMAGE_PATH_OUTPUTS}/image_in1.mhd)
  ADD_TEST(ccvnt_from_jpg_unsigned_int    ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.jpg    ${IMAGE_PATH_OUTPUTS}/image_in2.mhd unsigned_short)
  ADD_TEST(ccvnt_from_tiff                ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.tiff   ${IMAGE_PATH_OUTPUTS}/image_in3.mhd)
  ADD_TEST(ccvnt_from_analyze75           ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.hdr    ${IMAGE_PATH_OUTPUTS}/image_in4.mhd)
  ADD_TEST(ccvnt_from_bmp_unsigned_char   ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.bmp    ${IMAGE_PATH_OUTPUTS}/image_in5.mhd unsigned_short)
  ADD_TEST(ccvnt_from_nrrd_float          ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.nrrd   ${IMAGE_PATH_OUTPUTS}/image_in6.mhd unsigned_short)
  ADD_TEST(ccvnt_from_vtk_long            ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.vtk    ${IMAGE_PATH_OUTPUTS}/image_in7.mhd unsigned_short)
  ADD_TEST(ccvnt_from_gipl                ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.gipl   ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
  ADD_TEST(ccvnt_from_nifti               ${ConvertBetweenFileFormats_BINARY_DIR}/ConvertBetweenFileFormats ${IMAGE_PATH_OUTPUTS}/image_out.nii.gz ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)

  # Compare with original png
  ADD_TEST(compare_png                    ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in1.mhd)
 #ADD_TEST(compare_jpg_unsigned_int       ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in2.mhd)
  ADD_TEST(compare_tiff                   ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in3.mhd)
  ADD_TEST(compare_analyze75              ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in4.mhd)
  ADD_TEST(compare_bmp_unsigned_char      ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in5.mhd)
  ADD_TEST(compare_nrrd_float             ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in6.mhd)
  ADD_TEST(compare_vtk_long               ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in7.mhd)
  ADD_TEST(compare_gipl                   ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
  ADD_TEST(ccvnt_compare_nifti            ${ConvertBetweenFileFormats_BINARY_DIR}/ImageCompare ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)

  INSTALL_TARGETS(/bin ImageCompare)
ENDIF( BUILD_TESTING )

