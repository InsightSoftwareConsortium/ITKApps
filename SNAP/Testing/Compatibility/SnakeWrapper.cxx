/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeWrapper.cxx
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
 *          2   P(A) - P(B)       : field 3D scalar uniform float OPTIONAL
 *   output 0   implicit function : field 3D scalar uniform float
 *   param  0   dt                : float
 *          1   update interval   : int
 *          2   speed             : float
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

#include "SnakeWrapper.h"
#include <math.h>
#include <vnl/vnl_math.h>

snake3D::snake3D()
{
  //everything set to null
  xdim = ydim = zdim = size = 0;
  indt = 0;//NULL
  ing = inpapb = 0;//NULL
  outdt1 = outdt2 = 0;//NULL

  papb = g = u = inu = outu = gx = gy = gz = 0;//NULL
  g_MCF = g_del_g = g_c = g_smooth = 0;//NULL
  initflag = allocflag = calcflag = 0;
  converged = 0;

  set_default_params();
}
snake3D::~snake3D()
{
  cleanup();
}

void snake3D::cleanup()
{
  if (indt)
    {
    delete [] indt; indt = NULL;
    }
  if (outdt1)
    {
    delete [] outdt1; outdt1 = NULL;
    }
  if (outdt2)
    {
    delete [] outdt2; outdt2 = NULL;
    }
  if (g_c)
    {
    delete [] g_c; g_c = NULL;
    }
  if (g_MCF)
    {
    delete [] g_MCF; g_MCF = NULL;
    }
  if (g_del_g)
    {
    delete [] g_del_g; g_del_g = NULL;
    }
  if (g_smooth)
    {
    delete [] g_smooth; g_smooth = NULL;
    }
  if (gx)
    {
    delete [] gx; gx = NULL;
    }
  if (gy)
    {
    delete [] gy; gy = NULL;
    }
  if (gz)
    {
    delete [] gz; gz = NULL;
    }
  xdim = ydim = zdim = size = 0;
  initflag = allocflag = calcflag = 0;
  converged = 0;
  set_default_params();
}

void snake3D::init_prob(int _xdim, int _ydim, int _zdim, const unsigned char *init, const float *prob)
{
  initialize(_xdim,_ydim,_zdim,init);
  inpapb = prob;

  //make sure inpapb is within the range [-1..1]
  for (int ii=0; ii<size; ii++)
    {
    if (inpapb[ii] < -1.0 || inpapb[ii] > 1.0)
      {
      ECHO("Input P(A)-P(B) must have (min, max) of (-1.0, 1.0).");
      return;
      }
    }
}

void snake3D::init_prob(const int *dims, const unsigned char *init, const float *prob)
{
  init_prob(dims[0],dims[1],dims[2],init,prob);
}

void snake3D::init_edge(int _xdim, int _ydim, int _zdim, const unsigned char *init, const float *edge)
{
  initialize(_xdim,_ydim,_zdim,init);
  ing = edge;
}

void snake3D::init_edge(const int *dims, const unsigned char *init, const float *edge)
{
  init_edge(dims[0],dims[1],dims[2],init,edge);
}

void snake3D::initialize(int _xdim, int _ydim, int _zdim, const unsigned char *init)
{
  int i;

  cleanup();

  xdim = _xdim;
  ydim = _ydim;
  zdim = _zdim;
  size = xdim*ydim*zdim;

  //This part converts the binary initialization into a function
  //that is the snake.  The snake starts as a distance transform
  //from the surface of the binary initialization, with positive
  //distances outside, and negative distances inside

  //variables for the distance transform
  short * odx = NULL;
  short * ody = NULL;
  short * odz = NULL;
  unsigned char *oclass = NULL;

  //danielsson distance transform on the init image
  if (-1 == edt3ddan(init,_xdim,_ydim,_zdim,26,&odx,&ody,&odz,&oclass))
    {
    ECHO("ERROR with dan dist transform!!");
    cleanup();
    return;
    }

  indt = new float[size];

  if (!indt)
    {
    ECHO("new failed (indt)!");
    cleanup();
    return;
    }

  //get distances out of results from distance transform
  for (i=0;i<size;i++)
    {
    indt[i] = (float) sqrt((double) (odx[i] * odx[i] + ody[i] * ody[i] +
                                     odz[i] * odz[i]));
    }

  //now we need to do the distance transform on the
  //inverse of the image
  unsigned char * invinit = new unsigned char[size];

  if (!invinit)
    {
    ECHO("new failed (invinit)");
    cleanup();
    return;
    }

  //invert the binary initialization
  for (i = 0; i < size; i++)
    {
    invinit[i] = (init[i] == 0) ? 1 : 0;
    }

  //free memory before calling edt3ddan
  free(odx);
  free(ody);
  free(odz);
  free(oclass);

  //distance transform for inside the object
  if (-1 == edt3ddan(invinit,_xdim,_ydim,_zdim,26,&odx,&ody,&odz,&oclass))
    {
    ECHO("ERROR with dan dist transform!!");
    cleanup();
    return;
    }

  //subtract the inside distance transform from
  //the outside distance transform
  for (i=0;i<size;i++)
    {
    indt[i] -= (float) sqrt((double) (odx[i] * odx[i] + ody[i] * ody[i] +
                                      odz[i] * odz[i]));
    }

  delete [] invinit;
  free(odx);
  free(ody);
  free(odz);
  free(oclass);

  initflag = 1;
  allocflag = 0;
  calcflag = 0;
  converged = 0;
}

