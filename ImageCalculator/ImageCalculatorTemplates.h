#if !defined(__ImageCalculatorTemplates_h____)
#define __ImageCalculatorTemplates_h____
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "Imgmath.h"
#include "itkAbsImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkOrientImageFilter.h"
#include "itkSpatialOrientation.h"
#include "itkAnalyzeImageIO.h"
#include "itkMetaDataObject.h"
#include "itkLabelStatisticsImageFilter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vcl_cmath.h>
#include "ImageCalculatorUtils.h"
#include <metaCommand.h>

/*This function if called performs arithmetic operation with a constant value to all the pixels in an input image.*/
    template <class PixelType , int dims>
typename itk::Image< PixelType, dims >::Pointer Ifilters(typename itk::Image< PixelType, dims >::Pointer input ,  MetaCommand command)
{
    typedef itk::Image< PixelType, dims> ImageType;
    typename ImageType::Pointer image =input;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    IteratorType in1(input , input->GetRequestedRegion());
    IteratorType out(image , input->GetRequestedRegion());

    /*Multiplies a constant value to all the pixels of the Input image.*/
    if(command.GetValueAsString("IMulC","constant")!= "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("IMulC","constant"));
        std::cout << "Multiplying Input image with" << temp;
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() * temp );
            }

        }
    /*Divides a constant value from all the pixels of the Input image.*/
    if(command.GetValueAsString("IDivC","constant")!= "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("IDivC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get()/temp );
            }

        }

    /*Adds a constant value to all the pixels of the Input image.*/
    if(command.GetValueAsString("IAddC","constant") != "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("IAddC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() + temp );
            }

        }

    /*Subtracts a constant value from all the pixels of the Input image.*/
    if(command.GetValueAsString("ISubC","constant") != "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("ISubC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() - temp );
            }

        }


    /*Make Binary Output image.*/
    if(command.GetValueAsBool("Ifbin","ifbin") )
        {
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set((in1.Get() > 0) ? 255 : 0 );
            }
        }

    /*Squares the pixels of the Input image.*/
    if(command.GetValueAsBool("ISqr","ifsqr") )
        {
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() * in1.Get() );
            }
        }

    /*Takes the square root of the pixels in the Input image.*/
    if(command.GetValueAsBool("ISqrt","ifsqrt") )
        {
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(static_cast<PixelType>(vcl_sqrt(
                        static_cast<double>(in1.Get()))));
            }
        }

    return image;
}

/*This function if called performs arithmetic operation with a constant value to all the pixels in the output image.*/
    template <class PixelType , int dims>
typename itk::Image< PixelType, dims >::Pointer Ofilters(typename itk::Image< PixelType, dims >::Pointer input , MetaCommand command)
{
    typedef itk::Image< PixelType, dims> ImageType;
    typename ImageType::Pointer image =input;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    IteratorType in1(input , input->GetRequestedRegion());
    IteratorType out(image , input->GetRequestedRegion());

    /*Multiplies a constant value to all the pixels of the Output image.*/
    if(command.GetValueAsString("OMulC","constant")!= "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("OMulC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() * temp );
            }

        }
    /*Divides a constant value from all the pixels of the Output image.*/
    if(command.GetValueAsString("ODivC","constant")!= "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("ODivC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get()/temp );
            }

        }

    /*Adds a constant value to all the pixels of the Output image.*/
    if(command.GetValueAsString("OAddC","constant") != "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("OAddC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() + temp );
            }

        }

    /*Subtracts a constant value from all the pixels of the Output image.*/
    if(command.GetValueAsString("OSubC","constant") != "" )
        {
        const PixelType temp=static_cast<PixelType>(command.GetValueAsInt("OSubC","constant"));
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() - temp );
            }

        }
    /*Squares the pixels of the Output image.*/
    if(command.GetValueAsBool("OSqr","ofsqr") )
        {
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(in1.Get() * in1.Get() );
            }
        }

    /*Make Binary Output image.*/
    if(command.GetValueAsBool("Ofbin","ofbin") )
        {
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set((in1.Get() > 0) ? 255 : 0 );
            }
        }


    /*Takes the square root of the pixels in the Output image.*/
    if(command.GetValueAsBool("OSqrt","ofsqrt") )
        {
        for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
            {
            out.Set(static_cast<PixelType>(vcl_sqrt(
                        static_cast<double>(in1.Get()))));
            }
        }

    return image;
}


