#include "e.h"
#include "e_mod_main.h"

typedef struct _Instance Instance;
typedef struct _Tageslosung Tageslosung;

struct _Instance {
  E_Gadcon_Client *gcc;
  Evas_Object     *tageslosung_obj;
  Tageslosung     *tageslosung;
  Ecore_Timer     *check_timer, *date_checker;
  Config_Item     *ci;
  
  ///
   E_Gadcon_Popup  *popup;
   double           val;
   Ecore_Timer     *popup_timer;
   Evas_Object     *o_backlight, *o_table, *o_slider;
   
   char *buffer;
   char *losungstext;
   char *lehrtext;
   char *losungsvers;
   char *lehrtextvers;
   char *date;
   char *losungs_vers; 
   char *lehrtext_vers;
   ///
};

struct _Tageslosung {
  Instance    *inst;
  Evas_Object *tageslosung_obj;
};

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown(E_Gadcon_Client *gcc);
static void             _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static const char      *_gc_label(const E_Gadcon_Client_Class *client_class);
static Evas_Object     *_gc_icon(const E_Gadcon_Client_Class *client_class, Evas *evas);
static const char      *_gc_id_new(const E_Gadcon_Client_Class *client_class);

/* Func Protos for Module */
static void _tageslosung_cb_mouse_down    (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tageslosung_cb_mouse_in      (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tageslosung_cb_mouse_out     (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tageslosung_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _tageslosung_menu_cb_post     (void *data, E_Menu *m);
static int  _xml_parse(void *data);
static const char _tageslosung_cb_menu_browser_open(void *data);

static Config_Item *_tageslosung_config_item_get(const char *id);
static Tageslosung        *_tageslosung_new            (Evas * evas);
static void         _tageslosung_free           (Tageslosung * tageslosung);
static Eina_Bool    _tageslosung_cb_check       (void *data);
static Eina_Bool    _check_date       (void *data);
// static void         _tageslosung_update_qual    (void *data, double value);
// static void         _tageslosung_update_level   (void *data, double value);

///

static void _tageslosung_popup_free(Instance *inst);
///

static E_Config_DD *conf_edd      = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *tageslosung_config = NULL;

char buf_time_old[255];


static const E_Gadcon_Client_Class _gc_class = {
    GADCON_CLIENT_CLASS_VERSION, "tageslosung", {
        _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new,
        NULL, NULL
    }, E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name,
const char *id, const char *style) {
    E_Gadcon_Client *gcc;
    Evas_Object     *o;
    Tageslosung            *tageslosung;

    Instance *inst = E_NEW (Instance, 1);
    inst->ci = _tageslosung_config_item_get(id);

    tageslosung = _tageslosung_new(gc->evas);
    tageslosung->inst = inst;
    inst->tageslosung = tageslosung;

    o = tageslosung->tageslosung_obj;
    gcc            = e_gadcon_client_new(gc, name, id, style, o);
    gcc->data      = inst;
    inst->gcc      = gcc;
    inst->tageslosung_obj = o;

    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
        _tageslosung_cb_mouse_down, inst);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,
        _tageslosung_cb_mouse_in, inst);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT,
        _tageslosung_cb_mouse_out, inst);

    if (!inst->ci->theme)
    {
        edje_object_signal_emit(inst->tageslosung_obj, "white", "");
        //Edje_Message_Float *val  = malloc(sizeof(Edje_Message_Float));

        //val->val = value;
        edje_object_message_send(inst->tageslosung_obj, EDJE_MESSAGE_FLOAT, 1, "white");
        //free(val);
    }else
    {
        edje_object_signal_emit(inst->tageslosung_obj, "", "black");
        edje_object_message_send(inst->tageslosung_obj, EDJE_MESSAGE_FLOAT, 2, "black");
    //_tageslosung_cb_check(inst);
    }
        
   
    edje_object_signal_emit(inst->tageslosung_obj, "active_losung", "");
    
    edje_object_signal_callback_add(inst->tageslosung_obj, "mouse,over", "part_right", _tageslosung_cb_menu_browser_open, inst);

    if(inst->ci->disable_interval != 1)
    {
     inst->check_timer = ecore_timer_add(inst->ci->poll_time,
        _tageslosung_cb_check, inst);
    
    }
    
    
    _xml_parse(inst);
    
    inst->date_checker = ecore_timer_add(10,
        _check_date, inst);
    
    tageslosung_config->instances = eina_list_append(tageslosung_config->instances, inst);

    
    
    
    return gcc;
}

