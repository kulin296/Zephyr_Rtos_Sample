#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
//#define UART_DEVICE_NODE DT_NODELABEL(usart2) 
const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

void uart_tx_str(unsigned char *msg)
{
    for (int i = 0; msg[i] != '\0'; i++) {
        uart_poll_out(uart_dev, msg[i]);
    }
}

unsigned char * uart_rx_str(int w)
{
    int i,ret=1,c=0;
    unsigned char *str=malloc(w+1);
    while(c != w)
    {
        while(ret != 0)
            ret=uart_poll_in(uart_dev,&str[i]);
        uart_poll_out(uart_dev,str[i]);
        if(ret==0){
            c++;
            i++;
        }
        ret=1;
    }
    str[i]='\0';
   // str[i]='\n';


    return str;
}

void main(void)
{
    
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }
    //char *msg = "FHS!\r\n";
    char *msg = "Enter Number of character: ";
    unsigned char *str;
    char n;
    int ret;
    uart_tx_str(msg);
    while(ret != 0)
            ret=uart_poll_in(uart_dev,&n);
    uart_poll_out(uart_dev,n);
    uart_tx_str("\n\r");
    str=uart_rx_str(n-48);
    uart_tx_str("\n\r");
    uart_tx_str(str);
}

// /*Optimized Code*/

// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/uart.h>
// #include <zephyr/sys/printk.h>
// #include <stdlib.h>

// #define UART_NODE DT_CHOSEN(zephyr_console)

// static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

// void uart_tx_str(const char *msg)
// {
//     while (*msg) {
//         uart_poll_out(uart_dev, *msg++);
//     }
// }

// char *uart_rx_str(size_t len)
// {
//     char *str = k_malloc(len + 1);
//     if (!str) {
//         uart_tx_str("Memory allocation failed!\r\n");
//         return NULL;
//     }

//     size_t i = 0;
//     uint8_t ch;
//     int ret;

//     while (i < len) {
//         ret = uart_poll_in(uart_dev, &ch);
//         if (ret == 0) {
//             uart_poll_out(uart_dev, ch);  // Echo back
//             str[i++] = ch;
//         } else {
//             k_msleep(10);  // Avoid busy-waiting
//         }
//     }

//     str[i] = '\0';
//     return str;
// }

// void main(void)
// {
//     if (!device_is_ready(uart_dev)) {
//         printk("UART device not ready\n");
//         return;
//     }

//     uart_tx_str("Enter number of characters: ");

//     uint8_t ch;
//     int ret;

//     do {
//         ret = uart_poll_in(uart_dev, &ch);
//         k_msleep(10);
//     } while (ret != 0);

//     uart_poll_out(uart_dev, ch);  // Echo input
//     uart_tx_str("\r\n");

//     int len = ch - '0';
//     if (len <= 0 || len > 100) {
//         uart_tx_str("Invalid length!\r\n");
//         return;
//     }

//     char *input = uart_rx_str(len);
//     if (input) {
//         uart_tx_str("\r\nYou entered: ");
//         uart_tx_str(input);
//         uart_tx_str("\r\n");
//         k_free(input);
//     }
// }
