/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    BubblesInteractionMode.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "BubblesInteractionMode.h"

#include "IRISApplication.h"
#include "IRISSliceWindow.h"
#include "UserInterfaceLogic.h"
#include "IRISImageData.h"

#include <assert.h>
#include <cmath>

BubblesInteractionMode
::BubblesInteractionMode(GenericSliceWindow *parent)
:GenericSliceWindow::EventHandler(parent)
{
}

void
BubblesInteractionMode
::OnDraw()
{
  // Draw the bubbles if necessary
  if (m_GlobalState->GetSnakeActive() == false)
    {
    // draw bubbles
    int numBubbles = m_ParentUI->GetNumberOfBubbles();
    if (numBubbles > 0)
      {
      // Get the active color label 
      int currentcolor =  m_GlobalState->GetDrawingColorLabel();      
      ColorLabel cl = 
        m_Driver->GetCurrentImageData()->GetColorLabel(currentcolor);
      
      if (cl.IsValid())
        {
        // Get the current alpha blending factor for displaying overlays
        unsigned char alpha = m_GlobalState->GetSegmentationAlpha();
        
        // Get the color of the active color label
        unsigned char rgb[3];
        cl.GetRGBVector(rgb);

        // Get the lust of bubbles
        Bubble *bubbles = m_ParentUI->GetBubbles();
        
        // Get the current crosshairs position
        Vector3ui cursorImage = m_GlobalState->GetCrosshairsPosition();

        // Get the image space dimension that corresponds to this window
        int iid = m_Parent->m_ImageAxes[2];
        
        // Get the other essentials from the parent
        Vector3f scaling = m_Parent->m_SliceSpacing;       
        
        // Turn on alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Create a filled circle object
        GLUquadricObj *object = gluNewQuadric();
        gluQuadricDrawStyle(object,GLU_FILL);

        // Draw each bubble
        for (int i = 0; i < numBubbles; i++)
          {  
          
          // Get the center and radius of the i-th bubble
          Vector3f ctrImage = to_float(bubbles[i].center) + Vector3f(0.5f);
          int radius = bubbles[i].radius;

          // Remap the center into slice coordinates
          Vector3f ctrSlice = m_Parent->MapImageToSlice(to_float(ctrImage));

          // Compute the offset from the center along the slice z-direction
          // in physical coordinates
          float dcenter = scaling(2) * (cursorImage(iid) - ctrImage(iid));
                    
          // Check if the bubble is intersected by the current slice plane
          if (dcenter >= radius || -dcenter >= radius) continue;
            
          // Compute the radius of the bubble in the cut plane
          float diskradius = sqrt(fabs(radius*radius - dcenter*dcenter));

          // Draw the bubble
          glColor4ub(rgb[0],rgb[1],rgb[2],alpha);
          glPushMatrix();
          
          glTranslatef(ctrSlice[0], ctrSlice[1], 0.0f);
          glScalef(1.0f / scaling(0),1.0f / scaling(1),1.0f);
          gluDisk(object,0,diskradius,100,100);
          glPopMatrix();

          }

        gluDeleteQuadric(object);
        glDisable(GL_BLEND);
        }
      }
    }
}


