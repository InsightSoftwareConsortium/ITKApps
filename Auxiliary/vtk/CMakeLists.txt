FOREACH(exe vtk2itk vtk2itkDouble itkCurvatureFlowToVTK
            itkVTKtoITKtoVTK itkPNGVTKtoITKtoVTK itkReadITKImageShowVTK
            vtkPolyDataToITKMesh itkReadITKImageSegmentShowVTK 
            itkReadITKImage3DSegmentShowVTK)
  ADD_EXECUTABLE(${exe} ${exe}.cxx)
  TARGET_LINK_LIBRARIES(${exe} ITKBasicFilters ITKIO vtkRendering vtkIO vtkWidgets)
ENDFOREACH(exe)
