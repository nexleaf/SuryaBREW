#ifndef SURYABREW_H_H
#define SURYABREW_H_H


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEImage.h"
#include "AEEPosDet.h"
#include "AEEdb.h"
#include "AEEWeb.h"
#include "AEETAPI.h"

#include "communication.h"
//#include "AEEStdDef.h"

#include "string.h"

#include "AEEHeap.h"
#include "AEEFile.h"			// File interface definitions
#include "AEEDB.h"				// Database interface definitions
#include "AEENet.h"				// Socket interface definitions
//#include "AEECamera.h"          // Camera interface definitions
#include "AEEMimeTypes.h"       // Get the mine types
#include "AEEMedia.h"
#include "AEEMediaUtil.h"
#include "AEEVocoder.h"
#include "AEESound.h"

#include "aeemultipeek.h"

#include "suryabrew.bid"
#include "suryabrew.brh"      // Resource ID definitions
//#include "suryabrew.mif"


/**
*
* Set Volume to max
* Test setting quality (to reduce image file size)
* Filter target
*
* Upload - make upload capability (for temp and img)
*
* ON HOLD - GPS into EXIF info
*
* PHONE ID - make ifdefs for sim, verify getting full
*/

///////////////////////////////////////////////////current configration parameter

#define	AVERGING_WINDOW 1    //in second
#define SAMPLE_DURATION	60   //in second
#define	PUASE_INTERVAL	240		//in second
#define	CALIBRATION_SCALAR	500 
#define	CALIBRATION_OFFSET	720000
#define	CALIBRATION_DIVIDER 10000
#define DEPLOYMENT_ID	   "ASTUTE"
#define MIME_TYPE		"text/csv"
#define DATA_TYPE		"temp"
#define DEVICEID		"A100000MARTIN1"
#define NO_UPLOAD_FAIL	3
//////////////////////////////////////////////////////


///////////////////////////////new changes
#define TRA   "0.000693546"
#define TRB   "0.000201139"
#define TRC   "0.0000000933821"
#define TR	"231.5k"
#define RES1   3000
#define RES2	0
#define BOARDREV	2
#define CALIBPCM	142
#define CALIBOHMS	176261
#define INPUTPCM 8485





/////////////////////////////////new changes

typedef enum
{
	SURYAMODE_DEFAULT,
	SURYAMODE_CAMERA,
	SURYAMODE_TEMP
} SuryaModeType;

typedef enum
{
	SOUNDMODE_PLAYBACK,
	SOUNDMODE_ONE,
    SOUNDMODE_LOW,
	SOUNDMODE_MED,
	SOUNDMODE_HIGH
} SuryaSoundMode;