/*statfilters performs user specified statistical operations on the output image.*/
    template <class PixelType , int dims>
void statfilters(typename itk::Image< PixelType, dims >::Pointer AccImage , MetaCommand command)
{

    typedef itk::Image< PixelType, dims> ImageType;
    std::map<std::string,std::string> StatDescription;
    std::map<std::string,float>       StatValues;


    //The statistics image filter calclates all the statistics of AccImage
    typedef itk::StatisticsImageFilter<ImageType> StatsFilterType;
    typename StatsFilterType::Pointer Statsfilter = StatsFilterType::New();
    Statsfilter->SetInput(AccImage);
    Statsfilter->Update();

    //The absolute Image filter calculates the absolute value of the pixels.
    typedef itk::AbsImageFilter<ImageType , ImageType> AbsFilterType;
    typename AbsFilterType::Pointer Absfilter = AbsFilterType::New();
    Absfilter->SetInput(AccImage);
    Absfilter->Update();
    typename StatsFilterType::Pointer AbsStatsfilter = StatsFilterType::New();
    AbsStatsfilter->SetInput(Absfilter->GetOutput());
    AbsStatsfilter->Update();


    bool havestatmask=false;
    int MaskValue = 0;
    //If user gives an Input Mask Calculate the statistics of the image in the mask
    typedef itk::ImageFileReader<itk::Image < unsigned int , dims> > ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();

    typedef itk::LabelStatisticsImageFilter<ImageType , itk::Image < unsigned int , dims> > LabelFilterType;
    typename LabelFilterType::Pointer MaskStatsfilter = LabelFilterType::New();
    typename LabelFilterType::Pointer MaskAbsStatsfilter = LabelFilterType::New();
    if( command.GetValueAsString("Statmask","File Name") != "" )
        {
        reader->SetFileName(command.GetValueAsString("Statmask","File Name").c_str());
        try
            {
            reader->Update();
            }
        catch( itk::ExceptionObject & excp )
            {
            std::cerr << "Error reading the series " << std::endl;
            std::cerr << excp << std::endl;
            throw excp;
            }
        havestatmask=true;
        if( command.GetValueAsString("Statmaskvalue","constant") == "" )
            {std::cout<<"Error: If a mask image is given, a pixel value should be entered and the Statistics in the input image will be calculated for the pixels masked by this value.\n Skipping Statistics , Writing output Image .\n";
            return;
            }
        MaskValue = command.GetValueAsInt("Statmaskvalue","constant");
        MaskStatsfilter->SetInput(AccImage);
        MaskStatsfilter->SetLabelInput(reader->GetOutput());
        MaskStatsfilter->Update();

        //Absolute Values of the Masked Output Image
        typename AbsFilterType::Pointer MaskAbsfilter = AbsFilterType::New();
        MaskAbsfilter->SetInput(AccImage);
        MaskAbsfilter->Update();
        //Statistics of The Absolute Masked Output Image.
        MaskAbsStatsfilter->SetInput(MaskAbsfilter->GetOutput());
        MaskAbsStatsfilter->SetLabelInput(reader->GetOutput());
        MaskAbsStatsfilter->Update();
        }
    StatDescription["AVG:"]="Average of all pixel values";
    StatDescription["MAVG:"]="Average of all pixel values where mask > 0";
    if(command.GetValueAsBool("StatAvg","statAVG"))
        {
        if(havestatmask)
            {
            StatValues["MAVG:"]= MaskStatsfilter->GetMean(MaskValue);
            }
        else
            {
            StatValues["AVG:"]=Statsfilter->GetMean();
            }
        }
    StatDescription["VAR:"]="Variance of all pixel values";
    StatDescription["MVAR:"]="Variance of all pixel values where mask > 0";
    if(command.GetValueAsBool("StatVAR","statVAR") )
        {
        if(havestatmask)
            {
            StatValues["MVAR:"]= MaskStatsfilter->GetVariance(MaskValue);
            }
        else
            {
            StatValues["VAR:"]=Statsfilter->GetVariance();
            }
        }
    StatDescription["SUM:"]="Sum of all pixel values";
    StatDescription["MSUM:"]="Sum of all pixel values where mask > 0";
    if(command.GetValueAsBool("StatSUM","statSUM"))
        {
        if(havestatmask)
            {
            StatValues["MSUM:"]= MaskStatsfilter->GetSum(MaskValue);
            }
        else
            {
            StatValues["SUM:"]=Statsfilter->GetSum();
            }
        }
    StatDescription["MIN:"]="Minimum of all pixel values";
    StatDescription["MMIN:"]="Minimum of all pixel values where mask > 0";
    if(command.GetValueAsBool("StatMIN","statMIN") )
        {
        if(havestatmask)
            {
            StatValues["MMIN:"]= MaskStatsfilter->GetMinimum(MaskValue);
            }
        else
            {
            StatValues["MIN:"]=Statsfilter->GetMinimum();
            }
        }
    StatDescription["MAX:"]="Maximum of all pixel values";
    StatDescription["MMAX:"]="Maximum of all pixel values where mask > 0";
    if(command.GetValueAsBool("StatMAX","statMAX"))
        {
        if(havestatmask)
            {
            StatValues["MMAX:"]= MaskStatsfilter->GetMaximum(MaskValue);
            }
        else
            {
            StatValues["MAX:"]=Statsfilter->GetMaximum();
            }
        }
    StatDescription["AMN:"]="Minimum of the absolute value of the pixel values";
    StatDescription["MAMN:"]="Minimum of the absolute value of the pixel values where mask > 0";
    if(command.GetValueAsBool("StatAMN","statAMN") )
        {
        if(havestatmask)
            {
            StatValues["MAMN:"]= MaskAbsStatsfilter->GetMinimum(MaskValue);
            }
        else
            {
            StatValues["AMN:"]=AbsStatsfilter->GetMinimum();
            }
        }
    StatDescription["AMX:"]="Maximum of the absolute value of the pixel values";
    StatDescription["MAMX:"]="Maximum of the absolute value of the pixel values where mask > 0";
    if(command.GetValueAsBool("StatAMX","statAMX") )
        {
        if(havestatmask)
            {
            StatValues["MAMX:"]= MaskAbsStatsfilter->GetMaximum(MaskValue);
            }
        else
            {
            StatValues["AMX:"]=AbsStatsfilter->GetMaximum() ;
            }
        }
    StatDescription["NPX:"]="Number of pixels used in calculations";
    StatDescription["MNPX:"]="Number of pixels used in calculations where mask > 0";
    if(command.GetValueAsBool("StatNPX","statNPX") )
        {
        if(havestatmask)
            {

            typename ImageType::SizeType  size;
            size = reader->GetOutput()->GetRequestedRegion().GetSize();
            float NumberOfPixels = size[0]*size[1];
            if(dims==3)
                {NumberOfPixels = NumberOfPixels*size[2];}
            StatValues["NPX:"]= NumberOfPixels;
            }
        else
            {
            typename ImageType::SizeType  size;
            size = AccImage->GetRequestedRegion().GetSize();
            float NumberOfPixels = size[0]*size[1];
            if(dims==3)
                {NumberOfPixels = NumberOfPixels*size[2];}
            StatValues["NPX:"]= NumberOfPixels;
            }
        }
    //Show the stat values which can be calculated.
    if(command.GetValueAsBool("Statallcodes","statallcodes")  )
        {
        for(std::map<std::string,std::string>::const_iterator p=StatDescription.begin(); p != StatDescription.end(); p++)
            {
            std::cout << p->first << '\t' << p->second << std::endl;
            }
        std::cout << std::endl;
        }
    //Print the value map
    if((command.GetValueAsString("Statmask","File Name") != "") ||
        command.GetValueAsBool("StatAvg","statAVG") ||
        command.GetValueAsBool("StatVAR","statVAR") ||
        command.GetValueAsBool("StatSUM","statSUM") ||
        command.GetValueAsBool("StatNPX","statNPX")  )
        {
        if(command.GetValueAsString("OutputFilename","filename") != "")
            {
            std::cout << "Stats for "<<command.GetValueAsString("OutputFilename","filename") << '\t';
            }
        }
        {
        for(std::map<std::string,float>::const_iterator p=StatValues.begin(); p != StatValues.end(); p++)
            {
            std::cout << p->first << ' ' << p->second << ",  ";
            }
        std::cout << std::endl;
        }
    return;
}

