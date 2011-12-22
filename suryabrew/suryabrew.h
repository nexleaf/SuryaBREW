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
//#include "AEEStdDef.h"

#include "string.h"

#include "AEEHeap.h"
#include "AEEFile.h"			// File interface definitions
#include "AEEDB.h"				// Database interface definitions
#include "AEENet.h"				// Socket interface definitions
#include "AEECamera.h"          // Camera interface definitions
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
	boolean do_upload;              // try to uplaod files
    boolean do_upload_delete_file;  // delete the image and DB row after upload
	boolean do_gps;                 // run the gps subsystem
	boolean allow_volume;          // allow someone to change the volume
	boolean allow_playback;        // let the 5 key switch to echo mode
	boolean enable_camera;         // whether the camera is enabled and accessible
	boolean enable_datalogger;      // whether to enable the data logger mode

	SuryaModeType SuryaMode; // Home screen, taking pictures, or taking temp

	int m_cxWidth;     // Stores the device screen width
	int m_cyHeight;    // Stores the device screen height
	//int m_nCursorX;    // Stores the cursor bitmap x coordinate
	//int m_nCursorY;    // Stores the cursor bitmap y coordinate

	IImage * pIImage;  // IImage interface pointer for captured images
	IImage * pIImage_checkyes; // Image for accepting filter image
	IImage * pIImage_checkno;  // image for rejecting filter image

	// File management for directory creation
	IFileMgr *pIFileMgr;
	char FileNamePrefix[64];

	// Camera data
	ICamera *pICamera;
	AEEMediaData MediaData;
	int CameraMode;
	//AEEHWIDType* HWID;
	//char *IMEI;
	char *HWID;
	JulianType CurrDate;
	uint32 CurrDateSec;
	char ImageFilename[128];

	// GPS data
	IPosDet *pIPosDet;
	AEEPositionInfoEx GPSPosition;
	AEEGPSInfo GPSInfo;
	AEEGPSConfig GPSConfig;
	AEECallback GPSDataCB;
	AEECallback GPSTimerCB;
	boolean GPSValid;

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

	// try debugging by writing file to SD card 
	char AudioFilename[128];
	IFile *pIFileAudioOut;

	// Data logging features
	boolean dataloggerRunning;
	char DatalogCounterFilename[128];
	uint32 dataloggerCounter;
	uint32 dataloggerCounterIncrementInterval; // in samples?
	uint32 dataloggerCounterSamples;
    char DatalogFilename[128];
	IFile *pIFileDatalogger;

	// stats and display for temp
	int16 maxSound;
	int16 minSound;
	int cycleMax;
	int cycleCount;
	int tempCurr;
    AECHAR tempDisp[64];
	AECHAR tempDispDBG[64];
	AECHAR tempDispFmt[16];
	AECHAR tempDispFmtDataLog[16];
	AECHAR tempDispDBGFmt[16];
	uint16 currVolume;

	ISound *pISound; // Sound pointer used for setting sound variables 
	AEESoundInfo soundInfo; // Holds info used for ISound_SetDevice

	// Upload image data
	IWeb *pIWeb;
	IWebResp *pIWebResp;
	IWebUtil *pIWebUtil;
	IMultiPeek *pIMultiPeek;
	IGetLine *pIGetLine;
	AEECallback UPIMGPostCB;
	AEECallback UPIMGTimerCB;
	int32 DBUploadRecord;
	uint32 UPIMGStartTime;
	uint32 UPIMGStopTime;
	char *poststr;
	char *poststrFooter;

	// Memory checks
	IHeap *pIHeap;

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

#define SURYABREW_UPLOAD_LOC "http://peir2.cens.ucla.edu/SuryaTest/upload_image/"

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
void suryabrew_DrawCheckYesNo(suryabrew *pMe);
void suryabrew_DrawTempTemp(suryabrew *pMe);
void suryabrew_PrintMem(suryabrew *pMe);

/*******************************
* Camera Code
********************************/

void suryabrew_CameraNotify(void * pUser, AEECameraNotify * pNotify);
int suryabrew_CameraLoad(suryabrew* pMe);
void suryabrew_CameraUnload(suryabrew* pMe);
int suryabrew_CameraInitSnapshot(suryabrew* pMe);
int suryabrew_CameraInitPreview(suryabrew* pMe);
void suryabrew_CameraDropImage(suryabrew* pMe);
int suryabrew_CameraStop(suryabrew* pMe); // Call when in preview mode


