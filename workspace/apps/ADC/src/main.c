#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>

// Register a log module for this application
LOG_MODULE_REGISTER(adc_demo_revised, LOG_LEVEL_INF);

// --- Configuration Defines ---
// Use the ADC node defined in the devicetree (see nucleo_f446re.overlay)
#define ADC_NODE DT_NODELABEL(adc1)

// Define the ADC channel to use.
// This typically corresponds to an analog pin on your board.
// For Nucleo-F446RE, ADC1_IN0 is usually PA0.
#define ADC_CHANNEL_ID 0

#define ADC_RESOLUTION  12      // 12-bit resolution
#define ADC_GAIN        ADC_GAIN_1 // No gain
#define ADC_REFERENCE   ADC_REF_INTERNAL // Internal voltage reference
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT // Default acquisition time

#define BUFFER_SIZE     1       // Single sample buffer
static int16_t adc_raw_value;   // Buffer to store the raw ADC reading

// ADC device pointer
static const struct device *adc_dev;

// ADC channel configuration
static const struct adc_channel_cfg channel_cfg = {
    .gain             = ADC_GAIN,
    .reference        = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id       = ADC_CHANNEL_ID,
    .differential     = 0, // Single-ended mode
};

// ADC sequence configuration
static struct adc_sequence sequence = {
    .channels    = BIT(ADC_CHANNEL_ID),
    .buffer      = &adc_raw_value,
    .buffer_size = sizeof(adc_raw_value),
    .resolution  = ADC_RESOLUTION,
};
int map(int n,int in_min,int in_max,int op_min,int op_max)
{
    int op;
    op=(((n-in_min)*(op_max-op_min)) / (in_max-in_min)) + op_min;
    return op;
}
void main(void)
{
    //unsigned int op;
    // 1. Get the ADC device from the devicetree
    adc_dev = DEVICE_DT_GET(ADC_NODE);
    if (!device_is_ready(adc_dev)) {
        printk("ADC device not ready! Check devicetree configuration.");
        return;
    }
    printk("ADC device %s found and ready.\n", adc_dev->name);

    // 2. Setup the ADC channel
    int err = adc_channel_setup(adc_dev, &channel_cfg);
    if (err) {
        printk("ADC channel setup failed (err %d) for channel %d!\n", err, ADC_CHANNEL_ID);
        return;
    }
    printk("ADC channel %d setup successful.\n", ADC_CHANNEL_ID);

    // 3. Main loop for continuous ADC readings
    while (1) {
        err = adc_read(adc_dev, &sequence);
        if (err) {
            printk("ADC read failed (err %d)!", err);
        } else {
            //op=map(adc_raw_value,0,4095,0,100);
            // Raw ADC value is available in adc_raw_value
            printk("ADC_1 Channel %d raw value: %d\n", ADC_CHANNEL_ID, adc_raw_value);
            //printk("ADC_1 Channel %d raw value: %d\n", ADC_CHANNEL_ID, op);

            // If you want to convert to voltage (assuming 3.3V reference and 12-bit resolution)
            //  int voltage = (float)adc_raw_value * 3.3f / (1 << ADC_RESOLUTION);
            //  printk("ADC_1 Channel %d voltage: %d V\n\n", ADC_CHANNEL_ID, voltage);
        }

        k_sleep(K_MSEC(1000)); // Read every 1 second
    }
}

// /* OPtimize Code*/

// #include <zephyr/kernel.h>
// #include <zephyr/device.h>
// #include <zephyr/drivers/adc.h>
// #include <zephyr/logging/log.h>

// LOG_MODULE_REGISTER(adc_demo_revised, LOG_LEVEL_INF);

// // --- Configuration ---
// #define ADC_NODE DT_NODELABEL(adc1)
// #define ADC_CHANNEL_ID 0
// #define ADC_RESOLUTION 12
// #define ADC_GAIN ADC_GAIN_1
// #define ADC_REFERENCE ADC_REF_INTERNAL
// #define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT
// #define BUFFER_SIZE 1

// static int16_t adc_raw_value;
// static const struct device *adc_dev;

// static const struct adc_channel_cfg channel_cfg = {
//  .gain = ADC_GAIN,
//  .reference = ADC_REFERENCE,
//  .acquisition_time = ADC_ACQUISITION_TIME,
//  .channel_id = ADC_CHANNEL_ID,
//  .differential = 0,
// };

// static struct adc_sequence sequence = {
//  .channels = BIT(ADC_CHANNEL_ID),
//  .buffer = &adc_raw_value,
//  .buffer_size = sizeof(adc_raw_value),
//  .resolution = ADC_RESOLUTION,
// };

// // Utility: Map value from one range to another
// static inline int map(int n, int in_min, int in_max, int out_min, int out_max) {
//  return ((n - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
// }

// void main(void)
// {
//  adc_dev = DEVICE_DT_GET(ADC_NODE);
//  if (!device_is_ready(adc_dev)) {
//  LOG_ERR("ADC device not ready! Check devicetree configuration.");
//  return;
//  }
//  LOG_INF("ADC device %s is ready.", adc_dev->name);

//  int err = adc_channel_setup(adc_dev, &channel_cfg);
//  if (err) {
//  LOG_ERR("ADC channel setup failed (err %d) for channel %d!", err, ADC_CHANNEL_ID);
//  return;
//  }
//  LOG_INF("ADC channel %d setup successful.", ADC_CHANNEL_ID);

//  while (1) {
//  err = adc_read(adc_dev, &sequence);
//  if (err) {
//  LOG_ERR("ADC read failed (err %d)!", err);
//  } else {
//  int percentage = map(adc_raw_value, 0, 4095, 0, 100);
//  LOG_INF("ADC_1 Channel %d raw value: %d (%d%%)", ADC_CHANNEL_ID, adc_raw_value, percentage);
//  }

//  k_sleep(K_MSEC(1000));
//  }
// }