/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _suryabrew {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information

    IDisplay      *pIDisplay;  // Give a standard way to access the Display interface
    IShell        *pIShell;    // Give a standard way to access the Shell interface

	// configuration
//	boolean do_upload;              // try to uplaod files
//    boolean do_upload_delete_file;  // delete the image and DB row after upload
//	boolean do_gps;                 // run the gps subsystem
	boolean allow_volume;          // allow someone to change the volume
//	boolean enable_camera;         // whether the camera is enabled and accessible

	SuryaModeType SuryaMode; // Home screen, taking pictures, or taking temp

	int m_cxWidth;     // Stores the device screen width
	int m_cyHeight;    // Stores the device screen height
	//int m_nCursorX;    // Stores the cursor bitmap x coordinate
	//int m_nCursorY;    // Stores the cursor bitmap y coordinate

//	IImage * pIImage;  // IImage interface pointer for captured images
//	IImage * pIImage_checkyes; // Image for accepting filter image
//	IImage * pIImage_checkno;  // image for rejecting filter image

	// File management for directory creation
	IFileMgr *pIFileMgr;
	char FileNamePrefix[64];

	// Camera data
//	ICamera *pICamera;
	AEEMediaData MediaData;
	IMedia *imedia;
//	int CameraMode;
	//AEEHWIDType* HWID;
	//char *IMEI;
	char *HWID;
	JulianType CurrDate;
	uint32 CurrDateSec;
	boolean Flag;
	char ImageFilename[128];

	// GPS data
/*	IPosDet *pIPosDet;
	AEEPositionInfoEx GPSPosition;
	AEEGPSInfo GPSInfo;
	AEEGPSConfig GPSConfig;
	AEECallback GPSDataCB;
	AEECallback GPSTimerCB;
	boolean GPSValid;
*/
	// Database data
	IDatabase *pIDatabase;

	// Temperature data
	//IMediaUtil *pIMediaUtilPlay;
	//IMediaUtil *pIMediaUtilRec;
	//IMedia *pIMediaPlay;
	//IMedia *pIMediaRec;

	boolean tempActive;
	boolean tempVocOn;
	IVocoder *pIVocoder;
	IVocoderConfigType vocConfig;
	IVocoderInfoType vocInfo;
	byte frameDataIN[512];  // Buffer for reading and writing frames
	//byte frameDataOUT[512];  // Buffer for reading and writing frames
	byte *tempSoundOut;
	SuryaSoundMode soundMode;
	char AudioFilename[128];
	IFile *pIFileAudioOut;
	boolean headset_insert;

	// stats and display for temp
	int16 maxSound;
	int16 minSound;
	int cycleMax;
	int cycleCount;
    AECHAR tempDisp[64];
	AECHAR tempDispDBG[64];
	AECHAR tempDispFmt[16];
	AECHAR tempDispDBGFmt[16];
	AECHAR	maxtempsoudfmt[16];
	AECHAR maxtempsound[64];
	uint16 currVolume;

	ISound *pISound; // Sound pointer used for setting sound variables 
	AEESoundInfo soundInfo; // Holds info used for ISound_SetDevice

	// Upload image data
/*	IWeb *pIWeb;
	IWebResp *pIWebResp;
	IWebUtil *pIWebUtil;
	IMultiPeek *pIMultiPeek;
	IGetLine *pIGetLine;*/
//	AEECallback UPIMGPostCB;
	AEECallback UPIMGTimerCB;
	AEECallback Get_Current_TimerCB;
	uint16 DBUploadRecord;
	uint32 UPIMGStartTime;
	uint32 UPIMGStopTime;
	char *poststr;
	char *poststrFooter;
//	IPeek                *pipPostData;
	// Memory checks
	IHeap *pIHeap;
	//WebWork *communication;
//	IVersion *deviceverison;
	char *pszBuffV;
	uint32 Tempra_Data[240];
	uint32 temp_data_index;

	//set data variable
	char phone_id[10];
	char upload_version[10];
//	char *upload_curr_configration;
	char *upload_time;
	char *upload_Temprature_Data;
//	char *averging_window/*[2]*/; //
//	char *sample_duration;//
//	char *puase_interval;//
	char *sound_volume;
	boolean is_data_upload;
//	char *calibration_scalar;//
//	char *calibration_offset;//
//	char *calibration_devider;//
//	char curr_configration_param[100];
	char *min_number;
	char *Deployment_Id;

	ITAPI              *pITAPI;

	boolean Pause_Interval_Flag;
	boolean done_sample_time;
	boolean Add_Record_Flag;
	uint32 Tempt_Data_Lenght,SEC,Cur_Sec;
	char *Tempt_Data_str;
	boolean Upload_Data;
//	boolean upload_flag;
	boolean Get_Time_Flag;
	boolean is_database_null;
	boolean set_data_indexid;
	uint32  check_recod_fail_4times;
	boolean sound_play_by_key;
	//set data variable
} suryabrew;


/*****************************
* Constants 
******************************/

// this is bad... I need to figure out how to figure out how
// many pixels high a given font is. I think I can through
// the IFont interface... the 'ascent' and 'descent'
#define SURYABREW_TITLEBAR_HEIGHT 14

// The time in milliseconds to request a subsequent gps position
#define SURYABREW_GPS_REQUEST_INTERVAL 15000

