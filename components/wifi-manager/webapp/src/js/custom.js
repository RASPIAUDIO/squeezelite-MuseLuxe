import he from 'he';
import { Promise } from 'es6-promise';

if (!String.prototype.format) {
  Object.assign(String.prototype, {
    format() {
      const args = arguments;
      return this.replace(/{(\d+)}/g, function(match, number) {
        return typeof args[number] !== 'undefined' ? args[number] : match;
      });
    }, 
  }); 
}
if (!String.prototype.encodeHTML) {
  Object.assign(String.prototype, {
    encodeHTML() {
      return he.encode(this).replace(/\n/g, '<br />')
    },
  });
}
Object.assign(Date.prototype, {
  toLocalShort() {
    const opt = { dateStyle: 'short', timeStyle: 'short' };
    return this.toLocaleString(undefined, opt);
  },
});


const nvsTypes = {
  NVS_TYPE_U8: 0x01,

  /*! < Type uint8_t */
  NVS_TYPE_I8: 0x11,

  /*! < Type int8_t */
  NVS_TYPE_U16: 0x02,

  /*! < Type uint16_t */
  NVS_TYPE_I16: 0x12,

  /*! < Type int16_t */
  NVS_TYPE_U32: 0x04,

  /*! < Type uint32_t */
  NVS_TYPE_I32: 0x14,

  /*! < Type int32_t */
  NVS_TYPE_U64: 0x08,

  /*! < Type uint64_t */
  NVS_TYPE_I64: 0x18,

  /*! < Type int64_t */
  NVS_TYPE_STR: 0x21,

  /*! < Type string */
  NVS_TYPE_BLOB: 0x42,

  /*! < Type blob */
  NVS_TYPE_ANY: 0xff /*! < Must be last */,
};
const btIcons = {
  bt_playing: 'play-circle-fill',
  bt_disconnected: 'bluetooth-fill',
  bt_neutral: '',
  bt_connected: 'bluetooth-connect-fill',
  bt_disabled: '',
  play_arrow:  'play-circle-fill',
  pause: 'pause-circle-fill',
  stop:  'stop-circle-fill',
  '': '',
};

const btStateIcons = [
  { desc: 'Idle', sub: ['bt_neutral'] },
  { desc: 'Discovering', sub: ['bt_disconnected'] },
  { desc: 'Discovered', sub: ['bt_disconnected'] },
  { desc: 'Unconnected', sub: ['bt_disconnected'] },
  { desc: 'Connecting', sub: ['bt_disconnected'] },
  { 
    desc: 'Connected',
    sub: ['bt_connected', 'play_arrow', 'bt_playing', 'pause', 'stop'],
  },
  { desc: 'Disconnecting', sub: ['bt_disconnected'] },
];