static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) {
   
   Instance *inst;

   inst = gcc->data;
   switch (orient)
     {
      case E_GADCON_ORIENT_FLOAT:
          e_gadcon_client_aspect_set  (gcc, 150, 48);
          e_gadcon_client_min_size_set(gcc, 150, 48);
          break;
      case E_GADCON_ORIENT_HORIZ:
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_TL:
          e_gadcon_client_aspect_set  (gcc, 150, 16);
          e_gadcon_client_min_size_set(gcc, 150, 16);
          break;
      case E_GADCON_ORIENT_CORNER_TR:
          e_gadcon_client_aspect_set  (gcc, 150, 16);
          e_gadcon_client_min_size_set(gcc, 150, 16);
          break;
      case E_GADCON_ORIENT_CORNER_BL:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;
      case E_GADCON_ORIENT_CORNER_BR:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;

      case E_GADCON_ORIENT_VERT:
      case E_GADCON_ORIENT_LEFT:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;
      case E_GADCON_ORIENT_RIGHT:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;
      case E_GADCON_ORIENT_CORNER_LT:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;
      case E_GADCON_ORIENT_CORNER_RT:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;
      case E_GADCON_ORIENT_CORNER_LB:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;
      case E_GADCON_ORIENT_CORNER_RB:
          e_gadcon_client_aspect_set  (gcc, 16, 16);
          e_gadcon_client_min_size_set(gcc, 16, 16);
          break;

      default:
          e_gadcon_client_aspect_set  (gcc, 150, 16);
          e_gadcon_client_min_size_set(gcc, 150, 16);
        break;
     }
     
          //e_gadcon_client_aspect_set  (gcc, 150, 16);
          //e_gadcon_client_min_size_set(gcc, 150, 16);
}

static const char *
_gc_label(const E_Gadcon_Client_Class *client_class EINA_UNUSED) {
    return D_("Tageslosung");
}


static Evas_Object *_gc_icon(const E_Gadcon_Client_Class *client_class,
Evas *evas) {
    char buf[PATH_MAX];
    Evas_Object *o = edje_object_add(evas);
    snprintf(buf, sizeof(buf), "%s/e-module-tageslosung.edj", tageslosung_config->mod_dir);
    edje_object_file_set(o, buf, "icon");
    return o;
}

static const char *_gc_id_new(const E_Gadcon_Client_Class *client_class) {
    return _tageslosung_config_item_get(NULL)->id;
}

static void _gc_shutdown(E_Gadcon_Client *gcc) {
    Instance *inst = gcc->data;
///    
   _tageslosung_popup_free(inst);
///
    if (inst->date_checker) ecore_timer_del(inst->date_checker);
    if (inst->check_timer) ecore_timer_del(inst->check_timer);
    tageslosung_config->instances = eina_list_remove(tageslosung_config->instances, inst);
    _tageslosung_free(inst->tageslosung);
    E_FREE(inst);
}



static void _tageslosung_menu_cb_post(void *data, E_Menu *m) {
    if (!tageslosung_config->menu) return;
    e_object_del(E_OBJECT(tageslosung_config->menu));
    tageslosung_config->menu = NULL;
}

static void _tageslosung_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) {
    _config_tageslosung_module(((Instance*)data)->ci);
}

