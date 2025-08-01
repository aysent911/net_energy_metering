#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <cjson/cJSON.h>

typedef struct sample_data{
  char *id;
  float timestamp;
  float Pload;
  float Qload;
  float Ppv;
  float Qpv;
  float Pgrid;
  float Qgrid;
}sample_data_t;

typedef struct net_meter_data{
  sample_data_t sample;
  float S;	//grid/net apparent power kVA sqrt()
  float Eexp;	//kWh
  float Eimp;	//kWh
  float E;	//grid/net active energy kWh
  float Ea;	//grid/net apparent energy kVAh
  float pf;	//grid/net power factor pf = real_power/apparent_power
  struct net_meter_data *next;
}net_meter_data_t;

net_meter_data_t *new_meter_data(sample_data_t sample){
  net_meter_data_t *meter_data = (net_meter_data_t *)malloc(sizeof(net_meter_data_t));
  meter_data->sample = sample;
  meter_data->S = sqrtf(pow(sample.Pgrid, (float)2) + pow(sample.Qgrid, (float)2)) / (float)1000;
  meter_data->pf = (float)fabs(sample.Pgrid/ (float)1000 / meter_data->S);
  meter_data->next = NULL;
  return meter_data;
}

net_meter_data_t *append_meter_data(net_meter_data_t *first_sample, sample_data_t sample){
  net_meter_data_t *counter, *new_sample;
  new_sample = new_meter_data(sample);
  for(counter = first_sample; counter->next != NULL; counter = counter->next);
  
  counter->next = new_sample;
  new_sample->E = (sample.Pgrid / (float)1000) * (sample.timestamp - counter->sample.timestamp) / (float)3600;
  new_sample->Eimp = (new_sample->E > 0) ? new_sample->E : 0;
  new_sample->Eexp = (new_sample->E < 0) ? new_sample->E : 0;
  new_sample->Ea = new_sample->S * (sample.timestamp - counter->sample.timestamp) / (float)3600;
  return new_sample;  
}


const char *estimate_energy(const char* JSON_string){
    char *out_string = (char *)malloc(4000);
    cJSON *JSON_data = cJSON_Parse(JSON_string);
    if(JSON_data){
        static cJSON *prosumers;
        prosumers = cJSON_GetObjectItemCaseSensitive(JSON_data, "prosumers");
        if(prosumers){
	    static cJSON *prosumer;
	    cJSON_ArrayForEach(prosumer, prosumers){
	      	net_meter_data_t *first_sample, *last_sample;
	      	cJSON *payload = cJSON_GetObjectItemCaseSensitive(prosumer, "payload");
	      	
	      	static cJSON *payload_element;
	      	uint8_t sample_index = 1;
	      	cJSON_ArrayForEach(payload_element, payload){
	      	    sample_data_t sample;
	      	    cJSON *id = cJSON_GetObjectItemCaseSensitive(payload_element, "id");
	      	    sample.id = cJSON_IsString(id)? id->valuestring: NULL;
	      	    cJSON *timestamp = cJSON_GetObjectItemCaseSensitive(payload_element, "timestamp");
	      	    sample.timestamp = cJSON_IsNumber(timestamp)? (float)timestamp->valuedouble: 0; 
	      	    cJSON *Pload = cJSON_GetObjectItemCaseSensitive(payload_element, "Pload");
	      	    sample.Pload = cJSON_IsNumber(Pload)? (float)Pload->valuedouble: 0; 
	      	    cJSON *Qload = cJSON_GetObjectItemCaseSensitive(payload_element, "Qload");
	      	    sample.Qload = cJSON_IsNumber(Qload)? (float)Qload->valuedouble: 0; 
	      	    cJSON *Ppv = cJSON_GetObjectItemCaseSensitive(payload_element, "Ppv");
	      	    sample.Ppv = cJSON_IsNumber(Ppv)? (float)Ppv->valuedouble: 0; 
	      	    cJSON *Qpv = cJSON_GetObjectItemCaseSensitive(payload_element, "Qpv");
	      	    sample.Qpv = cJSON_IsNumber(Qpv)? (float)Qpv->valuedouble: 0; 
	      	    cJSON *Pgrid = cJSON_GetObjectItemCaseSensitive(payload_element, "Pgrid");
	      	    sample.Pgrid = cJSON_IsNumber(Pgrid)? (float)Pgrid->valuedouble: 0; 
	      	    cJSON *Qgrid = cJSON_GetObjectItemCaseSensitive(payload_element, "Qgrid");
	      	    sample.Qgrid = cJSON_IsNumber(Qgrid)? (float)Qgrid->valuedouble: 0;
	      	  
	      	    if(sample_index == 1){
	      	        first_sample = new_meter_data(sample);
	      	        first_sample->E = (sample.Pgrid / (float)1000) * sample.timestamp / (float)3600;
	      	        first_sample->Eimp = (first_sample->E > 0) ? first_sample->E : 0;
	      	        first_sample->Eexp = (first_sample->E < 0) ? first_sample->E : 0;
	      	        first_sample->Ea = first_sample->S * sample.timestamp/ (float)3600;
	      	        last_sample = first_sample;	      	    
	      	    }else{
	      	        last_sample = append_meter_data(first_sample, sample);
	      	    }
	      	    sample_index++;
	      	    cJSON_AddNumberToObject(payload_element, "S", last_sample->S);
	      	    cJSON_AddNumberToObject(payload_element, "E", last_sample->E);
	      	    cJSON_AddNumberToObject(payload_element, "Eexp", last_sample->Eexp);
	      	    cJSON_AddNumberToObject(payload_element, "Eimp", last_sample->Eimp);
	      	    cJSON_AddNumberToObject(payload_element, "Ea", last_sample->Ea);
	      	    cJSON_AddNumberToObject(payload_element, "pf", last_sample->pf);
	      	}
	    }
        }else{
	      const char *error_ptr = cJSON_GetErrorPtr();
	      if(error_ptr){
		  printf("CJSON ERROR: %s", error_ptr);
	      }
	}	
	out_string = cJSON_Print(JSON_data);
    
    }else{
        const char *error_ptr = cJSON_GetErrorPtr();
        if(error_ptr){
            printf("CJSON ERROR: %s", error_ptr);
        }
    }
    cJSON_Delete(JSON_data);
    return out_string;
}
//To compile run:
//gcc main.c -o main.o `pkg-config --cflags --libs libcjson`

