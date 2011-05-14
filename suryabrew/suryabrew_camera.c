
#include "suryabrew.h"




/***********************************************
* CAMERA INTERFACE AND OPERATION
************************************************/


void suryabrew_CameraNotifyImage(void * pUser, IImage * po, AEEImageInfo * pi, int nErr)
{
	AEESize p1 = {0};
	AEESize p2 = {0};

	suryabrew* pMe = (suryabrew *) pUser;
	DBGPRINTF("CameraNotifyImage Decode complete %d", nErr);
	//if (nErr == 0)
	{
		if (pi != NULL)
		{
			DBGPRINTF("CameraNotifyImage cx %d cy %d color %d anim %d frame %d",
				pi->cx, pi->cy, pi->nColors, pi->bAnimated, pi->cxFrame);
		}
		
		nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_DISPLAY_SIZE, (void *) &p1, (void *) &p2);
		//IIMAGE_SetParm(po, IPARM_SCALE, pMe->m_cxWidth, pMe->m_cyHeight - SURYABREW_TITLEBAR_HEIGHT);
		IIMAGE_SetParm(po, IPARM_SCALE, pMe->m_cxWidth, p1.cy);

		IIMAGE_Draw(po, 0, SURYABREW_TITLEBAR_HEIGHT);
		IDISPLAY_Update(pMe->pIDisplay);
		suryabrew_DrawCheckYesNo(pMe);

	}
}

void suryabrew_CameraDropImage(suryabrew* pMe)
{
	if (pMe->SuryaMode == SURYAMODE_CAMERA)
	{
		suryabrew_FileDeleteFile(pMe, pMe->ImageFilename);
	}
}


