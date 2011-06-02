/*===========================================================================

FILE: multipeek.c
===========================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEStdLib.h"
#include "AEEShell.h"

#include "AEEFile.h"			// File interface definitions

#include "aeemultipeek.h"
#include "aeemultipeek_intl.h"

int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
  int nRetVal= SUCCESS;
  *ppObj = NULL;

  do  {
    if (ClsId == AEECLSID_MULTIPEEK) {
      if ((nRetVal=IMultiPeek_New(pIShell,po,ppObj))!=SUCCESS) break;
    } else {
      nRetVal = ECLASSNOTSUPPORT; break;
    }
  } while(FALSE);

  return nRetVal;
}

int32 IMultiPeek_New(IShell *pIShell,IModule *pIModule,void **ppobj) {
	IMultiPeek *pIMPeek=NULL;
	int32 nRet=SUCCESS;

	pIMPeek=MALLOC(ALLOC_NO_ZMEM|(sizeof(IMultiPeek)+sizeof(AEEVTBL(IMultiPeek))));
	if (pIMPeek==NULL)
		nRet=ENOMEMORY;
	else {
		if ((pIMPeek->pPeekBuffer=MALLOC(ALLOC_NO_ZMEM|MAXPEEKBUFFERSIZE))==NULL) {
			nRet=ENOMEMORY; FREEIF(pIMPeek);
		} else {
			pIMPeek->pDescriptors=NULL; pIMPeek->nDescriptors=0; pIMPeek->nPeekBufferSize=MAXPEEKBUFFERSIZE;
			pIMPeek->nDescriptorPos=0; pIMPeek->nCurrentDescriptor=0;
			pIMPeek->bIsStreamLocked=FALSE;
			pIMPeek->nRefs=1; pIMPeek->pm=pIModule; pIMPeek->psh=pIShell;
			ISHELL_AddRef(pIShell); IMODULE_AddRef(pIModule);
			pIMPeek->pvt=(AEEVTBL(IMultiPeek) *)(((uint8 *)pIMPeek)+sizeof(IMultiPeek));
			pIMPeek->pvt->AddRef=IMultiPeek_AddRef; pIMPeek->pvt->Release=IMultiPeek_Release;
			pIMPeek->pvt->QueryInterface=IMultiPeek_QueryInterface;
			pIMPeek->pvt->Peek=IMultiPeek_Peek; pIMPeek->pvt->Peekable=IMultiPeek_Peekable; pIMPeek->pvt->Advance=IMultiPeek_Advance;
			pIMPeek->pvt->Readable=IMultiPeek_Readable; pIMPeek->pvt->Read=IMultiPeek_Read;
			pIMPeek->pvt->AddFileSource=IMultiPeek_AddFileSource; pIMPeek->pvt->AddMemorySource=IMultiPeek_AddMemorySource;
		}
	};
	*ppobj=pIMPeek; return nRet;
}

uint32 IMultiPeek_AddRef(IMultiPeek* po) {
	if (po!=NULL) {
		po->nRefs++; return po->nRefs;
	} else return 0;
}

uint32 IMultiPeek_Release(IMultiPeek* po) {
	if (po!=NULL) {
		uint32 nRes;
		
		po->nRefs--; nRes=po->nRefs;
		if (nRes==0) {
			uint16 i;

			ISHELL_Release(po->psh); IMODULE_Release(po->pm);
			for (i=0;i<po->nDescriptors;i++) {
				if (po->pDescriptors[i].type==FILE_PART)
					IFILE_Release((IFile *)(po->pDescriptors[i].data));
			}
			FREEIF(po->pPeekBuffer);
			FREEIF(po->pDescriptors);
			FREE(po);
		}
		return nRes;
	} else return 0;
}

int IMultiPeek_QueryInterface(IMultiPeek *po, AEECLSID clsId, void** ppObj) {
	int  nRet=EUNSUPPORTED;

	if( NULL == po || NULL==ppObj ) return EBADPARM;

	*ppObj = NULL;
	if ((clsId==AEECLSID_MULTIPEEK)||(clsId==AEECLSID_PEEK)||(clsId==AEECLSID_SOURCE)||(clsId==AEECLSID_QUERYINTERFACE)) {
        nRet = SUCCESS; *ppObj = po; IMultiPeek_AddRef(po);
	}
	return nRet;
}

int32 IMultiPeek_AddMemorySource(IMultiPeek *po,const void *pbuf,uint32 buflen) {
	source_part *pp;

	if ((po==NULL)||(pbuf==NULL)||(buflen==0)||po->bIsStreamLocked) return EBADPARM;
	
	pp=(po->pDescriptors==NULL)?MALLOC(ALLOC_NO_ZMEM|sizeof(source_part)):REALLOC(po->pDescriptors,(po->nDescriptors+1)*sizeof(source_part));
	if (pp==NULL) return ENOMEMORY;
	po->pDescriptors=pp;
	pp=&po->pDescriptors[po->nDescriptors]; po->nDescriptors++;
	pp->data=(void *)pbuf; pp->len=buflen; pp->type=MEMORY_PART;
	return SUCCESS;
}

int32 IMultiPeek_AddFileSource(IMultiPeek *po,IFile *pif) {
	source_part *pp;
	FileInfo fi;

	if ((po==NULL)||(pif==NULL)||po->bIsStreamLocked) return EBADPARM;
	if ((IFILE_GetInfo(pif,&fi)==EFAILED)||(fi.dwSize==0)) return EBADPARM;
	pp=(po->pDescriptors==NULL)?MALLOC(ALLOC_NO_ZMEM|sizeof(source_part)):REALLOC(po->pDescriptors,(po->nDescriptors+1)*sizeof(source_part));
	if (pp==NULL) return ENOMEMORY;
	po->pDescriptors=pp;
	pp=&po->pDescriptors[po->nDescriptors]; po->nDescriptors++;
	pp->data=pif; pp->len=fi.dwSize; pp->type=FILE_PART; IFILE_AddRef(pif);
	return SUCCESS;
}

char *IMultiPeek_Peek(IMultiPeek *po,int32 nbWanted,int32 *pbAvail,int32 *pnRet) {
	int32 nBytesAvail=0, nRetCode=IPEEK_END;
	char *pcResult=NULL;

	if (po!=NULL) {
		pcResult=(char *)po->pPeekBuffer;

		if (nbWanted==0) {
			if (!po->bIsStreamLocked) {
				nBytesAvail=0; nRetCode=IPEEK_UNK;
			} else {
				pcResult=IMultiPeek_Peek(po,po->nPeekBufferSize,&nBytesAvail,&nRetCode); po->pPeekBuffer=(uint8 *)pcResult;
			}
		} else if (nbWanted==IPEEK_GETBUFSIZE) {
			pcResult=0; nBytesAvail=po->nPeekBufferSize; nRetCode=IPEEK_UNK;
		} else {
			uint32 nSavedCurrentDescriptor,nSavedCurrentDescriptorPos;

			po->bIsStreamLocked=TRUE;
			if ((nbWanted==IPEEK_PEEKMAX)||(nbWanted<po->nPeekBufferSize))
				nbWanted=po->nPeekBufferSize;
			nSavedCurrentDescriptor=po->nCurrentDescriptor; nSavedCurrentDescriptorPos=po->nDescriptorPos;
			nBytesAvail=Multipart_Read(po,po->pPeekBuffer,nbWanted);
			po->nCurrentDescriptor=nSavedCurrentDescriptor; po->nDescriptorPos=nSavedCurrentDescriptorPos;
			nRetCode=(nBytesAvail==0)?IPEEK_END:((nBytesAvail<nbWanted)?IPEEK_UNK:IPEEK_FULL);
		}
	}

	if (pbAvail) *pbAvail=nBytesAvail;
	if (pnRet) *pnRet=nRetCode;
	return pcResult;
}

void IMultiPeek_Peekable(IMultiPeek *po,AEECallback *pIMPeekCbk) {
	if (pIMPeekCbk) ISHELL_Resume(po->psh,pIMPeekCbk);
}

void IMultiPeek_Advance(IMultiPeek *po,int32 nAdvance) {
	if ((po==NULL)||(nAdvance==0)||!po->bIsStreamLocked) return;
	Multipart_RelativeSeek(po,nAdvance);
}

int32 IMultiPeek_Read(IMultiPeek *po,char *pDstBuf,int32 nDstBufLen) {
	int32 nr;

	if ((po==NULL)||(pDstBuf==NULL)||(nDstBufLen==0)) return ISOURCE_END;
	po->bIsStreamLocked=TRUE;
	nr=Multipart_Read(po,(uint8 *)pDstBuf,nDstBufLen);
/*	if (nr>0) {
		Multipart_RelativeSeek(po,nr);
	}
*/
	return nr;
}