void snake3D::set_params(float _deltat, float _convergence, float _ground,
                         float _c_smooth, float _c_MCF, int _constraint, 
                         int _r_MCF, int _use_del_g, int _r_del_g, int _r_c, 
                         int _r_smooth)
{
  deltat = _deltat;
  convergence = _convergence;
  if (ground != _ground) allocflag = 0;
  ground = _ground;
  c_smooth = _c_smooth;
  c_MCF = _c_MCF;
  constraint = _constraint;
  r_MCF = _r_MCF;
  r_del_g = _r_del_g;
  use_del_g = _use_del_g;
  r_c = _r_c;
  r_smooth = _r_smooth;

  converged = 0;
  calcflag = 0;
}

void snake3D::set_default_params()
{
  set_params( DT_INIT, CONV_INIT, GROUND_INIT, C_SMOOTH_INIT,
              C_MCF_INIT, CONSTRAINT_INIT, R_MCF_INIT, 
              USE_DELG_INIT, R_DELG_INIT, R_C_INIT, R_SMOOTH_INIT);
}

float * snake3D::get_current_state()
{
  return u;
}

bool snake3D::isconverged()
{
  return converged!=0;
}

void snake3D::alloc_internal()
{
  int i,x,y,z;
  if (allocflag) return;

  ECHO("alloc_internal... ");

  // Allocate output fields
  if (outdt1) delete [] outdt1;
  outdt1 = new float[size];
  if (!outdt1)
    {
    ECHO("Allocation of output field failed.");
    return;
    }
  if (outdt2) delete [] outdt2;
  outdt2 = new float[size];
  if (!outdt2)
    {
    ECHO("Allocation of output field failed.");
    return;
    }
  //  outdt2 = outdt1;

  //set up internal variables
  //(this is not all necessary or
  //intuitive, but it was the easiest
  //conversion from the AVS code
  if (ing) g = ing;
  if (inpapb) papb = inpapb;
  u = outdt1;
  inu = indt;
  outu = outdt2;

  ECHO("BIG_REALLOC... ");

  // Initialize internal fields
#define BIG_REALLOC(field) \
  if (field) free(field); field = (float *) malloc(size * sizeof(float));

  BIG_REALLOC(g_c);
  if (ing)
    {
    BIG_REALLOC(g_MCF);
    BIG_REALLOC(g_del_g);
    BIG_REALLOC(g_smooth);
    BIG_REALLOC(gx);
    BIG_REALLOC(gy);
    BIG_REALLOC(gz);
    }

  ECHO("clip u to bounds... ");

  for (i=0; i<size; i++)  // clip u to +-ground
    {
    u[i] = inu[i];
    if (u[i] >  ground) u[i] =  ground;
    if (u[i] < -ground) u[i] = -ground;
    outu[i] = u[i];
    }

  ECHO("build gx/gy/gz... ");
  if (ing)
    {
    for (z=1; z<zdim-1; z++)
      for (y=1; y<ydim-1; y++)
        for (x=1; x<xdim-1; x++)
          {
          IDX(gx,x,y,z) = (IDX(g,x+1,y,z) - IDX(g,x-1,y,z)) /2.0f;
          IDX(gy,x,y,z) = (IDX(g,x,y+1,z) - IDX(g,x,y-1,z)) /2.0f;
          IDX(gz,x,y,z) = (IDX(g,x,y,z+1) - IDX(g,x,y,z-1)) /2.0f;
          }
    }

  ECHO("interp_border... ");

  interp_border(u, xdim, ydim, zdim);

  if (!ing) use_del_g = 0;

  allocflag = 1;
  calcflag = 0;
}

