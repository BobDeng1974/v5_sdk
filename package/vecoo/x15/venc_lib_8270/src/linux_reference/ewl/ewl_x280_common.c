/*------------------------------------------------------------------------------
  --                                                                            --
  --       This software is confidential and proprietary and may be used        --
  --        only as expressly authorized by a licensing agreement from          --
  --                                                                            --
  --                            Hantro Products Oy.                             --
  --                                                                            --
  --                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
  --                            ALL RIGHTS RESERVED                             --
  --                                                                            --
  --                 The entire notice above must be reproduced                 --
  --                  on all copies and should not be removed.                  --
  --                                                                            --
  --------------------------------------------------------------------------------
  --
  --  Abstract : Encoder Wrapper Layer for 6280/7280/8270, common parts
  --
  --------------------------------------------------------------------------------
  --
  --  Version control information, please leave untouched.
  --
  --  $RCSfile: ewl_x280_common.c,v $
  --  $Revision: 1.1 $
  --
  ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  1. Include headers
  ------------------------------------------------------------------------------*/

#include "basetype.h"
#include "ewl.h"
#include "ewl_x280_common.h"

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/sem.h>
#include <utils/Log.h>

#include <assert.h>


/* macro to convert CPU bus address to ASIC bus address */
static const char *busTypeName[7] = { "UNKNOWN", "AHB", "OCP", "AXI", "PCI", "AXIAHB", "AXIAPB" };
static const char *synthLangName[3] = { "UNKNOWN", "VHDL", "VERILOG" };

volatile u32 asic_status = 0;

int MapAsicRegisters(hx280ewl_t *ewl)
{
	unsigned long base, size;
	u32 *pRegs;

	if(ioctl(ewl->fd_enc, HX280ENC_IOCGHWOFFSET, &base))
	{
		PTRACE("EWLInit: Failed to get regs base\n");
		return -1;
	}

	if(ioctl(ewl->fd_enc, HX280ENC_IOCGHWIOSIZE, &size))
	{
		PTRACE("EWLInit: Failed to get regs size\n");
		return -1;
	}

	/* map hw registers to user space */
	pRegs = (u32 *)mmap(0, size, PROT_READ | PROT_WRITE, \
			MAP_SHARED, ewl->fd_enc, base);
	if(pRegs == MAP_FAILED)
	{
		PTRACE("EWLInit: Failed to mmap regs\n");
		return -1;
	}

	ewl->regSize	= size;
	ewl->regBase	= base;
	ewl->pRegBase	= pRegs;

	return 0;
}

/*******************************************************************************
  Function name   : EWLReadAsicID
Description     : Read ASIC ID register, static implementation
Return type     : u32 ID
Argument        : void
 *******************************************************************************/
u32 EWLReadAsicID(void)
{
	u32 id = ~0;
	int fd_mem = -1, fd_enc = -1;
	unsigned long base = ~0, size;
	u32 *pRegs = MAP_FAILED;

	EWLHwConfig_t cfg_info;
    
    PTRACE("%s()\n", __FUNCTION__);

	memset(&cfg_info, 0, sizeof(cfg_info));

	fd_enc = open(ENC_MODULE_PATH, O_RDWR | O_SYNC);
	if(fd_enc == -1)
	{
		PTRACE("EWLReadAsicID: failed to open: %s\n", ENC_MODULE_PATH);
		goto end;
	}

	if(ioctl(fd_enc, HX280ENC_IOCGHWOFFSET, &base))
	{
		PTRACE("EWLReadAsicID: ioctl get regs base error\n");
		goto end;
	}
	if(ioctl(fd_enc, HX280ENC_IOCGHWIOSIZE, &size))
	{
		PTRACE("EWLReadAsicID: ioctl get regs size error\n");
		goto end;
	}

	/* map hw registers to user space */
    size += 4095;
    size &= ~4095;
	pRegs = (u32 *)mmap(0, size, PROT_READ, MAP_SHARED, fd_enc, base);
	if(pRegs == MAP_FAILED)
	{
		PTRACE("EWLReadAsicID: Failed to mmap regs\n");
		goto end;
	}

	id = pRegs[0];

end:
	if(pRegs != MAP_FAILED)
		munmap(pRegs, size);
	if(fd_enc != -1)
		close(fd_enc);

	return id;
}

/*******************************************************************************
  Function name   : EWLReadAsicConfig
Description     : Reads ASIC capability register, static implementation
Return type     : EWLHwConfig_t 
Argument        : void
 *******************************************************************************/