/*******************************
* GPS Code
********************************/

int suryabrew_GPSInit(suryabrew* pMe);
void suryabrew_GPSUnload(suryabrew* pMe);
void suryabrew_GPSStop(suryabrew *pMe);
void suryabrew_GPSNotify(void *pData);
void suryabrew_GPSTimer(void *pData);
boolean suryabrew_GPSValid(suryabrew *pMe);



/*******************************
* DB Code
********************************/

typedef enum
{
  DB_RECORD_FIELD_DATETIME,
  DB_RECORD_FIELD_IMAGEFILE,
  DB_RECORD_FIELD_LAT,
  DB_RECORD_FIELD_LON,
  DB_RECORD_FIELD_ALT,
  DB_RECORD_FIELD_HASGPS,
  DB_RECORD_FIELD_UPLOADED,
  NUM_DB_RECORD_FIELDS
} EDBRecordFieldType;

typedef struct _uploaddata {
	uint32 datetime;
	char imagefile[128];
	double lat;
	double lon;
	int alt;
	boolean hasgps;
	boolean uploaded;
	uint16 DBRecordID;
} suryabrew_uploaddata;

void suryabrew_DBInit(suryabrew *pMe);
void suryabrew_DBUnload(suryabrew *pMe);
void suryabrew_DBInitFieldStruct(suryabrew *pMe, AEEDBField* pFieldArray, 
								   uint32 *CurrDateSec, char *ImageFilename,
								   double * Latitude, double *Longitude,
								   int *nAltitude, boolean *GPSValid, char *uploaded);
void suryabrew_DBAddRecord(suryabrew *pMe);
void suryabrew_DBDEBUG(suryabrew *pMe);
int suryabrew_DBGetNotUploaded(suryabrew *pMe, suryabrew_uploaddata *ud);
void suryabrew_DBPopulateUploadData(IDBRecord *pRecord, suryabrew_uploaddata *ud);
void suryabrew_DBSetUploaded(suryabrew *pMe);



/*******************************
* Temperature Code
********************************/
//void suryabrew_TempInit(suryabrew *pMe);
int suryabrew_TempInitVOC(suryabrew *pMe);
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
int suryabrew_TempCalcTemp(suryabrew* pMe);

/*******************************
* File & Dir Code
********************************/

void suryabrew_FileDeleteFile(suryabrew *pMe, char *filename);
boolean suryabrew_FileExists(suryabrew *pMe, char *filename, int *size); // last arg can be null
void suryabrew_FileCreateDirPrefixDate(suryabrew *pMe);
void suryabrew_FileCreateDirPrefix(suryabrew *pMe, char *filename);
IFile* suryabrew_FileCreateFile(suryabrew *pMe, char *filename, boolean *exists); // last arg can be null
IFile* suryabrew_FileAppendOrCreateFile(suryabrew *pMe, char *filename, boolean *exists); // last arg can be null
IFile* suryabrew_FileGetForRead(suryabrew *pMe, char *filename);

/*******************************
* upload Code
********************************/

void suryabrew_UPIMGInit(suryabrew *pMe);
void suryabrew_UPIMGStop(suryabrew *pMe);
void suryabrew_UPIMGUnload(suryabrew *pMe);
void suryabrew_UPIMGTimer(void *pData);
void suryabrew_UPIMGPostDone(void *pData);
void suryabrew_UPIMGStatusNotification(void *pNotifyData, WebStatus ws, void *pData);


/*******************************
* datalogger code
********************************/
void suryabrew_DataLoggerInit(suryabrew *pMe);
void suryabrew_DataLoggerUnload(suryabrew *pMe);
void suryabrew_DataLoggerLog(suryabrew *pMe);
__inline void suryabrew_DataLoggerIncremenetSamples(suryabrew *pMe, int count);
void suryabrew_DataLoggerIncrementCounter(suryabrew *pMe);
void suryabrew_DataLoggerLog(suryabrew *pMe); // call at same time when displaying


#endif // SURYABREW_H_H
