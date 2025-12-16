

const String para = "<p>";

//String functions
//

String pageHeader(String my_hostname, String accessPoint){
  String html = "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += styleSheet();
  html += webLink("", skullImage());
  html += para + my_hostname + " on " + accessPoint + " @ " + webLink("/", local_ip);
  return html;
}

// html element wrapping
//
String htmHead(int level, String legend) {
  return "<h" + String(level) + ">" + legend + "</h" + String(level) + ">";
}

String formStart(String action, String hid_name, String hid_val) {
  return "<form action='/" + action + "' method='post'><input type='hidden' name='" + hid_name + "' value='" + hid_val + "'>";
}

String formSubmitEnd(String message) {
  return "<input type='submit' value = '" + message + "'>" + formEnd();
}

String formEnd() {
  return "</form>";
}

String actionBtnForm(String action, String hid_name, String hid_val, String message){
  return formStart(action, hid_name, hid_val) + formSubmitEnd(message);
}

String formTextInput(String title, String id, String val) {
  return title + ": <input type='text' name='" + id + "' value='" + val + "'/>";
}

String formPassInput(String title, String id, String val) {
  return title + ": <input type='password' name='" + id + "' value='" + val + "'/>";
}

String formCheckedbox(String id, String legend, bool checked, bool submit) {
  String elem = (checked?" checked":"");
  return "<p><input type='checkbox' name='" + id + "'" + elem + onChangeSubmit(submit) + "> " + legend;
}

String formRange(String legend, String id, int val, int vmin, int vmax, bool submit, bool parens) {
  String form = legend + ": " + String(val);
  form += "<br><input id='" + id + "' type='range' name='" + id + "' min='" + String(vmin) + "' max='" + String(vmax) + "' ";
  form += "value='" + String(val) + "'" + onChangeSubmit(submit) + ">";
  if(parens){
    form += " (" + String(vmin) + " - " + String(vmax) + ") ";
  }
  return form;
}

String onChangeSubmit(bool submit){
  return (submit?" onchange='this.form.submit()'":"");
}

String selectRange(String id, String legend, int value, int range){
  String options = "";
  for (int i = 0; i < range; ++i) {
    String val = String(i);
    options += singleOption(val, val, value == i);
  }
  return wrapOptions(legend, id, id, false, options);
}

String singleOption(String legend, String value, bool selected){
  return "<option value='" + value + "'" + (selected?" selected":"") + ">" + legend + "</option>";
}

String wrapOptions(String title, String name, String id, bool submit, String options){
  return title + "<select name='" + name + "' id='" + id + "' " + onChangeSubmit(submit) + ">" + options + "</select>";
}
String listItem(String item){
  return "<li>" + item + "</li>";
}
String wrapUList(String items){
  return "<p><ul>" + items + "</ul>";
}

String webLink(String dest, String text){
  return "<a href=\"" + dest + "\">" + text + "</a>";
}

String styleSheet(){
  String theme = "#00940d";
  String style = "<style>";
  style += "body {font-family: sans;}";
  style += "a {color: "+ theme + ";}";
  style += "input[type='range'], input[type='checkbox'] { accent-color: "+ theme + ";}";
  style += "td, th {vertical-align:top;}";
  style += "</style>";
  return style;
}

String skullImage(){
  return "<img src='data:image/gif;base64,R0lGODdhZABkAOeZAACSAgCTBACUAACUBgCVCBWUCROVGiWXMCiXJyaYMSiZMjSXMTWYMjSZOjaZMzWaOzacPD+bQ0GbPECcREKcPUGdRUKeRkOfR0udRkqeTkyeR0ufT0ygUE2hUU6iUlWhWFehUlaiWViiU1ejWlikW2GkXGKlXWGmZGKnZWOoZmSpZ2uoZ2qpbmuqb2yrcG2scXSrcXWscnatc3WuenivdXawe3exfH6vfXiyfYCxf4GygIK0gYO1goK2iYO3iou3i4y4jI25jY66jo+7j5C8kJG9kZW8l5e8kZa9mJe+mZi/mpnAm5rBnJvCnZzDnqTDn6TFqKbFoqXGqafGo6bHqqfIq6jJrK7IrK/JranLrbDKrrLMsLPNsbTOsrbQs7TRurfRtLzQvLbSvL3Rvb7Svr/Uv8DVwMHWwcLXwsPYw8TZxMXaxcbbxsjcyM3cz8/cyc7d0M/e0dDf0tHg09Lh1NPj1dTk19Xl2Nzk2dbm2d3l2tfn2t/m2+Dn3OHp3eLq3uPr4OTs4eXt4uTu6ebu4+Xv6unw5ebx6+jy7O/x7unz7vDy7+r07/Hz8PL08fP28vT38/X49Pb59vj69/n7+Pr8+fv9+vz/+/3//P///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ywAAAAAZABkAAAI/gAvCRxIsKDBgwgTKlzIsKHDhxAjSpxIsaLFixghWqI0aRIlSxlDDtwYKdKkSiIJRhIEB4wUJkygdGHzBxLIlBInGYrD5UmRIkmojLmDCCXGRW6YxAhxgYIECRQsfGiBJM2hmzgZPprTxEWH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' />";
  }