EWLHwConfig_t EWLReadAsicConfig(void)
{
	int fd_mem = -1, fd_enc = -1;
	unsigned long base, size;
	u32 *pRegs = MAP_FAILED, cfgval;

	EWLHwConfig_t cfg_info;

    PTRACE("%s()\n", __FUNCTION__);

	memset(&cfg_info, 0, sizeof(cfg_info));

	fd_enc = open(ENC_MODULE_PATH, O_RDWR | O_SYNC);
	if(fd_enc == -1)
	{
		PTRACE("EWLReadAsicConfig: failed to open: %s\n", ENC_MODULE_PATH);
		goto end;
	}

	if(ioctl(fd_enc, HX280ENC_IOCGHWOFFSET, &base))
	{
		PTRACE("EWLReadAsicConfig: failed to get regs base\n");
		goto end;
	}
	if(ioctl(fd_enc, HX280ENC_IOCGHWIOSIZE, &size))
	{
		PTRACE("EWLReadAsicConfig: failed to get regs size\n");
		goto end;
	}

	/* map hw registers to user space */
    size += 4095;
    size &= ~4095;
	pRegs = (u32 *)mmap(0, size, PROT_READ, MAP_SHARED, fd_enc, base);
	if(pRegs == MAP_FAILED)
	{
		PTRACE("EWLReadAsicConfig: Failed to mmap regs\n");
		goto end;
	}

	cfgval = pRegs[63];

	cfg_info.maxEncodedWidth = cfgval & ((1 << 11) - 1);
	cfg_info.h264Enabled = (cfgval >> 27) & 1;
	cfg_info.mpeg4Enabled = (cfgval >> 26) & 1;
	cfg_info.jpegEnabled = (cfgval >> 25) & 1;
	cfg_info.vsEnabled = (cfgval >> 24) & 1;
    cfg_info.rgbEnabled = (cfgval >> 28) & 1;

	cfg_info.busType = (cfgval >> 20) & 15;
	cfg_info.synthesisLanguage = (cfgval >> 16) & 15;
	cfg_info.busWidth = (cfgval >> 12) & 15;

	PTRACE("EWLReadAsicConfig:\n"
			"    maxEncodedWidth   = %d\n"
			"    h264Enabled       = %s\n"
			"    jpegEnabled       = %s\n"
			"    mpeg4Enabled      = %s\n"
			"    vsEnabled         = %s\n"
            "    rgbEnabled        = %s\n"
			"    busType           = %s\n"
			"    synthesisLanguage = %s\n"
			"    busWidth          = %d\n",
			cfg_info.maxEncodedWidth,
			cfg_info.h264Enabled == 1 ? "YES" : "NO",
			cfg_info.jpegEnabled == 1 ? "YES" : "NO",
			cfg_info.mpeg4Enabled == 1 ? "YES" : "NO",
			cfg_info.vsEnabled == 1 ? "YES" : "NO",
            cfg_info.rgbEnabled == 1 ? "YES" : "NO",
            cfg_info.busType < 7 ? busTypeName[cfg_info.busType] : "UNKNOWN",
			cfg_info.synthesisLanguage <
			3 ? synthLangName[cfg_info.synthesisLanguage] : "ERROR",
			cfg_info.busWidth * 32);

end:
	if(pRegs != MAP_FAILED)
		munmap(pRegs, size);
	if(fd_enc != -1)
		close(fd_enc);
	return cfg_info;
}

/*******************************************************************************
  Function name   : EWLInit
Description     : Allocate resources and setup the wrapper module
Return type     : ewl_ret 
Argument        : void
 *******************************************************************************/
const void *EWLInit(EWLInitParam_t *param)
{
	hx280ewl_t *enc = NULL;
    PTRACE("%s()\n", __FUNCTION__);

	/* Check for NULL pointer */
	if(param == NULL || param->clientType > 4)
	{

		PTRACE(("EWLInit: Bad calling parameters!\n"));
		return NULL;
	}

	/* Allocate instance */
	if((enc = (hx280ewl_t *)EWLmalloc(sizeof(hx280ewl_t))) == NULL)
	{
		PTRACE("EWLInit: failed to alloc hx280ewl_t struct\n");
		return NULL;
	}

	enc->clientType = param->clientType;
	enc->fd_enc = -1;
	enc->alc = IM_NULL;

	if(alc_open(&enc->alc, "venc") != IM_RET_OK){
		PTRACE("EWLInit: alc_open() failed");
		goto err;
	}

	enc->fd_enc = open(ENC_MODULE_PATH, O_RDWR | O_SYNC);
	if(enc->fd_enc == -1)
	{
		PTRACE("EWLInit: failed to open: %s\n", ENC_MODULE_PATH);
		goto err;
	}

	/* map hw registers to user space */
	if(MapAsicRegisters(enc) != 0)
	{
		goto err;
	}

	return enc;

err:
	EWLRelease(enc);
	return NULL;
}