const pillcolors = {
  MESSAGING_INFO: 'badge-success',
  MESSAGING_WARNING: 'badge-warning',
  MESSAGING_ERROR: 'badge-danger',
};
const connectReturnCode = {
  UPDATE_CONNECTION_OK : 0, 
	UPDATE_FAILED_ATTEMPT : 1,
	UPDATE_USER_DISCONNECT : 2,
  UPDATE_LOST_CONNECTION : 3,
  UPDATE_FAILED_ATTEMPT_AND_RESTORE : 4
}
const taskStates = {
  0: 'eRunning',
  /*! < A task is querying the state of itself, so must be running. */
  1: 'eReady',
  /*! < The task being queried is in a read or pending ready list. */
  2: 'eBlocked',
  /*! < The task being queried is in the Blocked state. */
  3: 'eSuspended',
  /*! < The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
  4: 'eDeleted',
};
const flash_status_codes = {
  NONE : 0,
  REBOOT_TO_RECOVERY: 2,
  SET_FWURL: 5,
  FLASHING: 6,
  DONE: 7,
  UPLOADING: 8,
  ERROR: 9
};
let flash_state=flash_status_codes.FLASH_NONE;
let flash_ota_dsc='';
let flash_ota_pct=0;
let older_recovery=false;
function isFlashExecuting(data){
  return (flash_state!=flash_status_codes.UPLOADING ) && (data.ota_dsc!='' || data.ota_pct>0);
}
function post_config(data){
  let confPayload={
    timestamp: Date.now(),
    config : data
  };
  $.ajax({
    url: '/config.json',
    dataType: 'text',
    method: 'POST',
    cache: false,
    contentType: 'application/json; charset=utf-8',
    data: JSON.stringify(confPayload),
    error: handleExceptionResponse,
  });
}
function process_ota_event(data){
  if(data.ota_dsc){
    flash_ota_dsc=data.ota_dsc;
  }
  if( data.ota_pct != undefined){
    flash_ota_pct=data.ota_pct;
  }
  
  if(flash_state==flash_status_codes.ERROR){
    return;
  }
  else if(isFlashExecuting(data)){
    flash_state=flash_status_codes.FLASHING;
  }  
  else if(flash_state==flash_status_codes.FLASHING ){
    if(flash_ota_pct ==100){
      // we were processing OTA, and we've reached 100%
      flash_state=flash_status_codes.DONE;
      $('#flashfilename').val('');
    } 
    else if(flash_ota_pct<0 && older_recovery){
      // we were processing OTA on an older recovery and we missed the 
      // end of flashing.
      console.log('End of flashing from older recovery');
      if(data.ota_dsc==''){
        flash_ota_dsc = 'OTA Process Completed';
      }
      flash_state=flash_status_codes.DONE;
    }
  }
  else if(flash_state ==flash_status_codes.UPLOADING){ 
    if(flash_ota_pct ==100){
      // we were processing OTA, and we've reached 100%
      // reset the progress bar 
      flash_ota_pct = 0;
      flash_state=flash_status_codes.FLASHING;
    } 
  }
}
function set_ota_error(message){
  flash_state=flash_status_codes.ERROR;
  handle_flash_state({
    ota_pct: 0,
    ota_dsc: message,
    event: flash_events.SET_ERROR
  });  
}
function show_update_dialog(){
  $('#otadiv').modal();
    if (flash_ota_pct >= 0) {
      update_progress();
    }
    if (flash_ota_dsc !== '') {
      $('span#flash-status').html(flash_ota_dsc);
    }
}
const flash_events={
  SET_ERROR: function(data){
    if(data.ota_dsc){
      flash_ota_dsc=data.ota_dsc;
    }
    else {
      flash_ota_dsc = 'Error';
    }
    flash_ota_pct=data.ota_pct??0;
    $('#fwProgressLabel').parent().addClass('bg-danger');
    update_progress();
    show_update_dialog();
  },
  START_OTA : function() {
    if (flash_state == flash_status_codes.NONE || flash_state == flash_status_codes.ERROR || flash_state == undefined) {
      $('#fwProgressLabel').parent().removeClass('bg-danger');
      flash_state=flash_status_codes.REBOOT_TO_RECOVERY;
      if(!recovery){
        flash_ota_dsc = 'Starting recovery mode...';
        // Reboot system to recovery mode
        const data = {
          timestamp: Date.now(),
        };

        $.ajax({
          url: '/recovery.json',
          dataType: 'text',
          method: 'POST',
          cache: false,
          contentType: 'application/json; charset=utf-8',
          data: JSON.stringify(data),
          error:  function(xhr, _ajaxOptions, thrownError){
            set_ota_error(`Unexpected error while trying to restart to recovery. (status=${xhr.status??''}, error=${thrownError??''} ) `);
          },
          complete: function(response) {
            console.log(response.responseText);
          },
        });     
      }
      else {
        flash_ota_dsc='Starting Update';
      }
      show_update_dialog();
      
    }
    else {
      console.warn('Unexpected status while starting flashing');
    }    
  },
  FOUND_RECOVERY: function(data) {
    console.log(JSON.stringify(data));
    const url=$('#fw-url-input').val();
    if(flash_state == flash_status_codes.REBOOT_TO_RECOVERY){
        const fileInput = $('#flashfilename')[0].files;
        if (fileInput.length > 0) {
          flash_ota_dsc = 'Sending file to device.';
          flash_state= flash_status_codes.UPLOADING;
          const uploadPath = '/flash.json';
          const xhttp = new XMLHttpRequest();
    //      xhrObj.upload.addEventListener("loadstart", loadStartFunction, false);  
          xhttp.upload.addEventListener("progress", progressFunction, false);  
          //xhrObj.upload.addEventListener("load", transferCompleteFunction, false);  
          xhttp.onreadystatechange = function() {
            if (xhttp.readyState === 4) {
              if(xhttp.status === 0 || xhttp.status === 404) {
                set_ota_error(`Upload Failed. Recovery version might not support uploading. Please use web update instead.`);
                $('#flashfilename').val('');
              }
            }
          };
          xhttp.open('POST', uploadPath, true);
          xhttp.send(fileInput[0] );
        }
        else if(url==''){
          flash_state= flash_status_codes.NONE;
        }
        else {
          flash_ota_dsc = 'Saving firmware URL location.';
          flash_state= flash_status_codes.SET_FWURL;
          let confData= { fwurl: {
                value: $('#fw-url-input').val(),
                type: 33,
            }
          };
          post_config(confData);          
        }
        show_update_dialog();
    }
  },
  PROCESS_OTA_UPLOAD: function(data){
    flash_state= flash_status_codes.UPLOADING;
    process_ota_event(data);
    show_update_dialog();
  },
  PROCESS_OTA_STATUS: function(data){
    if(data.ota_pct>0){
      older_recovery = true;
    }
    if(flash_state == flash_status_codes.REBOOT_TO_RECOVERY){
      data.event = flash_events.FOUND_RECOVERY;
      handle_flash_state(data);
    }
    else if(flash_state==flash_status_codes.DONE && !recovery){
      flash_state=flash_status_codes.NONE;
      $('#rTable tr.release').removeClass('table-success table-warning');
      $('#fw-url-input').val('');
    }

    else {
      process_ota_event(data);
      if(flash_state && (flash_state >flash_status_codes.NONE && flash_ota_pct>=0) ) {
        show_update_dialog();
      }
    } 
  },
  PROCESS_OTA: function(data) {
    process_ota_event(data);
    if(flash_state && (flash_state >flash_status_codes.NONE && flash_ota_pct>=0) ) {
      show_update_dialog();
    }
  }
};
window.hideSurrounding = function(obj){
  $(obj).parent().parent().hide();
}
function update_progress(){
  $('.progress-bar')
    .css('width', flash_ota_pct + '%')
    .attr('aria-valuenow', flash_ota_pct)
    .text(flash_ota_pct+'%')
  $('.progress-bar').html((flash_state==flash_status_codes.DONE?100:flash_ota_pct) + '%');

}
function handle_flash_state(data) {
  if(data.event)  {
    data.event(data);
  } 
  else {
    console.error('Unexpected error while processing handle_flash_state');
    return;
  }

}
window.hFlash = function(){
  // reset file upload selection if any;
  $('#flashfilename').val('');
  handle_flash_state({ event: flash_events.START_OTA, url: $('#fw-url-input').val() });
}
window.handleReboot = function(link){
  
  if(link=='reboot_ota'){
    $('#reboot_ota_nav').removeClass('active').prop("disabled",true); delayReboot(500,'', 'reboot_ota');
  }
  else {
    $('#reboot_nav').removeClass('active'); delayReboot(500,'',link);
  }
}
function progressFunction(evt){  
  // if (evt.lengthComputable) {  
  //   progressBar.max = evt.total;  
  //   progressBar.value = evt.loaded;  
  //   percentageDiv.innerHTML = Math.round(evt.loaded / evt.total * 100) + "%";  
  // }  
  handle_flash_state({
    ota_pct: ( Math.round(evt.loaded / evt.total * 100)),
    ota_dsc: ('Uploading file to device'),
    event: flash_events.PROCESS_OTA_UPLOAD
  });  
}  
function handlebtstate(data) {
  let icon = '';
  let tt = '';
  if (data.bt_status !== undefined && data.bt_sub_status !== undefined) {
    const iconsvg = btStateIcons[data.bt_status].sub[data.bt_sub_status];
    if (iconsvg) {
      icon = `#${btIcons[iconsvg]}`;
      tt = btStateIcons[data.bt_status].desc;
    } else {
      icon = `#${btIcons.bt_connected}`;
      tt = 'Output status';
    }
  }
  $('#o_type').title = tt;
  $('#o_bt').attr('xlink:href',icon);

  
}
function handleTemplateTypeRadio(outtype) {
  if (outtype === 'bt') {
    $('#bt').prop('checked', true);
    $('#o_bt').attr('display', 'inline');
    $('#o_spdif').attr('display', 'none');
    $('#o_i2s').attr('display', 'none');
    output = 'bt';
  } else if (outtype === 'spdif') {
    $('#spdif').prop('checked', true);
    $('#o_bt').attr('display', 'none');
    $('#o_spdif').attr('display', 'inline');
    $('#o_i2s').attr('display', 'none');
    output = 'spdif';
  } else {
    $('#i2s').prop('checked', true);
    $('#o_bt').attr('display', 'none');
    $('#o_spdif').attr('display', 'none');
    $('#o_i2s').attr('display', 'inline');
    output = 'i2s';
  }
}

function handleExceptionResponse(xhr, _ajaxOptions, thrownError) {
  console.log(xhr.status);
  console.log(thrownError);
  if (thrownError !== '') {
    showLocalMessage(thrownError, 'MESSAGING_ERROR');
  }
}
function HideCmdMessage(cmdname) {
  $('#toast_' + cmdname).css('display', 'none');
  $('#toast_' + cmdname)
    .removeClass('table-success')
    .removeClass('table-warning')
    .removeClass('table-danger')
    .addClass('table-success');
  $('#msg_' + cmdname).html('');
}
function showCmdMessage(cmdname, msgtype, msgtext, append = false) {
  let color = 'table-success';
  if (msgtype === 'MESSAGING_WARNING') {
    color = 'table-warning';
  } else if (msgtype === 'MESSAGING_ERROR') {
    color = 'table-danger';
  }
  $('#toast_' + cmdname).css('display', 'block');
  $('#toast_' + cmdname)
    .removeClass('table-success')
    .removeClass('table-warning')
    .removeClass('table-danger')
    .addClass(color);
  let escapedtext = msgtext
    .substring(0, msgtext.length - 1)
    .encodeHTML()
    .replace(/\n/g, '<br />');
  escapedtext =
    ($('#msg_' + cmdname).html().length > 0 && append
      ? $('#msg_' + cmdname).html() + '<br/>'
      : '') + escapedtext;
  $('#msg_' + cmdname).html(escapedtext);
}

let releaseURL =
  'https://api.github.com/repos/sle118/squeezelite-esp32/releases';
  
let recovery = false;
const commandHeader = 'squeezelite -b 500:2000 -d all=info -C 30 -W';
let blockAjax = false;
//let blockFlashButton = false;
let apList = null;
//let selectedSSID = '';
//let checkStatusInterval = null;
let messagecount = 0;
let messageseverity = 'MESSAGING_INFO';
let StatusIntervalActive = false;
let LastRecoveryState = null;
let SystemConfig={};
let LastCommandsState = null;
var output = '';
let hostName = '';
let versionName='Squeezelite-ESP32';
let prevmessage='';
let project_name=versionName;
let platform_name=versionName;
let btSinkNamesOptSel='#cfg-audio-bt_source-sink_name';
let ConnectedToSSID={};
let ConnectingToSSID={};
let lmsBaseUrl;
let prevLMSIP='';
const ConnectingToActions = {
  'CONN' : 0,'MAN' : 1,'STS' : 2,
}

Promise.prototype.delay = function(duration) {
  return this.then(
    function(value) {
      return new Promise(function(resolve) {
        setTimeout(function() {
          resolve(value);
        }, duration);
      });
    },
    function(reason) {
      return new Promise(function(_resolve, reject) {
        setTimeout(function() {
          reject(reason);
        }, duration);
      });
    }
  );
};

function startCheckStatusInterval() {
  StatusIntervalActive = true;
  setTimeout(checkStatus, 3000);
}


function RepeatCheckStatusInterval() {
  if (StatusIntervalActive) {
    startCheckStatusInterval();
  }
}

function getConfigJson(slimMode) {
  const config = {};
  $('input.nvs').each(function(_index, entry) {
    if (!slimMode) {
      const nvsType = parseInt(entry.attributes.nvs_type.value, 10);
      if (entry.id !== '') {
        config[entry.id] = {};
        if (
          nvsType === nvsTypes.NVS_TYPE_U8 ||
          nvsType === nvsTypes.NVS_TYPE_I8 ||
          nvsType === nvsTypes.NVS_TYPE_U16 ||
          nvsType === nvsTypes.NVS_TYPE_I16 ||
          nvsType === nvsTypes.NVS_TYPE_U32 ||
          nvsType === nvsTypes.NVS_TYPE_I32 ||
          nvsType === nvsTypes.NVS_TYPE_U64 ||
          nvsType === nvsTypes.NVS_TYPE_I64
        ) {
          config[entry.id].value = parseInt(entry.value);
        } else {
          config[entry.id].value = entry.value;
        }
        config[entry.id].type = nvsType;
      }
    } else {
      config[entry.id] = entry.value;
    }
  });
  const key = $('#nvs-new-key').val();
  const val = $('#nvs-new-value').val();
  if (key !== '') {
    if (!slimMode) {
      config[key] = {};
      config[key].value = val;
      config[key].type = 33;
    } else {
      config[key] = val;
    }
  }
  return config;
}

// eslint-disable-next-line no-unused-vars
function onFileLoad(elementId, event) {
  let data = {};
  try {
    data = JSON.parse(elementId.srcElement.result);
  } catch (e) {
    alert('Parsing failed!\r\n ' + e);
  }
  $('input.nvs').each(function(_index, entry) {
    if (data[entry.id]) {
      if (data[entry.id] !== entry.value) {
        console.log(
          'Changed ' + entry.id + ' ' + entry.value + '==>' + data[entry.id]
        );
        $(this).val(data[entry.id]);
      }
    }
  });
}

// eslint-disable-next-line no-unused-vars
function onChooseFile(event, onLoadFileHandler) {
  if (typeof window.FileReader !== 'function') {
    throw "The file API isn't supported on this browser.";
  }
  const input = event.target;
  if (!input) {
    throw 'The browser does not properly implement the event object';
  }
  if (!input.files) {
    throw 'This browser does not support the `files` property of the file input.';
  }
  if (!input.files[0]) {
    return undefined;
  }
  const file = input.files[0];
  let fr = new FileReader();
  fr.onload = onLoadFileHandler;
  fr.readAsText(file);
  input.value = '';
}
function delayReboot(duration, cmdname, ota = 'reboot') {
  const url = '/'+ota+'.json';
  $('tbody#tasks').empty();
  $('#tasks_sect').css('visibility', 'collapse');
  Promise.resolve({ cmdname: cmdname, url: url })
    .delay(duration)
    .then(function(data) {
      if (data.cmdname.length > 0) {
        showCmdMessage(
          data.cmdname,
          'MESSAGING_WARNING',
          'System is rebooting.\n',
          true
        );
      } else {
        showLocalMessage('System is rebooting.\n', 'MESSAGING_WARNING');
      }
      console.log('now triggering reboot');
      $("button[onclick*='handleReboot']").addClass('rebooting');
      $.ajax({
        url: data.url,
        dataType: 'text',
        method: 'POST',
        cache: false,
        contentType: 'application/json; charset=utf-8',
        data: JSON.stringify({
          timestamp: Date.now(),
        }),
        error: handleExceptionResponse,
        complete: function() {
          console.log('reboot call completed');
          Promise.resolve(data)
            .delay(6000)
            .then(function(rdata) {
              if (rdata.cmdname.length > 0) {
                HideCmdMessage(rdata.cmdname);
              }
              getCommands();
              getConfig();
            });
        },
      });
    });
}
// eslint-disable-next-line no-unused-vars
window.saveAutoexec1 = function(apply) {
  showCmdMessage('cfg-audio-tmpl', 'MESSAGING_INFO', 'Saving.\n', false);
  let commandLine = commandHeader + ' -n "' + $('#player').val() + '"';
  if (output === 'bt') {
    commandLine += ' -o "BT" -R -Z 192000';
    showCmdMessage(
      'cfg-audio-tmpl',
      'MESSAGING_INFO',
      'Remember to configure the Bluetooth audio device name.\n',
      true
    );
  } else if (output === 'spdif') {
    commandLine += ' -o SPDIF -Z 192000';
  } else {
    commandLine += ' -o I2S';
  }
  if ($('#optional').val() !== '') {
    commandLine += ' ' + $('#optional').val();
  }
  const data = {
    timestamp: Date.now(),
  };
  data.config = {
    autoexec1: { value: commandLine, type: 33 },
    autoexec: {
      value: $('#disable-squeezelite').prop('checked') ? '0' : '1',
      type: 33,
    },
  };

  $.ajax({
    url: '/config.json',
    dataType: 'text',
    method: 'POST',
    cache: false,
    contentType: 'application/json; charset=utf-8',
    data: JSON.stringify(data),
    error: handleExceptionResponse,
    complete: function(response) {
      if (
        response.responseText.result &&
        JSON.parse(response.responseText).result === 'OK'
      ) {
        showCmdMessage('cfg-audio-tmpl', 'MESSAGING_INFO', 'Done.\n', true);
        if (apply) {
          delayReboot(1500, 'cfg-audio-tmpl');
        }
      } else if (response.responseText.result) {
        showCmdMessage(
          'cfg-audio-tmpl',
          'MESSAGING_WARNING',
          JSON.parse(response.responseText).Result + '\n',
          true
        );
      } else {
        showCmdMessage(
          'cfg-audio-tmpl',
          'MESSAGING_ERROR',
          response.statusText + '\n'
        );
      }
      console.log(response.responseText);
    },
  });
  console.log('sent data:', JSON.stringify(data));
}
window.handleDisconnect = function(){
   $.ajax({
       url: '/connect.json',
       dataType: 'text',
       method: 'DELETE',
       cache: false,
       contentType: 'application/json; charset=utf-8',
       data: JSON.stringify({
         timestamp: Date.now(),
       }),
     });
}
function setPlatformFilter(val){
  if($('.upf').filter(function(){ return $(this).text().toUpperCase()===val.toUpperCase()}).length>0){
    $('#splf').val(val).trigger('input');
    return true;
  }
  return false;
}
window.handleConnect = function(){
  ConnectingToSSID.ssid = $('#manual_ssid').val();
  ConnectingToSSID.pwd = $('#manual_pwd').val();
  ConnectingToSSID.dhcpname = $('#dhcp-name2').val();
  $("*[class*='connecting']").hide();
  $('#ssid-wait').text(ConnectingToSSID.ssid);
  $('.connecting').show();

  $.ajax({
    url: '/connect.json',
    dataType: 'text',
    method: 'POST',
    cache: false,
    contentType: 'application/json; charset=utf-8',
    data: JSON.stringify({
      timestamp: Date.now(),
      ssid: ConnectingToSSID.ssid,
      pwd: ConnectingToSSID.pwd
    }),
    error: handleExceptionResponse,
  });

  // now we can re-set the intervals regardless of result
  startCheckStatusInterval();

}
$(document).ready(function() {
  $('#wifiTable').on('click','tr', function() {

  });
  $('#fw-url-input').on('input', function() {
    if($(this).val().length>8 && ($(this).val().startsWith('http://') || $(this).val().startsWith('https://'))){
      $('#start-flash').show();
    } 
    else {
      $('#start-flash').hide();
    }
  });
  $('.upSrch').on('input', function() {
    const val = this.value;
    $("#rTable tr").removeClass(this.id+'_hide');
    if(val.length>0) {
      $(`#rTable td:nth-child(${$(this).parent().index()+1})`).filter(function(){ 
        return !$(this).text().toUpperCase().includes(val.toUpperCase());
      }).parent().addClass(this.id+'_hide');
    }
    $('[class*="_hide"]').hide();
    $('#rTable tr').not('[class*="_hide"]').show()

  });
  setTimeout(refreshAP,1500);

  
  $('#otadiv').on('hidden.bs.modal', function () {
    // reset flash status. This should stop the state machine from
    // executing steps up to flashing itself.
    flash_state=flash_status_codes.NONE;
  });
  $('#WifiConnectDialog').on('shown.bs.modal', function () {
    $("*[class*='connecting']").hide();
    if(ConnectingToSSID.Action!==ConnectingToActions.STS){
      $('.connecting-init').show();
      $('#manual_ssid').trigger('focus');      
    }
    else {
      handleWifiDialog();
    }
  })
  $('#WifiConnectDialog').on('hidden.bs.modal', function () {
    $('#WifiConnectDialog input').val('');
  })
  
  $('#uCnfrm').on('shown.bs.modal', function () {
    $('#selectedFWURL').text($('#fw-url-input').val());
  })
 
  $('input#show-commands')[0].checked = LastCommandsState === 1;
  $('a[href^="#tab-commands"]').hide();
  $('#load-nvs').on('click', function() {
    $('#nvsfilename').trigger('click');
  });
  $('#clear-syslog').on('click', function() {
    messagecount = 0;
    messageseverity = 'MESSAGING_INFO';
    $('#msgcnt').text('');
    $('#syslogTable').html('');
  });
  
  $('#wifiTable').on('click','tr', function() {
    ConnectingToSSID.Action=ConnectingToActions.CONN;
    if($(this).children('td:eq(1)').text() == ConnectedToSSID.ssid){
      ConnectingToSSID.Action=ConnectingToActions.STS;
       return;
     }
     if(!$(this).is(':last-child')){
      ConnectingToSSID.ssid=$(this).children('td:eq(1)').text();
      $('#manual_ssid').val(ConnectingToSSID.ssid);
     } 
     else {
       ConnectingToSSID.Action=ConnectingToActions.MAN;
       ConnectingToSSID.ssid='';
       $('#manual_ssid').val(ConnectingToSSID.ssid);
     }
   });


  $('#ok-credits').on('click', function() {
    $('#credits').slideUp('fast', function() {});
    $('#app').slideDown('fast', function() {});
  });

  $('#acredits').on('click', function(event) {
    event.preventDefault();
    $('#app').slideUp('fast', function() {});
    $('#credits').slideDown('fast', function() {});
  });

  $('input#show-commands').on('click', function() {
    this.checked = this.checked ? 1 : 0;
    if (this.checked) {
      $('a[href^="#tab-commands"]').show();
      LastCommandsState = 1;
    } else {
      LastCommandsState = 0;
      $('a[href^="#tab-commands"]').hide();
    }
  });

  $('input#show-nvs').on('click', function() {
    this.checked = this.checked ? 1 : 0;
    if (this.checked) {
      $('*[href*="-nvs"]').show();
    } else {
      $('*[href*="-nvs"]').hide();
    }
  });
 
  $('#save-as-nvs').on('click', function() {
    const config = getConfigJson(true);
    const a = document.createElement('a');
    a.href = URL.createObjectURL(
      new Blob([JSON.stringify(config, null, 2)], {
        type: 'text/plain',
      })
    );
    a.setAttribute(
      'download',
      'nvs_config_' + hostName + '_' + Date.now() + 'json'
    );
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  });

  $('#save-nvs').on('click', function() {
    post_config(getConfigJson(false));
  });
  
  $('#fwUpload').on('click', function() {
    const fileInput = document.getElementById('flashfilename').files;
    if (fileInput.length === 0) {
      alert('No file selected!');
    } else {
      handle_flash_state({ event: flash_events.START_OTA, file: fileInput[0] });
    }
    
  });
   $('[name=output-tmpl]').on('click', function() {
    handleTemplateTypeRadio(this.id);
  });

  $('#chkUpdates').on('click', function() {
    $('#rTable').html('');
    $.getJSON(releaseURL, function(data) {
      let i = 0;
      const branches = [];
      data.forEach(function(release) {
        const namecomponents = release.name.split('#');
        const branch = namecomponents[3];
        if (!branches.includes(branch)) {
          branches.push(branch);
        }
      });
      let fwb='';
      branches.forEach(function(branch) {
        fwb += '<option value="' + branch + '">' + branch + '</option>';
      });
      $('#fwbranch').append(fwb);

      data.forEach(function(release) {
        let url = '';
        release.assets.forEach(function(asset) {
          if (asset.name.match(/\.bin$/)) {
            url = asset.browser_download_url;
          }
        });
        const namecomponents = release.name.split('#');
        const ver = namecomponents[0];
        const cfg = namecomponents[2];
        const branch = namecomponents[3];
        var bits = ver.substr(ver.lastIndexOf('-')+1);
        bits = (bits =='32' || bits == '16')?bits:'';

        let body = release.body;
        body = body.replace(/'/gi, '"');
        body = body.replace(
          /[\s\S]+(### Revision Log[\s\S]+)### ESP-IDF Version Used[\s\S]+/,
          '$1'
        );
        body = body.replace(/- \(.+?\) /g, '- ');
        $('#rTable').append(`<tr class='release ' fwurl='${url}'>
        <td data-toggle='tooltip' title='${body}'>${ver}</td><td>${new Date(release.created_at).toLocalShort()}
        </td><td class='upf'>${cfg}</td><td>${branch}</td><td>${bits}</td></tr>`
        );
      });
      if (i > 7) {
        $('#releaseTable').append(
          "<tr id='showall'>" +
            "<td colspan='6'>" +
            "<input type='button' id='showallbutton' class='btn btn-info' value='Show older releases' />" +
            '</td>' +
            '</tr>'
        );
        $('#showallbutton').on('click', function() {
          $('tr.hide').removeClass('hide');
          $('tr#showall').addClass('hide');
        });
      }
      $('#searchfw').css('display', 'inline');
      if(!setPlatformFilter(platform_name)){
        setPlatformFilter(project_name)
      }
      $('#rTable tr.release').on('click', function() {
        var url=this.attributes['fwurl'].value;
        if (lmsBaseUrl) {
          url = url.replace(/.*\/download\//, lmsBaseUrl + '/plugins/SqueezeESP32/firmware/');
        }
        $('#fw-url-input').val(url);
        $('#start-flash').show();
        $('#rTable tr.release').removeClass('table-success table-warning');
        $(this).addClass('table-success table-warning');
      });

    }).fail(function() {
      alert('failed to fetch release history!');
    });    
  });
    $('#fwcheck').on('click', function() {    
    $('#releaseTable').html('');
    $('#fwbranch').empty();
    $.getJSON(releaseURL, function(data) {
      let i = 0;
      const branches = [];
      data.forEach(function(release) {
        const namecomponents = release.name.split('#');
        const branch = namecomponents[3];
        if (!branches.includes(branch)) {
          branches.push(branch);
        }
      });
      let fwb;
      branches.forEach(function(branch) {
        fwb += '<option value="' + branch + '">' + branch + '</option>';
      });
      $('#fwbranch').append(fwb);

      data.forEach(function(release) {
        let url = '';
        release.assets.forEach(function(asset) {
          if (asset.name.match(/\.bin$/)) {
            url = asset.browser_download_url;
          }
        });
        const namecomponents = release.name.split('#');
        const ver = namecomponents[0];
        const idf = namecomponents[1];
        const cfg = namecomponents[2];
        const branch = namecomponents[3];

        let body = release.body;
        body = body.replace(/'/gi, '"');
        body = body.replace(
          /[\s\S]+(### Revision Log[\s\S]+)### ESP-IDF Version Used[\s\S]+/,
          '$1'
        );
        body = body.replace(/- \(.+?\) /g, '- ');
        const trclass = i++ > 6 ? ' hide' : '';
        $('#releaseTable').append(
          "<tr class='release" +
            trclass +
            "'>" +
            "<td data-toggle='tooltip' title='" +
            body +
            "'>" +
            ver +
            '</td>' +
            '<td>' +
            new Date(release.created_at).toLocalShort() +
            '</td>' +
            '<td>' +
            cfg +
            '</td>' +
            '<td>' +
            idf +
            '</td>' +
            '<td>' +
            branch +
            '</td>' +
            "<td><input type='button' class='btn btn-success' value='Select' data-url='" +
            url +
            "' onclick='setURL(this);' /></td>" +
            '</tr>'
        );
      });
      if (i > 7) {
        $('#releaseTable').append(
          "<tr id='showall'>" +
            "<td colspan='6'>" +
            "<input type='button' id='showallbutton' class='btn btn-info' value='Show older releases' />" +
            '</td>' +
            '</tr>'
        );
        $('#showallbutton').on('click', function() {
          $('tr.hide').removeClass('hide');
          $('tr#showall').addClass('hide');
        });
      }
      $('#searchfw').css('display', 'inline');
    }).fail(function() {
      alert('failed to fetch release history!');
    });
  });

  $('#updateAP').on('click', function() {
    refreshAP();
    console.log('refresh AP');
  });

  // first time the page loads: attempt to get the connection status and start the wifi scan
  getConfig();
  getCommands();

  // start timers
  startCheckStatusInterval();
});

// eslint-disable-next-line no-unused-vars
window.setURL = function(button) {
  let url = button.dataset.url;

  $('[data-url^="http"]')
    .addClass('btn-success')
    .removeClass('btn-danger');
  $('[data-url="' + url + '"]')
    .addClass('btn-danger')
    .removeClass('btn-success');

  // if user can proxy download through LMS, modify the URL
  if (lmsBaseUrl) {
    url = url.replace(/.*\/download\//, lmsBaseUrl + '/plugins/SqueezeESP32/firmware/');
  }

  $('#fwurl').val(url);
}


function rssiToIcon(rssi) {
  if (rssi >= -55) {
    return `signal-wifi-fill`;
  } else if (rssi >= -60) {
    return `signal-wifi-3-fill`;
  } else if (rssi >= -65) {
    return `signal-wifi-2-fill`;
  } else if (rssi >= -70) {
    return `signal-wifi-1-fill`;
  } else {   
    return `signal-wifi-line`;
  }
}

function refreshAP() {
  $.getJSON('/scan.json', async function() {
    await sleep(2000);
    $.getJSON('/ap.json', function(data) {
      if (data.length > 0) {
        // sort by signal strength
        data.sort(function(a, b) {
          const x = a.rssi;
          const y = b.rssi;
          // eslint-disable-next-line no-nested-ternary
          return x < y ? 1 : x > y ? -1 : 0;
        });
        apList = data;
        refreshAPHTML2(apList);

      }
    });
  }); 
}
function formatAP(ssid, rssi, auth){
  return `<tr data-toggle="modal" data-target="#WifiConnectDialog"><td></td><td>${ssid}</td><td>
  
  	<svg style="fill:white; width:1.5rem; height: 1.5rem;">
				<use xlink:href="#${rssiToIcon(rssi)}"></use>
			</svg>
  </td><td>
 
  <svg style="fill:white; width:1.5rem; height: 1.5rem;">
  <use xlink:href="#lock${(auth == 0 ? '-unlock':'')}-fill"></use>
</svg>

  </td></tr>`;
}
function refreshAPHTML2(data) {
  let h = '';
  $('#wifiTable tr td:first-of-type').text('');
  $('#wifiTable tr').removeClass('table-success table-warning');
  if(data){
    data.forEach(function(e) {
      h+=formatAP(e.ssid, e.rssi, e.auth);
    });
    $('#wifiTable').html(h);
  }
  if($('.manual_add').length == 0){
    $('#wifiTable').append(formatAP('Manual add', 0,0));
    $('#wifiTable tr:last').addClass('table-light text-dark').addClass('manual_add');
  }
  if(ConnectedToSSID.ssid && ( ConnectedToSSID.urc === connectReturnCode.UPDATE_CONNECTION_OK || ConnectedToSSID.urc === connectReturnCode.UPDATE_FAILED_ATTEMPT_AND_RESTORE )){
    const wifiSelector=`#wifiTable td:contains("${ConnectedToSSID.ssid}")`;
    if($(wifiSelector).filter(function() {return $(this).text() === ConnectedToSSID.ssid;  }).length==0){
      $('#wifiTable').prepend(`${formatAP(ConnectedToSSID.ssid, ConnectedToSSID.rssi ?? 0, 0)}`);
    }
    $(wifiSelector).filter(function() {return $(this).text() === ConnectedToSSID.ssid;  }).siblings().first().html('&check;').parent().addClass((ConnectedToSSID.urc === connectReturnCode.UPDATE_CONNECTION_OK?'table-success':'table-warning'));
    $('span#foot-wifi').html(`SSID: <strong>${ConnectedToSSID.ssid}</strong>, IP: <strong>${ConnectedToSSID.ip}</strong>`);    
    $('#wifiStsIcon').attr('xlink:href',rssiToIcon(ConnectedToSSID.rssi));
  }
  else {
    $('span#foot-wifi').html('');
  }
  
}
function showTask(task) {
  console.debug(
    this.toLocaleString() +
      '\t' +
      task.nme +
      '\t' +
      task.cpu +
      '\t' +
      taskStates[task.st] +
      '\t' +
      task.minstk +
      '\t' +
      task.bprio +
      '\t' +
      task.cprio +
      '\t' +
      task.num
  );
  $('tbody#tasks').append(
    '<tr class="table-primary"><th scope="row">' +
      task.num +
      '</th><td>' +
      task.nme +
      '</td><td>' +
      task.cpu +
      '</td><td>' +
      taskStates[task.st] +
      '</td><td>' +
      task.minstk +
      '</td><td>' +
      task.bprio +
      '</td><td>' +
      task.cprio +
      '</td></tr>'
  );
}
function btExists(name){
  return getBTSinkOpt(name).length>0;
}
function getBTSinkOpt(name){
  return $(`${btSinkNamesOptSel} option:contains('${name}')`);
}
function getMessages() {
  $.getJSON('/messages.json', async function(data) {
    for (const msg of data) {
      const msgAge = msg.current_time - msg.sent_time;
      var msgTime = new Date();
      msgTime.setTime(msgTime.getTime() - msgAge);
      switch (msg.class) {
        case 'MESSAGING_CLASS_OTA':
          var otaData = JSON.parse(msg.message);
          handle_flash_state({
            ota_pct: (otaData.ota_pct ?? -1),
            ota_dsc: (otaData.ota_dsc ??''),
            event: flash_events.PROCESS_OTA
          });
          break;
        case 'MESSAGING_CLASS_STATS':
          // for task states, check structure : task_state_t
          var statsData = JSON.parse(msg.message);
          console.debug(
            msgTime.toLocalShort() +
              ' - Number of running tasks: ' +
              statsData.ntasks
          );
          console.debug(
            msgTime.toLocalShort() +
              '\tname' +
              '\tcpu' +
              '\tstate' +
              '\tminstk' +
              '\tbprio' +
              '\tcprio' +
              '\tnum'
          );
          if (statsData.tasks) {
            if ($('#tasks_sect').css('visibility') === 'collapse') {
              $('#tasks_sect').css('visibility', 'visible');
            }
            $('tbody#tasks').html('');
            statsData.tasks
              .sort(function(a, b) {
                return b.cpu - a.cpu;
              })
              .forEach(showTask, msgTime);
          } else if ($('#tasks_sect').css('visibility') === 'visible') {
            $('tbody#tasks').empty();
            $('#tasks_sect').css('visibility', 'collapse');
          }
          break;
        case 'MESSAGING_CLASS_SYSTEM':
          showMessage(msg, msgTime);
          break;
        case 'MESSAGING_CLASS_CFGCMD':
          var msgparts = msg.message.split(/([^\n]*)\n(.*)/gs);
          showCmdMessage(msgparts[1], msg.type, msgparts[2], true);
          break;
        case 'MESSAGING_CLASS_BT':
          if($("#cfg-audio-bt_source-sink_name").is('input')){
          var attr=$("#cfg-audio-bt_source-sink_name")[0].attributes;
          var attrs='';
          for (var j = 0; j < attr.length; j++) {
              if(attr.item(j).name!="type"){
                attrs+=`${attr.item(j).name } = "${attr.item(j).value}" `;
              }
          }
          var curOpt=$("#cfg-audio-bt_source-sink_name")[0].value;
            $("#cfg-audio-bt_source-sink_name").replaceWith(`<select id="cfg-audio-bt_source-sink_name" ${attrs}><option value="${curOpt}" data-description="${curOpt}">${curOpt}</option></select> `);
          }
          JSON.parse(msg.message).forEach(function(btEntry) {
            //<input type="text" class="form-control bg-success" placeholder="name" hasvalue="true" longopts="sink_name" shortopts="n" checkbox="false" cmdname="cfg-audio-bt_source" id="cfg-audio-bt_source-sink_name" name="cfg-audio-bt_source-sink_name">
            //<select hasvalue="true" longopts="jack_behavior" shortopts="j" checkbox="false" cmdname="cfg-audio-general" id="cfg-audio-general-jack_behavior" name="cfg-audio-general-jack_behavior" class="form-control "><option>--</option><option>Headphones</option><option>Subwoofer</option></select>            
            if(!btExists(btEntry.name)){
              $("#cfg-audio-bt_source-sink_name").append(`<option>${btEntry.name}</option>`);
              showMessage({ type:msg.type, message:`BT Audio device found: ${btEntry.name} RSSI: ${btEntry.rssi} `}, msgTime);
            }
            getBTSinkOpt(btEntry.name).attr('data-description', `${btEntry.name} (${btEntry.rssi}dB)`)
                                      .attr('rssi',btEntry.rssi)
                                      .attr('value',btEntry.name)
                                      .text(`${btEntry.name} [${btEntry.rssi}dB]`).trigger('change');
            
          });
          $(btSinkNamesOptSel).append($(`${btSinkNamesOptSel} option`).remove().sort(function(a, b) { 
              console.log(`${parseInt($(a).attr('rssi'))} < ${parseInt( $(b).attr('rssi'))} ? `);
              return parseInt($(a).attr('rssi')) < parseInt( $(b).attr('rssi')) ? 1 : -1; 
            }));
          break;
        default:
          break;
      }
    }
  }).fail(function(xhr, ajaxOptions, thrownError){
      if(xhr.status==404){
        $('.orec').hide(); // system commands won't be available either
      } 
      else {
        handleExceptionResponse(xhr, ajaxOptions, thrownError);
      }
      
    }
  );

  /*
    Minstk is minimum stack space left
Bprio is base priority
cprio is current priority
nme is name
st is task state. I provided a "typedef" that you can use to convert to text
cpu is cpu percent used
*/
}
function handleRecoveryMode(data) {
  const locRecovery= data.recovery ??0;
  if (LastRecoveryState !== locRecovery) {
    LastRecoveryState = locRecovery;
    $('input#show-nvs')[0].checked = LastRecoveryState === 1;
  }
  if ($('input#show-nvs')[0].checked) {
    $('*[href*="-nvs"]').show();

  } else {
    $('*[href*="-nvs"]').hide();
  }
  if (locRecovery === 1) {
    recovery = true;
    $('.recovery_element').show();
    $('.ota_element').hide();
    $('#boot-button').html('Reboot');
    $('#boot-form').attr('action', '/reboot_ota.json');
  } else {
    recovery = false;
    $('.recovery_element').hide();
    $('.ota_element').show();
    $('#boot-button').html('Recovery');
    $('#boot-form').attr('action', '/recovery.json');
  }
}
function hasConnectionChanged(data){
// gw: "192.168.10.1"
// ip: "192.168.10.225"
// netmask: "255.255.255.0"
// ssid: "MyTestSSID"

  return (data.urc !== ConnectedToSSID.urc || 
    data.ssid !== ConnectedToSSID.ssid || 
    data.gw !== ConnectedToSSID.gw  ||
    data.netmask !== ConnectedToSSID.netmask ||
    data.ip !== ConnectedToSSID.ip || data.rssi !== ConnectedToSSID.rssi )
}
function handleWifiDialog(data){
  if($('#WifiConnectDialog').is(':visible')){
    if(ConnectedToSSID.ip) {
      $('#ipAddress').text(ConnectedToSSID.ip);
    }
    if(ConnectedToSSID.ssid) {
      $('#connectedToSSID' ).text(ConnectedToSSID.ssid);
    }    
    if(ConnectedToSSID.gw) {
      $('#gateway' ).text(ConnectedToSSID.gw);
    }        
    if(ConnectedToSSID.netmask) {
      $('#netmask' ).text(ConnectedToSSID.netmask);
    }            
    if(ConnectingToSSID.Action===undefined || (ConnectingToSSID.Action && ConnectingToSSID.Action == ConnectingToActions.STS)) {
      $("*[class*='connecting']").hide();
      $('.connecting-status').show();
    }
    if(SystemConfig.ap_ssid){
      $('#apName').text(SystemConfig.ap_ssid);
    }
    if(SystemConfig.ap_pwd){
      $('#apPass').text(SystemConfig.ap_pwd);
    }    
    if(!data)
    {
      return;
    }
    else {
      switch (data.urc) {
        case connectReturnCode.UPDATE_CONNECTION_OK:
          if(data.ssid && data.ssid===ConnectingToSSID.ssid){
            $("*[class*='connecting']").hide();
            $('.connecting-success').show();            
            ConnectingToSSID.Action = ConnectingToActions.STS;
          }
          break;
          case connectReturnCode.UPDATE_FAILED_ATTEMPT:
          // 
          if(ConnectingToSSID.Action !=ConnectingToActions.STS && ConnectingToSSID.ssid == data.ssid ){
            $("*[class*='connecting']").hide();
            $('.connecting-fail').show();
          }
          break;
          case connectReturnCode.UPDATE_LOST_CONNECTION:
    
          break;            
          case connectReturnCode.UPDATE_FAILED_ATTEMPT_AND_RESTORE:
            if(ConnectingToSSID.Action !=ConnectingToActions.STS && ConnectingToSSID.ssid != data.ssid ){
              $("*[class*='connecting']").hide();
              $('.connecting-fail').show();
            }
          break;
        case connectReturnCode.UPDATE_USER_DISCONNECT:
            // that's a manual disconnect
            // if ($('#wifi-status').is(':visible')) {
            //   $('#wifi-status').slideUp('fast', function() {});
            //   $('span#foot-wifi').html('');
    
            // }                 
          break;
        default:
          break;
      }
    }

  }
}
function handleWifiStatus(data) {
  if(hasConnectionChanged(data)){
    ConnectedToSSID=data;
    refreshAPHTML2();
  }
  handleWifiDialog(data);
}

