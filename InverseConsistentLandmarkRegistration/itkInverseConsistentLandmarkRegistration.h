/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkInverseConsistentLandmarkRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkInverseConsistentLandmarkRegistration_h
#define __itkInverseConsistentLandmarkRegistration_h
#include "itkObject.h"
#include "itkImage.h"
#include "itkThinPlateR2LogRSplineKernelTransform.h"
#include "itkThinPlateSplineKernelTransform.h"
#include "itkWarpImageFilter.h"

#if !defined(CLREG_PRECISION)
#define CLREG_PRECISION double
#endif

namespace itk
{

  template <typename CoordRepType, unsigned int Dimension=3>
  class InverseConsistentLandmarkRegistration : public Object
    {
    public:
      typedef InverseConsistentLandmarkRegistration Self;
      typedef Object Superclass;
      typedef SmartPointer<Self> Pointer;
      typedef SmartPointer<const Self> ConstPointer;

      /** Method for creation through the object factory. */
      itkNewMacro(Self);

      /** Run-time type information (and related methods). */
      itkTypeMacro(Image, ImageBase);

      /** Type of Image to transform */
      typedef itk::Image<CoordRepType,Dimension> ImageType;
      typedef typename ImageType::Pointer ImagePointer;

      /** Type of vector in a deformation field image */
      typedef itk::Vector<CoordRepType,Dimension> WarpPixelType;
      /** Image of deformation vectors */
      typedef itk::Image< WarpPixelType ,Dimension> WarpImageType;
      typedef typename WarpImageType::Pointer WarpImagePointer;

      typedef itk::WarpImageFilter<ImageType,ImageType,WarpImageType>  WarpFilterType;

      /** Transform type for forward warping */
      typedef ThinPlateR2LogRSplineKernelTransform<CoordRepType,Dimension> WMatrix2DTransformType;
      typedef ThinPlateSplineKernelTransform<CoordRepType,Dimension> WMatrix3DTransformType;
      typedef KernelTransform<CoordRepType,Dimension> WMatrixTransformType;
      typedef typename WMatrixTransformType::Pointer TransformPointerType;

      /** Type of pointset used as input for Landmark Registration */
      typedef typename WMatrixTransformType::PointSetType PointSetType;
      typedef typename PointSetType::Pointer PointSetTypePointer;
      typedef typename PointSetType::PointIdentifier PointIdentifierType;

      //
      // deformation types
      enum ENUM_DEF_TYPES
        {
          ENUM_STANDARD_DEF=0,
          ENUM_INVERSE_DEF=1
        };
      typedef enum ENUM_DEF_TYPES DEF_TYPES;

      void SetRegion(typename ImageType::RegionType& imageSize)
        {
        m_ImageRegion = imageSize;
        m_ImageSize = m_ImageRegion.GetSize();
        }
      /** Set input landmarks for thin plate spline - index 0 = first landmark set, index 1 is target landmark set */
      void SetLandmarks(unsigned int index, PointSetTypePointer &Landmarks); // set landmarks[0] and [1]

      /** Transform an Image using TPS algorithm */
      ImagePointer TransformByWMatrix(ImagePointer &inputImage);

      ImagePointer TransformByVectorField(const DEF_TYPES desiredTransform, ImagePointer &inputImage);

      /** Derive the forward deformation */
      void ComputeDeformation();

      /** Computes inverse deformation */
      void ComputeInverseDeformation();

      /** Get the Deformation vector used to warp from landmarks[0] to landmarks[1] */
      WarpImagePointer GetDeformationVector();

      /** Get the  InverseDeformationVector derived by the Inverse Consistent algorithm. */
      WarpImagePointer GetInverseDeformationVector();

      TransformPointerType GetLandmarkKernelTransform() const { return m_LandmarkKernelTransform; };
    private:

      InverseConsistentLandmarkRegistration();

      int InverseTransform3D_OnN(const CoordRepType INV_EPSILON=MY_EPSILON_ONN,
                                 const int INV_MAX_ITERATIONS=MY_MAX_ITERATIONS,
                                 const int INV_LOWERBOUNDCHECK=MY_LOWERBOUNDCHECK);
      bool m_PointsAssigned[2];
      bool m_XformsComputed[2];
      PointSetTypePointer m_Pointsets[2];
      WarpImagePointer m_DeformationFields[2];//NOTE [0] is the Standard, and [1] is the inverse
      typename ImageType::RegionType m_ImageRegion;
      typename ImageType::RegionType::SizeType m_ImageSize;
      TransformPointerType m_LandmarkKernelTransform;
    };
}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInverseConsistentLandmarkRegistration.txx"
#endif

#endif
