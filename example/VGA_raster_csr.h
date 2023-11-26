
/* This header file describes the CSR Slave for the VGA_raster component */

#ifndef __VGA_RASTER_CSR_REGS_H__
#define __VGA_RASTER_CSR_REGS_H__



/******************************************************************************/
/* Memory Map Summary                                                         */
/******************************************************************************/

/*
  Register  | Access  |   Register Contents      | Description
  Address   |         |      (64-bits)           | 
------------|---------|--------------------------|-----------------------------
        0x0 |       R |         {reserved[62:0], |     Read the busy status of
            |         |               busy[0:0]} |               the component
            |         |                          |  0 - the component is ready
            |         |                          |       to accept a new start
            |         |                          |    1 - the component cannot
            |         |                          |          accept a new start
------------|---------|--------------------------|-----------------------------
        0x8 |       W |         {reserved[62:0], |  Write 1 to signal start to
            |         |              start[0:0]} |               the component
------------|---------|--------------------------|-----------------------------
       0x10 |     R/W |         {reserved[62:0], |      0 - Disable interrupt,
            |         |   interrupt_enable[0:0]} |        1 - Enable interrupt
------------|---------|--------------------------|-----------------------------
       0x18 |  R/Wclr |         {reserved[61:0], | Signals component completion
            |         |               done[0:0], |       done is read-only and
            |         |   interrupt_status[0:0]} | interrupt_status is write 1
            |         |                          |                    to clear

NOTE: Writes to reserved bits will be ignored and reads from reserved
      bits will return undefined values.
*/


/******************************************************************************/
/* Register Address Macros                                                    */
/******************************************************************************/

/* Byte Addresses */
#define VGA_RASTER_CSR_BUSY_REG (0x0)
#define VGA_RASTER_CSR_START_REG (0x8)
#define VGA_RASTER_CSR_INTERRUPT_ENABLE_REG (0x10)
#define VGA_RASTER_CSR_INTERRUPT_STATUS_REG (0x18)

/* Argument Sizes (bytes) */

/* Argument Masks */

/* Status/Control Masks */
#define VGA_RASTER_CSR_BUSY_MASK   (1<<0)
#define VGA_RASTER_CSR_BUSY_OFFSET (0)

#define VGA_RASTER_CSR_START_MASK   (1<<0)
#define VGA_RASTER_CSR_START_OFFSET (0)

#define VGA_RASTER_CSR_INTERRUPT_ENABLE_MASK   (1<<0)
#define VGA_RASTER_CSR_INTERRUPT_ENABLE_OFFSET (0)

#define VGA_RASTER_CSR_INTERRUPT_STATUS_MASK   (1<<0)
#define VGA_RASTER_CSR_INTERRUPT_STATUS_OFFSET (0)
#define VGA_RASTER_CSR_DONE_MASK   (1<<1)
#define VGA_RASTER_CSR_DONE_OFFSET (1)


#endif /* __VGA_RASTER_CSR_REGS_H__ */



