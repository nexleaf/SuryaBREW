/*===========================================================================

FILE: suryabrew.c
===========================================================================*/


#include "suryabrew.h"


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
	that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the applet which is being loaded

	pIShell: [in]: Contains pointer to the IShell object. 

	pIModule: pin]: Contains pointer to the IModule object to the current module to which
	this app belongs

	ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
	of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_SURYABREW )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(suryabrew),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)suryabrew_HandleEvent,
                          (PFNFREEAPPDATA)suryabrew_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(suryabrew_InitAppData((suryabrew*)*ppObj))
			{
				//Data initialized successfully
				return(AEE_SUCCESS);
			}
			else
			{
				//Release the applet. This will free the memory allocated for the applet when
				// AEEApplet_New was called.
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}

        } // end AEEApplet_New

    }

	return(EFAILED);
}


static boolean suryabrew_HandleEventCameraMode(suryabrew* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{

	int nErr = 0;
    switch (eCode) 
	{


	case EVT_KEY:

		/*
		Any of the top keys will take the image when we go into the
		camera mode
*/

		switch (wParam)
		{
		case AVK_LEFT:
			break;
		case AVK_RIGHT:
			break;
		case AVK_UP:
			suryabrew_PrintMem(pMe);
			break;
		case AVK_DOWN:
			DBGPRINTF("State %d-%d", pMe->SuryaMode, pMe->CameraMode);
			break;
		case AVK_SOFT1:
			if (pMe->CameraMode == CAM_MODE_READY)
			{
				// Keep the image and stick it into the DB
				suryabrew_GPSValid(pMe);
				suryabrew_DBAddRecord(pMe);
				//suryabrew_DBDEBUG(pMe);

				pMe->SuryaMode = SURYAMODE_DEFAULT;
				suryabrew_DrawScreen(pMe);
				suryabrew_CameraUnload(pMe);
				return TRUE;
			} // fall through since in mode where taking picture
		case AVK_END:
		case AVK_SOFT2:
		case AVK_CLR:
			if (pMe->CameraMode == CAM_MODE_READY)
			{
				// Drop the image
				suryabrew_CameraDropImage(pMe);
				pMe->SuryaMode = SURYAMODE_DEFAULT;
				suryabrew_DrawScreen(pMe);
				suryabrew_CameraUnload(pMe);
				return TRUE;
			} // fall through since in mode where taking picture
		case AVK_SELECT:
			if (pMe->CameraMode == CAM_MODE_PREVIEW)
			{
				suryabrew_CameraStop(pMe);
				return TRUE;
			}
			break;
		default:
			return FALSE;
		}


        default:
            break;
	}
	nErr = 0;
	return FALSE;

}

static boolean suryabrew_HandleEventTempMode(suryabrew* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{

	int nErr = 0;
    switch (eCode) 
	{

	case EVT_KEY:

		if (pMe->tempActive)
		{
			DBGPRINTF("!!!! sndconf in evhandler");
			ISOUND_Set(pMe->pISound, &pMe->soundInfo);
			ISOUND_SetDevice(pMe->pISound);
		}

		// Add your code here...
		switch (wParam)
		{
		case AVK_LEFT:
			suryabrew_DBDEBUG(pMe);
			break;
		case AVK_RIGHT:
			break;
		case AVK_UP:
			suryabrew_PrintMem(pMe);
			break;
		case AVK_DOWN:
			DBGPRINTF("State %d-%d-%d-%d", pMe->SuryaMode, pMe->CameraMode, pMe->tempActive, pMe->tempVocOn);
			break;
		case AVK_END:
		case AVK_CLR:
		case AVK_SOFT1:
			if (pMe->tempActive)
			{
				DBGPRINTF("Stopping temp sensing");
				suryabrew_TempSetActive(pMe, FALSE);
				suryabrew_DrawTempScreen(pMe);
			} 
			else 
			{
				DBGPRINTF("Unloading temp sensing");
				//suryabrew_TempInit(pMe);
				suryabrew_TempUnloadVOC(pMe);
				pMe->SuryaMode = SURYAMODE_DEFAULT;
				suryabrew_DrawScreen(pMe);
			}
			return(TRUE);
		case AVK_SOFT2:
			// on the right key, start the temp sensor
			DBGPRINTF("Running temp start/stop!!!");
			if (pMe->tempActive) {
				suryabrew_TempSetActive(pMe, FALSE);
				suryabrew_DrawTempScreen(pMe);
				//suryabrew_TempStopVOC(pMe);
			} else {
				if (pMe->allow_volume == 0) {
					pMe->soundMode = SOUNDMODE_LOW;
					suryabrew_TempLoadSound(pMe);
				}
				suryabrew_TempSetActive(pMe, TRUE);
			}
			suryabrew_DrawTempScreen(pMe);
			return TRUE;
		case AVK_SELECT:

			return TRUE;
		case AVK_VOLUME_UP:
			if (pMe->soundMode == SOUNDMODE_PLAYBACK || pMe->allow_volume == 0)
			{
				return TRUE;
			}
			switch (pMe->soundMode)
			{
			case SOUNDMODE_ONE:
				pMe->soundMode = SOUNDMODE_LOW;
				break;
			case SOUNDMODE_LOW:
				pMe->soundMode = SOUNDMODE_MED;
				break;
			case SOUNDMODE_MED:
				pMe->soundMode = SOUNDMODE_HIGH;
				break;
			case SOUNDMODE_HIGH:
				pMe->soundMode = SOUNDMODE_ONE;
				break;
			default:
				break;
			}
			suryabrew_TempLoadSound(pMe);			
			return TRUE;
		case AVK_VOLUME_DOWN:
			if (pMe->soundMode == SOUNDMODE_PLAYBACK || pMe->allow_volume == 0)
			{
				return TRUE;
			}
			switch (pMe->soundMode)
			{
			case SOUNDMODE_ONE:
				pMe->soundMode = SOUNDMODE_HIGH;
				break;
			case SOUNDMODE_LOW:
				pMe->soundMode = SOUNDMODE_ONE;
				break;
			case SOUNDMODE_MED:
				pMe->soundMode = SOUNDMODE_LOW;
				break;
			case SOUNDMODE_HIGH:
				pMe->soundMode = SOUNDMODE_MED;
				break;
			default:
				break;
			}
			suryabrew_TempLoadSound(pMe);			
			return TRUE;
		case AVK_5:
			if (pMe->allow_playback == 0)
			{
				return TRUE;
			}
			if (pMe->soundMode != SOUNDMODE_PLAYBACK)
			{
				pMe->soundMode = SOUNDMODE_PLAYBACK;
			} 
			else
			{
				pMe->soundMode = SOUNDMODE_MED;
			}
			suryabrew_TempLoadSound(pMe);
			return TRUE;
		case AVK_7:
			//suryabrew_TempPlayTone(pMe);
			return TRUE;
		case AVK_9:
			//suryabrew_TempStopTone(pMe);
			return TRUE;
		default:
			return FALSE;
		}


        default:
            break;
	}
	nErr = 0;			
	return FALSE;
}

static boolean suryabrew_HandleEventDefaultMode(suryabrew* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{

	int nErr = 0;
    switch (eCode) 
	{

	case EVT_KEY:


		// Add your code here...
		switch (wParam)
		{
		case AVK_LEFT:
			suryabrew_DBDEBUG(pMe);
			break;
		case AVK_RIGHT:
			break;
		case AVK_UP:
			suryabrew_PrintMem(pMe);
			break;
		case AVK_DOWN:
			DBGPRINTF("State %d-%d", pMe->SuryaMode, pMe->CameraMode);
			break;
		case AVK_SOFT1:
			// on the left key start the camera
			if (pMe->enable_camera)
			{
				// INIT Camera
				suryabrew_DrawPhotoScreen(pMe);
				suryabrew_CameraLoad(pMe);
				if (pMe->CameraMode == CAM_MODE_READY)
				{
					//					suryabrew_CameraInitSnapshot(pMe);
					DBGPRINTF("Running Camera Code!!!");
					suryabrew_CameraInitPreview(pMe);
					pMe->SuryaMode = SURYAMODE_CAMERA;
				}
				return(TRUE);
			}
			break;
		case AVK_SOFT2:
			// on the right key, start the temp sensor
			DBGPRINTF("Running temp code!!!");
			//suryabrew_TempInit(pMe);
			suryabrew_TempInitVOC(pMe);
			pMe->SuryaMode = SURYAMODE_TEMP;
			suryabrew_DrawTempScreen(pMe);
			return TRUE;
		case AVK_END:
		case AVK_CLR:
			break;
		case AVK_SELECT:
			break;
		default:
			return(FALSE);
		}


        default:
            break;
	}
	nErr = 0;
	return FALSE;

}

static boolean suryabrew_HandleEventSplitOnMode(suryabrew* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch (pMe->SuryaMode)
	{
	case SURYAMODE_DEFAULT:
		return suryabrew_HandleEventDefaultMode(pMe, eCode, wParam, dwParam); 
		break;
	case SURYAMODE_CAMERA:
		return suryabrew_HandleEventCameraMode(pMe, eCode, wParam, dwParam); 
		break;
	case SURYAMODE_TEMP:
		return suryabrew_HandleEventTempMode(pMe, eCode, wParam, dwParam); 
		break;
	default:
		DBGPRINTF("SplitOnMode: unkown mode!!!  %d",  pMe->SuryaMode);
		return FALSE;
	}

}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
	This is the EventHandler for this app. All events to this app are handled in this
	function. All APPs must supply an Event Handler.

PROTOTYPE:
	boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
	pi: Pointer to the AEEApplet structure. This structure contains information specific
	to this applet. It was initialized during the AEEClsCreateInstance() function.

	ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean suryabrew_HandleEvent(suryabrew* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	int nErr = 0;
    switch (eCode) 
	{
        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);

        // App is told it is starting up
        case EVT_APP_START:                        

			// SAME AS APP_RESUME, so just drop to it

        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...
			{
				// STARTUP code that runs no matter what

				// INIT Display
				suryabrew_DrawScreen(pMe);

				// INIT THE GPS
				if (pMe->do_gps)
				{
					suryabrew_GPSInit(pMe);
				}

				// INIT THE DB
				suryabrew_DBInit(pMe);

				// INIT IMG UPLOAD
				if (pMe->do_upload)
				{
					suryabrew_UPIMGInit(pMe);
				}

			}

			return(TRUE);

        // App is being suspended 
        case EVT_APP_SUSPEND:

			suryabrew_FreeSurya(pMe, 1);
      		return(TRUE);

        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		    // Add your code here...

      		return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
			DBGPRINTF("KEYPRESS!!!!!!!!!!!!!!!!!!!");

      		return suryabrew_HandleEventSplitOnMode(pMe, eCode, wParam, dwParam);

	  // Flip event. Look at the wParam above to know flip state.
        case EVT_FLIP:
			// wParam = FALSE, when flip is closed.
			if(wParam == FALSE)
			{
				// Add your code here...				
			}			
			else 
			{				
				// Add your code here...	
			}

			return(TRUE);

	   // Key-guard event. Look at the wParam above to know key-guard state.
        case EVT_KEYGUARD:

			// wParam = TRUE, when key-guard is enabled.
			if(wParam == TRUE)
			{
				// Add your code here...	
			}			
			else
			{				
				// Add your code here...	
			}	
				
      		return(TRUE);	

        // If nothing fits up to this point then we'll just break out
        default:
            break;
	}
	nErr = 0;
	return FALSE;
}


// this function is called when your application is starting up
boolean suryabrew_InitAppData(suryabrew* pMe)
{
	int nErr = 0;
	int pnSize = 0;
	char * IMEI = NULL;
	AEEHWIDType* HWID = NULL;

    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // Insert your code here for initializing or allocating resources...
	pMe->pIShell = pMe->a.m_pIShell;

	///////////////////////
	// CONFIG INIT
	pMe->do_gps = 0;
	pMe->do_upload = 0;
	pMe->do_upload_delete_file = 1;
	pMe->allow_volume = 1;
	pMe->allow_playback = 0;
	pMe->enable_camera = 0;
	pMe->enable_datalogger = 1;

	///////////////////////
	// DISPLAY INIT
	pMe->pIDisplay = pMe->a.m_pIDisplay;
	pMe->m_cxWidth = pMe->DeviceInfo.cxScreen;  // Cache the width of the device screen
	pMe->m_cyHeight = pMe->DeviceInfo.cyScreen;  // Cache the height of the device screen
	DBGPRINTF("InitAppData Res is %d x %d", pMe->m_cxWidth, pMe->m_cyHeight);
	pMe->pIImage_checkyes = ISHELL_LoadResImage(pMe->pIShell, SURYABREW_RES_FILE, IDI_OBJECT_CHECKYES);
	pMe->pIImage_checkno = ISHELL_LoadResImage(pMe->pIShell, SURYABREW_RES_FILE, IDI_OBJECT_CHECKNO);
	IIMAGE_SetDisplay(pMe->pIImage_checkyes, pMe->pIDisplay);
	IIMAGE_SetDisplay(pMe->pIImage_checkno, pMe->pIDisplay);

	///////////////////////
	// CAMERA INIT
	pMe->pICamera = NULL;
	pMe->CameraMode = 0;
	IMEI = MALLOC(sizeof(char) * 18); // IMEI is a null terminated 16 char string
	HWID = MALLOC(sizeof(AEEHWIDType) + 32); // in worst case if AEEHWID + 8 ... just making sure

	// Get some form of HW ID for the file name output
	pnSize = sizeof(char) + 18;
	nErr = ISHELL_GetDeviceInfoEx(pMe->pIShell, AEE_DEVICEITEM_IMEI, (void *) IMEI, &pnSize);
	DBGPRINTF("InitAppData IMEI %d %d %s", nErr, pnSize, IMEI);
	if (nErr != SUCCESS)
	{
		pMe->HWID = MALLOC(sizeof(char) * 32);
		pnSize = sizeof(AEEHWIDType) + 10;
		nErr = ISHELL_GetDeviceInfoEx(pMe->pIShell, AEE_DEVICEITEM_HWID, (void *) HWID, &pnSize);
		if (HWID->bHWIDType == AEE_HWID_ESN || HWID->bHWIDType == AEE_HWID_PESN)
		{
			SPRINTF(pMe->HWID, "%u", *HWID->HWID);
			DBGPRINTF("InitAppData HWID %d %d %u", nErr, pnSize, HWID->HWID[0]);
		}
		else if (HWID->bHWIDType == AEE_HWID_MEID)
		{
			SPRINTF(pMe->HWID, "%lu", *HWID->HWID);
			DBGPRINTF("InitAppData HWID %d %d %lu", nErr, pnSize, HWID->HWID[0]);
		}
		else 
		{
			SPRINTF(pMe->HWID, "%s", HWID->HWID);
			DBGPRINTF("InitAppData HWID %d %d %s", nErr, pnSize, HWID->HWID);
		}
	} else {
		pMe->HWID = MALLOC(STRLEN(IMEI) + 1);
		SPRINTF(pMe->HWID, "%s", IMEI);
	}
	FREE(IMEI);
	FREE(HWID);

	///////////////////////
	// IMAGE INIT
	pMe->pIImage = NULL;

	///////////////////////
	// GPS INIT
	pMe->pIPosDet = NULL;

	///////////////////////
	// DB Init
	pMe->pIDatabase = NULL;

	///////////////////////
	// Temp Init
	//pMe->pIMediaPlay = NULL;
	//pMe->pIMediaRec = NULL;
	//pMe->pIMediaUtilPlay = NULL;
	//pMe->pIMediaUtilRec = NULL;
	pMe->pIVocoder = NULL;
	pMe->tempVocOn = FALSE;
	pMe->tempActive = FALSE;
	//	pMe->playing = FALSE;
	//pMe->soundMode = SOUNDMODE_PLAYBACK;
	pMe->soundMode = SOUNDMODE_LOW;
	pMe->pIFileAudioOut = NULL;

	pMe->maxSound = 0;
	pMe->cycleMax = 20;
	pMe->cycleCount = 0;
	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE, IDS_STRING_TEMPFMT,
		pMe->tempDispFmt, sizeof(pMe->tempDispFmt));
	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE, IDS_STRING_TEMPFMT_DATALOG,
		pMe->tempDispFmtDataLog, sizeof(pMe->tempDispFmtDataLog));
	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE, IDS_STRING_TEMPDBGFMT,
		pMe->tempDispDBGFmt, sizeof(pMe->tempDispDBGFmt));

	///////////////////////
	// DataLogger Init
	pMe->dataloggerRunning = FALSE;
	pMe->dataloggerCounter = 0; // will be set when datalogger is initialized
	pMe->dataloggerCounterIncrementInterval = 1440000; // 8000 * 60 * 3 = 3 minutes
	pMe->dataloggerCounterSamples = 0;
	pMe->pIFileDatalogger = NULL;


	///////////////////////
	// IWeb Init
	pMe->pIWeb = NULL;
	pMe->pIWebResp = NULL;
	pMe->pIWebUtil = NULL;
	pMe->pIMultiPeek = NULL;
	pMe->pIGetLine = NULL;
	pMe->poststr = NULL;

	///////////////////////
	// IHeap Inint
	pMe->pIHeap = NULL;

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void suryabrew_FreeAppData(suryabrew* pMe)
{
    // insert your code here for freeing any resources you have allocated...
	suryabrew_FreeSurya(pMe, (int) 0);
}

