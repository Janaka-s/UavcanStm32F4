/*
******************************************************************************
File:     main.cpp
Info:     Generated by Atollic TrueSTUDIO(R) 9.0.0   2019-12-25

The MIT License (MIT)
Copyright (c) 2018 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <uavcan_stm32/uavcan_stm32.hpp>
#include <string>
#include <sstream>

/*
 * We're going to use messages of type uavcan.protocol.debug.KeyValue, so the appropriate header must be included.
 * Given a data type named X, the header file name would be:
 *      X.replace('.', '/') + ".hpp"
 */
#include <uavcan/protocol/debug/KeyValue.hpp> // uavcan.protocol.debug.KeyValue
#include <uavcan/protocol/debug/LogMessage.hpp>
#include <uavcan/helpers/ostream.hpp>

/*
 * This example uses the service type uavcan.protocol.file.BeginFirmwareUpdate.
 */
#include <uavcan/protocol/file/BeginFirmwareUpdate.hpp>

static constexpr int RxQueueSize = 64;
static constexpr std::uint32_t BitRate = 1000000;

#define PUBLISHER 0

void STM_EVAL_CANInit()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  //PD0 (pullup),PD1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // Alternate functions
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);

  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // Alternate functions
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

/**
 * These functions are platform dependent, so they are not included in this example.
 * Refer to the relevant platform documentation to learn how to implement them.
 */
uavcan::ICanDriver& getCanDriver();
uavcan::ISystemClock& getSystemClock();

uavcan::ISystemClock& getSystemClock()
{
    return uavcan_stm32::SystemClock::instance();
}

uavcan::ICanDriver& getCanDriver()
{
    static uavcan_stm32::CanInitHelper<RxQueueSize> can;
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        int res = can.init(BitRate);
        if (res < 0)
        {
            // Handle the error
            printf("\nCan init error");
        }
    }
    return can.driver;
}

/**
 * Memory pool size largely depends on the number of CAN ifaces and on application's logic.
 * Please read the documentation for the class uavcan::Node to learn more.
 */
constexpr unsigned NodeMemoryPoolSize = 16384;

typedef uavcan::Node<NodeMemoryPoolSize> Node;

/**
 * Node object will be constructed at the time of the first access.
 * Note that most library objects are noncopyable (e.g. publishers, subscribers, servers, callers, timers, ...).
 * Attempt to copy a noncopyable object causes compilation failure.
 */
static Node& getNode()
{
    static Node node(getCanDriver(), getSystemClock());
    return node;
}

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
class keyValPublisher
{
  public:
  uavcan::Publisher<uavcan::protocol::debug::KeyValue> kv_pub;

  keyValPublisher(Node &node) :
    kv_pub(node)
  {
    /*
      * Create the publisher object to broadcast standard key-value messages of type uavcan.protocol.debug.KeyValue.
      * Keep in mind that most classes defined in the library are not copyable; attempt to copy objects of
      * such classes will result in compilation failure.
      * A publishing node won't see its own messages.
      */
    
    const int kv_pub_init_res = kv_pub.init();
    if (kv_pub_init_res < 0)
    {
        std::cerr << "Failed to start the publisher; error: " << kv_pub_init_res << std::endl;
    }

    /*
      * TX timeout can be overridden if needed.
      * Default value should be OK for most use cases.
      */
    kv_pub.setTxTimeout(uavcan::MonotonicDuration::fromMSec(1000));


    /*
      * Priority of outgoing tranfers can be changed as follows.
      * Default priority is 16 (medium).
      */
    kv_pub.setPriority(uavcan::TransferPriority::MiddleLower);

    std::cout << "Setting up keyValPair publisher \n";
  };

