

#include "suryabrew.h"
#include "communication.h"

static void suryabrew_UPIMGHeaderNotification(void *p, const char *cpszName, GetLine *pglVal)  //NOT USE
{
	if (cpszName != NULL) {
		DBGPRINTF("UPIMG HEADER PRINT %s %s\n", cpszName, pglVal->psz);
   }
}


void suryabrew_UPIMGStatusNotification(void *pNotifyData, WebStatus ws, void *pData) //NOT USE
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


void suryabrew_UPIMGPostDone(void *pData) //NOT USE 
{
/*	int nErr = 0;
	int bytes = 0;
	char buf[32] = {0};
	boolean success = FALSE;
	suryabrew *pMe = (suryabrew *) pData;
*/
/*	WebRespInfo *respi = IWEBRESP_GetInfo(pMe->pIWebResp);
	pMe->UPIMGStopTime = GETTIMESECONDS();

	DBGPRINTF("UPIMGPostDone Code %d len %d exp %d mod %d TIME: %d",
		respi->nCode, respi->lContentLength, respi->tExpires,
		respi->tModified, pMe->UPIMGStopTime - pMe->UPIMGStartTime);
-*/
	// Return codes are a number in the document body
	// upok  means everything worked
	// dupok  means already have image
	// o.w. error

	// The IGetLine way seem too complicated, doing the simple way
/*	if (WEB_ERROR_SUCCEEDED(respi->nCode))
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
*/

	//if (pMe->pIMultiPeek != NULL)
	//{
	//	IMULTIPEEK_Release(pMe->pIMultiPeek);
	//	pMe->pIMultiPeek = NULL;
	//}
/*	if (pMe->pIWebResp != NULL)
	{
		IWEBRESP_Release(pMe->pIWebResp);
		pMe->pIWebResp = NULL;
	}*/
	//if (pMe->pIGetLine != NULL)
	//{
	//	IGETLINE_Release(pMe->pIGetLine);
	//	pMe->pIGetLine = NULL;
	//}

//	if (success)
//	{
	/*	nErr = ISHELL_SetTimer(pMe->pIShell, SURYABREW_UPIMG_UPLOAD_INTERVAL,
			pMe->UPIMGTimerCB.pfnNotify, pData);
		DBGPRINTF("UPIMGTIME set timer: %d", nErr);*/
//	}

}



void suryabrew_UPDATATimer(void *pData)
{
	int nErr = 0;
	int size = 0; 
	suryabrew *pMe = (suryabrew *) pData;
	WebWork *a_App=(WebWork*)GETAPPINSTANCE();

	suryabrew_uploaddata *ud = (suryabrew_uploaddata *) MALLOC(sizeof(suryabrew_uploaddata));
	DBGPRINTF("in suryabrew upimgtimer");
	if (ud == NULL)
	{
		DBGPRINTF("Unable to allocated space for upload data!!!!");
		return;
	}

	// First setup and encode the post string

	if(pMe->poststr==NULL)
	pMe->poststr = MALLOC(sizeof(char) * 4096);

//	pMe->poststrFooter = MALLOC(sizeof(char) * 128);


	if (suryabrew_DBGetNotUploaded(pMe, ud) != SUCCESS)
	{
	/*	nErr = ISHELL_SetTimer(pMe->pIShell, SURYABREW_UPIMG_UPLOAD_INTERVAL,
			pMe->UPIMGTimerCB.pfnNotify, pData);*/
		DBGPRINTF("dbgetnotuploaded not success ");
		pMe->DBUploadRecord = -1;
		FREE(ud);
		return;
	} 

	pMe->DBUploadRecord = ud->DBRecordID;
	DBGPRINTF("sucess in uptimer");
/*	if(Make_Request_Fun(pMe,SURYABREW_UPLOAD_LOC,"response.txt")==TRUE)
	{
	}*/
	if(pMe->Upload_Data)
	{
		pMe->Upload_Data=FALSE;
	Web_Init(a_App);
	Web_Action_Start_For_Communication(a_App);
	}

	if (ud != NULL)
	{
		FREE(ud);
	}
}



void suryabrew_UPDATAInit(suryabrew *pMe)
{
/*	int nErr = 0;
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


	CALLBACK_Init(&pMe->UPIMGPostCB, suryabrew_UPIMGPostDone, pMe);*/
	CALLBACK_Init(&pMe->UPIMGTimerCB, suryabrew_uploaddata_notify, pMe);
	suryabrew_uploaddata_notify(pMe);

}

void suryabrew_UPDataUnload(suryabrew *pMe)
{
/*	if (pMe->do_upload)
	{
		if ( pMe->pIWeb != NULL)
		{
			suryabrew_UPIMGStop(pMe);
			IWEB_Release(pMe->pIWeb);
			pMe->pIWeb = NULL;
		}
	}*/
	if (pMe->poststr != NULL)
	{
		FREE(pMe->poststr);
		pMe->poststr = NULL;
	}
//	suryabrew_UPIMGStop(pMe);
}

void suryabrew_UPIMGStop(suryabrew *pMe)  //NOT USE 
{


//	CALLBACK_Cancel(&pMe->UPIMGPostCB);
	CALLBACK_Cancel(&pMe->UPIMGTimerCB);
	ISHELL_CancelTimer(pMe->pIShell, pMe->UPIMGTimerCB.pfnNotify, pMe);

/*	if (pMe->pIWebResp != NULL)
	{
		IWEBRESP_Release(pMe->pIWebResp);
		pMe->pIWebResp = NULL;
	}

	if (pMe->pIWebUtil != NULL)
	{
		IWEBUTIL_Release(pMe->pIWebUtil);
		pMe->pIWebUtil = NULL;
	}*/

	if (pMe->poststr != NULL)
	{
		FREE(pMe->poststr);
		pMe->poststr = NULL;
	}
/*	if (pMe->poststrFooter != NULL)
	{
		FREE(pMe->poststrFooter);
		pMe->poststrFooter = NULL;
	}*/
	//if (pMe->pIGetLine != NULL)
	//{
	//	IGETLINE_Release(pMe->pIGetLine);
	//	pMe->pIGetLine = NULL;
	//}

}

void suryabrew_Remove_DataBase(suryabrew *pMe)
{

		 IDBMgr *pDBMgr = NULL;
		int nErr;
	;
		nErr = ISHELL_CreateInstance(pMe->pIShell, AEECLSID_DBMGR, (void**)(&pDBMgr));
		if ( nErr != SUCCESS)
		{
			DBGPRINTF("Unable to create DBMgr instance %d", nErr);
		}
	  
	
		  if (pDBMgr != NULL)
			{
			  if(pMe->pIDatabase!=NULL)
			  {
				  IDATABASE_Release(pMe->pIDatabase);
				  pMe->pIDatabase=NULL;
			  }
			//  suryabrew_FileDeleteFile(pMe,"suryabrew.db");
			nErr=  IDBMGR_Remove(pDBMgr, SURYABREW_DB_FILE);
			if ( nErr != SUCCESS)
			{
			DBGPRINTF("DataBase is not remove %d", nErr);
			}
			else
			{
				suryabrew_DBInit(pMe);

			}
		  }
ISHELL_SetTimer(pMe->pIShell, 8000,pMe->UPIMGTimerCB.pfnNotify, pMe);

}