/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    itkInverseConsistentLandmarkRegistration.txx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkInverseConsistentLandmarkregistration_txx
#define __itkInverseConsistentLandmarkregistration_txx
#include "itkInverseConsistentLandmarkRegistration.h"
#include "itkExceptionObject.h"
#include "itkResampleImageFilter.h"

//#define VERBOSITY_LEVEL_1 1
const double MY_EPSILON_ONN=1;
const double MY_EPSILON=1.0e-4;
//Must be less than 2147483647;
const int MY_MAX_ITERATIONS = 10000;
const int MY_LOWERBOUNDCHECK= 100;

/**
 * \author  Hans J. Johnson
 * \short
 *  Function: Calculate Wrapped Indicies i.e. Calculates indices into an image
 *             That is assummed to repeat infinitly with periods iMaxX, iMaxY, iMaxZ
 *             in each of their respective directions
 *  Algorithm: The algorithm assumes that you will not try to access more than
 *             2*iMaxX, 2*iMaxY, 2*iMaxZ which is usually a safe assumption
 * Func. Ret.: The function returns the index into a single dimentional array
 * \param iDesiredX The desired x location
 * \param iDesiredY The desired y location
 * \param iDesiredZ The desired z location
 * \param iMaxX The X periodicity of the image (Usually the image dimensions
 * \param iMaxY The Y periodicity of the image (Usually the image dimensions
 * \param iMaxZ The Z periodicity of the image (Usually the image dimensions
 * \param iStrideX The memory elements in a row (May be greater or equal to iMaxX)
 * \param iStrideY The memory elements in a column (May be greater or equal to iMaxY)
 * \return the index into the array of the desired location
 */
inline int CircularIndexing( const int iDesiredX, const int
iDesiredY, const int iDesiredZ,
const int iMaxX, const int iMaxY, const int iMaxZ,
const int iStrideX, const int iStrideY )
{
  const int rval=iStrideX*
    (
    iStrideY*
    (
    ( iDesiredZ >= 0 ) ?
    ( ( iDesiredZ < iMaxZ ) ? iDesiredZ:( iDesiredZ - iMaxZ ) ): ( iMaxZ + iDesiredZ )
    )
    +(
    ( iDesiredY >= 0 ) ?
    ( ( iDesiredY < iMaxY ) ? iDesiredY:( iDesiredY - iMaxY ) ): ( iMaxY + iDesiredY )
    )
    )
    +(
    ( iDesiredX >= 0 ) ?
    ( ( iDesiredX < iMaxX ) ? iDesiredX:( iDesiredX - iMaxX ) ): ( iMaxX + iDesiredX )
    ) ;
  return rval;
}

