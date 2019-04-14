   var set_real_time;
   function handleServerResponse(){
    clearTimeout(set_real_time);
    var res = jsonResponse.time.split(":");
    real_time(hours=res[0],min=res[1],sec=res[2]);
    document.body.style.backgroundColor="rgb("+jsonResponse.rgb+")";
   }
   function real_time(hours,min,sec) {
   sec=Number(sec)+1;
    if (sec>=60){min=Number(min)+1;sec=0;}
    if (min>=60){hours=Number(hours)+1;min=0;}
    if (hours>=24){hours=0};
	if (min<10) {min_display="0"+min;} else {min_display=min;}
	if (sec<10) {sec_display="0"+sec;} else {sec_display=sec;}
    //document.getElementById("time").innerHTML = hours+":"+min+":"+sec;
	document.getElementById("time").innerHTML = hours+":"+min_display+":"+sec_display;
    set_real_time = setTimeout("real_time("+hours+","+min+","+sec+");", 1000);
   }
   function load_time(submit){
    server = "/Time";
    send_request(submit,server);
    load();
   }
   function time_zone(submit){
    server = "/TimeZone?timezone="+val('timezone');
    send_request(submit,server);
   }
   function set_time_zone(submit){
    var set_date = new Date();
    var gmtHours = -set_date.getTimezoneOffset()/60;
    document.getElementById('timezone').value = gmtHours;
    server = "/TimeZone?timezone="+gmtHours;
    send_request(submit,server);
   }
   function set_ssid(submit){
    server = "/ssid?ssid="+val('ssid')+"&password="+encodeURIComponent(val('password'));
    send_request(submit,server);
    alert("Изменения вступят в силу после перезагрузки. Пожалуйста перезагрузите устройство.");
   }
   function set_ssdp(submit){
    server = "/ssdp?ssdp="+val('ssdp');
    send_request(submit,server);
    document.getElementById('ssdp_t').innerHTML = val('ssdp');
   }   
   function set_ssid_ap(submit){
    server = "/ssidap?ssidAP="+val('ssidap')+"&passwordAP="+encodeURIComponent(val('passwordAp'));
    send_request(submit,server);
    alert("Изменения вступят в силу после перезагрузки. Пожалуйста перезагрузите устройство.");
   }
   function restart(submit,texts){
    if (confirm(texts)) {
	 server = "/restart?device=ok";
     send_request(submit,server);
     return true;
    } else {
     return false;
    }
   }   
   function set_new_time(submit, texts){
	if (confirm(texts)) {
		server = "/TimeNew?time_h="+val('time_h')+"&time_m="+val('time_m')+"&time_s="+val('time_s');
		send_request(submit,server);
		load_time(submit);
		alert("Ok");
		window.location.reload();
		return true;
    } else {
		return false;
    }
   }  
   function set_ntp(submit){
	var usertc = 1;
	var checkbox = document.getElementById('use_sync');
	if (checkbox.checked != true){
		usertc = 1;
	}
	else {
		usertc = 0;
	}
    server = "/TimeNtp?use_sync="+usertc;
    send_request(submit,server);
	load_time(submit);
    alert("Ok");
	window.location.reload();
   }      
   function set_ntp_server(submit){
    server = "/setntpserver?ntpserver="+val('ntpserver');
    send_request(submit,server);
	alert("Ok");
   }     
   function set_text1(submit){
    server = "/text?text1="+val('text1');
    send_request(submit,server);
	alert("Ok");
   }  
   function set_bright(submit){
    server = "/setbright?bright="+val('bright');
    send_request(submit,server);
	alert("Ok");
   }  
   function set_speed(submit){
    server = "/setspeed?speed_d="+val('speed_d');
    send_request(submit,server);
	alert("Ok");
   } 
   function set_weather(submit){
    server = "/weather?city_code="+val('city_code')+"&w_api="+val('w_api');
    send_request(submit,server);
    alert("Ok");
   }  
   function set_lang(submit){
    server = "/lang?lang="+val('lang_sel');
    send_request(submit,server);
    alert("Ok");
   }      
   function set_alarm1(submit){
		server = "/setalarm1?alarm1_h="+val('alarm1_h')+"&alarm1_m="+val('alarm1_m');
		send_request(submit,server);
		load_time(submit);
		alert("Ok");
		window.location.reload();
   }  
   function on_alarm1(submit){
	var usealarm1 = 1;
	var checkbox = document.getElementById('use_alarm1');
	if (checkbox.checked == true){
		usealarm1 = 1;
	}
	else {
		usealarm1 = 0;
	}
    server = "/usealarm1?use_alarm1="+usealarm1;
    send_request(submit,server);
    alert("Ok");
	window.location.reload();
   }  
   function set_alarm2(submit){
		server = "/setalarm2?alarm2_h="+val('alarm2_h')+"&alarm2_m="+val('alarm2_m');
		send_request(submit,server);
		load_time(submit);
		alert("Ok");
		window.location.reload();
   }  
   function on_alarm2(submit){
	var usealarm2 = 1;
	var checkbox = document.getElementById('use_alarm2');
	if (checkbox.checked == true){
		usealarm2 = 1;
	}
	else {
		usealarm2 = 0;
	}
    server = "/usealarm2?use_alarm2="+usealarm2;
    send_request(submit,server);
    alert("Ok");
	window.location.reload();
   }            
