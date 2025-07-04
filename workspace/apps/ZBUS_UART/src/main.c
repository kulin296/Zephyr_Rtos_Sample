#include <stdint.h>
#include<stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

void uart_tx_str(unsigned char *msg)
{
    for (int i = 0; msg[i] != '\0'; i++) {
        uart_poll_out(uart_dev, msg[i]);
    }
}

unsigned char * uart_rx_str(int w)
{
    int i=0,ret=1,c=0;
    unsigned char *str=malloc(w+1);
    while(c != w)
    {
        while(ret != 0)
            ret=uart_poll_in(uart_dev,&str[i]);
        uart_poll_out(uart_dev,str[i]);
		if(str[i] == '\r' || str[i] == '\n')
			break;
        if(ret==0){
            c++;
            i++;
        }
        ret=1;
    }
    str[i]='\0';
    return str;
}

struct msg{
	char s[5];
};

ZBUS_CHAN_DEFINE(msg_chan,
		struct msg,
		
		NULL,
		NULL,
		ZBUS_OBSERVERS(M1,M2),
		ZBUS_MSG_INIT(0)
);

ZBUS_SUBSCRIBER_DEFINE(M1,4);

void subscriber_task(void *sub)
{
	const struct zbus_channel *chan;

	//zbus_obs_attach_to_thread(&M2);
	while(!zbus_sub_wait(&M1,&chan,K_FOREVER))
	{
		struct msg *msg;
		msg=malloc(sizeof(struct msg));
		uart_tx_str("Enter the message for subscriber: ");
		char *str=uart_rx_str(5);
		strcpy(msg->s,str);

		if (&msg_chan == chan) {
                        // Indirect message access
                        zbus_chan_read(&msg_chan, msg, K_NO_WAIT); //read Message
                        printk("\nFrom subscriber -> Message=%s\n", msg->s);
                }
	}
}

K_THREAD_DEFINE(subscriber_task_id, 512, subscriber_task, NULL, NULL, NULL, 4, 0, 0);

void listner_cb(const struct zbus_channel *chan)
{
	const struct msg *msg;
	//zbus_obs_attach_to_thread(&M1);
	if(&msg_chan == chan)
	{
		msg=zbus_chan_const_msg(chan);
		printk("From Listener: -> Message=%s\n", msg->s);
	}
}

ZBUS_LISTENER_DEFINE(M2,listner_cb);

int main()
{
		struct msg data;
		strcpy(data.s,"hel\n");
		
		// if (zbus_chan_read(&msg_chan, &data, K_NO_WAIT) == 0) {
		// 	printk("DATA: %s\n",data.s);
		// }
		
if (zbus_chan_pub(&msg_chan, &data, K_MSEC(100)) == 0) {
 printk("Message published to msg_chan\n");
} else {
	 printk("Failed to publish message\n");
}


}


// /*Optimized code*/
// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>
// #include <zephyr/kernel.h>
// #include <zephyr/logging/log.h>
// #include <zephyr/zbus/zbus.h>
// #include <zephyr/drivers/uart.h>

// #define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
// LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

// static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

// // UART transmit string
// static void uart_tx_str(const char *msg)
// {
//     while (*msg) {
//         uart_poll_out(uart_dev, *msg++);
//     }
// }

// // UART receive string of fixed length
// static char *uart_rx_str(size_t max_len)
// {
//     char *str = k_malloc(max_len + 1);
//     if (!str) {
//         uart_tx_str("Memory allocation failed!\r\n");
//         return NULL;
//     }

//     size_t i = 0;
//     uint8_t ch;
//     int ret;

//     while (i < max_len) {
//         ret = uart_poll_in(uart_dev, &ch);
//         if (ret == 0) {
//             uart_poll_out(uart_dev, ch);  // Echo
//             if (ch == '\r' || ch == '\n') {
//                 break;
//             }
//             str[i++] = ch;
//         } else {
//             k_msleep(10);  // Avoid busy-waiting
//         }
//     }

//     str[i] = '\0';
//     return str;
// }

// // Message structure
// struct msg {
//     char s[6];  // 5 chars + null terminator
// };

// // Zbus channel
// ZBUS_CHAN_DEFINE(msg_chan,
//     struct msg,
//     NULL, NULL,
//     ZBUS_OBSERVERS(M1, M2),
//     ZBUS_MSG_INIT(.s = "")
// );

// // Subscriber
// ZBUS_SUBSCRIBER_DEFINE(M1, 4);

// // Subscriber thread
// static void subscriber_task(void *arg1, void *arg2, void *arg3)
// {
//     const struct zbus_channel *chan;

//     while (!zbus_sub_wait(&M1, &chan, K_FOREVER)) {
//         if (chan == &msg_chan) {
//             struct msg received;
//             zbus_chan_read(&msg_chan, &received, K_NO_WAIT);
//             printk("\nFrom Subscriber -> Message: %s\n", received.s);
//         }
//     }
// }

// K_THREAD_DEFINE(subscriber_task_id, 512, subscriber_task, NULL, NULL, NULL, 4, 0, 0);

// // Listener callback
// static void listener_cb(const struct zbus_channel *chan)
// {
//     if (chan == &msg_chan) {
//         const struct msg *msg = zbus_chan_const_msg(chan);
//         printk("From Listener -> Message: %s\n", msg->s);
//     }
// }

// ZBUS_LISTENER_DEFINE(M2, listener_cb);

// // Main
// void main(void)
// {
//     if (!device_is_ready(uart_dev)) {
//         printk("UART device not ready\n");
//         return;
//     }

//     uart_tx_str("Enter a 5-character message: ");
//     char *input = uart_rx_str(5);

//     if (input) {
//         struct msg data;
//         strncpy(data.s, input, sizeof(data.s) - 1);
//         data.s[sizeof(data.s) - 1] = '\0';  // Ensure null-termination
//         k_free(input);

//         if (zbus_chan_pub(&msg_chan, &data, K_MSEC(100)) == 0) {
//             printk("Message published to msg_chan\n");
//         } else {
//             printk("Failed to publish message\n");
//         }
//     }
// }
