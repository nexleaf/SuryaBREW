#include "communication.h"
//#include "CMenu.h"
#include "suryabrew.h"

/*CWebWork::CWebWork(BaseApp*pApp)
{
	this->pApp=pApp;
	if (ISHELL_CreateInstance(GET_ISHELL(), AEECLSID_FILEMGR, (void**)&p_responce_file_mgr) != SUCCESS)
	{
		//Error_Message();// ERROR
	}
}

CWebWork::~CWebWork()
{
	WebAction_Stop_Of_Communication();
	if(p_responce_file_mgr)
	{
		IFILEMGR_Release(p_responce_file_mgr);
		p_responce_file_mgr=NULL;
	}
	if(p_responce_file  !=NULL)
	{
		IFILE_Release(p_responce_file);
		p_responce_file=NULL;
	}
}
*/
boolean Make_Request_Fun(WebWork *pMe,char * url, const char * pFileName)  //not use
{
//	ISHELL_CancelTimer(pApp->m_pIShell,AllowVoiceCall,pApp);
	STRCPY(pMe->ch_Url,url);
/*	if(pMe->p_responce_file  !=NULL)
	{
		IFILE_Release(pMe->p_responce_file);
		pMe->p_responce_file=NULL;
	}*/
/*	if((IFILEMGR_Test(pMe->p_responce_file_mgr,pFileName))==SUCCESS)//test4
	{
		if((IFILEMGR_Remove(pMe->p_responce_file_mgr,pFileName))!=SUCCESS)
		{
			return FALSE;
		}
	}*/
	if((pMe->p_responce_file = IFILEMGR_OpenFile(pMe->p_responce_file_mgr,pFileName, _OFM_CREATE))== NULL)
	{
		return FALSE;
	}
	if(!(Web_Action_Start_For_Communication(pMe)))
	{
		return FALSE;
	}
	return TRUE;
}

boolean Web_Action_Start_For_Communication(WebWork *pMe)
{
	suryabrew *p_app=(suryabrew*)GETAPPINSTANCE();
//	WebOpt	awo[11]; //static
	int i=0;
	ISourceUtil *pisu;
	
	STRCPY(pMe->ch_Url,SURYABREW_UPLOAD_LOC);
	DBGPRINTF("in webstart conn");
	if(pMe->web_res_ui16 & WEB_RES_I)
	{
		if(pMe->piWeb_IW!=NULL)
		{
		
		IWEB_Release(pMe->piWeb_IW);
		pMe->piWeb_IW = NULL;
		pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_I);
		}
	}
	DBGPRINTF("in webstart conn1");
	if ((ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void **)&pMe->piWeb_IW)) != SUCCESS)
	{
		DBGPRINTF("instance not creat aeecisid_wev");
		return FALSE;
	}
	else
	{
		DBGPRINTF("in webstart conn2");
		pMe->web_res_ui16 = (pMe->web_res_ui16 | WEB_RES_I);
	}	
	if (pMe->web_res_ui16 & WEB_RES_VIII)
	{
		
		ISOURCEUTIL_Release(pMe->piSrcUtl_VIIIW);
		pMe->piSrcUtl_VIIIW = NULL;
		pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VIII);
	}
	/*if(pApp->m_b_module->servlet_no == REQ_CALL)
	{
		
		if(pch_appBuffer)
		{
			FREE(pch_appBuffer);
			pch_appBuffer = NULL;
		}
		pch_appBuffer = (char*)MALLOC(sizeof(char)*2000);   
		
		STRCPY(pch_appBuffer,"mdn=2267339740&sid=ex16b22zmddp6mny&capsule=46");
	}*/
	DBGPRINTF("in webstart conn3");
	CALLBACK_Init(&pMe->m_webact.callback, (PFNNOTIFY)Web_Action_Got_Responce_Of_Communication, pMe);	
	DBGPRINTF("in webstart conn4");
	if ((char *)0 != p_app->poststr) 
	{
		DBGPRINTF("in webconn5");
		if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, 
		  (void **)&pisu)) {			
			DBGPRINTF("in webconn6");
			ISOURCEUTIL_PeekFromMemory(pisu, p_app->poststr, 
				STRLEN(p_app->poststr), 0, 0,
				&pMe->m_webact.pipPostData);
			DBGPRINTF("in webconn7");
			/*if (ISOURCEUTIL_PeekFromMemory(piSrcUtl_VIIIW, ch_appBuffer, STRLEN(ch_appBuffer), NULL, NULL, &m_webact.pipPostData_VIW) == SUCCESS)
			{
				web_res_ui16 = (web_res_ui16 | WEB_RES_VI);
			}*/
		}
	}
	else
	{
		/* Making a callback registered */		
		
		if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOURCEUTIL, (void **)&pMe->piSrcUtl_VIIIW) == SUCCESS)
		{
			pMe->web_res_ui16 = (pMe->web_res_ui16 | WEB_RES_VIII);
		}
		else
		{
			return FALSE;
		}
		
		if (pMe->web_res_ui16 & WEB_RES_VI)
		{
			IPEEK_Release(pMe->m_webact.pipPostData_VIW);
			pMe->m_webact.pipPostData_VIW = NULL;
			pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VI);
		}
		if (ISOURCEUTIL_PeekFromMemory(pMe->piSrcUtl_VIIIW, pMe->szTempBuffer, STRLEN(pMe->szTempBuffer), NULL, NULL, &pMe->m_webact.pipPostData_VIW) == SUCCESS)
		{
			pMe->web_res_ui16 = (pMe->web_res_ui16 | WEB_RES_VI);
		}
		
		if (pMe->web_res_ui16 & WEB_RES_VIII)
		{
			ISOURCEUTIL_Release(pMe->piSrcUtl_VIIIW);
			pMe->piSrcUtl_VIIIW = NULL;
			pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VIII);
		}
		DBGPRINTF("poststr is null");
	}
	