void _tageslosung_config_updated(Config_Item *ci) {
    Eina_List *l;
    
//     Instance *inst;
//     inst = data;
    
    if (!tageslosung_config) return;
    for (l = tageslosung_config->instances; l; l = l->next) {
        Instance *inst = l->data;
        if (inst->ci != ci) continue;
    
        

    if(inst->ci->disable_interval !=1)
    {      
        if (inst->check_timer) ecore_timer_del(inst->check_timer);
        inst->check_timer = ecore_timer_add(inst->ci->poll_time,
            _tageslosung_cb_check, inst);
    }
    else
    {
        if (inst->check_timer) ecore_timer_del(inst->check_timer);
        _tageslosung_cb_check(inst);
    }
//          int timer;
//    if (inst->check_timer)  timer = ecore_timer_interval_get(inst->check_timer); 	
//     
//     printf("TIMER:");
//     e_util_dialog_show("TIMER","%d", timer);
    if (!inst->ci->theme)
    {
        edje_object_signal_emit(inst->tageslosung_obj, "white", "");
        edje_object_message_send(inst->tageslosung_obj, EDJE_MESSAGE_FLOAT, 1, "white");
    }else
    {
        edje_object_signal_emit(inst->tageslosung_obj, "black", "");
        edje_object_message_send(inst->tageslosung_obj, EDJE_MESSAGE_FLOAT, 2, "black");
    }
        
    }
}

static Config_Item *_tageslosung_config_item_get(const char *id) {
    Config_Item *ci;
    char buf[128];

    if (!id)  {
        int num = 0;
        if (tageslosung_config->items) {
            const char *p;
            ci = eina_list_last(tageslosung_config->items)->data;
            p  = strrchr(ci->id, '.');
            if (p) num = atoi(p + 1) + 1;
        }
        snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
        id = buf;
    } else {
        Eina_List *l;
        for (l = tageslosung_config->items; l; l = l->next) {
            ci = l->data;
            if (!ci->id) continue;
            if (!strcmp (ci->id, id)) {
                if (!ci->device) ci->device = eina_stringshare_add("tageslosung0");
                return ci;
            }
        }
    }

    ci               = E_NEW(Config_Item, 1);
    ci->id           = eina_stringshare_add(id);
    ci->device       = eina_stringshare_add("tageslosung0");
    ci->poll_time    = 15.0;
    ci->always_text  = 1;
    ci->show_popup   = 0;
    ci->theme        = 1;
    ci->openonline   = 1;
    ci->translation  = 1;
    ci->disable_interval  = 0;

    tageslosung_config->items = eina_list_append(tageslosung_config->items, ci);
    return ci;
}


E_API E_Module_Api e_modapi = 
{ 
  E_MODULE_API_VERSION, "Tageslosung" 
};


E_API void *
e_modapi_init(E_Module *m) 
{
    char buf[PATH_MAX];

    snprintf(buf, sizeof (buf), "%s/locale", m->dir);
    bindtextdomain         (PACKAGE, buf);
    bind_textdomain_codeset(PACKAGE, "UTF-8");

    conf_item_edd = E_CONFIG_DD_NEW("Tageslosung_Config_Item", Config_Item);
    #undef T
    #undef D
    #define T Config_Item
    #define D conf_item_edd
    E_CONFIG_VAL(D, T, id,           STR);
    E_CONFIG_VAL(D, T, device,       STR);
    E_CONFIG_VAL(D, T, poll_time,    DOUBLE);
    E_CONFIG_VAL(D, T, always_text,  INT);
    E_CONFIG_VAL(D, T, show_popup, INT);
    E_CONFIG_VAL(D, T, theme, INT);
    E_CONFIG_VAL(D, T, openonline, INT);
    E_CONFIG_VAL(D, T, translation, INT);
    E_CONFIG_VAL(D, T, disable_interval, INT);

    conf_edd = E_CONFIG_DD_NEW("Tageslosung_Config", Config);
    #undef T
    #undef D
    #define T Config
    #define D conf_edd
    E_CONFIG_LIST(D, T, items, conf_item_edd);

    tageslosung_config = e_config_domain_load("module.tageslosung", conf_edd);
    if (!tageslosung_config) {
        Config_Item *ci;
        tageslosung_config        = E_NEW(Config, 1);
        ci                 = E_NEW(Config_Item, 1);
        ci->id             = eina_stringshare_add("0");
        ci->device         = eina_stringshare_add("tageslosung0");
        ci->poll_time      = 15.0;
        ci->always_text    = 0;
        ci->show_popup     = 1;
        ci->theme          = 1;
        ci->openonline     = 1;
        ci->translation    = 1;
        ci->disable_interval    = 0;
        tageslosung_config->items = eina_list_append(tageslosung_config->items, ci);
    }
    tageslosung_config->mod_dir = eina_stringshare_add(m->dir);
    e_gadcon_provider_register(&_gc_class);
    return m;
}