void suryabrew_CameraNotify(void * pUser, AEECameraNotify * pNotify)
{
	int err = 0;
	suryabrew *pMe = (suryabrew *) pUser;
	if (!pMe || !pNotify)
	{
		return;
	}

	if (pNotify->nStatus != CAM_STATUS_FRAME)
	{
		DBGPRINTF("CamStatNot nStat: %d nCmd: %d nSubCmd: %d", pNotify->nStatus, pNotify->nCmd, pNotify->nSubCmd);
	}


	switch (pNotify->nStatus)
	{
	case CAM_STATUS_START:

		DBGPRINTF("CamNot CAM_STATUS_START");
		if(pNotify->nSubCmd == CAM_MODE_PREVIEW)
		{
			DBGPRINTF("MODE_PREVIEW");
			suryabrew_DrawPhotoScreen(pMe);
		}
		if(pNotify->nSubCmd == CAM_MODE_SNAPSHOT)
		{
			DBGPRINTF("MODE_SNAPSHOT");
		}
		if(pNotify->nCmd == CAM_CMD_SETPARM)
		{
			DBGPRINTF("MODE_SETPARAM");
		}
		break;

	case CAM_STATUS_FRAME:
		{

			IBitmap *pFrame = NULL;
			AEEBitmapInfo bi = {0};

			//DBGPRINTF("CameraNotify CAM_STATUS_FRAME");

			//
			// IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
			//
			if (ICAMERA_GetFrame(pMe->pICamera, &pFrame) != SUCCESS)
			{
				return;
			}

			// Get the bitmap info...this can be saved in app global structure.
			IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

			// Display the frame at (0, 0) location of the screen
			IDISPLAY_BitBlt(pMe->pIDisplay, 0, SURYABREW_TITLEBAR_HEIGHT, bi.cx, bi.cy, pFrame, 0, 0, AEE_RO_COPY);
			IDISPLAY_Update(pMe->pIDisplay);
			IBITMAP_Release(pFrame);

			break;
		}

	case CAM_STATUS_DONE:
		// For RecordSnapShot, pData = TRUE/FALSE => Defered encode enabled/disabled
		// For EncodeSnapshot, pData = Number of bytes encoded
		// ICAMERA_Stop() was called and preview operation stopped.

		DBGPRINTF("CamNot CAM_STATUS_DONE %d",  ((int32*) pNotify->pData));

		//nCmd = 3 (CAM_CMD_START), subcmd = 3 (CAM_MODE_SNAPSHOT) 
		// DO NOTHING ... first phase of snapshot finishing 
		// If we had called deffered encode, this is where we would call encodesnapshot()

		//nCmd = 3 (CAM_CMD_START), subcmd = 2 (CAM_MODE_PREVIEW)
		if (pNotify->nCmd == CAM_CMD_START && pNotify->nSubCmd == CAM_MODE_PREVIEW)
		{
			// Called when coming from Stop command called during preview mode
			DBGPRINTF("CameraNotify preview stopped");
			pMe->CameraMode = CAM_MODE_READY;
			suryabrew_CameraInitSnapshot(pMe);
		}

		// nCmd = 4 (CAM_CMD_ENCODESNAPSHOT), subcmd = 0
		// second phase of snapshot finishing
		if (pNotify->nCmd == CAM_CMD_ENCODESNAPSHOT)
		{
			DBGPRINTF("CameraNotify encoding complete");
			pMe->CameraMode = CAM_MODE_READY;

			// Display the image
			if (pMe->pIImage != NULL)
			{
				IIMAGE_Release(pMe->pIImage);
				pMe->pIImage = NULL;
			}
			pMe->pIImage = ISHELL_LoadImage(pMe->pIShell, pMe->ImageFilename);
			if (pMe->pIImage != NULL)
			{
				IIMAGE_Notify(pMe->pIImage, suryabrew_CameraNotifyImage, pMe);
			}
			else 
			{
				DBGPRINTF("CameraNotify Unable to load image");
			}
		}
		break;

	case CAM_STATUS_PAUSE:
		DBGPRINTF("CameraNotify CAM_STATUS_PAUSE");
		break;
	case CAM_STATUS_RESUME:
		DBGPRINTF("CameraNotify CAM_STATUS_PAUSE");
		// AEECameraNotify::pData is IBitmap pointer representing the snapshot
		break;
	case CAM_STATUS_DATA_IO_DELAY:
		DBGPRINTF("CameraNotify CAM_STATUS_DATA_IO_DELAY");
		break;
	case CAM_STATUS_SPACE_WARNING:
		DBGPRINTF("CameraNotify CAM_STATUS_SPACE_WARNING");
		break;
	case CAM_STATUS_FAIL:
		pMe->CameraMode = CAM_MODE_READY;
		DBGPRINTF("CameraNotify CAM_STATUS_FAIL");
		break;
	case CAM_STATUS_ABORT:
		pMe->CameraMode = CAM_MODE_READY;
		DBGPRINTF("CameraNotify CAM_STATUS_ABORT");
		// Preview got aborted.
		break;
	case CAM_STATUS_SPACE_ERROR:
		DBGPRINTF("CameraNotify CAM_STATUS_SPACE_ERROR");
		// [Record] Memory unavailable to store recording
		break;
	case CAM_STATUS_FILE_SIZE_EXCEEDED:
		DBGPRINTF("CameraNotify CAM_STATUS_FILE_SIZE_EXCEEDED");
		// [EncodeSnapshot/RecordMovie] Operation completed but output encoded image exceeds specified file size
		break;
	case CAM_STATUS_IO_ERROR:
		DBGPRINTF("CameraNotify CAM_STATUS_IO_ERROR %d", pNotify->pData);
		// [EncodeSnapshot/RecordMovie] I/O Error, pData contains CAM_ERR_XXX
		break;
	default:
		break;
	}


}