/*This function is called when the user wants to write the ouput image to a file. The output image is typecasted to the user specified data type. */
    template <class InPixelType , class PixelType , int dims>
void Typecastimage( typename itk::Image< InPixelType, dims >::Pointer AccImage , MetaCommand command )
{


    typedef itk::Image<InPixelType , dims>  InputImageType;
    typedef itk::Image<PixelType , dims>    RealImageType;
    typedef itk::CastImageFilter< InputImageType, RealImageType> CastToRealFilterType;
    typename CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();


    typedef itk::ImageFileWriter< RealImageType > WriterType;
    toReal->SetInput( AccImage );
    toReal->Update();
    typename RealImageType::Pointer image = RealImageType::New ();
    image = toReal->GetOutput ();

    if (dims==3)
        {

        //The output Image orientation is set to Input Image orientation.
        itk::SpatialOrientation::ValidCoordinateOrientationFlags inputimage_orient;
        std::cout<<"Setting orientation of the output image."<<std::endl;

        itk::ExposeMetaData <
            itk::SpatialOrientation::ValidCoordinateOrientationFlags >
            (AccImage->GetMetaDataDictionary (), itk::ITK_CoordinateOrientation,
             inputimage_orient);

        itk::EncapsulateMetaData <
            itk::SpatialOrientation::ValidCoordinateOrientationFlags >
            (image->GetMetaDataDictionary (), itk::ITK_CoordinateOrientation,
             inputimage_orient);

        }




    typename  WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(command.GetValueAsString("OutputFilename","filename").c_str() );
    writer->SetInput(image);

    try {
        writer->Update();
    }
    catch (itk::ExceptionObject &err) {
        std::cout << "Exception Object caught: " << std::endl;
        std::cout << err << std::endl;
        exit(-1);
    }


}

