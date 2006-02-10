/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    matitk.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "matitk.h"
#include "MATITKTemplatedVariables.h"
#include "itkcore.h"
#include "seedcontainer.h"
#include <Vector>
extern void _main();
char* pstrzOp = 0;
int nParametersSupplied;
bool bHelpMode;
bool emptyImportFilter[2];


template <class ITKPIXELTYPE, int MATPIXELTYPE>
class MainClass{
public: 
  #include "typedefs.inl"
  static MATITKTemplatedVariables<ITKPIXELTYPE> GTV;

  static void createGlobalITKImageHolder(int whichImageVolume, ITKPIXELTYPE* pbuffer, int* itkdims, double* pSpacing_M){
    if (!pbuffer || !itkdims) {
      GTV.importFilter[whichImageVolume] = 0; //null
      emptyImportFilter[whichImageVolume]=true;
      return;  //return if empty pointers
    }
    emptyImportFilter[whichImageVolume]=false;
    GTV.importFilter[whichImageVolume] = ImportFilterType::New();      

    ImportFilterType::SizeType  size;

    size[0]  = itkdims[0];  // size along X
    size[1]  = itkdims[1];  // size along Y
    size[2]  = itkdims[2];  // size along Z

    ImportFilterType::IndexType start;
    start.Fill( 0 );

    ImportFilterType::RegionType region;
    region.SetIndex( start );
    region.SetSize(  size  );

    (GTV.importFilter[whichImageVolume])->SetRegion( region );

    double origin[ DIMENSION ];
    origin[0] = 0.0;    // X coordinate 
    origin[1] = 0.0;    // Y coordinate
    origin[2] = 0.0;    // Z coordinate

    (GTV.importFilter[whichImageVolume])->SetOrigin( origin );

    double spacing[ DIMENSION ];
    if (pSpacing_M==0){
      spacing[0] = 1.0;    // along X direction 
      spacing[1] = 1.0;    // along Y direction
      spacing[2] = 1.0;    // along Z direction
    }
    else {
      spacing[0] = pSpacing_M[whichImageVolume*3+1];    // along X direction 
      spacing[1] = pSpacing_M[whichImageVolume*3];    // along Y direction
      spacing[2] = pSpacing_M[whichImageVolume*3+2];    // along Z direction
    }

    (GTV.importFilter[whichImageVolume])->SetSpacing( spacing );

    const unsigned int numberOfPixels =  size[0] * size[1] * size[2];
    const bool importImageFilterWillOwnTheBuffer = false;
    (GTV.importFilter[whichImageVolume])->SetImportPointer( pbuffer, numberOfPixels, 
      importImageFilterWillOwnTheBuffer );
  }



  static void mexcpp(ITKPIXELTYPE* pbufferA, int* itkdimsA, ITKPIXELTYPE* pbufferB, int* itkdimsB, double* pSpacing_M)
  {  
    if (!bHelpMode){
      MainClass<ITKPIXELTYPE,MATPIXELTYPE>::createGlobalITKImageHolder(0,pbufferA, itkdimsA, pSpacing_M);
      MainClass<ITKPIXELTYPE,MATPIXELTYPE>::createGlobalITKImageHolder(1,pbufferB, itkdimsB, pSpacing_M);
      GTV.pixelContainer = (GTV.importFilter[IMPORTFILTERA])->GetOutput()->GetPixelContainer();
    }
    ITKCore<ITKPIXELTYPE>::ITKEntry(GTV);
    return;
  }

  static void transpose(const ITKPIXELTYPE* indata, ITKPIXELTYPE* outdata, const int* originaldims){
    int i=0,j=0,k=0,lindex=0;
    for (k=0;k<originaldims[2];k++){
      for (j=0;j<originaldims[0];j++){
        for (i=0;i<originaldims[1];i++){      
          outdata[lindex]=indata[k*originaldims[1]*originaldims[0]+j+i*originaldims[0]];
          lindex++;
        }
      }
    }
  }

  static int pred(int k,int L,int M){return (k%M)*L+k/M;}
  static void exchange(int indexA, int indexB, ITKPIXELTYPE* arrayptr){
    ITKPIXELTYPE tmp=arrayptr[indexA];
    arrayptr[indexA]=arrayptr[indexB];
    arrayptr[indexB]=tmp;
  }
  static void inmemtranspose(ITKPIXELTYPE* inoutdata, const int* originaldims){
    int L=originaldims[0], M=originaldims[1];
    int LxM=L*M;
    int i,j,k, stillToMove;
    ITKPIXELTYPE* sliceptr;
    for (int z=0; z<originaldims[2];z++){
      sliceptr=inoutdata+z*L*M;
      for (i=0,stillToMove=LxM; stillToMove>0; i++) {
        for (j=pred(i,L,M);j>i;j=pred(j,L,M));
        if (j<i) continue;
        for (k=i,j=pred(i,L,M);j!=i;k=j,j=pred(j,L,M)) {
          exchange(k,j,sliceptr);
          --stillToMove;
        }
        --stillToMove;
      } 
    }
  }

