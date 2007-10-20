IF( ITK_USE_REVIEW )
   FOREACH( itkQEMeshVTKIO 
     QEMeshToVTKUnstructuredGrid
     vtkToItkQEImport
     vtkUGToItkQEImport
     #vtkPolyDataImportTest
     #vtkUnstructuredGridImportTest
     )
     ADD_EXECUTABLE(${itkQEMeshVTKIO} ${itkQEMeshVTKIO}.cxx)
     INSTALL_TARGETS(/bin ${itkQEMeshVTKIO})
     # Stuff was moved from vtkHybrid to vtkWidgets
     # Link against vtkWidgets if it is found, otherwise link against the older
     # library, vtkHybrid .
     IF(VTK_KITS MATCHES WIDGETS)
       TARGET_LINK_LIBRARIES(${itkQEMeshVTKIO}
          ITKBasicFilters
          ITKIO
          ITKQuadEdgeMesh
          vtkCommon
          vtkRendering
          vtkIO
          vtkWidgets)
     ELSE(VTK_KITS MATCHES WIDGETS)
       TARGET_LINK_LIBRARIES(${itkQEMeshVTKIO}
          ITKBasicFilters
          ITKIO
          ITKQuadEdgeMesh
          vtkCommon
          vtkRendering
          vtkIO
          vtkHybrid)
     ENDIF(VTK_KITS MATCHES WIDGETS)
   ENDFOREACH(itkQEMeshVTKIO)
ELSE( ITK_USE_REVIEW )
   MESSAGE("ITK was not compiled with USE_REVIEW option ON.")
ENDIF( ITK_USE_REVIEW )

FOREACH(exe vtk2itk vtk2itkDouble itkCurvatureFlowToVTK
            itkVTKtoITKtoVTK itkPNGVTKtoITKtoVTK itkReadITKImageShowVTK
            itkReadITKImageShowSplineVTK
            vtkPolyDataToITKMesh itkReadITKImageSegmentShowVTK
            itkReadITKImage3DSegmentShowVTK)
  ADD_EXECUTABLE(${exe} ${exe}.cxx)
  INSTALL_TARGETS(/bin ${exe})

  # Stuff was moved from vtkHybrid to vtkWidgets
  # Link against vtkWidgets if it is found, otherwise link against the older
  # library, vtkHybrid .
  IF(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(${exe} ITKBasicFilters ITKIO vtkRendering vtkIO vtkWidgets)
  ELSE(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(${exe} ITKBasicFilters ITKIO vtkRendering vtkIO vtkHybrid)
  ENDIF(VTK_KITS MATCHES WIDGETS)
ENDFOREACH(exe)
