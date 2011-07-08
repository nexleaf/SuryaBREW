

#include "suryabrew.h"

/*
	char DatalogCounterFilename[128];
	uint32 dataloggerCounter;
	uint32 dataloggerCounterIncrementInterval; // in samples?
	uint32 dataloggerCounterSamples;
    char DatalogFilename[128];
	IFile *pIFileDatalogger;
*/

void suryabrew_DataLoggerInit(suryabrew *pMe)
{
	int nErr = 0;
	char tempstr[128];

	if (pMe->dataloggerRunning == TRUE) {
		return;
	}

	nErr = suryabrew_DataLoggerGetOrCreateCounter(pMe);
	if (nErr) {
			DBGPRINTF("Error creating coutner file!");
			pMe->dataloggerRunning = FALSE;
			return;
	}

	// get or create the counter value from the file
	SPRINTF(tempstr, "%06u_dl.csv", pMe->dataloggerCounter);
	suryabrew_FileCreateFileNamePrefix(pMe, tempstr);
	SPRINTF(pMe->DatalogFilename, pMe->FileNamePrefix);
	
	DBGPRINTF("%s", pMe->DatalogFilename);

	// open the file for writing/appending
	pMe->pIFileDatalogger = suryabrew_FileAppendOrCreateFile(pMe, pMe->DatalogFilename, NULL); 
	
	if (pMe->pIFileDatalogger != NULL) {
		pMe->dataloggerRunning = TRUE;
	}

}

void suryabrew_DataLoggerUnload(suryabrew *pMe)
{

	// flush output of counter and datalog file
	
	// close the ifile

	if (pMe->pIFileDatalogger != NULL) {
		IFILE_Release(pMe->pIFileDatalogger);
		pMe->pIFileDatalogger = NULL;
	}

	pMe->dataloggerRunning = FALSE;

}


void suryabrew_DataLoggerLog(suryabrew *pMe) 
{
    int temp = 0;
	int res = 0;
	int res2 = 0;
	char outstr[256];
	JulianType jt;

	if (pMe->dataloggerRunning == FALSE) {
		return;
	}

	temp = suryabrew_TempCalcTemp(pMe); //(pMe->maxSound * 258 + 360000)/10000;

	GETJULIANDATE(GETUTCSECONDS(), &jt);

	res = SPRINTF(outstr, "%u,%u,%u,%u%02u%02u%02u%02u%02u,%d,%d,%d,%d,%d,%d\n", 
		pMe->dataloggerCounter,
		GETUTCSECONDS(),
		GETUPTIMEMS(),
		jt.wYear, jt.wMonth, jt.wDay, jt.wHour, jt.wMinute, jt.wSecond,
		temp,
		pMe->maxSound,
		pMe->minSound,
		449,
		168995,
		10000);

	res2 = IFILE_Write(pMe->pIFileDatalogger, outstr, res);
	//DBGPRINTF("Wrote %d of %d bytes to log", res2, res);
	//DBGPRINTF("%s", outstr);

	if (pMe->dataloggerCounterSamples > pMe->dataloggerCounterIncrementInterval) {
		suryabrew_DataLoggerIncrementCounter(pMe);
	}
}


__inline void suryabrew_DataLoggerIncremenetSamples(suryabrew *pMe, int count) 
{
	pMe->dataloggerCounterSamples = pMe->dataloggerCounterSamples + count;
}

void suryabrew_DataLoggerIncrementCounter(suryabrew *pMe)
{
	int nErr = 0;
	IFile *cfile = NULL;
	
	if (pMe->dataloggerRunning == FALSE) {
		return;
	}

	// overwrites whats there
	cfile = suryabrew_FileCreateFile(pMe, pMe->DatalogCounterFilename, NULL);

	if (cfile != NULL) {
		pMe->dataloggerCounter = pMe->dataloggerCounter + 1;
		pMe->dataloggerCounterSamples = 0;
		nErr = IFILE_Write(cfile, &pMe->dataloggerCounter, sizeof(uint32));
		DBGPRINTF("Counter set to %d, wrote %d bytes", pMe->dataloggerCounter, nErr);
	}

	if (cfile != NULL) {
		IFILE_Release(cfile);
	}

}

int suryabrew_DataLoggerGetOrCreateCounter(suryabrew *pMe)
{

	IFile* cfile = NULL;
	boolean exists = FALSE;
	int nErr = 0;
	int retval = 0;

	if (pMe->dataloggerRunning == TRUE) {
		return 0;
	}

	// get or create the counter value from the file
	suryabrew_FileCreateFileNamePrefix(pMe, "counter.txt");
	SPRINTF(pMe->DatalogCounterFilename, pMe->FileNamePrefix);
	DBGPRINTF("%s", pMe->DatalogCounterFilename);

	exists = suryabrew_FileExists(pMe, pMe->DatalogCounterFilename, NULL);

	if (exists) {
		cfile = suryabrew_FileGetForRead(pMe, pMe->DatalogCounterFilename);
	} else {
		cfile = suryabrew_FileAppendOrCreateFile(pMe, pMe->DatalogCounterFilename, NULL); 
	}

	if (cfile != NULL) {
		if (exists) { // read the number and set pMe->dataloggerCounter
			nErr = IFILE_Read(cfile, &pMe->dataloggerCounter, sizeof(uint32));
			DBGPRINTF("Read %d bytes counter at %d", nErr, pMe->dataloggerCounter);
		} else { // write in first number
			pMe->dataloggerCounter = 1;
			nErr = IFILE_Write(cfile, &pMe->dataloggerCounter, sizeof(uint32));
			DBGPRINTF("Seting counter to 1, wrote %d bytes", nErr);
		}
		if (nErr == 0) {
			DBGPRINTF("Error reading/writing counter file!!!");
			retval = 1;
		}
	} else {
		DBGPRINTF("Unable to access/create counter file");
		retval = 1;
	}

	if (cfile != NULL) {
		IFILE_Release(cfile);
	}

	return retval;
}




