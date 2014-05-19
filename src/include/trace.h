/*!
 * \file trace.h
 * \author Claude BIDEAU
 * \version 0.1
 * \brief This module provides print traces definition with different level.
 * \date  2014/03/21
 * 
 * \section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This module provides  print traces definition with different level.
 *
 */
                       
#ifndef __TRACE_H_
#define __TRACE_H_

/**********************************************************************
**   1a. Standard Library Include                                    **
**********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/**********************************************************************
**   1b. Include files  (own)                                        **
**********************************************************************/

/**********************************************************************
**   2a.   External Functions                                        **
**********************************************************************/

/**********************************************************************
**   2b.   External Data                                             **
**********************************************************************/

/**********************************************************************
**   3.  Type Definitions                                            **
**********************************************************************/

/**********************************************************************
**   4.  Global Functions (Exported)                                 **
**********************************************************************/
extern int print_trace(char * fmt,...);

extern int Q_i32Str2Byte(const char * F_pcStr, unsigned char * F_u8aData, char * F_pcaSeperator, char * F_caStrConv);
extern int Q_i32Byte2Str(unsigned char * F_u8aData, char * F_pcStr, char * F_caStrConv, char * F_pcaSeperator, unsigned int F_u32Size);

/**********************************************************************
**   5.  Local Functions                                             **
**********************************************************************/

/**********************************************************************
**   6.  Macros / Defines                                            **
**********************************************************************/

#define TRACES_LEVEL_ERROR		0x01
#define TRACES_LEVEL_WARNING    0x02
#define TRACES_LEVEL_CMD		0x04
#define TRACES_LEVEL_MSG		0x08
#define TRACES_LEVEL_LOAD		0x10
#define TRACES_LEVEL_INFO		0x20
#define TRACES_LEVEL_DEBUG		0x40

#define TRACES_ERROR(message)   { if ((E_u32TraceLevel & TRACES_LEVEL_ERROR) != 0) print_trace("\033[0;31m ERROR  ) %s : %4d : %s \033[0m \n",__FILE__,__LINE__,message); }
#define TRACES_WARNING(message) { if ((E_u32TraceLevel & TRACES_LEVEL_WARNING) != 0) print_trace("\033[0;34m WARNING) %s : %4d : %s \033[0m \n",__FILE__,__LINE__,message); }
#define TRACES_CMD(message)     { if ((E_u32TraceLevel & TRACES_LEVEL_CMD) != 0)     print_trace("\033[0;32m CMD    )\033[0m  %s  \n",message); }
#define TRACES_MSG(message)     { if ((E_u32TraceLevel & TRACES_LEVEL_MSG) != 0)     print_trace("\033[0;32m MSG    )\033[0m %s  \n",message); }
#define TRACES_LOAD(message)    { if ((E_u32TraceLevel & TRACES_LEVEL_LOAD) != 0)    print_trace("\033[0;32m LOAD   )\033[0m %s  \n",message); }
#define TRACES_INFO(message)    { if ((E_u32TraceLevel & TRACES_LEVEL_INFO) != 0)    print_trace("\033[0;33m INFO   )\033[0m %s  \n",message); }
#define TRACES_DEBUG(message)   { if ((E_u32TraceLevel & TRACES_LEVEL_DEBUG) != 0)   print_trace("\033[0;35m DEBUG  )\033[0m %s : %4d : %s  \n",__FILE__,__LINE__,message); }

#define TRACES_ERROR_ARG1(message,arg)                      { if ((E_u32TraceLevel & TRACES_LEVEL_ERROR) != 0)   print_trace("\033[0;31m ERROR  ) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg); }
#define TRACES_ERROR_ARG2(message,arg1,arg2)                { if ((E_u32TraceLevel & TRACES_LEVEL_ERROR) != 0)   print_trace("\033[0;31m ERROR  ) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2); }
#define TRACES_ERROR_ARG3(message,arg1,arg2,arg3)           { if ((E_u32TraceLevel & TRACES_LEVEL_ERROR) != 0)   print_trace("\033[0;31m ERROR  ) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2,arg3); }
#define TRACES_ERROR_ARG4(message,arg1,arg2,arg3,arg4)      { if ((E_u32TraceLevel & TRACES_LEVEL_ERROR) != 0)   print_trace("\033[0;31m ERROR  ) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2,arg3,arg4); }
#define TRACES_ERROR_ARG5(message,arg1,arg2,arg3,arg4,arg5) { if ((E_u32TraceLevel & TRACES_LEVEL_ERROR) != 0)   print_trace("\033[0;31m ERROR  ) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2,arg3,arg4,arg5); }

#define TRACES_WARNING_ARG1(message,arg)                 { if ((E_u32TraceLevel & TRACES_LEVEL_WARNING) != 0) print_trace("\033[0;34m WARNING) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg); }
#define TRACES_WARNING_ARG2(message,arg1,arg2)           { if ((E_u32TraceLevel & TRACES_LEVEL_WARNING) != 0) print_trace("\033[0;34m WARNING) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2); }
#define TRACES_WARNING_ARG3(message,arg1,arg2,arg3)      { if ((E_u32TraceLevel & TRACES_LEVEL_WARNING) != 0) print_trace("\033[0;34m WARNING) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2,arg3); }
#define TRACES_WARNING_ARG4(message,arg1,arg2,arg3,arg4) { if ((E_u32TraceLevel & TRACES_LEVEL_WARNING) != 0) print_trace("\033[0;34m WARNING) %s : %4d : "message" \033[0m \n",__FILE__,__LINE__,arg1,arg2,arg3,arg4); }

