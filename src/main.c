#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define TEMPO_DE_ESPERA_MS 1

#define BOTAO_GPIO_NODE DT_NODELABEL(gpioc)
#define BOTAO_PIN       13

// setando os pinos
static const struct gpio_dt_spec pinos_display[] = {
    {DEVICE_DT_GET(DT_NODELABEL(gpioa)), 0, GPIO_OUTPUT},
    {DEVICE_DT_GET(DT_NODELABEL(gpioa)), 1, GPIO_OUTPUT},
    {DEVICE_DT_GET(DT_NODELABEL(gpioa)), 4, GPIO_OUTPUT},
    {DEVICE_DT_GET(DT_NODELABEL(gpiob)), 0, GPIO_OUTPUT},
    {DEVICE_DT_GET(DT_NODELABEL(gpiob)), 4, GPIO_OUTPUT},
    {DEVICE_DT_GET(DT_NODELABEL(gpiob)), 10, GPIO_OUTPUT},
    {DEVICE_DT_GET(DT_NODELABEL(gpioa)), 8, GPIO_OUTPUT},
};

// def do pino do botao
static const struct gpio_dt_spec botao = {
    DEVICE_DT_GET(BOTAO_GPIO_NODE),
    BOTAO_PIN,
    GPIO_INPUT,
};

// tabela displ
static const uint8_t valores_segmentos[16][7] = {
    {1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1}, {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 1}, {1, 1, 1, 0, 1, 1, 1}, {0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 1, 1, 0}, {0, 1, 1, 1, 1, 0, 1}, {1, 0, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 1}
};

// pra exibir o num

void exibir_numero(int numero) {
    for (int i = 0; i < 7; i++) {
        gpio_pin_set(pinos_display[i].port, pinos_display[i].pin, valores_segmentos[numero][i]);
    }
}


int main(void) {
    int ret, contador = 0;
    bool botao_pressionado = false;

    // conf dos pino
    for (int i = 0; i < 7; i++) {
        if (!device_is_ready(pinos_display[i].port)) return 0;
        ret = gpio_pin_configure(pinos_display[i].port, pinos_display[i].pin, GPIO_OUTPUT_ACTIVE);
        if (ret != 0) return 0;
    }

    // pino e botao
    if (!device_is_ready(botao.port)) return 0;
    ret = gpio_pin_configure(botao.port, botao.pin, GPIO_INPUT);
    if (ret != 0) return 0;

    while (1) {
        int estado_botao = gpio_pin_get(botao.port, botao.pin);
        if (!estado_botao && !botao_pressionado) {
            contador = (contador + 1) % 16;
            botao_pressionado = true;
        } else if (estado_botao && botao_pressionado) {
            botao_pressionado = false;
        }
        exibir_numero(contador);
        k_msleep(TEMPO_DE_ESPERA_MS);
    }

    return 0;
}