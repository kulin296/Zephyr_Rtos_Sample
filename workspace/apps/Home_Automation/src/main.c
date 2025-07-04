#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_DEVICE_NODE DT_NODELABEL(usart2)

unsigned int flag;
unsigned char c;

static const int32_t sleep_time_ms = 1000;
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static void uart_cb(const struct device *dev, void *user_data)
{

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            uart_fifo_read(dev, &c, 1);
            //printk("Received: %c\n", c);
            flag=1;
        }
    }
}

int main(void)
{
	int ret;
	int state = 0;


    if (!device_is_ready(uart_dev)) {
        printk("UART device not found!\n");
        return;
    }

    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);

	// Make sure that the GPIO was initialized
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	// Set the GPIO as output
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}
    
    printk("MENU:\nA:Fan On\nB:Fan Off\nC:TV On\nD:TV Off\nE:ALL On\nF:ALL Off\nEnter OPtion: ");
	// Do forever
	while (1) {

		// Change the state of the pin and print
		state = !state;
		// printk("LED state: %d\r\n", state);
		
		// Set pin state
		ret = gpio_pin_set_dt(&led, state);
		if (ret < 0) {
			return 0;
		}
        
        if(flag==1)
        {
            flag=0;
            printk("%c\n",c);
            switch(c)
            {
                case 'A': printk("Fan On!\n"); break;
                case 'B': printk("Fan off!\n"); break;
                case 'C': printk("AC On!\n"); break;
                case 'D': printk("AC Off!\n"); break;
                case 'E': printk("ALL On!\n"); break;
                case 'F': printk("ALL Off!\n"); break;
                default: printk("INVALID!\n");
            }
            printk("MENU:\nA:Fan On\nB:Fan Off\nC:TV On\nD:TV Off\nE:ALL On\nF:ALL Off\nEnter OPtion: ");
        }
		// Sleep
		k_msleep(sleep_time_ms);
	}

	return 0;
}

// /*Optimized code*/
// #include <zephyr/kernel.h>
// #include <zephyr/drivers/gpio.h>
// #include <zephyr/drivers/uart.h>
// #include <zephyr/logging/log.h>  // ✅ Replaced printk with structured logging

// LOG_MODULE_REGISTER(uart_gpio_control, LOG_LEVEL_INF);  // ✅ Enables log filtering and levels

// #define UART_DEVICE_NODE DT_NODELABEL(usart2)
// #define SLEEP_TIME_MS    1000

// // ✅ Used GPIO_DT_SPEC_GET for cleaner GPIO config
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
// static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

// static volatile bool flag = false;  // ✅ Used `volatile` for ISR-safe flag
// static unsigned char c;

// // ✅ ISR-based UART receive handler
// static void uart_cb(const struct device *dev, void *user_data)
// {
//     while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
//         if (uart_irq_rx_ready(dev)) {
//             uart_fifo_read(dev, &c, 1);
//             flag = true;
//         }
//     }
// }

// // ✅ Modularized menu printing
// void print_menu(void)
// {
//     LOG_INF("MENU:\nA: Fan On\nB: Fan Off\nC: TV On\nD: TV Off\nE: ALL On\nF: ALL Off\nEnter Option:");
// }

// int main(void)
// {
//     int ret;
//     bool led_state = false;

//     // ✅ Device readiness check
//     if (!device_is_ready(uart_dev)) {
//         LOG_ERR("UART device not ready!");
//         return 1;
//     }

//     uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
//     uart_irq_rx_enable(uart_dev);

//     if (!gpio_is_ready_dt(&led)) {
//         LOG_ERR("LED GPIO not ready!");
//         return 1;
//     }

//     ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
//     if (ret < 0) {
//         LOG_ERR("Failed to configure LED GPIO!");
//         return 1;
//     }

//     print_menu();  // ✅ Cleaner menu display

//     while (1) {
//         // ✅ LED toggling logic
//         led_state = !led_state;
//         gpio_pin_set_dt(&led, led_state);

//         // ✅ UART command handling
//         if (flag) {
//             flag = false;
//             LOG_INF("Received: %c", c);

//             switch (c) {
//                 case 'A': LOG_INF("Fan On!"); break;
//                 case 'B': LOG_INF("Fan Off!"); break;
//                 case 'C': LOG_INF("TV On!"); break;
//                 case 'D': LOG_INF("TV Off!"); break;
//                 case 'E': LOG_INF("ALL On!"); break;
//                 case 'F': LOG_INF("ALL Off!"); break;
//                 default:  LOG_WRN("Invalid Option!");  // ✅ Clear feedback for invalid input
//             }

//             print_menu();  // ✅ Reprint menu after each command
//         }

//         k_msleep(SLEEP_TIME_MS);  // ✅ Consistent sleep interval
//     }

//     return 0;
// }