E_API int e_modapi_shutdown(E_Module *m) {
    e_gadcon_provider_unregister(&_gc_class);

    if (tageslosung_config->config_dialog)
        e_object_del(E_OBJECT(tageslosung_config->config_dialog));

    if (tageslosung_config->menu) {
        e_menu_post_deactivate_callback_set(tageslosung_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(tageslosung_config->menu));
        tageslosung_config->menu = NULL;
    }

    while (tageslosung_config->items) {
        Config_Item *ci = tageslosung_config->items->data;
        tageslosung_config->items = eina_list_remove_list(tageslosung_config->items,
            tageslosung_config->items);
        if (ci->id)     eina_stringshare_del(ci->id);
        if (ci->device) eina_stringshare_del(ci->device);
        E_FREE(ci);
    }

    if (tageslosung_config->mod_dir) eina_stringshare_del(tageslosung_config->mod_dir);
    E_FREE(tageslosung_config);
    E_CONFIG_DD_FREE(conf_item_edd);
    E_CONFIG_DD_FREE(conf_edd);

    return 1;
}

E_API int e_modapi_save (E_Module *m) {
    e_config_domain_save("module.tageslosung", conf_edd, tageslosung_config);
    return 1;
}

static Tageslosung *_tageslosung_new(Evas *evas)
{
    Tageslosung *tageslosung = E_NEW(Tageslosung, 1);
    char  buf[PATH_MAX];

    tageslosung->tageslosung_obj = edje_object_add(evas);
    snprintf(buf, sizeof (buf), "%s/e-module-tageslosung.edj", tageslosung_config->mod_dir);
    if (!e_theme_edje_object_set(tageslosung->tageslosung_obj, "base/theme/modules/tageslosung",
        "modules/tageslosung/main")) edje_object_file_set(tageslosung->tageslosung_obj, buf,
        "modules/tageslosung/main");
    evas_object_show(tageslosung->tageslosung_obj);

    return tageslosung;
}

static void _tageslosung_free(Tageslosung *m) {
   evas_object_del(m->tageslosung_obj);
   E_FREE(m);
}


static Eina_Bool _tageslosung_cb_check(void *data) {
    Instance *inst;
    inst = data;

    
        if(!strcmp(edje_object_part_state_get(inst->tageslosung_obj, "losung", NULL), "default"))
	  {
              
	     edje_object_signal_emit(inst->tageslosung_obj, "active_losung", "");
	     edje_object_signal_emit(inst->tageslosung_obj, "passive_lehrtext", "");
             
	  }
	else          
          {
	     edje_object_signal_emit(inst->tageslosung_obj, "passive_losung", "");
	     edje_object_signal_emit(inst->tageslosung_obj, "active_lehrtext", "");
          }


    return EINA_TRUE;
}

static Eina_Bool _check_date(void *data) {
    Instance *inst;
    inst = data;
    char buf_time[255];

   
   struct tm *local_time, *local_time2;
   time_t current_time, start_time;
   

   current_time = time(NULL);
   local_time = localtime(&current_time);
   strftime (buf_time, sizeof(buf_time), "%d", local_time);

//    printf(" 1 TIMER:\n");
   
   if(strstr(buf_time_old, buf_time) != NULL)
   {
//     printf("2 NICHTS MACHEN AKTUELL: %s\n", buf_time);
//     printf("2 NICHTS MACHEN OLD: %s\n", buf_time);
   }

   if(strstr(buf_time_old, buf_time) == NULL)
   {
    //xml auswerten
//     printf("3 OLD: %s\n", buf_time_old);   
    strftime (buf_time_old, sizeof(buf_time_old), "%d", local_time);
//     printf("3 xml auswerten OLD: %s\n", buf_time_old);   
//     printf("4 xml auswerten AKTUELL: %s\n", buf_time);   
    
//      _xml_parse(inst);
   }
       

//     return EINA_TRUE;
}