template <typename floatType,unsigned int XYZIndex>
floatType
TrilinearInterpolationFast( const typename itk::Vector<floatType,3> *rawdata,
    const floatType fDesiredX, const floatType fDesiredY, const floatType fDesiredZ,
    const int nx, const int ny, const int nz,
    const int iStrideX, const int iStrideY, const int iPlaneOffset )
{
  // Calculate the Closet point in direction of the origin
  // Calculate Offset to desired GetPixel
    const floatType fXtemp = floorf( fDesiredX );
    const floatType fYtemp = floorf( fDesiredY );
    const floatType fZtemp = floorf( fDesiredZ );
    //const floatType * rawdata=Volume->GetBufferPointer();

        {
        const int iFlooredX = static_cast < int >( fXtemp );
        const int iFlooredY = static_cast < int >( fYtemp );
        const int iFlooredZ = static_cast < int >( fZtemp );

        /*********************/
        /*Start Interpolation */
        /*********************/
        /* If in the Middle of the Image */
        if ( ( iFlooredX > 0 ) && ( iFlooredX < nx - 1 ) && ( iFlooredY > 0 )
            && ( iFlooredY < ny - 1 ) && ( iFlooredZ > 0 ) && ( iFlooredZ < nz - 1 ) )
            {
            const floatType fXDelta = fDesiredX - fXtemp;
            const int a000 = (iFlooredX + iStrideX * iFlooredY + iPlaneOffset * iFlooredZ );
            const floatType a000_eval=rawdata[a000][XYZIndex];
            const int a001 = (a000 + 1);
            //const floatType a001_eval=rawdata[a001][XYZIndex];
            const floatType b00_eval = fXDelta * ( rawdata[ a001 ][XYZIndex] - ( a000_eval ) ) + ( a000_eval );

            const int a010 = (a000 + iStrideX);
            const floatType a010_eval=rawdata[a010][XYZIndex];
            const int a011 = (a001 + iStrideX);
            //const floatType a011_eval= rawdata[a011][XYZIndex];
            const floatType b01_eval = fXDelta * ( rawdata[ a011 ][XYZIndex] - ( a010_eval ) ) + ( a010_eval );

            const floatType fYDelta = fDesiredY - fYtemp;
            const floatType c0_eval = fYDelta * ( b01_eval - b00_eval ) + b00_eval;

            const floatType a100_eval= rawdata[a000 + iPlaneOffset][XYZIndex];
            //const floatType a101_eval= rawdata[a001 + iPlaneOffset][XYZIndex];
            //const floatType b10_eval = fXDelta * ( ( a101_eval ) - ( a100_eval ) ) + ( a100_eval );
            const floatType b10_eval = fXDelta * ( rawdata[ a001 + iPlaneOffset ][XYZIndex] - ( a100_eval ) ) + ( a100_eval );

            const floatType a110_eval= rawdata[a010 + iPlaneOffset][XYZIndex];
            //const floatType a111_eval= rawdata[a011 + iPlaneOffset][XYZIndex];
            //const floatType b11_eval = fXDelta * ( ( a111_eval ) - ( a110_eval ) ) + ( a110_eval );
            const floatType b11_eval = fXDelta * ( rawdata[ a011 + iPlaneOffset ][XYZIndex] - ( a110_eval ) ) + ( a110_eval );

            const floatType c1_eval = fYDelta * ( b11_eval - b10_eval ) + b10_eval;

            const floatType fZDelta = fDesiredZ - fZtemp;
            return fZDelta * ( c1_eval - c0_eval ) + c0_eval;
            }
        else                         /* If on a boundary */
            {
            const floatType fXDelta = fDesiredX - fXtemp;
            const int iNextX=iFlooredX+1;
            const int iNextY=iFlooredY+1;
            const int iNextZ=iFlooredZ+1;
            /*Circular Boundaries */
            const floatType a000_eval = rawdata[CircularIndexing( iFlooredX, iFlooredY, iFlooredZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];
            const floatType a001_eval = rawdata[CircularIndexing( iNextX, iFlooredY, iFlooredZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];

            const floatType b00_eval = fXDelta * ( ( a001_eval ) - ( a000_eval ) ) + ( a000_eval );

            const floatType a010_eval = rawdata[CircularIndexing( iFlooredX, iNextY, iFlooredZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];
            const floatType a011_eval = rawdata[CircularIndexing( iNextX, iNextY, iFlooredZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];

            const floatType b01_eval = fXDelta * ( ( a011_eval ) - ( a010_eval ) ) + ( a010_eval );

            const floatType fYDelta = fDesiredY - fYtemp;
            const floatType c0_eval = fYDelta * ( b01_eval - b00_eval ) + b00_eval;

            const floatType a100_eval = rawdata[CircularIndexing( iFlooredX, iFlooredY, iNextZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];
            const floatType a101_eval = rawdata[CircularIndexing( iNextX, iFlooredY, iNextZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];

            const floatType b10_eval = fXDelta * ( ( a101_eval ) - ( a100_eval ) ) + ( a100_eval );

            const floatType a110_eval = rawdata[CircularIndexing( iFlooredX, iNextY, iNextZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];
            const floatType a111_eval = rawdata[CircularIndexing( iNextX, iNextY, iNextZ, nx, ny, nz, iStrideX, iStrideY )][XYZIndex];

            const floatType b11_eval = fXDelta * ( ( a111_eval ) - ( a110_eval ) ) + ( a110_eval );

            const floatType c1_eval = fYDelta * ( b11_eval - b10_eval ) + b10_eval;

            const floatType fZDelta = fDesiredZ - fZtemp;
            return fZDelta * ( c1_eval - c0_eval ) + c0_eval;
            }
        }
}


namespace itk
{

    template <typename CoordRepType, unsigned int Dimension>
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        InverseConsistentLandmarkRegistration()
            {
            m_XformsComputed[ENUM_STANDARD_DEF] = m_XformsComputed[ENUM_INVERSE_DEF] =
                m_PointsAssigned[0] = m_PointsAssigned[1] = false;
            }


  template <typename CoordRepType, unsigned int Dimension>
  int 
  InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
  InverseTransform3D_OnN(const CoordRepType INV_EPSILON,
                         const int INV_MAX_ITERATIONS,
                         const int INV_LOWERBOUNDCHECK)
  {
    WarpImagePointer U = m_DeformationFields[ENUM_STANDARD_DEF];
    WarpImagePointer UInv = m_DeformationFields[ENUM_INVERSE_DEF];
#ifdef VERBOSITY_LEVEL_1
    /*This needs to be the same size as INV_MAX_ITERATIONS+1 */
    long int *iaIterationsNeeded = new long int[INV_MAX_ITERATIONS + 1];
    memset( iaIterationsNeeded, 0, ( INV_MAX_ITERATIONS + 1 ) * sizeof( long int ) );
#endif                         /*VERBOSITY_LEVEL_1*/
    // const int nx = U[0].getXDim(  );
    // const int ny = U[0].getYDim(  );
    // const int nz = U[0].getZDim(  );
    const typename WarpImageType::RegionType::SizeType ImageSize =
      U->GetLargestPossibleRegion().GetSize();
    const int &nx = ImageSize[0];
    const int &ny = ImageSize[1];
    const int &nz = ImageSize[2];

    //const int iMaxT=U[0].getTDim();
    const int &iStrideX = nx;
    const int &iStrideY = ny;
    const int iPlaneOffset = iStrideX * iStrideY;

    // CoordRepType *UInvData[3];//Get a pointer to the raw memeory
    // CoordRepType const * UData[3];//Get a pointer to the raw memeory of the Displacement field
    // for(int i = 0; i < 3; i++)
    // {
    // UData[i] = U[i]->GetBufferPointer();
    // UInvData[i] = UInv[i]->GetBufferPointer();
    // }
    WarpPixelType *UData = U->GetBufferPointer();
    WarpPixelType *UInvData = UInv->GetBufferPointer();
    typename ImageType::IndexType index;
    index[0]=0;
    index[1]=0;
    index[2]=0;
    typename ImageType::OffsetValueType iLastIndex = UInv->ComputeOffset(index);
 
    /*Initialize by assuming a rigid translation */
    //UInvData[0][iLastIndex] = -UData[0][iLastIndex];
    /*Initialize by assuming a rigid translation */
    //UInvData[1][iLastIndex] = -UData[1][iLastIndex];
    /*Initialize by assuming a rigid translation */
    //UInvData[2][iLastIndex] = -UData[2][iLastIndex];
    UInvData[iLastIndex][0] = -UData[iLastIndex][0];
    UInvData[iLastIndex][1] = -UData[iLastIndex][1];
    UInvData[iLastIndex][2] = -UData[iLastIndex][2];

    //DEBUG
    //    std::cout << "UInv" << iLastIndex << '\t'
    //    << UInvData[0][iLastIndex] << ' '
    //    << UInvData[1][iLastIndex] << ' '
    //    << UInvData[2][iLastIndex] << ' '
    //    << std::endl;

    //const CoordRepType fEpsilon=5e-6F; //NOTE: 1e-6 is to small to resolve.
    //const CoordRepType fEpsilon = 5e-6;  This is too slow, and not much improvement
    const CoordRepType fEpsilon = INV_EPSILON ;
    const CoordRepType &fEpsilon1= fEpsilon;
    const CoordRepType &fEpsilon2= fEpsilon;
    const CoordRepType &fEpsilon3= fEpsilon;
    for ( int k = 0; k < nz; k++ )
      {
#ifdef VERBOSITY_LEVEL_1
      putchar( '.' );
      fflush( stdout );
#endif                       /*VERBOSITY_LEVEL_1*/
      const CoordRepType fBasePnt3 = static_cast < CoordRepType >( k );
      for ( int j = 0; j < ny; j++ )
        {
        const CoordRepType fBasePnt2 = static_cast < CoordRepType >( j );
        for ( int i = 0; i < nx; i++ )
          {
          const CoordRepType fBasePnt1 = static_cast < CoordRepType >( i );

          /* Make an inital fGuess of where the next ConstPixel should come from */
          CoordRepType fDestPnt1 = fBasePnt1 + UInvData[iLastIndex][0];
          CoordRepType fDestPnt2 = fBasePnt2 + UInvData[iLastIndex][1];
          CoordRepType fDestPnt3 = fBasePnt3 + UInvData[iLastIndex][2];
          /* Reset Iteration Counter */
          int iIteration = 0;
          CoordRepType fDelta1, fDelta2, fDelta3;
          CoordRepType fDispU1, fDispU2, fDispU3;
          //The first INV_LOWERBOUNDCHECK intervals
          do
            {
            {
            const CoordRepType &fullDestPnt1=fDestPnt1;//NOTE: Same as the fDestPnt when doing OnN
            const CoordRepType &fullDestPnt2=fDestPnt2;//NOTE: Same as the fDestPnt when doing OnN
            const CoordRepType &fullDestPnt3=fDestPnt3;//NOTE: Same as the fDestPnt when doing OnN
            /*Calculate new h(x) */
            fDispU1 = TrilinearInterpolationFast<CoordRepType,0>( UData,fullDestPnt1, fullDestPnt2, fullDestPnt3,
                                                                nx, ny, nz, iStrideX, iStrideY, iPlaneOffset );
            fDispU2 = TrilinearInterpolationFast<CoordRepType,1>( UData,fullDestPnt1, fullDestPnt2, fullDestPnt3,
                                                                nx, ny, nz, iStrideX, iStrideY, iPlaneOffset );
            fDispU3 = TrilinearInterpolationFast<CoordRepType,2>( UData,fullDestPnt1, fullDestPnt2, fullDestPnt3,
                                                                nx, ny, nz, iStrideX, iStrideY, iPlaneOffset );
            //                        std::cout << fDispU1 << ' ' << fDispU2 << ' '  << fDispU3 << std::endl;
            }
            {
            /*This is the actual fGuessU1(x) = pfU1+fDestPnt1 */
            const CoordRepType fGuessU1 = fDispU1 + fDestPnt1;
            /*This is the actual fGuessU2(x) = pfU2+fDestPnt2 */
            const CoordRepType fGuessU2 = fDispU2 + fDestPnt2;
            /*This is the actual fGuessU3(x) = pfU3+fDestPnt3 */
            const CoordRepType fGuessU3 = fDispU3 + fDestPnt3;

            fDelta1 = fBasePnt1 - fGuessU1;
            fDelta2 = fBasePnt2 - fGuessU2;
            fDelta3 = fBasePnt3 - fGuessU3;
            }
            //NOTE: THIS is a hack so that if oscillation occurs, it will get out of it.
            fDestPnt1 += fDelta1;
            fDestPnt2 += fDelta2;
            fDestPnt3 += fDelta3;
            //                    std::cout << iLastIndex << " " << iIteration << " " << 1.0
            //                    << " " << fDelta1 <<" " << fDelta2 << " " << fDelta3
            //                    << " " << fDestPnt1 <<" " << fDestPnt2 << " " << fDestPnt3 <<"              "
            //                    << std::endl;
            iIteration++;
            }
          while ( ( ( fabsf( fDelta1 ) > fEpsilon1 )
                    || ( fabsf( fDelta2 ) > fEpsilon2 )
                    || ( fabsf( fDelta3 ) > fEpsilon3 )
                    )
                  && ( iIteration < INV_LOWERBOUNDCHECK ) );

          //Modification to get out of oscillatory location.
          if(iIteration == INV_LOWERBOUNDCHECK)
            {
            do
              {
              {
              const CoordRepType &fullDestPnt1=fDestPnt1;
              const CoordRepType &fullDestPnt2=fDestPnt2;
              const CoordRepType &fullDestPnt3=fDestPnt3;
              /*Calculate new h(x) */
              fDispU1 = TrilinearInterpolationFast<CoordRepType,0>( UData, fullDestPnt1, fullDestPnt2, fullDestPnt3,
                                                                  nx, ny, nz, iStrideX, iStrideY, iPlaneOffset );
              fDispU2 = TrilinearInterpolationFast<CoordRepType,1>( UData,fullDestPnt1, fullDestPnt2, fullDestPnt3,
                                                                  nx, ny, nz, iStrideX, iStrideY, iPlaneOffset );
              fDispU3 = TrilinearInterpolationFast<CoordRepType,2>( UData,fullDestPnt1, fullDestPnt2, fullDestPnt3,
                                                                  nx, ny, nz, iStrideX, iStrideY, iPlaneOffset );
              }
              {
              /*This is the actual fGuessU1(x) = pfU1+fDestPnt1 */
              const CoordRepType fGuessU1 = fDispU1 + fDestPnt1;
              /*This is the actual fGuessU2(x) = pfU2+fDestPnt2 */
              const CoordRepType fGuessU2 = fDispU2 + fDestPnt2;
              /*This is the actual fGuessU3(x) = pfU3+fDestPnt3 */
              const CoordRepType fGuessU3 = fDispU3 + fDestPnt3;

              fDelta1 = fBasePnt1 - fGuessU1;
              fDelta2 = fBasePnt2 - fGuessU2;
              fDelta3 = fBasePnt3 - fGuessU3;
              }
              //NOTE: THIS is a hack so that if oscillation occurs, it will get out of it.
              fDestPnt1 += ( fDelta1 )*0.5 ;
              fDestPnt2 += ( fDelta2 )*0.5 ;
              fDestPnt3 += ( fDelta3 )*0.5 ;
              iIteration++;
              }
            while ( ( ( fabsf( fDelta1 ) > fEpsilon1 )
                      || ( fabsf( fDelta2 ) > fEpsilon2 )
                      || ( fabsf( fDelta3 ) > fEpsilon3 )
                      )
                    && ( iIteration < INV_MAX_ITERATIONS ) );
            }

          /* Index For Array */
          typename ImageType::IndexType index;
          index[0] = i;
          index[1] = j;
          index[2] = k;
          iLastIndex = UInv->ComputeOffset(index);
          UInvData[iLastIndex][0] = -fDispU1;
          UInvData[iLastIndex][1] = -fDispU2;
          UInvData[iLastIndex][2] = -fDispU3;

          //                std::cerr << "Index " << index << std::flush << std::endl;
#ifdef VERBOSITY_LEVEL_1
          ( iaIterationsNeeded[iIteration] )++;
#endif                   /*VERBOSITY_LEVEL_1*/
          if ( iIteration == INV_MAX_ITERATIONS )
            {
            return -1;
            }
          iIteration = 0;
          }
        }
      }
#ifdef VERBOSITY_LEVEL_1
    printf( "\n" );
    for ( int i = 0; i < INV_MAX_ITERATIONS; i++ )
      {
      if ( iaIterationsNeeded[i] != 0 )
        {
        printf( "i=%15u sCount=%15lu\n", i, iaIterationsNeeded[i] );
        }
      }
#endif                         /*VERBOSITY_LEVEL_1*/
    return 0;
  }
  //  template class <typename CoordRepType, unsigned int Dimension=3>
    //  InverseConsistentLandmarkRegistration : public Object

    /** Set input landmarks for thin plate spline - index 0 = first landmark set, index 1 is target landmark set */
    template <typename CoordRepType, unsigned int Dimension>
        void
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        SetLandmarks(unsigned int index,PointSetTypePointer &Landmarks) // set landmarks[0] and [1]
            {
            m_Pointsets[index] = Landmarks;
            m_PointsAssigned[index] = true;
            }


    /** Transform an Image using TPS algorithm */
    template <typename CoordRepType, unsigned int Dimension>
        typename Image<CoordRepType,Dimension>::Pointer
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        TransformByWMatrix(ImagePointer &inputImage)
            {
            if(!m_XformsComputed[ENUM_STANDARD_DEF])
                {
                ComputeDeformation();
                }
            if(!m_XformsComputed[ENUM_STANDARD_DEF])
                {
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Missing Landmarks");
                throw exception;
                }

            typedef itk::ResampleImageFilter<ImageType,ImageType,CLREG_PRECISION> ResampleImageFilterType;
            typedef itk::LinearInterpolateImageFunction<ImageType,CoordRepType> InterpolatorType;

            typename ResampleImageFilterType::Pointer ResampleFilter = ResampleImageFilterType::New();
            typename InterpolatorType::Pointer theInterpolator = InterpolatorType::New();

            theInterpolator->SetInputImage(inputImage);
            ResampleFilter->SetInterpolator(theInterpolator);

            ResampleFilter->SetTransform(m_LandmarkKernelTransform);
            ResampleFilter->SetSize(m_ImageSize);
            ResampleFilter->SetOutputSpacing(inputImage->GetSpacing());
            ResampleFilter->SetOutputOrigin(inputImage->GetOrigin());
            ResampleFilter->SetInput(inputImage);
            ResampleFilter->SetDefaultPixelValue(128);
            ResampleFilter->Update();
            return ResampleFilter->GetOutput();
            }


    /** Inverse Transform an Image using computed Inverse Consistent deformation vector */
    template <typename CoordRepType, unsigned int Dimension>
        typename Image<CoordRepType,Dimension>::Pointer
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        TransformByVectorField(const DEF_TYPES desiredTransform,
            typename Image<CoordRepType,Dimension>::Pointer &inputImage)
            {
            switch (desiredTransform)
                {
            case ENUM_STANDARD_DEF:
                if(!m_XformsComputed[ENUM_STANDARD_DEF])
                    {
                    ComputeDeformation();
                    }
                break;
            case ENUM_INVERSE_DEF:
                if(!m_XformsComputed[ENUM_INVERSE_DEF])
                    {
                    ComputeInverseDeformation();
                    }
                break;
            default:
                assert(0==1);
                }
            if(!m_XformsComputed[desiredTransform])
                {
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Failed to compute inverse deformation");
                throw exception;
                }
            typename WarpFilterType::Pointer warpFilter = WarpFilterType::New();
            warpFilter->SetInput(inputImage);
            warpFilter->SetDeformationField(m_DeformationFields[desiredTransform]);
            warpFilter->SetOutputOrigin(inputImage->GetOrigin());
            warpFilter->SetOutputSpacing(inputImage->GetSpacing());
            warpFilter->SetEdgePaddingValue(0.5F);
            warpFilter->Update();
            return warpFilter->GetOutput();
            }
    /** Derive the forward deformation */
    template <typename CoordRepType, unsigned int Dimension>
        void
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        ComputeDeformation()
            {
            //
            // need both pointsets set.
            // TODO -- move the code to remove the duplicates and code to add extended
            // landmark points in here.
            for(unsigned int i = 0; i < 2; i++)
                {
                if(!m_PointsAssigned[i])
                    {
                    ExceptionObject exception(__FILE__, __LINE__);
                    exception.SetDescription("Missing Landmarks");
                    throw exception;
                    }
                }
            // Do the transform
            if(m_ImageSize[2] == 1)
              {
              m_LandmarkKernelTransform = WMatrix2DTransformType::New();
              }
            else
              {
              m_LandmarkKernelTransform = WMatrix3DTransformType::New();
              }
            m_LandmarkKernelTransform->SetSourceLandmarks(m_Pointsets[1]);
            m_LandmarkKernelTransform->SetTargetLandmarks(m_Pointsets[0]);
            m_LandmarkKernelTransform->ComputeWMatrix();

            //
            // extract deformation field
            m_DeformationFields[ENUM_STANDARD_DEF] = WarpImageType::New();
            m_DeformationFields[ENUM_STANDARD_DEF]->SetRegions(m_ImageSize);
            m_DeformationFields[ENUM_STANDARD_DEF]->Allocate();
            typename ImageType::IndexType Index;

            for(Index[2] = 0; Index[2] < (long)m_ImageSize[2]; Index[2]++)
                {
                for(Index[1] = 0; Index[1] < (long)m_ImageSize[1]; Index[1]++)
                    {
                    for(Index[0] = 0; Index[0] < (long)m_ImageSize[0]; Index[0]++)
                        {
                        typename WMatrixTransformType::InputPointType ReferencePointInTargetSpace;
                        for(unsigned int i = 0; i < Dimension; i++)
                            {
                            ReferencePointInTargetSpace[i] = Index[i];
                            }
                        const typename WMatrixTransformType::OutputPointType
                            CameFromPoint = m_LandmarkKernelTransform->TransformPoint(ReferencePointInTargetSpace);
                        typename WarpPixelType::ValueType val[3];
                        for(unsigned int i = 0; i < Dimension; i++)
                            {
                            val[i] = CameFromPoint[i] - ReferencePointInTargetSpace[i];
                            }
                        m_DeformationFields[ENUM_STANDARD_DEF]->SetPixel(Index,WarpPixelType(val));
                        }
                    }
                }
            m_XformsComputed[ENUM_STANDARD_DEF] = true;
            }

    /** Computes inverse deformation */
    template <typename CoordRepType, unsigned int Dimension>
        void
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        ComputeInverseDeformation()
            {
            //
            // make sure we have the forward deformation
            if(!m_XformsComputed[ENUM_STANDARD_DEF])
                {
                ComputeDeformation();
                if(!m_XformsComputed[ENUM_STANDARD_DEF])
                    {
                    ExceptionObject exception(__FILE__, __LINE__);
                    exception.SetDescription("Can't compute forward transform");
                    throw exception;
                    }
                }

            //
            // copy the forward deformation into 3 scalar images;
            // TODO -- get InverseTransform3D_OnN to work directly from the
            // deformation vector
            //typedef Image<float,Dimension> TmpImageType;
            //typedef typename TmpImageType::Pointer TmpImagePointer;
            
            m_DeformationFields[ENUM_INVERSE_DEF] = WarpImageType::New();
            m_DeformationFields[ENUM_INVERSE_DEF]->SetRegions(m_ImageSize);
            m_DeformationFields[ENUM_INVERSE_DEF]->Allocate();
            //
            // call InverseTransform3d_OnN
            if(InverseTransform3D_OnN(MY_EPSILON,MY_MAX_ITERATIONS,MY_LOWERBOUNDCHECK) != 0)
                {
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Inverse deformation failed to converge");
                throw exception;
                }
            m_XformsComputed[ENUM_INVERSE_DEF] = true;
            }
    /** Get the Deformation vector used to warp from landmarks[0] to landmarks[1] */
    template <typename CoordRepType, unsigned int Dimension>
        typename Image< Vector<CoordRepType,Dimension> , Dimension>::Pointer
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        GetDeformationVector()
            {
            if(!m_XformsComputed[ENUM_STANDARD_DEF])
                {
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Transform Not Computed Yet");
                throw exception;

                }
            return m_DeformationFields[ENUM_STANDARD_DEF];
            }

    /** Get the  InverseDeformationVector derived by the Inverse Consistent algorithm. */
    template <typename CoordRepType, unsigned int Dimension>
        typename Image< Vector<CoordRepType,Dimension> , Dimension>::Pointer
        InverseConsistentLandmarkRegistration<CoordRepType,Dimension>::
        GetInverseDeformationVector()
            {
            if(!m_XformsComputed[ENUM_INVERSE_DEF])
                {
                ExceptionObject exception(__FILE__, __LINE__);
                exception.SetDescription("Inverse Transform Not Computed Yet");
                throw exception;
                }
            return m_DeformationFields[ENUM_INVERSE_DEF];
            }
}

#endif