  static void transposeSeed(ITKPIXELTYPE* mseeds, ITKPIXELTYPE* itkseeds, int num){
    for (int i=0; i<num; i++){
      if (i%3==0)
        itkseeds[i]=mseeds[i];
      else if (i%3==1)
        itkseeds[i]=mseeds[i+1];
      else if (i%3==2)
        itkseeds[i]=mseeds[i-1];
    }
    for (int i=0; i<num; i++)
      mexPrintf("%f",itkseeds[i]);
  }

  static void CMexFunction(
    int          nlhs,
    mxArray      *plhs[],
    int          nrhs,
    const mxArray *prhs[]
    )
    {
      if (nrhs <1) {
        aboutMATITK();
      } 
      if (nrhs >= 3) {

        bHelpMode=false;
        GTV.pParameters = (MATPARAMTYPE*) mxGetData(prhs[RPARAMINDEX]);
        ParameterContainer<ITKPIXELTYPE>::pParameters = GTV.pParameters;
        nParametersSupplied = mxGetN(prhs[RPARAMINDEX]);
      }
      else {
        //mexErrMsgTxt("Not enough param");
        nParametersSupplied = 0;
        bHelpMode=true;
      }
      if (!(mxIsChar(prhs[ROPINDEX]))){
        mexErrMsgTxt("Opcode input field must be of type string.\n.");
      }

      int m, n, mA, nA;
      const int* crdimsA; const int* crdimsB;
      int rdimsA[3]; int rdimsB[3];
      int itkdimsA[3]; int itkdimsB[3];

      pstrzOp=mxArrayToString(prhs[ROPINDEX]);
      ITKPIXELTYPE *inputA_M=0, *inputA_ITK=0, *inputB_M=0, *inputB_ITK=0;
      double* spacing_M=0;
      MATSEEDTYPE *seeds_M=0;


      //////working with spacing input ////////
      if (!bHelpMode && nrhs >= 6) { //if user specifies spacing
        if (!mxIsDouble(prhs[RSPACINGINDEX])){
          mexErrMsgTxt("Spacing array must contain double elements only.");
        }
        m = mxGetM(prhs[RSPACINGINDEX]);
        n = mxGetN(prhs[RSPACINGINDEX]);  
        if (m*n>0) { //otherwise it might just be a dummy
          if (mxGetNumberOfDimensions(prhs[RSPACINGINDEX])>2 || m>1) mexErrMsgTxt("Spacing array must be a vector."); 
          if (n%3!=0) mexErrMsgTxt("Spacing array should have size that is a multiple of 3 (i-th triplet represents spacing for the i-th input)."); 
          spacing_M = (double*) mxGetPr(prhs[RSPACINGINDEX]);
        }
      } else 
        spacing_M=0;


      //////working with primary input (inputA)////////
      if (!bHelpMode && nrhs >= 3) {
        /*if (!mxIsDouble(prhs[RINPUTAINDEX])){
          mexErrMsgTxt("Input image volume A must contain double elements only.");
        }*/
        mA = mxGetM(prhs[RINPUTAINDEX]);
        nA = mxGetN(prhs[RINPUTAINDEX]);    
        crdimsA = mxGetDimensions(prhs[RINPUTAINDEX]);
        int inputADimen=mxGetNumberOfDimensions(prhs[RINPUTAINDEX]);
        if (inputADimen!=3) mexErrMsgTxt("Input volume A must be a 3D image."); 
        rdimsA[0]=crdimsA[0];rdimsA[1]=crdimsA[1];inputADimen==2?rdimsA[2]=1:rdimsA[2]=crdimsA[2];
        itkdimsA[0]=rdimsA[1];itkdimsA[1]=rdimsA[0];itkdimsA[2]=rdimsA[2];

        inputA_M = (ITKPIXELTYPE*) mxGetData(prhs[RINPUTAINDEX]);
        inputA_ITK = (ITKPIXELTYPE *) mxMalloc(mA*nA * sizeof(ITKPIXELTYPE));
        MainClass<ITKPIXELTYPE, MATPIXELTYPE>::transpose(inputA_M,inputA_ITK,rdimsA);
      }
      //////working with secondary input (inputB)////////
      if (!bHelpMode && nrhs >= 4) {
        /*if (!mxIsDouble(prhs[RINPUTBINDEX])){
          mexErrMsgTxt("Input image volume B must contain double elements only.");
        }*/
        m = mxGetM(prhs[RINPUTBINDEX]);
        n = mxGetN(prhs[RINPUTBINDEX]); 
        if (m*n>0) { //otherwise it might just be a dummy
          crdimsB = mxGetDimensions(prhs[RINPUTBINDEX]);
          int inputBDimen=mxGetNumberOfDimensions(prhs[RINPUTBINDEX]);
          if (inputBDimen!=3) mexErrMsgTxt("Input volume B must be a 3D image."); 
          rdimsB[0]=crdimsB[0];rdimsB[1]=crdimsB[1];inputBDimen==2?rdimsB[2]=1:rdimsB[2]=crdimsB[2];
          itkdimsB[0]=rdimsB[1];itkdimsB[1]=rdimsB[0];itkdimsB[2]=rdimsB[2];
          inputB_M = (ITKPIXELTYPE*) mxGetData(prhs[RINPUTBINDEX]);
          inputB_ITK = (ITKPIXELTYPE *) mxMalloc(m*n * sizeof(ITKPIXELTYPE));
          MainClass<ITKPIXELTYPE,MATPIXELTYPE>::transpose(inputB_M,inputB_ITK,rdimsB);
        }
      }
      //////working with seed input ////////
      if (!bHelpMode && nrhs >= 5) {
        /*if (!mxIsDouble(prhs[RSEEDINDEX])){
          mexErrMsgTxt("Seeds array must contain double elements only.");
        }*/
        m = mxGetM(prhs[RSEEDINDEX]);
        n = mxGetN(prhs[RSEEDINDEX]); 
        if (m*n>0) { //otherwise it might just be a dummy
          if (mxGetNumberOfDimensions(prhs[RSEEDINDEX])>2 || m>1) mexErrMsgTxt("Seed array must be a vector."); 
          if (n%3!=0) mexErrMsgTxt("Seed array should have size that is a multiple of 3 (each seed is 3-dimensional)."); 
          seeds_M = (MATSEEDTYPE*) mxGetPr(prhs[RSEEDINDEX]);
          GTV.seedsIndex.set(seeds_M, n, rdimsA);
        }
      }




      ///////////////////////////////////////////////////
      MainClass<ITKPIXELTYPE,MATPIXELTYPE>::mexcpp(inputA_ITK,itkdimsA, inputB_ITK,itkdimsB, spacing_M);

      ///////////////////////////////////////////////////
      if (nrhs >= 3) {
        if ((GTV.pixelContainers.size()>1 && nlhs!=GTV.pixelContainers.size()) ||
          (GTV.pixelContainers.size()==1 && nlhs>1)||
          (GTV.pixelContainers.size()==0 && nlhs>1)
          ){      
            mexPrintf("Expected number of output(s): %d.  Supplied output(s): %d\n",GTV.pixelContainers.size(),nlhs);
            GTV.pixelContainers.clear();
            mexErrMsgTxt("Mismatch number of output arguments.");     
          }
        else if ( (nlhs>0 && nlhs==GTV.pixelContainers.size()) || GTV.pixelContainers.size()==1){
          std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> >::iterator it;
          int count=0;
          for (it=GTV.pixelContainers.begin();it!=GTV.pixelContainers.end();it++){
            if(it->needTranspose)
            {
              ITKPIXELTYPE* outputMatlabImage = (ITKPIXELTYPE *) mxMalloc((it->numElem) * sizeof(ITKPIXELTYPE));        
              MainClass<ITKPIXELTYPE,MATPIXELTYPE>::transpose(it->theArray,
                outputMatlabImage,
                itkdimsA);
              plhs[count] = mxCreateNumericArray(DIMENSION,rdimsA,(mxClassID)MATPIXELTYPE,mxREAL);
              mxSetData(plhs[count], outputMatlabImage);
            }
            else{
              int dim[1];
              dim[0] = it->numElem;
              plhs[count] = mxCreateNumericArray(1, dim, (mxClassID)MATPIXELTYPE, mxREAL);
              mxSetData(plhs[count], it->theArray);
            }       
            count++;
          }
        }
        else{
          if (GTV.pixelContainer->Size()!=0) MainClass<ITKPIXELTYPE,MATPIXELTYPE>::transpose((ITKPIXELTYPE*)(GTV.pixelContainer->GetImportPointer()),inputA_ITK,itkdimsA);
          else mexErrMsgTxt("The filter did not produce an output.  Please check parameters again.");
          //inmemtranspose(data2,itkdims);          
          plhs[LRESULTINDEX] = mxCreateNumericArray(DIMENSION,rdimsA,(mxClassID)MATPIXELTYPE,mxREAL);
          mxSetData(plhs[LRESULTINDEX], inputA_ITK);
        }
        GTV.pixelContainer=0;
        GTV.pixelContainers.clear();
        mxFree(pstrzOp);
      }

    }
};

