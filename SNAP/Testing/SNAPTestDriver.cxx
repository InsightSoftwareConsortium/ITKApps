/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPTestDriver.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SNAPTestDriver.h"
#include "TestImageWrapper.h"
#include "TestCompareLevelSets.h"

#include "CommandLineArgumentParser.h"
#include <iostream>
#include <iomanip>

using namespace itk;
using namespace std;

const unsigned int SNAPTestDriver::NUMBER_OF_TESTS = 5;
const char *SNAPTestDriver::m_TestNames[] = { "ImageWrapper",
  "IRISImageData","SNAPImageData","CompareLevelSets","Preprocessing" };
const bool SNAPTestDriver::m_TestTemplated[] = { true, false, false, false, false };

void
SNAPTestDriver
::PrintUsage()
{
  cerr << "SNAP Test Driver Usage:" << endl;
  cerr << "  SNAPTest list" << endl;
  cerr << "or " << endl;
  cerr << "  SNAPTest help NAME " << endl;
  cerr << "or " << endl;
  cerr << "  SNAPTest test NAME [type TYPE] [options]" << endl;
  cerr << "Commands :" << endl;
  cerr << "  list                List all known tests" << endl;
  cerr << "  help NAME           Provide help on test 'NAME'" << endl;
  cerr << "  test NAME           Run test 'NAME'" << endl;
  cerr << "  type TYPE           Specify template parameter of test" << endl;    
  cerr << "                      (e.g., unsigned_short)" << endl;      
}

template <class TPixel> 
SNAPTestDriver::TemplatedTestCreator<TPixel>
::TemplatedTestCreator(const char *name)
{
  string strName = name;

  if(strName == "ImageWrapper")
    m_Test = new TestImageWrapper<TPixel>();
  else
    m_Test = NULL;
}

template <class TPixel> 
TestBase *
SNAPTestDriver::TemplatedTestCreator<TPixel>
::GetTest()
{
  return m_Test;
}
  
TestBase *
SNAPTestDriver
::CreateNonTemplateTest(const char *name)
{
  string strName = name;
  TestBase *test = NULL;

  if(strName == "CompareLevelSets")
    test = new TestCompareLevelSets;
  
  return test;
}

TestBase *
SNAPTestDriver
::CreateTestInstance(const char *name)
{
  TestBase *test = CreateNonTemplateTest(name);
  if(!test)
    test = TemplatedTestCreator<unsigned char>(name).GetTest();
  return test;
}

void
SNAPTestDriver
::Run(int argc, char *argv[])
{
  // Configure the command line
  CommandLineArgumentParser clap;
  clap.AddOption("help",1);
  clap.AddOption("list",0);
  clap.AddOption("test",1);
  clap.AddOption("type",1);

  // Parse the command line
  CommandLineArgumentParseResult parms;
  if(!clap.TryParseCommandLine(argc,argv,parms,false))
    {
    PrintUsage();
    return;
    }

  // Check if the user wants help
  if(parms.IsOptionPresent("help"))
    {
    // Get the file name
    const char *name = parms.GetOptionParameter("help");
    
    // Create a test instance, ingoring type
    TestBase *test = CreateTestInstance(name);

    // Print test usage
    if(test)
      {
      cout << "SNAPTests " << name << " options" << endl;
      cout << "Options: " << endl;
      test->PrintUsage();
      }
      
    else
      cerr << "Unknown test name: " << name << endl;
    }

  else if(parms.IsOptionPresent("list"))
    {
    // Print out a header
    cout << std::setw(20) << std::left << "Test Name";
    cout << std::setw(12) << std::left << "Templated";
    cout << "Description" << endl;
    
    // Go through the list of known tests
    for(unsigned int i=0;i<NUMBER_OF_TESTS;i++)
      {
      TestBase *test = CreateTestInstance(m_TestNames[i]);

      if(test) 
        {
        // Print out test info
        cout << std::setw(20) << std::left << m_TestNames[i];
        cout << std::setw(12) << std::left << m_TestTemplated[i] ? "Yes" : "No";
        cout << test->GetDescription() << endl;
        }
      }
    }
  else if(parms.IsOptionPresent("test"))
    {
    // Get the file name
    const char *name = parms.GetOptionParameter("test");
  
    // Check if the test can be created without a template parameter
    TestBase *test = CreateNonTemplateTest(name);

    // If that failed, check if the test can be created using a template 
    // parameter
    if(!test && (test = TemplatedTestCreator<unsigned char>(name).GetTest()))
      {
      // Get the template type or a blank string
      string type = parms.IsOptionPresent("type") ? 
        parms.GetOptionParameter("type") : "";

      // Instantiate the template test of the right type
      if(type == "char")      
        test = TemplatedTestCreator<char>(name).GetTest();
      else if(type == "unsigned_char") 
        test = TemplatedTestCreator<unsigned char>(name).GetTest();
      if(type == "short")      
        test = TemplatedTestCreator<short>(name).GetTest();
      else if(type == "unsigned_short") 
        test = TemplatedTestCreator<unsigned short>(name).GetTest();
      if(type == "int")      
        test = TemplatedTestCreator<int>(name).GetTest();
      else if(type == "unsigned_int") 
        test = TemplatedTestCreator<unsigned int>(name).GetTest();
      if(type == "long")      
        test = TemplatedTestCreator<long>(name).GetTest();
      else if(type == "unsigned_long") 
        test = TemplatedTestCreator<unsigned long>(name).GetTest();
      if(type == "float")      
        test = TemplatedTestCreator<float>(name).GetTest();
      else if(type == "double") 
        test = TemplatedTestCreator<double>(name).GetTest();
      else
        {
        cerr << "Missing or invalid or missing type parameter.  Using default (char)" << endl;
        cerr << "Should be one of: " << endl;
        cerr << "  char, unsigned_char," << endl;
        cerr << "  short, unsigned_short," << endl;
        cerr << "  int, unsigned_int," << endl;
        cerr << "  long, unsigned_long," << endl;
        cerr << "  float, " << endl;
        cerr << "  double." << endl;
        }
      }

    // See if a test has been created after all
    if(test) 
      {
      try 
        {
        // Configure the parameters of the test
        test->ConfigureCommandLineParser(clap);

        // Get the command line result
        CommandLineArgumentParseResult testParms;
        if(clap.TryParseCommandLine(argc,argv,testParms))
          {
          test->SetCommandLineParameters(testParms);
          test->Run();
          }
        else
          {
          test->PrintUsage();
          }
        
        delete test;
        }
      catch(TestUsageException)
        {
        test->PrintUsage();
        }
      catch(itk::ExceptionObject &exc)
        {
        cerr << "ITK Exception: " << endl << exc << endl;
        }
      catch(...)
        {
        cerr << "Unknowm Exception!" << endl;
        }
      }
    else
      {
      cerr << "Could not create test!" << endl;
      PrintUsage();
      }    
    }
  else
    {
    PrintUsage();
    }
}

