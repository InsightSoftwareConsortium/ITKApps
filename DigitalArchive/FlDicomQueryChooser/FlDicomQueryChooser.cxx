/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FlDicomQueryChooser.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "FlDicomQueryChooser.h"

#include <dbpriv.h>
#include <dcistrmz.h>    /* for dcmZlibExpectRFC1950Encoding */
#include <dctk.h>

#define MAXREMOTEDBTITLES 20

const char *remoteDBTitles[ MAXREMOTEDBTITLES ];
typedef struct 
{
   const char *findSyntax;
   const char *moveSyntax;
} QuerySyntax;

typedef struct 
{
   T_ASC_Association *assoc;
   T_ASC_PresentationContextID presId;
} MyCallbackInfo;

// global variables
E_TransferSyntax  opt_networkTransferSyntax = EXS_Unknown;
const char *      opt_moveDestination = NULL;
E_TransferSyntax  opt_out_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_in_networkTransferSyntax = EXS_Unknown;
static T_ASC_Network *net = NULL; /* the global DICOM network */
static DcmDataset *overrideKeys = NULL;
Fl_Window *form;

//extern E_TransferSyntax networkTransferSyntax = EXS_Unknown;


//static 
void FlDicomQueryChooser::CreateConfigEntries( Config *aConfig, int remoteDBTitlesCount, const char* peerName, char *port, const char *peertitletest )
{
  const char **ctnTitles = NULL;
  int ctnTitleCount = 0;
  int i =0;
  DBEntry *dbEntry = NULL;
  int portNumber = atoi(port) ;

  // add DB
  dbEntry = (DBEntry*) malloc( sizeof( DBEntry ) );
  bzero( (char*)dbEntry, sizeof(*dbEntry) );
  dbEntry->m_Title = remoteDBTitles[i];
  dbEntry->m_IsRemoteDB = OFTrue;

  aConfig->dbEntries = (DBEntry**) malloc( sizeof( DBEntry* ) );
  aConfig->dbEntries[0] = dbEntry;
 
  const char **peerTitles = NULL;
  int peerTitleCount = 1;
  // add peer title to database's peer title list
  if( dbEntry->peerTitles == NULL )
    dbEntry->peerTitles = (const char**) malloc( sizeof( const char* ) );
  else
    dbEntry->peerTitles = (const char**) realloc( dbEntry->peerTitles, (dbEntry->m_PeerTitleCount + 1) * sizeof(const char*) );
                                          
  dbEntry->peerTitles[ dbEntry->m_PeerTitleCount ] = peertitletest;
  dbEntry->m_PeerTitleCount++;
             
  // throw away the old list
  free( peerTitles );
}


//
// FlDicomQueryChooser ()
//  Default constructor.  Takes a pointer to FlDicomQueryChooser Prefs.
//
FlDicomQueryChooser::FlDicomQueryChooser()
{
  fl_filename_expand(m_PrefsFileName, DCMTKPREFSCFG);
  fl_filename_expand(m_SaveFileName, "");
  m_Prefs = new fldqcPreferences(m_PrefsFileName);
  qMod  = new RemoteNetworkQuery(m_Prefs);
  m_LocalQuery = new LocalDirectoryQuery();
  m_Output = NULL;  
  m_CalledAP  = NULL;
  m_CallingAP = NULL;
  m_ServerName = NULL;
  m_ServerPort = NULL;
  m_QueryLevel = NULL;
  m_PatientID = NULL;
  m_StudyUID = NULL;
  m_SeriesUID = NULL;
  m_SopUID = NULL;
  m_ClientPort = NULL; 
  m_ModelLevel = UNDEFINED; 
 
  m_Remotedb = OFFalse;

  #ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    // we need at least version 1.1
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup( winSockVersionNeeded, &winSockData );
  #endif

  #ifdef HAVE_GUSI_H
    // needed for Macintosh
    GUSISetup( GUSIwithSIOUXSockets );
    GUSISetup( GUSIwithInternetSockets );
  #endif
    
 this->SetOutput(outputBrowser);
}

/** Destructor */
FlDicomQueryChooser::~FlDicomQueryChooser ()
{
  if (m_Prefs->GetAutoSave())
    {
    m_Prefs->WriteToFile(m_PrefsFileName);
    }
  
  OFCondition cond = EC_Normal;
  delete m_Prefs;
  delete qMod;
  m_FilesInDirectory.clear();
  m_PatientFileNames.clear();
  m_StudyFileNames.clear();
  m_SeriesFileNames.clear();
  m_ImageFileNames.clear();
  m_PatientIDList.clear();
  m_StudyUIDList.clear();
  m_SeriesUIDList.clear();
  m_ImageUIDList.clear();
   
  DetatchAssociation(&m_Configuration, OFFalse);
  // clean up network
  cond = ASC_dropNetwork( &m_Configuration.net );
  if( cond.bad() )
    {
    std::cerr << " Error Dropping Network: " << std::endl;
    DimseCondition::dump( cond );
    }
}  


void FlDicomQueryChooser::AddOverrideKey( const char* s)
{
  unsigned int g = 0xffff;
  unsigned int e = 0xffff;
  int n = 0;
  char val[1024];
  OFString msg;
  char msg2[200];

  val[0] = '\0';
  n = sscanf(s, "%x,%x=%s", &g, &e, val);

  if (n < 2) 
  {
    msg = "bad key format: ";
    msg += s;
  }

  const char* spos = s;
  char ccc;
  do
  {
    ccc = *spos;
    if (ccc == '=') break;
    if (ccc == 0) { spos = NULL; break; }
    spos++;
  } 
  while(1);

  if (spos && *(spos+1)) 
  {
    strcpy(val, spos+1);
  }

  DcmTag tag(g,e);
  if (tag.error() != EC_Normal) 
  {
    sprintf(msg2, "unknown tag: (%04x,%04x)", g, e);
  }
  DcmElement *elem = newDicomElement(tag);
  if (elem == NULL) 
  {
    sprintf(msg2, "cannot create element for tag: (%04x,%04x)", g, e);
  }
  
  if (strlen(val) > 0) 
  {
    elem->putString(val);
    if (elem->error() != EC_Normal)
    {
      sprintf(msg2, "cannot put tag value: (%04x,%04x)=\"", g, e);
      msg = msg2;
      msg += val;
      msg += "\"";
    }
  }

  if (overrideKeys == NULL) overrideKeys = new DcmDataset;
  overrideKeys->insert(elem, OFTrue);
  if (overrideKeys->error() != EC_Normal) 
  {
    sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
  }
}