/*	if(pApp->m_b_module->servlet_no != REQ_CALL)
	{
		
		if (m_webact.pipPostData_VIW != (IPeek *)0)
		{
			awo[i].nId = WEBOPT_HEADERHANDLER;
			awo[i].pVal = (void*)WebHeaderHandler;
			i++;

			awo[i].nId = WEBOPT_HANDLERDATA;
			awo[i].pVal = (void *)&m_webact;
			i++;
			
			awo[i].nId = WEBOPT_METHOD;
			awo[i].pVal = (void *)"GET";
			i++;
			
			awo[i].nId = WEBOPT_BODY;
			awo[i].pVal = (void *)m_webact.pipPostData_VIW;
			i++;
			
			awo[i].nId = WEBOPT_CONTENTLENGTH;
			//awo[i].pVal = (void *)(long)STRLEN(ch_appBuffer);
			awo[i].pVal = NULL;
			i++;
			
			awo[i].nId = WEBOPT_CONNECTTIMEOUT;
			awo[i].pVal = (void *)35000;		//To be in milliseconds
			i++;
			
			awo[i].nId = WEBOPT_IDLECONNTIMEOUT;
			awo[i].pVal = (void *)30;
			i++;			
			
			
			awo[i].nId = WEBOPT_STATUSHANDLER;
			awo[i].pVal = (void*)WebStatusHandler;
			i++;

			/*awo[1].nId = WEBOPT_PROXYSPEC;
			awo[1].pVal = (void *)"http:///http://192.168.151.25:80";*/
			
