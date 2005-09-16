#include "TransferFunctionEditor.h"
#include <iostream>
#include<cmath>
#include <math.h>
#define M_PI 3.145
#define Radius 5
// the constructor method

TransferFunctionEditor::TransferFunctionEditor(int x,int y,int w,int h,const char *l):Fl_Gl_Window(x,y,w,h,l)
{
  glShadeModel (GL_SMOOTH);
  m_SliceViewer = NULL;
  m_colorchooser = NULL;
  activeTransFunc=1;
  currentSelection=transFuncPoints[0].end();
  maxTransFuncPoints=0;
   CurrentPoint.r=1.0;
  CurrentPoint.g=0.0;
  CurrentPoint.b=0.0;
  CurrentPoint.x=0;
  CurrentPoint.y=0;//80
  transFuncPoints[0].push_back (CurrentPoint); 
  CurrentPoint.r=1.0;
  CurrentPoint.g=0.0;
  CurrentPoint.b=0.0;
  CurrentPoint.x=694;
  CurrentPoint.y=0;//80
  transFuncPoints[0].push_back (CurrentPoint);
  CurrentPoint.r=0.0;
  CurrentPoint.g=1.0;
  CurrentPoint.b=1.0;
  CurrentPoint.x=0;
  CurrentPoint.y=30;
  transFuncPoints[1].push_back (CurrentPoint); 
  CurrentPoint.r=0.0;
  CurrentPoint.g=1.0;
  CurrentPoint.b=0.0;
  CurrentPoint.x=694;
  CurrentPoint.y=30;
  transFuncPoints[1].push_back (CurrentPoint); 
}

std::list<coord> & TransferFunctionEditor::GetOpacityTransferFunction() 
{
  return transFuncPoints[0];
}

void 
TransferFunctionEditor::SetPointcolor(double r,double g,double b)
{
    CurrentPoint.r=(float)r;
    CurrentPoint.g=(float)g;
    CurrentPoint.b=(float)b;
    (*currentSelection).r=(float)r;
    (*currentSelection).g=(float)g;
    (*currentSelection).b=(float)b;
}
//Set the pointer to the vtk drawing class
void 
TransferFunctionEditor::SetImageSliceViewer(fltk::DeformableRegistration3DTimeSeriesBase* viewer)
{
  m_SliceViewer = viewer;
}
//Set the pointer to the colorchooser object from GUI
void
TransferFunctionEditor::SetColorChooser(Fl_Color_Chooser* colorchooser)
{
  m_colorchooser = colorchooser;
}

