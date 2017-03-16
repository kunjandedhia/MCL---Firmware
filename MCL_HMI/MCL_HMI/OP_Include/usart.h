/* Define size of allocated UART RX/TX buffer*/
#define SIZE_OF_UART_RX_BUFFER (uint_16)50
#define SIZE_OF_UART_TX_BUFFER SIZE_OF_UART_RX_BUFFER

extern int UARTgets(char *pcBuf, unsigned long ulLen);
extern void UARTprintf(const char *pcString, ...);
extern int UARTwrite(const char *pcBuf, unsigned long ulLen);
