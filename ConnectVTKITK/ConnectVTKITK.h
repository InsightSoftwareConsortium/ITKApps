#ifndef __ConnectVTKITK_h
#define __ConnectVTKITK_h

// vtkFloatingPointType is defined by VTK 4.4 and above.  We need to
// define it to float if it is not defined (VTK 4.2).  SWIG's
// preprocessor does not include the definition from VTK's headers, so
// we cannot let it's preprocessor see the float definition.
// Therefore we let SWIG think the type is called vtkFloatingPointType
// and then make sure the definition is correct when the compiler
// builds the generated code.
#ifndef SWIG
# ifndef vtkFloatingPointType
#  define vtkFloatingPointType float
# endif
#endif

template <typename ImageType>
void ConnectITKToVTK(itk::VTKImageExport<ImageType ,vtkFloatingPointType> *in, 
                     vtkImageImport* out ) 
{
  out->SetUpdateInformationCallback(in->GetUpdateInformationCallback());
  out->SetPipelineModifiedCallback(in->GetPipelineModifiedCallback());
  out->SetWholeExtentCallback(in->GetWholeExtentCallback());
  out->SetSpacingCallback(in->GetSpacingCallback());
  out->SetOriginCallback(in->GetOriginCallback());
  out->SetScalarTypeCallback(in->GetScalarTypeCallback());
  out->SetNumberOfComponentsCallback(in->GetNumberOfComponentsCallback());
  out->SetPropagateUpdateExtentCallback(in->GetPropagateUpdateExtentCallback());
  out->SetUpdateDataCallback(in->GetUpdateDataCallback());
  out->SetDataExtentCallback(in->GetDataExtentCallback());
  out->SetBufferPointerCallback(in->GetBufferPointerCallback());
  out->SetCallbackUserData(in->GetCallbackUserData());
};


template <typename ImageType>
void ConnectVTKToITK (vtkImageExport* in, 
                      itk::VTKImageImport<ImageType, vtkFloatingPointType > *out ) 
{
  out->SetUpdateInformationCallback(in->GetUpdateInformationCallback());
  out->SetPipelineModifiedCallback(in->GetPipelineModifiedCallback());
  out->SetWholeExtentCallback(in->GetWholeExtentCallback());
  out->SetSpacingCallback(in->GetSpacingCallback());
  out->SetOriginCallback(in->GetOriginCallback());
  out->SetScalarTypeCallback(in->GetScalarTypeCallback());
  out->SetNumberOfComponentsCallback(in->GetNumberOfComponentsCallback());
  out->SetPropagateUpdateExtentCallback(in->GetPropagateUpdateExtentCallback());
  out->SetUpdateDataCallback(in->GetUpdateDataCallback());
  out->SetDataExtentCallback(in->GetDataExtentCallback());
  out->SetBufferPointerCallback(in->GetBufferPointerCallback());
  out->SetCallbackUserData(in->GetCallbackUserData());
};

#endif
