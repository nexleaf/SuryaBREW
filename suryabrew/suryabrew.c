
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
	DBGPRINTF("in aeeclscreateinstance");
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
			DBGPRINTF("BEFORE INIT");
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

/*
static boolean suryabrew_HandleEventCameraMode(suryabrew* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{

	int nErr = 0;
    switch (eCode) 
	{


	case EVT_KEY:

		
		Any of the top keys will take the image when we go into the
		camera mode


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
				 Keep the image and stick it into the DB
				suryabrew_GPSValid(pMe);
				suryabrew_DBAddRecord(pMe);
				suryabrew_DBDEBUG(pMe);

				pMe->SuryaMode = SURYAMODE_DEFAULT;
				suryabrew_DrawScreen(pMe);
				suryabrew_CameraUnload(pMe);
				return TRUE;
			}  fall through since in mode where taking picture
		case AVK_END:
		case AVK_SOFT2:
		case AVK_CLR:
			if (pMe->CameraMode == CAM_MODE_READY)
			{
				 Drop the image
				suryabrew_CameraDropImage(pMe);
				pMe->SuryaMode = SURYAMODE_DEFAULT;
				suryabrew_DrawScreen(pMe);
				suryabrew_CameraUnload(pMe);
				return TRUE;
			}  fall through since in mode where taking picture
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

}*/

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
//			DBGPRINTF("State %d-%d-%d-%d", pMe->SuryaMode, pMe->CameraMode, pMe->tempActive, pMe->tempVocOn);
			break;
		case AVK_END:
		
		case AVK_SOFT1:
			
		//	if(pMe->headset_insert==FALSE)
		//		ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
			if (pMe->tempActive)
			{
			/*	DBGPRINTF("Stopping temp sensing");
				suryabrew_TempSetActive(pMe,FALSE);
				suryabrew_DrawTempScreen(pMe);*/
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
			case AVK_CLR:
//				suryabrew_Imedia(pMe);
//				
//			if(pMe->headset_insert==5)
//			{
//				ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
//			}
			
//			if(suryabrew_HeadphoneDetect(pMe))
		//	if(pMe->headset_insert==TRUE)
			// on the right key, start the temp sensor
				pMe->done_sample_time=FALSE;
			if(pMe->check_recod_fail_4times<NO_UPLOAD_FAIL)
			{
		
			DBGPRINTF("Running temp start/stop!!!");
			if (pMe->tempActive) {
				CALLBACK_Cancel(&pMe->Get_Current_TimerCB);
				ISHELL_CancelTimer(pMe->pIShell,(PFNNOTIFY) suryabrew_getcurrtime,pMe);
				suryabrew_TempSetActive(pMe, FALSE);
				suryabrew_DrawTempScreen(pMe);
				//suryabrew_TempStopVOC(pMe);
			} else {
				
					pMe->Flag=0;
					pMe->SEC=0;
					pMe->temp_data_index=0;
					if (pMe->allow_volume == 0) {
													pMe->soundMode = SOUNDMODE_LOW;
													suryabrew_TempLoadSound(pMe);
												}
	//				suryabrew_gettime(pMe);
					suryabrew_TempSetActive(pMe, TRUE);
					CALLBACK_Init(&pMe->Get_Current_TimerCB, suryabrew_getcurrtime, pMe);
					suryabrew_getcurrtime(pMe);
			}
			suryabrew_DrawTempScreen(pMe);
			}
			
			
//			else
//			{
//////				pMe->headset_insert=5;
//				DBGPRINTF("sesing device msg");
//					suryabrew_TempSetActive(pMe, FALSE);
//				
//				suryabrew_DrawMessage(pMe);
//			}
			
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
			if (pMe->soundMode != SOUNDMODE_PLAYBACK)
			{
				pMe->soundMode = SOUNDMODE_PLAYBACK;
			} 
			else
			{
				pMe->soundMode = SOUNDMODE_LOW;
			}
			suryabrew_TempLoadSound(pMe);
			return TRUE;

		case AVK_7:
			//suryabrew_TempPlayTone(pMe);
		//	suryabrew_gettime(pMe);
			suryabrew_Remove_DataBase(pMe); //FOR TESTING 
			return TRUE;
		case AVK_9:
			suryabrew_DBDEBUG(pMe);    //FOR TESTING
			//suryabrew_TempStopTone(pMe);
			return TRUE;
		case AVK_6:
			suryabrew_UPDATATimer(pMe);  //FOR TESTING
			return TRUE;
		case AVK_0:
			suryabrew_setdata(pMe);          //FOR TESTING
			suryabrew_DBAddRecord(pMe);
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

		DBGPRINTF("in handleeventdefaultmode");
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
//			DBGPRINTF("State %d-%d", pMe->SuryaMode, pMe->CameraMode);
			break;
		case AVK_SOFT1:
			// on the left key start the camera
//			if(pMe->headset_insert==FALSE)
//				ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
	/*		if (pMe->enable_camera)
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
			}*/
			break;
		case AVK_SOFT2:
		case AVK_CLR:
			// on the right key, start the temp sensor
			DBGPRINTF("Running temp code!!!");
			//suryabrew_TempInit(pMe);
			suryabrew_TempInitVOC(pMe);
			pMe->SuryaMode = SURYAMODE_TEMP;
			suryabrew_DrawTempScreen(pMe);
			return TRUE;
		case AVK_END:
		
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
//		return suryabrew_HandleEventCameraMode(pMe, eCode, wParam, dwParam); 
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
	//AEESoundDevice sounddeviceinfo;
	//sounddeviceinfo.

//	BaseApp *pMe=(BaseApp*)GETAPPINSTANCE();
//	BaseApp *pMe=(BaseApp)GETAPPINSTANCE();

	
    switch (eCode) 
	{
        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);

        // App is told it is starting up
		case EVT_HEADSET:


			///headset
//		if(wParam==TRUE)
//		{
//			pMe->headset_insert=TRUE;
//			if (pMe->tempActive) {
//				suryabrew_TempSetActive(pMe, FALSE);
//				suryabrew_DrawTempScreen(pMe);
//				//suryabrew_TempStopVOC(pMe);
//			} else {
//				if (pMe->allow_volume == 0) {
//					pMe->soundMode = SOUNDMODE_LOW;
//					suryabrew_TempLoadSound(pMe);
//				}
//				suryabrew_TempSetActive(pMe, TRUE);
//			}
//			
//			suryabrew_DrawTempScreen(pMe);
//		}
//		else
//			{
////				pMe->headset_insert=5;
//			pMe->headset_insert=FALSE; 
//				DBGPRINTF("sesing device msg");
//					suryabrew_TempSetActive(pMe, FALSE);
//			//		suryabrew_DrawTempScreen(pMe);
//				suryabrew_DrawMessage(pMe);
//			}
//
//		DBGPRINTF("evt_headset:%d %d",wParam,pMe->headset_insert);

///headset
			return(TRUE);
        case EVT_APP_START:                        



			// SAME AS APP_RESUME, so just drop to it

        // App is being resumed
        case EVT_APP_RESUME:

	//	if(wParam==TRUE)
//			pMe->headset_insert=TRUE;
			pMe->SuryaMode =SURYAMODE_DEFAULT;

		    // Add your code here...
			{
				// STARTUP code that runs no matter what

				// INIT Display
				DBGPRINTF("IN RESUME");
				suryabrew_DrawScreen(pMe);

				// INIT THE GPS
			/*	if (pMe->do_gps)
				{
					suryabrew_GPSInit(pMe);
				}*/

				// INIT THE D
				suryabrew_DBInit(pMe);

			//	ISHELL_SetTimer(pMe->a.m_pIShell,3,(PFNNOTIFY)suryabrew_uploaddata_notify,pMe);

			//	CALLBACK_Init(&pMe->UPIMGTimerCB, suryabrew_uploaddata_notify, pMe);
				// INIT IMG UPLOAD
			///	if (pMe->do_upload)
				{
					suryabrew_UPDATAInit(pMe);
			
					
				}

			}

			return(TRUE);

        // App is being suspended 
        case EVT_APP_SUSPEND:

			DBGPRINTF(" APP SUSPEND");
			if(pMe->tempActive)
				pMe->tempActive=FALSE;
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

boolean suryabrew_HeadphoneDetect(suryabrew *pMe)
{
	int nErr=0;
	uint32   dwCaps;
	AEESoundInfo soundinfo;
	ISOUND_Get(pMe->pISound,&soundinfo);
//	int size=sizeof(pMe->headset_insert);
//	if(pMe->headset_insert==FALSE)
//	AEE_SOUND_METHOD_UNKNOWN
//	EUNSUPPORTED
//	AEE_SOUND_DEVICE_UNKNOWN 
//		 MM_PARM_AUDIO_DEVICE
/////		nErr=ISHELL_GetDeviceInfoEx(pMe->pIShell, AEE_DEVICESTATE_HEADPHONE_ON, (void*)pMe->headset_insert,&(size));
//		DBGPRINTF("value of headphone detected or not : %d",nErr);
//nErr=	IMEDIA_GetMediaParm(pMe->imedia,MM_PARM_AUDIO_DEVICE,(int32 *)&dwCaps,0);
//DBGPRINTF("imedia_getmediaparm==>%d",nErr);
	if(/*soundinfo.eDevice==AEE_SOUND_DEVICE_HEADSET*/dwCaps==SUCCESS)

	{
		DBGPRINTF("headphone detected");
	pMe->headset_insert=TRUE;

//	suryabrew_DrawTempScreen(pMe);
	}
	else{
		pMe->headset_insert=FALSE;
		DBGPRINTF("headphone not detected");
		return FALSE;
	}
	
	return TRUE;
}

// this function is called when your application is starting up
boolean suryabrew_InitAppData(suryabrew* pMe)
{
	int nErr = 0;
	int pnSize = 0;
	int intbuff=10;
	uint8 verbuff=0;
	byte pszversnbuff;
	 
	//
	char * IMEI = NULL;
	AEEHWIDType* HWID = NULL;
	DBGPRINTF("IN INIT");
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // Insert your code here for initializing or allocating resources...
	pMe->pIShell   = pMe->a.m_pIShell;

	///////////////////////
	// CONFIG INIT
//	pMe->do_gps = 0;
//	pMe->do_upload = 0;
//	pMe->do_upload_delete_file = 1;
	pMe->allow_volume = 1;
//	pMe->enable_camera = 0;

	///////////////////////
	// DISPLAY INIT
	pMe->pIDisplay = pMe->a.m_pIDisplay;
	pMe->m_cxWidth = pMe->DeviceInfo.cxScreen;  // Cache the width of the device screen
	pMe->m_cyHeight = pMe->DeviceInfo.cyScreen;  // Cache the height of the device screen
	DBGPRINTF("InitAppData Res is %d x %d", pMe->m_cxWidth, pMe->m_cyHeight);
//	pMe->pIImage_checkyes = ISHELL_LoadResImage(pMe->pIShell, SURYABREW_RES_FILE, IDI_OBJECT_CHECKYES);
//	pMe->pIImage_checkno = ISHELL_LoadResImage(pMe->pIShell, SURYABREW_RES_FILE, IDI_OBJECT_CHECKNO);
//	IIMAGE_SetDisplay(pMe->pIImage_checkyes, pMe->pIDisplay);
//	IIMAGE_SetDisplay(pMe->pIImage_checkno, pMe->pIDisplay);

	///////////////////////
	// CAMERA INIT
//	pMe->pICamera = NULL;
//	pMe->CameraMode = 0;
	IMEI = MALLOC(sizeof(char) * 18); // IMEI is a null terminated 16 char string
	HWID = MALLOC(sizeof(AEEHWIDType) + 32); // in worst case if AEEHWID + 8 ... just making sure

	// Get some form of HW ID for the file name output
	pnSize = sizeof(char) + 18;
	nErr = ISHELL_GetDeviceInfoEx(pMe->pIShell, AEE_DEVICEITEM_IMEI/*AEE_DEVICEITEM_MEIDS*/, (void *) IMEI, &pnSize);
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
	//GET VERSION
	//	 version=ISHELL_GetDeviceInfoEx(pMe->pIShell,AEE_DEVICEITEM_FIRMWARE_VERSION,&verbuff,&intbuff);


	GETAEEVERSION(&pszversnbuff,32, GAV_LATIN1);
	DBGPRINTF("pszvrsbuff-->%c",pszversnbuff);
				
	pMe->pszBuffV=(char*)MALLOC(sizeof(char*)*8);
	MEMSET(pMe->pszBuffV,0,sizeof(char*)*8);
	pMe->pszBuffV=convertByte2Char(pMe->pszBuffV,&pszversnbuff,-1,0);
	DBGPRINTF("Device Version==>%s",pMe->pszBuffV);
	SPRINTF(pMe->upload_version,"%s",pMe->pszBuffV);
//
	FREE(pMe->pszBuffV);
	//GET VERSION

	///////////////////////
	// IMAGE INIT
//	pMe->pIImage = NULL;

	///////////////////////
	// GPS INIT
//	pMe->pIPosDet = NULL;

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
//	pMe->soundMode = SOUNDMODE_PLAYBACK;
	pMe->soundMode = SOUNDMODE_LOW;
	pMe->pIFileAudioOut = NULL;
	pMe->check_recod_fail_4times=0;
	pMe->maxSound = 0;
	pMe->cycleMax = 20;
	pMe->cycleCount = 0;
	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE, IDS_STRING_TEMPFMT,
		pMe->tempDispFmt, sizeof(pMe->tempDispFmt));
	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE, IDS_STRING_TEMPDBGFMT,
		pMe->tempDispDBGFmt, sizeof(pMe->tempDispDBGFmt));

	///////////////////////
	// IWeb Init