void snake3D::calc_internal()
{
  int i;
  if (calcflag) return;

  // Calculate g_MCF field
  if (ing)
    {
    ECHO("g_MCF... ");
    for (i=0; i<size; i++)
      g_MCF[i] = (float) pow(g[i], r_MCF+1);
    }

  // Calculate g_del_g field
  if (ing)
    {
    ECHO("g_del_g... ");
    for (i=0; i<size; i++)
      g_del_g[i] = use_del_g ? (float) pow(g[i], r_del_g) : 0.0f;
    }

  // Calculate g_c field
  ECHO("g_c... ");
  if (inpapb)         // Overloaded as P(A)-P(B)
    {
    //    if (initflag)
    for (i=0; i<size; i++) g_c[i] = -papb[i];
    } else
    {
    //    if (initflag || r_c != old_r_c) 
    for (i=0; i<size; i++) g_c[i] = (ing) ? (float) pow(g[i], r_c) : 1.0f;
    }

  ECHO("g_smooth... ");

  // Calculate g_smooth field
  if (ing)
    {
    for (i=0; i<size; i++)
      g_smooth[i] = (float) pow(g[i], r_smooth);
    }

  calcflag = 1;
}

int snake3D::run(int num_steps)
{

  if (!initflag) return 0;

  //  if (converged) return 1;

  if (!allocflag) alloc_internal();

  if (!calcflag) calc_internal();

/* internal variables */
  int iter_count=0; /* iteration count */

  int i, n, x, y, z;

  ECHO("isnake3D run... ");

//   ECHO("parameter presets... ");

//   // Parameter Presets
//   if (set_Sapiro)
//     r_MCF = 0, r_del_g = 0, r_c = 1, c_smooth = 0.0, r_smooth = 0;
//   if (set_Schlegel)
//     r_MCF = 1, r_del_g = 1, r_c = 2, c_smooth = 1.0, r_smooth = 2;
//   if (set_Turello)
//     r_MCF = 0, r_del_g = 1, r_c = 2, c_smooth = 0.25, r_smooth = 2;


  ECHO("enter main loop.\n");
  for (n=0; n<num_steps; n++)
    {
    char status[50];
    //snprintf(status, 50, "%d iters left", num_steps-n);
    sprintf(status, "%d iters left", num_steps-n);
    ECHO(status);

#define EPSILON 0.05
    char u_changed = 0;   // convergence flag

    int xydim = xdim*ydim;

    /* do the calculation */
    for (z=1; z<zdim-1; z++)
      {
      for (y=1; y<ydim-1; y++)
        {
        for (x=1; x<xdim-1; x++)
          {
          i = z*xdim*ydim + y*xdim + x;

          float ux = (u[i+1] - u[i-1]) / 2;
          float uy = (u[i+xdim] - u[i-xdim]) / 2;
          float uz = (u[i+xydim] - u[i-xydim]) / 2;
          float uxx = u[i-1] - 2*u[i] + u[i+1];
          float uyy = u[i-xdim] - 2*u[i] + u[i+xdim];
          float uzz = u[i-xydim] - 2*u[i] + u[i+xydim];
          float uxy = (u[i+xdim+1] - u[i+xdim-1] +
                       u[i-xdim-1] - u[i-xdim+1]) / 4;
          float uyz = (u[i+xydim+xdim] - u[i+xydim-xdim] +
                       u[i-xydim-xdim] - u[i-xydim+xdim]) / 4;
          float uzx = (u[i+xydim+1] - u[i+xydim-1] +
                       u[i-xydim-1] - u[i-xydim+1]) / 4;
          float ux2 = ux*ux;
          float uy2 = uy*uy;
          float uz2 = uz*uz;
          float grad_u = sqrt(ux2 + uy2 + uz2);

          // Forward and backward differences (added by paul)
          float ufx = u[i+1] - u[i];
          float ufy = u[i+xdim] - u[i];
          float ufz = u[i+xydim] - u[i];
          float ubx = u[i] - u[i-1];
          float uby = u[i] - u[i-xdim];
          float ubz = u[i] - u[i-xydim];

          float MCF_term = (float) (c_MCF * (
                                   (ux2+uy2)*uzz + (uy2+uz2)*uxx + (uz2+ux2)*uyy
                                   - 2*( uxy*ux*uy + uyz*uy*uz + uzx*uz*ux ) 
                                   ) / ( ux2 + uy2 + uz2 + eps ));
          if (ing) MCF_term *= g_MCF[i];

          float del_g_term = 0.0;      
          // if (ing) del_g_term = 
          //     g_del_g[i] * ( gx[i]*ux + gy[i]*uy + gz[i]*uz );

          // Use the simple upwind scheme to compute the advection term
          if (ing)
            {
            del_g_term = 
            ((-g_del_g[i] * gx[i] > 0) ? -ubx * g_del_g[i] * gx[i] : -ufx * g_del_g[i] * gx[i]) +
            ((-g_del_g[i] * gy[i] > 0) ? -uby * g_del_g[i] * gy[i] : -ufy * g_del_g[i] * gy[i]) + 
            ((-g_del_g[i] * gz[i] > 0) ? -ubz * g_del_g[i] * gz[i] : -ufz * g_del_g[i] * gz[i]);
            }

          // float c_term = convergence * g_c[i] * grad_u;

          float gradFB;
          if (g_c[i] > 0)
            {
            gradFB = 
            vnl_math_sqr( vnl_math_max(ubx, 0.0f) ) + vnl_math_sqr( vnl_math_min(ufx, 0.0f) ) + 
            vnl_math_sqr( vnl_math_max(uby, 0.0f) ) + vnl_math_sqr( vnl_math_min(ufy, 0.0f) ) + 
            vnl_math_sqr( vnl_math_max(ubz, 0.0f) ) + vnl_math_sqr( vnl_math_min(ufz, 0.0f) );
            } else
            {
            gradFB = 
            vnl_math_sqr( vnl_math_min(ubx, 0.0f) ) + vnl_math_sqr( vnl_math_max(ufx, 0.0f) ) + 
            vnl_math_sqr( vnl_math_min(uby, 0.0f) ) + vnl_math_sqr( vnl_math_max(ufy, 0.0f) ) + 
            vnl_math_sqr( vnl_math_min(ubz, 0.0f) ) + vnl_math_sqr( vnl_math_max(ufz, 0.0f) );
            }

          float c_term = convergence * g_c[i] * sqrt(gradFB);



          float smooth_term = c_smooth * (uxx + uyy + uzz);
          if (ing) smooth_term *= g_smooth[i];

          float du = MCF_term - del_g_term + c_term + smooth_term;
          outu[i] = u[i] + deltat*du;

          if (constraint)
            {
            if (outu[i] >  ground) outu[i] =  ground;
            if (outu[i] < -ground) outu[i] = -ground;
            }

          // Update convergence check (no motion)
          u_changed |= (du > EPSILON);
          }/* for x */
        } /* for y */
      } /* for z */

    float *tmp = u; u = outu; outu = tmp; /* swap maps */
    interp_border(u, xdim, ydim, zdim);

    iter_count++;

    // Check for convergence
    if (!u_changed)
      {
      ECHO("\nconverged!\n");
      converged = 1;
      break;
      }

    } /* for (n=0; n<interval; n++) */

  //#ifdef DEBUG
  //  printf("%d u = %p outdt1 = %p outdt2 = %p\n", iter_count, u, outdt1->data, outdt2->data);
  printf("%d u = %p outdt1 = %p outdt2 = %p\n", iter_count, u, *outdt1, *outdt2);
  //#endif

  return 1;

} /* run */

