import express from 'express';
import {PORT} from './config/index.js'; 
import { addPowerMeasurements } from './controllers/pub.js';

let app = express();
app.use(express.json());
app.use(express.urlencoded({extended: false}));

app.post('/pub', addPowerMeasurements, (req, res) => {
	console.log(req.body);
	res.status(200).send('Received');
});

app.listen(PORT||4200, () => {
	console.log(`Server listening on port ${PORT}`);
})
