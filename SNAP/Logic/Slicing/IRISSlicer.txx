/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISSlicer.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
template<class TPixel> 
IRISSlicer<TPixel>
::IRISSlicer()
{
  // There is a single input to the filter
  SetNumberOfRequiredInputs(1);

  // There are three outputs from the filter
  SetNumberOfRequiredOutputs(1);

  // Initialize slice indices
  m_SliceIndex = 0;
  m_SliceAxis = 0;
}

template<class TPixel> 
void IRISSlicer<TPixel>
::GenerateOutputInformation()
{
  // Get pointers to the inputs and outputs
  typename Superclass::InputImageConstPointer inputPtr = GetInput();
  typename Superclass::OutputImagePointer outputPtr = GetOutput();
  
  // The inputs and outputs should exist
  if (!outputPtr || !inputPtr) return;

  // Get the input's largest possible region
  InputImageRegionType inputRegion = inputPtr->GetLargestPossibleRegion();
  
  // Arrays to specify the output spacing and origin
  double outputSpacing[2];
  double outputOrigin[2];
  
  // Compute the axes directions
  ComputeAxes();

  // Process each dimension separately in a loop
  OutputImageRegionType outputRegion;
  for(unsigned int i=0;i<2;i++)
    {
    // Get the corresponding dimension in image space
    unsigned int imageAxis = m_ImageAxes(i+1);

    // Set the properties of the output region
    outputRegion.SetIndex(i,inputRegion.GetIndex(imageAxis));
    outputRegion.SetSize(i,inputRegion.GetSize(imageAxis));

    // Set the output image spacing
    outputSpacing[i] = inputPtr->GetSpacing()[imageAxis];
      
    // Set the output image origin
    outputOrigin[i] = 0.0;    
    }
    
  // Set the region of the output slice
  outputPtr->SetLargestPossibleRegion(outputRegion);

  // Set the spacing and origin
  outputPtr->SetSpacing(outputSpacing);
  outputPtr->SetOrigin(outputOrigin);
}

template<class TPixel>
void IRISSlicer<TPixel>
::CallCopyOutputRegionToInputRegion(InputImageRegionType &destRegion,
                                    const OutputImageRegionType &srcRegion)
{
  // Compute the axes directions
  ComputeAxes();

  // Set the size of the region to 1 in the requested direction
  destRegion.SetSize(m_ImageAxes(0),1);

  // Set the index of the region in that dimension to the number of the slice
  destRegion.SetIndex(m_ImageAxes(0),m_SliceIndex);

  // Compute the bounds of the input region for the other two dimensions (for 
  // the case when the output region is not equal to the largest possible 
  // region (i.e., we are requesting a partial slice)
  for(unsigned int i=1;i<3;i++)
    {
    // The size of the region does not depend of the direction of axis 
    // traversal
    destRegion.SetSize(m_ImageAxes(i),srcRegion.GetSize(i-1));

    // However, the index of the region does depend on the direction!
    if(m_AxesDirection[i] > 0)
      {
      destRegion.SetIndex(m_ImageAxes(i),srcRegion.GetIndex(i-1));
      }
    else
      {
      // This case is a bit trickier.  The axis direction is reversed, so
      // range [i,...,i+s-1] in the output image corresponds to the range
      // [S-(i+s),S-(i+1)] in the input image, where i is the in-slice index, 
      // S is the largest size of the input and s is the requested size of the 
      // output
      long idx = 
        GetInput()->GetLargestPossibleRegion().GetSize(m_ImageAxes(i)) - 
        (srcRegion.GetIndex(i) + srcRegion.GetSize(i-1));
      
      destRegion.SetIndex(m_ImageAxes(i),idx);
      }
    }
}