template <class ITKPIXELTYPE, int MATPIXELTYPE>
MATITKTemplatedVariables<ITKPIXELTYPE> MainClass<ITKPIXELTYPE, MATPIXELTYPE>::GTV;

void sampleUsageMATITK(){
  mexPrintf("\nSample usage:\n");
  mexPrintf("=============\n");
  mexPrintf("\%prepare 3D volume, mri comes with MATLAB\nload mri; D=squeeze(D); size(D);\n\%perform processing\nb=matitk('FCA',[5 0.0625 3],double(D));\nc=matitk('SCC',[1.4 10 255],double(b),[],[102 82 25]);\n\%display results\nsubplot(131); imagesc(squeeze(D(:,:,15))); axis image; title('original '); colormap gray\nsubplot(132); imagesc(squeeze(b(:,:,15))); axis image; title('smoothed '); colormap gray\nsubplot(133); imagesc(squeeze(c(:,:,15))); axis image; title('segmented'); colormap gray \n\n");
}

void aboutMATITK(){
  mexPrintf("\nMATITK v.2.4.02 "__DATE__"\n");
  mexPrintf("Based on Insight Segmentation and Registration Toolkit (ITK) v.2.4 (http://www.itk.org)\n");
  mexPrintf("Please report any comments/bugs to: "CONTACT".\n");
  if (pstrzOp == 0 || pstrzOp[0]!='?')
  {
    mexPrintf("For a list of allowed operations and a demo of sample usage, type matitk('?')\n");
    mexErrMsgTxt(OPCOMMAND);
  }
  else{
    sampleUsageMATITK();
  }
}

