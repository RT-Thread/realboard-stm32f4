/*------------------------------------------------------------------------*/
/* Sample code of OS dependent controls for FatFs R0.08                   */
/* (C)ChaN, 2010                                                          */
/*------------------------------------------------------------------------*/

#include <stdlib.h>		/* ANSI memory controls */
#include <malloc.h>		/* ANSI memory controls */

#include "../ff.h"


#if _FS_REENTRANT
/*------------------------------------------------------------------------*/
/* Create a Synchronization Object
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to create a new
/  synchronization object, such as semaphore and mutex. When a FALSE is
/  returned, the f_mount function fails with FR_INT_ERR.
*/

BOOL ff_cre_syncobj (	/* TRUE:Function succeeded, FALSE:Could not create due to any error */
	BYTE vol,			/* Corresponding logical drive being processed */
	_SYNC_t *sobj		/* Pointer to return the created sync object */
)
{
	BOOL ret;

	*sobj = CreateMutex(NULL, FALSE, NULL);					/* Win32 */
	ret = (*sobj != INVALID_HANDLE_VALUE) ? TRUE : FALSE;

//	*sobj = SyncObjects[vol];	/* uITRON (give a static created sync object) */
//	ret = TRUE;					/* The initial value of the semaphore must be 1. */

//	*sobj = OSMutexCreate(0, &err);				/* uC/OS-II */
//	ret = (err == OS_NO_ERR) ? TRUE : FALSE;

//	*sobj = xSemaphoreCreateMutex();			/* FreeRTOS */
//	ret = (*sobj != NULL) ? TRUE : FALSE;

	return ret;
}



/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a synchronization
/  object that created with ff_cre_syncobj function. When a FALSE is
/  returned, the f_mount function fails with FR_INT_ERR.
*/

BOOL ff_del_syncobj (	/* TRUE:Function succeeded, FALSE:Could not delete due to any error */
	_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
	BOOL ret;

	ret = CloseHandle(sobj);	/* Win32 *

//	ret = TRUE;					/* uITRON (nothing to do) *

//	OSMutexDel(sobj, OS_DEL_ALWAYS, &err);		/* uC/OS-II */
//	ret = (err == OS_NO_ERR) ? TRUE : FALSE;

//	ret = TRUE;					/* FreeRTOS (nothing to do) */

	return ret;
}



/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a FALSE is returned, the file function fails with FR_TIMEOUT.
*/

BOOL ff_req_grant (	/* TRUE:Got a grant to access the volume, FALSE:Could not get a grant */
	_SYNC_t sobj	/* Sync object to wait */
)
{
	BOOL ret;

	ret = (WaitForSingleObject(sobj, _FS_TIMEOUT) == WAIT_OBJECT_0) ? TRUE : FALSE;	/* Win32 */

//	ret = (wai_sem(sobj) == E_OK) ? TRUE : FALSE;	/* uITRON */

//	OSMutexPend(sobj, _FS_TIMEOUT, &err));			/* uC/OS-II */
//	ret = (err == OS_NO_ERR) ? TRUE : FALSE;

//	ret = (xSemaphoreTake(sobj, _FS_TIMEOUT) == pdTRUE) ? TRUE : FALSE;	/* FreeRTOS */

	return ret;
}



/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	_SYNC_t sobj	/* Sync object to be signaled */
)
{
	ReleaseMutex(sobj);		/* Win32 */

//	sig_sem(sobj);			/* uITRON */

//	OSMutexPost(sobj);		/* uC/OS-II */

//	xSemaphoreGive(sobj);	/* FreeRTOS */

}

#endif




#if _USE_LFN == 3	/* LFN with a working buffer on the heap */
/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/
/* If a NULL is returned, the file function fails with FR_NOT_ENOUGH_CORE.
*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block */
	UINT size		/* Number of bytes to allocate */
)
{
	return malloc(size);
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree(
	void* mblock	/* Pointer to the memory block to free */
)
{
	free(mblock);
}

#endif
