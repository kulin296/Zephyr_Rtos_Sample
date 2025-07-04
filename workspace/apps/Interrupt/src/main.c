#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define LED_NODE DT_ALIAS(led0)
#define BUTTON_NODE DT_ALIAS(sw0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led);
    printk("Button pressed! LED toggled.\n");
}

int main(void)
{
    if (!device_is_ready(led.port) || !device_is_ready(button.port)) {
        printk("Device not ready\n");
        return;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    printk("Button driver ready. Press the button!\n");

    while (1) {
        k_msleep(1000);
    }
}

// /*Optimize code*/

// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/gpio.h>
// #include <zephyr/logging/log.h>  // ✅ Replaced printk with structured logging

// LOG_MODULE_REGISTER(button_led_demo, LOG_LEVEL_INF);

// #define LED_NODE    DT_ALIAS(led0)
// #define BUTTON_NODE DT_ALIAS(sw0)

// // ✅ Use gpio_dt_spec for clean GPIO abstraction
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
// static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
// static struct gpio_callback button_cb_data;

// // ✅ Interrupt callback for button press
// static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
// {
//     gpio_pin_toggle_dt(&led);
//     LOG_INF("Button pressed! LED toggled.");
// }

// int main(void)
// {
//     // ✅ Check device readiness
//     if (!device_is_ready(led.port) || !device_is_ready(button.port)) {
//         LOG_ERR("LED or Button device not ready");
//         return 1;
//     }

//     // ✅ Configure LED as output (initially off)
//     int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
//     if (ret < 0) {
//         LOG_ERR("Failed to configure LED");
//         return 1;
//     }

//     // ✅ Configure button as input with interrupt
//     ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
//     if (ret < 0) {
//         LOG_ERR("Failed to configure button");
//         return 1;
//     }

//     ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
//     if (ret < 0) {
//         LOG_ERR("Failed to configure button interrupt");
//         return 1;
//     }

//     gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
//     gpio_add_callback(button.port, &button_cb_data);

//     LOG_INF("Button driver ready. Press the button!");

//     while (1) {
//         k_msleep(1000);  // ✅ Idle loop with sleep to reduce CPU usage
//     }

//     return 0;
// }
