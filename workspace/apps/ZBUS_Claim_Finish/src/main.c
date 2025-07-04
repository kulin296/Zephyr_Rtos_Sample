#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);


//Define STructure
struct msg{
	char s[20];
	int x;
};

//Define Channel
ZBUS_CHAN_DEFINE(msg_chan,
		struct msg,
		
		NULL,
		NULL,
		ZBUS_OBSERVERS(M1,M2),
		ZBUS_MSG_INIT(.s="KULIN",.x=10)
);

//Define Subscriber
ZBUS_SUBSCRIBER_DEFINE(M1,4);
ZBUS_SUBSCRIBER_DEFINE(M3,4);

//Add Observer
ZBUS_CHAN_ADD_OBS(msg_chan,M3,4);

//Subscriber Task when Subscriber execute 
void subscriber_task(void *sub)
{
	const struct zbus_channel *chan;
	/*For Common Subscriber task for all the subscriber*/
	// const struct zbus_observer *subscriber = sub;
	//  while (!zbus_sub_wait(subscriber, &chan, K_FOREVER)) //Wait until data received  
	//  {
    //             struct msg msg = {.s="FLUKE",.x=50}; //Assign Value to Structure member for Sunscriber

    //             if (&msg_chan == chan) {
    //                     // Indirect message access
    //                     zbus_chan_read(&msg_chan, &msg, K_NO_WAIT); //read Message
    //                     printk("From subscriber -> Name=%s, x=%d\n", msg.s, msg.x);
    //             }
    //  }

	 while (!zbus_sub_wait(&M1, &chan, K_FOREVER)) //Wait until data received  
	 {
                struct msg msg = {.s="FLUKE",.x=50}; //Assign Value to Structure member for Sunscriber

                if (&msg_chan == chan) {
                        // Indirect message access
                        zbus_chan_read(&msg_chan, &msg, K_NO_WAIT); //read Message
                        printk("From subscriber -> Name=%s, x=%d\n", msg.s, msg.x);
                }
     }
}

//For another subscriber 
void subscriber_task1(void)
{
	const struct zbus_channel *chan;
	 while (!zbus_sub_wait(&M3, &chan, K_FOREVER)) {
                struct msg msg = {.s="F",.x=20};

                if (&msg_chan == chan) {
                        // Indirect message access
                        zbus_chan_read(&msg_chan, &msg, K_NO_WAIT);
                        printk("From subscriber -> Name=%s, x=%d\n", msg.s, msg.x);
                }
        }
}

// Create Thread to run Subscriber
K_THREAD_DEFINE(subscriber_task_id, 512, subscriber_task, NULL, NULL, NULL, 3, 0, 0);
K_THREAD_DEFINE(subscriber_task_id1, 512, subscriber_task1, NULL, NULL, NULL, 4, 0, 0);
/*For Multiple subscriber with same task*/
// K_THREAD_DEFINE(subscriber_task_id, 512, subscriber_task, &M1, NULL, NULL, 3, 0, 0);
// K_THREAD_DEFINE(subscriber_task_id1, 512, subscriber_task, &M3, NULL, NULL, 3, 0, 0);


// CallBack Function for Listener 
void listner_cb(const struct zbus_channel *chan)
{
	const struct msg *msg;
	if(&msg_chan == chan)
	{
		msg=zbus_chan_const_msg(chan);
		printk("From Listener: -> Name=%s, x=%d\n", msg->s, msg->x);
	}
}

// Define Listener 
ZBUS_LISTENER_DEFINE(M2,listner_cb);

// Define USer data Call back function
static bool callback(const struct zbus_observer *obs, void *user_data)
{
	printk("HEYYYYY\n");
	return true;
}

int main()
{
	const struct msg *data; // Define the variabe to read data
	struct msg msg1={.s="FHS",.x=100}; // updated value which we update in channel/Structure
	int count=0;
	data = zbus_chan_const_msg(&msg_chan);// Read data from Channel
	printk("From main() -> Name=%s, x=%d\n", data->s, data->x);

	zbus_chan_pub(&msg_chan,&msg1,K_SECONDS(1)); // Update old data with msg1 structure data
    if(!zbus_chan_claim(&msg_chan,K_MSEC(200)))
    {
        struct msg *msg=zbus_chan_msg(&msg_chan);
        //msg->s="HELLLLL";
        strcpy(msg->s,"HELLLLL");
        msg->x=100000000;
      //  zbus_chan_finish(&msg_chan);
    }

	data = zbus_chan_const_msg(&msg_chan);
	printk("From main() -> Name=%s, x=%d\n", data->s, data->x);

	zbus_iterate_over_observers_with_user_data(callback,&count); //call user data function
	return 0;
}

