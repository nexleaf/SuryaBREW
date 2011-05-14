

#include "suryabrew.h"



static void suryabrew_UPIMGHeaderNotification(void *p, const char *cpszName, GetLine *pglVal)
{
	if (cpszName != NULL) {
		DBGPRINTF("UPIMG HEADER PRINT %s %s\n", cpszName, pglVal->psz);
   }
}


void suryabrew_UPIMGStatusNotification(void *pNotifyData, WebStatus ws, void *pData)
{
   switch (ws) {
   case WEBS_CANCELLED:
	  DBGPRINTF("UPIMGStatus ** cancelled...\n");
      break;
   case WEBS_GETHOSTBYNAME:
      DBGPRINTF("UPIMGStatus ** finding host...\n");
      break;
   case WEBS_CONNECT:
      DBGPRINTF("UPIMGStatus ** connecting...\n");
      break;
   case WEBS_SENDREQUEST:
      DBGPRINTF("UPIMGStatus ** sending...\n");
      break;
   case WEBS_READRESPONSE:
      DBGPRINTF("UPIMGStatus ** receiving...\n");
      break;
   case WEBS_GOTREDIRECT:
      DBGPRINTF("UPIMGStatus ** redirect...\n");
      break;
   case WEBS_CACHEHIT:
      DBGPRINTF("UPIMGStatus ** cache hit...\n");
      break;
   }

}


void suryabrew_UPIMGPostDone(void *pData)
{
	int nErr = 0;
	int bytes = 0;
	char buf[32] = {0};
	boolean success = FALSE;
	suryabrew *pMe = (suryabrew *) pData;

	WebRespInfo *respi = IWEBRESP_GetInfo(pMe->pIWebResp);
	pMe->UPIMGStopTime = GETTIMESECONDS();

	DBGPRINTF("UPIMGPostDone Code %d len %d exp %d mod %d TIME: %d",
		respi->nCode, respi->lContentLength, respi->tExpires,
		respi->tModified, pMe->UPIMGStopTime - pMe->UPIMGStartTime);

	// Return codes are a number in the document body
	// upok  means everything worked
	// dupok  means already have image
	// o.w. error

	// The IGetLine way seem too complicated, doing the simple way
	if (WEB_ERROR_SUCCEEDED(respi->nCode))
	{

		bytes = ISOURCE_Read(respi->pisMessage, buf, sizeof(buf));
		switch (bytes)
		{
		case ISOURCE_WAIT:
			DBGPRINTF("UPIMGPostDone need to wait on data, rerunning callback on readable");
			// since we have not allocated anything, then this should be fine to do
			ISOURCE_Readable(respi->pisMessage, &pMe->UPIMGPostCB);
			return;
		case ISOURCE_ERROR:
			DBGPRINTF("UPIMGPostDone Error reading document cbody");
			success = FALSE;
			break;
		case ISOURCE_END:
			DBGPRINTF("UPIMGPostDone Read complete document");
		default:
			DBGPRINTF("UPIMGPostDone Read Document (not all)");
			if (bytes)
			{
				char *res = STRSTR(buf, "upok");
				if (res != NULL)
				{
					success = TRUE;
				}
				res = STRSTR(buf, "dupok");
				if (res != NULL)
				{
					success = TRUE;
				}
			}
		}

	}

	// update or delete the entry about this upload being successfull
	if (success)
	{
		DBGPRINTF("UPIMGPostDone GOT SUCCESS!!!!");
		if (pMe->do_upload_delete_file)
		{
			// Call DB delete code which deletes the files as well
		} 
		else 
		{
			suryabrew_DBSetUploaded(pMe);
		}
	} 
	else 
	{
		DBGPRINTF("UPIMGPostDone GOT FAILURE!!!!");
	}


	FREE(pMe->poststr);
	FREE(pMe->poststrFooter);
	pMe->poststr = NULL;
	pMe->poststrFooter = NULL;
	pMe->DBUploadRecord = -1;


	//if (pMe->pIMultiPeek != NULL)
	//{
	//	IMULTIPEEK_Release(pMe->pIMultiPeek);
	//	pMe->pIMultiPeek = NULL;
	//}
	if (pMe->pIWebResp != NULL)
	{
		IWEBRESP_Release(pMe->pIWebResp);
		pMe->pIWebResp = NULL;
	}
	//if (pMe->pIGetLine != NULL)
	//{
	//	IGETLINE_Release(pMe->pIGetLine);
	//	pMe->pIGetLine = NULL;
	//}

//	if (success)
//	{
		nErr = ISHELL_SetTimer(pMe->pIShell, SURYABREW_UPIMG_UPLOAD_INTERVAL,
			pMe->UPIMGTimerCB.pfnNotify, pData);
		DBGPRINTF("UPIMGTIME set timer: %d", nErr);
//	}

}



