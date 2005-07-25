Examples of usage:

A) GenerateProjection


A.1) Translate the image by (-100,-100,-100) then project.

./GenerateProjection  -t -100 -100 -100 /data/BrainWeb/brainweb165a10f17.mha  projection.mhd      

A.2) Translate the image by (-100,-100,-100), 
     rotate 10 degrees around x, then project.

./GenerateProjection  -t -100 -100 -100 -rx 10 /data/BrainWeb/brainweb165a10f17.mha projection.mhd


B) IntensityBased2D3DRegistration


B.1) Register the image that was generated.

./IntensityBased2D3DRegistration -t -100 -100 -100 projection.mhd /data/BrainWeb/brainweb165a10f17.mha


B.2) Register the image that was generated with rotation.


./IntensityBased2D3DRegistration -t -100 -100 -100  -rx 10 projection.mhd /data/BrainWeb/brainweb165a10f17.mha
