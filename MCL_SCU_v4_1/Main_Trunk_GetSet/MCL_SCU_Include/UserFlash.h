/******************************************************************************
  **@file     :    UserFlash.h
  * 
  * @brief   	:		 MACROS, Variables and Functions used for Data logging
  ******************************************************************************
  */

/* ----------------------------------------------------------------------------
 *                           Includes
 * ----------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ----------------------------------------------------------------------------
 */

#define FLASH_PAGE_SIZE         							((uint32_t)0x0000FFFF)   /* FLASH Page Size */
#define FLASH_SUPERVISOR_NAME_START_ADDR   		((uint32_t)0x08040000)   /* Start @ of user Flash area */
#define FLASH_SUPERVISOR_NAME_END_ADDR     		((uint32_t)0x0804FFFF)   /* End @ of user Flash area */
#define FLASH_JOB_NAME_START_ADDR   					((uint32_t)0x08060000)   /* Start @ of user Flash area */
#define FLASH_JOB_NAME_END_ADDR     					((uint32_t)0x0806FFFF)   /* End @ of user Flash area */
#define FLASH_JOB_ID_START_ADDR   						((uint32_t)0x08080000)   /* Start @ of user Flash area */
#define FLASH_JOB_ID_END_ADDR     						((uint32_t)0x0808FFFF)   /* End @ of user Flash area */
#define FLASH_RX_TX_BUFFER_SIZE								((uint8_t)22)
#define FLASH_SUPERVISOR_NAME_ADDR(index)		 	(FLASH_SUPERVISOR_NAME_START_ADDR + (FLASH_RX_TX_BUFFER_SIZE*index))
#define FLASH_JOB_NAME_ADDR(index)		 				(FLASH_JOB_NAME_START_ADDR + (FLASH_RX_TX_BUFFER_SIZE*index))
#define FLASH_JOB_ID_ADDR(index)		 					(FLASH_JOB_ID_START_ADDR + (FLASH_RX_TX_BUFFER_SIZE*index))

#define FLASH_MAX_INDEX												((uint8_t)255)
