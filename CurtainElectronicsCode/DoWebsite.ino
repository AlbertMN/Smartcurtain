String print_website() {
  String to_return;

  to_return = "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
  "<style>"
    ".badge-default{background-color:#636c72}.badge{display:inline-block;padding:.25em .4em;font-size:75%;font-weight:700;line-height:1;color:#fff;text-align:center;white-space:nowrap;vertical-align:baseline;border-radius:.25rem}.wifi_header{user-select:none;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none}body{font-family:-apple-system,system-ui,BlinkMacSystemFont,'Segoe UI',Roboto,'Helvetica Neue',Arial,sans-serif;font-size:1rem;font-weight:400;line-height:1.5;color:#292b2c;background-color:#fff;padding:20px;padding-top:0}.wifi_element{width:200px;background:red;color:white;display:none;overflow:hidden}.button{cursor:pointer;margin-top:8px;background-color:#008CBA;border:none;color:white;padding:8px 25px;text-align:center;text-decoration:none;display:inline-block;font-size:16px}input{padding:8px 12px;margin:8px 0;display:inline-block;border:1px solid #ccc;border-radius:4px;box-sizing:border-box}#wifi_settings{display:none;width:80%;border:1px solid grey;border-radius:8px;padding-left:15px;padding-right:15px;padding-bottom:15px;margin-bottom:15px}.wifi_collapse_carret{cursor:pointer}"
  "</style>"

  "</head>"
  "<body>"
  "<div id='main_container'>"
    "<h2><b>Smart Gardin:</b> settings</h2>";

    to_return += get_near_wifi();
  
    to_return += "<br><div class='display_wifi'>"
    "<span class='wifi_header' onclick='toggle_collapse(this)'>Manual connect <span class='carret'>&#9660;</span><hr></span>"
    "</div>"
  "</div>"
  
  "<div id='wifi_settings'>"
  "<div id='wifi_ssid_is_set'>"
  "<b><p id='the_wifi_ssid'>SSID</p></b>"
  "</div>"
  "<form type='GET' action='submit'>"
  "<div id='wifi_requires_ssid' style='margin-top:15px;'>"
  "<label><b>WiFi name (SSID)</b></label><br>"
  "<input type='text' id='wifi_ssid' name='wifi_ssid' value=''>"
  "</div>"
  "<span id='no_pass_required'>Network is open. No password required.</span>"
  ""
  "<div id='wifi_requires_pass'>"
  "<label><b>Password</b></label><br>"
  "<input id='wifi_password' name='wifi_pass' type='password'>"
  "</div>"
  "<button id='connect_button' type='submit' class='button'>Connect</button>"
  "</form>"
  "</div>"

  "<script>"
    "function hasClass(ele,cls){return ele.classList.contains(cls)}"
    "function removeClass(ele,cls){ele.classList.remove(cls)}"
    "var wiFiSettingsDiv=document.getElementById('wifi_settings');function toggle_collapse(elem){elem.parentElement.appendChild(wiFiSettingsDiv);var carret=elem.getElementsByClassName('carret')[0];if(hasClass(elem.parentElement,'settings_open')){removeClass(elem.parentElement,'settings_open');carret.innerHTML='&#9660;';wiFiSettingsDiv.style.display='none'}else{document.getElementById('wifi_password').value='';var allCarrets=document.getElementsByClassName('carret');for(var the_carret in allCarrets){allCarrets[the_carret].innerHTML='&#9660;'};carret.innerHTML='&#9650;';var other_open=document.getElementsByClassName('settings_open');if(other_open.length!=0){removeClass(document.getElementsByClassName('settings_open')[0],'settings_open')}"
    "elem.parentElement.className+=' settings_open';document.getElementById('wifi_requires_ssid').style.display='none';document.getElementById('wifi_requires_pass').style.display=(hasClass(elem.parentElement,'wifi_open')?'none':'block');document.getElementById('no_pass_required').style.display=(hasClass(elem.parentElement,'wifi_open')?'block':'none');if(!elem.parentElement.hasAttribute('data-ssid')){document.getElementById('wifi_ssid').value='';document.getElementById('wifi_ssid_is_set').style.display='none';document.getElementById('wifi_requires_ssid').style.display='block'}else{var ssid=elem.parentElement.getAttribute('data-ssid');document.getElementById('wifi_requires_ssid').style.display='none';document.getElementById('wifi_ssid_is_set').style.display='block';document.getElementById('the_wifi_ssid').innerHTML=ssid;document.getElementById('wifi_ssid').value=ssid}"
    "wiFiSettingsDiv.style.display='block'}}"
  "</script>"
  
  "</body>"
  "</html>";

  return to_return;
}

String get_near_wifi() {
  String to_return;
  int n = WiFi.scanNetworks(false, false);
  int o = n;
  int loops = 0;

  if (n == 0) {
    to_return += "No networks found. Internet connection is required for the Smart Courtain to work. If there is no accessible WiFi you can use an ethernet cable.";
  } else {
    // sort by RSSI
    int indices[n];
    int skip[n];

    String ssid;

    for (int i = 0; i < n; i++) {
      indices[i] = i;
    }

    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
          loops++;
          //int temp = indices[j];
          //indices[j] = indices[i];
          //indices[i] = temp;
          std::swap(indices[i], indices[j]);
          std::swap(skip[i], skip[j]);
        }
      }
    }

    for (int i = 0; i < n; i++) {
      if (indices[i] == -1) {
        --o;
        continue;
      }
      ssid = WiFi.SSID(indices[i]);
      for (int j = i + 1; j < n; j++) {
        loops++;
        if (ssid == WiFi.SSID(indices[j])) {
          indices[j] = -1;
        }
      }
    }
    to_return = String(o) + " networks found<br>";

    int actual_i = 0;
    for (int i = 0; i < n; ++i) {
      bool isOpen = encryptionTypeStr(WiFi.encryptionType(indices[i])) == "OPEN";
      if (indices[i] != -1) {
        actual_i++;
        String extraClass = (isOpen ? " wifi_open" : "");
        to_return += "<div class='display_wifi" + extraClass + "' data-ssid='" + WiFi.SSID(indices[i]) + "'><span class='wifi_header' onclick='toggle_collapse(this)'>" + String(actual_i);
        to_return += ": " + WiFi.SSID(indices[i]) + " ";
        to_return += (WiFi.isHidden(indices[i]) ? "<span class=\"badge badge-default\"><i>hidden</i></span>" : "");
        to_return += " ";
        to_return += (isOpen ? "<span class=\"badge badge-default\">open</span>" : "");
        to_return += " <span class='carret'>&#9660;</span><hr></span></div>";
      }
      delay(10);
    }
  }
  return to_return;
}
