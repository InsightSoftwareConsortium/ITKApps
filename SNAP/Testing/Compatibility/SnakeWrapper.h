/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeWrapper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

/**********************
 * Wrapper class for the snake code, taken
 * from the AVS module general_isnake_3D
 * 
 * Original conversion: Nathan Moon
 * start date: 15 Feb 2002
 *
 * Overview:
 * There are two basic "snakes," edge-driven and probablistic.
 * Both start with a binary initialization (a blob in the volume,
 * basically).  The snake code creates a distance transform as an
 * initialization of the implicit function for the snake.
 *
 * The edge-driven snake either grows or shrinks globally !!!**don't know how to control this yet!!!***
 * and is attracted to edges in the image.
 *
 * The probablistic snake runs on a probability image that tells
 * the snake basically "in" or "out", and it grows or shrinks
 * locally depending on the probability.
 *
 * original header comments preserved below:
 ***********************/
/**********
 * gensnake.c - source code of general_isnake_3D module
 *   Author : Yongjik Kim
 *            (Original source of 'implicite snake 3D' by :
 *               Silvio Turello, Joachim Schlegel, SA, Brech Szekely)
 *
 * AVS description:
 *   input  0   initialization    : field 3D scalar uniform float REQUIRED
 *          1   g                 : field 3D scalar uniform float OPTIONAL
 *          2   P(A) - P(B)   : field 3D scalar uniform float OPTIONAL
 *   output 0   implicit function : field 3D scalar uniform float
 *   param  0   dt                : float
 *          1   update interval   : int
 *          2   speed         : float
 *          3   ground            : float
 *          4   c_MCF             : float
 *          5   r_MCF             : int
 *          6   use  del g        : boolean
 *          7   r_ del g          : int
 *          8   r_c               : int
 *          9   c_smooth          : float
 *         10   r_smooth          : int
 *              Set Parameters to Presets
 *         11     set to Sapiro   : oneshot
 *         12     set to Schlegel : oneshot
 *         13     set to Turello  : oneshot
 *         16   clamp to ground   : boolean
 *         17   run continuously  : boolean
 *         18   run one step      : oneshot
 *         19   reset             : oneshot
 **********/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

// #define eps 0.000001 /* small number to prevent division by zero */
static const double eps = 0.000001;

#define ECHO(str) fprintf(stderr, str)

// index into any one of the full-image arrays (u, g, papb, etc)
#define IDX(u,x,y,z) (u)[((z)*ydim+(y))*xdim+(x)]

/*******************************
 * Default Value of Parameters *
 *******************************/
//default and range for time step (delta t)
#define DT_INIT           0.2f
#define DT_MIN            0.0f
#define DT_MAX            1.0f
//default and range for convergence 
//(really speed of snake propagation)
#define CONV_INIT         2.0f
#define CONV_MIN         -5.0f
#define CONV_MAX          5.0f
//default number of iterations to run
//between visualization updates
#define INTERVAL_INIT     1
//default values for various variables
#define R_MCF_INIT        0
#define R_DELG_INIT       0
#define USE_DELG_INIT     1
#define R_C_INIT          1
#define R_SMOOTH_INIT     0
#define C_SMOOTH_INIT     0.0f
#define C_MCF_INIT        1.0f
//default and range for implicit function clamp values
#define GROUND_INIT       5.0f
#define GROUND_MIN        0.0f
#define GROUND_MAX       30.0f
//flag whether or not to use the ground value
#define CONSTRAINT_INIT   1 /* on */

class snake3D
{

public:

  /////////////////////////////
  //constructor
  //does little (inits everything to NULL)
  snake3D();

  /////////////////////////////
  //destructor
  //calls cleanup
  ~snake3D();

  /////////////////////////////
  //init_prob
  //initializes the snake object to run as a probablistic snake
  //pre: none
  //params:
  //_xdim,_ydim,_zdim: dimensions of the volume.  init and prob must have
  //the same dimensionality
  //init: volume binary initialization of the snake
  //prob: volume probability image that snake runs on.  Values must be in 
  //the range [-1:1] **don't know which is inside, which is outside
  void init_prob(int _xdim, int _ydim, int _zdim, const unsigned char *init, const float *prob);
  void init_prob(const int *dims, const unsigned char *init, const float *prob);

