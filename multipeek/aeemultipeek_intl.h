static int32 IMultiPeek_New(IShell *,IModule *,void **);
static int32 Multipart_Read(IMultiPeek *,uint8 *,uint32);
static uint32 Multipart_RelativeSeek(IMultiPeek *,int32);

#define MAXPEEKBUFFERSIZE 32767

enum part_type { FILE_PART=1, MEMORY_PART };

typedef struct _source_part { 
	uint8 type;
	uint32 len;
	void *data; // IFILE *, when type==FILE_PART; uint8 *, when type==MEMORY_PART
} source_part;

struct IMultiPeek {
	AEEVTBL(IMultiPeek) *pvt;
	uint32 nRefs;
	IShell *psh;
	IModule *pm;
	source_part *pDescriptors;
	uint32 nDescriptors;
	uint32 nCurrentDescriptor,nDescriptorPos;
	uint8 *pPeekBuffer;
	uint32 nPeekBufferSize;
	boolean bIsStreamLocked;
};


static int32 IMultiPeek_AddMemorySource(IMultiPeek *,const void *,uint32);
static int32 IMultiPeek_AddFileSource(IMultiPeek *,IFile *);

static char *IMultiPeek_Peek(IMultiPeek *,int32,int32 *,int32 *);
static void IMultiPeek_Peekable(IMultiPeek *,AEECallback *);
static void IMultiPeek_Advance(IMultiPeek *,int32);

static int32 IMultiPeek_Read(IMultiPeek *,char *,int32);
static void IMultiPeek_Readable(IMultiPeek *,AEECallback *);

static uint32 IMultiPeek_AddRef(IMultiPeek*);
static uint32 IMultiPeek_Release(IMultiPeek*);
static int IMultiPeek_QueryInterface(IMultiPeek*, AEECLSID clsId, void** ppObj);

static int32 IMultiPeek_New(IShell *,IModule *,void **);

static int32 Multipart_Read(IMultiPeek *,uint8 *,uint32);
static uint32 Multipart_RelativeSeek(IMultiPeek *,int32);




