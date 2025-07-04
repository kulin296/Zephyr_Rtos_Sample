#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_DEVICE_NODE DT_NODELABEL(usart2)

const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            uart_fifo_read(dev, &c, 1);
            printk("Received: %c\n", c);
        }
    }
}

void main(void)
{
    if (!device_is_ready(uart_dev)) {
        printk("UART device not found!\n");
        return;
    }

    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);

    printk("UART interrupt example started\n");
}

// /*Optimized code*/
// #include <zephyr/kernel.h>
// #include <zephyr/drivers/uart.h>
// #include <zephyr/sys/printk.h>

// #define UART_NODE DT_NODELABEL(usart2)

// static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

// static void uart_cb(const struct device *dev, void *user_data)
// {
//     uint8_t c;

//     while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
//         if (uart_irq_rx_ready(dev)) {
//             while (uart_fifo_read(dev, &c, 1)) {
//                 printk("Received: %c\n", c);
//             }
//         }
//     }
// }

// void main(void)
// {
//     if (!device_is_ready(uart_dev)) {
//         printk("Error: UART device %s is not ready\n", uart_dev->name);
//         return;
//     }

//     uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
//     uart_irq_rx_enable(uart_dev);

//     printk("UART interrupt example started on %s\n", uart_dev->name);

//     while (1) {
//         k_sleep(K_FOREVER);  // Let the interrupt handler do the work
//     }
// }