/*	pMe->pIWeb = NULL;
	pMe->pIWebResp = NULL;
	pMe->pIWebUtil = NULL;
	pMe->pIMultiPeek = NULL;
	pMe->pIGetLine = NULL;*/
	pMe->poststr = NULL;

	///////////////////////
	// IHeap Inint
	pMe->pIHeap = NULL;

	pMe->DBUploadRecord=0;
	pMe->check_recod_fail_4times=0;
	//////////////////////////////////////////////////////////////////////////
	
	///
	pMe->Tempt_Data_str=(char*)MALLOC(5*sizeof(char));
	pMe->Tempt_Data_Lenght=0;
	//GETTING MIN NO.
	suryabrew_Get_Min_Number(pMe);
	//GETTING MIN NO
//	suryabrew_Imedia(pMe);
    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void suryabrew_FreeAppData(suryabrew* pMe)
{
    // insert your code here for freeing any resources you have allocated...


	DBGPRINTF("IN SURYABREW FREE APP DATA");
	
	suryabrew_FreeSurya(pMe, (int) 0);
	suryabrew_resetdata(pMe);
	DBGPRINTF("free ITAPI instance");
	if(pMe->pITAPI)
	{
		ITAPI_Release(pMe->pITAPI);
		pMe->pITAPI=NULL;
	}
	/*if(pMe->imedia)
		IMEDIA_Release(pMe->imedia);*/
	DBGPRINTF("free data string length");
	if(pMe->Tempt_Data_str)
	{
		FREE(pMe->Tempt_Data_str);
		pMe->Tempt_Data_str=NULL;
	}
	DBGPRINTF("free min no.");
	if(pMe->min_number)
	{
		FREE(pMe->min_number);
		pMe->min_number=NULL;

	}
}

