/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    UserInterfaceLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __UserInterfaceLogic_h_
#define __UserInterfaceLogic_h_

#include "UserInterface.h"

// Necessary forward references
class IRISApplication;
class GlobalState;
class IntensityCurveUILogic;
template<class TPixel> class ImageIOWizardLogic;
class PreprocessingUILogic;

// ITK forward references
namespace itk {
  template <class TObject> class SimpleMemberCommand;
};


/**
 * \class UserInterfaceLogic
 * \brief Logic for the main user interface.
 *
 * This class implements virtual function from
 * the GUI class that is generated by fluid.
 * The idea is to have virtual functions for
 * all the GUI callbacks, and then override
 * them in this class, allowing us to avoid
 * coding inside fluid, since it's annoying.
 *
 * Note that the GUI class contains all the
 * windows (the IRIS window, the 3D Snake window,
 * and all the dialogs).
 */
class UserInterfaceLogic : public UserInterface {
public:

  /* 
   * DESCRIPTION:
   * constructor.  calls GUI constructor. calls init
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - IRIS window is ready to be shown
   */
  UserInterfaceLogic(IRISApplication *iris);
  virtual ~UserInterfaceLogic();

  /**
   * DESCRIPTION
   * hides the snake window and shows the iris window
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   * iris window is shown
   *
   * RETURNS
   */
  void ShowIRIS();

  /**
   * DESCRIPTION
   * Resets the region of interest to contain the entire volume (full_data)
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   * global state region of interest is set to entire full_data volume
   *
   * RETURNS
   */
  void OnResetROIAction();

  /**
   * DESCRIPTION
   * Callback for the checkbox that shows or hides the region of interest in
   * the 2D windows
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   * 2D windows are redrawn, showing the region of interest if box is checked
   *
   * RETURNS
   */
  void OnShowROISelect();

  /**
   *
   * DESCRIPTION:
   * callback for the snake button
   * this function takes care of initializing
   * the global variable roi_data, sets vox_data
   * to point to roi_data, sets up the snake window
   * and calls ShowSNAP()
   *
   * PRECONDITIONS:
   * - global_state must have a valid ROI
   *
   * POSTCONDITIONS:
   * - snake window is displayed, IRIS is hidden
   */
  void OnSnakeStartAction();

  /**
   *
   * DESCRIPTION:
   * Callback for showing the dialog box for preprocessing the grey data.
   * Depending on the type of snake chosen either the dialog for thresholding
   * or edge detection is displayed.
   *
   * PRECONDITIONS:
   * - main snake window is initialized and active
   *
   * POSTCONDITIONS:
   * - preprocessing dialog appears on screen
   */
  void OnPreprocessAction();
  
  /** Callback for when the preprocessing completes */
  void OnPreprocessClose();

  /**
   *
   * DESCRIPTION:
   * callback to set bubbles for snake initialization
   * - sets a bubble at the current crosshairs position with the radius
   * equal to the current value on the  radius slider
   * - adds the bubble to the Active Bubbles browser
   *
   * PRECONDITIONS:
   * - Snake GUI is active
   * - Crosshairs position is within allowed boundaries
   *
   * POSTCONDITIONS:
   * - all previously set bubbles as well as the new bubble are displayed
   * in 2d windows
   * - Active Bubbles browser contains all previously set bubbles as
   * well as the new bubble and their parameters are displayed in the browser
   */
  void OnAddBubbleAction();

  /**
   *
   * DESCRIPTION:
   * callback to remove a bubble
   * - removes a highlighted bubble from Active Bubbles browser
   *
   * PRECONDITIONS:
   * - Snake GUI is active
   * - Active Bubbles browser contains at least one bubble
   *
   * POSTCONDITIONS:
   * - highlighted bubble has been removed from Active Bubbles browser
   * and is no longer displayed in 2d windows
   * - Active Bubbles browser contains all remaining bubbles and their
   * parameters are displayed in the browser
   * - all remaining bubbles are displayed in 2d windows
   *
   */
  void OnRemoveBubbleAction();

