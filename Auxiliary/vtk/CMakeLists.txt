FOREACH(exe vtk2itk vtk2itkDouble itkCurvatureFlowToVTK
            itkVTKtoITKtoVTK itkPNGVTKtoITKtoVTK itkReadITKImageShowVTK
            itkReadITKImageShowSplineVTK
            vtkPolyDataToITKMesh itkReadITKImageSegmentShowVTK
            itkReadITKImage3DSegmentShowVTK itkReadITKImage3DExtractContourVTK)
  ADD_EXECUTABLE(${exe} ${exe}.cxx)
  INSTALL_TARGETS(/bin ${exe})

  # Stuff was moved from vtkHybrid to vtkWidgets
  # Link against vtkWidgets if it is found, otherwise link against the older
  # library, vtkHybrid .
  IF(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(${exe} ${ITK_LIBRARIES} vtkRendering vtkIO vtkWidgets)
  ELSE(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(${exe} ${ITK_LIBRARIES} vtkRendering vtkIO vtkHybrid)
  ENDIF(VTK_KITS MATCHES WIDGETS)
ENDFOREACH(exe)

ADD_LIBRARY(ITKVTKIntegration 
  vtkRegistrationMonitor.cxx
  vtkKWImage.cxx
  vtkKWImageIO.cxx
  )

ADD_EXECUTABLE(itkRegisterITKMonitorVTK itkRegisterITKMonitorVTK)
TARGET_LINK_LIBRARIES(itkRegisterITKMonitorVTK 
  ITKVTKIntegration ${ITK_LIBRARIES} vtkRendering vtkIO vtkHybrid)

ADD_EXECUTABLE(ExtractIsoSurface ExtractIsoSurface.cxx)
TARGET_LINK_LIBRARIES(ExtractIsoSurface 
  ITKVTKIntegration ${ITK_LIBRARIES} vtkRendering vtkIO vtkHybrid)

ADD_EXECUTABLE(ScreenShotGenerator ScreenShotGenerator.cxx)
TARGET_LINK_LIBRARIES(ScreenShotGenerator vtkRendering vtkIO vtkHybrid)


IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES (itkRegisterITKMonitorVTK.cxx PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