void IMultiPeek_Readable(IMultiPeek *po,AEECallback *pIMPeekCbk) {
	if (pIMPeekCbk) ISHELL_Resume(po->psh,pIMPeekCbk);
}

int32 Multipart_Read(IMultiPeek *pme,uint8 *pbuf,uint32 len) {
	int32 nr=0;
	source_part *pp;

	if ((pbuf==NULL)||(pme==NULL)||(len==0)) return IPEEK_END; // bad parameters
	if (pme->nDescriptors==0) return IPEEK_END; // empty source stream
	if (pme->nCurrentDescriptor>=pme->nDescriptors) return IPEEK_END; // end of stream
	
	pme->bIsStreamLocked=TRUE;
	pp=&pme->pDescriptors[pme->nCurrentDescriptor];
	while (nr<len) {
		uint32 rest;

		rest=pp->len-pme->nDescriptorPos; if (len-nr<rest) rest=len-nr;
		if (pp->type==MEMORY_PART) {
			MEMCPY(pbuf+nr,(uint8 *)(pp->data)+pme->nDescriptorPos,rest);
		} else {
			IFILE_Seek((IFile *)pp->data,_SEEK_START,pme->nDescriptorPos);
			rest=IFILE_Read((IFile *)pp->data,pbuf+nr,rest);
		}
		nr+=rest; pme->nDescriptorPos+=rest;
		if (pme->nDescriptorPos>=pp->len) {
			pme->nDescriptorPos=0;
			pme->nCurrentDescriptor++; pp++; if (pme->nCurrentDescriptor>=pme->nDescriptors) break;
		}
	}

	return nr;
}

