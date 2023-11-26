/*
 * raster.h
 *
 *  Created on: Nov 20, 2023
 *      Author: TomasHägg
 */

#ifndef RASTER_H_
#define RASTER_H_

#include "VGA_raster_csr.h"

// Read and write macros for the component
#define VGA_RASTER_read(offset)               IORD_32DIRECT  (VGA_RASTER_VGA_RASTER_INTERNAL_INST_AVS_CRA_BASE, offset)
#define VGA_RASTER_write(offset, data)        IOWR_32DIRECT  (VGA_RASTER_VGA_RASTER_INTERNAL_INST_AVS_CRA_BASE, offset, data)

// Read and write to memory attached to component
#define result_memory_read(offset)          IORD_32DIRECT  (VGA_RASTER_VGA_RASTER_INTERNAL_INST_AVS_TRI_ARGS_BASE, offset)
#define sample_memory_write(offset, data)   IOWR_32DIRECT  (VGA_RASTER_VGA_RASTER_INTERNAL_INST_AVS_TRI_ARGS_BASE, offset, data)

// Component register macros
#define VGA_RASTER_enable_interrupt()   VGA_RASTER_write  (VGA_RASTER_CSR_INTERRUPT_ENABLE_REG, VGA_RASTER_CSR_INTERRUPT_ENABLE_MASK & 0x1)
#define VGA_RASTER_disable_interrupt()  VGA_RASTER_write  (VGA_RASTER_CSR_INTERRUPT_ENABLE_REG, VGA_RASTER_CSR_INTERRUPT_ENABLE_MASK & 0x0)
#define VGA_RASTER_clear_done_status()  VGA_RASTER_write  (VGA_RASTER_CSR_INTERRUPT_STATUS_REG, VGA_RASTER_CSR_INTERRUPT_STATUS_MASK & 0x1)
#define VGA_RASTER_start()		      	VGA_RASTER_write  (VGA_RASTER_CSR_START_REG, VGA_RASTER_CSR_START_MASK & 0x1)
#define VGA_RASTER_busy()               VGA_RASTER_read   (VGA_RASTER_CSR_BUSY_REG) & VGA_RASTER_CSR_BUSY_MASK
#define VGA_RASTER_done()               VGA_RASTER_read   (VGA_RASTER_CSR_INTERRUPT_STATUS_REG) & VGA_RASTER_CSR_INTERRUPT_STATUS_MASK
#endif /* RASTER_H_ */