  /**
   *
   * DESCRIPTION:
   * callback to browse through the bubbles in the Active Bubbles browser
   * - highlights a line clicked in the browser window and sets the radius
   * slider to the value of the highlighted bubble's radius
   *
   * PRECONDITIONS:
   * - Snake GUI is active
   * - Active Bubbles browser contains at least one bubble
   *
   * POSTCONDITIONS:
   * - the line clicked in the browser window is highlighted
   * - value of m_HighlightedBubble integer variable is set to the
   * number of the line highlighted in the browser window
   * - radius slider value is reset to the value of the highlighted
   * bubble's radius
   */
  void OnActiveBubblesChange();

  /**
   *
   * DESCRIPTION:
   * callback to set the radius of a bubble highlighted in the
   * Active Bubbles browser
   * - sets the radius of the highlighted bubble to the current value of
   * the radius slider
   * - only displays the current value of the radius slider if no bubble is
   * highlighted or if the Active Bubbles browser is empty
   *
   * PRECONDITIONS:
   * - Snake GUI is active
   *
   * POSTCONDITIONS:
   * - current value of the radius slider is displayed
   * - radius of the highlighted bubble is set to the current value of
   * the radius slider
   * - visualisation of the highlighted bubble in 2d windows changes
   * its radius correspondingly
   * - all remaining bubbles are left unchanged
   */
  void OnBubbleRadiusChange();

  /**
   *
   * DESCRIPTION:
   * callback to set snake parameters
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - if the snake exists, it's parameters have been set
   *
   */
  void OnSnakeParametersApplyAction();

  /**
   * DESCRIPTION
   * Opens the snake params dialog, allowing the user to modify snake params
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  void OnSnakeParametersAction();

  /**
   * DESCRIPTION
   * Sets the snake params dialog to Schlegel values
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  void OnSnakeParametersSchlegelSelect();

  /**
   * DESCRIPTION
   * Sets the snake params dialog to Sapiro values
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  void OnSnakeParametersSapiroSelect();

  /**
   * DESCRIPTION
   * Sets the snake params dialog to Turello values
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  void OnSnakeParametersTurelloSelect();

  /**
   * DESCRIPTION
   * Allows user to modify snake params
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  void OnSnakeParametersUserDefinedSelect();

  /**
   * DESCRIPTION
   * Activates the ground slider based on the clamp toggle in the snake params
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  void OnSnakeParametersClampChange();

  /**
   *
   * DESCRIPTION:
   * callback for accepting a snake segmentation
   *
   * PRECONDITIONS:
   * - roi_data is valid
   * - roi_data.snakeImageData is initialized
   *
   * POSTCONDITIONS:
   * - roi_data.snakeImageData has been merged with full_data.segImageData
   * - roi_data is deleted
   * - IRIS window is shown
   *
   */
  void OnAcceptSegmentationAction();

  /**
   *
   * DESCRIPTION:
   * callback for canceling a snake segmentation
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - snake window is hidden, IRIS is shown
   * - roi_data is deleted
   *
   */
  void OnCancelSegmentationAction();

  /**
   *
   * DESCRIPTION:
   * sync the scrollbars with the current cursor position.
   * snake window version of ResetScrollbars
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void ResetSNAPScrollbars();

  /**
   *
   * DESCRIPTION:
   * snake window version of UpdatePositionDisplay
   * set the textbox display of slice number for 2d window id
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void UpdateSNAPPositionDisplay(int id);

  /**
   *
   * DESCRIPTION:
   * snake window version of UpdateImageProbe
   * update the intensity and label outputs to reflect values at
   * the current cursor position
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void UpdateSNAPImageProbe();

  /**
   *
   * DESCRIPTION:
   * disables or enables the UpdateMesh button in snake win depending
   * on the state of the checkbox
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void OnContinuousViewUpdateChange();

  /**
   *
   * DESCRIPTION:
   * callback for moving the 2d window sliders
   * snake window version of PositionSliderCallback
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void OnIRISSliceSliderChange(int id);

  /**
   *
   * DESCRIPTION:
   * Switches to InOut snake mode
   *
   * PRECONDITIONS:
   * - Snake UI is active
   *
   * POSTCONDITIONS:
   * - all current preprocessed data is invalidated
   */
  void OnInOutSnakeSelect();

  /**
   *
   * DESCRIPTION:
   * Switches to Edge snake mode
   *
   * PRECONDITIONS:
   * - Snake UI is active
   *
   * POSTCONDITIONS:
   * - all current preprocessed data is invalidated
   */
  void OnEdgeSnakeSelect();