void  FlDicomQueryChooser::LoadConfigurationInfo( char * m_CallingAP, const char *m_ServerName, const char *m_CalledAP, char *m_ServerPort, const char* name, const char * id, Fl_Browser *patientBrowser )
{
  OFBool noCommandLineValueForMaxReceivePDULength = OFTrue;
  int returnValue = 0;
  int remoteDBTitlesCount = 0;
  OFCondition cond = EC_Normal;

  // initialize conf structure
  bzero((char*)&m_Configuration, sizeof(m_Configuration));
  m_Configuration.m_MyAETitle = m_CallingAP;
  remoteDBTitles[0] = m_CalledAP;
  remoteDBTitlesCount++;
  m_Configuration.m_MaxReceivePDULength = 16384;

  // set "peer to talk to" to the first host name in the array (this is the default)
  m_Configuration.m_PeerHostName = m_ServerName;
  m_Configuration.m_Peerport = atoi(m_ServerPort);
  // load up configuration info
  CreateConfigEntries( &m_Configuration, remoteDBTitlesCount,m_ServerName, m_ServerPort,m_CalledAP );

  // dump information
  int i,j;
  std::cout <<" Configuration:"<< std::endl;
  std::cout <<"My AE Title:"<< m_Configuration.m_MyAETitle << std::endl;
  std::cout <<"DatabaseTitles    Peer AE Titles" << std::endl;
  
  for( i=0 ; i< 1 ; i++ )
  {
    printf("%-18s", m_Configuration.dbEntries[i]->m_Title );
    for( j=0 ; j<m_Configuration.dbEntries[i]->m_PeerTitleCount ; j++ )
      std::cout << m_Configuration.dbEntries[i]->peerTitles[j] << std::endl;
  }

  // if starting up network is successful
  cond = ASC_initializeNetwork( NET_REQUESTOR, 0, 200, &m_Configuration.net);
  if( cond.good() )
  {
    // make the first database current 
    m_Configuration.m_Currentdb = 0;
    // make the first peer title for this database current 
    m_Configuration.m_CurrentPeerTitle = m_Configuration.dbEntries[m_Configuration.m_Currentdb]->peerTitles[0];
    WriteOutput (" Network Association Established "); 
    WriteOutput (" Querying Patients"); 
    qMod-> Patient(&m_Configuration,-1,patientBrowser, name,id);
  }
  else
  {
    std::cerr << " Error Initialising Network: "<< std::endl;
    DimseCondition::dump( cond );
    WriteOutput("Error Initialising Network ");
  }
}

//
// setOutput (_browser)
//  Sets up the output browser, used for display of transfer status, etc
//
void FlDicomQueryChooser::SetOutput(Fl_Browser *_output)
{
  m_Output = _output;
}

//
// writeOutput (_msg)
//  Add _msg to output browser (or stdout if it doesn't exist) and update
//

void FlDicomQueryChooser::WriteOutput (const char *_msg)
{
  if (m_Output) 
    {
    m_Output->add(_msg);
    m_Output->bottomline(m_Output->size());
    Fl::flush();
    }
  else
    {
    std::cout << "FlDicomQueryChooser : " << _msg << std::endl;
    }
}

// This function registers image files in the image database creating the index.dat file.

int FlDicomQueryChooser::DataBaseRegister( char * directory)
{
  dirent     **files;             // file list returned by filename_list
  int          numFiles;          // number of files in *files
  char         fullname[F_LEN];   // contains full path
  char         tmp[F_LEN];
  char         dir[F_LEN];
  DB_Handle *hdl = NULL;
  char sclass [120] ;
  char sinst  [120] ;
  #ifdef DEBUG
    char fname  [120] ;
  #endif
  DB_Status status;
  const char *opt_storageArea = directory;
  OFBool opt_debug = OFFalse;
  OFBool opt_verbose = OFFalse;
  OFBool opt_print = OFFalse;
  OFBool opt_isNewFlag = OFTrue;

  SetDebugLevel(( 0 ));

  // bad code follows -- check for . and .. and null filenames
  if (strlen(directory) == 0)
    return 1;
  //  end of bad code

  // Convert relative filename to absolute and clean it up a bit
  fl_filename_expand(tmp,directory);
  fl_filename_absolute(dir, tmp);
 
  // DBRegister requires the trailing backslash
  sprintf(tmp, "%s%c", dir, SLASH);
  
  // Get a list of files in the directory
  numFiles = fl_filename_list(tmp, &files);
  if (DB_createHandle(opt_storageArea, DB_UpperMaxStudies, DB_UpperMaxBytesPerStudy, &hdl).good())
  {
    for (int param = 0; param < numFiles; param++)
    {
      // First we're going to trash the . and .. entries
      if (files[param]->d_name[0] == '.')
        continue;
      sprintf(fullname, "%s%s", tmp, files[param]->d_name);
         
      if (access(fullname, R_OK) < 0)
        fprintf(stderr, "cannot access: %s\n", fullname);
      else
      {
        std::cout << "registering: " << fullname << std::endl;
        if (DU_findSOPClassAndInstanceInFile(fullname, sclass, sinst))
        {
          #ifdef DEBUG
          if (DB_getDebugLevel() > 0)
          {
            // Test what filename is recommended by DB_Module 
            DB_makeNewStoreFileName (hdl, sclass, sinst, fname) ;
            std::cout <<"DB_Module recommends " << fname << " for filename" << std::endl ;
          }
          #endif
          DB_storeRequest(hdl, sclass, sinst, fullname, &status, opt_isNewFlag) ;
        } 
        else
          std::cout << "Cannot Load Dicom File" << std::endl;
    }
  }

  DB_destroyHandle (&hdl);
  std::cout <<"-- DB Index File --\n" << std::endl;
  DB_PrintIndexFile((char *)opt_storageArea);
  // Finally, delete our file list
  delete [] files;

  return 0;
}
  
  DB_destroyHandle (&hdl);
  // Finally, delete our file list
  delete [] files;

  return 1;
}

void printDataset(DcmDataset *ds)
{
  ds->print(COUT);
}


// This function loads the Dicom Object into memory and then is written out at a user-specified Image Location and Inage Format
void FlDicomQueryChooser::SaveObject(OFList<OFString> *loadfilelist, char *Level, char *_filename)
{
  OFListIterator(OFString) iter = loadfilelist->begin();
  OFListIterator(OFString) last = loadfilelist->end();
  typedef itk::GDCMImageIO            ImageIOType;
 
  std::cout << "SaveObject " << Level << " : " << _filename << std::endl;
  if ((loadfilelist->size()) > 1) 
    {          
    // Software Guide : BeginCodeSnippet
    typedef itk::Image<short,3>            ImageType;
    typedef itk::ImageSeriesReader< ImageType >     ReaderType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    // Software Guide : EndCodeSnippet
    
    std::cout << "ReaderType" << std::endl;

    ReaderType::Pointer reader = ReaderType::New();
    typedef std::vector<std::string> fileNamesContainer;
    fileNamesContainer fileNames;  

     std::cout << "iterate over all input filenames " << std::endl;

    // iterate over all input filenames 
    while ((iter != last) )
      { 
      fileNames.push_back((*iter).c_str());
      ++iter;
      }
  
    std::cout << "reader" << std::endl;
    reader->SetFileNames( fileNames );
    reader->SetImageIO( dicomIO );
    try
      {
      reader->Update();
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << "Exception in GDCM Reader: " << ex << std::endl;
      return;
      }


    std::cout << "reader done" << std::endl;

    typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();

    std::cout  << "Writing the Dicom Object as " << _filename << std::endl;
    char outStr[255];
    sprintf(outStr, "Writing the Single Dicom Object as:%s ", _filename);
    WriteOutput(outStr);

    writer->SetFileName( _filename );
    writer->SetInput( reader->GetOutput() );
    try
      {
      writer->Update();
      WriteOutput("Dicom Object Saved Successfully");
      }
    catch (itk::ExceptionObject &ex)
      {
      std::cout << "Exception in Writer: " << ex << std::endl;
      std::cout << ex << std::endl;
      return ;
      }             
    }
  else 
    {
    typedef short InputPixelType;
    typedef itk::Image< InputPixelType, 3> InputImageType;
    typedef itk::ImageFileReader< InputImageType > ReaderType;
    ReaderType::Pointer readerfile = ReaderType::New();

    std::cout << "FILE = " << (*iter).c_str() << std::endl;
    readerfile->SetFileName( (*iter).c_str() );
    
        std::cout << "gdcmImageIO ImageIOType::New();" << std::endl;

    ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
            std::cout << "gdcmImageIO SetImageIO;" << std::endl;
    readerfile->SetImageIO( gdcmImageIO );
        std::cout << "Update;" << std::endl;
    try
      {
      readerfile->Update();
      }
    catch (itk::ExceptionObject & e)
      {
      std::cerr << "exception in file reader " << std::endl;
      std::cerr << e << std::endl;
      return;
      }

    std::cout << "Done Update;" << std::endl;


    typedef unsigned char WritePixelType;
    typedef itk::Image< WritePixelType, 3 > WriteImageType;
    typedef itk::RescaleIntensityImageFilter< InputImageType, WriteImageType > RescaleFilterType;
    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    rescaler->SetOutputMinimum(   0 );
    rescaler->SetOutputMaximum( 255 );
   
    typedef itk::ImageFileWriter< WriteImageType >  Writer2Type;
    Writer2Type::Pointer writer2 = Writer2Type::New();
    writer2->SetFileName( _filename );

    std::cout  << "Writing the  Single Dicom Object as " << _filename << std::endl; 
    char outStr[100];
    sprintf(outStr, "Writing the Single Dicom Object as:%s ", _filename);
    WriteOutput(outStr);  

    rescaler->SetInput( readerfile->GetOutput() );
    writer2->SetInput( rescaler->GetOutput() );
    try
      {
      writer2->Update();
      WriteOutput("Dicom Object Saved Successfully");
      }
    catch (itk::ExceptionObject & e)
      {
      std::cerr << "exception in file writer " << std::endl;
      std::cerr << e << std::endl;
      return ;
      }
    }
  return;
}