void suryabrew_FreeSurya(suryabrew *pMe, int from)
{
	// if from 0, then from FreeAppData
	// if from 1, then from Suspend
	DBGPRINTF("IN SURYABREW FREE SURYA");
	if (from == 0)
	{

		DBGPRINTF("From FreeAppData");
		//FREE(pMe->IMEI);
		FREE(pMe->HWID);

	} else {

		DBGPRINTF("From suspend");

	}


//	DBGPRINTF("Unload Camera");
//	suryabrew_CameraUnload(pMe);
		DBGPRINTF("FREE CALLBACK FUN");
	CALLBACK_Cancel(&pMe->UPIMGTimerCB);
	ISHELL_CancelTimer(pMe->pIShell,(PFNNOTIFY) suryabrew_uploaddata_notify,pMe);
	CALLBACK_Cancel(&pMe->Get_Current_TimerCB);
	ISHELL_CancelTimer(pMe->pIShell,(PFNNOTIFY) suryabrew_getcurrtime,pMe);

	DBGPRINTF("Unload icons");
/*	if ( pMe->pIImage_checkyes != NULL )
	{
		IIMAGE_Release(pMe->pIImage_checkyes);
		pMe->pIImage_checkyes = NULL;
	}

	if ( pMe->pIImage_checkno != NULL )
	{
		IIMAGE_Release(pMe->pIImage_checkno);
		pMe->pIImage_checkno = NULL;
	}*/

	DBGPRINTF("Unload GPS");
//	suryabrew_GPSUnload(pMe);

	DBGPRINTF("Unload DB");
	suryabrew_DBUnload(pMe);

	DBGPRINTF("Unload Upload");
	suryabrew_UPDataUnload(pMe);

	DBGPRINTF("Unload Temp");
	suryabrew_TempUnloadVOC(pMe);

	// Do not need this... always freed right after use anyways
	DBGPRINTF("Unload FileMgr");
	suryabrew_FileUnload(pMe);
	//free(pMe);

}