/*******************************************************************************
  Function name   : EWLRelease
Description     : Release the wrapper module by freeing all the resources
Return type     : ewl_ret 
Argument        : void
 *******************************************************************************/
i32 EWLRelease(const void *inst)
{
	hx280ewl_t *enc = (hx280ewl_t *) inst;
    PTRACE("%s()\n", __FUNCTION__);

	assert(enc != NULL);

	if(enc == NULL)
		return EWL_OK;

	/* Release the instance */
	if(enc->pRegBase != MAP_FAILED)
		munmap((void *) enc->pRegBase, enc->regSize);

	if(enc->fd_enc != -1)
		close(enc->fd_enc);
	if(enc->alc != IM_NULL)
		alc_close(enc->alc);
	EWLfree(enc);
   
	return EWL_OK;
}

/*******************************************************************************
  Function name   : EWLWriteReg
Description     : Set the content of a hadware register
Return type     : void 
Argument        : u32 offset
Argument        : u32 val
 *******************************************************************************/
void EWLWriteReg(const void *inst, u32 offset, u32 val)
{
	hx280ewl_t *enc = (hx280ewl_t *) inst;

	assert(enc != NULL && offset < enc->regSize);

	if(offset == 0x68 && enc->clientType != 3)
		assert(0);  /* only JPEG should write QP table */

	if(offset == 0x04)
	{
		asic_status = val;
	}

	offset = offset / 4;
	*(enc->pRegBase + offset) = val;
}

/*------------------------------------------------------------------------------
  Function name   : EWLEnableHW
Description     : 
Return type     : void 
Argument        : const void *inst
Argument        : u32 offset
Argument        : u32 val
------------------------------------------------------------------------------*/
void EWLEnableHW(const void *inst, u32 offset, u32 val)
{
	hx280ewl_t *enc = (hx280ewl_t *) inst;
    PTRACE("%s()\n", __FUNCTION__);

	assert(enc != NULL && offset < enc->regSize);

	if(offset == 0x04)
	{
		asic_status = val;
	}

	offset = offset / 4;
	*(enc->pRegBase + offset) = val;
}

/*------------------------------------------------------------------------------
  Function name   : EWLDisableHW
Description     : 
Return type     : void 
Argument        : const void *inst
Argument        : u32 offset
Argument        : u32 val
------------------------------------------------------------------------------*/
void EWLDisableHW(const void *inst, u32 offset, u32 val)
{
	hx280ewl_t *enc = (hx280ewl_t *) inst;
    PTRACE("%s()\n", __FUNCTION__);

	assert(enc != NULL && offset < enc->regSize);

	offset = offset / 4;
	*(enc->pRegBase + offset) = val;

	asic_status = val;
}

/*******************************************************************************
  Function name   : EWLReadReg
Description     : Retrive the content of a hadware register
Note: The status register will be read after every MB
so it may be needed to buffer it's content if reading
the HW register is slow.
Return type     : u32 
Argument        : u32 offset
 *******************************************************************************/
u32 EWLReadReg(const void *inst, u32 offset)
{
	u32 val;
	hx280ewl_t *enc = (hx280ewl_t *)inst;

	assert(offset < enc->regSize);

	if(offset == 0x04)
	{
		return asic_status;
	}

	offset = offset / 4;
	val = *(enc->pRegBase + offset);

	return val;
}

/*------------------------------------------------------------------------------
  Function name   : EWLMallocRefFrm
Description     : Allocate a frame buffer (contiguous linear RAM memory)

Return type     : i32 - 0 for success or a negative error code 

Argument        : const void * instance - EWL instance
Argument        : u32 size - size in bytes of the requested memory
Argument        : EWLLinearMem_t *info - place where the allocated memory
buffer parameters are returned
------------------------------------------------------------------------------*/
i32 EWLMallocRefFrm(const void *instance, u32 size, EWLLinearMem_t *info)
{
	return (EWLMallocLinear(instance, size, info));
}

/*------------------------------------------------------------------------------
  Function name   : EWLFreeRefFrm
Description     : Release a frame buffer previously allocated with 
EWLMallocRefFrm.

Return type     : void 

Argument        : const void * instance - EWL instance
Argument        : EWLLinearMem_t *info - frame buffer memory information
------------------------------------------------------------------------------*/
void EWLFreeRefFrm(const void *instance, EWLLinearMem_t *info)
{
	EWLFreeLinear(instance, info);
}