// This function loads the Dicom Object into memory to be used by an itkApplication

void FlDicomQueryChooser::LoadToITK(OFList<OFString> *loadfilelist, char *Level)
{
  OFListIterator(OFString) iter = loadfilelist->begin();
  OFListIterator(OFString) last = loadfilelist->end();
  typedef itk::GDCMImageIO            ImageIOType;
 
  if ((loadfilelist->size()) > 1) 
  {               
    // Software Guide : BeginCodeSnippet
    typedef itk::Image< short,3>            ImageType;
    typedef itk::ImageSeriesReader< ImageType >     ReaderType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    // Software Guide : EndCodeSnippet
    
    ReaderType::Pointer reader = ReaderType::New();
    typedef std::vector<std::string> fileNamesContainer;
    fileNamesContainer fileNames;  

    // iterate over all input filenames 
    while ((iter != last) )
    { 
      fileNames.push_back((*iter).c_str());
      ++iter;
    }
  
    reader->SetFileNames( fileNames );
    reader->SetImageIO( dicomIO );
    try
    {
      reader->Update();
      WriteOutput("Dicom Object Loaded into Memory");
      std::cout  << "Dicom Object Loaded into Memory " << std::endl; 
    }
    catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return ;
    }
              
  }
  else 
  {
    typedef short InputPixelType;
    typedef itk::Image< InputPixelType, 3> InputImageType;
    typedef itk::ImageFileReader< InputImageType > ReaderType;
    ReaderType::Pointer readerfile = ReaderType::New();
    readerfile->SetFileName( (*iter).c_str() );
    ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
    readerfile->SetImageIO( gdcmImageIO );
    try
    {
      readerfile->Update();
      WriteOutput("Dicom Object Loaded into Memory");
      std::cout  << "Dicom Object Loaded into Memory " << std::endl; 
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << "exception in file reader " << std::endl;
      std::cerr << e << std::endl;
      return;
    }

  }
   
  return;
}



static int dumpFile(ostream & out,const char *ifname,const OFBool isDataset,const E_TransferSyntax xfer,const size_t printFlags,
            const OFBool loadIntoMemory,const OFBool stopOnErrors,const OFBool writePixelData,const char *pixelDirectory)
{
  int result = 0;
  if ((ifname == NULL) || (strlen(ifname) == 0))
  {
    std::cerr << ": invalid filename: <empty string>" << std::endl;
    return 1;
  }

  DcmFileFormat dfile;
  DcmObject *dset = &dfile;
  if (isDataset) 
    dset = dfile.getDataset();

  OFCondition cond = dfile.loadFile(ifname, xfer, EGL_noChange, DCM_MaxReadLength, isDataset);
  if (! cond.good())
  {
    result = 1;
    if (stopOnErrors) 
      return result;
  }
  if (loadIntoMemory) 
    dfile.loadAllDataIntoMemory();
  if (writePixelData)
  {
    OFString str = ifname;
    OFString rname = pixelDirectory;
    if ((rname.length() > 0) && (rname[rname.length() - 1] != PATH_SEPARATOR))
      rname += PATH_SEPARATOR;
    size_t pos = str.find_last_of(PATH_SEPARATOR);
    if (pos == OFString_npos)
      rname += str;
    else
      rname += str.substr(pos + 1);
    size_t counter = 0;
    dset->print(out, printFlags, 0 /*level*/, rname.c_str(), &counter);
  } 
  else
    dset->print(out, printFlags);
  
  return result;
}

// Dumps the header contents of the Dicom Object to std::out and creates a filename.raw file of the pixel data.
// The .raw file is written back at the location of the original Dicom Object
int FlDicomQueryChooser::Dump(char *current)
{
  OFBool loadIntoMemory = OFTrue;
  size_t printFlags = DCMTypes::PF_shortenLongTagValues /*| DCMTypes::PF_showTreeStructure*/;
  OFBool printFilename = OFFalse;
  OFBool isDataset = OFFalse;
  OFBool writePixelData = OFTrue;
  E_TransferSyntax xfer = EXS_Unknown;
  OFBool stopOnErrors = OFTrue;
  const char *pixelDirectory = NULL;
  int errorCount = 0;

  errorCount += dumpFile(COUT, current, isDataset, xfer, printFlags, loadIntoMemory, stopOnErrors,writePixelData, pixelDirectory);

  return errorCount;
}


// Dicom Object is displayed using the ImageViewer
int FlDicomQueryChooser::Display(OFList<OFString> *loadfilelist )
{
  OFListIterator(OFString) iter = loadfilelist->begin();
  OFListIterator(OFString) last = loadfilelist->end();
  typedef itk::GDCMImageIO            ImageIOType;
  typedef itk::Image<float,3>            ImageType;
  typedef itk::ImageFileReader< ImageType> VolumeReaderType;
  ImageType::Pointer imP;

  if ((loadfilelist->size()) > 1) 
  {  
    typedef itk::ImageSeriesReader< ImageType >     ReaderType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    ReaderType::Pointer reader = ReaderType::New();
    typedef std::vector<std::string> fileNamesContainer;
    fileNamesContainer fileNames;

    // iterate over all input filenames 
    while ((iter != last) )
    { 
      fileNames.push_back((*iter).c_str());
      ++iter;
    }
    reader->SetFileNames( fileNames );
    reader->SetImageIO( dicomIO );
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return 0;
    }
     
    imP = reader->GetOutput();
    try
    {
      reader->Update();
    }
    catch( ... )
    { 
      std::cout << "Problems reading file format" << std::endl;
      return 0;
    }
  }
  else 
  {
    typedef itk::ImageFileReader< ImageType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( (*iter).c_str() );
    ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
    reader->SetImageIO( gdcmImageIO );
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << "exception in file reader " << std::endl;
      std::cerr << e << std::endl;
      return 0;
    }
    imP = reader->GetOutput();
    try
    {
      reader->Update();
    }
    catch( ... )
    { 
      std::cout << "Problems reading file format" << std::endl;
      return 0;
    }    
  }
    
  std::cout << "...Done Loading File" << std::endl;
  char mainName[255];
  sprintf(mainName, "metaView: Displaying selected Dicom Object");
  std::cout << std::endl;
  std::cout << "For directions on interacting with the window," << std::endl;
  std::cout << "   type 'h' within the window" << std::endl;
  
  form = make_window();
  tkMain->label(mainName);
  tkWin->SetInputImage(imP);
  tkWin->flipY(true);
  
  form->show();
  tkWin->show();
  tkWin->update();

  // force a first redraw
  Fl::check();
  tkWin->update();
  Fl::run();

  return 1;
}