template<class TPixel> 
void IRISSlicer<TPixel>
::GenerateData()
{
  // Here's the input and output
  InputImagePointer  inputPtr = GetInput();
  OutputImagePointer  outputPtr = GetOutput();
  
  // Do we really need this?
  // ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

  // Allocate (why is this necessary?)
  AllocateOutputs();

  // Compute the region in the image for which the slice is being extracted
  InputImageRegionType inputRegion = inputPtr->GetRequestedRegion();

  // Compute the axes directions
  ComputeAxes();

  // Create an iterator that will parse the desired slice in the image
  InputIteratorType it(inputPtr,inputRegion);
  it.SetFirstDirection(m_ImageAxes(1));
  it.SetSecondDirection(m_ImageAxes(2));

  // Create an iterator into the slice itself
  OutputIteratorType itSlice(outputPtr,outputPtr->GetRequestedRegion());

  // Copy the contents
  CopySlice(it,itSlice,m_AxesDirection(1),m_AxesDirection(2));
}

template<class TPixel> 
void IRISSlicer<TPixel>
::ComputeAxes()
{
  // This is how the axes in the display space map onto the U,V axes of the
  // individual slices
  static unsigned int linkedAxes[3][3] = {{0,1,2},{1,0,2},{2,0,1}};

  // Compute the slice axes in display and image
  for(unsigned int i=0;i<3;i++) 
    {
    // Compute the directions of the slice in display space  
    m_DisplayAxes(i) = linkedAxes[m_SliceAxis][i];
    
    // Map the dimensions into the image space
    m_ImageAxes(i) = 
      m_DisplayToImage.GetCoordinateIndexZeroBased(m_DisplayAxes(i));
    
    // Compute the direction of traversal in these dimensions
    m_AxesDirection(i) = 
      m_DisplayToImage.GetCoordinateOrientation(m_DisplayAxes(i));
    }
}

template<class TPixel> 
void IRISSlicer<TPixel>
::SetImageToDisplayTransform(const ImageCoordinateTransform &imageToDisplay)
{
  // Set the transform
  m_ImageToDisplay = imageToDisplay;
  m_DisplayToImage = imageToDisplay.Inverse();

  // Modified!
  Modified();
}

template<class TPixel> 
void IRISSlicer<TPixel>
::PrintSelf(std::ostream &os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Slice Index: " << m_SliceIndex << std::endl;
  os << indent << "Slice Axis: " << m_SliceAxis << std::endl;
}


template<class TPixel> 
void IRISSlicer<TPixel>
::CopySlice(InputIteratorType itImage, OutputIteratorType itSlice, 
            int sliceDir, int lineDir)
{
  if (sliceDir > 0 && lineDir > 0) {
    itImage.GoToBegin();
    while (!itImage.IsAtEndOfSlice()) {
      while (!itImage.IsAtEndOfLine()) {
        itSlice.Set(itImage.Value());
        ++itSlice;
        ++itImage;
      }
      itImage.NextLine();
    }
  } else if (sliceDir > 0 && lineDir < 0) {
    itImage.GoToBegin();
    while (!itImage.IsAtEndOfSlice()) {
      itImage.NextLine();
      itImage.PreviousLine();
      while (1) {
        itSlice.Set(itImage.Value());
        ++itSlice;
        if (itImage.IsAtReverseEndOfLine())
          break;
        else
          --itImage;
      }
      itImage.NextLine();
    }
  } else if (sliceDir < 0 && lineDir > 0) {
    itImage.GoToReverseBegin();
    while (!itImage.IsAtReverseEndOfSlice()) {
      itImage.PreviousLine();
      itImage.NextLine();
      while (!itImage.IsAtEndOfLine()) {
        itSlice.Set(itImage.Value());
        ++itSlice;
        ++itImage;
      }
      itImage.PreviousLine();
    } 
  } else if (sliceDir < 0 && lineDir < 0) {
    itImage.GoToReverseBegin();
    while (1) {
      while (1) {
        itSlice.Set(itImage.Value());
        ++itSlice;
        if (itImage.IsAtReverseEndOfLine())
          break;
        else
          --itImage;
      }            
      if (itImage.IsAtReverseEndOfSlice())
        break;
      else
        itImage.PreviousLine();
    } 
  }
}