/*		awo[i].nId = WEBOPT_END;
			
			IWEB_AddOpt(piWeb_IW, awo);
			
			if (web_res_ui16 & WEB_RES_IV)
			{
				IWEBRESP_Release(m_webact.piWebResp_IVW);
				m_webact.piWebResp_IVW = NULL;
				web_res_ui16 = (web_res_ui16 ^ WEB_RES_IV);
			}
			
			IWEB_GetResponse(piWeb_IW, (piWeb_IW, &m_webact.piWebResp_IVW, &m_webact.callback, ch_Url, WEBOPT_END));
			//pApp->m_b_module->B_Out_Function();
			return TRUE;
			//	B_Out_Function()
		}
	}
	else*/
	{
		DBGPRINTF("in webconn8");
		IWEB_GetResponse(pMe->piWeb_IW,
                       (pMe->piWeb_IW, &pMe->m_webact.piWebResp_IVW, 
					   &pMe->m_webact.callback, pMe->ch_Url, 
                        //WEBOPT_HANDLERDATA, m_webact,
						WEBOPT_METHOD, "POST",
                        WEBOPT_HEADER,"Content-Type: application/x-www-form-urlencoded\r\n", /* for kicks */						
					//	WEBOPT_PROXYSPEC,(void*)"http//"
						//WEBOPT_HEADER,"application/x-www-form-urlencoded\r\n", /* for kicks */
                       // WEBOPT_HEADERHANDLER, WebHeaderHandler, 
                        WEBOPT_STATUSHANDLER, WebStatusHandler,                         
                        WEBOPT_BODY, pMe->m_webact.pipPostData,
                        WEBOPT_CONTENTLENGTH, STRLEN(p_app->poststr),
                        WEBOPT_END));
//	p_app->upload_flag=FALSE;
		DBGPRINTF("in webconn9");

		/*IWEB_GetResponse(piWeb_IW,
                       (piWeb_IW, &m_webact.piWebResp_IVW, &m_webact.callback,ch_Url, 
                        WEBOPT_HANDLERDATA, m_webact,
						WEBOPT_METHOD,"POST",
                        WEBOPT_HEADER,"Content-Type: application/x-www-form-urlencoded\r\n", 
                        WEBOPT_HEADERHANDLER, WebHeaderHandler, 
                        WEBOPT_STATUSHANDLER, WebStatusHandler,                         
                        WEBOPT_BODY,(void *)m_webact.pipPostData_VIW,
                        WEBOPT_CONTENTLENGTH, STRLEN(ch_appBuffer),
                        WEBOPT_END));*/
		ISOURCEUTIL_Release(pisu);
/*		if(p_app->poststr!=NULL)
		{
			FREE(p_app->poststr);
			p_app->poststr==NULL;
		}*/
//		suryabrew_UPDataUnload(p_app);
		return TRUE;		
	}
	return FALSE;
}

void WebStatusHandler(void *pNotifyData, WebStatus ws, void *pData)
{
	switch(ws)
	{
	case WEBS_STARTING:
		DBGPRINTF("WEBS_STARTING");
		break;
	case WEBS_CANCELLED:
		DBGPRINTF("WEBS_CANCELLED");
		break;
	case WEBS_GETHOSTBYNAME:
		DBGPRINTF("WEBS_GETHOSTBYNAME");
		break;
	case WEBS_CONNECT:
		DBGPRINTF("WEBS_CONNECT");
		break;
	case WEBS_SENDREQUEST:
		DBGPRINTF("WEBS_SENDREQUEST");
		break;
	case WEBS_READRESPONSE:
		DBGPRINTF("WEBS_READRESPONSE");
		break;
	case WEBS_GOTREDIRECT:
		DBGPRINTF("WEBS_GOTREDIRECT");
		break;
	case WEBS_CACHEHIT:
		DBGPRINTF("WEBS_CACHEHIT");
		break;
	}
}


void Web_Action_Got_Responce_Of_Communication(WebWork *pMe)
{
	char buf[32]={0};
	int bytes=0;
	int nErr=0;
	boolean success=FALSE;
		WebWork *pApp = (WebWork*)GETAPPINSTANCE();
		suryabrew *p_App=(suryabrew*)GETAPPINSTANCE();
	WebRespInfo	*pWebRespInfo;
	DBGPRINTF("in web action got response");
	CALLBACK_Cancel(&pMe->m_webact.callback);
	if (pMe->m_webact.piWebResp_IVW != NULL)
	{
		pMe->web_res_ui16 = (pMe->web_res_ui16 | WEB_RES_IV);
	}
	pWebRespInfo = IWEBRESP_GetInfo(pMe->m_webact.piWebResp_IVW);
	DBGPRINTF("return code==> %d",pWebRespInfo->nCode);
	if ((ISource *)0 != pWebRespInfo->pisMessage)
	{
		if (pMe->web_res_ui16 & WEB_RES_VIII)//If Source Util Resource not free, release it.
		{
			ISOURCEUTIL_Release(pMe->piSrcUtl_VIIIW);
			pMe->piSrcUtl_VIIIW = NULL;
			pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VIII);
		}
		
		  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOURCEUTIL, (void **)&pMe->piSrcUtl_VIIIW) == SUCCESS)
		{
			pMe->web_res_ui16 = (pMe->web_res_ui16 | WEB_RES_VIII);
			
		}
