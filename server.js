const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');
const env = require('dotenv').config();

const app = express();
const port = 3000;
// app.use(express.static('public'))
app.use(express.static(__dirname + '/public'));
app.use('/static', express.static(__dirname + '/public'));

let temperature = "0";
let humidity = "0";
let sensor = "0";
let apiKey = process.env.API_KEY;

const connection = mysql.createConnection({
    host: process.env.DB_HOST,
    user: process.env.DB_USER,
    password: process.env.DB_PASS,
    database: process.env.DB_NAME
});

let sql = 'SELECT * FROM iot_entry';
let sql200 = 'SELECT * FROM iot_entry ORDER BY id DESC LIMIT 200';


connection.connect(function(err) {
    if (err) throw err;
    console.log("Connected!");


    connection.query(sql, function (err, result) {
        if (err) throw err;
        console.log(result[0]);
    });
});



app.get('/api', (req, res) => {
      connection.query(sql, function (err, result) {
        if (err) throw err;
        res.send(JSON.stringify(result));
    });
})


app.get('/api/200', (req, res) => {
      connection.query(sql200, function (err, result) {
        if (err) throw err;
        res.send(JSON.stringify(result.reverse()));
    });
})


// app.get('/', (req, res) => {
//   res.send('Temperature: ' + temperature + ', Humidity: ' + humidity);
// })


app.use(bodyParser.json())
app.use(function (req, res, next) {

 if (req.body["api_key"] === apiKey) {
 sensor = req.body["sensor"];
 temperature = req.body["value1"];
 humidity = req.body["value2"];

 let sqlInsert = `INSERT INTO iot_entry (sensor, temperature, humidity) VALUES ("${sensor}", "${temperature}", "${humidity}");`;

	    connection.query(sqlInsert, function (err, result) {
	        if (err) throw err;
	        console.log("row inserted");
	    });

 console.log(req.body);
}

  // res.send('OK');
  next()
})


app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
})