  /**
   *
   * DESCRIPTION:
   * callback for accepting the snake initialization.  The active
   * bubbles are voxelized.  roi_data.segImageData and snakeInitImageData
   * are initialized and synchronized.  The bubble voxels are put into
   * snakeInitImageData. If the grey data has been preprocessed, and
   * some type of initialization is present in snakeInitImageData (either
   * a bubble or pre-existing segmentation data from segImageData), the
   * snake can now be run.
   *
   * PRECONDITIONS:
   * - snake window is up and running
   *
   * POSTCONDITIONS:
   * - global_state.GetSnakeActive is true
   *
   */
  void OnAcceptInitializationAction();

  /**
   *
   * DESCRIPTION:
   * callback for restarting the snake initialization.  Any current snake
   * state is forgotten, the snake controls are disabled, the segmentation
   * shown in the 2D windows reverts back to the segImageData, and the bubbles
   * are shown again.  The initialization widgets (bubbles, preproc) are enabled
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - global_state.GetSnakeActive is false
   *
   */
  void OnRestartInitializationAction();

  /**
   * User wants to go back to the preprocessing page from the segmentation 
   * initialization page.  This callback will simply flip the page and clear
   * the bubbles
   */
  void OnRestartPreprocessingAction();

  /** The user has finished preprocessing the image and flips to the next 
   * page in the wizard.  The speed image must be valid for this button to be
   * active */
  void OnAcceptPreprocessingAction();

  /**
   *
   * DESCRIPTION:
   * callback for the snake "vcr" rewind button.  Restarts the snake
   * at the same state as when AcceptInitialization button was pressed
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void OnSnakeRewindAction();

  /**
   *
   * DESCRIPTION:
   * simply sets a flag so that the play button knows to stop
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void OnSnakeStopAction();

  /**
   *
   * DESCRIPTION:
   * continuously steps the snake until m_SnakeIsRunning flag is unset
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void OnSnakePlayAction();

  /**
   *
   * DESCRIPTION:
   * runs the snake for one step.
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - 2D displays are updated according to the current snake state
   * - if updatecontinuously checkbox is checked, 3D display is updated
   *
   */
  void OnSnakeStepAction();

  /**
   *
   * DESCRIPTION:
   * callback for choice box of how many snake iterations to run per "step"
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * m_SnakeStepSize is set
   *
   */
  void OnSnakeStepSizeChange();

  /**
   *
   * DESCRIPTION:
   * returns a pointer to an array of Bubble objects contained in
   * the Active Bubbles browser
   *
   * PRECONDITIONS:
   * - Snake GUI is active
   *
   * POSTCONDITIONS:
   * - a pointer to an array of Bubble objects contained in
   * the Active Bubbles browser is returned
   * - returns NULL if Active Bubbles browser is empty
   */
  Bubble* getBubbles();

  /**
   *
   * DESCRIPTION:
   * returns the number of bubbles contained in the Active Bubbles
   * browser
   *
   * PRECONDITIONS:
   * - Snake GUI is active
   *
   * POSTCONDITIONS:
   * - the number of bubbles in the Active Bubbles browser is returned
   * - returns zero if the browser is empty
   */
  int getNumberOfBubbles();

  /**
   * Pops up a dialog to choose a preprocessed data file
   * pre: LoadPreproc_win is active
   * post: header information is filled in in the dialog
   * void LoadPreprocSelectCallback();

   * Fills the header information in the Load Preprocessed data dialog
   * pre: the filename field contains a valid filename
   * post: header information is filled in
   * void SelectPreprocFileCallback();

   * Implements action performed when user presses OK
   * in the Load Preprocessed data dialog
   * PRE: nothing
   * POST: the preprocessed in roi_data is valid
   * AUTHORS: Talbert and Scheuermann
   * void LoadPreprocessedDataCallback();

   * Implements callback for Save->Preprocessed
   * menu item
   * PRE: nothing
   * POST: save dialog appears
   * void SavePreprocessedData_Callback();

   * Implements callback for Okay button in the
   * Save Preprocessed dialog box
   * PRE: nothing
   * POST: If a valid filename was supplied then
   * the preprocessed data set is saved to the
   * specified file
   * void OkaySavePreproc_button_Callback();

   * callback of the radio button for selecting the grey data
   * for display
   */
  void OnSNAPViewOriginalSelect();