  bool publishKeyValPair()
  {
      /*
          * Publishing a random value using the publisher created above.
          * All message types have zero-initializing default constructors.
          * Relevant usage info for every data type is provided in its DSDL definition.
          */
          uavcan::protocol::debug::KeyValue kv_msg;  // Always zero initialized
          kv_msg.value = std::rand() / float(RAND_MAX);

          /*
          * Arrays in DSDL types are quite extensive in the sense that they can be static,
          * or dynamic (no heap needed - all memory is pre-allocated), or they can emulate std::string.
          * The last one is called string-like arrays.
          * ASCII strings can be directly assigned or appended to string-like arrays.
          * For more info, please read the documentation for the class uavcan::Array<>.
          */
          kv_msg.key = "a";   // "a"
          kv_msg.key += "b";  // "ab"
          kv_msg.key += "c";  // "abc"

          /*
          * Publishing the message.
          */
          const int pub_res = kv_pub.broadcast(kv_msg);
          if (pub_res < 0)
          {
              std::cerr << "KV publication failure: " << pub_res << std::endl;
          }
          return true;
  };
};

class logAndKeyValSubscriber
{
  public:
  uavcan::Subscriber<uavcan::protocol::debug::LogMessage> log_sub;
  uavcan::Subscriber<uavcan::protocol::debug::KeyValue> kv_sub;
  uint64_t m_counter;

  logAndKeyValSubscriber( Node &node):
  log_sub(node),
  kv_sub(node),
  m_counter(0)
  {};

  void subscribeLog()
  {    
    /*
     * Subscribing to standard log messages of type uavcan.protocol.debug.LogMessage.
     *
     * Received messages will be passed to the application via a callback, the type of which can be set via the second
     * template argument.
     * In C++11 mode, callback type defaults to std::function<>.
     * In C++03 mode, callback type defaults to a plain function pointer; use a binder object to call member
     * functions as callbacks (refer to uavcan::MethodBinder<>).
     *
     * N.B.: Some libuavcan users report that C++ lambda functions when used with GCC may actually break the code
     *       on some embedded targets, particularly ARM Cortex M0. These reports still remain unconfirmed though;
     *       please refer to the UAVCAN mailing list to learn more.
     *
     * The type of the argument of the callback can be either of these two:
     *  - T&
     *  - uavcan::ReceivedDataStructure<T>&
     * For the first option, ReceivedDataStructure<T>& will be cast into a T& implicitly.
     *
     * The class uavcan::ReceivedDataStructure extends the received data structure with extra information obtained from
     * the transport layer, such as Source Node ID, timestamps, Transfer ID, index of the redundant interface this
     * transfer was picked up from, etc.
     */

    const int log_sub_start_res = log_sub.start(
        [&](const uavcan::ReceivedDataStructure<uavcan::protocol::debug::LogMessage>& msg)
        {
            /*
             * The message will be streamed in YAML format.
             */
            //std::cout << msg << std::endl;
            /*
             * If the standard iostreams are not available (they rarely available in embedded environments),
             * use the helper class uavcan::OStream defined in the header file <uavcan/helpers/ostream.hpp>.
             */
             //uavcan::OStream::instance() << msg << uavcan::OStream::endl;
             std::cout << "Node:" << msg.getSrcNodeID().get() << ", Text:" << msg.text.c_str() << std::endl;
        });
      /*
      * C++03 WARNING
      * The code above will not compile in C++03, because it uses a lambda function.
      * In order to compile the code in C++03, move the code from the lambda to a standalone static function.
      * Use uavcan::MethodBinder<> to invoke member functions.
      */

      if (log_sub_start_res < 0)
      {
          std::cerr <<"Failed to start the log subscriber; error: " << log_sub_start_res;
      }
      else
      {
        std::cout <<"Subscribing to Logs\n";
      }
  };

