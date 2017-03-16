/******************************************************************************
**@file     :   Ethernet_App.c

**@brief    :   Ethernet Tx RX application.
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
* All rights reserved. 
* 
* Redistribution and use in source and binary forms, with or without modification, 
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission. 
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
* OF SUCH DAMAGE.
*
* This file is part of and a contribution to the lwIP TCP/IP stack.
*
* Credits go to Adam Dunkels (and the current maintainers) of this software.
*
* Christiaan Simons rewrote this file to get a more stable echo example.

******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Config.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
RECEIVER_t stEthernet; 
struct tcp_pcb *pstConnectPCB;
/* ECHO protocol states */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};
/* structure for maintaing connection infos to be passed as argument 
to LwIP callbacks*/
struct Tcp_Server_Struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
static err_t fnServerAccept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t fnEthernetReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void fnServerError(void *arg, err_t err);
static err_t fnServerPoll(void *arg, struct tcp_pcb *tpcb);
static err_t fnServerSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void fnServerSend(struct tcp_pcb *tpcb, struct Tcp_Server_Struct *es);
static void fnServerClose(struct tcp_pcb *tpcb, struct Tcp_Server_Struct *es);


/*****************************************************************************
**@Function 		  : 	fnTcpServerInit
**@Descriptions	: 	Initializes the tcp server
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnTcpServerInit(void)
{
  /* create new tcp pcb */
  struct tcp_pcb *tcp_echoserver_pcb;
  tcp_echoserver_pcb = tcp_new();
  
  if (tcp_echoserver_pcb != NULL)
  {
    err_t err;
    
    /* bind tcp_echoserver_pcb to port (ECHO protocol) */
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, stEthernetIP.uiPort);
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb */
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
      
      /* initialize LwIP tcp_accept callback function */
      tcp_accept(tcp_echoserver_pcb, fnServerAccept);
    }
    else 
    {
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
#ifdef DBG_ETH
      printf("Can not bind pcb\n");
#endif
    }
  }
  else
  {
#ifdef DBG_ETH
    printf("Can not create new pcb\n");
#endif
  }
}
/*****************************************************************************
**@Function 		  : 	fnServerAccept
**@Descriptions	: 	This function is the implementation of tcp_accept LwIP callback
**@parameters		: 	arg			: not used
newpcb	: pointer on tcp_pcb struct for the newly created tcp connection
err			: not used 
**@return				: 	err_t: error status
*****************************************************************************/ 
static err_t fnServerAccept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct Tcp_Server_Struct *es;
  
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);
  
  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MAX);
  
  /* allocate structure es to maintain tcp connection informations */
  es = (struct Tcp_Server_Struct *)mem_malloc(sizeof(struct Tcp_Server_Struct));
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    pstConnectPCB=newpcb;
    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, es);
    
    /* initialize lwip tcp_recv callback function for newpcb  */ 
    tcp_recv(newpcb, fnEthernetReceive);
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(newpcb, fnServerSent);
    
    /* initialize lwip tcp_err callback function for newpcb  */
    tcp_err(newpcb, fnServerError);
    
    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, fnServerPoll, 1);
    
    /*Set Ethernet connected Flag*/
    stEthernet.ucConnectedStatus = SET;
#ifdef DBG_ETH
    printf("\n\rEthernet Connected");
#endif
    ret_err = ERR_OK;
  }
  else
  {
    /*  close tcp connection */
    fnServerClose(newpcb, es);
    /* return memory error */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}