static OFCondition addPresentationContext(T_ASC_Parameters *params,T_ASC_PresentationContextID pid,const char* abstractSyntax)
{
  // We prefer to use Explicitly encoded transfer syntaxes.If we are running on a Little Endian machine we prefer
  // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.Some SCP implementations will just select the first transfer
  // syntax they support (this is not part of the standard) so organise the proposed transfer syntaxes to take advantage of such 
  // behaviour.The presentation contexts proposed here are only used for C-FIND and C-MOVE, so there is no need to support compressed transmission.

  const char* transferSyntaxes[] = { NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  switch (opt_out_networkTransferSyntax) 
  {
    case EXS_LittleEndianImplicit:
      // we only support Little Endian Implicit 
      transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      // we prefer Little Endian Explicit
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_BigEndianExplicit:
      // we prefer Big Endian Explicit 
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    default:
      // We prefer explicit transfer syntaxes.If we are running on a Little Endian machine we prefer
      // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
      //
      if (gLocalByteOrder == EBO_LittleEndian)  // defined in dcxfer.h 
      {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      } 
      else 
      {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      }
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
  }

  return ASC_addPresentationContext(params, pid, abstractSyntax,transferSyntaxes, numTransferSyntaxes);
}



static OFCondition acceptSubAssoc(T_ASC_Network * aNet, T_ASC_Association ** assoc)
{
  const char* knownAbstractSyntaxes[] = 
  {
    UID_VerificationSOPClass
  };
    
  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
  int numTransferSyntaxes;

  OFCondition cond = ASC_receiveAssociation(aNet, assoc, ASC_DEFAULTMAXPDU);
  if (cond.good())
  {
    switch (opt_in_networkTransferSyntax)
    {
      case EXS_LittleEndianImplicit:
        // we only support Little Endian Implicit
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
      case EXS_LittleEndianExplicit:
        // we prefer Little Endian Explicit 
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
      case EXS_BigEndianExplicit:
        // we prefer Big Endian Explicit 
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
      case EXS_JPEGProcess14SV1TransferSyntax:
        // we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) 
        transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEGProcess1TransferSyntax:
        // we prefer JPEGBaseline (default lossy for 8 bit images) 
        transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEGProcess2_4TransferSyntax:
        // we prefer JPEGExtended (default lossy for 12 bit images) 
        transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_RLELossless:
        // we prefer RLE Lossless 
        transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      default:
        // We prefer explicit transfer syntaxes.If we are running on a Little Endian machine we prefer
        // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } 
        else 
        {
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
     }

     // accept the Verification SOP Class if presented 
     cond = ASC_acceptContextsWithPreferredTransferSyntaxes((*assoc)->params,knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),transferSyntaxes, numTransferSyntaxes);

     if (cond.good())
     {
       // the array of Storage SOP Class UIDs comes from dcuid.h 
       cond = ASC_acceptContextsWithPreferredTransferSyntaxes((*assoc)->params,dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,transferSyntaxes, numTransferSyntaxes);
     }
   }
   if (cond.good()) cond = ASC_acknowledgeAssociation(*assoc);
   if (cond.bad()) 
   {
     ASC_dropAssociation(*assoc);
     ASC_destroyAssociation(assoc);
   }
   return cond;
}


static OFCondition echoSCP(T_ASC_Association * assoc,T_DIMSE_Message * msg,T_ASC_PresentationContextID presID)
{
  std::cout << "Received " << std::endl;
  DIMSE_printCEchoRQ(stdout, &msg->msg.CEchoRQ);

  // the echo succeeded !! 
  OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
  if (cond.bad())
  {
    fprintf(stderr, "storescp: Echo SCP Failed:\n");
    DimseCondition::dump(cond);
  }
  return cond;
}


struct StoreCallbackData
{
  char* imageFileName;
  DcmFileFormat* dcmff;
  T_ASC_Association* assoc;
};

static void storeSCPCallback(
  void *callbackData,
  T_DIMSE_StoreProgress *progress,    // progress state 
  T_DIMSE_C_StoreRQ *req,             // original store request 
  char *imageFileName, DcmDataset **imageDataSet, // being received into  
  T_DIMSE_C_StoreRSP *rsp,            // final store response 
  DcmDataset **statusDetail)
{
  DIC_UI sopClass;
  DIC_UI sopInstance;
  OFBool opt_useMetaheader = OFTrue;
  OFBool  opt_bitPreserving = OFFalse;
  OFBool  opt_abortDuringStore = OFFalse;
  OFBool  opt_abortAfterStore = OFFalse;

  if ((opt_abortDuringStore && progress->state != DIMSE_StoreBegin) || (opt_abortAfterStore && progress->state == DIMSE_StoreEnd)) 
  {
    std::cout << "ABORT initiated (due to command line options)" << std::endl;
    ASC_abortAssociation(((StoreCallbackData*) callbackData)->assoc);
    rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
    return;
  }

  //    if (opt_verbose)
  switch (progress->state)
  {
    case DIMSE_StoreBegin:
      std::cout << "RECV:" << std::endl;
      break;
    case DIMSE_StoreEnd:
      std::cout << " " << std::endl;
      break;
    default:
      putchar('.');
      break;
  }
  fflush(stdout);
  
  if (progress->state == DIMSE_StoreEnd)
  {
    // no status detail 
    *statusDetail = NULL;    

    // could save the image somewhere else, put it in database, etc 
    // An appropriate status code is already set in the resp structure, it need not be success.
    // For example, if the caller has already detected an out of resources problem then the
    // status will reflect this.  The callback function is still called to allow cleanup.
    //
    // rsp->DimseStatus = STATUS_Success;
        
    if ((imageDataSet)&&(*imageDataSet)&&(!opt_bitPreserving))
    {
      StoreCallbackData *cbdata = (StoreCallbackData*) callbackData;
      // Change the FileName Here  
      const char* fileName = cbdata->imageFileName;
      E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
      E_TransferSyntax xfer = opt_writeTransferSyntax;
      if (xfer == EXS_Unknown) 
         xfer = (*imageDataSet)->getOriginalXfer();
      E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;

      OFCmdUnsignedInt  opt_itempad = 0;
      OFCmdUnsignedInt  opt_filepad = 0;
      E_EncodingType    opt_sequenceType = EET_ExplicitLength;
      E_PaddingEncoding opt_paddingType = EPD_withoutPadding;

      OFCondition cond = cbdata->dcmff->saveFile(fileName, xfer, opt_sequenceType, opt_groupLength,
      opt_paddingType, (Uint32)opt_filepad, (Uint32)opt_itempad, !opt_useMetaheader);
      if (cond.bad())
      {
        fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
      }

      // should really check the image to make sure it is consistent,
      // that its sopClass and sopInstance correspond with those in
      // the request.

      OFBool    opt_correctUIDPadding = OFFalse;

      if ((rsp->DimseStatus == STATUS_Success))
      {
        // which SOP class and SOP instance 
        if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
        {
          fprintf(stderr, "storescp: Bad image file: %s\n", imageFileName);
          rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
        }
        else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
        {
          rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
        else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
        {
          rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
      }
    }
  }
  return;
}

static OFCondition storeSCP( T_ASC_Association *assoc,T_DIMSE_Message *msg,T_ASC_PresentationContextID presID)
{
  OFCondition cond = EC_Normal;
  T_DIMSE_C_StoreRQ *req;
  char imageFileName[2048];
  OFBool   opt_useMetaheader = OFTrue;
  req = &msg->msg.CStoreRQ;

  sprintf(imageFileName, "%s.%s",dcmSOPClassUIDToModality(req->AffectedSOPClassUID),req->AffectedSOPInstanceUID);
  std::cout << "Received " << std::endl;
  DIMSE_printCStoreRQ(stdout, req);

  StoreCallbackData callbackData;
  callbackData.assoc = assoc;
  callbackData.imageFileName = imageFileName;
  DcmFileFormat dcmff;
  callbackData.dcmff = &dcmff;
  DcmDataset *dset = dcmff.getDataset();
  OFBool            opt_bitPreserving = OFFalse;
    
  if (opt_bitPreserving)
  {
    cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,NULL, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
  } 
  else 
  {
    cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, opt_useMetaheader,&dset, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
  }

  if (cond.bad())
  {
    fprintf(stderr, "storescp: Store SCP Failed:\n");
    DimseCondition::dump(cond);
    if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) 
      unlink(imageFileName);
  } 

  return cond;
}

static OFCondition subOpSCP(T_ASC_Association **subAssoc)
{
  T_DIMSE_Message     msg;
  T_ASC_PresentationContextID presID;
  // just in case 
  if (!ASC_dataWaiting(*subAssoc, 0)) 
    return DIMSE_NODATAAVAILABLE;

  OFCondition cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &presID,&msg, NULL);

  if (cond == EC_Normal) 
  {
    switch (msg.CommandField) 
    {
      case DIMSE_C_STORE_RQ:
        cond = storeSCP(*subAssoc, &msg, presID);
        break;
      case DIMSE_C_ECHO_RQ:
        cond = echoSCP(*subAssoc, &msg, presID);
        break;
      default:
        cond = DIMSE_BADCOMMANDTYPE;
        break;
    }
  }
  // clean up on association termination 
  if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    cond = ASC_acknowledgeRelease(*subAssoc);
    ASC_dropSCPAssociation(*subAssoc);
    ASC_destroyAssociation(subAssoc);
    return cond;
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
  }
  else if (cond != EC_Normal)
  {
    std::cerr << "DIMSE Failure (aborting sub-association):" << std::endl;
    DimseCondition::dump(cond);
    // some kind of error so abort the association 
    cond = ASC_abortAssociation(*subAssoc);
  }

  if (cond != EC_Normal)
  {
    ASC_dropAssociation(*subAssoc);
    ASC_destroyAssociation(subAssoc);
  }
  return cond;
}

