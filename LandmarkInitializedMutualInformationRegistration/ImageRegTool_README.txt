ImageRegTool
============


ImageRegTool demonstrates the application-level flexibility for ITK for
image-image registration.


Features:
---------

1) This software provides multiple methods for registration initialization:
  * Centers of the images
  * Centers of mass of the voxels in the images
  * Second moments of the voxels in the images (provides an initial
      orientation as well as aligning their centers of mass)
  * Four common landmarks in the images
  * Loading a rigid transform from a file

2) This software provides multiple registration transforms / strategies
  * Rigid registration (using a versor transform)
  * Affine registration (using a versor, scale, and skew decomposition)
  * Rigid registration followed by affine registration (using a versor
      transform until convergence, then including scale and skew
      information until convergence)

3) Files are written using a compressed, meta-image format by default, but
a variety of other file formats can be read and written.  See ITK's software
guide for a description of the file formats supported by ITK.

4) This program can be operated using command-line arguments instead of its
GUI.  This option allows this program to be used for batch-processing.  A
batch-processing tool is underdevelopment for the inclusion in ITK.


Important Facts:
----------------

Unlike the base implementation of the ITK registration framework, this
application provides moving-image-to-fixed-image transforms (the default
behaviour in ITK is to have registration optimization return 
fixed-image-to-moving-image transforms).

The registration process uses a two-phase optimization scheme.   An initial,
coarse registration (after initialization) using a one-plus-one evolutionary
optimizer.  This is optimizer has Bayesian characterists.   After it
converges, a Powell optimizer with Brent line searching is applied.


TO-DO:
------

This tool will be extended in the future.  Please submit bug reports to the
ITK-users list and/or Stephen Aylward (aylward@unc.edu).

Planned extensions include
  * Loading application-specific settings.   We have defined settings for
    inter-patient head MR registration, intra-patient head MR registration,
    intra-patient liver CT registration, etc.   We will make these settings
    available and provide an easy method for selecting them.

