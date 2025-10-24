# Malacate

Código para utilizar la grúa sensor (malacate) de los barcos. Maneja el motor y la comunicación serie con el autopiloto, y graba los logs locales.

Esta versión es compatible con la nueva sonda, con el sensor de conductividad y con el nuevo protocolo de comunicación.

## Instalación y uso

En la Raspberry Pi 4 del barco:

```bash
git clone https://github.com/UCM-237/Malacate.git
cd Malacate
make
sudo ./onda_cuadrada
```
