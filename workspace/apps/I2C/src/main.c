#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
//#include <zephyr/drivers/uart.h>

LOG_MODULE_REGISTER(main);
// #define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
#define I2C_NODE DT_NODELABEL(i2c1)
// const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
// void uart_tx_str(unsigned char *msg)
// {
//     for (int i = 0; msg[i] != '\0'; i++) {
//         uart_poll_out(uart_dev, msg[i]);
//     }
// }
void main(void)
{
    
    const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev)) {
        LOG_ERR("I2C device not ready");
        // uart_tx_str("I2C device not ready");
        return;
    }

    uint8_t tx_buf[2] = {0x00, 0xAA};
    uint8_t rx_buf[1];

    if (i2c_write(i2c_dev, tx_buf, sizeof(tx_buf), 0x50) != 0) {
        LOG_ERR("I2C write failed");
        // uart_tx_str("I2C write failed\n");
    } else {
        LOG_INF("I2C write successful");
        // uart_tx_str("I2C write successful\n");
    }

    k_msleep(10);

    if (i2c_read(i2c_dev, rx_buf, sizeof(rx_buf), 0x50) != 0) {
        LOG_ERR("I2C read failed");
        // uart_tx_str("I2C read failed\n");
    } else {
        LOG_INF("I2C read: 0x%02x", rx_buf[0]);
        // uart_tx_str("I2C read:\n");
    }
}

// /*Optimize Code*/
// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/i2c.h>
// #include <zephyr/logging/log.h>

// LOG_MODULE_REGISTER(i2c_demo, LOG_LEVEL_INF);  // ✅ Clearer module name and log level

// #define I2C_NODE DT_NODELABEL(i2c1)
// #define I2C_ADDR 0x50  // ✅ Defined I2C address as a macro for clarity

// void main(void)
// {
//     const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

//     // ✅ Check if I2C device is ready
//     if (!device_is_ready(i2c_dev)) {
//         LOG_ERR("I2C device not ready");
//         return;
//     }

//     // ✅ Use const for buffers that don't change
//     const uint8_t tx_buf[] = {0x00, 0xAA};
//     uint8_t rx_buf[1] = {0};

//     // ✅ Write to I2C device
//     int ret = i2c_write(i2c_dev, tx_buf, sizeof(tx_buf), I2C_ADDR);
//     if (ret != 0) {
//         LOG_ERR("I2C write failed (err %d)", ret);
//     } else {
//         LOG_INF("I2C write successful");
//     }

//     k_msleep(10);  // ✅ Small delay before reading

//     // ✅ Read from I2C device
//     ret = i2c_read(i2c_dev, rx_buf, sizeof(rx_buf), I2C_ADDR);
//     if (ret != 0) {
//         LOG_ERR("I2C read failed (err %d)", ret);
//     } else {
//         LOG_INF("I2C read: 0x%02X", rx_buf[0]);
//     }
// }
