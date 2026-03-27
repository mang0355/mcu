#include "stdio.h"
#include <string.h>
#include "stdlib.h"
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"


#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

void mem_callback(const char* args)
{
    uint32_t addr = 0;
    sscanf(args, "%x", &addr);  
    
    if (addr == 0) {
        printf("Ошибка: укажите корректный адрес в hex\n");
        return;
    }
    
    uint32_t value = *((volatile uint32_t*)addr);
    printf("mem 0x%08X: 0x%08X (%u)\n", addr, value, value);
}

void wmem_callback(const char* args)
{
    uint32_t addr = 0;
    uint32_t value = 0;
    sscanf(args, "%x %x", &addr, &value);  
    
    if (addr == 0) {
        printf("Ошибка: укажите корректный адрес в hex\n");
        return;
    }
    
    *((volatile uint32_t*)addr) = value;
    printf("wmem 0x%08X = 0x%08X (%u)\n", addr, value, value);
}

void led_blink_set_period_ms_callback(const char* args)
{
    uint32_t period_ms = 0;
    
    sscanf(args, "%u", &period_ms);
    
    if (period_ms == 0) {
        printf("Ошибка: период должен быть больше 0\n");
        return;
    }
    
    led_task_set_blink_period_ms(period_ms);
    printf("Период мигания установлен: %u мс (%u мкс)\n", 
           period_ms, period_ms * 1000);
}

void version_callback(const char* args)
{
 printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
printf("led_on:\n");
led_task_state_set(LED_STATE_ON);
}

void led_off_callback(const char* args)
{
printf("led_off:\n");
led_task_state_set(LED_STATE_OFF);
}

void led_blink_callback(const char* args)
{
printf("led_blink:\n");
led_task_state_set(LED_STATE_BLINK);
}

void help_callback(const char* args);

api_t device_api[] =
{
 {"help", help_callback, "show all commands"}, 
 {"version", version_callback, "get device name and firmware version"},
 {"on", led_on_callback, "turns led on"},
 {"off", led_off_callback, "turns led off"},
 {"blink", led_blink_callback, "turns led in blink state"},
 {"set_period", led_blink_set_period_ms_callback, "set blink period in ms"}, 
 {"mem", mem_callback, "read memory: mem <addr>"},
 {"wmem", wmem_callback, "write memory: wmem <addr> <value>"}, 
 {NULL, NULL, NULL}
};


void help_callback(const char* args)
{
    printf("Available commands:\n");
    for (int i = 0; device_api[i].command_name != NULL; i++)
    {
        printf("  %s - %s\n", device_api[i].command_name, device_api[i].command_help);
    }
}


int main(){
    stdio_task_init();
    stdio_init_all();
    protocol_task_init(device_api);
    led_task_init();
    while (1)
    {
        led_task_handle();
        char* command = stdio_task_handle();
        
        if (command != NULL)
        {                   
            protocol_task_handle(command);

        }
    }
}