  /**
   * callback of the radio button for selecting the preprocessed data
   * for display
   */
  void OnViewPreprocessedSelect();

  /**
   *
   * DESCRIPTION:
   * close both windows so FL::run returns, and program exits
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - program will exit
   *
   */
  void shutdown();

  /**
    *
    * DESCRIPTION:
    * this function fiddles with the ROI points
    * so that pt1 has the smaller coordinates (upper left),
    * and pt2 has the larger coordinates (lower right). Some
    * methods depend on this being true for simplicity.
    *
    * PRECONDITIONS:
    *
    * POSTCONDITIONS:
    *
    */
  void TweakROI(Vector3i &pt1, Vector3i &pt2);

  void InitColorMap();
  void RedrawWindows();
  void ResetScrollbars();
  void MakeSegTexturesCurrent();
  void UpdateColorChips();
  void UpdateImageProbe();
  void UpdateMainLabel(char *greyImg, char* segImg);
  void ActivatePaste(int id, bool on);
  void ActivatePaste(bool on);
  void ActivateAccept(int id, bool on);
  void ActivateAccept(bool on);
  void Activate3DAccept(bool on);
  void UpdateEditLabelWindow();
  void UpdatePositionDisplay(int id);
  int CheckOrient(const char *txt, Vector3i &RAI);

  /** Set the current interaction mode */
  void SetToolbarMode(ToolbarModeType mode);

  /** Get the pointer to the driving application */
  irisGetMacro(Driver,IRISApplication *);

  /** TODO: Move this to a UserInterfaceDriver class or something */
  void DoLoadImage(GreyImageWrapper *source);

protected:

  /**
   *
   * DESCRIPTION:
   * Initializes everything so the IRIS window can be shown.
   * This method is essentially the same as IRIS2000, but
   * we added a couple initializations of snake window stuff.
   *
   * PRECONDITIONS:
   * - basically assumes it's going to be called in the constructor.
   *
   * POSTCONDITIONS:
   * - the IRIS window can now be shown
   */
  void init();

  /**
   * UserInterfaceLogic::ShowSNAP
   *
   * DESCRIPTION:
   *   hides the iris window and shows the snake window
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   * - the IRIS window is hidden and the snake window is shown
   *
   */
  void ShowSNAP();

  /**
   * DESCRIPTION
   * Runs the snake, and updates the GUI
   *
   * PARAMETERS
   * numsteps   = input, number of iterations to run the snake
   *
   * PRE
   * snake must be ready to run
   *
   * POST
   *
   * RETURNS
   * 1 if running is successful, 0 if an error occurred
   */
  int RunSnake(int numsteps);

  void LoadLabelsCallback();
  void ChangeLabelsCallback();
  void PositionSliderCallback(int id);
  void AcceptPolygonCallback(int id);
  void SaveLabelsCallback();
  void PrintVoxelCountsCallback();

  // Menu item callbacks
  void OnMenuLoadGrey();
  void OnMenuLoadSegmentation();
  void OnMenuLoadLabels();
  void OnMenuSaveSegmentation();
  void OnMenuSaveLabels();
  void OnMenuIntensityCurve();
  void OnMenuLoadPreprocessed();  
  void OnMenuSavePreprocessed();  

  // Rendering options callbacks
  void OnRenderOptionsChange();
  void OnRenderOptionsCancel();
  void OnRenderOptionsOk();
  void OnSNAPLabelOpacityChange();
  void OnIRISLabelOpacityChange();

  // SNAP pipeline step buttons
  void OnSNAPStepPreprocess();
  void OnSNAPStepInitialize();
  void OnSNAPStepSegment();

  // Help system callbacks
  void OnLaunchTutorialAction();

  // Update rendering options widgets from the Global state
  void UpdateRenderOptionsUI();

  // Set the active page in the segmentation pipeline
  void SetActiveSegmentationPipelinePage(unsigned int page);
  
  char *m_ChosedFile;

private:

  // Typedef for event callback commands
  typedef itk::SimpleMemberCommand<UserInterfaceLogic> SimpleCommandType;

