#ifndef ITKFlFileWriter_h
#define ITKFlFileWriter_h

#include <FL/Fl.H>
#include "Fl_File_ChooserModified.H"
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <time.h>
#include <itkCastImageFilter.h>
#include <itkImageFileWriter.h>

template<class ImageType>
class ITKFlFileWriter : public Fl_File_ChooserModified
  {
 
public:
  ITKFlFileWriter(ImageType *imP, const char *d, const char *p, int t, const char *title);
  int GetITKPixelType();
  bool GetITKCompressFile();

  void SetDefaultSelectionType(unsigned int val) {guiITKPixelType->value(val);}
  void SetShowAlertWindow(bool show) {m_ShowAlertWindow = show;}
  bool GetShowAlertWindow() {return m_ShowAlertWindow;}

protected:
  Fl_Choice *guiITKPixelType;
  Fl_Check_Button *guiITKCompressFile;
  static void cb_okButton(Fl_Return_Button *o, void *d);
  bool m_ShowAlertWindow;
};

static Fl_Menu_Item  menu_guiITKPixelType[] = {
   {"char", 0,  0, 0, 0, 0, 0, 11, 56},
   {"unsigned char", 0,  0, 0, 0, 0, 0, 11, 56},
   {"short", 0,  0, 0, 0, 0, 0, 11, 56},
   {"unsigned short", 0,  0, 0, 0, 0, 0, 11, 56},
   {"int", 0,  0, 0, 0, 0, 0, 11, 56},
   {"unsigned int", 0,  0, 0, 0, 0, 0, 11, 56},
   {"float", 0,  0, 0, 0, 0, 0, 11, 56},
   {"double", 0,  0, 0, 0, 0, 0, 11, 56},
   {0, 0,  0, 0, 0, 0, 0, 0, 0}
 };


template <class ImageType>
ITKFlFileWriter<ImageType>::
ITKFlFileWriter(ImageType * imP, const char *d, const char *p, int t, const char *title)
:Fl_File_ChooserModified(d, p, t, title)
  { 
  okButton -> label( "Save" );

  Fl_Choice* o = guiITKPixelType = new Fl_Choice(85, 345, 105, 25, "Pixel Type:");

  o -> box(FL_PLASTIC_UP_BOX);
  o -> down_box(FL_BORDER_BOX);
  o -> color(49);
  o -> labelsize(11);
  o -> labelcolor(0);
  o -> align(FL_ALIGN_LEFT);
  o -> menu(menu_guiITKPixelType);
  o -> value(2);
  window -> add(o);

  
  Fl_Check_Button* oo = guiITKCompressFile = new Fl_Check_Button(200, 345, 85, 25, "Compress");
  oo -> down_box(FL_DOWN_BOX);
  oo -> value(1);
  window -> add(oo);

  window -> end();
  m_ShowAlertWindow = true;
  }


template <class ImageType>
void
ITKFlFileWriter<ImageType>::
cb_okButton(Fl_Return_Button * o, void * d)
  {
  Fl_File_ChooserModified::cb_okButton(o, d);
  }

template<class ImageType>
int ITKFlFileWriter<ImageType>::
GetITKPixelType()
  {
  return guiITKPixelType -> value();
  }

template<class ImageType>
bool ITKFlFileWriter<ImageType>::
GetITKCompressFile()
  {
  return guiITKCompressFile -> value();
  }



template <class TImage, class TPixelType>
class itkFlWriteFile
{
public:

  typedef itkFlWriteFile Self;
  typedef TImage ImageType;
  typedef TPixelType PixelType;
  itkStaticConstMacro(ImageDimension,unsigned int, ImageType::ImageDimension);
  
template <class ImageType, class PixelType>
itkFlWriteFile( ImageType * img, PixelType val, char * filename, bool compress, bool showAlert )
  {
  typedef itk::Image< PixelType, itkGetStaticConstMacro(ImageDimension)>  SavedImageType;
  typedef itk::ImageFileWriter< SavedImageType  >             WriterType;
  typename WriterType::Pointer writer = WriterType::New();

  // to cast from a float pixel type to another type of pixel ( choosen by the user )
  typedef itk::CastImageFilter< ImageType, SavedImageType >   CastImageFilterType;
  
  typename CastImageFilterType::Pointer mCastImageFilter = CastImageFilterType::New();

  mCastImageFilter -> SetInput( img );
  mCastImageFilter -> Update();

  writer -> SetFileName( filename );
  writer -> SetInput( mCastImageFilter -> GetOutput() );
  writer -> SetUseCompression( compress );

  try
    {
    writer -> Update();
    }
  catch( itk::ExceptionObject & exp ) 
    {
    if(showAlert)
      {
      fl_alert(exp.GetDescription());
      }
    else
      {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << exp << std::endl;
      }
    }
  }

  ~itkFlWriteFile(){};

};


template <class ImageType>                                 
bool
itkFlFileWriter( ImageType *imP,       // O - Filename or NULL
                 const char *message,  // I - Message in titlebar
                 const char *pat,      // I - Filename pattern
                 const char *fname,    // I - Initial filename selection
                 int        relative,
                 unsigned int defaultPixelSelection = 2,
                 bool showAlertWindow = true
                 )   // I - 0 for absolute path
  {
  static char  retname[1024];              // Returned filename
  ITKFlFileWriter<ImageType> *fc = (ITKFlFileWriter<ImageType> *)0;

  if (!fname || !*fname) 
    {
    fname = ".";
    }
  
  fc = new ITKFlFileWriter<ImageType>(imP, fname, pat, ITKFlFileWriter<ImageType>::CREATE, message);
  fc->SetDefaultSelectionType(defaultPixelSelection);
  fc->SetShowAlertWindow(showAlertWindow);
  fc->callback(0,0);
  fc -> show();


  while (fc -> shown())
    {
    Fl::wait();
    }
 
  if (fc -> value() && relative) 
    {
    fl_filename_relative(retname, sizeof(retname), fc -> value());
    } 
  else 
    if (fc -> value()) 
      strcpy(retname, (char *)fc -> value());
    else 
      return false;

  bool compress = fc -> GetITKCompressFile();

  switch(fc -> GetITKPixelType())
    {
    case 0 :
        itkFlWriteFile<ImageType, char>( imP, (char)0, retname, compress, showAlertWindow );
        break;
    case 1 :
        itkFlWriteFile<ImageType, unsigned char>( imP, (unsigned char)0, retname, compress, showAlertWindow );
        break;
    case 2 :
        itkFlWriteFile<ImageType, short>( imP, (short)0, retname, compress, showAlertWindow );
        break;
    case 3 :
        itkFlWriteFile<ImageType, unsigned short>( imP, (unsigned short)0, retname, compress, showAlertWindow );
        break;
    case 4 :
        itkFlWriteFile<ImageType, int>( imP, (int)0, retname, compress, showAlertWindow );
        break;
    case 5 :
        itkFlWriteFile<ImageType, unsigned int>( imP, (unsigned int)0, retname, compress, showAlertWindow );
        break;
    case 6 :
        itkFlWriteFile<ImageType, float>( imP, (float)0, retname, compress, showAlertWindow );
        break;
    case 7 :
        itkFlWriteFile<ImageType, double>( imP, (double)0, retname, compress, showAlertWindow );
        break;
    };
  return true;
}


#endif
