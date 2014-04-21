#include <pebble.h>
  
Window *window;
TextLayer *time_layer;
TextLayer *seconds_layer;
TextLayer *date_layer;
Layer *battery_layer;
int batt_level;


InverterLayer *inv_layer;
char buffer[] = "00:00";
char buffer2[] = ":00";
char buffer3[] = "00-000";

#define BATTERY_LEVEL 80
#define CIRCLE_RADIUS 4
  
static void handle_battery(BatteryChargeState charge_state) {

if (charge_state.is_charging) {
    batt_level = 100;
  } else {
    //snprintf(batt_level, 3, "%d", charge_state.charge_percent);
    batt_level = charge_state.charge_percent;  
  }
  
}

//batt_level=charge_state.charge_percent;
  
void battery_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);

  //graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
if (batt_level >= 80) {
    graphics_fill_circle(ctx, GPoint(4,4), CIRCLE_RADIUS);
    graphics_fill_circle(ctx, GPoint(13,4), CIRCLE_RADIUS);
    graphics_fill_circle(ctx, GPoint(22,4), CIRCLE_RADIUS);
    graphics_fill_circle(ctx, GPoint(31,4), CIRCLE_RADIUS);
  }

  if (batt_level >= 60 && batt_level < 80 ) {
    graphics_fill_circle(ctx, GPoint(4,4), CIRCLE_RADIUS);
    graphics_fill_circle(ctx, GPoint(13,4), CIRCLE_RADIUS);
    graphics_fill_circle(ctx, GPoint(22,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(31,4), CIRCLE_RADIUS);

  }
  if (batt_level >= 40 && batt_level < 60 ) {
    graphics_fill_circle(ctx, GPoint(4,4), CIRCLE_RADIUS);
    graphics_fill_circle(ctx, GPoint(13,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(22,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(31,4), CIRCLE_RADIUS);
  }
  if (batt_level >= 20 && batt_level < 40 ) {
    graphics_fill_circle(ctx, GPoint(4,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(13,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(22,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(31,4), CIRCLE_RADIUS);

  }
  if (batt_level < 20 ) {
    graphics_draw_circle(ctx, GPoint(4,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(13,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(22,4), CIRCLE_RADIUS);
    graphics_draw_circle(ctx, GPoint(31,4), CIRCLE_RADIUS);
  }
      
}  

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  //Here we will update the watchface display
  //Format the buffer string using tick_time as the time source
  if (clock_is_24h_style()){
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  }
  else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  
  strftime(buffer2, sizeof(":00"), ":%S", tick_time);
  strftime(buffer3, sizeof("00-000"), "%d %b", tick_time); 

  
  handle_battery(battery_state_service_peek());
 
  //Change the TextLayer text to show the new time!
  text_layer_set_text(time_layer, buffer);
  text_layer_set_text(seconds_layer, buffer2);
  text_layer_set_text(date_layer, buffer3);
   
}

void window_load(Window *window)
{
  //Display Battery
  battery_layer = layer_create(GRect(0, 0, 144, 10));
  layer_set_update_proc(battery_layer, battery_layer_update_callback);
  
  //Load font
  ResHandle font_time = resource_get_handle(RESOURCE_ID_FONT_META_BOLD_40);
  ResHandle font_seconds = resource_get_handle(RESOURCE_ID_FONT_META_BOOK_30);
  ResHandle font_date = resource_get_handle(RESOURCE_ID_FONT_META_BOOK_30);
  
  //Date layer
  date_layer = text_layer_create(GRect(0, 97, 144, 168));
  text_layer_set_text_color(date_layer, GColorBlack);
  text_layer_set_text_alignment(date_layer, GTextAlignmentLeft);
  text_layer_set_font(date_layer, fonts_load_custom_font(font_date));

  //Time Layer
  time_layer = text_layer_create(GRect(0, 127, 144, 168));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorBlack);
  text_layer_set_text_alignment(time_layer, GTextAlignmentLeft);
  text_layer_set_font(time_layer, fonts_load_custom_font(font_time));

  //Seconds Layer
  seconds_layer = text_layer_create(GRect(102, 137, 144, 168));
  text_layer_set_text_color(seconds_layer, GColorBlack);
  text_layer_set_text_alignment(seconds_layer, GTextAlignmentLeft);
  text_layer_set_font(seconds_layer, fonts_load_custom_font(font_seconds));

  //Inverter layer
  inv_layer = inverter_layer_create(GRect(0, 0, 144, 168));

  
  //Get a time structure so that the face doesn't start blank
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);
 
  //Manually call the tick handler when the window is loading
  tick_handler(t, SECOND_UNIT);

  //Display layers
  layer_add_child(window_get_root_layer(window), (Layer*) battery_layer);  
  layer_add_child(window_get_root_layer(window), (Layer*) date_layer);
  layer_add_child(window_get_root_layer(window), (Layer*) time_layer);
  layer_add_child(window_get_root_layer(window), (Layer*) seconds_layer);
  layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);

}
 
void window_unload(Window *window)
{
    //We will safely destroy the Window's elements here!
    layer_destroy(battery_layer);
    text_layer_destroy(date_layer);
    text_layer_destroy(time_layer);
    text_layer_destroy(seconds_layer);
    inverter_layer_destroy(inv_layer);
}
  
void init()
{
  //Initialize the app elements here!
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
  
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  battery_state_service_subscribe(&handle_battery);


}
 
void deinit()
{
  //We will safely destroy the Window's elements here!
  tick_timer_service_unsubscribe();
  text_layer_destroy(date_layer);
  text_layer_destroy(seconds_layer);
  text_layer_destroy(time_layer);
  layer_destroy(battery_layer);
  window_destroy(window);
  battery_state_service_unsubscribe();
}
  
  
int main(void)
{
  init();
  app_event_loop();
  deinit();
}