void suryabrew_FreeSurya(suryabrew *pMe, int from)
{
	// if from 0, then from FreeAppData
	// if from 1, then from Suspend

	if (from == 0)
	{

		DBGPRINTF("From FreeAppData");
		//FREE(pMe->IMEI);
		FREE(pMe->HWID);

	} else {

		DBGPRINTF("From suspend");

	}


	DBGPRINTF("Unload Camera");
	suryabrew_CameraUnload(pMe);

	DBGPRINTF("Unload icons");
	if ( pMe->pIImage_checkyes != NULL )
	{
		IIMAGE_Release(pMe->pIImage_checkyes);
		pMe->pIImage_checkyes = NULL;
	}

	if ( pMe->pIImage_checkno != NULL )
	{
		IIMAGE_Release(pMe->pIImage_checkno);
		pMe->pIImage_checkno = NULL;
	}

	DBGPRINTF("Unload GPS");
	suryabrew_GPSUnload(pMe);

	DBGPRINTF("Unload DB");
	suryabrew_DBUnload(pMe);

	DBGPRINTF("Unload Upload");
	suryabrew_UPIMGUnload(pMe);

	DBGPRINTF("Unload Temp");
	suryabrew_TempUnloadVOC(pMe);

	// Do not need this... always freed right after use anyways
	DBGPRINTF("Unload FileMgr");
	suryabrew_FileUnload(pMe);
	//free(pMe);

}




