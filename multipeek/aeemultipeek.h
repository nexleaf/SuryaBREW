#include "AEEInterface.h"
#include "AEEQueryInterface.h"
#include "AEESource.h"
#include "multipeek.bid"

//typedef struct IMultiPeek IMultiPeek;
#define INHERIT_IMultiPeek(iname) INHERIT_IPeek(iname);\
	int32 (*AddMemorySource)(IMultiPeek *,const void *,uint32);\
	int32 (*AddFileSource)(IMultiPeek *,IFile *)

AEEINTERFACE_DEFINE(IMultiPeek);

#define IMULTIPEEK_AddMemorySource(po,pm,l) AEEGETPVTBL((po),IMultiPeek)->AddMemorySource((po),(pm),(l))
#define IMULTIPEEK_AddFileSource(po,pif) AEEGETPVTBL((po),IMultiPeek)->AddFileSource((po),(pif))
#define IMULTIPEEK_Peek(po,nb,pb,pr) AEEGETPVTBL((po),IMultiPeek)->Peek((po),(nb),(pb),(pr))
#define IMULTIPEEK_Peekable(po,pcb) AEEGETPVTBL((po),IMultiPeek)->Peekable((po),(pcb))
#define IMULTIPEEK_Advance(po,na) AEEGETPVTBL((po),IMultiPeek)->Advance((po),(na))
#define IMULTIPEEK_Read(po,pb,nb) AEEGETPVTBL((po),IMultiPeek)->Read((po),(pb),(nb))
#define IMULTIPEEK_Readable(po,pcb) AEEGETPVTBL((po),IMultiPeek)->Readable((po),(pcb))
#define IMULTIPEEK_AddRef(po) AEEGETPVTBL((po),IMultiPeek)->AddRef((po))
#define IMULTIPEEK_Release(po) AEEGETPVTBL((po),IMultiPeek)->Release((po))
#define IMULTIPEEK_QueryInterface(po,clsid,ppobj) AEEGETPVTBL((po),IMultiPeek)->QueryInterface((po),(clsid),(ppobj))

