#ifndef __ISIS_LiverTumorSegmentation__h__
#define __ISIS_LiverTumorSegmentation__h__


#include "LiverTumorSegmentationGUI.h"
#include "ImageSliceViewer.h"

class vtkImageShiftScale;

class vtkImageBlend;

/** 
  \class LiverTumorSegmentation 
  \brief This class implements Liver Tumor Segmentation with FLTK interface. 
  It is derived from the LiverTumorSegmentationGUI class, and it implements
  all the callbacks for the user interface actions.

*/

class LiverTumorSegmentation : public LiverTumorSegmentationGUI
{
public:

   LiverTumorSegmentation();

   virtual ~LiverTumorSegmentation();
  
   /** The Show method brings the interface widgets up. */
   virtual void Show();

   /** The Hide method hides the interface widgets. */
   virtual void Hide();

   /** Quit method quits from the program.*/
   virtual void Quit();

   /** Load method loads the volume data. It props up a window for
   choosing the volume data file. The pixel type of the data has be 
   "unsigned char".
   */

   virtual void Load();

   /**
   LoadDICOM method loads the volume data from DICOM files. It first props up 
   a window for choosing the directory containing the DICOM data files. Once the
   directory name is provided, it props up another window listing all the "series 
   ids" of serieses whose files are in the chosen directory. The user is required 
   to select the "series id" of the series of interest. The pixel type of the data 
   has be "unsigned char". */

   virtual void LoadDICOM();

   /** SelectAxialSlice method selects the requested axial slice */
   virtual void SelectAxialSlice( int );

   /** SelectCoronalSlice method selects the requested coronal slice */
   virtual void SelectCoronalSlice( int );

   /** SelectSaggitalSlice method selects the requested saggital slice */
   virtual void SelectSaggitalSlice( int );

   /** ProcessAxialViewInteraction method is called when user clicks on
   the axial view window. */
   virtual void ProcessAxialViewInteraction( void );

   /** ProcessCoronalViewInteraction method is called when user clicks on
   the coronal view window. */
   virtual void ProcessCoronalViewInteraction( void );

   /** ProcessSaggitalViewInteraction method is called when user clicks on
   the saggital view window. */
   virtual void ProcessSaggitalViewInteraction( void );

   /** SyncAllViews is used for synchronizing the axial, coronal and 
   saggital views about a clicked point in any of the views. */ 

   virtual void SyncAllViews(void);

   /** ProcessDicomReaderInteraction implements user interaction required
   for selecting DICOM data for read. */

   virtual void ProcessDicomReaderInteraction( void );

   /** OnOpacityControl method implements setting opacity of the segmented
   volume. */
   virtual void OnOpacityControl( double opacity );

   /** OnSegmentation implements invocation of segmentation process. */
   virtual void OnSegmentation( void );

   /** OnSegmentationModuleSelection implements updating window with parameters
   specific to the selected segmentation module. */

   virtual void OnSegmentationModuleSelection( int module );

   /** OnSegmentationParameters invokes OnSegmentationModuleSelection method with
   the chosen segmentation module type as the parameter. */
   virtual void OnSegmentationParameters( void );

   /** OnSegmentationParametersOk updates segmentation parameters for the selected 
   module from the interface. */
   virtual void OnSegmentationParametersOk( int module );

   /** SetSeedPoint sets the sedd point for segmentation. */
   virtual void SetSeedPoint( double x, double y, double z );

   /** GetImageScale gets the scaling of the image displayed. */
   virtual double GetImageScale( void );
   
   /** SetImageScale sets the scaling of the image displayed. */
   virtual void SetImageScale( double val );
  
   /** GetImageShift gets the shift of the pixel value of the image displayed. */
   virtual double GetImageShift( void );
  
   /** SetImageShift sets the shift of the pixel value of the image displayed. */
   virtual void SetImageShift( double val );

   /** GetSegmentedVolumeOpacity gets the opacity of the segmented volume. */
   virtual double GetSegmentedVolumeOpacity( void );

   /** SetSegmentedVolumeOpacity sets the opacity of the segmented volume. */
   virtual bool SetSegmentedVolumeOpacity( const double value );

private:

   virtual void LoadPostProcessing();

  ISIS::ImageSliceViewer  m_AxialViewer;
  ISIS::ImageSliceViewer  m_CoronalViewer;
  ISIS::ImageSliceViewer  m_SaggitalViewer;

  vtkImageShiftScale    * m_ShiftScaleImageFilter;

  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_AxialViewerCommand;
  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_CoronalViewerCommand;
  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_SaggitalViewerCommand;

  itk::SimpleMemberCommand<LiverTumorSegmentation>::Pointer      m_DicomReaderCommand;

  vtkImageBlend        * m_vtkImageBlender;

  double                m_SegmentedVolumeOpacity;

  SegmentationModuleType    m_ModuleType;

};


#endif
