/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    OpenGLSliceTexture.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __OpenGLSliceTexture_h_
#define __OpenGLSliceTexture_h_

#include <FL/gl.h>

#include "itkImage.h"
#include "itkConstantPadImageFilter.h"

/**
 * \class OpenGLSliceTexture
 * \brief This class is used to turn a 2D ITK image of (arbitrary) type
 * into a GL texture.  
 *
 * The calls to Update will make sure that the texture is up to date.  
 */
template<class TPixel> class OpenGLSliceTexture 
{
public:
  // Image typedefs
  typedef itk::Image<TPixel,2> ImageType;
  typedef typename itk::SmartPointer<ImageType> ImagePointer;

  /** Constructor, initializes the texture object */
  OpenGLSliceTexture();

  /** Destructor, deallocates texture memory */
  virtual ~OpenGLSliceTexture();
  
  /** Pass in a pointer to a 2D image */
  void SetImage(ImagePointer inImage);

  /** Get the dimensions of the texture image, which are powers of 2 */
  irisGetMacro(TextureSize,Vector2ui);

  /** Get the GL texture number automatically allocated by this object */
  irisGetMacro(TextureIndex,int);

  /** Set the number of components used in call to glTextureImage */
  irisSetMacro(GlComponents,GLuint);

  /** Get the format (e.g. GL_LUMINANCE) in call to glTextureImage */
  irisSetMacro(GlFormat,GLenum);

  /** Get the type (e.g. GL_UNSIGNED_INT) in call to glTextureImage */
  irisSetMacro(GlType,GLenum);

  /**
   * Make sure that the texture is up to date (reflects the image)
   */
  void Update();

  /**
   * Draw the texture in the current OpenGL context on a polygon with vertices
   * (0,0) - (size_x,size_y)
   */
  void Draw();

  /**
   * Draw the texture in transparent mode, with given level of alpha blending.
   */
  void DrawTransparent(unsigned char alpha);

private:
  
  // Filter typedefs
  typedef itk::ConstantPadImageFilter<ImageType,ImageType> FilterType;
  typedef typename itk::SmartPointer<FilterType> FilterPointer;

  // The dimensions of the texture as stored in memory
  Vector2ui m_TextureSize;

  // The pointer to the image from which the texture is computed
  ImagePointer m_Image;

  // The padding filter used to resize the image
  FilterPointer m_PadFilter;

  // The texture number (index)
  unsigned int m_TextureIndex;

  // Has the texture been initialized?
  bool m_IsTextureInitalized;

  // The pipeline time of the source image (vs. our pipeline time)
  unsigned long m_UpdateTime;

  // The number of components for Gl op
  GLuint m_GlComponents;

  // The format for Gl op
  GLenum m_GlFormat;

  // The type for Gl op
  GLenum m_GlType;
};

template<class TPixel>
OpenGLSliceTexture<TPixel>
::OpenGLSliceTexture()
{
  // Set to -1 to force a call to 'generate'
  m_IsTextureInitalized = false;

  // Create the filter
  m_PadFilter = FilterType::New();

  // Set the update time to -1
  m_UpdateTime = 0;

  // Init the GL settings to uchar, luminance defautls, which are harmless
  m_GlComponents = 1;
  m_GlFormat = GL_LUMINANCE;
  m_GlType = GL_UNSIGNED_BYTE;
}

template<class TPixel>
OpenGLSliceTexture<TPixel>
::~OpenGLSliceTexture()
{
  if(m_IsTextureInitalized)
    glDeleteTextures(1,&m_TextureIndex);
}

template<class TPixel>
void
OpenGLSliceTexture<TPixel>
::SetImage(ImagePointer inImage)
{
  if(m_Image != inImage)
  {
    m_Image = inImage;
    m_UpdateTime = 0;
  }
}

template<class TPixel>
void
OpenGLSliceTexture<TPixel>
::Update()
{
  // Better have an image
  assert(m_Image);

  // Update the image (necessary?)
  m_Image->Update();

  // Check if everything is up-to-date and no computation is needed
  if (m_IsTextureInitalized && m_UpdateTime == m_Image->GetPipelineMTime())
    return;

  // Promote the image dimensions to powers of 2
  itk::Size<2> szImage = m_Image->GetLargestPossibleRegion().GetSize();
  m_TextureSize = Vector2ui(1);

  // Use shift to quickly double the coordinates
  for (unsigned int i=0;i<2;i++)
    while (m_TextureSize(i) < szImage[i])
      m_TextureSize(i) <<= 1;

  // Compute the pad offset
  vnl_vector_fixed<unsigned long,2> 
    offset(m_TextureSize(0) - szImage[0],m_TextureSize(1) - szImage[1]);

  // Set the parameters of the pad filter
  m_PadFilter->SetInput(m_Image);
  m_PadFilter->SetPadUpperBound(offset.data_block());

  // Apply the padding
  m_PadFilter->UpdateLargestPossibleRegion();

  // Create the texture index if necessary
  if(!m_IsTextureInitalized)
    {
    // Generate one texture
    glGenTextures(1,&m_TextureIndex);
    m_IsTextureInitalized = true;
    }

  // Select the texture for pixel pumping
  glBindTexture(GL_TEXTURE_2D,m_TextureIndex);

  // Properties for the texture
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  
  // Pump the pixels into the texture
  glTexImage2D(GL_TEXTURE_2D,0,m_GlComponents,
               m_TextureSize(0),m_TextureSize(1),
               0,m_GlFormat,m_GlType,
               m_PadFilter->GetOutput()->GetBufferPointer());

  // Remember the image's timestamp
  m_UpdateTime = m_Image->GetPipelineMTime();
}

template<class TPixel>
void
OpenGLSliceTexture<TPixel>
::Draw()
{
  // Update the texture
  Update();

  // Should have a texture number
  assert(m_IsTextureInitalized);

  // GL settings
  glPushAttrib(GL_TEXTURE_BIT);
  glEnable(GL_TEXTURE_2D);

  // Select our texture
  glBindTexture(GL_TEXTURE_2D,m_TextureIndex);

  // Set the color to white
  glColor3ub(255,255,255);
    
  // Draw quad 
  glBegin(GL_QUADS);
  glTexCoord2d(0,0);
  glVertex2d(0,0);
  glTexCoord2d(0,1);
  glVertex2d(0,m_TextureSize(1));
  glTexCoord2d(1,1);
  glVertex2d(m_TextureSize(0),m_TextureSize(1));
  glTexCoord2d(1,0);
  glVertex2d(m_TextureSize(0),0);
  glEnd();

  glPopAttrib();
}

template<class TPixel>
void
OpenGLSliceTexture<TPixel>
::DrawTransparent(unsigned char alpha)
{
  // Update the texture
  Update();

  // Should have a texture number
  assert(m_IsTextureInitalized);

  // GL settings
  glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);

  // Turn on alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  // Select our texture
  glBindTexture(GL_TEXTURE_2D,m_TextureIndex);

  // Set the color to white
  glColor4ub(255,255,255,alpha);
    
  // Draw quad 
  glBegin(GL_QUADS);
  glTexCoord2d(0,0);
  glVertex2d(0,0);
  glTexCoord2d(0,1);
  glVertex2d(0,m_TextureSize(1));
  glTexCoord2d(1,1);
  glVertex2d(m_TextureSize(0),m_TextureSize(1));
  glTexCoord2d(1,0);
  glVertex2d(m_TextureSize(0),0);
  glEnd();

  glPopAttrib();
}











#endif // __OpenGLSliceTexture_h_