static void
_tageslosung_popup_del_cb(void *obj)
{
   _tageslosung_popup_free(e_object_data_get(obj));
}

static void
_tageslosung_popup_comp_del_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
   _tageslosung_popup_free(data);
}

static void
_tageslosung_popup_new(Instance *inst)
{
   Evas *evas;
   Evas_Object *o, *ob, *of;
   E_Zone *zone;
   if (inst->popup) return;

   
   inst->popup = e_gadcon_popup_new(inst->gcc, 0);
   evas = e_comp->evas;
   
    o  = e_widget_list_add(evas, 0, 0);   
    of = e_widget_framelist_add(evas, D_("Losungstext"), 0);
        
//         e_widget_framelist_content_align_set(of, 0, 0);
        ob = e_widget_label_add(evas, inst->losungstext);
        e_widget_framelist_object_append(of, ob);
        
        ob = e_widget_label_add(evas, inst->losungsvers);
        e_widget_framelist_object_append(of, ob);


    e_widget_list_object_append(o, of, 1, 0, 0.0);

    
     of = e_widget_framelist_add(evas, D_("Lehrtext"), 0);
 
//         e_widget_framelist_content_align_set(of, 0.0, 0.0);
        ob = e_widget_label_add(evas, inst->lehrtext);
        e_widget_framelist_object_append(of, ob);
        //e_widget_framelist_object_append_full(of, ob, 1, 1,1, 1, 0, 0, 0, 0, 0, 0);
                   
        ob = e_widget_label_add(evas, inst->lehrtextvers);
        e_widget_framelist_object_append(of, ob);
        //e_widget_framelist_object_append_full(of, ob, 1, 1,1, 1, 0, 0, 0, 0, 0, 0);
 
     e_widget_list_object_append(o, of, 1, 0, 0.0);
                                      
     
   
   e_gadcon_popup_content_set(inst->popup, o);
   e_gadcon_popup_show(inst->popup);
   e_object_data_set(E_OBJECT(inst->popup), inst);
   E_OBJECT_DEL_SET(inst->popup, _tageslosung_popup_del_cb);
}

static void
_tageslosung_popup_free(Instance *inst)
{
   E_FREE_FUNC(inst->popup, e_object_del);
}

static void _tageslosung_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
void *event_info)
{
    Instance *inst = data;
    Evas_Event_Mouse_Down *ev = event_info;
    if (ev->button == 1)
     {
        if (!inst->ci->show_popup)
        {
        if (inst->popup) _tageslosung_popup_free(inst);
        else _tageslosung_popup_new(inst);
        }
     }
    else if (ev->button == 2) 
    {
       if(!strcmp(edje_object_part_state_get(inst->tageslosung_obj, "losung", NULL), "default"))
	  {
	     edje_object_signal_emit(inst->tageslosung_obj, "active_losung", "");
	     edje_object_signal_emit(inst->tageslosung_obj, "passive_lehrtext", "");
	  }
	else          
          {
	     edje_object_signal_emit(inst->tageslosung_obj, "passive_losung", "");
	     edje_object_signal_emit(inst->tageslosung_obj, "active_lehrtext", "");
          }
        
    }
   else if ((ev->button == 3) && (!tageslosung_config->menu)) {
        E_Menu      *m, *mo;
        E_Menu_Item *mi;
        int x, y;

        m  = e_menu_new();
        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _tageslosung_menu_cb_configure, inst);
        
        //cpufreq_config->menu_poll = mo;
        mo  = e_menu_new();
        mi = e_menu_item_new(mo);
        e_menu_item_label_set(mi, inst->losungstext);
        
        mi = e_menu_item_new(mo);
        e_menu_item_label_set(mi, inst->lehrtext);
        //e_menu_item_radio_set(mi, 1);
        //e_menu_item_radio_group_set(mi, 1);
        //if (cpufreq_config->poll_interval <= 4) e_menu_item_toggle_set(mi, 1);
        //e_menu_item_callback_set(mi, _cpufreq_menu_fast, NULL);
        
        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("History"));
        e_util_menu_item_theme_icon_set(mi, "modules/tageslosung/main/tageslosung");
        e_menu_item_submenu_set(mi, mo);
        
        

        m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
        e_menu_post_deactivate_callback_set(m, _tageslosung_menu_cb_post, inst);
        tageslosung_config->menu = m;

        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y,
            NULL, NULL);
        e_menu_activate_mouse(m,
            e_zone_current_get(),
            x + ev->output.x, y + ev->output.y, 1, 1,
            E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
            EVAS_BUTTON_NONE, ev->timestamp, NULL);
    }


}