/*------------------------------------------------------------------------------
  Function name   : EWLMallocLinear
Description     : Allocate a contiguous, linear RAM  memory buffer

Return type     : i32 - 0 for success or a negative error code  

Argument        : const void * instance - EWL instance
Argument        : u32 size - size in bytes of the requested memory
Argument        : EWLLinearMem_t *info - place where the allocated memory
buffer parameters are returned
------------------------------------------------------------------------------*/
i32 EWLMallocLinear(const void *instance, u32 size, EWLLinearMem_t *info)
{
	hx280ewl_t *enc_ewl = (hx280ewl_t *)instance;
	EWLLinearMem_t *buff = (EWLLinearMem_t *)info;
	IM_Buffer buffer;
	
	assert(enc_ewl->alc != IM_NULL);
	
	if(alc_alloc(enc_ewl->alc, size, &buffer, ALC_FLAG_PHY_MUST) != IM_RET_OK){
		PTRACE("EWLMallocLinear: alc_alloc() failed\n");
		return EWL_ERROR;
	}
	buff->virtualAddress = buffer.vir_addr;
	buff->busAddress = buffer.phy_addr;
	buff->size = buffer.size;

	return EWL_OK;
}

/*------------------------------------------------------------------------------
  Function name   : EWLFreeLinear
Description     : Release a linera memory buffer, previously allocated with 
EWLMallocLinear.

Return type     : void 

Argument        : const void * instance - EWL instance
Argument        : EWLLinearMem_t *info - linear buffer memory information
------------------------------------------------------------------------------*/
void EWLFreeLinear(const void *instance, EWLLinearMem_t * info)
{
	hx280ewl_t *enc_ewl = (hx280ewl_t *)instance;
	EWLLinearMem_t *buff = (EWLLinearMem_t *)info;
	IM_Buffer buffer;

	assert(enc_ewl->alc != IM_NULL);

	buffer.vir_addr = buff->virtualAddress;
	buffer.phy_addr = buff->busAddress;
	buffer.size = buff->size;
	if(alc_free(enc_ewl->alc, &buffer) != IM_RET_OK){
		PTRACE("EWLFreeLinear: alc_free() failed\n");
		return;
	}
}

/*******************************************************************************
  Function name   : EWLReleaseHw
Description     : Release HW resource when frame is ready
 *******************************************************************************/
void EWLReleaseHw(const void *inst)
{
	u32 val;
	hx280ewl_t *enc = (hx280ewl_t *) inst;
    PTRACE("%s()\n", __FUNCTION__);

	assert(enc != NULL);

	val = EWLReadReg(inst, 0x38);
	EWLWriteReg(inst, 0x38, val & (~0x01)); /* reset ASIC */

	if(ioctl(enc->fd_enc, HX280ENC_IOC_RELEASE_HW, NULL))
	{
		PTRACE("ioctl(HX280ENC_IOC_RELEASE_HW) failed\n");
	}
}

/* SW/SW shared memory */
/*------------------------------------------------------------------------------
  Function name   : EWLmalloc
Description     : Allocate a memory block. Same functionality as
the ANSI C malloc()

Return type     : void pointer to the allocated space, or NULL if there
is insufficient memory available

Argument        : u32 n - Bytes to allocate
------------------------------------------------------------------------------*/
void *EWLmalloc(u32 n)
{
	void *p = malloc((size_t) n);

	return p;
}

/*------------------------------------------------------------------------------
  Function name   : EWLfree
Description     : Deallocates or frees a memory block. Same functionality as
the ANSI C free()

Return type     : void 

Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void EWLfree(void *p)
{
	if(p != NULL)
		free(p);
}

/*------------------------------------------------------------------------------
  Function name   : EWLcalloc
Description     : Allocates an array in memory with elements initialized
to 0. Same functionality as the ANSI C calloc()

Return type     : void pointer to the allocated space, or NULL if there
is insufficient memory available

Argument        : u32 n - Number of elements
Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *EWLcalloc(u32 n, u32 s)
{
	void *p = calloc((size_t) n, (size_t) s);

	return p;
}

/*------------------------------------------------------------------------------
  Function name   : EWLmemcpy
Description     : Copies characters between buffers. Same functionality as
the ANSI C memcpy()

Return type     : The value of destination d

Argument        : void *d - Destination buffer
Argument        : const void *s - Buffer to copy from
Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *EWLmemcpy(void *d, const void *s, u32 n)
{
	return memcpy(d, s, (size_t) n);
}

/*------------------------------------------------------------------------------
  Function name   : EWLmemset
Description     : Sets buffers to a specified character. Same functionality
as the ANSI C memset()

Return type     : The value of destination d

Argument        : void *d - Pointer to destination
Argument        : i32 c - Character to set
Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
void *EWLmemset(void *d, i32 c, u32 n)
{
	return memset(d, (int) c, (size_t) n);
}
