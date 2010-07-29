ITKApps README
--------------

This directory contains a variety of applications written with ITK. Many are
quite simple, come are complex and include GUI. Insight/Applications was
originally a directory within the Insight source code repository. However,
because of the size and complexity of this directory, the applications are
now a separate software module (e.g., CVS checkout). Please make sure that
you successfully build the core ITK prior to building this directory. Many of
these applications are tricky to build and depend on external packages. 

You must configure the build of this directory by using CMake. For this
you should create a binary directory where you plan to build the applications,
and then indicate to CMake the locations of the source directory and the 
binary directory.

This source tree is organized to be configured in a single piece. That is
you must provide to CMake the *TOP* directory of this tree as the source 
directory. Please do not give CMake any of the subdirectories, since in 
that case the application will lack elements required for the global configuration.

Once you configure the entire set of applications you can build applications
selectively. In VisualStudio you can do this by loading the ITKApps
workspace (.dsw) or solution (.sln) and then, from the IDE, selecting one of the 
specific projects that you want to build. In Linux you can simply "cd" in to the
appropriate subdirectory of the binary tree and type "make" at that level.
In general you may want to build first the "Auxiliary" directory, since it provides
services that may be used by other applications.

Some important configuration details:

     * VTK requires VTK version 4.4 and must be built ANSI_STDLIB
       (the ANSI_STDLIB flag is only a concern with VisualStudio 6.0).
       VTK should also be build with the flag VTK_USE_HYBRID ON.

     * CMake version 2.4.3 or later should be used.

     * FLTK version 1.1.6 is required

     * Qt  version 2.3 is required (if you select to build Qt examples)

Building with any other configurations is liable to give problems. 
Unfortunately, developers are unlikely to help with problems due to
using other configurations.