void interp_border(float *u, int xsize, int ysize, int zsize)
{
  int i, j, xysize, x, y, z;

  xysize = ysize*xsize;

  i = 0;
  j = (zsize-1)*xysize;
  for (y = 0; y < ysize; y++)
    {
    for (x = 0; x < xsize; x++, i++, j++)
      {
      u[i] = 2*u[i+xysize] - u[i+2*xysize];
      u[j] = 2*u[j-xysize] - u[j-2*xysize];
      }
    }

  for (z = 0; z < zsize; z++)
    {
    i = z*ysize*xsize;
    j = i + (ysize-1)*xsize;
    for (x = 0; x < xsize; x++, i++, j++)
      {
      u[i] = 2*u[i+xsize] - u[i+2*xsize];
      u[j] = 2*u[j-xsize] - u[j-2*xsize];
      }
    }

  for (z = 0; z < zsize; z++)
    {
    i = z*ysize*xsize;
    j = i + (xsize-1);
    for (y = 0; y < ysize; y++, i+=xsize, j+=xsize)
      {
      u[i] = 2*u[i+1] - u[i+2];
      u[j] = 2*u[j-1] - u[j-2];
      }
    }
} /* interp_border */


#define SQ(x) sqtab[x]
#define DIS(x,y,z) (SQ(x)+SQ(y)+SQ(z))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