int suryabrew_CameraInitSnapshot(suryabrew* pMe)
{
	int nErr = 0;
	int p1 = 0;

	boolean bRange = FALSE;
	AEESize *pSizeList = NULL;


	AEESize previewSize = {0};
	AEESize previewSize2 = {0};
	AEEParmInfo parmInfo = {0};


	// Get the display sizes ... seems unsupported in my phones
	pSizeList = (AEESize *)CAM_MODE_SNAPSHOT;
	nErr = ICAMERA_GetDisplaySizeList(pMe->pICamera, &pSizeList, &bRange);
	DBGPRINTF("GetDisplaySizeList: %d %d", nErr, bRange);

	if (nErr == SUCCESS)
	{
		p1 = 0;
		while(pSizeList[p1].cx != 0 | pSizeList[p1].cy != 0)
		{
			DBGPRINTF("%d x %d", pSizeList[p1].cx, pSizeList[p1].cy);
		}
	} 

	// get the display size
	nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_DISPLAY_SIZE, (void *) &previewSize, (void *) &previewSize2);
	DBGPRINTF("CamInitSnap GETDisplaySize: %dx%d %dx%d - %d",
		previewSize.cx, previewSize.cy, previewSize2.cx, previewSize2.cy, nErr);

	previewSize.cx  = pMe->m_cxWidth;
	//previewSize.cy  = pMe->m_cyHeight;// - SURYABREW_TITLEBAR_HEIGHT;

	nErr = ICAMERA_SetDisplaySize( pMe->pICamera, &previewSize );
	DBGPRINTF("CamInitSnap SetDisplaySize: %dx%d - %d", previewSize.cx, previewSize.cy, nErr);

	previewSize.cx = 0;
	previewSize.cy = 0;

	nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_SIZE, (void *) &previewSize, (void *) &previewSize2);
	DBGPRINTF("CamInitSnap GETSize: %dx%d %dx%d - %d",
		previewSize.cx, previewSize.cy, previewSize2.cx, previewSize2.cy, nErr);

	pMe->CurrDateSec = GETTIMESECONDS();
	GETJULIANDATE(pMe->CurrDateSec, &pMe->CurrDate);

	// Assumes CurrDate set!
	suryabrew_FileCreateDirPrefixDate(pMe);
#ifdef AEE_SIMULATOR
	SPRINTF(pMe->ImageFilename, "%s%d%02d%02d%02d%02d%02d_%s.bmp", pMe->FileNamePrefix, pMe->CurrDate.wYear, pMe->CurrDate.wMonth, pMe->CurrDate.wDay,
		pMe->CurrDate.wHour, pMe->CurrDate.wMinute, pMe->CurrDate.wSecond, pMe->HWID);
#else
	SPRINTF(pMe->ImageFilename, "%s%d%02d%02d%02d%02d%02d_%s.jpg", pMe->FileNamePrefix, pMe->CurrDate.wYear, pMe->CurrDate.wMonth, pMe->CurrDate.wDay,
		pMe->CurrDate.wHour, pMe->CurrDate.wMinute, pMe->CurrDate.wSecond, pMe->HWID);
#endif
	DBGPRINTF("to: %s", pMe->ImageFilename);

	pMe->MediaData.clsData = MMD_FILE_NAME;
	pMe->MediaData.pData = pMe->ImageFilename;
	pMe->MediaData.dwSize = 0;
#ifdef AEE_SIMULATOR
	nErr = ICAMERA_SetMediaData(pMe->pICamera, &pMe->MediaData, MT_BMP);
#else
	nErr = ICAMERA_SetMediaData(pMe->pICamera, &pMe->MediaData, MT_JPEG);
#endif
	DBGPRINTF("CameSnap SetMediaData: %d", nErr);

#ifdef AEE_SIMULATOR
	nErr = ICAMERA_SetVideoEncode(pMe->pICamera, CAM_ENCODE_RAW, 0);
#else
	nErr = ICAMERA_SetVideoEncode(pMe->pICamera, AEECLSID_JPEG, 0);