class string_tokenizer : public std::vector<std::string> {
public:
    string_tokenizer(const std::string &s, const char *const sep = " ")
        {
        this->init(s,sep);
        }
    string_tokenizer(const char *const s, const char *const sep = " ")
        {
        this->init(std::string(s),sep);
        }

protected:
    string_tokenizer &operator=(const string_tokenizer &rhs) { return *this; };//explicitly prevent this
    string_tokenizer(const string_tokenizer &rhs) {};//explicitly prevent this
private:
    void init(const std::string &input, const char *const sep = " ")
        {
        std::string::size_type start, _end = 0;
        int i = 0;
        while((start = input.find_first_not_of(sep,_end)) != std::string::npos)
            {
            this->resize(i+1);
            _end = input.find_first_of(sep,start);
            (*this)[i] = input.substr(start, _end - start);
            i++;
            }
        }
};

/*This fuction reads in the input images and writes the output image , delegating the computations to other functions*/
    template <class PixelType , int dims>
void ImageCalculatorReadWrite( MetaCommand command )
{
    string_tokenizer InputList(command.GetValueAsString("in")," ");

    typedef itk::Image<PixelType,dims> ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;

    //Read the first Image
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(InputList.at(0).c_str());
    std::cout << "Reading 1st Image..." << InputList.at(0).c_str() << std::endl;
    try
        {
        reader->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Error reading the series " << std::endl;
        throw excp;
        }


    typename ImageType::SizeType  size;
    size = reader->GetOutput()->GetRequestedRegion().GetSize();

    typename ImageType::SpacingType  space;
    space=reader->GetOutput()->GetSpacing();


    const int nx = size[0];
    const int ny = size[1];
    const int nz = size[2];

    const double vx = space[0];
    const double vy = space[1];
    const double vz = space[2];

    itk::SpatialOrientation::ValidCoordinateOrientationFlags Firstimage_orient;
    if (dims == 3)
        {
        //Grab the orientation of the first image.
        itk::ExposeMetaData <
            itk::SpatialOrientation::ValidCoordinateOrientationFlags >
            (reader->GetOutput()->GetMetaDataDictionary (), itk::ITK_CoordinateOrientation,
             Firstimage_orient);

        }

    //Create an Accumulator Image.
    typename ImageType::Pointer AccImage = ImageType::New();

    AccImage = Ifilters<PixelType , dims>(reader->GetOutput(),command);
    typename ImageType::Pointer SqrImageSum = ImageType::New();
    /*For variance image first step is to square the input image.*/
    if(command.GetValueAsBool("Var","var") )
        {

        SqrImageSum = Imul<PixelType , dims>(reader->GetOutput() , reader->GetOutput());
        }


    /* Accumulator contains the first image initially and is updated by the next image at each count */
    for(unsigned int currimage=1; currimage < InputList.size(); currimage++)
        {
        std::cout << "Reading image.... " << InputList.at(currimage).c_str() << std::endl;

        typename ReaderType::Pointer reader2 = ReaderType::New();
        reader2->SetFileName( InputList.at(currimage).c_str() );
        try
            {
            reader2->Update();
            }
        catch( itk::ExceptionObject & excp )
            {
            std::cerr << "Error reading the series " << std::endl;
            throw excp;
            }


        typename ImageType::Pointer image = Ifilters<PixelType , dims>(reader2->GetOutput() ,command);
        typename ImageType::SizeType size;
        size = image->GetRequestedRegion().GetSize();

        typename ImageType::SpacingType  space;
        space =image->GetSpacing();

        const int tempnx = size[0];
        const int tempny = size[1];

        const double tempvx = space[0];
        const double tempvy = space[1];

        //Check whether the image dimensions and the spacing are the same.
        if( (nx != tempnx) || (ny != tempny) )
            {
            std::cout<<"Error::The size of the images don't match. \n";
            exit(-1);
            }

        if( (vx != tempvx) || (vy != tempvy) )
            {
            std::cout<<"Error::The pixel spacing of the images don't match. \n";
            exit(-1);
            }

        if(dims==3)
            {
            const int tempnz = size[2];
            const double tempvz = space[2];
            if( (nz != tempnz))
                {
                std::cout<<"Error::The size of the images don't match. \n";
                exit(-1);
                }
            if( (vz != tempvz) )
                {
                std::cout<<"Error::The pixel spacing of the images don't match. \n";
                exit(-1);
                }
            itk::SpatialOrientation::ValidCoordinateOrientationFlags
                Accumulator_orient;

            itk::ExposeMetaData <
                itk::SpatialOrientation::ValidCoordinateOrientationFlags >
                (image->GetMetaDataDictionary (), itk::
                 ITK_CoordinateOrientation,
                 Accumulator_orient);
            if(Accumulator_orient != Firstimage_orient)
                {
                std::cout<<"Error::The orientation of the images are different. \n";
                exit(-1);


                }
            }





        //Do the math for the Accumulator image and the image read in for each iteration.
        /*Call the multiplication function*/
        if(command.GetValueAsBool("Mul","mul"))
            {
            AccImage  = Imul<PixelType , dims>(AccImage , image );
            }
        /*Call the addition function*/
        if(command.GetValueAsBool("Add","add"))
            {
            AccImage  = Iadd<PixelType , dims>(AccImage , image );
            }
        /*Call the subtraction function*/
        if(command.GetValueAsBool("Sub","sub"))
            {
            AccImage  = Isub<PixelType , dims>(AccImage , image );
            }

        /*Call the divisionion function*/
        if(command.GetValueAsBool("Div","div"))
            {
            AccImage  = Idiv<PixelType , dims>(AccImage , image );
            }
        /*For Average we add the images first*/
        if(command.GetValueAsBool("Avg","avg"))
            {
            AccImage  = Iadd<PixelType , dims>(AccImage , image);
            }
        /*For variance we add the square image to the image in the current
          iteration and store the sum of the square image.*/
        if(command.GetValueAsBool("Var","var") )
            {
            AccImage  = Iadd<PixelType , dims>(AccImage , image );
            image  = Imul<PixelType , dims>(image , image );
            SqrImageSum  = Iadd<PixelType , dims>(SqrImageSum, image );

            }
        }

    const int NumImages=InputList.size();
    //To get the average image we divide the accumulator image with the total number of images.
    if(command.GetValueAsBool("Avg","avg"))
        {
        AccImage  = Iavg<PixelType , dims>(AccImage , NumImages);
        }
    //Image variance is calculated.
    if(command.GetValueAsBool("Var","var")  )
        {
        SqrImageSum = ImageMultiplyConstant<PixelType, dims>(SqrImageSum,static_cast<PixelType>(NumImages));
        AccImage = Imul<PixelType , dims>(AccImage,AccImage);
        AccImage = Isub<PixelType , dims>(SqrImageSum,AccImage);
        AccImage =ImageDivideConstant<PixelType , dims>(AccImage,static_cast<PixelType>(NumImages*NumImages-NumImages));
        }

    AccImage = Ofilters<PixelType , dims>(AccImage,command);

    const std::string OutType(command.GetValueAsString("OutputPixelType","PixelType" ));
    //The resultant Image is written.
    if(command.GetValueAsString("OutputFilename","filename" ) != "")
        {
        std::cout << "Before write..." <<  command.GetValueAsString("OutputFilename","filename" ) << std::endl;
        //Type cast image according to the output type specified. Default is float.
        if ( command.GetValueAsString("OutputPixelType","PixelType" ) != "" )
            {
            // process the string for the data type
            if ( CompareNoCase( OutType.c_str(), std::string("UCHAR") ) == 0 ) {
                Typecastimage< PixelType , unsigned char , dims >(AccImage,command);
            }
            else if ( CompareNoCase( OutType.c_str(), std::string("SHORT") ) == 0 ) {
                Typecastimage< PixelType , short , dims  >(AccImage,command);
            }
            else if ( CompareNoCase( OutType.c_str(), std::string("USHORT") ) == 0 ) {
                Typecastimage< PixelType , unsigned short , dims  >(AccImage,command);
            }
            else if ( CompareNoCase( OutType.c_str(), std::string("INT") ) == 0 ) {
                Typecastimage< PixelType , int , dims  >(AccImage,command);
            }
            else if ( CompareNoCase( OutType.c_str(), std::string("UINT") ) == 0 ) {
                Typecastimage< PixelType , unsigned int , dims  >(AccImage,command);
            }
            else if ( CompareNoCase( OutType.c_str(), std::string("FLOAT") ) == 0 ) {
                Typecastimage< PixelType , float , dims  >(AccImage,command);
            }
            else if ( CompareNoCase( OutType.c_str(), std::string("DOUBLE") ) == 0 ) {

                Typecastimage< PixelType, double , dims  >(AccImage,command);


            }
            else {
                std::cout << "Error. Invalid data type for -outtype!  Use one of these:" << std::endl;
                PrintDataTypeStrings();
                exit(-1);
            }
            }

        else
            {
            //Default is the Input Pixel Type.
            Typecastimage<PixelType , PixelType , dims  >(AccImage,command);
            }

        //Caluculate Statistics of the Image.
        statfilters<PixelType , dims>(AccImage,command);
        }
}