void suryabrew_UPIMGTimer(void *pData)
{
	int nErr = 0;
	//int sIn = 0;
	//int sOut = 0;
	int size = 0;
	JulianType jd = {0};
	IFileMgr *pIFileMgr = NULL;
	IFile *pIFile = NULL;
	suryabrew *pMe = (suryabrew *) pData;
	FileInfo *pFileInfo = NULL;
	AECHAR latBuf[32] = {0};
	AECHAR lonBuf[32] = {0};

	suryabrew_uploaddata *ud = (suryabrew_uploaddata *) MALLOC(sizeof(suryabrew_uploaddata));

	if (ud == NULL)
	{
		DBGPRINTF("Unable to allocated space for upload data!!!!");
		return;
	}

	if (suryabrew_DBGetNotUploaded(pMe, ud) != SUCCESS)
	{
		nErr = ISHELL_SetTimer(pMe->pIShell, SURYABREW_UPIMG_UPLOAD_INTERVAL,
			pMe->UPIMGTimerCB.pfnNotify, pData);
		DBGPRINTF("UPIMGTIME set timer: %d", nErr);
		pMe->DBUploadRecord = -1;
		FREE(ud);
		return;
	} 

	// First setup and encode the post string

	pMe->DBUploadRecord = ud->DBRecordID;
	pMe->poststr = MALLOC(sizeof(char) * 4096);
	pMe->poststrFooter = MALLOC(sizeof(char) * 128);

	GETJULIANDATE(ud->datetime, &jd);


	FLOATTOWSTR(ud->lat, latBuf, 32);
	FLOATTOWSTR(ud->lon, lonBuf, 32);


	// Should put in json and have it as one post var so have only two multipart sections instead of 6
	SPRINTF(pMe->poststr, 
		//"phoneid=%s&datetime=%d%02d%02d%02d%02d%02d&lat=%lf&lon=%lf&alt=%d&hasgps=%d\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"phoneid\"\r\n\r\n"
		"%s\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"datetime\"\r\n\r\n"
		"%d%02d%02d%02d%02d%02d\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"lat\"\r\n\r\n"
		"%S\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"lon\"\r\n\r\n"
		"%S\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"alt\"\r\n\r\n"
		"%d\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"hasgps\"\r\n\r\n"
		"%d\r\n"
		"--%s\r\n"
		"Content-Disposition: form-data; name=\"image_file\"; filename=\"%s\"\r\n"
		"Content-Type: application/octet-stream\r\n\r\n",
		SURYABREW_MULTIPART_BOUNDRY,
		pMe->HWID,
		SURYABREW_MULTIPART_BOUNDRY,
		jd.wYear, jd.wMonth, jd.wDay, jd.wHour, jd.wMinute, jd.wSecond,
		SURYABREW_MULTIPART_BOUNDRY,
		latBuf,
		SURYABREW_MULTIPART_BOUNDRY,
		lonBuf,
		SURYABREW_MULTIPART_BOUNDRY,
		ud->alt,
		SURYABREW_MULTIPART_BOUNDRY,
		ud->hasgps,
		SURYABREW_MULTIPART_BOUNDRY,
		STRRCHR(ud->imagefile, '/') + 1);

	// make the footer
	SPRINTF(pMe->poststrFooter, "\r\n--%s--\r\n", SURYABREW_MULTIPART_BOUNDRY);

	// create the multipart post
	nErr = ISHELL_CreateInstance( pMe->pIShell, AEECLSID_MULTIPEEK, (void **)&pMe->pIMultiPeek );
	if (nErr != SUCCESS)
	{
		DBGPRINTF("UPIMGT Unable to init MULTIPEEK");
	}

	// the string parts
	nErr = IMULTIPEEK_AddMemorySource(pMe->pIMultiPeek, (void *) pMe->poststr, STRLEN(pMe->poststr));
	DBGPRINTF("UPIMGT Mpeek AddMemSrc %d %d %s", nErr, STRLEN(pMe->poststr), pMe->poststr);

	// the file parts
	nErr = ISHELL_CreateInstance( pMe->pIShell, AEECLSID_FILEMGR, (void **) &pIFileMgr);
	if (nErr != SUCCESS)
	{
		DBGPRINTF("UPIMGT Unable to init FileMGR");
	}
	pIFile = IFILEMGR_OpenFile(pIFileMgr, ud->imagefile, _OFM_READ);
	nErr = IMULTIPEEK_AddFileSource(pMe->pIMultiPeek, pIFile);

	// Get the file size
	pFileInfo = (FileInfo *) MALLOC(sizeof(FileInfo));
	IFILE_GetInfo(pIFile, pFileInfo);
	DBGPRINTF("UPIMGT Mpeek AddFileSrc %d %d %s", nErr, pFileInfo->dwSize, ud->imagefile);
	IFILE_Release(pIFile); pIFile = NULL;
	IFILEMGR_Release(pIFileMgr); pIFileMgr = NULL;

	// Add the footer
	nErr = IMULTIPEEK_AddMemorySource(pMe->pIMultiPeek, (void *) pMe->poststrFooter, STRLEN(pMe->poststrFooter));
	DBGPRINTF("UPIMGT Mpeek AddMemSrc %d %d %s", nErr, STRLEN(pMe->poststrFooter), pMe->poststrFooter);

	// how start the upload
	pMe->UPIMGStartTime = GETTIMESECONDS();
	IWEB_GetResponse(pMe->pIWeb,
		(pMe->pIWeb, &pMe->pIWebResp, &pMe->UPIMGPostCB, SURYABREW_UPLOAD_LOC,
		WEBOPT_HANDLERDATA, pMe,
		WEBOPT_HEADER, SURYABREW_MULTIPART_CONTENTTYPE,
		WEBOPT_HEADERHANDLER, suryabrew_UPIMGHeaderNotification,
		WEBOPT_STATUSHANDLER, suryabrew_UPIMGStatusNotification,
		WEBOPT_METHOD, "POST",
		WEBOPT_BODY, pMe->pIMultiPeek,
		WEBOPT_CONTENTLENGTH, STRLEN(pMe->poststr) + pFileInfo->dwSize + STRLEN(pMe->poststrFooter),
		WEBOPT_END));

	if (pFileInfo != NULL)
	{
		FREE(pFileInfo);
	}
	if (ud != NULL)
	{
		FREE(ud);
	}
}