    /*
     * Subscribing to messages of type uavcan.protocol.debug.KeyValue.
     * This time we don't want to receive extra information about the received message, so the callback's argument type
     * would be just T& instead of uavcan::ReceivedDataStructure<T>&.
     * The callback will print the message in YAML format via std::cout (also refer to uavcan::OStream).
     */
  void keyValSubscribe()
  {
    const int kv_sub_start_res =
        kv_sub.start([&](const uavcan::protocol::debug::KeyValue& msg) 
        { 
          //std::cout << msg << std::endl; 
          //uavcan::OStream::instance() << msg << uavcan::OStream::endl;
          std::cout << "Key:"<< msg.key.c_str() << ", Val:" << msg.value << ", Count:" << m_counter++ << "\r";
          STM_EVAL_LEDToggle(LED_TX);
        });

    if (kv_sub_start_res < 0)
    {
        std::cerr <<"Failed to start the key/value subscriber; error: " << kv_sub_start_res;
    }
    else
    {
      std::cout <<"Subscribing to keyValPair\n";
    }
  };
};

using uavcan::protocol::file::BeginFirmwareUpdate;
class FWUpdateService
{
  /*
    * Starting the server.
    * This server doesn't do anything useful; it just prints the received request and returns some meaningless
    * response.
    *
    * The service callback accepts two arguments:
    *  - a reference to a request structure (input)
    *  - a reference to a default-initialized response structure (output)
    * The type of the input can be either of these two:
    *  - T::Request&
    *  - uavcan::ReceivedDataStructure<T::Request>&
    * The type of the output is strictly T::Response&.
    * Note that for the service data structure, it is not possible to instantiate T itself, nor does it make any
    * sense.
    *
    * In C++11 mode, callback type defaults to std::function<>.
    * In C++03 mode, callback type defaults to a plain function pointer; use a binder object to call member
    * functions as callbacks (refer to uavcan::MethodBinder<>).
    */
    uavcan::ServiceServer<BeginFirmwareUpdate> srv;
    uint64_t m_counter;

    public:
    FWUpdateService( Node &node ):
    srv(node),
    m_counter(0) 
    {
    };

    void startService() 
    {
      const int srv_start_res = srv.start(
      [&](const uavcan::ReceivedDataStructure<BeginFirmwareUpdate::Request>& req, BeginFirmwareUpdate::Response& rsp)
      {
          std::cout << req << std::endl;
          rsp.error = rsp.ERROR_UNKNOWN;
          // std::string s = "Some Error Msg [" + patch::to_string(m_counter) + "]\n";
          // rsp.optional_error_message = s.c_str();
          rsp.optional_error_message = req.image_file_remote_path.path.c_str();
          STM_EVAL_LEDToggle(LED_CAN_ERR);
          std::cout << "FWUpdate msg, Counter:"<< m_counter++ << std::endl;
      });
      /*
      * C++03 WARNING
      * The code above will not compile in C++03, because it uses a lambda function.
      * In order to compile the code in C++03, move the code from the lambda to a standalone static function.
      * Use uavcan::MethodBinder<> to invoke member functions.
      */

      if (srv_start_res < 0)
      {
          std::exit(1);                   // TODO proper error handling
      }
      else
      {        
        std::cout<<"Starting FWUpdate service"<<std::endl;
      }
    };    
};


using uavcan::protocol::file::BeginFirmwareUpdate;
class FWUpdateClient
{  
  uavcan::ServiceClient<BeginFirmwareUpdate> client;

  public:
  FWUpdateClient( Node &node ):
  client(node)
  {
    std::cout<<"Instantiating FWUpdateClient\n";
  };

