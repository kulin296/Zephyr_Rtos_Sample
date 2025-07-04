#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
// #include <zephyr/logging/log.h>

// LOG_MODULE_REGISTER(spi_example);

#define SPI_NODE DT_NODELABEL(spi1)

static const struct spi_config spi_cfg = {
    .frequency = 1000000U,
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .slave = 0,
    .cs = NULL,
};

void main(void)
{
    const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE);

    if (!device_is_ready(spi_dev)) {
        printk("SPI device not ready");
        return;
    }

    uint8_t tx_buf[] = { 0x9A, 0xBC };
    uint8_t rx_buf[sizeof(tx_buf)] = { 0 };

    const struct spi_buf tx_spi_buf = {
        .buf = tx_buf,
        .len = sizeof(tx_buf),
    };
    const struct spi_buf_set tx_bufs = {
        .buffers = &tx_spi_buf,
        .count = 1,
    };

    struct spi_buf rx_spi_buf = {
        .buf = rx_buf,
        .len = sizeof(rx_buf),
    };
    const struct spi_buf_set rx_bufs = {
        .buffers = &rx_spi_buf,
        .count = 1,
    };

    int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
    if (ret == 0) {
        printk("SPI transfer successful. Received: 0x%X 0x%X", rx_buf[0], rx_buf[1]);
    } else {
        printk("SPI transfer failed: %d", ret);
    }
}


// /*Optimize code*/
// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/spi.h>
// #include <zephyr/sys/printk.h>
// // #include <zephyr/logging/log.h>
// // LOG_MODULE_REGISTER(spi_example, LOG_LEVEL_INF);

// #define SPI_NODE DT_NODELABEL(spi1)

// static const struct spi_config spi_cfg = {
//     .frequency = 1000000U,
//     .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
//     .slave = 0,
//     .cs = NULL,
// };

// void main(void)
// {
//     const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE);

//     if (!device_is_ready(spi_dev)) {
//         printk("Error: SPI device %s is not ready\n", spi_dev->name);
//         return;
//     }

//     const uint8_t tx_buf[] = { 0x9A, 0xBC };
//     uint8_t rx_buf[sizeof(tx_buf)] = { 0 };

//     const struct spi_buf tx_spi_buf = {
//         .buf = (void *)tx_buf,
//         .len = sizeof(tx_buf),
//     };
//     const struct spi_buf_set tx_bufs = {
//         .buffers = &tx_spi_buf,
//         .count = 1,
//     };

//     struct spi_buf rx_spi_buf = {
//         .buf = rx_buf,
//         .len = sizeof(rx_buf),
//     };
//     const struct spi_buf_set rx_bufs = {
//         .buffers = &rx_spi_buf,
//         .count = 1,
//     };

//     int ret = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
//     if (ret == 0) {
//         printk("SPI transfer successful. Received: 0x%02X 0x%02X\n", rx_buf[0], rx_buf[1]);
//     } else {
//         printk("SPI transfer failed with error code: %d\n", ret);
//     }
// }