static void subOpCallback(void * /*subOpCallbackData*/,T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{
  if (aNet == NULL) return;   // help no net ! 

  if (*subAssoc == NULL) 
  {
    // negotiate association 
    acceptSubAssoc(aNet, subAssoc);
  } 
  else 
  {
    // be a service class provider 
    subOpSCP(subAssoc);
  }
}


static void moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request,int responseCount, T_DIMSE_C_MoveRSP *response)
{
  OFCondition cond = EC_Normal;
  MyCallbackInfo *myCallbackData;
  myCallbackData = (MyCallbackInfo*)callbackData;

  std::cout << "Move Response: "<< responseCount << std::endl;
  DIMSE_printCMoveRSP(stdout, response);

  // should we send a cancel back ?? 
  OFCmdSignedInt    opt_cancelAfterNResponses = -1;
  if (opt_cancelAfterNResponses == responseCount) 
  {
    std::cout <<"Sending Cancel RQ, MsgId: " << request->MessageID <<" ,PresId: " << myCallbackData->presId << std::endl;
    cond = DIMSE_sendCancelRequest(myCallbackData->assoc,myCallbackData->presId, request->MessageID);
    if (cond != EC_Normal) 
    {
      std::cerr << "Cancel RQ Failed:" << std::endl;
      DimseCondition::dump(cond);
    }
  }
}


static void SubstituteOverrideKeys(DcmDataset *dset)
{
  if (overrideKeys == NULL) 
    return; // nothing to do 

  // copy the override keys 
  DcmDataset keys(*overrideKeys);

  // put the override keys into dset replacing existing tags 
  unsigned long elemCount = keys.card();
  for (unsigned long i=0; i<elemCount; i++) 
  {
    DcmElement *elem = keys.remove((unsigned long)0);
    dset->insert(elem, OFTrue);
  }
}


static  OFCondition moveSCU(T_ASC_Association * assoc)
{
  T_ASC_PresentationContextID presId;
  T_DIMSE_C_MoveRQ    req;
  T_DIMSE_C_MoveRSP   rsp;
  DIC_US              msgId = assoc->nextMsgID++;
  DcmDataset          *rspIds = NULL;
  const char          *sopClass;
  DcmDataset          *statusDetail = NULL;
  MyCallbackInfo      callbackData;
  static QuerySyntax querySyntax[1] = 
  {
    { 
      UID_FINDPatientRootQueryRetrieveInformationModel,
      UID_MOVEPatientRootQueryRetrieveInformationModel 
    }
  };
  std::cout <<"================================" << std::endl;
  std::cout <<"Sending query" << std::endl;
  
  DcmFileFormat dcmff;
  SubstituteOverrideKeys(dcmff.getDataset());
    
  sopClass = querySyntax[0].moveSyntax;

  // which presentation context should be used 
  presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
  if (presId == 0) 
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;

  //  if (opt_verbose) {
  //    printf("Move SCU RQ: MsgID %d\n", msgId);
  //    printf("Request:\n");
  //    dcmff.getDataset()->print(COUT);
  //  }

  callbackData.assoc = assoc;
  callbackData.presId = presId;
  req.MessageID = msgId;
  strcpy(req.AffectedSOPClassUID, sopClass);
  req.Priority = DIMSE_PRIORITY_MEDIUM;
  req.DataSetType = DIMSE_DATASET_PRESENT;
  if (opt_moveDestination == NULL) 
  {
    // set the destination to be me 
    ASC_getAPTitles(assoc->params, req.MoveDestination,NULL, NULL);
  } 
  else 
  {
    strcpy(req.MoveDestination, opt_moveDestination);
  } 

  OFCondition cond = DIMSE_moveUser(assoc, presId, &req, dcmff.getDataset(),moveCallback, &callbackData, DIMSE_BLOCKING, 0,
    net, subOpCallback, NULL,&rsp, &statusDetail, &rspIds);

  if (cond == EC_Normal) 
  {
    //   if (opt_verbose) {
    DIMSE_printCMoveRSP(stdout, &rsp);
    if (rspIds != NULL) 
    {
      std::cout <<"Response Identifiers:" << std::endl;
      rspIds->print(COUT);
    }
  } 
  else 
  {
    std::cerr << "Move Failed:" << std::endl;
    DimseCondition::dump(cond);
  }
  if (statusDetail != NULL) 
  {
    std::cout << "Status Detail:" <<std::endl;
    statusDetail->print(COUT);
    delete statusDetail;
  }

  if (rspIds != NULL) delete rspIds;

  return cond;
}

