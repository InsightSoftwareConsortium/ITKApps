/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CLReg.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImage.h"

#define CLREG_PRECISION double

#include "itkCastImageFilter.h"
#include "itkIOCommon.h"
#include "itkMetaDataObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkInverseConsistentLandmarkRegistration.h"
#include "itkDifferenceImageFilter.h"
#include "itkInverseConsistentLandmarks.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkMultiThreader.h"
#include "itkOrientImageFilter.h"
#include <itksys/SystemTools.hxx>
#include <string>

#include "itkVectorIndexSelectionCastImageFilter.h"

//TODO:  Make landmarks templated so that they can handle both double and
//       float numbers
//TODO:  Convert the Inverse function from using CImage strided to itk::Image
//TODO:  Convert interpolations from fTrilinear to itk::LinearInterpolator

void DisplacementFieldWriter(itk::Image<itk::Vector<CLREG_PRECISION,3>,3>::Pointer MyField, const std::string & MyFieldName)
{
    typedef itk::CastImageFilter<itk::Image<CLREG_PRECISION,3>,itk::Image<float,3> > ImageCastType;
    typedef itk::Image<itk::Vector<CLREG_PRECISION,3>,3> DeformationFieldType;
    itk::ImageFileWriter<itk::Image<float, 3> >::Pointer DisplacementImageWriter=itk::ImageFileWriter<itk::Image<float, 3> >::New();

    itk::Vector<itk::Image<float ,3>::Pointer,3> Disp;
    Disp[0]=itk::Image<float ,3>::New();
    Disp[1]=itk::Image<float ,3>::New();
    Disp[2]=itk::Image<float ,3>::New();
    typedef itk::VectorIndexSelectionCastImageFilter<DeformationFieldType, itk::Image<CLREG_PRECISION,3> > ComponentFilterType;
    std::string CurrentComponentFilename;
    try
        {
        char ext[3][11]={"_xdisp.img","_ydisp.img","_zdisp.img"};
        for(unsigned int extiter=0;extiter<3;extiter++)
            {
            ImageCastType::Pointer ImageCaster = ImageCastType::New();
            CurrentComponentFilename=MyFieldName+ext[extiter];
            std::cout << "Writing Transform Image: " << CurrentComponentFilename << std::endl;
            ComponentFilterType::Pointer myComponentFilter=ComponentFilterType::New();
            myComponentFilter->SetIndex(extiter);
            myComponentFilter->SetInput(MyField);

            ImageCaster->SetInput(myComponentFilter->GetOutput());
            ImageCaster->Update();
            DisplacementImageWriter->SetInput(ImageCaster->GetOutput());
            DisplacementImageWriter->SetFileName(CurrentComponentFilename.c_str());
            DisplacementImageWriter->Update();
#if 0
            typedef itk::StatisticsImageFilter<itk::Image<float, 3> > StatsType;
            typename StatsType::Pointer ImageStats = StatsType::New();
            ImageStats->SetInput( myComponentFilter->GetOutput());
            ImageStats->Update();
            std::cout << "STATS: sum:" <<ImageStats->GetSum()
                << "min:" << ImageStats->GetMinimum()
                << "max:" << ImageStats->GetMaximum()
                << "var:" << ImageStats->GetVariance()
                << std::endl;
#endif
            }
        }
    catch (itk::ExceptionObject & e)
        {
        std::cerr << "exception in file Displacement File Writer(" <<
            CurrentComponentFilename << ")" << std::endl;
        std::cerr << e.GetDescription() << std::endl;
        std::cerr << e.GetLocation() << std::endl;
        exit(-1);
        }
}

template <class InputPixelType,class CoordRepType,unsigned int Dimension=3>
class MagnitudeFunctor
{
public:
    MagnitudeFunctor() {}
    ~MagnitudeFunctor() {}
    CoordRepType operator()(const InputPixelType &inputPixel)
        {
        CoordRepType mag = 0;
        for(unsigned int i = 0; i < Dimension; i++)
            {
            mag += inputPixel[i] * inputPixel[i];
            }
        mag = sqrt(mag);
        return mag;
        }
};

template <class InputPixelType,unsigned int Dimension=3>
class VecDiffFunctor
{
public:
    VecDiffFunctor() {}
    ~VecDiffFunctor() {}
    InputPixelType operator()(const InputPixelType &a,const InputPixelType b)
        {
        return a - b;
        }
};