  // Pointer to the driving IRIS application object
  IRISApplication *m_Driver;

  // Bubble highlighted in the browser
  int m_HighlightedBubble;

  // current iteration of the snake
  int m_SnakeIteration;

  // how many snake iterations per step
  int m_SnakeStepSize;

  // flag for the play button to know when to stop
  int m_SnakeIsRunning;

  /**
   * UI object responsible for handling the File IO dialogs
   */
  ImageIOWizardLogic<GreyType> *m_WizGreyIO;

  /**
   * UI object used for handling Curve editing
   */
  IntensityCurveUILogic *m_WinIntensityCurve;

  /** 
   * Preprocessing UI object
   */
  PreprocessingUILogic *m_PreprocessingUI;

  // Intensity curve update callback (uses ITK event system)
  void OnIntensityCurveUpdate();

  /**
   * DESCRIPTION
   * Not used.  It was an attempt to solve the problem of SnAP not exiting
   * if the user clicks the X to quit rather than the quit menu option, when
   * one of the "non-modal" dialogs is open.  It didn't work, but I left it
   * anyway
   *
   * PARAMETERS
   *
   * PRE
   *
   * POST
   *
   * RETURNS
   */
  static void CloseCallback(Fl_Window*,void*);

  /**
   *
   * DESCRIPTION:
   * updates snake iteration output field
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void UpdateIterationOutput();

  /**
   *
   * DESCRIPTION:
   * some GUI state is shared between the windows,
   * like contrast/brightness settings, toolbar mode.
   * these functions sync the two
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void SyncIRISToSnake();

  /**
   *
   * DESCRIPTION:
   * some GUI state is shared between the windows,
   * like contrast/brightness settings, toolbar mode.
   * these functions sync the two
   *
   * PRECONDITIONS:
   *
   * POSTCONDITIONS:
   *
   */
  void SyncSnakeToIRIS();

  /** Common code for cancelling and accepting the segmentation */
  void CloseSegmentationCommon();
};

#endif