  /////////////////////////////
  //init_edge
  //initializes the snake object to run as an edge-driven snake
  //pre: none
  //params:
  //_xdim,_ydim,_zdim: dimensions of the volume.  init and edge must have
  //the same dimensionality
  //init: volume binary initialization of the snake
  //edge: volume edge image that snake runs on.
  void init_edge(int _xdim, int _ydim, int _zdim, const unsigned char *init, const float *edge);
  void init_edge(const int *dims, const unsigned char *init, const float *prob);

  /////////////////////////////
  //set_params
  //sets the snake parameters
  //pre: none
  //params:  see kimy-snakes paper for explanation of parameters
  void set_params(float _deltat, float _convergence, float _ground,
          float _c_smooth, float _c_MCF, int _constraint, 
          int _r_MCF, int _use_del_g, int _r_del_g, int _r_c, 
          int _r_smooth);

  /////////////////////////////
  //set_default_params
  //calls set_params with default values as defined above
  //pre: none
  void set_default_params();

  /////////////////////////////
  //run
  //runs the snake for num_steps iterations
  //pre: one of the init_* methods must have been called.  Otherwise run 
  //returns 0 without doing anything
  //params:
  //num_steps: number of iterations to run before returning
  int run(int num_steps=INTERVAL_INIT);

  /////////////////////////////
  //get_current_state
  //returns the current state of the snake
  //pre: run should have been called.  Otherwise NULL is returned
  //post: do not modify the array returned
  //return: float array the same size as the volume passed to init_*
  float * get_current_state();

  /////////////////////////////
  //isconverged
  //returns true if the snake has been run to convergence
  //pre: none
  bool isconverged();

  /////////////////////////////
  //cleanup
  //de-allocates all memory
  //pre: none
  //post: init_* must be called before the snake can be run again
  void cleanup();

private:

  /////////////////////////////
  //initialize
  //initializes the snake array
  void initialize(int _xdim, int _ydim, int _zdim, const unsigned char *init);

  /////////////////////////////
  //alloc_internal
  //does memory allocation for internal variables
  void alloc_internal();

  /////////////////////////////
  //calc_internal
  //does calculation of internal variables
  void calc_internal();


  /////////////////////////////
  // data structures

  // input field dimensions
  int xdim,ydim,zdim,size;
  // Input field data structures
  float *indt;
  const float *inpapb;
  const float *ing;
  // Output field data structures
  float *outdt1, *outdt2;
  // Parameters
  float deltat, convergence, ground, c_smooth, c_MCF;
  int constraint;
  int r_MCF, use_del_g, r_del_g, r_c, r_smooth;
//   int set_Sapiro, set_Schlegel, set_Turello;
//   int running=0, step=1, reset, temp_arg;

  /////////////////////////////
  //internal variables
  const float *g, *papb;
  float *u, *inu, *outu, *gx, *gy, *gz;
  float *g_MCF, *g_del_g, *g_c, *g_smooth;
  int initflag,allocflag,calcflag,converged;
};

/////////////////////////////
//interp_border
//no idea, part of original code
void interp_border(float *u, int xsize, int ysize, int zsize);

/////////////////////////////
//edt3ddan
//distance transform
//taken from AVS module
int edt3ddan(const unsigned char *input,int six,int siy,int siz,unsigned char metric,short **odx,short **ody,short **odz,unsigned char **oclass);

/*
 *Log: SnakeWrapper.h
 *Revision 1.3  2003/09/11 13:50:29  pauly
 *FIX: Enabled loading of images with different orientations
 *ENH: Implemented image save and load operations
 *
 *Revision 1.2  2003/08/27 14:03:22  pauly
 *FIX: Made sure that -Wall option in gcc generates 0 warnings.
 *FIX: Removed 'comment within comment' problem in the cvs log.
 *
 *Revision 1.1  2003/07/12 04:50:42  pauly
 *Initial checkin of SNAP testing code to the InsightApplications tree
 *
 *Revision 1.1  2003/07/11 23:26:58  pauly
 **** empty log message ***
 *
 *Revision 1.1  2003/03/07 19:29:47  pauly
 *Initial checkin
 *
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.4  2002/05/08 17:34:44  moon
 *Don't remember
 *
 *Revision 1.3  2002/04/10 21:21:10  moon
 *added RCS tags (finally!  I didn't realize they weren't on)
 **/
