#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

void uart_tx_str(unsigned char *msg)
{
    for (int i = 0; msg[i] != '\0'; i++) {
        uart_poll_out(uart_dev, msg[i]);
    }
}
void main()
{
    gpio_pin_configure_dt(&led, GPIO_OUTPUT);
   
    unsigned char ch,ch1;
    int ret,ret1;
    unsigned char *msg="Menu:\r\n1.Enter A : Turn On LED\r\n2.Enter B : Turn Off LED\r\nEnter Choice: ";
    
    while(1)
    {
        //k_msleep(1000);
        uart_tx_str(msg);
        ret= uart_poll_in(uart_dev,&ch);
        while(ret != 0)
        {
            ret= uart_poll_in(uart_dev,&ch);
        } 
        uart_poll_out(uart_dev,ch);
        uart_tx_str("\r\n");
        if(ch == 'A')
        {
            gpio_pin_set_dt(&led, 1);
            printk("LED: ON\n");
        }
        else if(ch == 'B')
        {
            gpio_pin_set_dt(&led, 0);
            printk("LED: OFF\n");
        }
        else
        {
            uart_tx_str("ERROR: Invalid Input!\r\n");
        }
    }
}

// /*Optimize Code*/
// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/gpio.h>
// #include <zephyr/drivers/uart.h>
// #include <zephyr/sys/printk.h>

// #define UART_NODE DT_CHOSEN(zephyr_console)
// #define LED_NODE  DT_ALIAS(led0)

// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
// static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

// void uart_tx_str(const char *msg)
// {
//     while (*msg) {
//         uart_poll_out(uart_dev, *msg++);
//     }
// }

// void main(void)
// {
//     if (!device_is_ready(uart_dev)) {
//         printk("Error: UART device not ready\n");
//         return;
//     }

//     if (!gpio_is_ready_dt(&led)) {
//         printk("Error: LED device not ready\n");
//         return;
//     }

//     if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
//         printk("Error: Failed to configure LED pin\n");
//         return;
//     }

//     const char *menu =
//         "\r\nMenu:\r\n"
//         "1. Enter A : Turn ON LED\r\n"
//         "2. Enter B : Turn OFF LED\r\n"
//         "Enter Choice: ";

//     while (1) {
//         uart_tx_str(menu);

//         unsigned char ch;
//         while (uart_poll_in(uart_dev, &ch) != 0) {
//             k_msleep(10);  // Avoid busy-waiting
//         }

//         uart_poll_out(uart_dev, ch);  // Echo input
//         uart_tx_str("\r\n");

//         switch (ch) {
//             case 'A':
//                 gpio_pin_set_dt(&led, 1);
//                 uart_tx_str("LED: ON\r\n");
//                 break;
//             case 'B':
//                 gpio_pin_set_dt(&led, 0);
//                 uart_tx_str("LED: OFF\r\n");
//                 break;
//             default:
//                 uart_tx_str("ERROR: Invalid Input!\r\n");
//                 break;
//         }
//     }
// }