//		else
//		{
//			//call helper function for no memory
//			//show_memory_error(pMe);
//			pMe->WebAction_Stop_Of_Communication();
//			//(*pMe->out_function)(MEM_MSG,pMe->req_no,FALSE);
//			b_module::B_Out_Function(GET_APPLET()->m_b_module,MEM_MSG,pMe->req_no,FALSE);
//			return;
//		}
		if (pMe->web_res_ui16 & WEB_RES_VII)
		{
			IGETLINE_Release(pMe->m_webact.piGetLine_VIIW);
			pMe->m_webact.piGetLine_VIIW = NULL;
			pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VII);
		}
		
		if (ISOURCEUTIL_GetLineFromSource(pMe->piSrcUtl_VIIIW, pWebRespInfo->pisMessage, RESP_BUFF, &pMe->m_webact.piGetLine_VIIW) == SUCCESS)
		{
			pMe->web_res_ui16 = (pMe->web_res_ui16 | WEB_RES_VII);
		}
		
		if (pMe->web_res_ui16 & WEB_RES_VIII)
		{
			ISOURCEUTIL_Release(pMe->piSrcUtl_VIIIW);
			pMe->piSrcUtl_VIIIW = NULL;
			pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VIII);
		}
	}    
	/*if(pApp->m_b_module->servlet_no == REQ_CALL)
	{
		char buffer[50] = {0};
		SPRINTF(buffer,"%d",pWebRespInfo->nCode);
		pApp->WriteInFile(buffer);
	}*/
	if(WEB_ERROR_SUCCEEDED(pWebRespInfo->nCode))//?:200 is acceptable
	{
 	 	pMe->m_webact.nBytes = 0;
			DBGPRINTF("return code==> %d",pWebRespInfo->nCode);
			bytes = ISOURCE_Read(pWebRespInfo->pisMessage, buf, sizeof(buf));
			DBGPRINTF("buf==>%s",buf);
		switch (bytes)
		{
		case ISOURCE_WAIT:
			DBGPRINTF("UPDatatDone need to wait on data, rerunning callback on readable");
			// since we have not allocated anything, then this should be fine to do
			ISOURCE_Readable(pWebRespInfo->pisMessage, &pMe->m_webact.callback);
			return;
		case ISOURCE_ERROR:
			DBGPRINTF("UPDatatDone Error reading document cbody");
			success = FALSE;
			break;
		case ISOURCE_END:
			DBGPRINTF("UPDatatDone Read complete document");
		default:
			DBGPRINTF("upload success");
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
			if(success)
			{
					p_App->check_recod_fail_4times=0;
				suryabrew_DBSetUploaded(p_App);
			}
			else
				p_App->check_recod_fail_4times++;

			DBGPRINTF("CHECK RECORD FAIL IN GOT RESPONSE==>%d",p_App->check_recod_fail_4times);
//			if(p_App->pIDatabase)
//			{
//				
//				if(nErr==SUCCESS)
//					DBGPRINTF("database successfully removed");
//			}
//			p_App->upload_flag=FALSE;
	//	suryabrew_Remove_DataBase(p_App);
		
		WebAction_Stop_Of_Communication(pMe);
	//	WebAction_ReadLines_Of_Communication(pMe);
		}
		
		//pMe->pApp->m_b_module->B_Out_Function();
		//	
		//GETAPPLET()->m_nSCREENID = PARSEDATA;
	}
	else
	{	// show error
		//int i = GETLASTFPERROR();
//		p_App->upload_flag=TRUE;
		p_App->check_recod_fail_4times++;
		WebAction_Stop_Of_Communication(pMe);
		//GET_APPLET()->Error_Message();
		//(*pMe->out_function)(WEB_MSG,pMe->req_no,FALSE);
	//	b_module::B_Out_Function(GET_APPLET()->m_b_module,WEB_MSG,pMe->req_no,FALSE);
		DBGPRINTF("5");
		DBGPRINTF("return code==> %d",pWebRespInfo->nCode);
	}
	p_App->set_data_indexid=FALSE;
	ISHELL_SetTimer(p_App->pIShell, 8000,p_App->UPIMGTimerCB.pfnNotify, p_App);
}

