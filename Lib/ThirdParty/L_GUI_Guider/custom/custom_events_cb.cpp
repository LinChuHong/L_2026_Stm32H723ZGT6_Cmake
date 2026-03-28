#include <custom_events_cb.h>
#include <custom.h>


#if USEMYCBINCPP == 1
void mycb(lv_event_t * e)
{
    lv_obj_t *target = (lv_obj_t*)lv_event_get_target(e);  // Get the object that triggered the event
    lv_ui *ui = (lv_ui*)lv_event_get_user_data(e);
    
    if (lv_event_get_code(e) == LV_EVENT_PRESSED) 
    {
        if (target == ui->screen_btn_2) 
        {
            Serial.println("button 2 pressed");
        }
        else if (target == ui->screen_btn_3)
        {
            Serial.println("button 3 pressed");
        }
    } else if (lv_event_get_code(e) == LV_EVENT_PRESSING) 
    {
        if (target == ui->screen_btn_2) 
        {
            Serial.println("button 2 pressing");
        }
        else if (target == ui->screen_btn_3)
        {
            Serial.println("button 3 pressing");
        }
    }
}
#endif