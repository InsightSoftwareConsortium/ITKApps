InsightApplications README
--------------------------

This directory contains a variety of applications written with ITK. Many are
quite simple, come are complex and include GUI. Insight/Applications was
originally a directory within the Insight source code repository. However,
because of the size and complexity of this directory, the applications are
now a separate software module (e.g., CVS checkout). Please make sure that
you successfully build the core ITK prior to building this directory. Many of
these applications are tricky to build and depend on external packages. 

Some important configuration details:

     * VTK requires VTK version 4.4 and must be built ANSI_STDLIB
       (the ANSI_STDLIB flag is only a concern with VisualStudio 6.0).

     * CMake version 2.0.3 or later should be used.

     * FLTK version 1.1.4 is required

     * Qt  version 2.3 is required

Building with any other configurations is liable to give problems. 
Unfortunately, developers are unlikely to help with problems due to
using other configurations.

Note that even if you are planning to build only one of the applications in
this directory, you must first configure the entire set by running CMake and
providing the top level directory of InsighApplications as the SOURCE_DIR.

The reason for doing this is that there are a number of common pieces that are
shared among multiple applications and that will not be available if you
attempt to configure one application in isolation.

