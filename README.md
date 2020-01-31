# miku-smc
System Management Controller Firmware For Loongson-3A4000.

Based on RT-Thread RTOS with various reverse engineering effort.

## 功能
### Sensors
传感器组件，从片上设施以及外设采样传感器信息供主核软件读取。
### Fan Control
风扇控制组件，根据采样的温度信息调整风扇转速。
### Dynamic voltage and frequency scaling
根据主核软件需求以及芯片状况动态调整电压与频率，包括睿频(Boost)机制。