static void _tageslosung_cb_mouse_in(void *data, Evas *e, Evas_Object *obj,
void *event_info) {
    Instance *inst = data;
    //edje_object_signal_emit(inst->tageslosung_obj, "active_losung", "");
        
    if (inst->ci->show_popup == 1)
    _tageslosung_popup_new(inst);
}

static void _tageslosung_cb_mouse_out(void *data, Evas *e, Evas_Object *obj,
void *event_info) {
    Instance *inst = data;
    
    //edje_object_signal_emit(inst->tageslosung_obj, "passive_losung", "");
    
    if ((inst->popup) && (inst->ci->show_popup))
    _tageslosung_popup_free(inst);
}

///Datum suchen///


static int
_xml_parse(void *data)
{
   Instance *inst;
   char *needle;
   char datum[256];
   char buf[1024];
   char buf1[1024];
   char buf2[64];
   char buf_time[255];
   
   struct tm *local_time, *local_time2;
   time_t current_time, start_time;
   
   inst = data;

   current_time = time(NULL);
   local_time = localtime(&current_time);
   strftime (buf_time, sizeof(buf_time), "%Y-%m-%d", local_time);
   
   FILE *fp;
   FILE *fp1;
   // Datei oeffnen
   fp = fopen("/home/simon/efl_src/tageslosung/losungen2017.xml", "r");
   fp1 = fopen("/home/simon/efl_src/tageslosung/losungen2017.xml", "r");

   if(fp == NULL) 
   {
	e_util_dialog_show("Dateifehler","Datei \"losungen2016.xml\" konnte NICHT geoeffnet werden.\n");
        return 0;
   }


   
#define BUF 512
#define BUF1 512
  char puffer[BUF];
  char puffer1[BUF1];
  int counter = 1;
  int counter1 = 1;

  int counter2 = 1;

  free(inst->losungstext);
  free(inst->losungsvers);
  free(inst->lehrtext);
  free(inst->lehrtextvers);
  /////////////////////////////////////
   Eina_List *Losungs_List = NULL;
   Eina_List *Losungs_Item_List = NULL;
   
   EINA_LIST_FREE(Losungs_List, data) eina_stringshare_del(data);
   eina_list_free(Losungs_List);
//  
   
    while( fgets(puffer1, BUF1, fp1) != NULL ) 
     {
//         while(counter1 <= 3297){
        //if(counter > 2)
            Losungs_List = eina_list_append(Losungs_List, eina_stringshare_add(puffer1)); 
         //if(strstr(puffer, buf_time) != 0)
//             counter1++;
//         }
     }
    
  while( fgets(puffer, BUF, fp) != NULL ) 
   {
       
       if(strstr(puffer, buf_time) != 0)
       {
         counter++;
       }
       if(counter >= 2 && counter <= 8)
       {
         switch(counter) {
                case 5: inst->losungstext = strdup(puffer);
                        //edje_object_part_text_set(inst->tageslosung_obj, "losung", puffer);
                    break;
                case 6: inst->losungsvers = strdup(puffer); 
                        //edje_object_part_text_set(inst->tageslosung_obj, "losungsvers", puffer);
                    break;
                case 7: inst->lehrtext = strdup(puffer);
                        //edje_object_part_text_set(inst->tageslosung_obj, "lehrtext", puffer);
                    break;
                case 8: inst->lehrtextvers = strdup(puffer); 
                        //edje_object_part_text_set(inst->tageslosung_obj, "lehrtextvers", puffer);
                    break;
                 default: 
                    break;
                    }
         
       counter++;
       }
       
   }
   if(counter == 1)
   {
        edje_object_part_text_set(inst->tageslosung_obj, "losung", "Fehler beim auswerten der xml Datei"); 
        edje_object_part_text_set(inst->tageslosung_obj, "lehrtext", "Fehler beim auswerten der xml Datei");
   }
   else
   {
         
         char buffer[PATH_MAX];
         char *buffer1;
         char buf[4096];
         int i=1;
         
         while(inst->losungstext[0] == ' ') 
                {
                    inst->losungstext ++;
                }
                
         snprintf(buffer, sizeof(buffer), "<Losungstext>%s</Losungstext><br><Losungsvers>%s</Losungsvers>", inst->losungstext, inst->losungsvers);
         edje_object_part_text_set(inst->tageslosung_obj, "losung", buffer);

         while(inst->lehrtext[0] == ' ') 
                {
                    inst->lehrtext ++;
                }
                         
//          printf("LOSUNGSTEXT %s\n", inst->losungstext);
//          printf("LEHRTEXT %s\n", inst->lehrtext);
         snprintf(buffer, sizeof(buffer), "<Lehrtext>%s</Lehrtext><br><Lehrtextvers>%s</Lehrtextvers>", inst->lehrtext, inst->lehrtextvers);
         edje_object_part_text_set(inst->tageslosung_obj, "lehrtext", buffer);
   }
       
                
	fclose(fp);
        
//////////////////////////////////
   // Declaring the list
//    Eina_List *list = NULL;
//    // Eina_List in which to place the elements or lists
      Eina_List *l;
//  
//    void *list_data;
//  
//    list = eina_list_append(list, eina_stringshare_add("Bertrand"));
//    list = eina_list_append(list, eina_stringshare_add("Cedric"));
//    list = eina_list_append(list, eina_stringshare_add("Nicolas"));
//    list = eina_list_append(list, eina_stringshare_add("Vincent"));
//    list = eina_list_append(list, eina_stringshare_add("Raoul"));
//    list = eina_list_append(list, eina_stringshare_add("Fabien"));
//    list = eina_list_append(list, eina_stringshare_add("Philippe"));
//    list = eina_list_append(list, eina_stringshare_add("billiob"));
//  
    
// printf("List size: %d\n", eina_list_count(Losungs_List));

//      printf("LISTE AUSGABE:\n");
     
//     Eina_List *list;
//Eina_List *l;
Eina_List *l_next;
char *list_data;
 
 
// Using EINA_LIST_FOREACH_SAFE to free the elements that match "enlightenment"
/* 
EINA_LIST_FOREACH(Losungs_List, l, list_data)
   printf("%s\n", (char*)list_data);
     */
   /*  
    for(l = Losungs_List; l; l = eina_list_next(l))
    {
        
        printf("%i ", counter2);
       printf("%s\n", eina_list_data_get(l));
     counter2++;
        
    }*/


  // EINA_LIST_FREE(list, list_data)
  //    eina_stringshare_del(list_data);
 
//   return 0;
   
////////////
        
}
 
