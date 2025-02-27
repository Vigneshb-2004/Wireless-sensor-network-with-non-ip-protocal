#include <16F877A.h>
#device ADC=10

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES PUT                      //Power Up Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal=4MHz)

#define LCD_ENABLE_PIN PIN_D2
#define LCD_RS_PIN PIN_D0
#define LCD_RW_PIN PIN_D1
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7

#define TRIG_PIN PIN_C0   // Trigger pin for Ultrasonic
#define ECHO_PIN PIN_C1   // Echo pin for Ultrasonic

#include <lcd.c>

// Function prototypes
float get_temperature();
unsigned int get_ultrasonic_distance();

void main() {
    
    float temperature;
    unsigned int distance;
    
    // Set up the ADC to use AN0 (for temperature sensor)
    setup_adc(ADC_CLOCK_INTERNAL);
    setup_adc_ports(AN0);
    
    // Initialize the LCD
    lcd_init();
    
    while (TRUE) {
        // Step 1: Get temperature from LM35
        temperature = get_temperature();
        
        // Step 2: Get distance from ultrasonic sensor
        distance = get_ultrasonic_distance();
        
        // Step 3: Display temperature on the LCD
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Temp: %.1fC", temperature);
        
        // Step 4: Display distance on the LCD
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "Dist: %ucm", distance);
        
        // Step 5: Delay for 1 second before updating again
        delay_ms(1000);
    }
}

// Function to get temperature from LM35 sensor
float get_temperature() {
    long int adc_value;
    float temperature;
    
    // Select channel 0 (AN0) for reading the temperature sensor
    set_adc_channel(0);
    delay_us(20);  // Wait for the ADC to stabilize
    adc_value = read_adc();
    
    // Convert ADC value to temperature
    temperature = (adc_value * 5.0 * 100.0) / 1024.0;
    
    return temperature;
}

// Function to get distance from the ultrasonic sensor
unsigned int get_ultrasonic_distance() {
    unsigned int distance = 0;
    unsigned long duration;
    
    // Send 10us trigger pulse to ultrasonic sensor
    output_low(TRIG_PIN);
    delay_us(2);
    output_high(TRIG_PIN);
    delay_us(10);
    output_low(TRIG_PIN);
    
    // Measure the time for the echo pulse
    while (!input(ECHO_PIN));  // Wait for echo to go high
    set_timer1(0);  // Clear the Timer1
    while (input(ECHO_PIN));  // Wait for echo to go low
    duration = get_timer1();  // Get the echo pulse duration
    
    // Calculate distance in cm (Speed of sound = 343m/s)
    // Time duration is in microseconds, so distance (cm) = (duration / 58)
    distance = duration / 58;
    
    return distance;
}
