#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_http_server.h>
#include <ArduinoJson.h>

//Replace with your network credentials
const char* ssid = "Said";
const char* password = "123456789";

// ThingSpeak details
const char* thingspeak_url = "https://api.thingspeak.com/channels/2576310/feeds.json?api_key=UFEIR9SZB1ZVZIGI&results=1";

// Define the camera pins
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM      5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#define LED_PIN_1        2
#define LED_PIN_2        14

httpd_handle_t server = NULL;

static esp_err_t capture_handler(httpd_req_t *req) {
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_send(req, (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return ESP_OK;
}

void start_CameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t capture_uri = {
            .uri       = "/capture",
            .method    = HTTP_GET,
            .handler   = capture_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &capture_uri);
        Serial.println("Camera server started");
    } else {
        Serial.println("Failed to start camera server");
    }
}

void stop_CameraServer() {
    if (server != NULL) {
        httpd_stop(server);
        server = NULL;
        Serial.println("Camera server stopped");
    } else {
        Serial.println("Camera server is not running");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    // Initialize LED pins
    pinMode(LED_PIN_1, OUTPUT);
    pinMode(LED_PIN_2, OUTPUT);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    start_CameraServer();

    Serial.print("Camera Stream Ready! Go to: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/capture");
}

void loop() {
    // Arrêter le serveur après un certain temps ou un événement spécifique
    delay(2000); // Attendre 5 secondes pour cet exemple
    stop_CameraServer();
    Serial.println("Server stopped");

    // Récupérer les données de ThingSpeak
    HTTPClient http;
    http.begin(thingspeak_url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
        // Parse JSON data
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
            int field1_value = doc["feeds"][0]["field1"].as<int>();
            Serial.println(field1_value);
            if (field1_value == 2) {
                digitalWrite(LED_PIN_1, HIGH);
                digitalWrite(LED_PIN_2, LOW);
            } else {
                digitalWrite(LED_PIN_1, LOW);
                digitalWrite(LED_PIN_2, HIGH);
            }
        } else {
            Serial.println("Failed to parse JSON");
        }
    } else {
        Serial.println("Error retrieving data from ThingSpeak");
    }
    http.end();

    start_CameraServer();
    Serial.println("Server started");
}