// Query/Retrieve Service Class User (C-MOVE operation)

int FlDicomQueryChooser::MoveObjectFromServer(char *m_CalledAP, char* m_CallingAP, char* m_ServerName, char *m_ServerPort,char *m_QueryLevel,const char *m_PatientID,const char *m_StudyUID,const char *m_SeriesUID,const char *m_SopUID, char *m_ClientPort )
{
  T_ASC_Parameters *params = NULL;
  OFCmdUnsignedInt  opt_retrievePort = atoi(m_ClientPort);

  OFCmdUnsignedInt opt_port = atoi (m_ServerPort);
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc = NULL;
  static QuerySyntax querySyntax[1] = 
  {
    { 
      UID_FINDPatientRootQueryRetrieveInformationModel,
      UID_MOVEPatientRootQueryRetrieveInformationModel 
    }
  };

  SetDebugLevel((0)); /* stop dcmdata debugging messages */
  OFBool            opt_useMetaheader = OFTrue;
  OFCmdUnsignedInt  opt_maxPDU = ASC_DEFAULTMAXPDU;

  OFBool  opt_verbose=OFTrue;

  AddOverrideKey(m_QueryLevel);
  AddOverrideKey(m_PatientID);
  AddOverrideKey(m_StudyUID);
  AddOverrideKey(m_SeriesUID);
  AddOverrideKey(m_SopUID);

  #ifdef HAVE_GETEUID
  // if retrieve port is privileged we must be as well 
  if (opt_retrievePort < 1024) 
  {
    if (geteuid() != 0) 
    {
      std::cerr << "cannot listen on port " << opt_retrievePort << ",insufficient privileges" << std::endl;
      return 0;
    }
  }
  #endif

  // network for move request and responses 
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, (int)opt_retrievePort,1000, &net);
  if (cond.bad())
  {
    std::cerr << "cannot create network:" << std::endl;
    DimseCondition::dump(cond);
    return 0;
  }

  #ifdef HAVE_GETUID
  // return to normal uid so that we can't do too much damage in case
  // things go very wrong.   Only does someting if the program is setuid
  // root, and run by another user.  Running as root user may be
  // potentially disasterous if this program screws up badly.
  //
  setuid(getuid());
  #endif

  // set up main association 
  cond = ASC_createAssociationParameters(&params, opt_maxPDU);
  if (cond.bad()) 
  {
    DimseCondition::dump(cond);
    return 0;
  }

  ASC_setAPTitles(params,m_CallingAP ,m_CalledAP , NULL);
  gethostname(localHost, sizeof(localHost) - 1);
  sprintf(peerHost, "%s:%d",m_ServerName , (int)opt_port);
  ASC_setPresentationAddresses(params, localHost, peerHost);

  // We also add a presentation context for the corresponding find sop class.
  
  cond = addPresentationContext(params, 1,querySyntax[0].findSyntax);
  cond = addPresentationContext(params, 3,querySyntax[0].moveSyntax);
  if (cond.bad()) 
  {
    DimseCondition::dump(cond);
    return 0;
  }
 
  OFBool  opt_debug = OFFalse;

  if (opt_debug) 
  {
    std::cout <<"Request Parameters:" << std::endl;
    ASC_dumpParameters(params, COUT);
  }

  // create association 
  if (opt_verbose)
    std::cout <<"Requesting Association" << std::endl;
  WriteOutput("Requesting Association");
  cond = ASC_requestAssociation(net, params, &assoc);
  if (cond.bad()) 
  {
    if (cond == DUL_ASSOCIATIONREJECTED) 
    {
      T_ASC_RejectParameters rej;
      ASC_getRejectParameters(params, &rej);
      std::cerr << "Association Rejected:" << std::endl;
      WriteOutput("Association Rejected:");
      ASC_printRejectParameters(stderr, &rej);
      return 0;
    } 
    else 
    {
      std::cerr << "Association Request Failed:" << std::endl;
      WriteOutput("Association Request Failed:");
      DimseCondition::dump(cond);
      return 0;
    }
  }

  // what has been accepted/refused 
  if (opt_debug) 
  {
    std::cout << "Association Parameters Negotiated:"<< std::endl;
    ASC_dumpParameters(params, COUT);
  }

  if (ASC_countAcceptedPresentationContexts(params) == 0) 
  {
    std::cerr << "No Acceptable Presentation Contexts" << std::endl;
    return 0;
  }

  if (opt_verbose) 
  {
    std::cout <<"Association Accepted (Max Send PDV): " << assoc->sendPDVLength << std::endl;
    WriteOutput("Association Accepted (Max Send PDV): ");
  }

  // do the real work 
  cond = EC_Normal;
  OFCmdUnsignedInt  opt_repeatCount = 1;
    
  int n = (int)opt_repeatCount;

  while (cond.good() && n--)
  {
    cond = moveSCU(assoc);
  }

  OFBool  opt_abortAssociation = OFFalse;

  // tear down association 
  if (cond == EC_Normal)
  {
    if (opt_abortAssociation) 
    {
      std::cout <<"Aborting Association" << std::endl;
      WriteOutput(" Aborting Association");
      cond = ASC_abortAssociation(assoc);
      if (cond.bad()) 
      {
        std::cerr << "Association Abort Failed:" << std::endl;
        WriteOutput(" Association Abort Failed:");
        DimseCondition::dump(cond);
        return 0;
      }
    } 
    else 
    {
      // release association 
      std::cout << "Releasing Association" << std::endl;
      WriteOutput(" Releasing Association ");
      cond = ASC_releaseAssociation(assoc);
      if (cond.bad())
      {
        std::cerr << "Association Release Failed:"<< std::endl;
        WriteOutput(" Association Release Failed");
        DimseCondition::dump(cond);
        return 0;
      }
    }
  }
  else 
  if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    std::cout << "Protocol Error: peer requested release (Aborting)" << std::endl;
    std::cout << "Aborting Association" << std::endl;
    cond = ASC_abortAssociation(assoc);
    if (cond.bad()) 
    {
      std::cerr << "Association Abort Failed:"<< std::endl;
      WriteOutput(" Association Abort Failed");
      DimseCondition::dump(cond);
      return 0;
    }
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
    if (opt_verbose) 
      std::cout <<"Peer Aborted Association" << std::endl;
  }
  else
  {
    std::cout << "SCU Failed:" << std::endl;
    DimseCondition::dump(cond);
    std::cout << "Aborting Association" << std::endl;
    WriteOutput("Aborting Association ");
    cond = ASC_abortAssociation(assoc);
    if (cond.bad()) 
    {
      std::cerr << "Association Abort Failed:"<< std::endl;
      WriteOutput("Association Abort Failed ");
      DimseCondition::dump(cond);
      return 0;
    }
  }

  cond = ASC_destroyAssociation(&assoc);
  if (cond.bad()) 
  {
    DimseCondition::dump(cond);
    return 0;
  }
  cond = ASC_dropNetwork(&net);
  if (cond.bad()) 
  {
    DimseCondition::dump(cond);
    return 0;
  }

  return 1 ;
}