static void suryabrew_DrawScreen(suryabrew * pMe)
{
	AECHAR szBuf[30] = {0};

	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IDISPLAY_Update(pMe->pIDisplay);

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TITLE,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_CENTER | IDF_ALIGN_TOP);

	if (pMe->enable_camera == 1) {
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
			IDS_STRING_CAMERA,
			szBuf, sizeof(szBuf));
		IDISPLAY_DrawText(pMe->pIDisplay,
			AEE_FONT_BOLD, 
			szBuf, 
			-1, 0, 0, NULL,
			IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
	}

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TEMP,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_BOTTOM | IDF_ALIGN_RIGHT);

	IDISPLAY_Update(pMe->pIDisplay);
}

void suryabrew_DrawTempTemp(suryabrew *pMe)
{
	//int res = (pMe->maxSound + 858) / 19;
	int res = suryabrew_TempCalcTemp(pMe); //(pMe->maxSound * 258 + 360000)/10000;
	if (pMe->dataloggerRunning) {
		WSPRINTF(pMe->tempDisp, 64, pMe->tempDispFmtDataLog, pMe->dataloggerCounter, res);	
	} else {
		WSPRINTF(pMe->tempDisp, 64, pMe->tempDispFmt, res);
	}
	WSPRINTF(pMe->tempDispDBG, 64, pMe->tempDispDBGFmt, pMe->maxSound, pMe->minSound, (pMe->maxSound + pMe->minSound)/2);

	IDISPLAY_ClearScreen(pMe->pIDisplay);

	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD,
		pMe->tempDisp,
		-1, 0, 0, NULL,
		IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER);
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD,
		pMe->tempDispDBG,
		-1, 0, 0, NULL,
		IDF_ALIGN_TOP | IDF_ALIGN_CENTER);

	IDISPLAY_Update(pMe->pIDisplay);

}



