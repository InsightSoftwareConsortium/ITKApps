/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ParameterContainer.inl
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "ParameterContainer.h"

template <class ITKPIXELTYPE>
class ParameterContainer{
private:
  const char* const* ppstrzParamNames;
  const char* const* ppstrzParamSuggestions;
  const int nParamExpected;
  
public:
  static MATPARAMTYPE* pParameters;
  ParameterContainer(const char* const* ParameterNames, const char* const* ParameterSuggestions, int nParametersExpected)
    :ppstrzParamNames(ParameterNames),ppstrzParamSuggestions(ParameterSuggestions),
       nParamExpected(nParametersExpected)
  {
    /*if (nParam==0) std::cout<<"Oops... it turns out that no parameters are required\n";
    ppParamList = new Parameter*[nParameters];*/
    if (bHelpMode || nParametersExpected>nParametersSupplied){
      promptParameters();
      mexPrintf("%d parameters must be supplied.  You supplied %d.\n", nParametersExpected,nParametersSupplied);
      mexErrMsgTxt("Correct number of parameters must be supplied.  At least one image volume has to be supplied.");
    }
    else if (nParametersSupplied>nParametersExpected){
      promptParameters();
      mexPrintf("%d parameters must be supplied.  You supplied %d.\n", nParametersExpected,nParametersSupplied);
      mexWarnMsgTxt("Too many parameters supplied.  Are you sure you're calling the right method? Proceeding anyway...");
    }
  }
  void promptParameters()
  {
    mexPrintf("You must supply parameters for this function in an array, with the elements in this order:\n");
    for (int i=0; i<nParamExpected; i++){
      if (strcmp(ppstrzParamSuggestions[i],"")) 
        mexPrintf("%s (which usually has value equal to %s)",ppstrzParamNames[i],ppstrzParamSuggestions[i]);
      else mexPrintf("%s",ppstrzParamNames[i]);
      if (i<nParamExpected-1) mexPrintf(",\n");
    }
    mexPrintf("\n");
  }
  MATPARAMTYPE getCurrentParam(int i)
  {
    if (i>=nParamExpected) mexErrMsgTxt("Out of index exception.  Programming error?!?");
    return pParameters[i];
  }
  //~ParameterContainer();
};

template <class ITKPIXELTYPE>
MATPARAMTYPE* ParameterContainer<ITKPIXELTYPE>::pParameters;