#endif
	DBGPRINTF("CamInitSnap SetVideoEncode: %d", nErr);

	// Get the max size

	pSizeList = (AEESize *)CAM_MODE_SNAPSHOT;
	nErr = ICAMERA_GetSizeList(pMe->pICamera, &pSizeList, &bRange);
	DBGPRINTF("GetSizeList: %d %d", nErr, bRange);

	if (nErr == SUCCESS)
	{
		p1 = 0;
		while(pSizeList[p1].cx != 0 | pSizeList[p1].cy != 0)
		{
			DBGPRINTF("%d x %d", pSizeList[p1].cx, pSizeList[p1].cy);
		}
	} 
	//else
	//{
#ifdef AEE_SIMULATOR
		previewSize.cx = 32; //160; //640; //1280;
		previewSize.cy = 20; //120; //480; //960;
#else
		previewSize.cx = 1280; //160; //640; //1280;
		previewSize.cy = 960; //120; //480; //960;
	//}
#endif
	nErr = ICAMERA_SetSize(pMe->pICamera, &previewSize);
	DBGPRINTF("CamSnap SetSize: %d", nErr);

	p1 = 0;
	nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_QUALITY, (void *) &p1, (void *) &parmInfo);
	DBGPRINTF("CamInitSnap GETQuality: %d %d %d %d %d %d - %d",
		p1, parmInfo.nCurrent, parmInfo.nDefault, parmInfo.nMax, parmInfo.nMin, parmInfo.nStep, nErr);

	//nErr = ICAMERA_SetQuality();

	// Disabled by default
	// ICAMERA_DeferEncode(pMe->pICamera, FALSE);

	pMe->CameraMode = CAM_MODE_SNAPSHOT;

	nErr = ICAMERA_RecordSnapshot(pMe->pICamera);
	DBGPRINTF("CamInitSnap RecordSnapshot: %d", nErr);

	return nErr;
}


int suryabrew_CameraStop(suryabrew* pMe)
{
	int nErr = 0;
	nErr = ICAMERA_Stop(pMe->pICamera);
	DBGPRINTF("CameraStop: %d", nErr);
	return nErr;
}

