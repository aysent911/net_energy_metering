
# Net Energy Metering API
The net energy metering API interfaces the esp32-based net meter with the database:  
1. Subscribes to the MQTT `meter-data` topic for receiving messages from the esp32-based net meters.
2. Parse the meter readings to InfluxDB timeseries database.  

## Usage
### Configure
```
git clone https://github.com/aysent911/net_energy_metering.git
cd net_energy_metering/api
touch .env
```
Set environment variables:
```
INFLUXDB_URL=
INFLUXDB_TOKEN=
INFLUXDB_ORG=
INFLUXDB_BUCKET=
PORT=
MQTT_HOST=
MQTT_USERNAME=
MQTT_PASSWORD=
```

### Install dependencies

```
npm i
```

### Run

```
npm run dev
```