/*
 *Log: UserInterfaceLogic.h
 *Revision 1.2  2003/08/27 04:57:47  pauly
 *FIX: A large number of bugs has been fixed for 1.4 release
 *
 *Revision 1.1  2003/07/12 04:46:50  pauly
 *Initial checkin of the SNAP application into the InsightApplications tree.
 *
 *Revision 1.17  2003/07/12 01:34:18  pauly
 *More final changes before ITK checkin
 *
 *Revision 1.16  2003/07/11 23:28:10  pauly
 **** empty log message ***
 *
 *Revision 1.15  2003/07/10 14:30:26  pauly
 *Integrated ITK into SNAP level set segmentation
 *
 *Revision 1.14  2003/06/23 23:59:32  pauly
 *Command line argument parsing
 *
 *Revision 1.13  2003/06/14 22:42:06  pauly
 *Several changes.  Started working on implementing the level set function
 *in ITK.
 *
 *Revision 1.12  2003/06/08 23:27:56  pauly
 *Changed variable names using combination of ctags, egrep, and perl.
 *
 *Revision 1.11  2003/06/08 16:11:42  pauly
 *User interface changes
 *Automatic mesh updating in SNAP mode
 *
 *Revision 1.10  2003/05/22 17:36:19  pauly
 *Edge preprocessing settings
 *
 *Revision 1.9  2003/05/17 21:39:30  pauly
 *Auto-update for in/out preprocessing
 *
 *Revision 1.8  2003/05/14 18:33:58  pauly
 *SNAP Component is working. Double thresholds have been enabled.  Many other changes.
 *
 *Revision 1.7  2003/05/07 19:14:46  pauly
 *More progress on getting old segmentation working in the new SNAP.  Almost there, region of interest and bubbles are working.
 *
 *Revision 1.6  2003/05/05 12:30:18  pauly
 **** empty log message ***
 *
 *Revision 1.5  2003/04/25 02:58:29  pauly
 *New window2d model with InteractionModes
 *
 *Revision 1.4  2003/04/23 06:05:18  pauly
 **** empty log message ***
 *
 *Revision 1.3  2003/04/18 17:32:18  pauly
 **** empty log message ***
 *
 *Revision 1.2  2003/04/18 00:25:37  pauly
 **** empty log message ***
 *
 *Revision 1.1  2003/04/16 05:04:17  pauly
 *Incorporated intensity modification into the snap pipeline
 *New IRISApplication
 *Random goodies
 *
 *Revision 1.1  2003/03/07 19:29:47  pauly
 *Initial checkin
 *
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.33  2002/04/28 20:12:40  scheuerm
 *tiny documentation changes
 *
 *Revision 1.32  2002/04/28 17:29:43  scheuerm
 *Added some documentation
 *
 *Revision 1.31  2002/04/27 18:30:03  moon
 *Finished commenting
 *
 *Revision 1.30  2002/04/27 17:48:34  bobkov
 *Added comments
 *
 *Revision 1.29  2002/04/27 00:08:27  talbert
 *Final commenting run through . . . no functional changes.
 *
 *Revision 1.28  2002/04/26 17:37:13  moon
 *Fixed callback on save preproc dialog cancel button.
 *Fixed bubble browser output.  Position was zero-based, which didn't match the 2D
 *window slice numbers (1 based), so I changed the bubble positions to be cursor
 *position +1.
 *Disallowed starting snake window if current label in not visible.
 *Put in Apply+ button in threshold dialog, which changes seg overlay to be an
 *overlay of the positive voxels in the preproc data (a zero-level visualization).
 *Added more m_OutMessage and m_OutMessage messages.
 *
 *Revision 1.27  2002/04/24 19:50:23  moon
 *Pulled LoadGreyFileCallback out of GUI into UserInterfaceLogic, made modifications due
 *to change in ROI semantics.  Before, the ROI was from ul to lr-1, which is a bad
 *decision.  I changed everything to work with a ROI that is inclusive, meaning
 *that all voxels from ul through lr inclusive are part of the ROI. This involved
 *a lot of small changes to a lot of files.
 *
 *Revision 1.26  2002/04/23 19:28:20  bobkov
 *declared TweakROI method in UserInterfaceLogic.h to be public
 *left ColorLabel.h and ColorLabel.cpp unchanged
 *
 *Revision 1.25  2002/04/19 23:03:59  moon
 *Changed more stuff to get the snake params state synched with the global state.
 *Changed the range of ground in snake params dialog.
 *Removed the use_del_g stuff, since it's really not necessary, I found out.
 *
 *Revision 1.24  2002/04/19 20:34:58  moon
 *Made preproc dialogs check global state and only preproc if parameters have changed.
 *So no if you hit apply, then ok, it doesn't re process on the ok.
 *
 *Revision 1.23  2002/04/18 21:14:03  moon
 *I had changed the Cancel buttons to be Close on the Filter dialogs, and I changed
 *the names of the callbacks in GUI, but not in UserInterfaceLogic.  So I just hooked them
 *up so the dialogs get closed.
 *
 *Revision 1.22  2002/04/18 21:04:51  moon
 *Changed the IRIS window ROI stuff.  Now the ROI is always valid if an image is
 *loaded, but there is a toggle to show it or not.  This will work better with
 *Konstantin's addition of being able to drag the roi box.
 *
 *I also changed a bunch of areas where I was calling InitializeSlice for the 2D windows,
 *when this is not at all what I should have done.  Now those spots call
 *MakeSegTextureCurrent, or MakeGreyTextureCurrent.  This means that the view is not
 *reset every time the snake steps, the preproc/orig radio buttons are changed, etc.
 *
 *Revision 1.21  2002/04/16 18:54:33  moon
 *minor bug with not stopping snake when play is pushed, and then other
 *buttons are pushed.  Also added a function that can be called when the user
 *clicks the "X" on a window, but it's not what we want, I don't think.  The
 *problem is if the user clicks the "X" on the snake window when a "non modal"
 *dialog is up, all the windows close, but the program doesn't quit.  I think
 *it's a bug in FLTK, but I can't figure out how to solve it.
 *
 *Revision 1.20  2002/04/16 13:07:56  moon
 *Added tooltips to some widgets, made minor changes to enabling/disabling of
 *widgets, clearing 3D window when initialization is restarted in snake window,
 *changed kappa in edge preproc dialog to be [0..1] range instead of [0..3]
 *
 *Revision 1.19  2002/04/14 22:02:54  scheuerm
 *Changed loading dialog for preprocessed image data. Code restructuring
 *along the way: Most important is addition of
 *SnakeVoxDataClass::ReadRawPreprocData()
 *
 *Revision 1.18  2002/04/11 23:07:43  bobkov
 *Commented the Bubble class
 *Commented m_BtnAddBubble, m_BtnRemoveBubble,
 *m_BrsActiveBubbles and m_InBubbleRadius callbacks
 *Commented GetBubbles and GetNumberOfBubbles methodS
 *
 *Revision 1.17  2002/04/10 20:19:40  moon
 *got play and stop vcr buttons to work.
 *put in lots of comments.
 *
 *Revision 1.16  2002/04/09 18:59:33  moon
 *Put in dialog to change snake parameters.  Also implemented Rewind button, which
 *now restarts the snake.  It seems for now that changing snake parameters restarts
 *the snake.  I don't know if this is the way it has to be, or I just did something
 *wrong in snakewrapper.  I'll have to check with Sean.
 *
 *Revision 1.15  2002/04/08 13:32:35  talbert
 *Added a preprocessed save dialog box as well as a save preprocessed menu
 *option in the snake window.  Added the code necessary to implement the
 *GUI side of saving.
 *
 *Revision 1.14  2002/04/07 02:22:49  scheuerm
 *Improved handling of OK and Apply buttons in preprocessing dialogs.
 *
 *Revision 1.13  2002/04/04 15:30:09  moon
 *Put in code to get StepSize choice box filled with values and working.
 *AcceptSegment button callback puts snake seg data into full_data (IRIS)
 *Fixed a couple more UI cosmetic things.
 *
 *Revision 1.12  2002/04/03 22:12:07  moon
 *Added color chip, image probe, seg probe to snake window, although seg probe
 *maybe shouldn't be there.  added update continuously checkbox to 3Dwindow.
 *changes accept/restart to be on top of each other, and one is shown at a time,
 *which I think is more intuitive.
 *changed snake iteration field to be text output.  added callback for step size
 *choice.
 *
 *Revision 1.11  2002/03/27 17:59:40  moon
 *changed a couple things.  nothing big. a callback in .fl was bool return type
 *which didn't compile in windows. this is the version I think will work for a
 *demo for Kye
 *
 *Revision 1.10  2002/03/26 18:16:32  scheuerm
 *Added loading and display of preprocessed data:
 *- added vtkImageDeepCopy function
 *- added flags indicating which dataset to display in GlobalState
 *- added flag indicating whether to load gray or preprocessed data
 *  in the GUI class
 *
 *Revision 1.9  2002/03/25 02:15:57  scheuerm
 *Added loading of preprocessed data. It isn't being converted
 *to floats yet. It's not possible to actually display the data
 *right now.
 *
 *Revision 1.8  2002/03/24 19:27:46  talbert
 *Added callback the preprocess button to show dialog boxes for filtering.  Added callbacks for buttons in filtering dialog boxes.  Modified the AddBubbles callback so that the newest bubble is selected in the Bubble Browser.  m_OutAboutCompiled and ran to verify that new bubbles are selected and that the dialogs appear over the
 *3d window.  talbert s f
 *
 *Revision 1.7  2002/03/21 15:45:46  bobkov
 *implemented callbacks for buttons AddBubble and RemoveBubble, implemented callbacks for Radius slider and ActiveBubble browser, created methods getBubbles and getNumberOfBubbles   e
 *
 *Revision 1.6  2002/03/19 19:35:32  moon
 *added snakewrapper to makefile so it gets compiled. started putting in callback,
 *etc. for snake vcr buttons.  added snake object to IrisGlobals, instantiated in Main
 *
 *Revision 1.5  2002/03/19 17:47:10  moon
 *added some code to disable widgets, make the radio buttons work, etc. in the snake window.  fixed the quit callback from the snake window to work (crashed before)
 *changed the [accept/restart]bubble_button widgets to be acceptinitialization_button and added callbacks (empty).
 *
 *Revision 1.4  2002/03/08 14:23:48  moon
 *added comments
 *
 *Revision 1.3  2002/03/08 14:06:29  moon
 *Added Header and Log tags to all files
 **/
