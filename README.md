# Malacate

Codigo para utilizar la grua sensor (malacate) de los barcos. Maneja el motor y la comunicacion serie con el autopiloto, y graba los logs locales.

Esta rama corresponde a la nueva sonda, con el sensor de conductividad (que tiene un protocolo de comunicación diferente).

## Instalación y uso

En la Raspberry Pi 4 del barco:

```bash
git clone https://github.com/UCM-237/Malacate.git
cd Malacate
make
sudo ./onda_cuadrada
```
