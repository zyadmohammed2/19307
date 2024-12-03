var sheet_id = '1Z8SgqiiLbAAVtXaEgmRQQ_BfMhjq57jq2sbLxm1OPFw';
var main_sheet_name = "mind_shaft";
var sheet_open = SpreadsheetApp.openById(sheet_id);
var sheet_target = sheet_open.getSheetByName(main_sheet_name);
var cache = CacheService.getScriptCache();

function doGet(e) {
  Logger.log(JSON.stringify(e));
  var result = 'Ok';
  if (e.parameter === "undefined") {
    result = 'No Parameters';
  } else {
    var newRow = sheet_target.getLastRow() + 1;
    var rowDataLog = [];

    let calender = new Date();
    let year = calender.getFullYear();
    let month = calender.getMonth() + 1;
    let day = calender.getDate();
    let hour = calender.getHours();
    let minute = calender.getMinutes();
    let second = calender.getSeconds();

    var current_Date = `${month}/${day}/${year}`;
    rowDataLog[0] = current_Date;

    var current_Time = `${hour}:${minute}:${second}`;
    rowDataLog[1] = current_Time;

    var sts_val = '';

    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);

      switch (param) {
        case 'sts':
          sts_val = value;
          break;
        case 'temp':
          rowDataLog[2] = value;
          result += ', Temperature added';
          break;
        case 'vibrate':
          rowDataLog[3] = value;
          result += ', Vibration added';
          break;
        case 'cot':
          rowDataLog[4] = value;
          result += ', MQ-7 added';
          break;
        default:
          result += ", unsupported parameter";
      }
    }

    if (sts_val === 'write') {
      Logger.log(JSON.stringify(rowDataLog));
      var newRangeDataLog = sheet_target.getRange(newRow, 1, 1, rowDataLog.length);
      newRangeDataLog.setValues([rowDataLog]);
      SpreadsheetApp.flush(); // Ensure data is written

      // Clear cache to force refresh
      cache.remove('latestData');
      return ContentService.createTextOutput(result);
    }
  }
}

function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}