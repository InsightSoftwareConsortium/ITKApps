
Description of Directory Insight/Auxiliary:

This directory is meant for code that interfaces packages
to Insight. The interface code typically requires external
libraries, header files, or build processes. Therefore the
interface code is not part of the mainstream Insight build
process. To build Insight with one or more of these packages,
extra steps are usually required. Each sub-directory will have
a README file describing what extra steps are required.



- FltkImageViewer 

  This directory contains classed intended to be built
  in a library. The library is used by several demo
  applications in InsightApplications.  The main interest
  of this library to provide viewers for displaying ITK
  images inside FLTK GUI's.

  If you want to use it for your own application
  you may want to copy the files into your project or you 
  can add this directory in the list of include directories
  of your project then also add the ITKFltkImageViewer 
  library to the list of libraries to link with.




- QtImageViewer

  This directory contains an example on how to interface
  Qt with ITK. It provides a functional small application
  that can load an image using ITK readers and display it
  withing a Qt GUI. This code is intended to be used for
  learning how to integrate Qt and ITK. It can however be
  used as a basic image viewer.




- VtkFltk
 
  The content of this directory is unrelated to ITK but
  it is commonly used for displaying a VTK render window
  within a FTLK GUI.  Many of the demo applications are 
  using it. If you want to use it for your own application
  you may want to copy the files into your project or you 
  can add this directory in the list of include directories
  of your project then also add the ITKVtkFltk library to
  the list of libraries to link with.



- vtk

  This directory contains classes and examples that are
  useful when integrating ITK and VTK in the same application.

  The more relevant classes in this directory are 
   
   ImageToVTKImageFilter
   VTKImageToImageFilter

  which are designed to act as adaptors between VTK and ITK
  filters that process images. With these classes you can 
  that a vtkImageData object produced as output of a VTK image
  filter and convert it to be passed as an itkImage into an
  ITK filter.  The opposite filter allows to take an itkImage
  produced by an ITK filter and pass it as input to a VTK image
  filter that is expecting a vtkImageData.

  The simplest way to use these classes is to copy them into
  your own project.

  Another examples of interest in this directory is the file

      vtk2itk.cxx

  which shows how to convert an itkMesh into a vtkPolyData
  structure.



- vispack

  This directory illustrate how to integrate ITK with 
  "vispack" a package developed at the University of Utah.




If you have questions about this classes and directories
please post them to the ITK users list.



