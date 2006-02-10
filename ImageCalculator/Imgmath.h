#ifndef _ITKMATH_H_
#define _ITKMATH_H_


/*This file contains the functions to perform pixel by pixel mathematical operations on 2 images. All the functions are performed by using ITK filters.*/

#include "itkImage.h"
#include "itkSquaredDifferenceImageFilter.h"
#include"itkImageRegionIterator.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include <vcl_cmath.h>


/* Iadd adds 2 images at every pixel location and outputs the resulting image.*/


    template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer Iadd(typename itk::Image< PixelType, Dimension >::Pointer input1,typename itk::Image< PixelType, Dimension >::Pointer input2 )
{

    typename itk::Image< PixelType, Dimension > ::Pointer image =  itk::Image< PixelType, Dimension >::New();
    typedef typename itk::Image< PixelType, Dimension > ImageType;

    typedef itk::AddImageFilter<ImageType,ImageType,ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( input1  );
    filter->SetInput2( input2 );
    try
        {
        filter->Update();
        }
    catch (itk::ExceptionObject & err)
        {
        std::cout << "Exception Object caught: " << std::endl;
        std::cout << err << std::endl;
        exit(-1);
        }
    image=filter->GetOutput();

    return image;


}



/* Isub subtracts 2 images at every pixel location and outputs the resulting image.*/

    template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer Isub(typename itk::Image< PixelType, Dimension >::Pointer input1,typename itk::Image< PixelType, Dimension >::Pointer input2 )
{
    typename itk::Image< PixelType, Dimension > ::Pointer image =  itk::Image< PixelType, Dimension >::New();

    typedef typename itk::Image< PixelType, Dimension > ImageType;

    typedef itk::SubtractImageFilter<ImageType,ImageType,ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( input1  );
    filter->SetInput2( input2 );
    try
        {
        filter->Update();
        }
    catch (itk::ExceptionObject & err)
        {
        std::cout << "Exception Object caught: " << std::endl;
        std::cout << err << std::endl;
        exit(-1);
        }
    image=filter->GetOutput();

    return image;

}

/* Imul multiplies 2 images at every pixel location and outputs the resulting image.*/

    template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer Imul(typename itk::Image< PixelType, Dimension >::Pointer input1,typename itk::Image< PixelType, Dimension >::Pointer input2 )
{

    typename itk::Image< PixelType, Dimension > ::Pointer image =  itk::Image< PixelType, Dimension >::New();

    typedef typename itk::Image< PixelType, Dimension > ImageType;

    typedef itk::MultiplyImageFilter<ImageType,ImageType,ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( input1  );
    filter->SetInput2( input2 );
    try
        {
        filter->Update();
        }
    catch (itk::ExceptionObject & err)
        {
        std::cout << "Exception Object caught: " << std::endl;
        std::cout << err << std::endl;
        exit(-1);
        }
    image=filter->GetOutput();

    return image;

}

/* Idiv divides 2 images at every pixel location and outputs the resulting image.*/

    template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer Idiv(typename itk::Image< PixelType, Dimension >::Pointer input1,typename itk::Image< PixelType, Dimension >::Pointer input2 )
{
    typename itk::Image< PixelType, Dimension > ::Pointer image =  itk::Image< PixelType, Dimension >::New();

    typedef typename itk::Image< PixelType, Dimension > ImageType;

    typedef itk::DivideImageFilter<ImageType,ImageType,ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( input1  );
    filter->SetInput2( input2 );
    try
        {
        filter->Update();
        }
    catch (itk::ExceptionObject & err)
        {
        std::cout << "Exception Object caught: " << std::endl;
        std::cout << err << std::endl;
        exit(-1);
        }
    image=filter->GetOutput();

    return image;

}


/* Iavg takes an image and the number of images as inputs , divides each pixel location of the image by the number of images outputs the resulting image.*/


template <class PixelType, int Dimension>                                       typename itk::Image< PixelType, Dimension >::Pointer Iavg(typename itk::Image< PixelType, Dimension >::Pointer input1, int nimgs)
{
    typename itk::Image< PixelType, Dimension > ::Pointer image =  itk::Image< PixelType, Dimension >::New();
    image->SetRegions(input1->GetBufferedRegion());
    image->CopyInformation(input1);
    image->Allocate();
    typedef itk::ImageRegionIterator< typename itk::Image< PixelType, Dimension > > ConstIteratorType;
    ConstIteratorType in1(input1 , input1->GetBufferedRegion());
    ConstIteratorType out(image , image->GetBufferedRegion());
    for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
        {
        out.Set(in1.Get()/nimgs);
        }

    return image;

}                                         

/*ImageMultiplyConstant multiplies the entire image with a constant value and outputs the resultant image*/

    template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer ImageMultiplyConstant(typename itk::Image< PixelType, Dimension >::Pointer input1 , typename itk::Image< PixelType, Dimension >::PixelType constant)
{


    typedef itk::ImageRegionIterator< typename itk::Image< PixelType, Dimension > > ConstIteratorType;
    ConstIteratorType in1(input1 , input1->GetBufferedRegion());
    for(in1.GoToBegin() ; !in1.IsAtEnd() ; ++in1)
        {
        in1.Set((in1.Get()*constant));
        }

    return input1;
}

/*ImageDivideConstant divides the entire image with a constant value and outputs the resultant image*/

    template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer ImageDivideConstant(typename itk::Image< PixelType, Dimension >::Pointer input1 , typename itk::Image< PixelType, Dimension >::PixelType constant)
{


    typedef itk::ImageRegionIterator< typename itk::Image< PixelType, Dimension
        > > ConstIteratorType;
    ConstIteratorType in1(input1 , input1->GetRequestedRegion());
    for(in1.GoToBegin() ; !in1.IsAtEnd() ; ++in1)
        {
        in1.Set((in1.Get()/constant));
        }

    return input1;
}

#endif