void WebAction_ReadLines_Of_Communication(WebWork *pMe)  //not use
{
	int32 len=0;
	
	CALLBACK_Cancel(&pMe->m_webact.callback);


/*	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pMe->p_responce_file_mgr) != SUCCESS)
	{
		DBGPRINTF("not suucess");
	}*/
//	GET_APPLET()->m_bAllowCall = FALSE;
	while(1)
	{
		//ZEROAT(pMe->big_buff_1000);
		MEMSET(pMe->big_buff_1000,'\0',BUFFER_LENGTH);
		
 		len=IGETLINE_Read(pMe->m_webact.piGetLine_VIIW,pMe->big_buff_1000,BUFFER_LENGTH-1);
		
		if(len>0)
		{
		//	if(0==(IFILE_Write(pMe->p_responce_file,pMe->big_buff_1000,len)))
			{
				WebAction_Stop_Of_Communication(pMe);
			//	b_module::B_Out_Function(GET_APPLET()->m_b_module,FILE_MSG,pMe->req_no,FALSE);
				DBGPRINTF("1");
				return;
			}
			
		//	IFILE_Seek(pMe->p_responce_file,_SEEK_END,0); 	
		}	
		else if(len == IGETLINE_WAIT)
		{
			CALLBACK_Init(&pMe->m_webact.callback,WebAction_ReadLines_Of_Communication, pMe);
			IGETLINE_Peekable(pMe->m_webact.piGetLine_VIIW, &pMe->m_webact.callback);
			return;
		} 
		else
		{
			if(len ==ISOURCE_ERROR)
			{
				WebAction_Stop_Of_Communication(pMe);
				
				//(*pMe->out_function)(SRC_MSG,pMe->req_no,FALSE);		
//				b_module::B_Out_Function(GET_APPLET()->m_b_module,SRC_MSG,pMe->req_no,FALSE);
				DBGPRINTF("2");
				return;
			}						
			else
			{//b_module::B_Out_Function(GET_APPLET()->m_b_module,FILE_MSG,pMe->req_no,FALSE);//new added anjani
				break;
			}
			
		}
		
	}
	WebAction_Stop_Of_Communication(pMe);
	//ISHELL_SetTimer(GET_APPLET()->m_pIShell,30000,(PFNNOTIFY)AllowVoiceCall,GET_APPLET());
//	b_module::B_Out_Function(GET_APPLET()->m_b_module,"SUCCESS",pMe->req_no,TRUE);
	DBGPRINTF("3");
	
}

void WebAction_Stop_Of_Communication(WebWork *pMe)
{
	DBGPRINTF("IN WEBACTION STOP OF COMMUNICATION");
	CALLBACK_Cancel(&pMe->m_webact.callback);
	if(pMe->web_res_ui16 & WEB_RES_I)
	{
		IWEB_Release(pMe->piWeb_IW);
		pMe->piWeb_IW = NULL;
		pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_I);
	}
	
	if (pMe->web_res_ui16 & WEB_RES_VII)
	{
		IGETLINE_Release(pMe->m_webact.piGetLine_VIIW);
		pMe->m_webact.piGetLine_VIIW = NULL;
		pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VII);
	}	
	if (pMe->web_res_ui16 & WEB_RES_IV)
	{
		IWEBRESP_Release(pMe->m_webact.piWebResp_IVW);
		pMe->m_webact.piWebResp_IVW = NULL;
		pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_IV);
	}
	
	if (pMe->web_res_ui16 & WEB_RES_VI)
	{
		IPEEK_Release(pMe->m_webact.pipPostData_VIW);
		pMe->m_webact.pipPostData_VIW = NULL;
		pMe->web_res_ui16 = (pMe->web_res_ui16 ^ WEB_RES_VI);
	}

	/*if(pMe->pch_appBuffer)
	{
		FREE(pMe->pch_appBuffer);
		pMe->pch_appBuffer = NULL;
	}*/

	////////