#define SURYABREW_UPIMG_UPLOAD_INTERVAL 10000

#define SURYABREW_DB_FILE "suryabrew.db"

// #define AEEFS_CARD0_DIR "fs:/card0/"  This already exists... find first MMC/SD card
#define SURYABREW_DIR_PREFIX "suryabrew/"

#define SURYABREW_UPLOAD_LOC	/*	"http://googl.com"*/	"http://surya-dev.nexleaf.org/surya/upload/"	// "http://192.168.0.178/surya/brew2.php" //"http://192.168.0.178/surya/index.php?"  //"http://surya.nexleaf.org/surya/temp/upload"//"http://peir2.cens.ucla.edu/SuryaTest/upload_image/"

#define SURYABREW_MULTIPART_CONTENTTYPE "Content-Type: multipart/form-data; boundary=------------------0121314151617181910\r\n"
#define SURYABREW_MULTIPART_BOUNDRY "------------------0121314151617181910"

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/





/*******************************
* Core & Display
********************************/

static  boolean suryabrew_HandleEvent(suryabrew* pMe, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean suryabrew_InitAppData(suryabrew* pMe);
void    suryabrew_FreeAppData(suryabrew* pMe);
void suryabrew_FreeSurya(suryabrew *pMe, int from);
static void suryabrew_DrawScreen(suryabrew* pMe);
void suryabrew_DrawPhotoScreen(suryabrew *pMe);
void suryabrew_DrawTempScreen(suryabrew *pMe);
void suryabrew_DrawMessage(suryabrew *pMe);
void suryabrew_DrawCheckYesNo(suryabrew *pMe);
void suryabrew_DrawTempTemp(suryabrew *pMe);
void suryabrew_PrintMem(suryabrew *pMe);

/*******************************
* Camera Code
********************************/
/*
void suryabrew_CameraNotify(void * pUser, AEECameraNotify * pNotify);
int suryabrew_CameraLoad(suryabrew* pMe);
void suryabrew_CameraUnload(suryabrew* pMe);
int suryabrew_CameraInitSnapshot(suryabrew* pMe);
int suryabrew_CameraInitPreview(suryabrew* pMe);
void suryabrew_CameraDropImage(suryabrew* pMe);
int suryabrew_CameraStop(suryabrew* pMe); // Call when in preview mode
*/

/*******************************
* GPS Code
********************************/

/*int suryabrew_GPSInit(suryabrew* pMe);
void suryabrew_GPSUnload(suryabrew* pMe);
void suryabrew_GPSStop(suryabrew *pMe);
void suryabrew_GPSNotify(void *pData);
void suryabrew_GPSTimer(void *pData);
boolean suryabrew_GPSValid(suryabrew *pMe);

*/

/*******************************
* DB Code
********************************/



typedef enum
{
  DB_RECORD_FIELD_DEVICEID, //PHONE IMEI OR MEID
  DE_RECORD_FIELD_DEPLOYMENT_ID,
  DB_RECORD_FIELD_DEVICEVERSION,//VERSION ETC.
  DB_RECORD_FIELD_DATETIME,
  DB_RECORD_FIELD_DATA,
  DB_RECORD_FIELD_OUTPUTSOUNDVOLUME,
//  DB_RECORD_FIELD_IMAGEFILE,
//  DB_RECORD_FIELD_LAT,
//  DB_RECORD_FIELD_LON,
//  DB_RECORD_FIELD_ALT,
//  DB_RECORD_FIELD_HASGPS,
  DB_RECORD_FIELD_UPLOADED,
  NUM_DB_RECORD_FIELDS
} EDBRecordFieldType;

typedef struct _uploaddata {
//	uint32 datetime;
	char *MinNumber;
	char *device_version;
	char *deploymentid;
//	char *cur_parameter;
	char *date;
	char *temp_sample_data;
	char	*output_sound;
/*	char imagefile[128];
	double lat;
	double lon;*/
//	uint32/*int*/ alt; // rohit [8/10/2011]
//	boolean hasgps;
	boolean uploaded;
	uint16 DBRecordID;
} suryabrew_uploaddata;

void suryabrew_DBInit(suryabrew *pMe);
void suryabrew_DBUnload(suryabrew *pMe);
void suryabrew_DBInitFieldStruct(suryabrew *pMe, AEEDBField* pFieldArray, 
								   char *phoneid,char *deploym_id,char * d_verion,char *CurrDateSec,
								   char *data,char *outputsound,char *isuploaded);//  [8/10/2011]
void suryabrew_DBAddRecord(suryabrew *pMe);
int suryabrew_DBDEBUG(suryabrew *pMe);
int suryabrew_DBGetNotUploaded(suryabrew *pMe, suryabrew_uploaddata *ud);
void suryabrew_DBPopulateUploadData(IDBRecord *pRecord, suryabrew_uploaddata *ud);
void suryabrew_DBSetUploaded(suryabrew *pMe);
void suryabrew_Remove_DataBase(suryabrew *pMe);



//get time
void suryabrew_gettime(suryabrew *pMe);
void suryabrew_getcurrtime(suryabrew *pMe);
//gettime
/*******************************
* Temperature Code
********************************/
//void suryabrew_TempInit(suryabrew *pMe);
int suryabrew_TempInitVOC(suryabrew *pMe);
int suryabrew_Imedia(suryabrew *pMe);
//void suryabrew_TempStartVOC(suryabrew *pMe);
//void suryabrew_TempStopVOC(suryabrew *pMe);
void suryabrew_TempUnloadVOC(suryabrew *pMe);
void VocUse_SetActive(suryabrew* pMe, boolean active);
void suryabrew_TempLoadSound(suryabrew *pMe);
void suryabrew_TempGetVolume(suryabrew *pMe);
void suryabrew_TempVolUp(suryabrew *pMe);
void suryabrew_TempVolDown(suryabrew *pMe);
void suryabrew_TempPlayTone(suryabrew *pMe);
void suryabrew_TempStopTone(suryabrew *pMe);
boolean suryabrew_HeadphoneDetect(suryabrew *pMe);
void suryabrew_TempSetActive(suryabrew* pMe, boolean active);
/*******************************
* File & Dir Code
********************************/

void suryabrew_FileDeleteFile(suryabrew *pMe, char *filename);
void suryabrew_FileCreateDirPrefixDate(suryabrew *pMe);
void suryabrew_FileCreateDirPrefix(suryabrew *pMe, char *filename);
IFile* suryabrew_FileCreateFile(suryabrew *pMe, char *filename);


/*******************************
* upload Code
********************************/

void suryabrew_UPDATAInit(suryabrew *pMe);
void suryabrew_UPIMGStop(suryabrew *pMe);
void suryabrew_UPDataUnload(suryabrew *pMe);
void suryabrew_UPDATATimer(void *pData);
void suryabrew_UPIMGPostDone(void *pData);
void suryabrew_UPIMGStatusNotification(void *pNotifyData, WebStatus ws, void *pData);
void suryabrew_FileUnload(suryabrew *pMe);
//
char *convertByte2Char(char *dest,byte *source,int start,int end);
void suryabrew_setdata(suryabrew *pMe);
void suryabrew_resetdata(suryabrew *pMe);
void suryabrew_Get_Min_Number(suryabrew *pMe);
void emptychararray(char *str, uint32 length);
void copy_char_to_string(char *dest, const char *source);
//void convert_int_to_char(char	*dest, uint32 *source);

//char suryabrew_concret_time(suryabrew *pMe) ;
void suryabrew_FileCreateFileNamePrefix(suryabrew *pMe, char* filename);

void suryabrew_uploaddata_notify(suryabrew *pMe);



// in calibration.cdd
int suryabrew_TempCalcTemp(suryabrew* pMe);
double natural_log(const double in) ;
#endif // SURYABREW_H_H
