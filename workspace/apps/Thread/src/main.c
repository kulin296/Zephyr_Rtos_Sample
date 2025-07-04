#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// Sleep settings
static const int32_t blink_sleep_ms = 500;
static const int32_t print_sleep_ms = 700;

// Stack size settings
#define BLINK_THREAD_STACK_SIZE 256

// Define stack areas for the threads
K_THREAD_STACK_DEFINE(blink_stack, BLINK_THREAD_STACK_SIZE);

// Declare thread data structs
static struct k_thread blink_thread;

// Get LED struct from Devicetree
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

// Blink thread entry point
void blink_thread_start(void *arg_1, void *arg_2, void *arg_3)
{
    int ret;
    int state = 0;

    while (1) {

        // Change the state of the pin
        state = !state;

        // Set pin state
        ret = gpio_pin_set_dt(&led, state);
        if (ret < 0) {
            printk("Error: could not toggle pin\r\n");
        }

        k_msleep(blink_sleep_ms);
    }
}

int main(void)
{
    int ret;
    k_tid_t blink_tid;

    // Make sure that the GPIO was initialized
    if (!gpio_is_ready_dt(&led)) {
        printk("Error: GPIO pin not ready\r\n");
        return 0;
    }

    // Set the GPIO as output
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
    if (ret < 0) {
        printk("Error: Could not configure GPIO\r\n");
        return 0;
    }

    // Start the blink thread
    blink_tid = k_thread_create(&blink_thread,          // Thread struct
                                blink_stack,            // Stack
                                K_THREAD_STACK_SIZEOF(blink_stack),
                                blink_thread_start,     // Entry point
                                NULL,                   // arg_1
                                NULL,                   // arg_2
                                NULL,                   // arg_3
                                7,                      // Priority
                                0,                      // Options
                                K_NO_WAIT);             // Delay

    // Do forever
    while (1) {
        printk("Hello\r\n");
        k_msleep(print_sleep_ms);
    }

    return 0;
}

// /*Optimized Code*/
// #include <zephyr/kernel.h>
// #include <zephyr/drivers/gpio.h>
// #include <zephyr/sys/printk.h>

// // Constants
// #define BLINK_THREAD_STACK_SIZE 256
// #define BLINK_THREAD_PRIORITY 7

// #define BLINK_SLEEP_MS 500
// #define PRINT_SLEEP_MS 700

// // Thread stack and data
// K_THREAD_STACK_DEFINE(blink_stack, BLINK_THREAD_STACK_SIZE);
// static struct k_thread blink_thread_data;

// // LED device from devicetree
// const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

// // Blink thread function
// void blink_thread(void *arg1, void *arg2, void *arg3)
// {
//     int state = 0;

//     while (1) {
//         state = !state;

//         if (gpio_pin_set_dt(&led, state) < 0) {
//             printk("Error: Failed to toggle LED\n");
//         }

//         k_msleep(BLINK_SLEEP_MS);
//     }
// }

// void main(void)
// {
//     if (!gpio_is_ready_dt(&led)) {
//         printk("Error: LED GPIO device not ready\n");
//         return;
//     }

//     if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
//         printk("Error: Failed to configure LED GPIO\n");
//         return;
//     }

//     // Start blink thread
//     k_thread_create(&blink_thread_data, blink_stack,
//                     K_THREAD_STACK_SIZEOF(blink_stack),
//                     blink_thread, NULL, NULL, NULL,
//                     BLINK_THREAD_PRIORITY, 0, K_NO_WAIT);

//     // Main thread prints periodically
//     while (1) {
//         printk("Hello\n");
//         k_msleep(PRINT_SLEEP_MS);
//     }
// }
