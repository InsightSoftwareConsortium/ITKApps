
/**
 *
 * Constructor
 *
 */

template<class TImagePixel>
LandmarkSliceViewer<TImagePixel>
::LandmarkSliceViewer( unsigned int x, unsigned int y, unsigned int w, unsigned int h, const char * label=0 )
: Superclass(x,y,w,h,label)
{ 
  m_LandmarkSelectionMode = false;
  m_Action = NoAction;
  m_SelectedLandmarkIndex = 0;

  OverlayPixelType p;

  p.Set(0,0,255,255);
  m_DefaultColor[0] = p;

  p.Set(255,0,0,255);
  m_DefaultColor[1] = p;

  p.Set(0,255,0,255);
  m_DefaultColor[2] = p;

  p.Set(255,255,0,255);
  m_DefaultColor[3] = p;

  m_UseCustomColor = false;
  m_RegistrationLandmarks = RegistrationPointSetType::New() ;
  m_LandmarkSpatialObject = LandmarkSpatialObjectType::New() ;
}

/**
 *
 * Destructor
 *
 */

template<class TImagePixel>
LandmarkSliceViewer<TImagePixel>
::~LandmarkSliceViewer()
{

}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetInputImage( ImageType* image )
{
  Superclass::SetInputImage(image) ;

  OverlayPixelType p ;
  p.Fill(0) ;
  typename OverlayImageType::Pointer overlay = OverlayImageType::New();
  overlay->SetRegions(image->GetLargestPossibleRegion());
  overlay->SetSpacing(image->GetSpacing());
  overlay->SetOrigin(image->GetOrigin());
  overlay->Allocate();
  overlay->FillBuffer(p);
  
  this->SetInputOverlay(overlay.GetPointer()) ;
  this->ViewOverlayData(true);
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetLandmarks( LandmarkSpatialObjectType* landmarks)
{
  unsigned int size = m_Landmarks.size() ;
  for ( unsigned int i = 0 ; i < size ; ++i )
    {
      this->DeleteLandmark(i) ;
    }

  m_Landmarks.clear() ;
  m_LandmarkAvailables.clear() ;
  m_LandmarkIndexes.clear() ;

  typedef LandmarkSpatialObjectType::PointListType ListType ;
  m_LandmarkSpatialObject->SetPoints( landmarks->GetPoints() ) ;
  ListType points = m_LandmarkSpatialObject->GetPoints() ;
  
  ListType::iterator iter = points.begin() ;
  while ( iter != points.end() )
    {
      this->AddLandmark(*iter) ;
      ++iter ;
    }
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::LandmarkSpatialObjectType*
LandmarkSliceViewer<TImagePixel>
::GetLandmarks()
{
  typedef LandmarkSpatialObjectType::PointListType ListType ;
  
  ListType landmarkList ;
  //  landmarkList.reserve(m_Landmarks.size()) ;

  //  m_LandmarkSpatialObject->Reserve(m_Landmarks.size()) ;
  PointVectorType::iterator iter = m_Landmarks.begin() ;
  BoolVectorType::iterator a_iter = m_LandmarkAvailables.begin() ;

  while ( iter != m_Landmarks.end() )
    {
      if ( *a_iter )
        {
          landmarkList.push_back( *iter ) ;
        } 
      ++iter ;
      ++a_iter ;
    }
  
  m_LandmarkSpatialObject->SetPoints(landmarkList) ;
  return m_LandmarkSpatialObject.GetPointer() ;
}

template<class TImagePixel>
unsigned int 
LandmarkSliceViewer<TImagePixel>
::GetNumberOfLandmarks()
{
  PointVectorType::iterator iter = m_Landmarks.begin() ;
  BoolVectorType::iterator a_iter = m_LandmarkAvailables.begin() ;

  unsigned int size = 0 ;
  while ( iter != m_Landmarks.end() )
    {
      if ( *a_iter )
        {
          ++size ;
        } 
      ++iter ;
      ++a_iter ;
    }

  return size ;
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::RegistrationPointSetType*
LandmarkSliceViewer<TImagePixel>
::GetRegistrationLandmarks( void )
{
  m_RegistrationLandmarks->Reserve(m_Landmarks.size()) ;
  PointVectorType::iterator iter = m_Landmarks.begin() ;
  BoolVectorType::iterator a_iter = m_LandmarkAvailables.begin() ;
  unsigned int id = 0 ;
  while ( iter != m_Landmarks.end() )
    {
      if ( *a_iter )
        {
          m_RegistrationLandmarks->InsertElement(id, (*iter).GetPosition()) ;
        } 
      ++iter ;
      ++a_iter ;
      ++id ;
    }
  return m_RegistrationLandmarks.GetPointer();
}

/**
 *
 * Return a reference to the 'index'th landmarks of the list.
 * If the 'index' specified as argument is invalid, then an
 * exception is thrown.
 *
 */

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::PointType*
LandmarkSliceViewer<TImagePixel>
::GetLandmark( unsigned int index )
{
  if( index >= m_Landmarks.size() || !m_LandmarkAvailables[index])
    {
      return 0 ;
    }

  return &(m_Landmarks[index]);
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::IndexType*
LandmarkSliceViewer<TImagePixel>
::GetIndexLandmark( unsigned int index )
{
  if( index >= m_Landmarks.size() || !m_LandmarkAvailables[index])
    {
      return 0 ;
    }

  return &(m_LandmarkIndexes[index]);
}

/**
 *
 * Add the landmark at the end of the list of current landmarks, and set 
 * its color to the color specified as argument.
 *
 */

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::AddLandmark( PointType& point )
{
  unsigned int v_index = m_Landmarks.size() ;
  IndexType index ;
  cOverlayData->TransformPhysicalPointToIndex(point.GetPosition(), index) ;
  m_UseCustomColor = true ;
  m_SelectedLandmarkColor = point.GetColor() ;
  this->CreateLandmark( v_index, index ) ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::AddLandmark( unsigned int index )
{
  m_LandmarkSelectionMode = true;
  m_SelectedLandmarkIndex = index;
  m_Action = Add;
  m_UseCustomColor = false;
}

template<class TImagePixel>
void 
LandmarkSliceViewer<TImagePixel>
::AddLandmark( OverlayPixelType color )
{
  m_LandmarkSelectionMode = true;
  m_Action = Add;
  m_UseCustomColor = true;
  m_SelectedLandmarkColor = color;
}


/**
 *
 * Modify the landmark at the 'index'th position in the
 * list of landmarks by the landmark passed as second argument.
 *
 */

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ModifyLandmark( unsigned int index )
{
  m_LandmarkSelectionMode = true;
  m_Action = Modify;
  m_SelectedLandmarkIndex = index;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::CreateLandmark( unsigned int v_index, const IndexType& index )
{
  if ( m_Landmarks.size() < (v_index + 1) )
    {
      m_Landmarks.resize( v_index + 1 ) ;
      m_LandmarkIndexes.resize( v_index + 1 ) ;
      m_LandmarkAvailables.resize( v_index + 1, false ) ;
    }
  m_LandmarkAvailables[v_index] = true ;
  m_LandmarkIndexes[v_index] = index ;
  
  PointType p ;
  RegistrationPointType temp_p ;
  cOverlayData->TransformIndexToPhysicalPoint
    (index, temp_p) ;
  p.SetPosition(temp_p) ;

  if( m_UseCustomColor )
    {
      //need to do the conversion of an integer into an RGBA pixel...
      p.SetColor(m_SelectedLandmarkColor);
    }
  else
    {
      PointPixelType color ;                
      for ( unsigned int i = 0 ; i < 4 ; i++ )
        {
          color[i] = m_DefaultColor[(v_index%4)][i] ;
        }
      
      //need to do the conversion of an integer into an RGBA pixel...
      // using the default colors.... m_DefaultColor[(m_Landmarks.size()%4)];
      p.SetColor(color);
      
    }

  m_Landmarks[v_index] =  p ;
  
  DrawPoint(index, p.GetColor());
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DeleteLandmark( unsigned int v_index )
{
  PointPixelType color ;
  color.Fill(0) ;
  m_LandmarkAvailables[v_index] = false ;
  DrawPoint(m_LandmarkIndexes[v_index], color);
  this->update() ;
  this->redraw() ;
}

/**
 *
 * Remove the 'index'th landmark.
 *
 */

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::RemoveLandmark( unsigned int index )
{
  this->DeleteLandmark(index) ;
}

template<class TImagePixel>
int 
LandmarkSliceViewer<TImagePixel>
::handle( int event )
{
  Superclass::handle(event);

  switch(event)
  {
  case FL_PUSH:
    if( m_LandmarkSelectionMode )
      {
        ClickPoint coord ;
        this->getClickedPoint(0, coord);
        IndexType index ;
        index[0] = (long) coord.x ;
        index[1] = (long) coord.y ;
        index[2] = (long) coord.z ;

      switch( m_Action )
        {
        case Add:
          {
            this->CreateLandmark(m_SelectedLandmarkIndex, index) ;
            this->update() ;
            this->redraw() ;
            m_Action = NoAction;
            break;
          }
        case Modify:
          this->DeleteLandmark(m_SelectedLandmarkIndex) ;
          this->CreateLandmark(m_SelectedLandmarkIndex, index) ;
          this->update() ;
          this->redraw() ;
          m_Action = NoAction;
          break;
        }
      }
    break;

  default:
    break;
  }

  return 1 ;
}

/**
 *
 * Draw the 'index'th landmark with the value passed as second argument.
 *
 */

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawPoint(const IndexType& index, PointPixelType color)
{
  unsigned int    x;
  unsigned int    y;
  SizeType        size; 
  // IndexType       index;
  IndexType       modifiedIndex;
  
  size = cOverlayData->GetLargestPossibleRegion().GetSize();
  //  cOverlayData->TransformPhysicalPointToIndex(landmark.GetPosition(),index);
  OverlayPixelType o_color ;
  for ( unsigned int i = 0 ; i < 4 ; i++ )
    {
      o_color[i] = (unsigned char) color[i] ;
    }

  for( int i=-5; i<6; i++ )
    {
    x = index[0]+i;
    
    if( x < 0 ) 
      {
      x = 0;
      }
    
    if( x >= size[0] )
      {
      x = size[0];
      }

    modifiedIndex[0] = x;
    modifiedIndex[1] = index[1];
    modifiedIndex[2] = index[2];

    cOverlayData->SetPixel(modifiedIndex, o_color);
    }

  for( int i=-5; i<6; i++ )
    {

    y = index[1]+i;

    if( y < 0 ) 
      {
      y = 0;
      }

    if( y >= size[1] )
      {
      y = size[1];
      }

    modifiedIndex[0] = index[0];
    modifiedIndex[1] = y;
    modifiedIndex[2] = index[2];

    cOverlayData->SetPixel(modifiedIndex, o_color);
    }
}

template<class TImagePixel>
unsigned int 
LandmarkSliceViewer<TImagePixel>
::GetColorTableValue( PointType & landmark )
{
  return landmark;
}