/*****************************************************************************
**@Function 		  : 	fnEthernetReceive
**@Descriptions	: 	This function is the implementation for tcp_recv LwIP callback
**@parameters		: 	arg		: pointer on a argument for the tcp_pcb connection
tpcb	: pointer on the tcp_pcb connection
pbuf	: pointer on the received pbuf
err		: error information regarding the reveived pbuf
**@return				: 	err_t: error code
*****************************************************************************/ 
static err_t fnEthernetReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct Tcp_Server_Struct *es;
  err_t ret_err;
  
  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct Tcp_Server_Struct *)arg;
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
      /* we're done sending, close connection */
      fnServerClose(tpcb, es);
    }
    /* ADDED and Commented on 17NOV15*/
    //														    else
    //														    {
    
    //														      /* we're not done yet */
    //														      /* acknowledge received packet */
    //														      tcp_sent(tpcb, fnServerSent);
    //														      
    //														      /* send remaining data*/
    //																	fnServerSend(tpcb, es);
    
    //														    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    es->p = NULL;
    pbuf_free(p);
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
    uint8_t *pc;
    
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    
    pc = (uint8_t *)es->p->payload;
    if(stEthernet.uiReceivePtr < RECEIVE_BUFFER)
      stEthernet.rgucReceiveBuff[stEthernet.uiReceivePtr++] = pc[0];
    else
    {
#ifdef DBG_ETH
      printf("\n\rEthernet Buffer Full");
#endif
      /*Empty Ethernet Receiving Buffer*/
      memset(stEthernet.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
      /*Move Received Buffer pointer to Zero position*/
      stEthernet.uiReceivePtr = 0;
    }
    
    /* initialize LwIP tcp_sent callback function */
    //	tcp_sent(tpcb, fnServerSent);
    
    
    
    /* ADDED and Commented on 17NOV15*/
    
    
    /* free pbuf and do nothing */
    es->p = NULL;
    pbuf_free(p);
    //    /* send back the received data (echo) */
    //   fnServerSend(tpcb, es);
    
    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {
    uint16_t uiLoop=0;
    uint8_t *pc;
    /* more data received from client and previous data has been already sent*/
    if(es->p == NULL)
    {
      es->p = p;
      
      pc = (uint8_t *)es->p->payload;
      
      for( uiLoop=0;es->p->tot_len > uiLoop; uiLoop++ )
      {
        if(stEthernet.uiReceivePtr < RECEIVE_BUFFER)
        {
          stEthernet.rgucReceiveBuff[stEthernet.uiReceivePtr++] = pc[uiLoop];
          if(pc[uiLoop]=='>')
          {
            //	if(strstr((char *)stEthernet.rgucReceiveBuff,"</CRC-Evans_MCL>"))
            //	{
            /*Ethernet Received flag set to process data*/
            stEthernet.ucReceivedFlag = SET;
            //	}
          }
        }
        else
        {
#ifdef DBG_ETH
          printf("\n\rEthernet Buffer Full");
#endif
          /*Empty Ethernet Receiving Buffer*/
          memset(stEthernet.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
          /*Move Received Buffer pointer to Zero position*/
          stEthernet.uiReceivePtr = 0;
        }
      }
      
      /* ADDED and Commented on 17NOV15*/
      /* free pbuf and do nothing */
      es->p = NULL;
      pbuf_free(p);
      //      /* send back received data */
      //   fnServerSend(tpcb, es);
    }
    else
    {
      struct pbuf *ptr;
      
      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
      
    }
    ret_err = ERR_OK;
  }
  
  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}
/*****************************************************************************
**@Function 		  : 	fnServerError
**@Descriptions	: 	This function implements the tcp_err callback function (called
** 					        when a fatal tcp_connection error occurs. 
**@parameters		: 	arg: pointer on argument parameter 
** 									err: not used
**@return				: 	None
*****************************************************************************/ 
static void fnServerError(void *arg, err_t err)
{
  struct Tcp_Server_Struct *es;
  
  LWIP_UNUSED_ARG(err);
  
  es = (struct Tcp_Server_Struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/*****************************************************************************
**@Function 		  : 	fnServerPoll
**@Descriptions	: 	This function implements the tcp_poll LwIP callback function
**@parameters		: 	arg		: pointer on argument passed to callback
tpcb	: pointer on the tcp_pcb for the current tcp connection
**@return				: 	err_t: error code
*****************************************************************************/ 
static err_t fnServerPoll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct Tcp_Server_Struct *es;
  es = (struct Tcp_Server_Struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      fnServerSend(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        fnServerClose(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}
/*****************************************************************************
**@Function 		  : 	fnServerSent
**@Descriptions	: 	This function implements the tcp_sent LwIP callback (called when ACK
**						        is received from remote host for sent data) 
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
static err_t fnServerSent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct Tcp_Server_Struct *es;
  
  LWIP_UNUSED_ARG(len);
  
  es = (struct Tcp_Server_Struct *)arg;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    fnServerSend(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      fnServerClose(tpcb, es);
  }
  return ERR_OK;
}

/*****************************************************************************
**@Function 		  : 	fnServerSend
**@Descriptions	: 	This function is used to send data for tcp connection
**@parameters		: 	tpcb: pointer on the tcp_pcb connection
**  										es: pointer on echo_state structure
**@return				: 	None
*****************************************************************************/ 
static void fnServerSend(struct tcp_pcb *tpcb, struct Tcp_Server_Struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
  
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
           (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;
    
    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK)
    {
      u16_t plen;
      
      plen = ptr->len;
      
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      
      if(es->p != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);
      
      /* Update tcp window size to be advertized : should be called when received
      data (with the amount plen) has been processed by the application layer */
      tcp_recved(tpcb, plen);
    }
    else if(wr_err == ERR_MEM)
    {
      /* we are low on memory, try later / harder, defer to poll */
      es->p = ptr;
    }
  }
}
/*****************************************************************************
**@Function 		  : 	fnServerClose
**@Descriptions	: 	This functions closes the tcp connection
**@parameters		: 	tcp_pcb	: pointer on the tcp connection
es			: pointer on echo_state structure
**@return				: 	None
*****************************************************************************/ 
static void fnServerClose(struct tcp_pcb *tpcb, struct Tcp_Server_Struct *es)
{
  
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
    mem_free(es);
  } 
  /* Reset Ethernet Link Flag */
  stEthernet.ucConnectedStatus = RESET;	
#ifdef DBG_ETH
  printf("\n\rEthernet Disconnected");
#endif
  /* close tcp connection */
  tcp_close(tpcb);
}
/*****************************************************************************
**@Function 		  : 	fnEthernetSend
**@Descriptions	: 	Data Transmission Over Ethernet
**@parameters		: 	pucTxData : pointer to Transmit string
**@return				: 	None
*****************************************************************************/ 
void fnEthernetSend(uint8_t *pucTxData)
{
  if(stEthernet.ucConnectedStatus == SET)
  {
    struct Tcp_Server_Struct *es = NULL;
    /* allocate structure es to maintain tcp connection informations */
    es = (struct Tcp_Server_Struct *)mem_malloc(sizeof(struct Tcp_Server_Struct));
    
    if (es != NULL)
    {
      es->state = ES_ACCEPTED;
      es->pcb = pstConnectPCB;
      
      /* allocate pbuf */
      es->p = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)pucTxData) , PBUF_POOL);
      
      if (es->p)
      {       
        /* copy data to pbuf */
        pbuf_take(es->p, (char*)pucTxData, strlen((char*)pucTxData));
        
        /* send data */
        fnServerSend(pstConnectPCB,es);
        
        fnDelay_ms(10);
        /* free pbuf */
        pbuf_free(es->p);
        es->p = NULL;
        
        /* delete es structure */
        mem_free(es);
      }
    }
    else
    {
      /* close connection */
      fnServerClose(pstConnectPCB, es);
    }
    //		#ifdef DBG_ETH
    //		printf("\n\rSend on Ethernet");
    //		#endif
    //		err_t wr_err = ERR_OK;
    //		if(strlen((char*)pucTxData) <= tcp_sndbuf(pstConnectPCB))
    //		{
    //			wr_err = tcp_write(pstConnectPCB, (char*)pucTxData, strlen((char*)pucTxData), 1);
    //			if (wr_err == ERR_OK)
    //			{
    //				//tcp_output(pstConnectPCB);
    //			}
    //			else if(wr_err == ERR_MEM)
    //			{
    //				#ifdef DBG_ETH
    //				printf("\n\rError");
    //				#endif
    //			}
    //		}
  }
}
/*****************************************************************************
**@Function 		  : 	fnEthernetProcessData
**@Descriptions	: 	Process Received Data through Ethernet
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnEthernetProcessData(void) 
{
  /*Check Ethernet Link*/
  fnEth_Link_Handler(KSZ8081_PHY_ADDR);		
  /*Ethernet Periodic Handle*/
  fnLwIP_Periodic_Handle(LocalTime);
  
  if(SET == stEthernet.ucReceivedFlag)
  {
    stEthernet.ucReceivedFlag = RESET;
#ifdef DBG_ETH
    printf("\n\rEthernet Rx : %s",stEthernet.rgucReceiveBuff);
#endif
    
    if(strstr((char *)stEthernet.rgucReceiveBuff,"</CRC-Evans_MCL>"))
    {
      if(strstr((char *)stEthernet.rgucReceiveBuff,"<CRC-Evans_MCL>"))
      {
        if(strstr((char *)stEthernet.rgucReceiveBuff,"Command"))
        {
#ifdef DBG_ETH
          printf("\n\rData Logging");
#endif
          /*Set flag if command recieved from Ethernet*/
          //stDatalog.ucETHSendFlag = SET;
          fnDataLogging(stEthernet.rgucReceiveBuff,&stEthernetLog);
          /*Empty Ethernet Receiving Buffer*/
          memset(stEthernet.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
          /*Move Received Buffer pointer to Zero position*/
          stEthernet.uiReceivePtr = 0;
        }
        //        else if(strstr((char *)stEthernet.rgucReceiveBuff,"Config"))
        //        {
        //#ifdef DBG_ETH
        //          printf("\n\rConfiguration");
        //#endif
        //          fnXMLConfigProcess(stEthernet.rgucReceiveBuff);
        //          /*Empty Ethernet Receiving Buffer*/
        //          memset(stEthernet.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
        //          /*Move Received Buffer pointer to Zero position*/
        //          stEthernet.uiReceivePtr = 0;
        //        }
        else
        {
#ifdef DBG_ETH
          printf("\n\rEhernet XML process");
#endif
          stEthernet.ucSendFlag = SET;
          fnXMLCmdProcess(stEthernet.rgucReceiveBuff);
          /*Empty Ethernet Receiving Buffer*/
          memset(stEthernet.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
          /*Move Received Buffer pointer to Zero position*/
          stEthernet.uiReceivePtr = 0;
        }
        //        /*Empty Ethernet Receiving Buffer*/
        //				memset(stEthernet.rgucReceiveBuff,'\0', strlen((char *)stEthernet.rgucReceiveBuff));
        //				/*Move Received Buffer pointer to Zero position*/
        //				stEthernet.uiReceivePtr = 0;
      }
    }
    //memset(stEthernet.rgucReceiveBuff,'\0', strlen((char *)stEthernet.rgucReceiveBuff));
    // stEthernet.uiReceivePtr = 0;//move to 0th position of recevie buffer
  }
}
/*****************************************************************************
**@Function 		  : 	fnEthernetConfig
**@Descriptions	: 	Configure Ethernet for Server Mode.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnEthernetConfig(void)
{
  /******************Initilaize the LwIP stack***************/
  fnLwIP_Init();
  /*******************Tcp main server Init*******************/
  fnTcpServerInit();
}