int suryabrew_CameraInitPreview(suryabrew* pMe)
{
	//uint32 p2[10] = {0,0,0,0,0,0,0,0,0,0};
	//boolean b = FALSE;

	int nErr = 0;
	int p1 = 0;

	AEESize previewSize = {0};
	AEESize previewSize2 = {0};
	AEEParmInfo parmInfo = {0};

	MEMSET(pMe->ImageFilename, 0, sizeof(pMe->ImageFilename));

	nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_DISPLAY_SIZE, (void *) &previewSize, (void *) &previewSize2);
	DBGPRINTF("CmInPrv GETDisplaySize: %dx%d %dx%d - %d",
		previewSize.cx, previewSize.cy, previewSize2.cx, previewSize2.cy, nErr);

	previewSize.cx  = pMe->m_cxWidth;
	//previewSize.cy  = pMe->m_cyHeight;// - SURYABREW_TITLEBAR_HEIGHT;

	nErr = ICAMERA_SetDisplaySize( pMe->pICamera, &previewSize );
	DBGPRINTF("CmInPrv SetDisplaySize: %dx%d - %d", previewSize.cx, previewSize.cy, nErr);

	nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_DISPLAY_SIZE, (void *) &previewSize, (void *) &previewSize2);
	DBGPRINTF("CmInPrv GETDisplaySize: %dx%d %dx%d - %d",
		previewSize.cx, previewSize.cy, previewSize2.cx, previewSize2.cy, nErr);

	nErr = ICAMERA_SetSize(pMe->pICamera, &previewSize);
	DBGPRINTF("CmInPrv SetSize: %d", nErr);

	nErr = ICAMERA_SetVideoEncode(pMe->pICamera, CAM_ENCODE_RAW, 0);
	DBGPRINTF("CmInPrv SetVideoEncode: %d", nErr);



	/*
	CAM_PARM_FPS
	Set:  Sets the frames per second of the camera preview and movie modes. This is done in Ready
	mode before calling ICAMERA_Preview() or ICAMERA_RecordMovie().
	p1 = uint32 dwFPS value. 
	dwFPS format: Lower 16 bits is Numerator. 
	Upper 16 bits is Denominator. Zero denominator is treated as 1.

	Get:  Gets the current FPS.
	p1 = Pointer to FPS value
	*/
	//p1 = (1 << 16) | 1;
	//nErr = ICAMERA_SetParm(pMe->pICamera, CAM_PARM_FPS, &p1, NULL);
	//DBGPRINTF("CameraInitPreview SETFPS: %x %d", p1, nErr);


	nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_FPS, &p1, NULL);
	DBGPRINTF("CmInPrv GETFPS: %d %d", p1, nErr);

	
	/*
	CAM_PARM_FPS_LIST
	Get:  Returns list of supported discrete frames per second(FPS) or continuos range 

	(e.g. any size between 5 to 30) for specified mode.
	Input: p1 = CAM_MODE_PREVIEW/CAM_MODE_MOVIE (*ppList must contain this value)
	Output: p1 = Pointer to NULL-terminated list of uint32 dwFPS (ppList of type uint32 **).
	See dwFPS format in CAM_PARM_FPS documentation.
	If NULL, indicates that any value is supported.
	p2 = Pointer to boolean, bRange, when TRUE indicates the passed list 
	is a NULL-terminated paired list (i.e. multiple of 2) of ranges
	Note: The list should be copied and should not be freed.
	*/
	//p2[0] = CAM_MODE_PREVIEW;
	//nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_FPS_LIST, &p2, &b);
	//p1 = 0;
	//DBGPRINTF("CameraInitPreview GETFPSList: %d %d", nErr, b);
	//while (p2[p1] != 0)
	//{
	//	DBGPRINTF("CameraInitPreview GETFPSList: %x %d / %d", p2[p1], p2[p1] & 0x0011, p2[p1] >> 16);
	//	p1 += 1;
	//}
					 

	//nErr = ICAMERA_SetParm(pMe->pICamera, CAM_PARM_PREVIEW_TYPE, CAM_PREVIEW_SNAPSHOT, 0);
	//nErr = ICAMERA_SetFramesPerSecond( pMe->pICamera, 5);
	//DBGPRINTF("CameraInit SetFraMesPerSecond: %d", nErr);

	// Setup extra params
	//nErr = ICAMERA_SetParm( pMe->pICamera,
	//	CAM_PARM_FLASH,
//		CAM_FLASH_AUTO,
	//	0);


	nErr = ICAMERA_Preview(pMe->pICamera);
	DBGPRINTF("CameraInit Preview: %d", nErr);

	if (nErr == 0)
	{
		pMe->CameraMode = CAM_MODE_PREVIEW;
	}


	return nErr;

}



int suryabrew_CameraLoad(suryabrew* pMe)
{

	int nErr = 0;

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CAMERA, (void**) &pMe->pICamera);
	DBGPRINTF("CameraInit CreateInstance: %d", nErr);

	if (nErr)
	{
		return nErr;
	}

	nErr = ICAMERA_RegisterNotify( pMe->pICamera, suryabrew_CameraNotify, pMe );
	DBGPRINTF("CameraInit RegisterNotify: %d", nErr);
	if (nErr)
	{
		return nErr;
	}

	pMe->CameraMode = CAM_MODE_READY;

	return 0;
}

void suryabrew_CameraUnload(suryabrew *pMe)
{

	if ( pMe->pICamera != NULL)
	{
		suryabrew_CameraDropImage(pMe); // if suspended during camera mode, delete
		ICAMERA_Release(pMe->pICamera);
		pMe->pICamera = NULL;
		pMe->CameraMode = 0;
	}

	DBGPRINTF("Unload Camera Image");
	if ( pMe->pIImage != NULL)
	{
		IIMAGE_Release(pMe->pIImage);
		pMe->pIImage = NULL;
	}

}