void suryabrew_DrawPhotoScreen(suryabrew * pMe)
{
	AECHAR szBuf[30] = {0};

	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IDISPLAY_Update(pMe->pIDisplay);

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TITLE,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_CENTER | IDF_ALIGN_TOP);

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TAKEPHOTO,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_CENTER | IDF_ALIGN_BOTTOM);

	IDISPLAY_Update(pMe->pIDisplay);
}


void suryabrew_DrawTempScreen(suryabrew * pMe)
{
	AECHAR szBuf[30] = {0};

	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IDISPLAY_Update(pMe->pIDisplay);

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TITLE,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_CENTER | IDF_ALIGN_TOP);

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_BACK,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);

	DBGPRINTF("in Draw tempActive: %d", pMe->tempActive);
	if (pMe->tempActive) {
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
			IDS_STRING_STOPTEMP,
			szBuf, sizeof(szBuf));
	} else {
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
			IDS_STRING_STARTTEMP,
			szBuf, sizeof(szBuf));
	}
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_BOTTOM | IDF_ALIGN_RIGHT);


	//	IDISPLAY_DrawText(pMe->pIDisplay,
	//		AEE_FONT_NORMAL, szBuf, -1, pMe->m_cxWidth/5, pMe->m_cyHeight/8, 0, IDF_ALIGN_CENTER);

	//AECHAR *szText = L"Surya Filter Capture";
	//IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
	//	AEE_FONT_BOLD,        // Use BOLD font
	//szText,               // Text - Normally comes from resource
	//	-1,                   // -1 = Use full string length
	//	0,                   // Ignored - IDF_ALIGN_CENTER
	//	0,                   // Ignored - IDF_ALIGN_MIDDLE
	//	NULL,                // No clipping
	//	IDF_ALIGN_CENTER | IDF_ALIGN_TOP);

	IDISPLAY_Update(pMe->pIDisplay);
}

