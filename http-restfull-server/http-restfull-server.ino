#include <WiFi.h>
#include "mongoose.h"

const char * ssid = "Lii Cafe";
const char * password = "68686868";

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        //Log HTTP Request
        Serial.println("New HTTP Request");
        Serial.write(c->recv.buf,c->recv.len);
        Serial.println();
        //Get HTTP Request Method
        char reqMethod[hm->method.len+1];
        snprintf(reqMethod,hm->method.len+1,"%s",hm->method.ptr);
        //Routing Resources
        if (mg_http_match_uri(hm, "/") && strcmp(reqMethod,"GET") == 0)
        {
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n","Hello World!");
        }
        else if (mg_http_match_uri(hm, "/api/result") && strcmp(reqMethod,"GET") == 0)
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n","{\"result\": 123}");
        }
        else 
        {
            mg_http_reply(c, 400, "", "");
        }
    }
    (void)fn_data;
}

void HTTPServerProcess(void *parameter)
{
    struct mg_mgr mgr; // Event manager
    const char *s_listen_on = "http://0.0.0.0:80";
    mg_mgr_init(&mgr);                           // Initialise event manager
    mg_http_listen(&mgr, s_listen_on, fn, NULL); // Create HTTP listener
                                                 /* Block for 10ms. */
    const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
    for (;;)
    {
        mg_mgr_poll(&mgr, 0); // Infinite event loop
        struct mg_connection *c;
        vTaskDelay(xDelay);
    }
}
void StartHTTPServer(){
    TaskHandle_t HTTPTask;
    xTaskCreateUniversal(
        HTTPServerProcess, /* Function to implement the task */
        "HTTP Server",     /* Name of the task */
        8192 * 2,          /* Stack size in words */
        NULL,              /* Task input parameter */
        3,                 /* Priority of the task */
        &HTTPTask,         /* Task handle. */
        -1);               /* Core where the task should run */
}
void setup()
{
    Serial.begin(115200);
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //Start HTTP Server
    StartHTTPServer();
}

void loop()
{

}