  void InitClient()
  {  /*
     * Initializing the client. Remember that client objects are noncopyable.
     * Note that calling client.init() is not necessary - the object can be initialized ad hoc during the first call.
     */
    const int client_init_res = client.init();
    if (client_init_res < 0)
    {
        std::cerr <<"Failed to init the client; error: " << patch::to_string(client_init_res);
        return;
    }

    /*
     * Setting the callback.
     * This callback will be executed when the call is completed.
     * Note that the callback will ALWAYS be called even if the service call has timed out; this guarantee
     * allows to simplify error handling in the application.
     */
    client.setCallback([](const uavcan::ServiceCallResult<BeginFirmwareUpdate>& call_result)
        {
            if (call_result.isSuccessful())  // Whether the call was successful, i.e. whether the response was received
            {
                // The result can be directly streamed; the output will be formatted in human-readable YAML.
                //std::cout << call_result << std::endl;
                std::cout << call_result.getResponse().optional_error_message.c_str() << std::endl;
                STM_EVAL_LEDToggle(LED_CAN_ERR);
            }
            else
            {
                std::cerr << "Service call to node "
                          << static_cast<int>(call_result.getCallID().server_node_id.get())
                          << " has failed" << std::endl;
            }
        });
    /*
     * C++03 WARNING
     * The code above will not compile in C++03, because it uses a lambda function.
     * In order to compile the code in C++03, move the code from the lambda to a standalone static function.
     * Use uavcan::MethodBinder<> to invoke member functions.
     */

    /*
     * Service call timeout can be overridden if needed, though it's not recommended.
     */
    client.setRequestTimeout(uavcan::MonotonicDuration::fromMSec(200));

    /*
     * It is possible to adjust priority of the outgoing service request transfers.
     * According to the specification, the services are supposed to use the same priority for response transfers.
     * Default priority is medium, which is 16.
     */
    client.setPriority(uavcan::TransferPriority::OneHigherThanLowest);
  };
  void callFWUpdate(uint8_t server_node_id)
    {
      static uint32_t counter=0;
      /*
      * Calling the remote service.
      * Generated service data types have two nested types:
      *   T::Request  - request data structure
      *   T::Response - response data structure
      * For the service data structure, it is not possible to instantiate T itself, nor does it make any sense.
      */
      BeginFirmwareUpdate::Request request;
      //request.image_file_remote_path.path = "/some/path/for/file";
      char s[20];
      snprintf(s, 20, "%08ld", counter++);
      request.image_file_remote_path.path = s;

      /*
      * It is possible to perform multiple concurrent calls using the same client object.
      * The class ServiceClient provides the following methods that allow to control execution of each call:
      *
      *  int call(NodeID server_node_id, const RequestType& request)
      *      Initiate a new non-blocking call.
      *
      *  int call(NodeID server_node_id, const RequestType& request, ServiceCallID& out_call_id)
      *      Initiate a new non-blocking call and return its ServiceCallID descriptor by reference.
      *      The descriptor allows to query the progress of the call or cancel it later.
      *
      *  void cancelCall(ServiceCallID call_id)
      *      Cancel a specific call using its descriptor.
      *
      *  void cancelAllCalls()
      *      Cancel all calls.
      *
      *  bool hasPendingCallToServer(NodeID server_node_id) const
      *      Whether the client object has pending calls to the given server at the moment.
      *
      *  unsigned getNumPendingCalls() const
      *      Returns the total number of pending calls at the moment.
      *
      *  bool hasPendingCalls() const
      *      Whether the client object has any pending calls at the moment.
      */
      const int call_res = client.call(server_node_id, request);
      if (call_res < 0)
      {
          //std::cerr << "Unable to perform service call: " << patch::to_string(call_res);
          std::cout<<"Got response from call\n";
          return;
      }
    };

    bool HasPendingCalls()
    {
      return client.hasPendingCalls();
    };
};

/** TODO:
 * Get alt key working on VSCode            [DONE]
 * Get state machine to remain in ready mode [DONE]
 * Define basic message and transmit        [DONE]
 * Why atolic debugger start delay          [DONE]
 * Why atolic hang when target reset        [DONE: Right click and terminate]
 * Upgrade atolic                           [DONE]
 * Install STM cube with examples           [DONE]
 * Get comms between two nodes going (LEDs to display receival)
 * Define:
 *  Node status
 *  Long binary message                     
 *  Request Response                        [DONE]
 *  Pub sub                                 [DONE]
 * 
 */ 
