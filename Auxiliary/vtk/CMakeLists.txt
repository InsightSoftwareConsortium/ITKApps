FOREACH(exe vtk2itk vtk2itkDouble itkCurvatureFlowToVTK
            itkVTKtoITKtoVTK itkPNGVTKtoITKtoVTK itkReadITKImageShowVTK
            vtkPolyDataToITKMesh itkReadITKImageSegmentShowVTK)
  ADD_EXECUTABLE(${exe} ${exe}.cxx)
  TARGET_LINK_LIBRARIES(${exe} ITKBasicFilters ITKIO vtkRendering vtkIO)
ENDFOREACH(exe)
