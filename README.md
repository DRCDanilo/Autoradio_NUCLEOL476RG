# Autoradio_NUCLEOL476RG
TP de synthèse Autoradio, par David CONTION et Danilo DEL RIO CISNEROS. Novembre 2025.

Ce Travail Pratique a pour but d'utiliser le protocole I2S avec le codec stéréo STGL5000. Le schéma du matériel à utiliser est :

<img width="882" height="672" alt="image" src="https://github.com/user-attachments/assets/3e2c0ef6-4026-4230-8cfe-6cea45072d88" />

## 1. Démarrage
- On a réussi les tests avec la LED D2, l'USART2 et la fonction ```printf```.
- On a fait fonctionner le shell que M. Fiack a fait (https://github.com/lfiack/monsieur_shell):
- Le shell fonctionne dans une tâche en utilisant RTOS. Commit 87fd22d.
- Le shell fonctionne en mode interruption dans une tâche. Commit 8c16768.
- Le sheel fonctionne dans une tâche de FreeRTOS en mode interruption et avec un dirver sous forme de structure. Commit 48ba3e5.

## 2. Le GPIO Expander et le VU-Metre

### 2.1 Configuration 

1. La référence du GPIO Expander est MCP23S17.

2. Sur le STM32 de la carte NUCLE-L476RG le SPI ustilisé est le SPI3 :
    - SPI3_SCK -> PC10.
    - SPI3_MISO -> PC11.
    - SPI3_MOSI -> PB5.
    - nCS -> PB7 (GPIO pour controler à la main).

3. Les paramètres à configurer dans STM32CubeIDE sont: 

    - Activer le SPI3.
    - Configurer les pins :
        - SPI3_SCK -> PC10.
        - SPI3_MISO -> PC11.
        - SPI3_MOSI -> PB5.
        - nCS -> PB7 (GPIO pour controler à la main).
    - Configurer les paramètres :
        - Frame Format : Motorola.
        - Data Size : 8 bits.
        - First Bit : MSB first.

### 2.2 Tests

1. Faire clignoter une ou plusieurs LEDs.

   On a fait clignoter deux leds. Commit f08320f.

   https://github.com/user-attachments/assets/8a6fa138-5392-4ab6-a35d-b5600cbe782d



