import express from 'express';
import {PORT} from './config/index.js'; 
import client from './services/mqtt_service.js';


let app = express();
app.use(express.json());
app.use(express.urlencoded({extended: false}));

client.subscribe('#', (error, success) => {
	if(error){
		console.error(error);
	}else{
		console.log(`Subscribed to ${success[0].topic}`);

	}});

app.listen(PORT||4200, () => {
	console.log(`Server listening on port ${PORT}`);
})
