Last update Oct 2003:

WINDOWS USERS: This application is supported on MS Windows, but you'll
need to pay close attention to the instructions below.

1. To build this application:

 You will need a recent version of VTK installed on your machine.  I
  suggest getting the latest release.

 In your VTK build, make sure you include the Patented classes and Tcl
  wrapping.  You will need the Wrapping/Tcl directories from the VTK
  source tree.

 WINDOWS USERS: The precompiled VTK distribution for Windows not built
  with USE_ANSI_STDLIB option enabled, which is required for
  compatibility with ITK.

 You will need to download the VTK source and build with
  USE_ANSI_STDLIB turned "On".

 When you configure InsightApplications with CMake, turn the following
  option "On"

   USE_VTK 

 Check the VTK paths in CMake by clicking the View Advanced Settings
  box.

2. To run this application:

 cd <Insight Applications SOURCE>/SegmentationEditor/Scripts
 <Insight Applications BUILD>/bin/WSTITK Main.tcl

 where "Insight Applications SOURCE" is the path to your
  InsightApplications source tree and "Insight Applications BUILD" is
  the path to your InsightApplications build tree.

3. When you run the application for the first time, a window will pop
    up asking for the paths to your VTK source and your SegmentationEditor
    source.  These paths are the directories to the source code, not to the 
    build tree.

   WINDOWS USERS: When specifying paths use double backslashes
    (i.e. c:\\path\\to\\VTK).  Tcl is picky about this and the application
    will not run if you use single backslashes or forward slashes.

4. The rest of the documentation you will need is displayed by the program
    itself.  Also see the documentation in the Applications section of the
    www.itk.org website.

5. Report any problems to the Insight-users list (see www.itk.org).

KNOWN BUGS:

1. VTK Warning and possible seg fault on exit.

 This is a known VTK issue when using VTK/Tk render window widgets.
 There are no plans to address this problem in the SegmentationEditor
 application.

2. "Cannot find package vtkinteraction ... " on startup.

 See instruction number 3.  It is likely that you have supplied an
 incorrect path to the VTK or SegmentationEditor source.  If you are 
 running on Windows, make sure that you have used double backslashes 
 in your path specification.