uint32 Multipart_RelativeSeek(IMultiPeek *pme,int32 relpos) {
	source_part *pp;

	if ((pme==NULL)||(pme->nDescriptors==0)) return EBADPARM;
	if (relpos==0) return SUCCESS;
	pp=&pme->pDescriptors[pme->nCurrentDescriptor];
	while(relpos!=0) {
		if (relpos>0) { // positive offset, jumping forward until end of stream.
			if (pme->nDescriptorPos+relpos>=pp->len) { // jump over current part needed
				if (pme->nCurrentDescriptor<pme->nDescriptors) {
					relpos=relpos-(pp->len-pme->nDescriptorPos);
				}
				pp++; pme->nCurrentDescriptor++; pme->nDescriptorPos=0;
				if (pme->nCurrentDescriptor>=pme->nDescriptors) return SUCCESS; // positioned to the end of stream
			} else { // set position on 
				pme->nDescriptorPos+=relpos; relpos=0;
			}
		} else { // negative offset, jumping backward until position 0.
			if ((int32)pme->nDescriptorPos+relpos>=0) { // set position on current part
				pme->nDescriptorPos=(int32)pme->nDescriptorPos+relpos; relpos=0;
			} else { // positioning over current part.
				relpos=relpos+(int32)pme->nDescriptorPos;
				if (pme->nCurrentDescriptor==0) {
					pme->nCurrentDescriptor=pme->nDescriptorPos=0; return SUCCESS; // set position on stream's start.
				}
				pme->nCurrentDescriptor--; pp--;
				pme->nDescriptorPos=pp->len;
			}
		}
	}
	/* pp=&pme->pDescriptors; pme->nCurrentDescriptor=0; pme->nDescriptorPos=0;
	while (abspos>=0) {
		if (pme->nCurrentDescriptor>=pme->nDescriptors) return EFAILED;
		if (abspos>=pp->len) {
			pme->nCurrentDescriptor++;
		} else {
			pme->nDescriptorPos=abspos;
		}
		abspos-=pp->len; pp++;
	} */
	return SUCCESS;
}

/*
Поведение стандартного IPEEK:
1) Операция Peek считывает максимально возможное количество байт входного потока. Т.е. если cbWanted<размера буфера,
заданного в ISOURCEUTIL_PeekFromSource - то считывается кол-во байт по размеру буфера, и соотв. результат помещается
в pbAvail.
2) Peek не сдвигает указатель потока. Последовательные вызовы Peek будут считывать одну и ту же порцию данных с текущей
позиции в потоке.
3) При считывании порции данных меньшей объема буфера (например, перед концом потока) Peek возвращает IPEEK_UNK; после
смещения указателя чтения на pbAvail байт вперед возвращается после очередного вызова IPeek возвращается IPEEK_END.
4) Сдвигать указатель надо через IPEEK_Advance; отрицательные смещения не поддерживаются - по крайней мере, для
объектов IPeek, созданных из файла.
5) Read читает данные, начиная начала текущего буфера. Указатель чтения смещается на число прочитанных байт, при этом
следующий Peek прочтет данные, начиная с 1-го непрочитанного Read байта.
6) Peek с cbWanted==0 возвращает IPEEK_UNK и 0 в pbAvail, если Peek еще ни разу не вызывалась; иначе возвращает точно то
же, что и предыдущий вызов Peek.
7) Peek с cbWanted==IPEEK_GETBUFSIZE возвращает NULL, чтение не выполняет. В pbAvail помещается размер внутреннего буфера
IPeek, код возврата==IPEEK_UNK;
7) Peek с cbWanted==IPEEK_PEEKMAX выполняет чтение исходя из размера внутреннего буфера.
8) Если файл считывается в буфер с первой же операции - то возвращается IPEEK_UNK, потом, после Advance на длину прочитан-
ного - IPEEK_END
9) До вызова первого Peek Advance не выполняет никаких действий. Read сдвигает указатель обычным порядком.
*/












