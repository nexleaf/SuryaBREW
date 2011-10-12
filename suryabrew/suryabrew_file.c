

#include "suryabrew.h"



int suryabrew_FileInitMgr(suryabrew *pMe)
{
	int nErr = 0;
	nErr = ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void**)(&pMe->pIFileMgr));
	if ( nErr != SUCCESS)
	{
		DBGPRINTF("HandleEvent Unable to create DBMgr instance %d", nErr);
	}
	return nErr;

}

void suryabrew_FileUnload(suryabrew *pMe)
{
	if ( pMe->pIFileMgr != NULL)
	{
		IFILEMGR_Release(pMe->pIFileMgr);
		pMe->pIFileMgr = NULL;
	}
}


void suryabrew_FileDeleteFile(suryabrew *pMe, char *filename)
{
	int nErr = 0;
	suryabrew_FileInitMgr(pMe);

	if (pMe->pIFileMgr != NULL)
	{
		nErr = IFILEMGR_Remove(pMe->pIFileMgr, filename);
		DBGPRINTF("suryabrew_FileDelete: %s", filename);
	}


	if (pMe->pIFileMgr != NULL)
	{
		IFILEMGR_Release(pMe->pIFileMgr);
		pMe->pIFileMgr = NULL;
	}
}


void suryabrew_FileCreateDirPrefixDate(suryabrew *pMe)
{
	int nErr = 0;
	suryabrew_FileInitMgr(pMe);

	if (pMe->pIFileMgr != NULL)
	{

		SPRINTF(pMe->FileNamePrefix, "%s%s%d/%02d/%02d/",
			AEEFS_CARD0_DIR, SURYABREW_DIR_PREFIX,
			pMe->CurrDate.wYear, 
			pMe->CurrDate.wMonth, pMe->CurrDate.wDay);

		// if we can not create the dir, force local storage
		if ((nErr = IFILEMGR_MkDir(pMe->pIFileMgr, pMe->FileNamePrefix)) != SUCCESS)
		{
			DBGPRINTF("FileCreateFileNamePrefixDate Unable to Create dir %s %d",
				pMe->FileNamePrefix, nErr);
			MEMSET(pMe->FileNamePrefix, 0, 16);
		}
	} else {
		// if no manager, then just zeor everything out
		MEMSET(pMe->FileNamePrefix, 0, 16);
	}

	
	if (pMe->pIFileMgr != NULL)
	{
		IFILEMGR_Release(pMe->pIFileMgr);
		pMe->pIFileMgr = NULL;
	}

}

void suryabrew_FileCreateFileNamePrefix(suryabrew *pMe, char* filename)
{
	int nErr = 0;
	// Init manager
//	suryabrew_FileInitMgr(pMe);

	// If manager actually do something
//	if (pMe->pIFileMgr != NULL)
//	{
//	/*	SPRINTF(pMe->FileNamePrefix, "%s%s",
//			AEEFS_CARD0_DIR, SURYABREW_DIR_PREFIX);
//*/
//			SPRINTF(pMe->FileNamePrefix, "%s",
//			 SURYABREW_DIR_PREFIX);
//		// If we can not create the dir, force local phone storage
//		if ((nErr = IFILEMGR_MkDir(pMe->pIFileMgr, pMe->FileNamePrefix)) != SUCCESS)
//		{
//			DBGPRINTF("FileCreateFileNamePrefix Unable to Create dir %s %d",
//				pMe->FileNamePrefix, nErr);
//			if (filename != NULL)
//			{
//				SPRINTF(pMe->FileNamePrefix, "%s",
//					filename);
//			} 
//			else
//			{
//				MEMSET(pMe->FileNamePrefix, 0, 16);
//			}
//		} else {
//			// since everything went well, if they want a file name, append it
//			if (filename != NULL)
//			{
//				/*SPRINTF(pMe->FileNamePrefix, "%s%s%s",
//					AEEFS_CARD0_DIR, SURYABREW_DIR_PREFIX, filename);
//					*/
//				SPRINTF(pMe->FileNamePrefix, "%s%s",
//					 SURYABREW_DIR_PREFIX, filename);
//			}
//		}
//	} else */
	{
		// If no manager, then zero out the prefix, unless there is a file name
		// this will force local phone storage
		if (filename != NULL)
		{
			SPRINTF(pMe->FileNamePrefix, "%s",
				filename);
		} else
		{
			MEMSET(pMe->FileNamePrefix, 0, 16);
		}

	}


/*	if (pMe->pIFileMgr != NULL)
	{
		IFILEMGR_Release(pMe->pIFileMgr);
		pMe->pIFileMgr = NULL;
	}*/

}


IFile* suryabrew_FileCreateFile(suryabrew *pMe, char *filename)
{
	int nErr = 0;
	IFile * ifile = NULL;

	suryabrew_FileInitMgr(pMe);

	if (pMe->pIFileMgr != NULL)
	{
		IFILEMGR_Remove(pMe->pIFileMgr, filename);
		ifile = IFILEMGR_OpenFile(pMe->pIFileMgr, filename, _OFM_CREATE);
		if (ifile == NULL)
		{
			nErr = IFILEMGR_GetLastError(pMe->pIFileMgr);
			DBGPRINTF("Error creating file %d", nErr);
			DBGPRINTF("%s", filename);
		} else 
		{
			DBGPRINTF("Created file %s", filename);
		}
	} 

	
	if (pMe->pIFileMgr != NULL)
	{
		IFILEMGR_Release(pMe->pIFileMgr);
		pMe->pIFileMgr = NULL;
	}

	return ifile;
}