function batteryToIcon(voltage) {
        /* Assuming Li-ion 18650s as a power source, 3.9V per cell, or above is treated
				as full charge (>75% of capacity).  3.4V is empty. The gauge is loosely
				following the graph here:
					https://learn.adafruit.com/li-ion-and-lipoly-batteries/voltages
				using the 0.2C discharge profile for the rest of the values.
			*/
  if (voltage > 0) {
    if (inRange(voltage, 5.8, 6.8) || inRange(voltage, 8.8, 10.2)) {
      return `battery-low-line`;
    } else if (inRange(voltage, 6.8, 7.4) || inRange(voltage, 10.2, 11.1)) {
      return `battery-low-line`;
    } else if (
      inRange(voltage, 7.4, 7.5) ||
      inRange(voltage, 11.1, 11.25)
    ) {
      return `battery-low-line`;
    } else if (
      inRange(voltage, 7.5, 7.8) ||
      inRange(voltage, 11.25, 11.7)
    ) {
      return `battery-fill`;
    } else {
      return `battery-line`;
    }
  }
}
function checkStatus() {
  RepeatCheckStatusInterval();
  if (blockAjax) {
    return;
  }
  blockAjax = true;
  getMessages();
  $.getJSON('/status.json', function(data) {
    handleRecoveryMode(data);
    handleWifiStatus(data);
    handlebtstate(data);
    handle_flash_state({
      ota_pct: (data.ota_pct ?? -1),
      ota_dsc: (data.ota_dsc ??''),
      event: flash_events.PROCESS_OTA_STATUS
    });
    if (data.project_name && data.project_name !== '') {
      project_name = data.project_name;
    }
    if(data.platform_name && data.platform_name!==''){
      platform_name = data.platform_name;
    }
    if (data.version && data.version !== '') {
      versionName=data.version;
      $("#navtitle").html(`${project_name}${recovery?'<br>[recovery]':''}`);
      $('span#foot-fw').html(`fw: <strong>${versionName}</strong>, mode: <strong>${recovery?"Recovery":project_name}</strong>`);
    } else {
      $('span#flash-status').html('');
    }
    if (data.Voltage) {
     $('#battery').attr('xlink:href', `#${batteryToIcon(data.Voltage)}`);
     $('#battery').show();
    } else {
      $('#battery').hide();
    }
    if((data.message??'')!='' && prevmessage != data.message){
      // supporting older recovery firmwares - messages will come from the status.json structure
      prevmessage = data.message;
      showLocalMessage(data.message, 'MESSAGING_INFO')
    }
    $("button[onclick*='handleReboot']").removeClass('rebooting');

    if (typeof lmsBaseUrl == "undefined" || data.lms_ip != prevLMSIP && data.lms_ip && data.lms_port) {
      const baseUrl = 'http://' + data.lms_ip + ':' + data.lms_port;
      prevLMSIP=data.lms_ip;
      $.ajax({
        url: baseUrl + '/plugins/SqueezeESP32/firmware/-check.bin', 
        type: 'HEAD',
        dataType: 'text',
        cache: false,
        error: function() {
          // define the value, so we don't check it any more.
          lmsBaseUrl = '';
        },
        success: function() {
          lmsBaseUrl = baseUrl;
        }
      });
    }
    
    $('#o_jack').attr('display', Number(data.Jack) ? 'inline' : 'none');
    blockAjax = false;
  }).fail(function(xhr, ajaxOptions, thrownError) {
    handleExceptionResponse(xhr, ajaxOptions, thrownError);
    blockAjax = false;
  });
}
// eslint-disable-next-line no-unused-vars
window.runCommand = function(button, reboot) {
  let cmdstring = button.attributes.cmdname.value;
  showCmdMessage(
    button.attributes.cmdname.value,
    'MESSAGING_INFO',
    'Executing.',
    false
  );
  const fields = document.getElementById('flds-' + cmdstring);
  cmdstring += ' ';
  if (fields) {
    const allfields = fields.querySelectorAll('select,input');
    for (var i = 0; i < allfields.length; i++) {
      const attr = allfields[i].attributes;
      let qts = '';
      let opt = '';
      let isSelect = $(allfields[i]).is('select');
      const hasValue=attr.hasvalue.value === 'true';
      const validVal=(isSelect && allfields[i].value !== '--' ) || ( !isSelect && allfields[i].value !== '' );

      if ( !hasValue|| hasValue && validVal)  {
        if (attr.longopts.value !== 'undefined') {
          opt += '--' + attr.longopts.value;
        } else if (attr.shortopts.value !== 'undefined') {
          opt = '-' + attr.shortopts.value;
        }

        if (attr.hasvalue.value === 'true') {
          if (allfields[i].value !== '') {
            qts = /\s/.test(allfields[i].value) ? '"' : '';
            cmdstring += opt + ' ' + qts + allfields[i].value + qts + ' ';
          }
        } else {
          // this is a checkbox
          if (allfields[i].checked) {
            cmdstring += opt + ' ';
          }
        }
      }
    }
  }
  console.log(cmdstring);

  const data = {
    timestamp: Date.now(),
  };
  data.command = cmdstring;

  $.ajax({
    url: '/commands.json',
    dataType: 'text',
    method: 'POST',
    cache: false,
    contentType: 'application/json; charset=utf-8',
    data: JSON.stringify(data),
    error: function(xhr, _ajaxOptions, thrownError){
      var cmd=JSON.parse(this.data  ).command;
      if(xhr.status==404){
        showCmdMessage(
          cmd.substr(0,cmd.indexOf(' ')),
          'MESSAGING_ERROR',
          `${recovery?'Limited recovery mode active. Unsupported action ':'Unexpected error while processing command'}`,
          true
        );
      }
      else {
        handleExceptionResponse(xhr, _ajaxOptions, thrownError);
        showCmdMessage(
          cmd.substr(0,cmd.indexOf(' ')-1),
          'MESSAGING_ERROR',
          `Unexpected error ${(thrownError !== '')?thrownError:'with return status = '+xhr.status}`,
          true
        );        
      }
    },
    success: function(response) {
      // var returnedResponse = JSON.parse(response.responseText);
      $('.orec').show();
      console.log(response.responseText);
      if (
        response.responseText &&
        JSON.parse(response.responseText).Result === 'Success' &&
        reboot
      ) {
        delayReboot(2500, button.attributes.cmdname.value);
      }
    },
  });
}
function getLongOps(data, name, longopts){
  return data.values[name]!==undefined?data.values[name][longopts]:"";
}
function getCommands() {
  $.getJSON('/commands.json', function(data) {
    console.log(data);
    $('.orec').show();
    data.commands.forEach(function(command) {
      if ($('#flds-' + command.name).length === 0) {
        const cmdParts = command.name.split('-');
        const isConfig = cmdParts[0] === 'cfg';
        const targetDiv = '#tab-' + cmdParts[0] + '-' + cmdParts[1];
        let innerhtml = '';

        // innerhtml+='<tr class="table-light"><td>'+(isConfig?'<h1>':'');
        innerhtml +=
          '<div class="card text-white mb-3"><div class="card-header">' +
          command.help.encodeHTML().replace(/\n/g, '<br />') +
          '</div><div class="card-body">';
        innerhtml += '<fieldset id="flds-' + command.name + '">';
        if (command.argtable) {
          command.argtable.forEach(function(arg) {
            let placeholder = arg.datatype || '';
            const ctrlname = command.name + '-' + arg.longopts;
            const curvalue =  getLongOps(data,command.name,arg.longopts);

            let attributes = 'hasvalue=' + arg.hasvalue + ' ';

            // attributes +='datatype="'+arg.datatype+'" ';
            attributes += 'longopts="' + arg.longopts + '" ';
            attributes += 'shortopts="' + arg.shortopts + '" ';
            attributes += 'checkbox=' + arg.checkbox + ' ';
            attributes += 'cmdname="' + command.name + '" ';
            attributes +=
              'id="' +
              ctrlname +
              '" name="' +
              ctrlname +
              '" hasvalue="' +
              arg.hasvalue +
              '"   ';
            let extraclass = arg.mincount > 0 ? 'bg-success' : '';
            if (arg.glossary === 'hidden') {
              attributes += ' style="visibility: hidden;"';
            }
            if (arg.checkbox) {
              innerhtml +=
                '<div class="form-check"><label class="form-check-label">';
              innerhtml +=
                '<input type="checkbox" ' +
                attributes +
                ' class="form-check-input ' +
                extraclass +
                '" value="" >' +
                arg.glossary.encodeHTML() +
                '<small class="form-text text-muted">Previous value: ' +
                (curvalue ? 'Checked' : 'Unchecked') +
                '</small></label>';
            } else {
              innerhtml +=
                '<div class="form-group" ><label for="' +
                ctrlname +
                '">' +
                arg.glossary.encodeHTML() +
                '</label>';
              if (placeholder.includes('|')) {
                extraclass = placeholder.startsWith('+') ? ' multiple ' : '';
                placeholder = placeholder
                  .replace('<', '')
                  .replace('=', '')
                  .replace('>', '');
                innerhtml += `<select ${attributes} class="form-control ${extraclass}" >`;
                placeholder = '--|' + placeholder;
                placeholder.split('|').forEach(function(choice) {
                  innerhtml += '<option >' + choice + '</option>';
                });
                innerhtml += '</select>';
              } else {
                innerhtml +=
                  '<input type="text" class="form-control ' +
                  extraclass +
                  '" placeholder="' +
                  placeholder +
                  '" ' +
                  attributes +
                  '>';
              }
              innerhtml +=
                '<small class="form-text text-muted">Previous value: ' +
                (curvalue || '') +
                '</small>';
            }
            innerhtml += '</div>';
          });
        }
        innerhtml += '<div style="margin-top: 16px;">';
        innerhtml +=
          '<div class="toast show" role="alert" aria-live="assertive" aria-atomic="true" style="display: none;" id="toast_' +
          command.name +
          '">';
        innerhtml +=
          '<div class="toast-header"><strong class="mr-auto">Result</strong><button type="button" class="ml-2 mb-1 close" data-dismiss="toast" aria-label="Close" onclick="$(this).parent().parent().hide()">';
        innerhtml +=
          '<span aria-hidden="true">×</span></button></div><div class="toast-body" id="msg_' +
          command.name +
          '"></div></div>';
        if (isConfig) {
          innerhtml +=
            '<button type="submit" class="btn btn-info" id="btn-save-' +
            command.name +
            '" cmdname="' +
            command.name +
            '" onclick="runCommand(this,false)">Save</button>';
          innerhtml +=
            '<button type="submit" class="btn btn-warning" id="btn-commit-' +
            command.name +
            '" cmdname="' +
            command.name +
            '" onclick="runCommand(this,true)">Apply</button>';
        } else {
          innerhtml +=
            '<button type="submit" class="btn btn-success" id="btn-run-' +
            command.name +
            '" cmdname="' +
            command.name +
            '" onclick="runCommand(this,false)">Execute</button>';
        }
        innerhtml += '</div></fieldset></div></div>';
        if (isConfig) {
          $(targetDiv).append(innerhtml);
        } else {
          $('#commands-list').append(innerhtml);
        }
      }
    });

    data.commands.forEach(function(command) {
      $('[cmdname=' + command.name + ']:input').val('');
      $('[cmdname=' + command.name + ']:checkbox').prop('checked', false);
      if (command.argtable) {
        command.argtable.forEach(function(arg) {
          const ctrlselector = '#' + command.name + '-' + arg.longopts;
          const ctrlValue = getLongOps(data,command.name,arg.longopts);
          if (arg.checkbox) {
            $(ctrlselector)[0].checked = ctrlValue;
          } else {
            if (ctrlValue !== undefined) {
              $(ctrlselector)
                .val(ctrlValue)
                .trigger('change');
            }
            if (
              $(ctrlselector)[0].value.length === 0 &&
              (arg.datatype || '').includes('|')
            ) {
              $(ctrlselector)[0].value = '--';
            }
          }
        });
      }
    });
  }).fail(function(xhr, ajaxOptions, thrownError) {
    if(xhr.status==404){
      $('.orec').hide();
    } 
    else {
      handleExceptionResponse(xhr, ajaxOptions, thrownError);
    }
    
    $('#commands-list').empty();
    blockAjax = false;
  });
}