/** Show the GUI */
void FlDicomQueryChooser::Show()
{
  // Display the window and call init()
  mainWindow->show();
  Init();
}

/** */
void FlDicomQueryChooser::NetworkQueryGo() 
{
  //clear all browsers and lists
  ClearAll();
  m_Remotedb = OFTrue;
  LoadConfigurationInfo(
  m_Prefs->GetCallingAP(),
  m_Prefs->GetServer(), 
  m_Prefs->GetCalledAP(), 
  m_Prefs->GetServerPort(),
  queryPatientNameInput->value(),
  queryPatientIDInput->value(), 
  patientBrowser);
}

/** Perform directory query */
void FlDicomQueryChooser::DirectoryQueryGo() 
{
  // Clear all browsers and lists
  ClearAll();

  char tmp[255];

  if (strcmp(directoryInput->value(),""))
    {
    sprintf(tmp, "Querying %s...", directoryInput->value());
    WriteOutput(tmp);
    m_Remotedb = OFFalse;

    // Make query
    m_LocalQuery->MakeDirectoryQuery (directoryInput->value(),&m_FilesInDirectory, patientBrowser, &m_PatientIDList);
    }
  else
    {
    WriteOutput("No Dicom Directory Selected");
    }
}

/** */
void FlDicomQueryChooser::Patient() 
{
  studyBrowser->clear();
seriesBrowser->clear();
imageBrowser->clear();
infoBrowser->clear();
m_ModelLevel = PATIENT;
infoBrowser->add("************************** Patient **************************");

if (m_Remotedb)
  { 
  for (int i = 1; i <=patientBrowser ->size(); i++) 
    {
      if (patientBrowser->selected(i)) 
      {
        int  arg = i-1;
        patient = NULL;
        patient = qMod-> Patient(&m_Configuration,arg);
        char outStr[100];
        sprintf(outStr, "PatientsName: %s",patient->m_PatientsName );
        infoBrowser->add(outStr);
        sprintf(outStr, "PatientsID: %s",patient->m_PatientID );
        infoBrowser->add(outStr);
        qMod-> Study(&m_Configuration,-1,studyBrowser);
      }
    }
  }
  else
  {     
    for (int i = 1; i <=patientBrowser ->size(); i++) 
    {
      if (patientBrowser->selected(i)) 
      {
         int tmp = i;
         OFListIterator(OFString) it = m_PatientIDList.begin();        
         while ((tmp > 1) )
   {  
           --tmp;  
           ++it;  
         } 
         const char* SelectedPatientID = ((*it).c_str());    
         m_LocalQuery->PatientFileQuery(SelectedPatientID,studyBrowser, &m_FilesInDirectory, &m_PatientFileNames,&m_StudyUIDList, infoBrowser, m_ModelLevel ); 
       }
     }   
  }
}

/** */
void FlDicomQueryChooser::Study() 
{
  infoBrowser->clear();
seriesBrowser->clear();
imageBrowser->clear();
m_ModelLevel = STUDY;
infoBrowser->add("************************** Study **************************");

if (m_Remotedb)
{

  for (int i = 1; i <=studyBrowser ->size(); i++) 
  {
    if (studyBrowser->selected(i)) 
    {
      int k =i-1;
      study = NULL;
      study = qMod-> Study(&m_Configuration,k);
      char outStr[100];
      sprintf(outStr, "PatientsName: %s",patient->m_PatientsName );
      infoBrowser->add(outStr);
      sprintf(outStr, "PatientsID: %s",patient->m_PatientID );
      infoBrowser->add(outStr);
      sprintf(outStr, "StudyID: %s",study->m_StudyID);
      infoBrowser->add(outStr);
      sprintf(outStr, "StudyInstanceUID: %s",study->m_StudyInstanceUID);
      infoBrowser->add(outStr);
      qMod-> Series(&m_Configuration,-1,seriesBrowser);
    }
  }
}
else 
{  
  for (int i = 1; i <=studyBrowser->size(); i++) 
  {
    if (studyBrowser->selected(i)) 
    {
      int tmp = i;
      OFListIterator(OFString) it = m_StudyUIDList.begin();
      while ((tmp > 1) )
      {  
        --tmp;  
        ++it;  
      } 
      const char* SelectedStudyUID = ((*it).c_str());
      m_LocalQuery->StudyFileQuery(SelectedStudyUID,seriesBrowser, &m_PatientFileNames, &m_StudyFileNames,&m_SeriesUIDList, infoBrowser, m_ModelLevel ); 
    }
  }
}
}

/** */
void FlDicomQueryChooser::Series() 
{
  infoBrowser->clear() ;
imageBrowser->clear();
m_ModelLevel = SERIES;
infoBrowser->add("************************** Series **************************");

if (m_Remotedb)
{
  for (int i = 1; i <=seriesBrowser ->size(); i++) 
  {
    if (seriesBrowser->selected(i)) 
    {
      int k =i-1;
      series = NULL;
      series = qMod-> Series(&m_Configuration,k);
      char outStr[100];
      sprintf(outStr, "PatientsName: %s",patient->m_PatientsName );
      infoBrowser->add(outStr);
      sprintf(outStr, "PatientsID: %s",patient->m_PatientID );
      infoBrowser->add(outStr);
      sprintf(outStr, "StudyID: %s",study->m_StudyID);
      infoBrowser->add(outStr);
      sprintf(outStr, "StudyInstanceUID: %s",study->m_StudyInstanceUID);
      infoBrowser->add(outStr);
      sprintf(outStr, "SeriesName: %s",series->m_SeriesNumber);
      infoBrowser->add(outStr);
      sprintf(outStr, "Modality: %s",series->m_Modality);
      infoBrowser->add(outStr);
      sprintf(outStr, "SeriesInstanceUID: %s",series->m_SeriesInstanceUID);
      infoBrowser->add(outStr);
      qMod-> Image(&m_Configuration,-1,imageBrowser);
    }
  }
}
else
{   
  for (int i = 1; i <=seriesBrowser ->size(); i++) 
  {
    if (seriesBrowser->selected(i)) 
    {
      int tmp = i;
      OFListIterator(OFString) it = m_SeriesUIDList.begin();
      while ((tmp > 1) )
      {   
        --tmp;  
        ++it;  
      } 
      const char* SelectedSeriesUID = ((*it).c_str());
      m_LocalQuery->SeriesFileQuery(SelectedSeriesUID,imageBrowser, &m_StudyFileNames, &m_SeriesFileNames,&m_ImageUIDList, infoBrowser, m_ModelLevel ); 
    }
  }
}
}

