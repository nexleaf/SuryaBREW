
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
	}

}


void suryabrew_DBInitFieldStruct(suryabrew *pMe, AEEDBField* pFieldArray, 
								   uint32 *CurrDateSec, char *ImageFilename,
								   double * Latitude,
								   double *Longitude,
								   int *nAltitude,
								   boolean *GPSValid,
								   char *uploaded)
{

	//// Fill in the date/time field

	pFieldArray[ DB_RECORD_FIELD_DATETIME ].fType = AEEDB_FT_DWORD;
	pFieldArray[ DB_RECORD_FIELD_DATETIME ].fName = AEEDBFIELD_DATE_TIME;
	pFieldArray[ DB_RECORD_FIELD_DATETIME ].wDataLen = sizeof( pMe->CurrDateSec);
	pFieldArray[ DB_RECORD_FIELD_DATETIME ].pBuffer = CurrDateSec;

	//// Fill in the file name

	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].fType = AEEDB_FT_BINARY;
	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].wDataLen = STRLEN( pMe->ImageFilename ) + 1;
	pFieldArray[ DB_RECORD_FIELD_IMAGEFILE ].pBuffer = ImageFilename;

	//// Fill in the Lat Lon and Alititude

	pFieldArray[ DB_RECORD_FIELD_LAT ].fType = AEEDB_FT_BINARY;
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

	//// Have GPS lock?

	pFieldArray[ DB_RECORD_FIELD_HASGPS ].fType = AEEDB_FT_BYTE;
	pFieldArray[ DB_RECORD_FIELD_HASGPS ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_HASGPS ].wDataLen = sizeof( char );
	pFieldArray[ DB_RECORD_FIELD_HASGPS ].pBuffer = GPSValid;

	//// UPloaded? Not yet

	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].fType = AEEDB_FT_BYTE;
	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].fName = AEEDBFIELD_NONE;
	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].wDataLen = sizeof( char );
	pFieldArray[ DB_RECORD_FIELD_UPLOADED ].pBuffer = uploaded;

}


void suryabrew_DBAddRecord(suryabrew *pMe)
{
	boolean uploaded = FALSE;
	IDBRecord *pRecord = NULL;
	AEEDBField *pFieldArray = (AEEDBField*) MALLOC(sizeof(AEEDBField) * NUM_DB_RECORD_FIELDS);

	if (pFieldArray == NULL)
	{
		return;
	}

	suryabrew_DBInitFieldStruct(pMe, pFieldArray, &pMe->CurrDateSec, pMe->ImageFilename,
		&pMe->GPSPosition.Latitude, &pMe->GPSPosition.Longitude, &pMe->GPSPosition.nAltitude,
		&pMe->GPSValid, &uploaded);

	pRecord = IDATABASE_CreateRecord(pMe->pIDatabase, pFieldArray, NUM_DB_RECORD_FIELDS);

	if (pRecord != NULL)
	{
		IDBRECORD_Release(pRecord);
	}
	FREE(pFieldArray);
}


void suryabrew_DBSetUploaded(suryabrew *pMe)
{

	IDBRecord *pRecord = NULL;

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
		ud->uploaded = TRUE;

		suryabrew_DBInitFieldStruct(pMe, pFieldArray, &ud->datetime, ud->imagefile, 
			&ud->lat, &ud->lon, &ud->alt, &ud->hasgps, &ud->uploaded);

		IDBRECORD_Update(pRecord, (AEEDBField *) pFieldArray, NUM_DB_RECORD_FIELDS);
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

	IDBRecord *pRecord = NULL;
	IDATABASE_Reset(pMe->pIDatabase);

	while ( (pRecord = IDATABASE_GetNextRecord(pMe->pIDatabase)) != NULL)
	{

		suryabrew_DBPopulateUploadData(pRecord, ud);

		IDBRECORD_Release(pRecord);
		if (ud->uploaded == FALSE)
		{
			DBGPRINTF("DBGetNotUploaded Rec %d File %s", ud->DBRecordID, ud->imagefile);
			return SUCCESS;
		}
	}

	return EFAILED;
}

void suryabrew_DBPopulateUploadData(IDBRecord *pRecord, suryabrew_uploaddata *ud)
{

	char *filename = NULL;

	uint16 nFieldLen;
	AEEDBFieldName rFieldName;
	AEEDBFieldType rFieldType;

	ud->DBRecordID = IDBRECORD_GetID(pRecord);

	IDBRECORD_Reset(pRecord); // Set the rec index to 0;

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	IDBRECORD_GetFieldDWord( pRecord, &ud->datetime);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	filename = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
	STRCPY(ud->imagefile, filename);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->lat = *(double *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->lon = *(double *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	IDBRECORD_GetFieldDWord( pRecord, &ud->alt);

	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->hasgps = *(boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		
	rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
	ud->uploaded = *(boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);

}




void suryabrew_DBDEBUG(suryabrew *pMe)
{

	IDBRecord *pRecord = NULL;
	char *filename = NULL;
	double *latlon = NULL;
	boolean *check = NULL;
	uint32 time = 0;
	int alt =0;
	uint32 now = GETTIMESECONDS();
	int count = 0;
	IDATABASE_Reset(pMe->pIDatabase);

	while ( (pRecord = IDATABASE_GetNextRecord(pMe->pIDatabase)) != NULL)
	{

		uint16 nFieldLen;
		AEEDBFieldName rFieldName;
		AEEDBFieldType rFieldType;

		DBGPRINTF("DBDEBUG --- RecID:       %d", IDBRECORD_GetID(pRecord));

		IDBRECORD_Reset(pRecord); // Set the rec index to 0;

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		IDBRECORD_GetFieldDWord( pRecord, &time);
		DBGPRINTF("DBDEBUG --- OBSAGO: %d", now - time);

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		filename = (char *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG --- FILNME: %s", filename);


		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		latlon = (double *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    LAT: %d", FLTTOINT(FMUL(*latlon, 1000.0)));

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		latlon = (double *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    LON: %d", FLTTOINT(FMUL(*latlon, 1000.0)) );

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		IDBRECORD_GetFieldDWord( pRecord, &alt);
		DBGPRINTF("DBDEBUG ---    ALT: %d", alt);


		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		check = (boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---    GPS: %d", *check );

		rFieldType = IDBRECORD_NextField(pRecord, &rFieldName, &nFieldLen);
		check = (boolean *) IDBRECORD_GetField(pRecord, &rFieldName, &rFieldType, &nFieldLen);
		DBGPRINTF("DBDEBUG ---     UP: %d", *check );

		count = count + 1;
		IDBRECORD_Release(pRecord);
	}
	DBGPRINTF("DBDEBUG --- Total: %d", count);
}