// the drawing method: it draws the transFunc into the window
void TransferFunctionEditor::draw() 
{
  // 
  if (!valid()) 
    {
    glLoadIdentity(); glViewport(0,0,w(),h()); gluOrtho2D(0,w(),0,h());
    make_current();
    }
  int i,j;
  // clear the window

  glClearColor(0.0,0.0,0.0,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

  // draw the histogram in white here:
  // ( h() gives you the window height, w() the width)
  int x1,y1,x2,y2,max;
  max=histogram[0];
  for(j=1;j<694;j++)
    {
    if(max<histogram[j])max=histogram[j];
    }
  for(j=0;j<694;j++)
    {
    y1=((float) histogram[j]/(float) max)*h();
    x1=j;
    x2=j;
    y2=0;
    glColor3f(1.0,1.0,1.0);  // white
    glBegin(GL_LINES);
      glVertex2i(x1,y1);
      glVertex2i(x2,y2);
    glEnd();
    }
  std::cout<<"start display function values"<<std::endl;
  for(j=0;j<2;j++)
    {
    // draw the maxTransFunc transFunc's
    
    glColor3f(1.0,0.0,0.0);  
    std::list<coord>::iterator iter;
    // Iterate through list and output each element.
    double angle;
    for (iter=transFuncPoints[j].begin(); iter != transFuncPoints[j].end(); iter++)
      {
      if(j==1)
        {
          glColor3f((*iter).r,(*iter).g,(*iter).b); 
        }

      glBegin(GL_POLYGON);
      for(i =0;i<14;i++)
          {
          angle = i*2*M_PI/14; 
          glVertex2f((*iter).x+cos(angle)*Radius, (*iter).y+sin(angle)*Radius);
          }
      glEnd(); 
      }

      //Draw a circle for the selected point
      glColor3f(1.0,1.0,0.0);  
      glBegin(GL_LINE_LOOP);         
      for(i =0;i<14;i++)
        { 
        angle = i*2*M_PI/14;
        glVertex2f((*currentSelection).x+cos(angle)*(Radius+2),(*currentSelection).y+sin(angle)*(Radius+2));
        }
      glEnd(); 
      glBegin(GL_LINES);
      for (iter=transFuncPoints[j].begin(); iter != transFuncPoints[j].end(); iter++)
        {
        if(activeTransFunc&&j)//Choose between color or opacity transfer function
          {
          std::cout<<"Color:"<<(*iter).r<<(*iter).g<<(*iter).b<<std::endl;
          m_SliceViewer->RemoveColorTransferFunctionPoint(( CurrentPoint.x*5.9));//Remove the color of the last dragged point
          m_SliceViewer->SetColorTransferFunctionPoint((*iter).x*5.9,(*iter).r,(*iter).g,(*iter).b);
          }
        if((activeTransFunc==0)&&(j==0))
          {
          std::cout<<"Opacity:"<<(*iter).y*0.004<<" at point"<<(*iter).x*5.9<<std::endl;
          m_SliceViewer->RemoveOpacityTransferFunctionPoint(( CurrentPoint.x*5.9));//Remove the opacity of the last dragged point
          m_SliceViewer->SetOpacityTransferFunctionPoint(((*iter).x*5.9),((*iter).y*0.004));
          }
        //std::cout<<((*iter).x*5.9)<<" "<<((*iter).y*0.004)<<std::endl;
        glVertex2i((*iter).x,(*iter).y);
        std::list<coord>::iterator next_iter = iter;
        next_iter++;
        // If we didn't fall off the end, print out next element.
        if (next_iter != transFuncPoints[j].end())
          {
          glVertex2i((*next_iter).x,(*next_iter).y);
          }
        } 
    glEnd();
    }   
  /*
  for (double i=0;i<1;i=i+0.01)
    for (double j=0;j<1;j=j+0.01)
      for (double k=0;k<1;k=k+0.01)
        {
        glBegin(GL_LINES); 
        glColor3f(i,j,k); 
        glVertex2i((i+j+k)*100,0);
        glVertex2i((i+j+k)*100,50);
        glEnd();
        }  
        */
}


// the event handler that processes the mouse events in the transfer function editor window
// you will not have to change anything here
void TransferFunctionEditor::SetHistogramSize(int size)
{
  for(int i=1;i<size;i++)
    {
    histogram[i]=0;
    }
}
void TransferFunctionEditor::SetActiveTransferFunction(int number)
{
  activeTransFunc=number;
}
void TransferFunctionEditor::SetHistogramValues(int bin,int value)
{
  histogram[bin]=value;
}

int TransferFunctionEditor::handle(int eventType)
{

  int curx,cury,button;
  button=Fl::event_button();
  //if((eventType==FL_RELEASE)&&(button==1)) currentSelection=transFuncPoints[activeTransFunc].end(); 
  curx=Fl::event_x();
  // FLTK's origin is at the top of the window, we would like it at the bottom
  // subtract returned y coordinate from the window's height
  cury=h()-Fl::event_y();
  if(curx>694) 
    curx=694;
  else if(curx<0)
    curx=0; 
  if(cury>h()) 
    cury=h();
  else if(cury<0)
    cury=0; 
  if((eventType==FL_DRAG)&&(button==1))
    {
    if(currentSelection!=transFuncPoints[activeTransFunc].end())
      {
      
      if(CurrentPoint.x<=Radius)
        {
        curx=0;//If is the first point
        }
      if(CurrentPoint.x>=(694-Radius))curx=694;//If is the Last point
      
      if(activeTransFunc==1)cury=30;
      
      if((max-curx>(Radius))&&(curx-min>(Radius)))  
        {
          (*currentSelection).x=curx;
          (*currentSelection).y=cury; 

        }
      }
    }
  else
  // mouse moves with button depressed
  if((eventType==FL_PUSH)&&(button==1))
    {
    if(activeTransFunc==1)cury=30;
    
    std::list<coord>::iterator iter;
    std::list<coord>::iterator next_iter;
    std::list<coord>::iterator prev_iter;
    // Iterate through list and output each element.
    for (iter=transFuncPoints[activeTransFunc].begin(); iter != transFuncPoints[activeTransFunc].end(); iter++)
      {
      if(((curx-(*iter).x)*(curx-(*iter).x)+(cury-(*iter).y)*(cury-(*iter).y))<=Radius*Radius)
        {
        currentSelection=iter; 
        //Compute the interval where the current selected point can be dragged
        next_iter = iter;
        next_iter++;
        if (next_iter != transFuncPoints[activeTransFunc].end())
          max=(*next_iter).x;
        else
          max=695+Radius;//Values is +Radius+1 greater than the max extreme
        if (iter != transFuncPoints[activeTransFunc].begin())
          {
          prev_iter=iter;
          --prev_iter;
          min=(*prev_iter).x;
          }
        else
          min=-Radius-1;//Values is +Radius+1 smaller than the min extreme
        
        //Use this point for override the point that is going to be dragged
        CurrentPoint.x=(*iter).x;
        CurrentPoint.y=(*iter).y;
        m_colorchooser->rgb((*iter).r,(*iter).g,(*iter).b);
        }
      //Inserting an a element between two existing elements
      else
        {
        next_iter = iter;
        next_iter++;
        if (next_iter != transFuncPoints[activeTransFunc].end())       
          if((curx-(*iter).x>(Radius+1))&&(curx-(*next_iter).x<(-1-Radius)))
            {
            CurrentPoint.x=curx;
            CurrentPoint.y=cury;  
            transFuncPoints[activeTransFunc].insert(next_iter, CurrentPoint);
            currentSelection=transFuncPoints[activeTransFunc].end(); 
            }    
        }
      }
    if(eventType==FL_FOCUS)show();
    //Adding a new element at the end of the function
    /*
    if(currentSelection==transFuncPoints.end())
      {
      CurrentPoint.x=curx;
      CurrentPoint.y=cury;
      transFuncPoints.push_back (CurrentPoint); 
      }
    */
  }
  // redraw the transFuncPoints into window
  redraw();

  return(1);
}


