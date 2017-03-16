

/* Configuration initialization parameters for  RS-232*/
#define SERIAL_PORT                   P1
#define TXD_PIN                       4
#define RXD_PIN                       5
#define RTS_PIN                       6
#define CTS_PIN                       7
#define TXD                           P1_4
#define RXD                           P1_5
#define RTS                           P1_6
#define CTS                           P1_7
#define BAUD_RATE                     1000.0

/* Define size of allocated UART RX/TX buffer*/
#define SIZE_OF_UART_RX_BUFFER 50
#define SIZE_OF_UART_TX_BUFFER SIZE_OF_UART_RX_BUFFER


extern int UARTgets(char *pcBuf, unsigned long ulLen);

extern void UARTprintf(const char *pcString, ...);
extern int UARTwrite(const char *pcBuf, unsigned long ulLen);