template <class WarpImageType,class ImageType>
int
WriteDeformationVector(typename WarpImageType::Pointer &img,
    const char *filenamebase,
    itk::SpatialOrientation::ValidCoordinateOrientationFlags &orientation,
    int *dims)
{

    typename ImageType::Pointer WarpingVector[3];

    for(unsigned int i = 0; i < 3; i++)
        {
        WarpingVector[i] = ImageType::New();
        WarpingVector[i]->CopyInformation(img);
        itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
            (WarpingVector[i]->GetMetaDataDictionary(),
             itk::ITK_CoordinateOrientation, orientation);
        WarpingVector[i]->SetBufferedRegion(WarpingVector[i]->GetLargestPossibleRegion());
        WarpingVector[i]->Allocate();
        }

    typename ImageType::IndexType Index;

    for(Index[2] = 0; Index[2] < dims[2]; Index[2]++)
        {
        for(Index[1] = 0; Index[1] < dims[1]; Index[1]++)
            {
            for(Index[0] = 0; Index[0] < dims[0]; Index[0]++)
                {
                typename WarpImageType::PixelType  DispPoint = img->GetPixel(Index);
                WarpingVector[0]->SetPixel(Index,static_cast<float>(DispPoint[0]));
                WarpingVector[1]->SetPixel(Index,static_cast<float>(DispPoint[1]));
                WarpingVector[2]->SetPixel(Index,static_cast<float>(DispPoint[2]));
                }
            }
        }

    // actually write
    typedef typename itk::ImageFileWriter<ImageType> ImageWriterType;
    typename ImageWriterType::Pointer writer = ImageWriterType::New();

    for(unsigned int i = 0; i < 3; i++)
        {
        std::string fname = std::string(filenamebase) +
            std::string("_") +
            std::string((i == 0 ? "x" : (i == 1 ? "y" : "z"))) +
            std::string(".hdr");
        writer->SetFileName(fname.c_str());
        writer->SetInput(WarpingVector[i]);
        writer->Write();
        }
    return 0;
}


    int