function getConfig() {
  $.getJSON('/config.json', function(entries) {
    $('#nvsTable tr').remove();
    const data = (entries.config? entries.config : entries);
    SystemConfig = data;
    Object.keys(data)
      .sort()
      .forEach(function(key) {
        let val = data[key].value;
        if (key === 'autoexec') {
          if (data.autoexec.value === '0') {
            $('#disable-squeezelite')[0].checked = true;
          } else {
            $('#disable-squeezelite')[0].checked = false;
          }
        } else if (key === 'autoexec1') {
          const re = /-o\s?(["][^"]*["]|[^-]+)/g;
          const m = re.exec(val);
          if (m[1].toUpperCase().startsWith('I2S')) {
            handleTemplateTypeRadio('i2s');
          } else if (m[1].toUpperCase().startsWith('SPDIF')) {
            handleTemplateTypeRadio('spdif');
          } else if (m[1].toUpperCase().startsWith('"BT')) {
            handleTemplateTypeRadio('bt');
          }
        } else if (key === 'host_name') {
          val = val.replaceAll('"', '');
          $('input#dhcp-name1').val(val);
          $('input#dhcp-name2').val(val);
          $('#player').val(val);
          document.title = val;
          hostName = val;
        } else if (key === 'rel_api') {
           releaseURL = val;
        }
        $('tbody#nvsTable').append(
          '<tr>' +
            '<td>' +
            key +
            '</td>' +
            "<td class='value'>" +
            "<input type='text' class='form-control nvs' id='" +
            key +
            "'  nvs_type=" +
            data[key].type +
            ' >' +
            '</td>' +
            '</tr>'
        );
        $('input#' + key).val(data[key].value);
      });
    $('tbody#nvsTable').append(
      "<tr><td><input type='text' class='form-control' id='nvs-new-key' placeholder='new key'></td><td><input type='text' class='form-control' id='nvs-new-value' placeholder='new value' nvs_type=33 ></td></tr>"
    );
    if (entries.gpio) {
      $('#pins').show();
      $('tbody#gpiotable tr').remove();
      entries.gpio.forEach(function(gpioEntry) {
        $('tbody#gpiotable').append(
          '<tr class=' +
            (gpioEntry.fixed ? 'table-secondary' : 'table-primary') +
            '><th scope="row">' +
            gpioEntry.group +
            '</th><td>' +
            gpioEntry.name +
            '</td><td>' +
            gpioEntry.gpio +
            '</td><td>' +
            (gpioEntry.fixed ? 'Fixed' : 'Configuration') +
            '</td></tr>'
        );
      });
    }
    else {
      $('#pins').hide();
    }
  }).fail(function(xhr, ajaxOptions, thrownError) {
    handleExceptionResponse(xhr, ajaxOptions, thrownError);
    blockAjax = false;
  });
}
function showLocalMessage(message, severity) {
  const msg = {
    message: message,
    type: severity,
  };
  showMessage(msg, new Date());
}

function showMessage(msg, msgTime) {
  let color = 'table-success';

  if (msg.type === 'MESSAGING_WARNING') {
    color = 'table-warning';
    if (messageseverity === 'MESSAGING_INFO') {
      messageseverity = 'MESSAGING_WARNING';
    }
  } else if (msg.type === 'MESSAGING_ERROR') {
    if (
      messageseverity === 'MESSAGING_INFO' ||
      messageseverity === 'MESSAGING_WARNING'
    ) {
      messageseverity = 'MESSAGING_ERROR';
    }
    color = 'table-danger';
  }
  if (++messagecount > 0) {
    $('#msgcnt').removeClass('badge-success');
    $('#msgcnt').removeClass('badge-warning');
    $('#msgcnt').removeClass('badge-danger');
    $('#msgcnt').addClass(pillcolors[messageseverity]);
    $('#msgcnt').text(messagecount);
  }

  $('#syslogTable').append(
    "<tr class='" +
      color +
      "'>" +
      '<td>' +
      msgTime.toLocalShort() +
      '</td>' +
      '<td>' +
      msg.message.encodeHTML() +
      '</td>' +
      '</tr>'
  );
}

function inRange(x, min, max) {
  return (x - min) * (x - max) <= 0;
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

