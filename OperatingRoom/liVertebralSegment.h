//--------------------------------------------
//
//     Project: Operating Room
//
//     Author:  Luis Ibanez
//
//     Division of Neurosugery
//     Department of Surgery
//     University of North Carolina
//     Chapel Hill, NC 27599
//
//--------------------------------------------

#ifndef liVERTEBRALSEGMENTMODELCLASS
#define liVERTEBRALSEGMENTMODELCLASS

#include <fstream>
#include <liPointSet3D.h>
#include <liVertebralBody.h>
#include <liVertebralPedicle.h>



namespace li {


/**
 *  This class implements a 3D model of a generic Vertebral Segment 
 */
class ITK_EXPORT VertebralSegment : public Shape3D
{

public:
  /**
   * Standard "Self" typedef.
   */
  typedef VertebralSegment   Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef Shape3D  Superclass;

  /** 
   * Smart pointer typedef support.
   */
  typedef itk::SmartPointer<Self>  Pointer;

  /** 
   * ConstSmart pointer typedef support.
   */
  typedef itk::SmartPointer<const Self>  ConstPointer;


  /** 
   * PointSet type
   */
  typedef PointSet3D::PointSetType  PointSetType;


  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro( VertebralSegment, Shape3D );


  /**
   * Method for creation through the object factory.
   */
  itkNewMacro( Self );
    


  /**
   *  Destructor
   */
	~VertebralSegment();


  /**
   *  Draw the Model using OpenGL commands
   */
	void DrawGeometry(void) const;

  
  /**
   *  Set the name of this model
   */
	virtual void SetName(const char *newName);
	
  
  /**
   *  Get the name of this model
   */
  const char * GetName(void);
	
  
  /**
   *  Set the vertical shift with respect to the previous Segment
   */
  void SetShift(double val);
	
  
  /**
   *  Get the vertical shift with respect to the previous Segment
   */
  double GetShift(void);
	
  
  /**
   *  Set the rotation with respect to the previous Segment
   */
  void SetRotation(double val);
	
  
  /**
   *  Get the rotation with respect to the previous Segment
   */
  double GetRotation(void);


  /**
   *  Get the Vertebral body
   */
  VertebralBody::Pointer        GetVertebralBody(void);

	
  /**
   *  Get the Vertebral Pedicles
   */
  VertebralPedicle::Pointer     GetVertebralPedicle(void);
	
  
  /**
   *  Write the serialized model to an ostream
   */
	int Write( Shape3D::OfstreamType &os, int level ) const;

  
  /**
   *  Read the serialized model of an ostream
   */
  virtual int Read( Shape3D::IfstreamType &is );


  virtual void SetAxisVisible( bool );


  virtual void SetSamplesVisible( bool );

  /**
   *  Draw the axis of the coordinate system 
   *  attached to a particular vertebra
   *  this will be removed from here... later
   */
  virtual void DrawAxis( void ) const;


  PointSet3D::Pointer & GetLeftPediclePointSet(void )  { return m_LeftPediclePointSet;  }
  PointSet3D::Pointer & GetRightPediclePointSet(void ) { return m_RightPediclePointSet; }
  PointSet3D::Pointer & GetTopBodyPointSet(void )      { return m_TopBodyPointSet;      }
  PointSet3D::Pointer & GetBottomBodyPointSet(void )   { return m_BottomBodyPointSet;   }

protected:

  /**
   *  Constructor
   */
	VertebralSegment();


private:

  /**
   *  Name assigned to this model
   */
	char *      m_Name;
  

  /**
   *  Separation from the previous Vertebral Segment
   */
	double      m_Shift;


  /**
   *  Rotation with respect to the previous Segment
   */
	double      m_Rotation;

  
  /**
   *  Model of the Vertebral Body
   */
  VertebralBody::Pointer        m_Body;


  /**
   *  Model of the Pedicles (both left and right)
   */
  VertebralPedicle::Pointer     m_Pedicles;


  /**
   *  Defines whether the axis are visible or not
   */
  bool                          m_AxisVisible;

  /**
   *  Defines whether the samples are visible or not
   */
  bool                          m_SamplesVisible;

  /**
   *  Receptors for synthetic samples
   */
  PointSet3D::Pointer               m_LeftPediclePointSet;
  PointSet3D::Pointer               m_RightPediclePointSet;
  PointSet3D::Pointer               m_TopBodyPointSet;
  PointSet3D::Pointer               m_BottomBodyPointSet;




};


} // end namespace li

#endif




