/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    myApp.java
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


public class myApp
{

  public static void main( String [] argv )
  {
    System.out.println("Example of ITK module inside a Java application");
    
    myApp app = new myApp();

    app.Run( argv[0], argv[1] );


    System.out.println("Enjoy ITK from Java without wrapping everything !");
  }




  // Constructor
  public myApp()
  {
    module = new itkModuleJava();
  }

  // Actual execution of the pipeline
  public void Run( String input, String output )
  {
    System.out.println("Input filename = "+input);
    module.SetInputFileName( input );
    
    System.out.println("Output filename = "+output );
    module.SetOutputFileName( output );

    System.out.println("Running the pipeline in the ITK module");
    module.RunPipeline();
  }


  
  itkModuleJava module;
}



