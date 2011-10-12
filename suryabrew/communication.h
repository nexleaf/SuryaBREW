
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_
#include "AEEAppGen.h"
#include "AEEModGen.h"
#include "AEEShell.h"
#include "AEESource.h"
#include "AEEError.h"
#include "AEEWeb.h"
#include "AEEFile.h"			// File interface definitions
#include "AEEStdLib.h"			// AEEStdLib standard library
#include "AEETAPI.h"
//#include "AForm.h"

//For GPS System


#include "suryabrew.h"
//class BaseApp;


#define FILE_MSG "File Can't Write.Not enough Space"
#define MEM_MSG  "Not enough Memory"
#define WEB_MSG	 "Web Not Connected"
#define SRC_MSG	 "Source Error"		

#define	APP_BUFF	   2000
#define MAX_URL		   1000
#define  RESP_BUFF     4000	
#define  BUFFER_LENGTH 2000


#define WEB_RES_I		1			//piWeb_IW
#define WEB_RES_II		2			//				//piWebResp_IIW
#define WEB_RES_III		4			//piNetMgr_IIIW
#define WEB_RES_IV		8			//piWebResp_IVW
#define WEB_RES_V		16			//				//pszPostData_VW
#define WEB_RES_VI		32			//pipPostData_VIW
#define WEB_RES_VII		64			//piGetLine_VIIW
#define WEB_RES_VIII	128			//piSrcUtl_VIIIW
#define WEB_RES_IX		256			//				//pWebRespInfo_IXW
#define WEB_RES_X		512			//p_tapi_XW
#define WEB_RES_XI		1024		//p_Static_alert_Ctl_XIW
#define WEB_RES_XII		2048		//
#define WEB_RES_XIII	4096		//p_file_alert_XIIIW;
#define WEB_RES_XIV		8192		//p_file_stock_XIVW;
#define WEB_RES_XV		16384		//p_file_app_XVW;
#define WEB_RES_XVI		32768		//p_filemgr_XVIW;

#define CONNECTING_SCREEN  1
#define	MEMORY_ERROR_SCREEN 2


typedef void (*FUN_PTR)(char *,uint8 ,boolean);


typedef struct _WebAction	
{
AEECallback			callback;		//how to call me back
IWebResp			*piWebResp_IVW;		//the answer I get from IWeb
IGetLine			*piGetLine_VIIW;	//the body of the response, if any
int					nLines;			//count of lines in response
int					nBytes;			//count of bytes in response
uint32				uStart;			//?
uint32				uRecvStart;		//?
IPeek				*pipPostData_VIW;	//POST stream, if any
IPeek               *pipPostData;
} WebAction;



typedef struct _WebWork
{
	AEEApplet      a ;
//	IDisplay      *pIDisplay;  // Give a standard way to access the Display interface
  //  IShell        *pIShell;    // Give a standard way to access the Shell interface

//static void AllowVoiceCall(void *pi);
//char					*pch_appBuffer;
char                    szTempBuffer[APP_BUFF];

uint16					web_res_ui16;
uint16					web_state_ui16;	
IWeb					*piWeb_IW;
WebRespInfo				*pWebRespInfo;
ISourceUtil				*piSrcUtl_VIIIW;
ISource                 *m_pISource;

WebAction				m_webact;
char					ch_Url[MAX_URL]; //static
//uint8					req_no;
char					big_buff_1000[BUFFER_LENGTH]; //static
//uint16					SCREEN_NO;

struct _WebWork	*app_ptr;


char*					WebHeader;
IFileMgr				*p_responce_file_mgr;
IFile					*p_responce_file;
FUN_PTR                 out_function;

} WebWork;

void  Web_Init(WebWork *pMe);
boolean Make_Request_Fun(WebWork *pMe,char * url, const char* pFileName);
boolean Web_Action_Start_For_Communication(WebWork *pMe);
void WebStatusHandler(void *pNotifyData, WebStatus ws, void *pData);
void Web_Action_Got_Responce_Of_Communication(WebWork *pMe);
void WebAction_ReadLines_Of_Communication(WebWork *pMe);
void WebAction_Stop_Of_Communication(WebWork *pMe);
void WebHeaderHandler(void *pNotifyData, const char *pszName, GetLine *pglVal);
void ReleaseObj(void ** ppObj);


#endif // _COMMUNICATION_H_

