main(int argc, char *argv[])
{
    bool no_inverse = false, twoD = false;
    typedef CLREG_PRECISION CoordRepType;
    typedef itk::InverseConsistentLandmarkRegistration<CoordRepType,3> RegistrationType;
    typedef RegistrationType::ImageType ImageType;
    typedef itk::Image<float,3> ImageSingleType;
    typedef ImageType::Pointer ImagePointer;
    typedef RegistrationType::PointSetType PointSetType;
    typedef PointSetType::Pointer PointSetTypePointer;

    typedef itk::InverseConsistentLandmarks<CoordRepType,PointSetType> LandmarksType;
    typedef LandmarksType::PointType LandmarkPoint;

    typedef PointSetType::PointIdentifier PointIdentifierType;


    typedef RegistrationType::WarpPixelType WarpPixelType;
    typedef RegistrationType::WarpImageType WarpImageType;
    typedef RegistrationType::WarpImagePointer WarpImagePointer;

    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typedef itk::CastImageFilter<ImageType,ImageSingleType> ImageCastType;
    typedef itk::ImageFileWriter<ImageSingleType> ImageWriterType;

    //
    // check a flag to suppress doing the inverse stuff.
    //
    while(argc > 1 && (*++argv)[0] == '-')
      {
      --argc;
      if(itksys::SystemTools::Strucmp(argv[1],"-noinverse") == 0)
        {
        no_inverse = true;
        }
      else if(itksys::SystemTools::Strucmp(*argv,"-2d") == 0)
        {
        twoD = true;
        }
      }

    if(argc < 5)
        {
        std::cerr << "CLReg: Usage: "<< argv[0] << " [-noinverse] ImageA ImageB LandmarksA LandmarksB" <<
            std::endl;
        exit(1);
        }
    itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);
    ImagePointer InputImages[2];

    itk::SpatialOrientation::ValidCoordinateOrientationFlags ImageOrientation;

    for(unsigned int i = 0; i < 2; i++)
        {
        try
            {
            ImageReaderType::Pointer imageReader = ImageReaderType::New();
            imageReader->SetFileName(argv[i]);
            imageReader->Update();
            InputImages[i] = imageReader->GetOutput();
            if (itk::ExposeMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
                (InputImages[i]->GetMetaDataDictionary(),
                 itk::ITK_CoordinateOrientation, ImageOrientation) )
                {
                // if orientation isn't RIP, make it so.
                itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter =
                  itk::OrientImageFilter<ImageType,ImageType>::New();
                orienter->SetGivenCoordinateOrientation(ImageOrientation);
                orienter->SetInput(InputImages[i]);
                orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
                orienter->Update();
                InputImages[i] = orienter->GetOutput();

                }
            else
                {
                //ImageOrientation=Coronal -- as a wild guess inthe absence of actual info
                itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
                  (InputImages[i]->GetMetaDataDictionary(),
                   itk::ITK_CoordinateOrientation, 
                   itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
                }
            }
        catch (itk::ExceptionObject e)
            {
            std::cerr << "lmkrg: Can't open " << argv[i] << std::endl;
            exit(-1);
            }
        }

    // get image dimensions
    // the Dimensions in ITK are stored fastest moving
    // first
    ImageType::RegionType InputImageRegion = InputImages[0]->GetLargestPossibleRegion();

    ImageType::RegionType::SizeType InputImageSize = InputImageRegion.GetSize();
    unsigned int imageDim = InputImages[0]->GetImageDimension();
    int dims[4] = { 1,1,1,1 };

    for(unsigned int i = 0; i < imageDim; i++)
        {
        dims[i] = InputImageSize.GetElement(i);
        }

    LandmarksType InputLandmarks[2];
    unsigned int InputLandmarkSizes[2];
    //  InputLandmarks[0].setXDim( dims[0] );
    //  InputLandmarks[0].setYDim( dims[1] );
    //  InputLandmarks[0].setZDim( dims[2] );
    //  InputLandmarks[0].setTDim( dims[3] );

    //  InputLandmarks[1].setXDim( dims[0] );
    //  InputLandmarks[1].setYDim( dims[1] );
    //  InputLandmarks[1].setZDim( dims[2] );
    //  InputLandmarks[1].setTDim( dims[3] );
    bool test;


    for(unsigned int i = 0; i < 2; i++)
        {
        test = InputLandmarks[i].ReadPointTypes( argv[i+2] );
        if(test == false)
            {
            std::cout << "ERROR in Read: " << argv[i+2] << std::endl;
            exit(-1);
            }
        InputLandmarkSizes[i] = InputLandmarks[i].size();
        }


    LandmarksType ExtendedLandmarks[2];

    std::cerr << "InputLandmarkSizes[0] = " << InputLandmarkSizes[0] <<
        " InputLandmarkSizes[1] " << InputLandmarkSizes[1] << std::endl;

    for(unsigned int i = 0; i < 2; i++)
        {
        InputLandmarks[i].RemoveUnmatchedPoints(InputLandmarks[(i+1) % 2]);
        InputLandmarks[(i+1) % 2].RemoveUnmatchedPoints(InputLandmarks[i]);
        }
    for(unsigned int i = 0; i < 2; i++)
        {
        InputLandmarkSizes[i] = InputLandmarks[i].size();
        }
    std::cerr << "Remove Unmatched Points: InputLandmarkSizes[0] = " << InputLandmarkSizes[0]<<
        " InputLandmarkSizes[1] " << InputLandmarkSizes[1] << std::endl;

    PointSetTypePointer LandmarksForAData;
    PointSetTypePointer LandmarksForBData;
    if(no_inverse==false)
        {
        //
        // extend landmarks around original image volume
        //ExtendedLandmarks[0].AddExtendedPointTypes3D_UnitCube(InputLandmarks[0]);
        //ExtendedLandmarks[1].AddExtendedPointTypes3D_UnitCube(InputLandmarks[1]);
        ExtendedLandmarks[0].AddExtendedPointTypes3D_OnN(InputLandmarks[0],dims[0],dims[1],dims[2]);
        ExtendedLandmarks[1].AddExtendedPointTypes3D_OnN(InputLandmarks[1],dims[0],dims[1],dims[2]);
        InputLandmarkSizes[0] = ExtendedLandmarks[0].size();
        InputLandmarkSizes[1] = ExtendedLandmarks[1].size();
        std::cerr << "AddExtendedPointTypes : InputLandmarkSizes[0] = " <<
            InputLandmarkSizes[0] << " InputLandmarkSizes[1] " << InputLandmarkSizes[1] << std::endl;
        LandmarksForAData = ExtendedLandmarks[0].GetPointSet();
        LandmarksForBData = ExtendedLandmarks[1].GetPointSet();
        }
    else
        {
        LandmarksForAData = InputLandmarks[0].GetPointSet();
        LandmarksForBData = InputLandmarks[1].GetPointSet();
        }
#if 0
       ExtendedLandmarks[0].PrintPointTypes();
       ExtendedLandmarks[1].PrintPointTypes();
        {
        typedef PointSetType::PointsContainer::ConstIterator PointDataIterator;
        PointDataIterator pointDataIteratorA=LandmarksForAData->GetPoints()->Begin();
        PointDataIterator pointDataIteratorB=LandmarksForBData->GetPoints()->Begin();
        while ( pointDataIteratorA != LandmarksForAData->GetPoints()->End() && pointDataIteratorB != LandmarksForBData->GetPoints()->End() )
            {
            std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@2[" << pointDataIteratorA.Value() << "]-["
                << pointDataIteratorB.Value() <<"]"
                << "= ["<< pointDataIteratorA.Value()-pointDataIteratorB.Value() << "]" <<std::endl;
            pointDataIteratorA++;
            pointDataIteratorB++;
            }
        }
#endif
    RegistrationType::Pointer LMRForward = RegistrationType::New();
    LMRForward->SetRegion(InputImageRegion);
    LMRForward->SetLandmarks(0,LandmarksForAData);
    LMRForward->SetLandmarks(1,LandmarksForBData);
    try
        {
        LMRForward->ComputeDeformation();
        }
    catch (itk::ExceptionObject e)
        {
        std::cerr << e << std::endl;
        }
    RegistrationType::TransformPointerType forwardXfrom= LMRForward->GetLandmarkKernelTransform();
        {
        typedef PointSetType::PointsContainer::ConstIterator PointDataIterator;
        }

    WarpImagePointer WarpForward = LMRForward->GetDeformationVector();
    DisplacementFieldWriter(WarpForward, std::string("WarpForward"));

    WarpImagePointer WarpInverseForward;
//    ImagePointer WarpedForwardImage = LMRForward->TransformByWMatrix(InputImages[0]);
    ImagePointer WarpedForwardImage = LMRForward->TransformByVectorField(RegistrationType::ENUM_STANDARD_DEF,InputImages[0]);
    itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
        (WarpedForwardImage->GetMetaDataDictionary(),itk::ITK_CoordinateOrientation, ImageOrientation);
    //
    // write out warped image
    ImageWriterType::Pointer ImageWriter = ImageWriterType::New();
    ImageCastType::Pointer ImageCaster = ImageCastType::New();
    ImageWriter->SetFileName("WarpedForwardImage.hdr");
    ImageCaster->SetInput(WarpedForwardImage);
    ImageCaster->Update();
    ImageWriter->SetInput(ImageCaster->GetOutput());
    ImageWriter->Write();

    if(!no_inverse)
        {
        LMRForward->ComputeInverseDeformation();
        WarpInverseForward = LMRForward->GetInverseDeformationVector();
        DisplacementFieldWriter(WarpInverseForward, std::string("WarpInverseForward"));
        //
        // invert the warped image
        ImageType::Pointer WarpedInverseForwardImage;
        WarpedInverseForwardImage = LMRForward->TransformByVectorField(RegistrationType::ENUM_INVERSE_DEF,InputImages[1]);
        WarpedInverseForwardImage->CopyInformation(WarpedForwardImage);
        itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
            (WarpedInverseForwardImage->GetMetaDataDictionary(),
             itk::ITK_CoordinateOrientation, ImageOrientation);
        ImageWriter->SetFileName("WarpedInverseForwardImage.hdr");
        ImageCaster->SetInput(WarpedInverseForwardImage);
        ImageCaster->Update();
        ImageWriter->SetInput(ImageCaster->GetOutput());
        ImageWriter->Write();
        }

    typedef itk::DifferenceImageFilter<ImageType,ImageType> DiffImageFilterType;
#if 0
    //
    // diff the second image and the warped image
    DiffImageFilterType::Pointer DiffImageFilter = DiffImageFilterType::New();
    DiffImageFilter->SetDifferenceThreshold(0.1);
    DiffImageFilter->SetToleranceRadius(0);
    DiffImageFilter->SetValidInput(InputImages[1]);
    DiffImageFilter->SetTestInput(WarpedForwardImage);
    DiffImageFilter->Update();

    ImagePointer DiffImage = DiffImageFilter->GetOutput();
    itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>(DiffImage->GetMetaDataDictionary(),itk::ITK_CoordinateOrientation, ImageOrientation);
    ImageWriter->SetFileName("DifferenceImageAB-B.hdr");
    ImageCaster->SetInput(DiffImage);
    ImageCaster->Update();
    ImageWriter->SetInput(ImageCaster->GetOutput());
    ImageWriter->Write();
#endif

    //
    // diff the original image and the inverse of the warped image
#if 0
    DiffImageFilter->SetValidInput(InputImages[0]);
    DiffImageFilter->SetTestInput(WarpedInverseForwardImage);
    DiffImageFilter->Update();
    ImagePointer DiffWarpedInverted = DiffImageFilter->GetOutput();
    ImageWriter->SetFileName("DifferenceImageInverseAB-A.hdr");
    ImageCaster->SetInput(DiffImageFilter->GetOutput());
    ImageCaster->Update();
    ImageWriter->SetInput(ImageCaster->GetOutput());
    ImageWriter->Write();
#endif

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    //
    // convert the forward deformation of the points in reverse.
    // this should be near the same as the INVERSE of the points in
    // the opposite order.
    RegistrationType::Pointer LMRReverse = RegistrationType::New();
    LMRReverse->SetRegion(InputImageRegion);
    LMRReverse->SetLandmarks(0,LandmarksForBData);
    LMRReverse->SetLandmarks(1,LandmarksForAData);
    try
        {
        LMRReverse->ComputeDeformation();
        }
    catch (itk::ExceptionObject e)
        {
        std::cerr << e << std::endl;
        }
    WarpImagePointer WarpReverse = LMRReverse->GetDeformationVector();
    DisplacementFieldWriter(WarpReverse, std::string("WarpReverse"));

    //
    // invert the warped image
    //ImagePointer WarpedReverseImage = LMRReverse->TransformByWMatrix(InputImages[1]);
    ImagePointer WarpedReverseImage = LMRReverse->TransformByVectorField(RegistrationType::ENUM_STANDARD_DEF,InputImages[1]);
    //
    // write out warped image
    itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
        (WarpedReverseImage->GetMetaDataDictionary(),itk::ITK_CoordinateOrientation, ImageOrientation);

    ImageWriter->SetFileName("WarpedReverseImage.hdr");
    ImageCaster->SetInput(WarpedReverseImage);
    ImageCaster->Update();
    ImageWriter->SetInput(ImageCaster->GetOutput());
    ImageWriter->Write();


#if 0
    //
    // an image of vectors blows the difference image filter's mind
    //
    typedef itk::DifferenceImageFilter<WarpImageType,WarpImageType> WarpDiffType;
#else
    typedef itk::BinaryFunctorImageFilter<WarpImageType,WarpImageType,WarpImageType,
    VecDiffFunctor<WarpPixelType,3> > WarpDiffType;
#endif

    if(!no_inverse)
        {
        LMRReverse->ComputeInverseDeformation();
        WarpImagePointer WarpInverseReverse = LMRReverse->GetInverseDeformationVector();
        DisplacementFieldWriter(WarpInverseReverse, std::string("WarpInverseReverse"));
        ImageType::Pointer WarpedInverseReverseImage = LMRReverse->TransformByVectorField(RegistrationType::ENUM_INVERSE_DEF,InputImages[0]);
        WarpedInverseReverseImage->CopyInformation(WarpedReverseImage);
        itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
            (WarpedInverseReverseImage->GetMetaDataDictionary(),itk::ITK_CoordinateOrientation, ImageOrientation);
        ImageWriter->SetFileName("WarpedInverseReverseImage.hdr");
        ImageCaster->SetInput(WarpedInverseReverseImage);
        ImageCaster->Update();
        ImageWriter->SetInput(ImageCaster->GetOutput());
        ImageWriter->Write();
        //
        //
        //
        //
        //
        //
        //
        //
        WarpDiffType::Pointer warpDiffFilter = WarpDiffType::New();
        warpDiffFilter->SetInput1(WarpInverseReverse);
        warpDiffFilter->SetInput2(WarpForward);
        warpDiffFilter->Update();
        WarpImagePointer warpDiffImage = warpDiffFilter->GetOutput();

        typedef MagnitudeFunctor<WarpPixelType,CoordRepType,3> MagFunctorType;

        typedef itk::UnaryFunctorImageFilter<WarpImageType,ImageType,MagFunctorType> MagFilterType;
        MagFilterType::Pointer magFilter = MagFilterType::New();
        magFilter->SetInput(warpDiffImage);
        magFilter->Update();
        ImageCaster->SetInput(magFilter->GetOutput());
        ImageCaster->Update();
        ImageWriter->SetInput(ImageCaster->GetOutput());
        ImageWriter->SetFileName("MagnitudeInvFor-Rev.hdr");
        ImageWriter->Write();
        }
    return 0;
}
