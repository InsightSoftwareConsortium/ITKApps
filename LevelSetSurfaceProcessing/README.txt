These examples show how to process surfaces with the 4th-order level set PDE framework.

You should read the online documentation for the appropriate filter
(also found in the header files) before attempting to any of these examples.

itk3DIsotropicFourthOrderLevelSetImageFilter
-------------------------------------------------

inputVolume: This should be a volume of floats in meta file format. 

outputVolume: The output of the filter is a volume of floats that is the same
size as the input volume.

isosurface_value: This is the value of the isosurface of the input volume that 
you want to process.

number_of_iterations: The more iterations the more smoothing.

itk3DAnisotropicFourthOrderLevelSetImageFilter
-------------------------------------------------

inputVolume: This should be a volume of floats in meta file format. 

outputVolume: The output of the filter is a volume of floats that is the same
size as the input volume.

isosurface_value: This is the value of the isosurface of the input volume that 
you want to process.

number_of_iterations: The more iterations the more smoothing.

conductance: This controls the amount of feature preservation. Its units are in 
curvature. Reasonable values for almost all surface are in the range 0.1-0.25 . 
The shape of the surface where the total curvature is above the value of this parameter 
will tend to stay the same (be preserved) or even be sharpened. The lower the value, 
the more feature preservation. Notice the difference between the number of iterations
parameter and the conductance parameter: for a given conductance parameter,
surface features with high enough curvature will be preserved even if the
number of iterations is set to be extremely large.
