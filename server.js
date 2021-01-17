const express = require('express')
const bodyParser = require('body-parser');

const app = express()
const port = 3000

let temperature = "";
let humidity = "";
let apiKey = "f00uA7uC47RCmdB3DWOAh4UkLhsVhfn1";

app.get('/', (req, res) => {
  res.send('Temperature: ' + temperature + ', Humidity: ' + humidity);
})


app.use(bodyParser.json())
app.use(function (req, res, next) {

 if (req.body["api_key"] == apiKey) {
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

