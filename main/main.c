// Esto es un nuevo cambio al proyecto
#include <stdio.h>
#include <stdbool.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "hal/gpio_types.h"

// Definiciones de los pines para los LEDs y los botones
const int LEDS_RGB [3] = {14, 13, 12}; // ROJO, VERDE, AZUL
const int BUTTONS [2] = {18, 19}; // IZQUIERDA, DERECHA

int estadoColor = 0;
int estadoAnterior = 0; // Para recordar el estado antes de apagar
int push_state [2] = {true, true};
int push_state_old [2] = {true, true};
int ambosPresionados = false;

void actualizar_leds() {
	// Esto es un cambio en la rama develop
	// Nueva modificación
    switch (estadoColor) {
        case 0: // ROJO
            gpio_set_level(LEDS_RGB[0], 0);  // ROJO encendido
            gpio_set_level(LEDS_RGB[1], 1);  // VERDE apagado
            gpio_set_level(LEDS_RGB[2], 1);  // AZUL apagado
            printf("Estado actual: ROJO\n");
            break;
        case 1: // VERDE
            gpio_set_level(LEDS_RGB[0], 1);  // ROJO apagado
            gpio_set_level(LEDS_RGB[1], 0);  // VERDE encendido
            gpio_set_level(LEDS_RGB[2], 1);  // AZUL apagado
            printf("Estado actual: VERDE\n");
            break;
        case 2: // AZUL
            gpio_set_level(LEDS_RGB[0], 1);  // ROJO apagado
            gpio_set_level(LEDS_RGB[1], 1);  // VERDE apagado
            gpio_set_level(LEDS_RGB[2], 0);  // AZUL encendido
            printf("Estado actual: AZUL\n");
            break;
        case 3: // APAGADO (cuando ambos botones están presionados)
            gpio_set_level(LEDS_RGB[0], 1);  // ROJO apagado
            gpio_set_level(LEDS_RGB[1], 1);  // VERDE apagado
            gpio_set_level(LEDS_RGB[2], 1);  // AZUL apagado
            printf("Estado actual: APAGADO\n");
            break;
    }
}

void app_main(void) {
	
    // Configuración salidas
    for (int i = 0; i < (sizeof(LEDS_RGB) / sizeof(int)); i++) {
        gpio_reset_pin(LEDS_RGB[i]);
        gpio_set_direction(LEDS_RGB[i], GPIO_MODE_OUTPUT);
        gpio_set_level(LEDS_RGB[i], true); 
    }
   
    // Configuración entradas
    for (int i = 0; i < (sizeof(BUTTONS) / sizeof(int)); i++) {
        gpio_reset_pin(BUTTONS[i]);
        gpio_set_direction(BUTTONS[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(BUTTONS[i], GPIO_PULLUP_ONLY);
    }

    actualizar_leds();

    while (true) {
        // Guardar estados anteriores
        push_state_old[0] = push_state[0];
        push_state_old[1] = push_state[1];
        
        // Leer estados actuales
        push_state[0] = gpio_get_level(BUTTONS[0]);
        push_state[1] = gpio_get_level(BUTTONS[1]);

        // Detectar si ambos botones están presionados 
        if (push_state[0] == false && push_state[1] == false && 
            (push_state_old[0] == true || push_state_old[1] == true)) {
            estadoAnterior = estadoColor; // Guardar el estado actual
            estadoColor = 3; // Cambiar a estado apagado
            actualizar_leds();
            ambosPresionados = true;
        } 
        // Detectar cuando se suelta al menos un botón después de tener ambos presionados
        else if (ambosPresionados && (push_state[0] == true || push_state[1] == true)) {
            estadoColor = estadoAnterior; // Restaurar el estado anterior
            actualizar_leds();
            ambosPresionados = false;
        }
        // Si no están ambos presionados, manejar los botones individualmente
        else if (!ambosPresionados) {
            // Botón izquierdo 
            if (push_state[0] == false && push_state_old[0] == true) {
                estadoColor = (estadoColor + 1) % 4;
                if (estadoColor == 3) estadoColor = 0; // Saltar estado apagado
                actualizar_leds();
            }

            // Botón derecho 
            if (push_state[1] == false && push_state_old[1] == true) {
                estadoColor = (estadoColor -1 + 4) % 4;
                if (estadoColor == 3) estadoColor = 2; // Saltar estado apagado
                actualizar_leds();
            }
        }

        usleep(10000); 
    }
}