const char
_losungen_de(void)
{
    
     Ecore_Exe *exe;
          exe = ecore_exe_pipe_run("xdg-open \"http://www.losungen.de\"", ECORE_EXE_USE_SH, NULL);
   
   if (exe > 0)
     {
        ecore_exe_free(exe);
     }
   else
     {
        e_util_dialog_show( D_("Error"), "<hilight>Error when opening your browser.</hilight><br><br>"
              "Tageslosung module uses the <hilight>xdg-open</hilight> script (xdg-utils package) from freedesktop.org<br>"
              "to open urls. The script seems to be present, but maybe it is not"
              "<hilight>configured corectly ?</hilight>");
         return 0;
     }
      return 1; 
}
 
 static const char
_tageslosung_cb_menu_browser_open(void *data)
{
     Instance *inst;
     inst = data;
     
     Ecore_Exe *exe;
     char buf[4096];
     int i=1;
     char *bibelstelle;
     if(!ecore_file_app_installed("xdg-open"))
     {      
       e_util_dialog_show( D_("Error"), "<hilight>xdg-open not found !</hilight><br><vr>"
	      "Tageslosung module uses the xdg-open script from freedesktop.org<br>"
	      "to open urls.<br>"
	      "You need to install the <hilight>xdg-utils package</hilight>, wich includes that script.");
	return 0;
     }
     
    const char *state;
    state = edje_object_part_state_get(inst->tageslosung_obj, "losung", NULL);
    
    
    char bibeltranslation[4];
    
    
 switch (inst->ci->translation)
     {
      case 0:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "LUT");
          break;
      case 1:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "LUT84");
          break;
      case 2:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "ELB");
          break;
      case 3:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "HFA");
          break;
      case 4:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "SLT");
          break;
      case 5:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "NGÜ");
          break;
      case 6:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "GNB");
          break;
      case 7:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "EU");
          break;
      case 8:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "NLB");
          break;
      case 9:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "NeÜ");
          break;
      default:
          snprintf(bibeltranslation, sizeof(bibeltranslation), "LUT");
          break;
     }
     
