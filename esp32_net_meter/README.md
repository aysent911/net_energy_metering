
# ESP32 Net Enerygy Meter Based on Embedded C
The ESP32 net energy meter uses firmware integration to perform:  
1. Computation of apparent power, power factor, exported, imported and net energies.
2. Communication; receive raw data from UART port and transmit computed parameters via MQTT to the API.  

## Usage

### Configure

```
idf.py menuconfig
```

### Build and Flash

```
idf.py -p PORT flash monitor
```

Type ``Ctrl-]`` to exit monitor.