#define TRACES_MSG_ARG1(message,arg1)               { if ((E_u32TraceLevel & TRACES_LEVEL_MSG) != 0)     print_trace("\033[0;32m MSG    )\033[0m "message"  \n",arg1); }
#define TRACES_MSG_ARG2(message,arg1,arg2)          { if ((E_u32TraceLevel & TRACES_LEVEL_MSG) != 0)     print_trace("\033[0;32m MSG    )\033[0m "message"  \n",arg1,arg2); }
#define TRACES_MSG_ARG3(message,arg1,arg2,arg3)     { if ((E_u32TraceLevel & TRACES_LEVEL_MSG) != 0)     print_trace("\033[0;32m MSG    )\033[0m "message"  \n",arg1,arg2,arg3); }


#define TRACES_INFO_ARG1(message,arg)                    { if ((E_u32TraceLevel & TRACES_LEVEL_INFO) != 0)    print_trace("\033[0;33m INFO   )\033[0m "message" \n",arg); }
#define TRACES_INFO_ARG2(message,arg1,arg2)              { if ((E_u32TraceLevel & TRACES_LEVEL_INFO) != 0)    print_trace("\033[0;33m INFO   )\033[0m "message" \n",arg1,arg2); }
#define TRACES_INFO_ARG3(message,arg1,arg2,arg3)         { if ((E_u32TraceLevel & TRACES_LEVEL_INFO) != 0)    print_trace("\033[0;33m INFO   )\033[0m "message" \n",arg1,arg2,arg3); }
#define TRACES_INFO_ARG4(message,arg1,arg2,arg3,arg4)    { if ((E_u32TraceLevel & TRACES_LEVEL_INFO) != 0)    print_trace("\033[0;33m INFO   )\033[0m "message" \n",arg1,arg2,arg3,arg4); }

#define TRACES_LOAD_ARG1(message,arg)                   { if ((E_u32TraceLevel & TRACES_LEVEL_LOAD) != 0)    print_trace("\033[0;32m LOAD   )\033[0m "message" \n",arg); }
#define TRACES_LOAD_ARG2(message,arg1,arg2)             { if ((E_u32TraceLevel & TRACES_LEVEL_LOAD) != 0)    print_trace("\033[0;32m LOAD   )\033[0m "message" \n",arg1,arg2); }
#define TRACES_LOAD_ARG3(message,arg1,arg2,arg3)        { if ((E_u32TraceLevel & TRACES_LEVEL_LOAD) != 0)    print_trace("\033[0;32m LOAD   )\033[0m "message" \n",arg1,arg2,arg3); }
#define TRACES_LOAD_ARG4(message,arg1,arg2,arg3,arg4)   { if ((E_u32TraceLevel & TRACES_LEVEL_LOAD) != 0)    print_trace("\033[0;32m LOAD   )\033[0m "message" \n",arg1,arg2,arg3,arg4); }

#define TRACES_DEBUG_ARG1(message,arg)              { if ((E_u32TraceLevel & TRACES_LEVEL_DEBUG) != 0)   print_trace("\033[0;35m DEBUG  )\033[0m %s : %4d : "message"  \n",__FILE__,__LINE__,arg); }
#define TRACES_DEBUG_ARG2(message,arg1,arg2)        { if ((E_u32TraceLevel & TRACES_LEVEL_DEBUG) != 0)   print_trace("\033[0;35m DEBUG  )\033[0m %s : %4d : "message"  \n",__FILE__,__LINE__,arg1,arg2); }
#define TRACES_DEBUG_ARG3(message,arg1,arg2,arg3)   { if ((E_u32TraceLevel & TRACES_LEVEL_DEBUG) != 0)   print_trace("\033[0;35m DEBUG  )\033[0m %s : %4d : "message"  \n",__FILE__,__LINE__,arg1,arg2,arg3); }
#define TRACES_DEBUG_ARG4(message,arg1,arg2,arg3,arg4)   { if ((E_u32TraceLevel & TRACES_LEVEL_DEBUG) != 0)   print_trace("\033[0;35m DEBUG  )\033[0m %s : %4d : "message"  \n",__FILE__,__LINE__,arg1,arg2,arg3,arg4); }
#define TRACES_DEBUG_ARG5(message,arg1,arg2,arg3,arg4,arg5)   { if ((E_u32TraceLevel & TRACES_LEVEL_DEBUG) != 0)   print_trace("\033[0;35m DEBUG  )\033[0m %s : %4d : "message"  \n",__FILE__,__LINE__,arg1,arg2,arg3,arg4,arg5); }


/**********************************************************************
**   7.  Global Data                                                 **
**********************************************************************/
extern unsigned int E_u32TraceLevel;



/**********************************************************************
*************************    END MODULE *******************************
**********************************************************************/
#endif /* __TRACE_H_ */