//         "LUT Luther 1984 Deutsch	AT, NT"
//         "ELB Elberfelder Bibel Deutsch	AT, NT<br>"
//         "HFA Hoffnung für alle Deutsch	AT, NT<br>"
//         "SLT Schlachter 2000 Deutsch	AT, NT<br>"
//         "NGÜ Neue Genfer Übersetzung Deutsch	Psalmen, NT<br>"
//         "GNB Gute Nachricht Bibel Deutsch	AT, NT, Apokryphen<br>"
//         "EU Einheitsübersetzung Deutsch AT, NT, Apokryphen<br>"
//         "NLB Neues Leben. Die Bibel Deutsch	AT, NT<br>"
//         "NeÜ Neue evangelistische Übersetzung Deutsch	AT, NT");

     
    if (!strcmp(state, "default"))
    {
      //Lehrtext
      bibelstelle = strtok(inst->lehrtextvers, ">");
      while(bibelstelle != NULL) 
      {
            if(i == 2)
            {
            snprintf(buf, sizeof(buf), "xdg-open \"http://www.bibleserver.com/text/%s/%s\"", bibeltranslation, bibelstelle);
            }
            i++;
            bibelstelle = strtok(NULL, "<");
      }
    }
    else
    {
      //Losungstext  
      bibelstelle = strtok(inst->losungsvers, ">");
      
      while(bibelstelle != NULL) 
            {
            if(i == 2)
            {
            snprintf(buf, sizeof(buf), "xdg-open \"http://www.bibleserver.com/text/%s/%s\"", bibeltranslation, bibelstelle);
            }
            i++;
            bibelstelle = strtok(NULL, "<");
            }
    }
    printf("ONLINE %s\n", buf);

    //if(inst->ci->openonline)
        exe = ecore_exe_pipe_run(buf, ECORE_EXE_USE_SH, NULL);
   
   if (exe > 0)
     {
        ecore_exe_free(exe);
     }
   else
     {
        e_util_dialog_show( D_("Error"), "<hilight>Error when opening your browser.</hilight><br><br>"
              "Tageslosung module uses the <hilight>xdg-open</hilight> script (xdg-utils package) from freedesktop.org<br>"
              "to open urls. The script seems to be present, but maybe it is not"
              "<hilight>configured corectly ?</hilight>");
         return 0;
     }
      return 1;

    
      
}
