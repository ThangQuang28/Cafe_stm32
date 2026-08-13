/**
  **************************************************************************************************
  * @file   trace.h
  * @brief  Starting point of this project.
  * @by     duonghd | 31-Dec-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * This file is part of WhiteCat-FWCore.
  **************************************************************************************************
  */

/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

#ifndef _ZTRACE_H
#define _ZTRACE_H

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
#include "zcfg.h"

/* Middleware. */
#include "led.h"
#include "zlog.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (ZASSERT)
#define ZTRACE_ASSERT(expr)     if ((expr) == 0) { ztrace_assert(); }
#else   /* WASSERT */
#define ZTRACE_ASSERT(expr)
#endif  /* WASSERT */

#define ZTRACE_ON       0x80
#define ZTRACE_OFF      0x00

#if defined (ZERROR)
/* If "expr" isn't true, then print "msg" and execute "hdl" expression. */
#define ZTRACE_ERR(dbg, msg, expr, hdl)     do {                                                \
												if (!(expr))                                    \
												{                                               \
													if ((dbg) & ZTRACE_ON)                      \
													{                                           \
														zlog_printf msg;						\
													}                                           \
													hdl;                                        \
												}                                               \
											} while (0);
#else   /* VERROR */
/* If "expr" isn't true, then execute "hdl" expression. */
#define ZTRACE_ERR(dbg, msg, expr, hdl)     do {                                                \
												if (!(expr))                                    \
												{                                               \
													hdl;                                        \
												}                                               \
											} while (0);
#endif  /* VERROR */


#if defined (ZDEBUG)
/* If "dbg" flag is ZTRACE_ON, then print "msg". */
#define ZTRACE_DBG(dbg, msg)    do {                                                            \
									if ((dbg) & ZTRACE_ON)                                      \
									{                                                           \
										zlog_printf msg;										\
									}                                                           \
								} while (0);

/* If "dbg" flag is ZTRACE_ON, then print "msg", use in isr context. */
#define ZTRACE_DBG_ISR(dbg, msg)    do {                                                        \
										if ((dbg) & ZTRACE_ON)                                  \
										{                                                       \
										}                                                       \
									} while (0);
#else   /* ZDEBUG */
#define ZTRACE_DBG(dbg, msg)
#define ZTRACE_DBG_ISR(dbg, msg)
#endif  /* ZDEBUG */

/* Public data types -----------------------------------------------------------------------------*/
/* Public function prototypes --------------------------------------------------------------------*/

/**
  * @brief  Initialize ztrace module.
  * @param  None.
  * @retval werr_t.
  */
zerr_t
ztrace_open (void);

/**
  * @brief  This function is called by VTRACE_ASSERT() macro throughout the source files to check
  *         how the application is using FreeRTOS. It is highly recommended to develop FreeRTOS
  *         applications with VTRACE_ASSERT() defined. An assertion is triggered if the parameter
  *         passed into VTRACE_ASSERT() is zero. It is normal to define VTRACE_ASSERT() in such a
  *         way that it will prevent the application from executing any further. This if for
  *         two reasons; stopping the application at the point of the assertion allows the cause of
  *         the assertion to be debugged, and executing past a triggered assertion will probably
  *         result in a crash anyway.
  * @note   Defining VTRACE_ASSERT() will increase both the application code size and execution
  *         time. When the application is stable the additional overhead can be removed by
  *         simply modify the ZASSERT macro to 0 in zcfg.h.
  * @param  None.
  * @retval None.
  */
void
ztrace_assert (void);

#ifdef __cplusplus
}
#endif

#endif /* _TRACE_H */

/* END OF FILE ************************************************************************************/