/*Optimized Code*/

// #include <stdint.h>
// #include <zephyr/kernel.h>
// #include <zephyr/logging/log.h>
// #include <zephyr/zbus/zbus.h>

// LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

// // Define message structure
// struct msg {
//     char s[20];
//     int x;
// };

// // Define channel
// ZBUS_CHAN_DEFINE(msg_chan,
//     struct msg,
//     NULL,
//     NULL,
//     ZBUS_OBSERVERS(M1, M2),
//     ZBUS_MSG_INIT(.s = "KULIN", .x = 10)
// );

// // Define subscribers
// ZBUS_SUBSCRIBER_DEFINE(M1, 4);
// ZBUS_SUBSCRIBER_DEFINE(M3, 4);

// // Add observer manually (optional if not in ZBUS_OBSERVERS)
// ZBUS_CHAN_ADD_OBS(msg_chan, M3, 4);

// // Subscriber task for M1
// void subscriber_task(void *arg1, void *arg2, void *arg3)
// {
//     const struct zbus_channel *chan;

//     while (!zbus_sub_wait(&M1, &chan, K_FOREVER)) {
//         if (chan == &msg_chan) {
//             struct msg received;
//             zbus_chan_read(&msg_chan, &received, K_NO_WAIT);
//             printk("M1 Subscriber -> Name: %s, x: %d\n", received.s, received.x);
//         }
//     }
// }

// // Subscriber task for M3
// void subscriber_task1(void *arg1, void *arg2, void *arg3)
// {
//     const struct zbus_channel *chan;

//     while (!zbus_sub_wait(&M3, &chan, K_FOREVER)) {
//         if (chan == &msg_chan) {
//             struct msg received;
//             zbus_chan_read(&msg_chan, &received, K_NO_WAIT);
//             printk("M3 Subscriber -> Name: %s, x: %d\n", received.s, received.x);
//         }
//     }
// }

// // Listener callback
// void listener_cb(const struct zbus_channel *chan)
// {
//     if (chan == &msg_chan) {
//         const struct msg *msg = zbus_chan_const_msg(chan);
//         printk("Listener -> Name: %s, x: %d\n", msg->s, msg->x);
//     }
// }

// // Define listener
// ZBUS_LISTENER_DEFINE(M2, listener_cb);

// // User-defined callback for observer iteration
// static bool user_callback(const struct zbus_observer *obs, void *user_data)
// {
//     printk("User callback triggered for observer\n");
//     (*(int *)user_data)++;
//     return true;
// }

// // Define subscriber threads
// K_THREAD_DEFINE(subscriber_task_id, 512, subscriber_task, NULL, NULL, NULL, 3, 0, 0);
// K_THREAD_DEFINE(subscriber_task_id1, 512, subscriber_task1, NULL, NULL, NULL, 4, 0, 0);

// // Main function
// int main(void)
// {
//     const struct msg *initial_data = zbus_chan_const_msg(&msg_chan);
//     printk("Main -> Initial: Name: %s, x: %d\n", initial_data->s, initial_data->x);

//     struct msg new_msg = {.s = "FHS", .x = 100};
//     zbus_chan_pub(&msg_chan, &new_msg, K_SECONDS(1));

//     if(!zbus_chan_claim(&msg_chan,K_MSEC(200)))
//     {
//         struct msg *msg=zbus_chan_msg(&msg_chan);
//         //msg->s="HELLLLL";
//         strcpy(msg->s,"HELLLLL");
//         msg->x=100000000;
//         zbus_chan_finish(&msg_chan);
//     }

//     const struct msg *updated_data = zbus_chan_const_msg(&msg_chan);
//     printk("Main -> Updated: Name: %s, x: %d\n", updated_data->s, updated_data->x);

//     int count = 0;
//     zbus_iterate_over_observers_with_user_data(user_callback, &count);
//     printk("User callback was called %d times\n", count);

//     return 0;
// }
