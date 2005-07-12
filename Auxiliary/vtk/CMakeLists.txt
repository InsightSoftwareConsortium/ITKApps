FOREACH(exe vtk2itk vtk2itkDouble itkCurvatureFlowToVTK
            itkVTKtoITKtoVTK itkPNGVTKtoITKtoVTK itkReadITKImageShowVTK
            vtkPolyDataToITKMesh itkReadITKImageSegmentShowVTK 
            itkReadITKImage3DSegmentShowVTK)
  ADD_EXECUTABLE(${exe} ${exe}.cxx)
 
  # Stuff was moved from vtkHybrid to vtkWidgets
  # Link against vtkWidgets if it is found, otherwise link against the older
  # library, vtkHybrid .
  IF(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(${exe} ITKBasicFilters ITKIO vtkRendering vtkIO vtkWidgets)
  ELSE(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(${exe} ITKBasicFilters ITKIO vtkRendering vtkIO vtkHybrid)
  ENDIF(VTK_KITS MATCHES WIDGETS)
ENDFOREACH(exe)