static void suryabrew_DrawScreen(suryabrew * pMe)
{
	AECHAR szBuf[100] = {0};
//	AECHAR szBuf1[]={'P','l','e','a','s','e', '\0' ,'I','n','S','e','r','t'};
//	AECHAR szBuf2[]={'T','e','p','t','.','\0','S','e','n','c','i','g','\0','D','e','v','i','c','e'};
	//	AECHAR Exit_btn[10]={0};
	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IDISPLAY_Update(pMe->pIDisplay);

	
	{
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TITLE,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_CENTER | IDF_ALIGN_TOP);

/*	if (pMe->enable_camera == 1) {
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
			IDS_STRING_CAMERA,
			szBuf, sizeof(szBuf));
		IDISPLAY_DrawText(pMe->pIDisplay,
			AEE_FONT_BOLD, 
			szBuf, 
			-1, 0, 0, NULL,
			IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
	}*/

	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_TEMP,
		szBuf, sizeof(szBuf));
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_BOTTOM | IDF_ALIGN_RIGHT);
	}
	IDISPLAY_Update(pMe->pIDisplay);
}

void suryabrew_DrawTempTemp(suryabrew *pMe)
{
	//int res = (pMe->maxSound + 858) / 19;
	int res;
	AECHAR szBuf[30];
	
	// res = (pMe->maxSound * 258 + 360000)/10000;  //y=mx+c 258 3600000
	res = (pMe->maxSound * CALIBRATION_SCALAR + CALIBRATION_OFFSET)/CALIBRATION_DIVIDER;  //y=mx+c 258 3600000
	DBGPRINTF("maxsound %d",pMe->maxSound);
		DBGPRINTF("minsound %d",pMe->minSound);
	DBGPRINTF("res %d \n",res);
	WSPRINTF(pMe->tempDisp, 64, pMe->tempDispFmt, res);
	WSPRINTF(pMe->tempDispDBG, 64, pMe->tempDispDBGFmt, pMe->maxSound, pMe->minSound, (pMe->maxSound + pMe->minSound)/2);
	WSPRINTF(pMe->maxtempsound, 64, pMe->maxtempsoudfmt, pMe->maxSound);
	IDISPLAY_ClearScreen(pMe->pIDisplay);
	//copying tempratur
//	if(pMe->SEC!=pMe->Cur_Sec)
	{

	if(pMe->Tempra_Data[pMe->temp_data_index]!=0)
		pMe->Tempra_Data[pMe->temp_data_index]=0;
	//	MEMSET(pMe->Tempra_Data[pMe->temp_data_index],0,sizeof(uint32));
	pMe->Tempra_Data[pMe->temp_data_index]=(uint32)res;
	DBGPRINTF("taemprature data==>%d",pMe->Tempra_Data[pMe->temp_data_index]);
	MEMSET(pMe->Tempt_Data_str,0,5*sizeof(char));
	SPRINTF(pMe->Tempt_Data_str,"%d",pMe->Tempra_Data[pMe->temp_data_index]);
	pMe->Tempt_Data_Lenght+=STRLEN(pMe->Tempt_Data_str);
	DBGPRINTF("length of temprature data %d%d",pMe->temp_data_index,pMe->Tempt_Data_Lenght);
	DBGPRINTF("array index of data==>%d",pMe->temp_data_index);
	
		pMe->temp_data_index++;
	}
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD,
		pMe->tempDisp,
		-1, 0, 0, NULL,
		IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER);
	DBGPRINTF("pme->tempdisp %u\n",pMe->tempDisp);
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD,
		pMe->tempDispDBG,
		-1, 0, 0, NULL,
		IDF_ALIGN_TOP | IDF_ALIGN_CENTER);
	DBGPRINTF("pme->tempDispDBG %u\n",pMe->tempDispDBG);
//////////////////////////////////////////////////////////////////////////
	ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
			IDS_STRING_STOPTEMP,
			szBuf, sizeof(szBuf));
	
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_BOTTOM | IDF_ALIGN_RIGHT);
	

	//////////////////////////////////////////////////////////////////////////
	
	

	pMe->Get_Time_Flag=FALSE;
	IDISPLAY_Update(pMe->pIDisplay);

}



void suryabrew_DrawPhotoScreen(suryabrew * pMe) //NOT USE
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
	AECHAR szBuf[50] = {0};

	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IDISPLAY_Update(pMe->pIDisplay);
//		if(pMe->headset_insert==FALSE){
//
//		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
//		IDS_STRING_1011,
//		szBuf, sizeof(szBuf));
//
//		IDISPLAY_DrawText(pMe->pIDisplay,
//		AEE_FONT_BOLD, 
//		szBuf, 
//		-1, 0, 70, NULL,
//		 IDF_ALIGN_CENTER);
//		
//		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
//		IDS_STRING_1013,
//		szBuf, sizeof(szBuf));
//
//		IDISPLAY_DrawText(pMe->pIDisplay,
//		AEE_FONT_BOLD, 
//		szBuf, 
//		-1, 0, 90, NULL,
//		 IDF_ALIGN_CENTER);
//
//			ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
//		IDS_STRING_1012,
//		szBuf, sizeof(szBuf));
//	
//		IDISPLAY_DrawText(pMe->pIDisplay,
//		AEE_FONT_BOLD, 
//		szBuf, 
//		-1, 0, 0, NULL,
//		IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
//DBGPRINTF("draw insert head phone message");
//	}
//	else

	{
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
	}
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

