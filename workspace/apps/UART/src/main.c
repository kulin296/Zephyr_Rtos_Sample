#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
//#define UART_DEVICE_NODE DT_NODELABEL(usart2) 

void main(void)
{
    const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }
    while(1){
    const char *msg = "FHS!\r\n";
    for (int i = 0; msg[i] != '\0'; i++) {
        uart_poll_out(uart_dev, msg[i]);
    }
    k_msleep(1000);
    }
}

// /*Optimized Code*/
// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/uart.h>
// #include <zephyr/sys/printk.h>

// #define UART_NODE DT_CHOSEN(zephyr_console)
// // #define UART_NODE DT_NODELABEL(usart2) // Optional override

// #define UART_MSG "FHS!\r\n"
// #define UART_DELAY_MS 1000

// void main(void)
// {
//     const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

//     if (!device_is_ready(uart_dev)) {
//         printk("Error: UART device %s is not ready\n", uart_dev->name);
//         return;
//     }

//     const char *msg = UART_MSG;

//     while (1) {
//         for (size_t i = 0; msg[i] != '\0'; i++) {
//             uart_poll_out(uart_dev, msg[i]);
//         }
//         k_msleep(UART_DELAY_MS);
//     }
// }