/** */
void FlDicomQueryChooser::Image() {
  infoBrowser->clear();
m_ModelLevel = IMAGE;
infoBrowser->add("************************** Image **************************");

if (m_Remotedb)
{
  for (int i = 1; i <=imageBrowser ->size(); i++) 
  {
    if (imageBrowser->selected(i))
    {
      int k =i-1;
      image = NULL;
      image = qMod-> Image(&m_Configuration,k);
      char outStr[100];
      sprintf(outStr, "PatientsName: %s",patient->m_PatientsName );
      infoBrowser->add(outStr);
      sprintf(outStr, "PatientsID: %s",patient->m_PatientID );
      infoBrowser->add(outStr);
      sprintf(outStr, "StudyID: %s",study->m_StudyID);
      infoBrowser->add(outStr);
      sprintf(outStr, "StudyInstanceUID: %s",study->m_StudyInstanceUID);
      infoBrowser->add(outStr);
      sprintf(outStr, "SeriesName: %s",series->m_SeriesNumber);
      infoBrowser->add(outStr);
      sprintf(outStr, "Modality: %s",series->m_Modality);
      infoBrowser->add(outStr);
      sprintf(outStr, "SeriesInstanceUID: %s",series->m_SeriesInstanceUID);
      infoBrowser->add(outStr);
      sprintf(outStr, "ImageNumber: %s",image->m_ImageNumber);
      infoBrowser->add(outStr);
      sprintf(outStr, "SOPInstanceUID: %s",image->m_SopInstanceUID);
      infoBrowser->add(outStr);
    }
  }
}
else
{ 
  for (int i = 1; i <=imageBrowser ->size(); i++) 
  {
    if (imageBrowser->selected(i)) 
    {
      int tmp = i;
      OFListIterator(OFString) it = m_ImageUIDList.begin();
      while ((tmp > 1) )
      {  
        --tmp;  
        ++it;  
      } 
      const char* SelectedImageUID = ((*it).c_str());            
      m_LocalQuery->ImageFileQuery(SelectedImageUID,&m_SeriesFileNames, &m_ImageFileNames, infoBrowser, m_ModelLevel ); 
    }
  }
}
}


void FlDicomQueryChooser::Move() {
  if (m_Remotedb)
{
  char QueryTag[50] = "0008,0052=";
  char *QueryLevel=NULL;
  string pidtag = "0010,0020=";
  string studyuidtag = "0020,000D=";
  string seriesuidtag = "0020,000E=";   
  string sopuidtag = "0008,0018=";

  for (int i = 1; i <=patientBrowser ->size(); i++) 
  {
    if (patientBrowser->selected(i))   
    {
      QueryLevel = "PATIENT";
      int  arg = i-1;
      patient = NULL; 
      patient = qMod-> Patient(&m_Configuration,arg);
      pidtag += patient->m_PatientID ; 
     
      for (int i = 1; i <=studyBrowser ->size(); i++) 
      {             
        if (studyBrowser->selected(i)) 
        {
          QueryLevel = "STUDY";  
          int k =i-1;
          study = NULL;
          study = qMod-> Study(&m_Configuration,k);
          studyuidtag += study->m_StudyInstanceUID ;
            
          for (int i = 1; i <=seriesBrowser ->size(); i++) 
          {
            if (seriesBrowser->selected(i))  
            {
              QueryLevel = "SERIES";
              int k =i-1;
              series = NULL;
              series = qMod-> Series(&m_Configuration,k);
              seriesuidtag += series->m_SeriesInstanceUID;

              for (int i = 1; i <=imageBrowser ->size(); i++) 
              {
                if (imageBrowser->selected(i))    
                {
                  QueryLevel = "IMAGE";
                  int k =i-1;
                  image = NULL;
                  image = qMod-> Image(&m_Configuration,k);
                  sopuidtag   += image->m_SopInstanceUID;
                }   
              }
            } 
          }          
        }
      }          
    }        
  }

  
  if (QueryLevel == NULL)
    {
    WriteOutput("No Dicom Object Selected");
    }
  else
    {
    strcat (QueryTag,QueryLevel);
    MoveObjectFromServer(
    m_Prefs->GetCalledAP(),
    m_Prefs->GetCallingAP(),
    m_Prefs->GetServer(),
    m_Prefs->GetServerPort(),
    QueryTag,pidtag.c_str(),
    studyuidtag.c_str(),
    seriesuidtag.c_str() ,
    sopuidtag.c_str(), 
    m_Prefs->GetClientPort());
    }
  }
}

void FlDicomQueryChooser::Load() 
{
  if (!m_Remotedb)
{
  OFList<OFString> *temporaryfilelist;
  temporaryfilelist = &m_FilesInDirectory; // Default load all files into memory
  char *level = NULL;

  for (int i = 1; i <=patientBrowser ->size(); i++) 
  {
    if (patientBrowser->selected(i))   
    {
      temporaryfilelist = &m_PatientFileNames; 
      level = "Patient";     
      
      for (int i = 1; i <=studyBrowser ->size(); i++) 
      {             
        if (studyBrowser->selected(i)) 
        {
          temporaryfilelist = &m_StudyFileNames;
          level = "Study";                
           
          for (int i = 1; i <=seriesBrowser ->size(); i++) 
          {
            if (seriesBrowser->selected(i))  
            {
              temporaryfilelist = &m_SeriesFileNames;
              level = "Series"; 
                  
              for (int i = 1; i <=imageBrowser ->size(); i++) 
              {
                if (imageBrowser->selected(i))    
                {
                  temporaryfilelist = &m_ImageFileNames;
                  level = "Image"; 
                }
              }
            } 
          }
        }
      }     
    }        
  }
   
  if (level == NULL)
    {
    WriteOutput("No Dicom Object Selected");
    }
  else
    {   
    char outStr[100];
    sprintf(outStr, "Loading Dicom Objects at: %s Level",level);
    WriteOutput(outStr);
    LoadToITK(temporaryfilelist,level);
    }   
}
}

/** Save the dicom file as an ITK supported image */
void FlDicomQueryChooser::Save() 
{
  char *file = fl_file_chooser("Save Dicom Object as...","*",m_SaveFileName );
  if (file)
    {
    strcpy(m_SaveFileName, file);
    if (!m_Remotedb)
      {
      OFList<OFString> *temporaryfilelist;
      temporaryfilelist = &m_FilesInDirectory; // Default load all files into memory
      char *level = NULL;

      for (int i = 1; i <= patientBrowser ->size(); i++) 
        {
        if (patientBrowser->selected(i))   
          {
          temporaryfilelist = &m_PatientFileNames; 
          level = "Patient";     
      
          for (int i = 1; i <=studyBrowser ->size(); i++) 
            {             
            if (studyBrowser->selected(i)) 
              {
              temporaryfilelist = &m_StudyFileNames;
              level = "Study";                
           
              for (int i = 1; i <=seriesBrowser ->size(); i++) 
                {
                if (seriesBrowser->selected(i))  
                  {
                  temporaryfilelist = &m_SeriesFileNames;
                  level = "Series"; 
                  
                  for (int i = 1; i <=imageBrowser ->size(); i++) 
                    {
                    if (imageBrowser->selected(i))    
                      {
                      temporaryfilelist = &m_ImageFileNames;
                      level = "Image"; 
                      }
                    }
                  } 
                }
              }
            }     
          }        
        }
        
    if (level == NULL)
      {
      WriteOutput("No Dicom Object Selected");
      }
    else
      {    
      char outStr[100];
      sprintf(outStr, "Loading Dicom Objects at: %s Level",level);
      WriteOutput(outStr);
      this->SaveObject(temporaryfilelist,level,m_SaveFileName);
      }  
    }
  }
}

/** Dump the file */
void FlDicomQueryChooser::DumpFile() 
{
  char * dumpfilename;
  dumpfilename = fl_file_chooser("Please Select the Dicom Object whose contents are to be dumped ","*",0);

  if ( dumpfilename)
    {
    Dump (dumpfilename); 
    }
  else
    {
    WriteOutput("No File is selected");
    }
}

/** Load the image to the ITK datastructure */
FlDicomQueryChooser::ImagePointer

FlDicomQueryChooser::LoadImage()
{
  this->Show();

  ImageType::Pointer image = ImageType::New();

  Fl::run();

  std::cout << "File Loaded!" << std::endl;
  return image;
}