/* extension of  Danielsson algorithm  to 3D */
//params:
//input: uchar label volume image
//six: x dimension of input
//siy: y dimension of input
//siz: z dimension of input
//metric: 6|18|26 connectedness
//odx: output of x distance
//ody: output of y distance
//odz: output of z distance
//oclass: output of label assignements.  the label
//in oclass is the label that is at the distance
//described by odx,ody,odz
//
//this final distance can be computed as
//sqrt(odx[i]^2 + ody[i]^2 + odz[i]^2)
//
//Nathan Moon modifications:
//-changed post-processing to not overwrite input
//-added oclass that returns the class closest to
//each voxel.
//-I changed input to be uchar, which is sufficient 
//for this application since the input will always 
//be binary
int edt3ddan(const unsigned char *input,int six,int siy,int siz,unsigned char metric,short **odx,short **ody,short **odz,unsigned char **oclass)
{

  printf("edt2ddan()\n");
  int x,y,z,index,indexc, dd;
  int *sqtab;
  int *sq;
  int maxsiz;
  short *dx, *dy, *dz;
  int stop, upy;
  int part;
  char msg[256];
  int dist, dst;
  unsigned char *cl;//Nathan Moon addition

  printf("starting DT(size=[%d,%d,%d], metric=%d\n",six,siy,siz,metric);
  maxsiz = MAX(six,siy);
  maxsiz = MAX(maxsiz,siz);
  maxsiz = 4 * maxsiz;
  if (!(sq=sqtab=(int *)malloc((4*maxsiz+1)*sizeof(int))))
    {
    return(-1);
    }
  sqtab = &sqtab[2*maxsiz];
  for (index=(-2*maxsiz);index<=(2*maxsiz);index++)
    {
    sqtab[index] = index * index;
    }
  if (!(dx=(short *)malloc((six*siy*siz)*sizeof(short))))
    {
    free(sq);
    return(-1);
    }
  if (!(dy=(short *)malloc((six*siy*siz)*sizeof(short))))
    {
    free(dx);
    free(sq);
    return(-1);
    }
  if (!(dz=(short *)malloc((six*siy*siz)*sizeof(short))))
    {
    free(dy);
    free(dx);
    free(sq);
    return(-1);
    }
  if (!(cl=(unsigned char*)malloc((six*siy*siz)*sizeof(unsigned char))))
    {
    free(dz);
    free(dy);
    free(dx);
    free(sq);
    return(-1);
    }
  dd = six * siy;
  printf("filling initial array\n");
  for (x=0;x<six;x++)
    {
    for (y=0;y<siy;y++)
      {
      for (z=0;z<siz;z++)
        {
        index = x + six * y + dd * z;
        if (input[index])
          {
          dx[index] = dy[index] = dz[index] = 0;
          } else
          {
          dx[index] = dy[index] = dz[index] = maxsiz;
          }
        }
      }
    }
  printf("forward scan\n");
  //  status("starting forward scan",20);  
  for (z=1;z<siz;z++)
    {
    part = (int) (20.0 + 25.0 * ((float) z - 1.0) / ((float) siz - 1.0));
    /* printf("%d. slice forward scan\n",z); */
    sprintf(msg,"%d. slice forward scan",z);
    //    status(msg,part);
    upy = siy - 1;
    stop = six - 1;
    for (y=0;y<siy;y++)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - dd;
        if ((dst=DIS(dx[indexc],dy[indexc],dz[indexc]-1)) < dist)
          {
          dx[index] = dx[indexc];
          dy[index] = dy[indexc];
          dz[index] = dz[indexc] - 1;
          dist = dst;
          }
        if (metric != 6)
          {
          if (y > 0)
            {
            indexc = index - six - dd;
            if ((dst=DIS(dx[indexc],dy[indexc]-1,dz[indexc]-1)) < dist)
              {
              dx[index] = dx[indexc];
              dy[index] = dy[indexc] - 1;
              dz[index] = dz[indexc] - 1;
              dist = dst;
              }
            }
          if (y < upy)
            {
            indexc = index + six - dd;
            if ((dst=DIS(dx[indexc],dy[indexc]+1,dz[indexc]-1)) < dist)
              {
              dx[index] = dx[indexc];
              dy[index] = dy[indexc] + 1;
              dz[index] = dz[indexc] - 1;
              dist = dst;
              }
            }
          if (x > 0)
            {
            indexc = index - 1 - dd;
            if ((dst=DIS(dx[indexc]-1,dy[indexc],dz[indexc]-1)) < dist)
              {
              dx[index] = dx[indexc] - 1;
              dy[index] = dy[indexc];
              dz[index] = dz[indexc] - 1;
              dist = dst;
              }
            }
          if (x < stop)
            {
            indexc = index + 1 - dd;
            if ((dst=DIS(dx[indexc]+1,dy[indexc],dz[indexc]-1)) < dist)
              {
              dx[index] = dx[indexc] + 1;
              dy[index] = dy[indexc];
              dz[index] = dz[indexc] - 1;
              dist = dst;
              }
            }
          if (metric == 26)
            {
            if (y > 0)
              {
              if (x > 0)
                {
                indexc = index - dd - six - 1;
                if ((dst=DIS(dx[indexc]-1,dy[indexc]-1,dz[indexc]-1)) < dist)
                  {
                  dx[index] = dx[indexc] - 1;
                  dy[index] = dy[indexc] - 1;
                  dz[index] = dz[indexc] - 1;
                  dist = dst;
                  }
                }
              if (x < stop)
                {
                indexc = index - dd - six + 1;    
                if ((dst=DIS(dx[indexc]+1,dy[indexc]-1,dz[indexc]-1)) < dist)
                  {
                  dx[index] = dx[indexc] + 1;
                  dy[index] = dy[indexc] - 1;
                  dz[index] = dz[indexc] - 1;
                  dist = dst;
                  }
                }
              }
            if (y < upy)
              {
              if (x > 0)
                {
                indexc = index - dd + six - 1;
                if ((dst=DIS(dx[indexc]-1,dy[indexc]+1,dz[indexc]-1)) < dist)
                  {
                  dx[index] = dx[indexc] - 1;
                  dy[index] = dy[indexc] + 1;
                  dz[index] = dz[indexc] - 1;
                  dist = dst;
                  }
                }
              if (x < stop)
                {
                indexc = index - dd + six + 1;
                if ((dst=DIS(dx[indexc]+1,dy[indexc]+1,dz[indexc]-1)) < dist)
                  {
                  dx[index] = dx[indexc] + 1;
                  dy[index] = dy[indexc] + 1;
                  dz[index] = dz[indexc] - 1;
                  dist = dst;
                  }
                }
              }
            }
          }
        }
      }
    for (y=1;y<siy;y++)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - six;
        if ((dst=DIS(dx[indexc],dy[indexc]-1,dz[indexc])) < dist)
          {
          dx[index] = dx[indexc];
          dy[index] = dy[indexc] - 1;
          dz[index] = dz[indexc];
          dist = dst;
          }
        if (metric != 6)
          {
          if (x > 0)
            {
            indexc = index - six - 1;
            if ((dst=DIS(dx[indexc]-1,dy[indexc]-1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] - 1;
              dy[index] = dy[indexc] - 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          if (x < stop)
            {
            indexc = index - six + 1;
            if ((dst=DIS(dx[indexc]+1,dy[indexc]-1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] + 1;
              dy[index] = dy[indexc] - 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          }
        }
      index = z * dd + y * six + 1;
      for (x=1;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - 1;
        if ((dst=DIS(dx[indexc]-1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] - 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      index = z * dd + (y + 1) * six - 2;
      for (x=(six-2);x>=0;x--,index--)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + 1;
        if ((dst=DIS(dx[indexc]+1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] + 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      }
    for (y=(siy-2);y>=0;y--)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + six;
        if ((dst=DIS(dx[indexc],dy[indexc]+1,dz[indexc])) < dist)
          {
          dx[index] = dx[indexc];
          dy[index] = dy[indexc] + 1;
          dz[index] = dz[indexc];
          dist = dst;
          }
        if (metric != 6)
          {
          if (x > 0)
            {
            indexc = index + six - 1;
            if ((dst=DIS(dx[indexc]-1,dy[indexc]+1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] - 1;
              dy[index] = dy[indexc] + 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          if (x < stop)
            {
            indexc = index + six + 1;
            if ((dst=DIS(dx[indexc]+1,dy[indexc]+1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] + 1;
              dy[index] = dy[indexc] + 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          }
        }
      index = z * dd + y * six + 1;
      for (x=1;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - 1;
        if ((dst=DIS(dx[indexc]-1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] - 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      index = z * dd + (y + 1) * six - 2;
      for (x=(six-2);x>=0;x--,index--)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + 1;
        if ((dst=DIS(dx[indexc]+1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] + 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      }
    }
  printf("backward scan\n");
  //  status("starting backward scan",45);
  for (z=siz-2;z>=0;z--)
    {
    part = (int) (45.0 + 25.0 * ((float) (siz - 2 - z) / ((float) siz - 1.0)));
    /* printf("%d. slice backward scan\n",z); */
    sprintf(msg,"%d. slice backward scan",z);
    //    status(msg,part);
    upy = siy - 1;
    stop = six - 1;
    for (y=0;y<siy;y++)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + dd;
        if ((dst=DIS(dx[indexc],dy[indexc],dz[indexc]+1)) < dist)
          {
          dx[index] = dx[indexc];
          dy[index] = dy[indexc];
          dz[index] = dz[indexc] + 1;
          dist = dst;
          }
        if (metric != 6)
          {
          if (y > 0)
            {
            indexc = index - six + dd;
            if ((dst=DIS(dx[indexc],dy[indexc]-1,dz[indexc]+1)) < dist)
              {
              dx[index] = dx[indexc];
              dy[index] = dy[indexc] - 1;
              dz[index] = dz[indexc] + 1;
              dist = dst;
              }
            }
          if (y < upy)
            {
            indexc = index + six + dd;
            if ((dst=DIS(dx[indexc],dy[indexc]+1,dz[indexc]+1)) < dist)
              {
              dx[index] = dx[indexc];
              dy[index] = dy[indexc] + 1;
              dz[index] = dz[indexc] + 1;
              dist = dst;
              }
            }
          if (x > 0)
            {
            indexc = index - 1 + dd;
            if ((dst=DIS(dx[indexc]-1,dy[indexc],dz[indexc]+1)) < dist)
              {
              dx[index] = dx[indexc] - 1;
              dy[index] = dy[indexc];
              dz[index] = dz[indexc] + 1;
              dist = dst;
              }
            }
          if (x < stop)
            {
            indexc = index + 1 + dd;
            if ((dst=DIS(dx[indexc]+1,dy[indexc],dz[indexc]+1)) < dist)
              {
              dx[index] = dx[indexc] + 1;
              dy[index] = dy[indexc];
              dz[index] = dz[indexc] + 1;
              dist = dst;
              }
            }
          if (metric == 26)
            {
            if (y > 0)
              {
              if (x > 0)
                {
                indexc = index + dd - six - 1;
                if ((dst=DIS(dx[indexc]-1,dy[indexc]-1,dz[indexc]+1)) < dist)
                  {
                  dx[index] = dx[indexc] - 1;
                  dy[index] = dy[indexc] - 1;
                  dz[index] = dz[indexc] + 1;
                  dist = dst;
                  }
                }
              if (x < stop)
                {
                indexc = index + dd - six + 1;    
                if ((dst=DIS(dx[indexc]+1,dy[indexc]-1,dz[indexc]+1)) < dist)
                  {
                  dx[index] = dx[indexc] + 1;
                  dy[index] = dy[indexc] - 1;
                  dz[index] = dz[indexc] + 1;
                  dist = dst;
                  }
                }
              }
            if (y < upy)
              {
              if (x > 0)
                {
                indexc = index + dd + six - 1;
                if ((dst=DIS(dx[indexc]-1,dy[indexc]+1,dz[indexc]+1)) < dist)
                  {
                  dx[index] = dx[indexc] - 1;
                  dy[index] = dy[indexc] + 1;
                  dz[index] = dz[indexc] + 1;
                  dist = dst;
                  }
                }
              if (x < stop)
                {
                indexc = index + dd + six + 1;
                if ((dst=DIS(dx[indexc]+1,dy[indexc]+1,dz[indexc]+1)) < dist)
                  {
                  dx[index] = dx[indexc] + 1;
                  dy[index] = dy[indexc] + 1;
                  dz[index] = dz[indexc] + 1;
                  dist = dst;
                  }
                }
              }
            }
          }
        }
      }
    for (y=1;y<siy;y++)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - six;
        if ((dst=DIS(dx[indexc],dy[indexc]-1,dz[indexc])) < dist)
          {
          dx[index] = dx[indexc];
          dy[index] = dy[indexc] - 1;
          dz[index] = dz[indexc];
          dist = dst;
          }
        if (metric != 6)
          {
          if (x > 0)
            {
            indexc = index - six - 1;
            if ((dst=DIS(dx[indexc]-1,dy[indexc]-1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] - 1;
              dy[index] = dy[indexc] - 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          if (x < stop)
            {
            indexc = index - six + 1;
            if ((dst=DIS(dx[indexc]+1,dy[indexc]-1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] + 1;
              dy[index] = dy[indexc] - 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          }
        }
      index = z * dd + y * six + 1;
      for (x=1;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - 1;
        if ((dst=DIS(dx[indexc]-1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] - 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      index = z * dd + (y + 1) * six - 2;
      for (x=(six-2);x>=0;x--,index--)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + 1;
        if ((dst=DIS(dx[indexc]+1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] + 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      }
    for (y=(siy-2);y>=0;y--)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + six;
        if ((dst=DIS(dx[indexc],dy[indexc]+1,dz[indexc])) < dist)
          {
          dx[index] = dx[indexc];
          dy[index] = dy[indexc] + 1;
          dz[index] = dz[indexc];
          dist = dst;
          }
        if (metric != 6)
          {
          if (x > 0)
            {
            indexc = index + six - 1;
            if ((dst=DIS(dx[indexc]-1,dy[indexc]+1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] - 1;
              dy[index] = dy[indexc] + 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          if (x < stop)
            {
            indexc = index + six + 1;
            if ((dst=DIS(dx[indexc]+1,dy[indexc]+1,dz[indexc])) < dist)
              {
              dx[index] = dx[indexc] + 1;
              dy[index] = dy[indexc] + 1;
              dz[index] = dz[indexc];
              dist = dst;
              }
            }
          }
        }
      index = z * dd + y * six + 1;
      for (x=1;x<six;x++,index++)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index - 1;
        if ((dst=DIS(dx[indexc]-1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] - 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      index = z * dd + (y + 1) * six - 2;
      for (x=(six-2);x>=0;x--,index--)
        {
        dist = DIS(dx[index],dy[index],dz[index]);
        indexc = index + 1;
        if ((dst=DIS(dx[indexc]+1,dy[indexc],dz[indexc])) < dist)
          {
          dx[index] = dx[indexc] + 1;
          dy[index] = dy[indexc];
          dz[index] = dz[indexc];
          dist = dst;
          }
        }
      }
    }
  printf("post-proc\n");
  //  status("starting post-processing",70);
  for (z=0;z<siz;z++)
    {
    for (y=0;y<siy;y++)
      {
      index = z * dd + y * six;
      for (x=0;x<six;x++,index++)
        {
        if (((x+dx[index]) < 0) || ((x+dx[index]) > (six - 1)) ||
            ((y+dy[index]) < 0) || ((y+dy[index]) > (siy - 1)) ||
            ((z+dz[index]) < 0) || ((z+dz[index]) > (siz - 1)))
          {
          printf("unclassified point [%d,%d,%d]\n",x,y,z);
          exit(-111);
          }
        if (!input[index])
          {
          cl[index] = input[(z+dz[index])*dd+(y+dy[index])*six+
                            (x+dx[index])];
          }
        }
      }
    }
  *odx = dx;
  *ody = dy;
  *odz = dz;
  *oclass = cl;
  free(sq);
  printf("finished edt2ddan()\n");
  return(0);
}





/*Log: SnakeWrapper.cxx
/*Revision 1.1  2003/07/11 23:26:58  pauly
/**** empty log message ***
/*
/*Revision 1.2  2003/07/10 14:30:26  pauly
/*Integrated ITK into SNAP level set segmentation
/*
/*Revision 1.1  2003/03/07 19:29:47  pauly
/*Initial checkin
/*
/*Revision 1.1.1.1  2002/12/10 01:35:36  pauly
/*Started the project repository
/*
/*
/*Revision 1.12  2002/05/08 17:34:43  moon
/*Don't remember
/*
/*Revision 1.11  2002/04/27 18:30:29  moon
/*Finished commenting
/*
/*Revision 1.10  2002/04/25 14:13:47  moon
/*Fiddled.  Didn't fix anything.
/*
/*Revision 1.9  2002/04/24 19:52:44  moon
/*Found a bug with reallocation of memory.
/*
/*Revision 1.8  2002/04/16 13:09:05  moon
/*Changed code so that modifying parameters doesn't start the snake over.
/*Was recalculating some things that I didn't need to recalc when the calcflag
/*was set.  This was a pretty big bug.
/*
/*Revision 1.7  2002/04/10 21:21:09  moon
/*added RCS tags (finally!  I didn't realize they weren't on)
/**/
