
#include "suryabrew.h"

/*
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
*/

void suryabrew_DBUnload(suryabrew *pMe)
{
	if ( pMe->pIDatabase != NULL)
	{
		IDATABASE_Release(pMe->pIDatabase);
		pMe->pIDatabase = NULL;
	}
}

void suryabrew_DBInit(suryabrew *pMe)
{
	int nErr = 0;
	IDBMgr *pDBMgr = NULL;

	nErr = ISHELL_CreateInstance(pMe->pIShell, AEECLSID_DBMGR, (void**)(&pDBMgr));
	if ( nErr != SUCCESS)
	{
		DBGPRINTF("HandleEvent Unable to create DBMgr instance %d", nErr);
	}
	if (pDBMgr != NULL)
	{
		suryabrew_FileCreateFileNamePrefix(pMe, SURYABREW_DB_FILE);
		pMe->pIDatabase = IDBMGR_OpenDatabase(pDBMgr, pMe->FileNamePrefix, TRUE);
		if (pMe->pIDatabase == NULL) {
			IDBMGR_Release(pDBMgr);
		}
		else
		{
			IDBMGR_Release(pDBMgr);
		}
		DBGPRINTF("db is created");
	}

}


void suryabrew_DBInitFieldStruct(suryabrew *pMe, AEEDBField* pFieldArray, 
								   char *phoneid,char *deploym_id,char *d_version,char *CurrDateSec,
								   char *data,char *output_sound_volume,char *isuploaded)
{
	DBGPRINTF("IN SURYABREW_DBINITFIELDSTRUCT");



	//fill phonid
	DBGPRINTF("DB_RECORD_FIELD_DEVICEID===%d,%d,%d%,%s\n",pFieldArray[DB_RECORD_FIELD_DEVICEID ].fType,pFieldArray[ DB_RECORD_FIELD_DEVICEID ].fName,	pFieldArray[ DB_RECORD_FIELD_DEVICEID ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DEVICEID ].pBuffer);
		pFieldArray[DB_RECORD_FIELD_DEVICEID ].fType = AEEDB_FT_STRING;
	pFieldArray[ DB_RECORD_FIELD_DEVICEID ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_DEVICEID ].wDataLen = sizeof( phoneid);
	pFieldArray[ DB_RECORD_FIELD_DEVICEID ].pBuffer = phoneid;
	//
	DBGPRINTF("DB_RECORD_FIELD_DEVICEID===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_DEVICEID ].fType,pFieldArray[ DB_RECORD_FIELD_DEVICEID ].fName,	pFieldArray[ DB_RECORD_FIELD_DEVICEID ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DEVICEID ].pBuffer);
	DBGPRINTF("phoneid==>%s",phoneid);
	//fill deployment id
	DBGPRINTF("DE_RECORD_FIELD_DEPLOYMENT_ID===%d,%d,%d,%s\n\t",pFieldArray[DE_RECORD_FIELD_DEPLOYMENT_ID ].fType,pFieldArray[ DE_RECORD_FIELD_DEPLOYMENT_ID ].fName,	pFieldArray[ DE_RECORD_FIELD_DEPLOYMENT_ID ].wDataLen,pFieldArray[ DE_RECORD_FIELD_DEPLOYMENT_ID ].pBuffer);
		
		pFieldArray[DE_RECORD_FIELD_DEPLOYMENT_ID].fType = AEEDB_FT_STRING;
	pFieldArray[DE_RECORD_FIELD_DEPLOYMENT_ID].fName = AEEDBFIELD_NONE;
	pFieldArray[DE_RECORD_FIELD_DEPLOYMENT_ID].wDataLen = sizeof( deploym_id);
	pFieldArray[DE_RECORD_FIELD_DEPLOYMENT_ID].pBuffer = deploym_id;
	//
	DBGPRINTF("DE_RECORD_FIELD_DEPLOYMENT_ID===%d,%d,%d,%s\n\t",pFieldArray[DE_RECORD_FIELD_DEPLOYMENT_ID ].fType,pFieldArray[ DE_RECORD_FIELD_DEPLOYMENT_ID ].fName,	pFieldArray[ DE_RECORD_FIELD_DEPLOYMENT_ID ].wDataLen,pFieldArray[ DE_RECORD_FIELD_DEPLOYMENT_ID ].pBuffer);
	DBGPRINTF(" deploym_id==>%s", deploym_id);

		//fill device version
	DBGPRINTF("DB_RECORD_FIELD_DEVICEVERSION===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_DEVICEVERSION ].fType,pFieldArray[ DB_RECORD_FIELD_DEVICEVERSION ].fName,	pFieldArray[ DB_RECORD_FIELD_DEVICEVERSION ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DEVICEVERSION ].pBuffer);
		pFieldArray[DB_RECORD_FIELD_DEVICEVERSION].fType = AEEDB_FT_STRING;
	pFieldArray[DB_RECORD_FIELD_DEVICEVERSION].fName = AEEDBFIELD_NONE;
	pFieldArray[DB_RECORD_FIELD_DEVICEVERSION].wDataLen = sizeof(d_version);
	pFieldArray[DB_RECORD_FIELD_DEVICEVERSION].pBuffer = d_version;
	DBGPRINTF("DB_RECORD_FIELD_DEVICEVERSION===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_DEVICEVERSION ].fType,pFieldArray[ DB_RECORD_FIELD_DEVICEVERSION ].fName,	pFieldArray[ DB_RECORD_FIELD_DEVICEVERSION ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DEVICEVERSION ].pBuffer);
	DBGPRINTF(" version==>%s", pFieldArray[DB_RECORD_FIELD_DEVICEVERSION].pBuffer);
	//
	

	//// Fill in the date/time field


	DBGPRINTF("DB_RECORD_FIELD_DATETIME===%s\n\t",pFieldArray[ DB_RECORD_FIELD_DATETIME ].pBuffer);

	pFieldArray[ DB_RECORD_FIELD_DATETIME ].fType = AEEDB_FT_STRING;
	pFieldArray[ DB_RECORD_FIELD_DATETIME ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_DATETIME ].wDataLen = sizeof(pMe->CurrDateSec);
	pFieldArray[ DB_RECORD_FIELD_DATETIME ].pBuffer = CurrDateSec;
		DBGPRINTF("DB_RECORD_FIELD_DATETIME===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_DATETIME ].fType,pFieldArray[ DB_RECORD_FIELD_DATETIME ].fName,	pFieldArray[ DB_RECORD_FIELD_DATETIME ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DATETIME ].pBuffer);
	DBGPRINTF(" recorddatetime==>%s", pFieldArray[ DB_RECORD_FIELD_DATETIME ].pBuffer);
	DBGPRINTF(" recorddatetime==>%s", CurrDateSec);

	




	

	//fill temprature data
	DBGPRINTF("DB_RECORD_FIELD_DATA===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_DATA ].fType,pFieldArray[ DB_RECORD_FIELD_DATA ].fName,	pFieldArray[ DB_RECORD_FIELD_DATA ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DATA ].pBuffer);
		pFieldArray[ DB_RECORD_FIELD_DATA ].fType = AEEDB_FT_STRING;
	pFieldArray[ DB_RECORD_FIELD_DATA ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_DATA ].wDataLen = sizeof( data);
	pFieldArray[ DB_RECORD_FIELD_DATA ].pBuffer = data;
	DBGPRINTF("DB_RECORD_FIELD_DATA===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_DATA ].fType,pFieldArray[ DB_RECORD_FIELD_DATA ].fName,	pFieldArray[ DB_RECORD_FIELD_DATA ].wDataLen,pFieldArray[ DB_RECORD_FIELD_DATA ].pBuffer);
	DBGPRINTF("data==>%s", pFieldArray[ DB_RECORD_FIELD_DATA ].pBuffer);


	DBGPRINTF("DB_RECORD_FIELD_OUTPUTSOUNDVOLUME===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].fType,pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].fName,	pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].wDataLen,pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].pBuffer);
	pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].fType = AEEDB_FT_STRING;
	pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].wDataLen = sizeof(output_sound_volume);
	pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].pBuffer = output_sound_volume;
	//// Fill in the file name
	DBGPRINTF("DB_RECORD_FIELD_OUTPUTSOUNDVOLUME===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].fType,pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].fName,	pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].wDataLen,pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].pBuffer);
	DBGPRINTF(" output_sound_volume==>%s", pFieldArray[ DB_RECORD_FIELD_OUTPUTSOUNDVOLUME ].pBuffer);
	
/*	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].fType = AEEDB_FT_BINARY;
	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].wDataLen = STRLEN( pMe->ImageFilename ) + 1;
	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].pBuffer = ImageFilename;
*/
	//// Fill in the Lat Lon and Alititude

/*	pFieldArray[ DB_RECORD_FIELD_LAT ].fType = AEEDB_FT_BINARY;
	pFieldArray[ DB_RECORD_FIELD_LAT ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_LAT ].wDataLen = sizeof( double );
	pFieldArray[ DB_RECORD_FIELD_LAT ].pBuffer = Latitude;

	pFieldArray[ DB_RECORD_FIELD_LON ].fType = AEEDB_FT_BINARY;
	pFieldArray[ DB_RECORD_FIELD_LON ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_LON ].wDataLen = sizeof( double );
	pFieldArray[ DB_RECORD_FIELD_LON ].pBuffer = Longitude;

	pFieldArray[ DB_RECORD_FIELD_ALT ].fType = AEEDB_FT_DWORD;
	pFieldArray[ DB_RECORD_FIELD_ALT ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_ALT ].wDataLen = sizeof( int );
	pFieldArray[ DB_RECORD_FIELD_ALT ].pBuffer = nAltitude;
*/
	//// Have GPS lock?

	/*pFieldArray[ DB_RECORD_FIELD_HASGPS ].fType = AEEDB_FT_BYTE;
	pFieldArray[ DB_RECORD_FIELD_HASGPS ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_HASGPS ].wDataLen = sizeof( char );
	pFieldArray[ DB_RECORD_FIELD_HASGPS ].pBuffer = GPSValid;*/

	//// UPloaded? Not yet
DBGPRINTF("DB_RECORD_FIELD_UPLOADED===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_UPLOADED ].fType,pFieldArray[ DB_RECORD_FIELD_UPLOADED ].fName,	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].wDataLen,pFieldArray[ DB_RECORD_FIELD_UPLOADED ].pBuffer);


	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].fType = AEEDB_FT_STRING;
	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].wDataLen = sizeof(isuploaded );
	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].pBuffer = isuploaded;
	DBGPRINTF("DB_RECORD_FIELD_UPLOADED===%d,%d,%d,%s\n\t",pFieldArray[DB_RECORD_FIELD_UPLOADED ].fType,pFieldArray[ DB_RECORD_FIELD_UPLOADED ].fName,	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].wDataLen,pFieldArray[ DB_RECORD_FIELD_UPLOADED ].pBuffer);

	DBGPRINTF(" isuploaded==>%s", pFieldArray[ DB_RECORD_FIELD_UPLOADED ].pBuffer);

}


void suryabrew_DBAddRecord(suryabrew *pMe)
{
	
	IDBRecord *pRecord = NULL;
	AEEDBField *pFieldArray = (AEEDBField*) MALLOC(sizeof(AEEDBField) * NUM_DB_RECORD_FIELDS);

	if (pFieldArray == NULL)
	{
		return;
	}

	/*suryabrew_DBInitFieldStruct(pMe, pFieldArray, &pMe->CurrDateSec, pMe->ImageFilename,
		&pMe->GPSPosition.Latitude, &pMe->GPSPosition.Longitude, (uint32*)&pMe->GPSPosition.nAltitude,
		&pMe->GPSValid,&uploaded);*/
	suryabrew_DBInitFieldStruct(pMe, pFieldArray, pMe->min_number,pMe->Deployment_Id,(char*)pMe->upload_version, pMe->upload_time/*pMe->phone_id*/,
		 pMe->upload_Temprature_Data,pMe->sound_volume,"FALSE");
	if(pMe->pIDatabase!=NULL)
	pRecord = IDATABASE_CreateRecord(pMe->pIDatabase, pFieldArray, NUM_DB_RECORD_FIELDS);

	if (pRecord != NULL)
	{	
		DBGPRINTF("PRECORD RELEASE IN DBADDRECORED");
		
//		pMe->upload_flag=TRUE;
		IDBRECORD_Release(pRecord);
	}
	FREE(pFieldArray);
}


void suryabrew_DBSetUploaded(suryabrew *pMe)
{
	IDBRecord *pRecord = NULL;
	int nErr=0;
	if (pMe->DBUploadRecord < 0)
	{
		DBGPRINTF("DBSetUploaded Which record do you want to update?");
		return;
	}


	pRecord = IDATABASE_GetRecordByID(pMe->pIDatabase, pMe->DBUploadRecord);
	if (pRecord != NULL)
	{
		AEEDBField *pFieldArray = (AEEDBField*) MALLOC(sizeof(AEEDBField) * NUM_DB_RECORD_FIELDS);
		suryabrew_uploaddata *ud = (suryabrew_uploaddata *) MALLOC(sizeof(suryabrew_uploaddata));

		suryabrew_DBPopulateUploadData(pRecord, ud);
	//	ud->uploaded = TRUE;

		suryabrew_DBInitFieldStruct(pMe, pFieldArray, ud->MinNumber, ud->deploymentid, 
			ud->device_version, ud->date, ud->temp_sample_data, ud->output_sound, "TRUE");
	//	pMe->set_data_indexid=FALSE;
		nErr= IDBRECORD_Update(pRecord, (AEEDBField *) pFieldArray, NUM_DB_RECORD_FIELDS);
		if(nErr==SUCCESS)
		{
			pMe->DBUploadRecord=-1;
		}
		IDBRECORD_Release(pRecord);

		FREE(pFieldArray);
		FREE(ud);
	}
	else
	{
		DBGPRINTF("DBSetUploaded Could not find record %d", pMe->DBUploadRecord);
	}




}



int suryabrew_DBGetNotUploaded(suryabrew *pMe, suryabrew_uploaddata *ud)
{
	int No_Of_Record=0;
	IDBRecord *pRecord = NULL;
	//for build date
	JulianType CurrDate;
	char builddate[3];
	uint32 CurrDateSec;
	//for build date



	DBGPRINTF("in suryabrew dbgetnot uploaded");
	IDATABASE_Reset(pMe->pIDatabase);

	MEMSET(pMe->poststr,0,sizeof(char) * 4096);
//	No_Of_Record=suryabrew_DBDEBUG(pMe);
//if(No_Of_Record!=0)
{
	if((pRecord = IDATABASE_GetRecordByID(pMe->pIDatabase, pMe->DBUploadRecord)) != NULL)
	{
		DBGPRINTF("database not null");
		suryabrew_DBPopulateUploadData(pRecord, ud);
		pMe->Upload_Data=TRUE;
	

	//	SPRINTF(pMe->poststr,"mime_type=%s & data_type=%s & device_id=%s & version=%s  & deployment_id=%s &  record_datetime=%s & data=%s &misc=averagingwindow:%d,sampleduration:%d,pauseinterval:%d,outputsoundvolume:%s,calibraionparameters:,calibraionscalar:%d,calibraionoffset:%d,calibrationdivider:%d",MIME_TYPE,DATA_TYPE,ud->MinNumber,ud->device_version,ud->deploymentid,ud->date,ud->temp_sample_data,AVERGING_WINDOW,SAMPLE_DURATION,PUASE_INTERVAL,ud->output_sound,CALIBRATION_SCALAR,CALIBRATION_OFFSET,CALIBRATION_DIVIDER);
		// 
		MEMSET(builddate,0,sizeof(builddate));
//		SPRINTF(builddate,"%s",suryabrew_concret_time(pMe));
		{
		CurrDateSec = GETTIMESECONDS();
		GETJULIANDATE(CurrDateSec, &CurrDate);
		DBGPRINTF("build date==> %d%d%d%d%d%d\n",CurrDate.wYear,CurrDate.wMonth,CurrDate.wDay,CurrDate.wHour,CurrDate.wMinute,CurrDate.wSecond);
		MEMSET(builddate,0,sizeof(builddate));
		SPRINTF(builddate,"%d-%d-%d",CurrDate.wYear,CurrDate.wMonth,CurrDate.wDay);

		}

			SPRINTF(pMe->poststr,"device_id=%s&deployment_id=%s&version=%s.%s&record_datetime=%s&data_type=%s&mime_type=%s&data=%s&misc=averagingwindow:%d,samplingduration:%d,pauseinterval:%d,outputsoundvolume:%s,calibraionscalar:%d,calibraionoffset:%d,calibrationdivider:%d",ud->MinNumber,ud->deploymentid,ud->device_version,builddate,ud->date,DATA_TYPE,MIME_TYPE,ud->temp_sample_data,AVERGING_WINDOW,SAMPLE_DURATION,PUASE_INTERVAL,ud->output_sound,CALIBRATION_SCALAR,CALIBRATION_OFFSET,CALIBRATION_DIVIDER);
	//	SPRINTF(pMe->poststr,"device_id=A100000MARTIN1&deployment_id=suryatest&version=console&record_datetime=20111007195000&data_type=temp&mime_type=text/csv&data=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59&misc=averagingwindow:1,samplingduration:60,pauseinterval:240,outputsoundvolume:5,calibrationscalar:220,calibrationoffset:15300");
		//	SPRINTF(pMe->poststr,"device_id=%s",DEVICEID);
			DBGPRINTF("pme->poststr==>%s",pMe->poststr);
			IDBRECORD_Release(pRecord);
	}
}
	if(pMe->Upload_Data==TRUE)
	{
		return SUCCESS;
	}
	return EFAILED;
}

void suryabrew_DBPopulateUploadData(IDBRecord *pRecord, suryabrew_uploaddata *ud)
{

	char *filename = NULL;

	uint16 nFieldLen;
	AEEDBFieldName rFieldName;
	AEEDBFieldType rFieldType;
	DBGPRINTF("in suryabrew dbpupulate uploaddata");
	ud->DBRecordID = IDBRECORD_GetID(pRecord);

	IDBRECORD_Reset(pRecord); // Set the rec index to 0;

/*	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	IDBRECORD_GetFieldDWord( pRecord, &ud->datetime);*/

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->MinNumber = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	DBGPRINTF("UD MIN .NO==>%s",ud->MinNumber);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->deploymentid = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	DBGPRINTF("UD DEPLOYMENT ID==>%s",ud->deploymentid);
//	STRCPY(ud->imagefile, filename);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->device_version = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	DBGPRINTF("UD DEVICE VERSION==>%s",ud->device_version);
//	ud->lat = *(double *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);


		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->date = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	DBGPRINTF("UD RECORD DATE==>%s",ud->date);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->temp_sample_data = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	DBGPRINTF("UD SAMPLE DATA==>%s",ud->temp_sample_data);
//	ud->lon = *(double *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);


//	IDBRECORD_GetFieldDWord( pRecord, &ud->date);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->output_sound = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	DBGPRINTF("UD O/P SOUND==>%s",ud->output_sound);
//	ud->hasgps = *(boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);


//	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
//	ud->uploaded = *(boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);


	

}




int suryabrew_DBDEBUG(suryabrew *pMe)
{

	IDBRecord *pRecord = NULL;
	char *deploy_id=NULL;
	char *Get_Time=NULL;
	char *Get_Min_Number=NULL;
	char *Get_Device_Version=NULL;
	char	*Get_Tempt_Data=NULL;
	char	*outputsoundvolume=NULL;
	uint32 count = 0,total_record=0,check_no_true=0;
	char *check=NULL;
	pMe->is_database_null=FALSE;
	IDATABASE_Reset(pMe->pIDatabase);

	while( (pRecord = IDATABASE_GetNextRecord(pMe->pIDatabase)) != NULL)
	{

		uint16 nFieldLen;
		AEEDBFieldName rFieldName;
		AEEDBFieldType rFieldType;

		DBGPRINTF("DBDEBUG --- RecID:       %d", IDBRECORD_GetID(pRecord));

		IDBRECORD_Reset(pRecord);  //Set the rec index to 0;



		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		Get_Min_Number = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    Min Number or Device id: %s", Get_Min_Number);

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		deploy_id = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    DEPLOYMENT ID: %s", deploy_id);

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		Get_Device_Version = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    Device Version: %s", Get_Device_Version);

		

		

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		Get_Time = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    upload time: %s", Get_Time);

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		Get_Tempt_Data = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    Tmprature Data: %s", Get_Tempt_Data);


			rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		outputsoundvolume = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    OUTPUT SOUND VOLUME: %s", outputsoundvolume);
	//	DBGPRINTF("DBDEBUG ---    LAT: %d", FLTTOINT(FMUL(*latlon, 1000.0)));

	/*	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		latlon = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	//	DBGPRINTF("DBDEBUG ---    LON: %d", FLTTOINT(FMUL(*latlon, 1000.0)) );

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		IDBRECORD_GetFieldDWord( pRecord, &alt);
		DBGPRINTF("DBDEBUG ---    ALT: %d", alt);


		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	//	check = (boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	//	DBGPRINTF("DBDEBUG ---    GPS: %d", *check );
*/
		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		check = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---     UP: %s", check );
		if(STRCMP(check,"FALSE")==0 && pMe->set_data_indexid==FALSE)
		{
		
		count = count + 1;
		pMe->set_data_indexid=TRUE;
		pMe->DBUploadRecord=IDBRECORD_GetID(pRecord);
	//	return count;
		}
		total_record++;
		IDBRECORD_Release(pRecord);
		if(STRCMP(check,"TRUE")==0)
		{
			check_no_true++;
		}
		
		
	}
	DBGPRINTF("DBDEBUG --- Total UNUPLAOD RECORD: %d", count);
	DBGPRINTF("DBDEBUG --- Total  RECORD: %d",total_record);
	if((check_no_true==total_record) && (total_record!=0))
	pMe->is_database_null=TRUE;

	return count;
}
