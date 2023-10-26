#include "esp_camera.h"
#include <WiFi.h>  // to access WiFi hardware
// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER  // Has PSRAM

#include "camera_pins.h"





// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "Naredar 2G";            // wifi name
const char* password = "7386110976";        // wifi password

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);  // to see detailed debug messages by setting up some flags
  Serial.println();

//  CAMERA PIN CONFIGURATION
// Creating an object "config" . 'camera_config_t' .it is        
//a structure present in esp_camera.h file  . It contains all the pin names of        
//camera module To store captured information of camera,we are going                    
// to assign camera pins to variables present in 'camera_config_t' structure                
//=======================================================================================
  camera_config_t config;                                                             //
  config.ledc_channel = LEDC_CHANNEL_0;                                               //
  config.ledc_timer = LEDC_TIMER_0;                                                   //
  config.pin_d0 = Y2_GPIO_NUM;                                                        //
  config.pin_d1 = Y3_GPIO_NUM;                                                        //
  config.pin_d2 = Y4_GPIO_NUM;                                                        //
  config.pin_d3 = Y5_GPIO_NUM;                                                        //
  config.pin_d4 = Y6_GPIO_NUM;                                                        //
  config.pin_d5 = Y7_GPIO_NUM;                                                        //
  config.pin_d6 = Y8_GPIO_NUM;                                                        //
  config.pin_d7 = Y9_GPIO_NUM;                                                        //
  config.pin_xclk = XCLK_GPIO_NUM;                                                    //
  config.pin_pclk = PCLK_GPIO_NUM;                                                    //
  config.pin_vsync = VSYNC_GPIO_NUM;                                                  //
  config.pin_href = HREF_GPIO_NUM;                                                    //
  config.pin_sccb_sda = SIOD_GPIO_NUM;                                                //
  config.pin_sccb_scl = SIOC_GPIO_NUM;                                                //
  config.pin_pwdn = PWDN_GPIO_NUM;                                                    //  
  config.pin_reset = RESET_GPIO_NUM;                                                  //
  config.xclk_freq_hz = 20000000;           // SPI clock frequency 20MHZ              //
  config.frame_size = FRAMESIZE_UXGA;       // Resolution of video                    //
  config.pixel_format = PIXFORMAT_JPEG;     // for streaming                          //
  config.pixel_format = PIXFORMAT_RGB565;   // for face detection/recognition         //
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;                                          //
  config.fb_location = CAMERA_FB_IN_PSRAM;                                            //
  config.jpeg_quality = 12;  // Lower  the value better jpeg quality(min val upto 0)  //
  config.fb_count = 1;      //Number of frame buffers at a time                       //
//======================================================================================

//======================================================================================
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality           //
  //                      for larger pre-allocated frame buffer.                      //
  if (config.pixel_format == PIXFORMAT_JPEG) {                                        //
    if (psramFound()) {                                                               //
      config.jpeg_quality = 10;                                                       //
      config.fb_count = 2;                                                            //
      config.grab_mode = CAMERA_GRAB_LATEST;                                          //
    } else {                                                                          //
      // Limit the frame size when PSRAM is not available                             //
      config.frame_size = FRAMESIZE_SVGA;                                             //
      config.fb_location = CAMERA_FB_IN_DRAM;                                         //
    }                                                                                 //
  } else {                                                                            //
    // Best option for face detection/recognition                                     //
    config.frame_size = FRAMESIZE_240X240;                                            //
#if CONFIG_IDF_TARGET_ESP32S3                                                         //
    config.fb_count = 2;                                                              //
  #endif                                                                              //
  }                                                                                   //
                                                                                      //
  #if defined(CAMERA_MODEL_ESP_EYE)                                                   //
  pinMode(13, INPUT_PULLUP);                                                          //
  pinMode(14, INPUT_PULLUP);                                                          //
  #endif                                                                              //
  //====================================================================================== 


  // camera initialisation
//==========================================================================================
  esp_err_t err = esp_camera_init(&config);                                               //
  if (err != ESP_OK) {                                                                    //
    Serial.printf("Camera init failed with error 0x%x", err);                             //
    return;                                                                               //
  }            // To check whether my esp32 cam is working or not                //
//=========================================================================================

sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif
  


// WiFi configuration
//=========================================================================================
IPAddress STATIC_IP(192,168,0,108);                                                      //
IPAddress gway(192,168,0,1);                                                             //
IPAddress sNet(255,255,0,0);                                                             //
IPAddress pDNS(8,8,8,8);                                                                 //
IPAddress sDNS(8,8,8,8);                                                                 //
                                                                                         //
if(!WiFi.config(STATIC_IP,gway,sNet,pDNS,sDNS)) //checking whether connection            //
//estableshed or not                                                                     //
{                                                                                        //
  Serial.println("Static IP configuration failed");                                      //
}                                                                                        //
                                                                                         //
//=========================================================================================

// Connecting WiFi to access point (Router)
//=========================================================================================
  WiFi.begin(ssid, password);                                                            //
  WiFi.setSleep(false);                                                                  //
                                                                                         //
  while (WiFi.status() != WL_CONNECTED)                                                  //
  {                                                                                      //
    delay(500);                                                                          //
    Serial.print(".");                                                                   //
  }       // wait until wifi connected                                                   //
  Serial.println("");                                                                    //
  Serial.println("WiFi connected");                                                      //
                                                                                         //
//=========================================================================================

// Starting camera server
//=========================================================================================
                                                                                         //
  startCameraServer();  // Right click and click on 'go to definition'                                                                 //
                                                                                         //
  Serial.print("Camera Ready! Use 'http://");                                            //
  Serial.print(WiFi.localIP());                                                          //
  Serial.println("' to connect");                                                        //
                                                                                         //
//=========================================================================================




  
  
}

void loop() {
  // Do nothing. Everything is done in another task by the web server
  delay(10000);
}