void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{
  pstrzOp = 0;
  if (nrhs >= 3 && (mxIsDouble(prhs[RINPUTAINDEX])) && 
    (nrhs < 4 ||mxIsDouble(prhs[RINPUTBINDEX])) && 
    (nrhs < 5 || mxIsDouble(prhs[RSEEDINDEX])) &&
    (mxIsDouble(prhs[RPARAMINDEX]))
    )
  {
    mexPrintf("Image input of type double detected, executing MATITK in double mode\n");
    MainClass<double, mxDOUBLE_CLASS>::CMexFunction(nlhs,plhs,nrhs,prhs); 
  }
  else if ( nrhs >= 3 && 
    (mxSINGLE_CLASS  == mxGetClassID(prhs[RINPUTAINDEX])) && 
    (nrhs < 4 ||mxSINGLE_CLASS  == mxGetClassID(prhs[RINPUTBINDEX])) && 
    (nrhs < 5 || mxIsDouble(prhs[RSEEDINDEX])) &&
    (mxIsDouble(prhs[RPARAMINDEX])) 
    )
  {
    mexPrintf("Image input of type single(float) detected, executing MATITK in single(float) mode\n");
    MainClass<float, mxSINGLE_CLASS>::CMexFunction(nlhs,plhs,nrhs,prhs);
  }
  else if ( nrhs >= 3 && 
    (mxUINT8_CLASS == mxGetClassID(prhs[RINPUTAINDEX])) && 
    (nrhs < 4 ||mxUINT8_CLASS == mxGetClassID(prhs[RINPUTBINDEX])) && 
    (nrhs < 5 || mxIsDouble(prhs[RSEEDINDEX])) &&
    (mxIsDouble(prhs[RPARAMINDEX])) 
    )
  {
    mexPrintf("Image input of type unsigned char detected, executing MATITK in unsigned char mode\n");
    MainClass<unsigned char, mxUINT8_CLASS>::CMexFunction(nlhs,plhs,nrhs,prhs);
  }
  else if ( nrhs >= 3 && 
    (mxINT32_CLASS == mxGetClassID(prhs[RINPUTAINDEX])) && 
    (nrhs < 4 ||mxINT32_CLASS == mxGetClassID(prhs[RINPUTBINDEX])) && 
    (nrhs < 5 || mxIsDouble(prhs[RSEEDINDEX])) &&
    (mxIsDouble(prhs[RPARAMINDEX])) 
    )
  {
    mexPrintf("Image input of type integer(32 bit) detected, executing MATITK in signed integer(32 bit) mode\n");
    MainClass<int, mxINT32_CLASS>::CMexFunction(nlhs,plhs,nrhs,prhs);
  }
  else{
    if (nrhs<3){
      MainClass<double, mxDOUBLE_CLASS>::CMexFunction(nlhs,plhs,nrhs,prhs);
    }
    else{
      mexPrintf("Images can be of type double/single(float)/unsigned char(uint8)/integer(int32) arrays.\n");
      mexPrintf("Both images (inputArrays) must be of the same data type.\n");
      mexPrintf("All other inputs (seeds and parameters) must be double arrays.\n"); 
      aboutMATITK();
    }
  }
}