void suryabrew_UPIMGInit(suryabrew *pMe)
{
	int nErr = 0;
	nErr = ISHELL_CreateInstance( pMe->pIShell, AEECLSID_WEB, (void **)&pMe->pIWeb );
	if (nErr != SUCCESS)
	{
		DBGPRINTF("UPIMGGPSInit Unable to init IWeb");
	}
	pMe->DBUploadRecord = -1;

	nErr = ISHELL_CreateInstance( pMe->pIShell, AEECLSID_WEBUTIL, (void **)&pMe->pIWebUtil );
	if (nErr != SUCCESS)
	{
		DBGPRINTF("UPIMGInit Unable to init IWeb");
	}


	// Some options?


	CALLBACK_Init(&pMe->UPIMGPostCB, suryabrew_UPIMGPostDone, pMe);
	CALLBACK_Init(&pMe->UPIMGTimerCB, suryabrew_UPIMGTimer, pMe);
	suryabrew_UPIMGTimer(pMe);

}

void suryabrew_UPIMGUnload(suryabrew *pMe)
{
	if (pMe->do_upload)
	{
		if ( pMe->pIWeb != NULL)
		{
			suryabrew_UPIMGStop(pMe);
			IWEB_Release(pMe->pIWeb);
			pMe->pIWeb = NULL;
		}
	}
}

void suryabrew_UPIMGStop(suryabrew *pMe)
{


	CALLBACK_Cancel(&pMe->UPIMGPostCB);
	CALLBACK_Cancel(&pMe->UPIMGTimerCB);
	ISHELL_CancelTimer(pMe->pIShell, pMe->UPIMGTimerCB.pfnNotify, pMe);

	if (pMe->pIWebResp != NULL)
	{
		IWEBRESP_Release(pMe->pIWebResp);
		pMe->pIWebResp = NULL;
	}

	if (pMe->pIWebUtil != NULL)
	{
		IWEBUTIL_Release(pMe->pIWebUtil);
		pMe->pIWebUtil = NULL;
	}

	if (pMe->poststr != NULL)
	{
		FREE(pMe->poststr);
		pMe->poststr = NULL;
	}
	if (pMe->poststrFooter != NULL)
	{
		FREE(pMe->poststrFooter);
		pMe->poststrFooter = NULL;
	}
	//if (pMe->pIGetLine != NULL)
	//{
	//	IGETLINE_Release(pMe->pIGetLine);
	//	pMe->pIGetLine = NULL;
	//}

}