void suryabrew_DrawCheckYesNo(suryabrew *pMe)
{
	AEEImageInfo II; // should malloc due to small stack

	AECHAR szBuf[30] = {0};

	int at_x = 0;
	int at_y = 0;
	IDISPLAY_Update(pMe->pIDisplay);
	MEMSET(&II, 0, sizeof(AEEImageInfo));

	IIMAGE_GetInfo(pMe->pIImage_checkyes, &II);

	at_x = 0;
	at_y = pMe->m_cyHeight - II.cy;
	IIMAGE_Draw(pMe->pIImage_checkyes, at_x, at_y); 
	DBGPRINTF("DrawCheckYesNo: yes at %d %d", at_x, at_y);
	IDISPLAY_Update(pMe->pIDisplay);

	at_x = pMe->m_cxWidth - II.cx;
	IIMAGE_Draw(pMe->pIImage_checkno, at_x, at_y); 
	DBGPRINTF("DrawCheckYesNo: no  at %d %d", at_x, at_y);

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_ACCEPTPHOTO,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_CENTER | IDF_ALIGN_BOTTOM);


	IDISPLAY_Update(pMe->pIDisplay);

}





void suryabrew_PrintMem(suryabrew *pMe)
{
	int res = 0;
	uint32 memres = 0;
	uint32 maxheap = 0;
	uint32 heapused = 0;

	res = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_HEAP, (void **)&pMe->pIHeap);

	if (res != SUCCESS)
	{
		DBGPRINTF("Error Creating IVocoder interface %i", res);
		return;
	}

	//memres = IHEAP_GetMemStats(pMe->pIHeap);
	//DBGPRINTF("GetMemStats: %d", memres);

	res = IHEAP_GetModuleMemStats(pMe->pIHeap, AEECLSID_SURYABREW, &maxheap, &heapused);
	DBGPRINTF("res: %d max: %d: curr: %d");
	//res = IHEAP_GetModuleMemStats(pMe->pIHeap, AEECLSID_CAMERA, &maxheap, &heapused);
	//DBGPRINTF("res: %d max: %d: curr: %d");
	//res = IHEAP_GetModuleMemStats(pMe->pIHeap, AEECLSID_WEB, &maxheap, &heapused);
	//DBGPRINTF("res: %d max: %d: curr: %d");
	//res = IHEAP_GetModuleMemStats(pMe->pIHeap, 0, &maxheap, &heapused);
	//DBGPRINTF("res: %d max: %d: curr: %d");

	IHEAP_Release(pMe->pIHeap);
	pMe->pIHeap = NULL;
}





