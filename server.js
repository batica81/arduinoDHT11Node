const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');
const env = require('dotenv').config();

const app = express();
const port = 3000;

let temperature = "0";
let humidity = "0";
let apiKey = process.env.API_KEY;

const con = mysql.createConnection({
    host: process.env.DB_HOST,
    user: process.env.DB_USER,
    password: process.env.DB_PASS,
    database: process.env.DB_NAME
});

let sql = 'SELECT * FROM iot_entry';

con.connect(function(err) {
    if (err) throw err;
    console.log("Connected!");


    con.query(sql, function (err, result) {
        if (err) throw err;
        result.forEach(e => console.log(e))
    });
});

app.get('/', (req, res) => {
  res.send('Temperature: ' + temperature + ', Humidity: ' + humidity);
})


app.use(bodyParser.json())
app.use(function (req, res, next) {

 if (req.body["api_key"] === apiKey) {
 temperature = req.body["value1"];
 humidity = req.body["value2"];
 console.log(req.body);
}

  res.send('OK');
  next()
})


app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
})