void suryabrew_DrawMessage(suryabrew *pMe)  //NOT USE
{
	
//if(pMe->headset_insert==FALSE)

	AECHAR szBuf[80] = {0};

	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IDISPLAY_Update(pMe->pIDisplay);
//	{
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_1011,
		szBuf, sizeof(szBuf));

		IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 70, NULL,
		 IDF_ALIGN_CENTER);
		
		ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_1013,
		szBuf, sizeof(szBuf));

		IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 90, NULL,
		 IDF_ALIGN_CENTER);

			ISHELL_LoadResString(pMe->pIShell, SURYABREW_RES_FILE,
		IDS_STRING_1012,
		szBuf, sizeof(szBuf));
	
		IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_BOLD, 
		szBuf, 
		-1, 0, 0, NULL,
		IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
DBGPRINTF("draw insert head phone message");
	IDISPLAY_Update(pMe->pIDisplay);
}
void suryabrew_DrawCheckYesNo(suryabrew *pMe)   //NOT USE
{
//	AEEImageInfo II; // should malloc due to small stack

/*	AECHAR szBuf[30] = {0};

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
*/
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




void suryabrew_gettime(suryabrew *pMe)
{


		//JulianType curdatetime;
	
	//uint32 cur_time=/*SHELL_GetSeconds(pMe->pIShell);*/GETTIMEMS();
	pMe->Flag=TRUE;
	pMe->Tempt_Data_Lenght=0;
	pMe->Pause_Interval_Flag=FALSE;
	pMe->Add_Record_Flag=FALSE;
	pMe->Get_Time_Flag=TRUE;
	pMe->CurrDateSec = GETTIMESECONDS();
	GETJULIANDATE(pMe->CurrDateSec, &pMe->CurrDate);
	DBGPRINTF("currentdate and time==> %d%d%d%d%d%d\n",pMe->CurrDate.wYear,pMe->CurrDate.wMonth,pMe->CurrDate.wDay,pMe->CurrDate.wHour,pMe->CurrDate.wMinute,pMe->CurrDate.wSecond);
////	return cur_time;
//	suryabrew_getcurrtime(pMe);
}

void suryabrew_getcurrtime(suryabrew *pMe)
{
	JulianType ct;
//    uint32 /*minute,*/sec,Cur_Sec;
	 uint32 cur_time=GETTIMESECONDS();
// 	int i;
	 GETJULIANDATE(cur_time, &ct);
	
	 /* minute= ct.wMinute-pMe->CurrDate.wMinute;
	  sec=ct.wSecond-pMe->CurrDate.wSecond;*/
	 // pMe->Cur_Sec=pMe->SEC;
	  //DBGPRINTF("cur_sec==>%d",pMe->Cur_Sec);
	  pMe->SEC=(ct.wHour*3600+ct.wMinute*60+ct.wSecond)-(pMe->CurrDate.wHour*3600+pMe->CurrDate.wMinute*60+pMe->CurrDate.wSecond);
	  DBGPRINTF("sec==>%d",pMe->SEC);
/*	  if(pMe->Cur_Sec==pMe->SEC && pMe->Pause_Interval_Flag==FALSE)
	  {
		  DBGPRINTF("return false");
		  pMe->Get_Time_Flag=FALSE;
		
	  }
	  else if(pMe->Cur_Sec!=pMe->SEC && pMe->Pause_Interval_Flag==FALSE)
	  {
		  pMe->Get_Time_Flag=TRUE;
	  }*/
	  if(pMe->SEC<SAMPLE_DURATION /*&& pMe->Pause_Interval_Flag==FALSE*/)
	  {
		  pMe->Get_Time_Flag=TRUE;
		  DBGPRINTF("get_time_flag is true");
	  }
	  else if(pMe->SEC>=SAMPLE_DURATION /*&& pMe->Pause_Interval_Flag==FALSE*/&&pMe->done_sample_time==FALSE)
	  {
	//	  sec=0;
		  DBGPRINTF("When Sec is ==>60");
		  pMe->Add_Record_Flag=TRUE;
	//	  pMe->Pause_Interval_Flag=TRUE;
		  pMe->done_sample_time=TRUE;
		//  pMe->Get_Time_Flag=FALSE;
	  }
	 else if((pMe->SEC<=PUASE_INTERVAL+SAMPLE_DURATION) /*&& pMe->Pause_Interval_Flag==TRUE*/)
	  {
		 DBGPRINTF("When sec is puseinterval+sampleduration ");
		  /*return FALSE*/;
	//	 pMe->Pause_Interval_Flag=TRUE;
	  }
	  if(pMe->SEC >=PUASE_INTERVAL+SAMPLE_DURATION /*&&pMe->Pause_Interval_Flag==TRUE*/)
	  {
		 pMe->SEC=0;
	/*	 for(i=0;i<PUASE_INTERVAL;i++)
		  pMe->Tempra_Data[i]=NULL;*/
		// pMe->Pause_Interval_Flag=FALSE;
		 pMe->done_sample_time=FALSE;
		DBGPRINTF("when sec is <=300");
			pMe->temp_data_index=0;
		  pMe->Flag=FALSE;
	  }
	  
//  if(sec<61 && pMe->Pause_Interval_Flag==FALSE)
//  {
//  }
//  else if(sec<299)
//	  return FALSE;
//	else 
//  {
//  sec=0;
//  for(i=0;i<61;i++)
//	  pMe->Tempra_Data[i]=NULL;
//		pMe->temp_data_index=0;
//  pMe->Flag=FALSE;
//  
//  }

	  ISHELL_SetTimer(pMe->pIShell, 1000,pMe->Get_Current_TimerCB.pfnNotify, pMe);
  //	suryabrew_getcurrtime(pMe);
  DBGPRINTF("Return True");
//	GETJULIANDATE(pMe->CurrDateSec, &pMe->CurrDate);
//	DBGPRINTF("currentdate and time==> %d%d%d%d%d%d\n",pMe->CurrDate.wYear,pMe->CurrDate.wMonth,pMe->CurrDate.wDay,pMe->CurrDate.wHour,pMe->CurrDate.wMinute,pMe->CurrDate.wSecond);
 
//	return cur_time;
}


char* convertByte2Char(char *dest,byte *source,int start,int end)
{
	int i,j;
	if(start!=-1)
	{
		for(i=(start-1),j=0;i<end&&source[i]!='\0';i++,j++)
			dest[j]=(char)source[i];
		dest[j]='\0';
	}
	else
	{
		for(i=0;source[i]!='\0';i++)
			dest[i]=(char)source[i];
		dest[i]='\0';
	}
	return dest;
}

void suryabrew_setdata(suryabrew *pMe)
{
	int i;
	uint32 length=0;
	uint32 success=0;
//	uint32 getuploadtime;
//	char str[5];
//	uint32 year;
//	JulianType gettime;
	char year[4],month[2],day[2],hour[2],minute[2],second[2];
	length=STRLEN(DEPLOYMENT_ID);
	if(pMe->Deployment_Id==NULL)
	pMe->Deployment_Id=(char*)MALLOC((length+1)*sizeof(char));
	MEMSET(pMe->Deployment_Id,0,(length+1)*sizeof(char));
	STRCPY(pMe->Deployment_Id,DEPLOYMENT_ID);
	DBGPRINTF("deployment id==>%s",pMe->Deployment_Id);
//	length=STRLEN(pMe->currVolume);
	if(pMe->sound_volume==NULL)
	pMe->sound_volume=(char*)MALLOC(4*sizeof(char));
	MEMSET(pMe->sound_volume,0,4*sizeof(char));
	SPRINTF(pMe->sound_volume,"%d",pMe->currVolume);
	DBGPRINTF("soundvolume==>%s",pMe->sound_volume);
	//pMe->sound_volume=(char*)MALLOC((length+1)*sizeof(char));
//	char tmp[2];
//	pMe->averging_window=(char*)MALLOC(2* sizeof(char));
//	pMe->sample_duration=(char*)MALLOC(3* sizeof(char));
//		pMe->puase_interval=(char*)MALLOC(4* sizeof(char));
//		pMe->output_sound_volume=(char*)MALLOC(5* sizeof(char));
//		pMe->calibration_scalar=(char*)MALLOC(5* sizeof(char));
//		pMe->calibration_offset=(char*)MALLOC(10* sizeof(char));
//		pMe->calibration_devider=(char*)MALLOC(6* sizeof(char));
//	STRCPY(pMe->phone_id,"123456445");JulianType ct;
//    uint32 minute,sec;
//	 uint32 cur_time=GETTIMESECONDS();
//	int i;
//  GETJULIANDATE(cur_time, &ct);
//	STRCPY(pMe->version,"3.1.5");
	//for current configration parameter
	//copy into current configration parameter
//	MEMSET(pMe->averging_window,0,2*sizeof(char));
//	MEMSET(pMe->sample_duration,0,3*sizeof(char));
//	MEMSET(pMe->puase_interval,0,4*sizeof(char));
//	MEMSET(pMe->output_sound_volume,0,5*sizeof(char));
//	MEMSET(pMe->calibration_scalar,0,5*sizeof(char));
//	MEMSET(pMe->calibration_offset,0,10*sizeof(char));
//	MEMSET(pMe->calibration_devider,0,6*sizeof(char));
//	SPRINTF(pMe->averging_window,"%d",AVERGING_WINDOW);
//	STRCPY(pMe->averging_window,AVERGING_WINDOW);
//	SPRINTF(pMe->sample_duration,"%d",SAMPLE_DURATION);
//	STRCPY(pMe->sample_duration,"20");
//	SPRINTF(pMe->puase_interval,"%d",PUASE_INTERVAL);
//	STRCPY(pMe->puase_interval,"240");
//	SPRINTF(pMe->output_sound_volume,"%d",pMe->currVolume);
	//STRCPY(pMe->output_sound_volume,"50");
//	SPRINTF(pMe->calibration_scalar,"%d",CALIBRATION_SCALAR);
//	STRCPY(pMe->calibration_scalar,"3443");
//	SPRINTF(pMe->calibration_offset,"%d",CALIBRATION_OFFSET);
//	STRCPY(pMe->calibration_offset,"607860");
//	SPRINTF(pMe->calibration_devider,"%d",CALIBRATION_DIVIDER);
//	STRCPY(pMe->calibration_devider,"10000");

//		pMe->upload_curr_configration=(char*)MALLOC(50*sizeof(char));
/*	MEMSET(pMe->upload_curr_configration,0,50*sizeof(char));	STRCAT(pMe->upload_curr_configration,pMe->averging_window);
	STRCAT(pMe->upload_curr_configration,",");
	STRCAT(pMe->upload_curr_configration,pMe->sample_duration);
	STRCAT(pMe->upload_curr_configration,",");
	STRCAT(pMe->upload_curr_configration,pMe->puase_interval);
	STRCAT(pMe->upload_curr_configration,",");
	STRCAT(pMe->upload_curr_configration,pMe->output_sound_volume);
	STRCAT(pMe->upload_curr_configration,",");
	STRCAT(pMe->upload_curr_configration,pMe->calibration_scalar);
	STRCAT(pMe->upload_curr_configration,",");
	STRCAT(pMe->upload_curr_configration,pMe->calibration_offset);
	STRCAT(pMe->upload_curr_configration,",");
	STRCAT(pMe->upload_curr_configration,pMe->calibration_devider);
	DBGPRINTF("configration patameter==>%s",pMe->upload_curr_configration);
*/
	//set time
//	suryabrew_gettime(pMe);
//	getuploadtime=GETTIMESECONDS();
//	int i;
//	GETJULIANDATE(getuploadtime, &gettime);
//	DBGPRINTF("get time for upload==> %d%d%d%d%d%d\n",gettime.wYear,gettime.wMonth,gettime.wDay,gettime.wHour,gettime.wMinute,gettime.wSecond);
//	SPRINTF(*time[0],"%s",gettime.wYear);
//	char c[sizeof(JulianType)];
//	year=(uint32)gettime.wYear;
	if(pMe->upload_time==NULL)
	pMe->upload_time=(char*)MALLOC(14*sizeof(char));
	MEMSET(pMe->upload_time,0,14*sizeof(char));
	//set year
	SPRINTF(year,"%d",pMe->CurrDate.wYear);
	STRCAT(pMe->upload_time,year);
	//set year
//	time=NULL;
	//set month
	SPRINTF(month,"%d",pMe->CurrDate.wMonth);
	if(STRLEN(month)==2)
	{
		
	}
	else
	{
	MEMSET(month,0,sizeof(month));
	SPRINTF(month,"0%d",pMe->CurrDate.wMonth);
	}
	STRCAT(pMe->upload_time,month);
	//set month
//	time=NULL;
	//set day
	SPRINTF(day,"%d",pMe->CurrDate.wDay);
	if(STRLEN(day)==2)
	{
		
	}
	else
	{
		MEMSET(day,0,sizeof(day));
	SPRINTF(day,"0%d",pMe->CurrDate.wDay);
	}
	STRCAT(pMe->upload_time,day);
	//set day
//	time=NULL;
	//set hour
	SPRINTF(hour,"%d",pMe->CurrDate.wHour);
	if(STRLEN(hour)==2)
	{
		
	}
	else
	{
		MEMSET(hour,0,sizeof(hour));
	SPRINTF(hour,"0%d",pMe->CurrDate.wHour);
	}
	STRCAT(pMe->upload_time,hour);
	//set hour
//	time=NULL;
	//set minute
	SPRINTF(minute,"%d",pMe->CurrDate.wMinute);
	if(STRLEN(minute)==2)
	{
		
	}
	else
	{
		MEMSET(minute,0,sizeof(minute));
	SPRINTF(minute,"0%d",pMe->CurrDate.wMinute);
	}
	STRCAT(pMe->upload_time,minute);
	//set minute
//	time=NULL;
	//set second
	SPRINTF(second,"%d",pMe->CurrDate.wSecond);
		if(STRLEN(second)==2)
	{
		
	}
	else
	{
		MEMSET(second,0,sizeof(second));
	SPRINTF(second,"0%d",pMe->CurrDate.wSecond);
	}
	STRCAT(pMe->upload_time,second);
	//set second
	DBGPRINTF("upload time==>%s",pMe->upload_time);
	//set time
	
	
#ifdef AEE_SIMULATOR
//	SPRINTF(pMe->upload_time,"2011107121121");
	{
		for(i=0;i<60;i++)
		pMe->Tempra_Data[i]=25;
	}
	pMe->Tempt_Data_Lenght=400;
#endif
	{	if(pMe->upload_Temprature_Data==NULL)
		pMe->upload_Temprature_Data=(char*)MALLOC((pMe->Tempt_Data_Lenght+60)*sizeof(char));
	}
	
	MEMSET(pMe->upload_Temprature_Data,0,(pMe->Tempt_Data_Lenght+60)*sizeof(char));
//	for (i=0;i<=60;i++) {
//		SPRINTF(str,"%d",pMe->Tempra_Data[i]);
//		length+=STRLEN(str);
//		//length+=sizeof(pMe->Tempra_Data[i]);
//	}
		
	for(i=0;i<SAMPLE_DURATION;i++)
	{
	//	itoa(pMe->Tempra_Data[i],pMe->Temprature_Data[i],10);
	//	tmp[1]=(char)pMe->Tempra_Data[i];
	//	STRCPY(tmp,(char)pMe->Tempra_Data[i]);
	//	STRCAT(pMe->Temprature_Data,tmp[1]);
//	STRCAT(pMe->Temprature_Data,",");
//	tmp[1]=NULL;
	//	SPRINTF(length,"%d",pMe->Tempra_Data[i])
		char c[sizeof(uint32)];
		MEMSET(c,0,4*sizeof(char));
		success= SNPRINTF(c,4*sizeof(char),"%d",pMe->Tempra_Data[i]);
		DBGPRINTF("value of c ===?%s,%d",c,success);
		STRCAT(pMe->upload_Temprature_Data,c);
		DBGPRINTF("pme->upload_temprature====?>",pMe->upload_Temprature_Data);
		if(i!=SAMPLE_DURATION-1)
		STRCAT(pMe->upload_Temprature_Data,",");


	}
	DBGPRINTF("pme->upload_temprature_data%s",pMe->upload_Temprature_Data);
	pMe->is_data_upload=FALSE;
//	for(i=0;i<=60;i++)
//	{
//		sprintf(pMe->Temprature_Data,"%s",pMe->Tempra_Data[i]);
//		pMe->Temprature_Data;
//	}
	//for current 
//	for(i=0;i<=60;i++)
//	{
//		pMe->Tempra_Data[i]=20;
//		STRCPY(pMe->Temprature_Data[i],(char)pMe->Tempra_Data[i]);
//	//	STRCAT(pMe->Temprature_Data,(char)pMe->Tempra_Data[i]);
//				if(i!=60)
//				STRCAT(pMe->curr_configration_param,",");
//	}

}

void suryabrew_resetdata(suryabrew *pMe)
{
//	int i;
//
/*if(pMe->averging_window!=NULL)
{
	FREE(pMe->averging_window);
}
if(pMe->sample_duration!=NULL)
{
	FREE(pMe->sample_duration);
}
if(pMe->puase_interval!=NULL)
{
	FREE(pMe->puase_interval);
}
if(pMe->output_sound_volume!=NULL)
{
	FREE(pMe->output_sound_volume);
}

if(pMe->calibration_scalar!=NULL)
{
	FREE(pMe->calibration_scalar);
}
if(pMe->calibration_offset!=NULL)
{
	FREE(pMe->calibration_offset);
}
if(pMe->calibration_devider!=NULL)
{
	FREE(pMe->calibration_devider);
}*/

	DBGPRINTF("free output sound");
if(pMe->sound_volume!=NULL)
{

	FREE(pMe->sound_volume);
	pMe->sound_volume=NULL;
}

DBGPRINTF("free deployment id");
if(pMe->Deployment_Id!=NULL)
{
	FREE(pMe->Deployment_Id);
	pMe->Deployment_Id=NULL;
}

DBGPRINTF("free upload time");

if(pMe->upload_time!=NULL)
{
	FREE(pMe->upload_time);
	pMe->upload_time=NULL;
}
DBGPRINTF("free data");
DBGPRINTF("in free data%s",pMe->upload_Temprature_Data);
if(pMe->upload_Temprature_Data!=NULL)  // will be check
{

	FREE(pMe->upload_Temprature_Data);
	pMe->upload_Temprature_Data=NULL;

}
/*if(pMe->pipPostData!=NULL)
{
	IPEEK_Release(pMe->pipPostData);
	pMe->pipPostData=NULL;
}
*/
//	for (i=0;i<) {
//	}
DBGPRINTF("free  upload data");
}

void suryabrew_Get_Min_Number(suryabrew *pMe)
{
	TAPIStatus t_status;
	int length=0;
	uint8 language=0;
	char str25[25];		
	int16 iLoop=0, i=0, jLoop=0;
	if(pMe->pITAPI==NULL)
	ISHELL_CreateInstance(pMe->pIShell, AEECLSID_TAPI,(void **)&pMe->pITAPI);

	if(pMe->pITAPI!=NULL)
	{
		ITAPI_GetStatus(pMe->pITAPI,&t_status);
	}
		
	////////
	emptychararray(str25, 25);
	copy_char_to_string(str25, t_status.szMobileID);
	DBGPRINTF("t_status.szMobileID%s",t_status.szMobileID);
	
	//404009223270103
	
#ifdef AEE_SIMULATOR
	{
		copy_char_to_string(str25, "401000101010");
	}

	
	while(str25[iLoop] != '\0')
	{
		iLoop++;
	}
	
	//iLoop--;
	
	
	if (iLoop > 10)
	{
		for ((i = iLoop - 14), jLoop = 0; i <= iLoop; i++, jLoop++)
		{
			str25[jLoop] = str25[i];
		}
	}
	#endif
	length=STRLEN(str25);
	pMe->min_number=(char*)MALLOC((length+1)*sizeof(char));
	MEMSET(pMe->min_number,0,(length+1)*sizeof(char));
	STRCPY(pMe->min_number,str25);
	DBGPRINTF("DEVICE MIN NO.==>%S",pMe->min_number);
	//emptychararray(pMe->min_number,sizeof(pMe->min_number));
//	copy_char_to_string(pMe->min_number,str25);
//	DBGPRINTF("min_number%s",pMe->min_number);
//	if ((pMe->pITAPI)) {
//
//		ITAPI_Release(pMe->pITAPI);
//		pMe->pITAPI=NULL;
//	}
}

void copy_char_to_string(char *dest, const char *source)
{
	while (*dest != '\0')
	{
		dest++;
	}
	
	while(*source!='\0')
	{
		*dest = *source;
		dest++;
		source++;
	}
	*dest = '\0';
}
void emptychararray(char *str, uint32 length)
{
	uint32 iLoop=0;
	for (iLoop=0; iLoop < length; iLoop++)
	{
		str[iLoop] = (char) '\0';
	}
}

void suryabrew_uploaddata_notify(suryabrew *pMe)
{
		IDBRecord *pRecord = NULL;
	//	IDATABASE_Reset(pMe->pIDatabase);
	if((suryabrew_DBDEBUG(pMe)) !=0)
	{
		DBGPRINTF("database is not null");
	//	IDBRECORD_Release(pRecord);
		suryabrew_UPDATATimer(pMe);
	}
	else if (pMe->is_database_null==TRUE)
	{
		DBGPRINTF("remove database");
		suryabrew_Remove_DataBase(pMe);
	}
	else 
	{
		DBGPRINTF("CALL UPLOAD TIMER");
		ISHELL_SetTimer(pMe->pIShell, 8000,pMe->UPIMGTimerCB.pfnNotify, pMe);
	}
}

/*char suryabrew_concret_time(suryabrew *pMe){

	JulianType CurrDate;
	char builddate[3];
	uint32 CurrDateSec = GETTIMESECONDS();
	GETJULIANDATE(CurrDateSec, &CurrDate);
	DBGPRINTF("build date==> %d%d%d%d%d%d\n",CurrDate.wYear,CurrDate.wMonth,CurrDate.wDay,CurrDate.wHour,CurrDate.wMinute,CurrDate.wSecond);
	MEMSET(builddate,0,sizeof(builddate));
	sprintf(builddate,"%d-%d-%d",CurrDate.wYear,CurrDate.wMonth,CurrDate.wDay);

	return builddate;

}*/