/*This function calls the ImageCalculatorReadWrite function based on the data type specified by the user.*/
    template <unsigned int DIMS>
void ImageCalculatorProcessND(const std::string & InType, MetaCommand command)
{
    if ( CompareNoCase( InType, std::string("UCHAR") ) == 0 )
        {
        ImageCalculatorReadWrite<unsigned char  , DIMS  >(command);
        }
    else if ( CompareNoCase( InType, std::string("SHORT") ) == 0 )
        {
        ImageCalculatorReadWrite<short , DIMS  >(command);
        }
    else if ( CompareNoCase( InType, std::string("USHORT") ) == 0 )
        {
        ImageCalculatorReadWrite<unsigned short , DIMS  >(command);
        }
    else if ( CompareNoCase( InType, std::string("INT") ) == 0 )
        {
        ImageCalculatorReadWrite<int, DIMS  >(command);
        }
    else if ( CompareNoCase( InType, std::string("UINT") ) == 0 )
        {
        ImageCalculatorReadWrite<unsigned int, DIMS   >(command);
        }

    else if ( CompareNoCase( InType, std::string("FLOAT") ) == 0 )
        {
        ImageCalculatorReadWrite<float, DIMS   >(command);
        }
    else if ( CompareNoCase( InType, std::string("DOUBLE") ) == 0 )
        {
        ImageCalculatorReadWrite< double , DIMS   >(command);
        }
}


#endif // __ImageCalculatorTemplates_h____
