Last update Feb 2004:

This file tells how to configure, build, and run the application.  For more
information see the Tutorial.pdf document in this directory.

WINDOWS USERS: This application is supported on MS Windows.  Please see the
special instructions in step 3.

1. To build this application:
   
   * You will need a recent version of VTK installed on your machine with 
   the options VTK_USE_PATENTED, and VTK_WRAP_TCL set to "On".  This
   application was last tested with VTK 4.2 and VTK 4.4.

   * Configure and build InsightApplications with USE_VTK set to "On".

2. To run this application:
   
   cd <InsightApplications-source>/SegmentationEditor/Scripts/

    where <InsightApplications-source> is the source directory of your build
    (the source code that you downloaded).

   <InsightApplications-build>/SegmentationEditor/Wrapping/Tcl/WSTITK Main.tcl

    where <InsightApplications-build> is the directory where you built
    InsightApplications.  Note that WSTITK will be found in EXECUTABLE_OUTPUT_PATH
    instead if one was specified during the CMake configuration process.

3. When you run the application for the first time, a window will pop
   up asking for the paths to your VTK BUILD tree and your SegmentationEditor
   SOURCE tree.

   WINDOWS USERS: When specifying paths use double backslashes
    (i.e. c:\\path\\to\\VTK).  Tcl is picky about this and the application
    will not run if you use single backslashes or forward slashes.

4. The rest of the documentation you will need is displayed by the program
    itself.  Also see the documentation in the Applications section of the
    www.itk.org website and the file SegmentationEditor.pdf in this directory.

5. Report any problems to the Insight-users list (see www.itk.org).

KNOWN BUGS:

1. VTK Warning and possible seg fault on exit.

 This is a known VTK issue when using VTK/Tk render window widgets.
 There are no plans to address this problem in the SegmentationEditor
 application.

2. "Cannot find package vtkinteraction ... " on startup.

 See instruction number 3.  It is likely that you have supplied an
 incorrect path to the VTK build tree or to the SegmentationEditor source.  If
 you are running on Windows, make sure that you have used double backslashes 
 in your path specification. 