/*	if(pMe->p_responce_file_mgr)
	{
		IFILEMGR_Release(pMe->p_responce_file_mgr);
		pMe->p_responce_file_mgr=NULL;
	}
	if(pMe->p_responce_file  !=NULL)
	{
		IFILE_Release(pMe->p_responce_file);
		pMe->p_responce_file=NULL;
	}*/
	///////// /
	ReleaseObj((void**)&pMe->m_webact.pipPostData);
	
}
/*void CWebWork::AllowVoiceCall(void *pi) //Manisha_12_Oct_09 now no need of it
{
	BaseApp*	pApp = (BaseApp*)pi;
	ISHELL_CancelTimer(pApp->m_pIShell,AllowVoiceCall,pApp);
	//pApp->m_bAllowCall = TRUE;
	ISHELL_SetTimer(pApp->m_pIShell,0,pApp->m_b_module->SetTheCall,pApp);
}*/
void WebHeaderHandler(void *pNotifyData, const char *pszName, GetLine *pglVal)  //NOT USE
{
	WebWork *pMe = (WebWork*)pNotifyData;
	if(STRCMP(pszName,"header")==0)
	{
		/*FREE_IF(GETAPPLET()->m_pCMenu->m_pCWebWork->WebHeader);
		GETAPPLET()->m_pCMenu->m_pCWebWork->WebHeader = (char*)MALLOC(pglVal->nLen+1);
		STRCPY(GETAPPLET()->m_pCMenu->m_pCWebWork->WebHeader,pglVal->psz);
		if(GETAPPLET()->m_nPrevSCREENID == DOWNLOADSMENUSCREENTRACK && GETAPPLET()->m_nReq == 2)
		{	
			MEMSET(GETAPPLET()->m_pSaveData.WebHeader,0,100);
			STRCPY(GETAPPLET()->m_pSaveData.WebHeader,pglVal->psz);
		}*/
		/*if(GET_APPLET()->m_b_module->servlet_no == REQ_EXERCISE_FILE)

			GET_APPLET()->m_b_module->B_Length_Parsing(pglVal->psz);*/
	}
}

void ReleaseObj(void ** ppObj)
{
	DBGPRINTF("IN RELEASEOBJ");
   if (*ppObj) {
      (void)IBASE_Release(((IBase *)*ppObj));
      *ppObj = NULL;
   }
}

void Web_Init(WebWork *pMe) 
{
	DBGPRINTF("IN WEB INIT");
	pMe->piWeb_IW=NULL;
	pMe->web_res_ui16=0;
	pMe->web_state_ui16=0;
	pMe->pWebRespInfo=NULL;
	pMe->piSrcUtl_VIIIW=NULL;
	pMe->m_pISource=NULL;

	pMe->m_webact.callback.pCancelData=0;
	pMe->m_webact.callback.pfnCancel=0;
	pMe->m_webact.callback.pfnNotify=0;
	pMe->m_webact.callback.pNext=0;
	pMe->m_webact.callback.pNotifyData=0;
	pMe->m_webact.callback.pmc=0;
	pMe->m_webact.callback.pReserved=0;
	pMe->m_webact.piGetLine_VIIW=NULL;
	pMe->m_webact.pipPostData=NULL;
	pMe->m_webact.pipPostData_VIW=NULL;
	pMe->m_webact.piWebResp_IVW=NULL;
	
/*	pMe->

	pMe->pIWeb = NULL;
	pMe->pIWebResp = NULL;
	pMe->pIWebUtil = NULL;
	pMe->pIMultiPeek = NULL;
	pMe->pIGetLine = NULL;*/
}