/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  /* TODO - Add your application code here */

  /* Initialize LEDs */
  printf("Initing Pins\n");
  STM_EVAL_LEDInit(LED_LOOP);
  STM_EVAL_LEDInit(LED_TX);
  STM_EVAL_LEDInit(LED_CAN_ERR);
  STM_EVAL_LEDInit(LED_CAN_OK);
  STM_EVAL_CANInit();
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

  /* Turn LEDs */
  STM_EVAL_LEDOn(LED_LOOP);
  STM_EVAL_LEDOff(LED_TX);
  STM_EVAL_LEDOff(LED_CAN_ERR);
  STM_EVAL_LEDOff(LED_CAN_OK);

  while (!STM_EVAL_PBGetState(BUTTON_USER));

#if (PUBLISHER==1)
  const int self_node_id = 1;
#else
  const int self_node_id = 2;
#endif

  /*
    * Node initialization.
    * Node ID and name are required; otherwise, the node will refuse to start.
    * Version info is optional.
    */
  auto& node = getNode();

  node.setNodeID(self_node_id);

#if (PUBLISHER==1)
  node.setName("org.uavcan.tutorial.test.messages.pub");
#else
  node.setName("org.uavcan.tutorial.test.messages.sub");
#endif

  uavcan::protocol::SoftwareVersion sw_version;  // Standard type uavcan.protocol.SoftwareVersion
  sw_version.major = 1;
  node.setSoftwareVersion(sw_version);

  uavcan::protocol::HardwareVersion hw_version;  // Standard type uavcan.protocol.HardwareVersion
  hw_version.major = 1;
  node.setHardwareVersion(hw_version);

  /*
    * Start the node.
    * All returnable error codes are listed in the header file uavcan/error.hpp.
    */
   /*
     * Dependent objects (e.g. publishers, subscribers, servers, callers, timers, ...) can be initialized only
     * if the node is running. Note that all dependent objects always keep a reference to the node object.
     */
  const int node_start_res = node.start();
  if (node_start_res < 0)
  {
      std::cout<<"Failed to start the node; error: " <<node_start_res;
  }

#if (PUBLISHER==1)
  keyValPublisher kvpub(node);
  FWUpdateService fwUpdate(node);
  fwUpdate.startService();
#else
  //logAndKeyValSubscriber subscriber(node);
  //subscriber.subscribeLog();
  //subscriber.keyValSubscribe();
  FWUpdateClient fwUpdateClient(node);
  fwUpdateClient.InitClient();
  uint32_t counter=0;
#endif
  
  /*
    * Informing other nodes that we're ready to work.
    * Default mode is INITIALIZING.
    */
  node.setModeOperational();

  /*
    * Some logging.
    * Log formatting is not available in C++03 mode.
    */
  node.getLogger().setLevel(uavcan::protocol::debug::LogLevel::DEBUG);
  node.logInfo("main", "Hello world! My Node ID: %*",
                static_cast<int>(node.getNodeID().get()));

  std::cout << "Hello world!" << std::endl;

  /*
    * Node loop.
    * The thread should not block outside Node::spin().
    */
  auto firstRun = true;
  while (true)
  {
    STM_EVAL_LEDToggle(LED_LOOP);
    /*
      * If there's nothing to do, the thread blocks inside the driver's
      * method select() until the timeout expires or an error occurs (e.g. driver failure).
      * All error codes are listed in the header uavcan/error.hpp.
      */
    const int res = node.spin(uavcan::MonotonicDuration::fromMSec(100));
    if (res < 0)
    {
        std::cerr << "Transient failure: " << res << std::endl;
    }

    /*
      * Random status transitions.
      * In real applications, the status code shall reflect node's health.
      */
    if (firstRun)
    {
        node.setHealthOk();
        STM_EVAL_LEDOn(LED_CAN_OK);
        STM_EVAL_LEDOff(LED_CAN_ERR);
        firstRun = false;
    }

  #if (PUBLISHER==1)
    kvpub.publishKeyValPair();
  #else
    if (!fwUpdateClient.HasPendingCalls())
    {
      if (((counter++)%2)==0) //only do it 2nd time around the loop so LEDs look different
        fwUpdateClient.callFWUpdate(1);
    }
  #endif
  }
}



/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
extern "C" void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
extern "C" uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}

