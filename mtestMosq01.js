
/*

initial script.


important things:  number one , ITS WORK!
number two, Think while its working.

"sing penting, nomer siji, mlaku dhisik"
"nomor loro, dipikir karo mlaku"

adalah script background-process,
berfungsi :
1. men-subscribe satu topic ke MQTT broker,
2. mendapatkan messages yang dikirimkan ke topic tersebut,
3. memproses setiap messages yang diterima, diolah dan dikirimkan ke time-series DB dengan protokol statsd format datadog

time series database+dashboard dapat menggunakan Telegraf-InfluxDB-Grafana

*/
const mqtt = require('mqtt')

const host = 'test.mosquitto.org'
const port = '1883'
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`

const connectUrl = `mqtt://${host}:${port}`

var STATSD_HOST = '192.168.1.222';
var STATSD_PORT = 2183;



var   url = require('url'),    
     sdc = require('statsd-client'),
	 SDC = new sdc({host: STATSD_HOST, port: STATSD_PORT,tcp: true });
	 
	 
const isNumber = require('is-number');


var MAXIMUM_TAG =10;
var MAXIMUM_TAG_DATA = 30; //mengurangi kardinalitas, namun menghilangkan informasi ... trade off
var MAXIMUM_LAST_TAG_DATA = 30; //mengurangi kardinalitas pada tag terakhir, namun menghilangkan informasi ... trade off
var MQprocIDnya = "ID002"; //identifikasi process yang subscribe ke MQTT. setiap fork process, harus uniq.

const client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  username: 'userq123',
  password: 'passq123',
  reconnectPeriod: 1000,
})


const topic = 'mytopic007/#';

client.on('connect', () => {
  console.log('Connected')
  client.subscribe([topic], () => {
    console.log(`Subscribe to topic '${topic}'`)
  })
})

/*
client.on('message', (topic, payload) => {
  console.log('Received Message:', topic, payload.toString())
})
*/

function dtku(){
    d = new Date();
    return [d.getFullYear(),
            d.getMonth() + 1,
            d.getDate()].join('-') + ' ' + [
		d.getHours(),
		d.getMinutes()].join(':');
}

client.on('message', (topic, payload, message) => {
  
    //var qs = url.parse(topic.toString(), true).query;
    var urlParts = url.parse(topic.toString());	
	var statusproses = 1;
	var dtonproc = dtku();
	

   	var processnameku = 'MQver116';
	var BUCKETnya = 'MQdataver116';
    //format payload adalah "N:xxx.xxx"
	var payloadstriplen = 2;
    console.log(dtonproc,'Received Message:',  JSON.stringify(message))
						
    console.log(dtonproc,' topicnya:' , JSON.stringify(urlParts.pathname,  null, "\t") , '  endlog' );
    console.log(dtonproc,' payloadnya:' , JSON.stringify(payload.toString(),  null, "\t") , '  endlog' );
	
	
	        if ((String(payload).length < (payloadstriplen+1) )) {
				console.log(dtonproc,processnameku, ' payload Length error < 3 ! ... endlog' );
				statusproses = 0;
			}
			
			
				var payloadsubs = String(payload).substring(payloadstriplen,String(payload).length) 
			if (statusproses == 1) {

                console.log(dtonproc,' strippayloadnya:' , JSON.stringify(payloadsubs,  null, "\t") , '  endlog' );
				if (!isNumber(payloadsubs)) {
				   statusproses = 0;	
				   console.log(dtonproc,processnameku, ' payload not number ! ... endlog' );
				}	
			}			
	
	
	
			if (statusproses == 1) {
				
 						var splittedpath = urlParts.pathname.split("/");
						
						var ITERATE_TAG = MAXIMUM_TAG;
						if (splittedpath.length < MAXIMUM_TAG) {
							
							ITERATE_TAG = splittedpath.length;
						}
						
						var TAGnya = {};

						TAGnya['SensorID'] = MQprocIDnya;
						TAGnya['ZoneOfID'] =  processnameku;
						
						for (let index = 0; index < ITERATE_TAG; index++) {
							  if (splittedpath[index].length < MAXIMUM_TAG_DATA) {
								TAGnya['gRUp'+String(index)] = splittedpath[index];
							     } else
							     {
								   if (index == (ITERATE_TAG-1)) {  //last tag
									TAGnya['gRUp'+String(index)] = splittedpath[index].substring(0,(MAXIMUM_LAST_TAG_DATA))+ "xDTRIM";								   
								   }else
								   TAGnya['gRUp'+String(index)] = splittedpath[index].substring(0,MAXIMUM_TAG_DATA)+ "xTRIM";
							     }

							}
						
						console.log(dtonproc,processnameku, ' split URL:', JSON.stringify(splittedpath, null, "\t") ,' endlog' );
						console.log(dtonproc,processnameku, ' data TAG:', JSON.stringify(TAGnya, null, "\t") ,' endlog' );

						SDC.gauge(BUCKETnya, payloadsubs ,TAGnya );
						SDC.increment('js_proxy.requests');
						
			}

  
  
  
  
})






