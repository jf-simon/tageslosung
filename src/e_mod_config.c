#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data {
    const char *device;
    double     poll_time;
    int        always_text;
    int        show_popup;
    int        theme;
    int        openonline;
    int        translation;
    int        disable_interval;
  struct
   {
      Evas_Object *disable;
   } ui;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data   (E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data   (Config_Item     *ci,  E_Config_Dialog_Data *cfdata);

static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data    (E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);


void _config_tageslosung_module(Config_Item *ci) {
    E_Config_Dialog      *cfd = NULL;
    E_Config_Dialog_View *v   = NULL;

    char buf[PATH_MAX];

    if (e_config_dialog_find("Tageslosung", "_e_modules_tageslosung_config_dialog"))
        return;

    v = E_NEW(E_Config_Dialog_View, 1);
    v->create_cfdata        = _create_data;
    v->free_cfdata          = _free_data;
    v->basic.apply_cfdata   = _basic_apply_data;
    v->basic.create_widgets = _basic_create_widgets;

    snprintf(buf, sizeof(buf), "%s/e-module-tageslosung.edj", tageslosung_config->mod_dir);


    cfd = e_config_dialog_new(NULL, D_("Tageslosung Settings"),
        "Tageslosung", "_e_modules_tageslosung_config_dialog", buf, 0, v, ci);
    tageslosung_config->config_dialog = cfd;
}

static void _fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata) {
    Eina_List *l;
    char      *tmp;
    int        i = 0;

    cfdata->poll_time    = ci->poll_time;
    cfdata->always_text  = ci->always_text;
    cfdata->show_popup = ci->show_popup;
    cfdata->theme = ci->theme;
    cfdata->openonline = ci->openonline;
    cfdata->translation = ci->translation;
    cfdata->disable_interval = ci->disable_interval;


}

static void *_create_data(E_Config_Dialog *cfd) {
    E_Config_Dialog_Data *cfdata = E_NEW(E_Config_Dialog_Data, 1);
    Config_Item          *ci     = cfd->data;
    _fill_data(ci, cfdata);
    return cfdata;
}

static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) {
    const char *data;

    if (!tageslosung_config) return;
    tageslosung_config->config_dialog = NULL;


    E_FREE(cfdata);
}



static void
_cb_show_interval_changed(void *data, Evas_Object *obj EINA_UNUSED)
{
   E_Config_Dialog_Data *cfdata = data;
   Eina_Bool disable_interval = cfdata->disable_interval;
   e_widget_disabled_set(cfdata->ui.disable, disable_interval);
}


static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas,
E_Config_Dialog_Data *cfdata) {
    Evas_Object   *o, *o1, *of, *ob, *otb;
    E_Radio_Group *rg;
    Eina_List    *l;
    char         *tmp;
    int           i = 0;
    
otb = e_widget_toolbook_add(evas, (48 * e_scale), (48 * e_scale));
   
    

/// SETTINGS
    o  = e_widget_list_add(evas, 0, 0);
    of = e_widget_framelist_add(evas, D_("Popup"), 0);
        
    rg = e_widget_radio_group_new(&(cfdata->show_popup));
    ob = e_widget_radio_add(evas, D_("On Mouse Over"), 1, rg);
    e_widget_framelist_object_append(of, ob);
 
    ob = e_widget_radio_add(evas, D_("On Mouse Klick"), 0, rg);
    e_widget_framelist_object_append(of, ob); 
    
    ob = e_widget_radio_add(evas, D_("No Popup"), 2, rg);
    e_widget_framelist_object_append(of, ob);
    

    e_widget_list_object_append(o, of, 1, 1, 0.5);    

    e_widget_toolbook_page_append(otb, NULL, D_("Popup"), o, 1, 0, 1, 0,
                                 0.5, 0.0);   
////////////////////////////////////////////////////////////////////////    
    o  = e_widget_list_add(evas, 0, 0);
    of = e_widget_framelist_add(evas, D_("Look"), 0); 
    

    rg = e_widget_radio_group_new(&(cfdata->theme));
    ob = e_widget_radio_add(evas, D_("Dark Theme"), 1, rg);
    e_widget_framelist_object_append(of, ob);
 
    ob = e_widget_radio_add(evas, D_("White Theme"), 0, rg);
    e_widget_framelist_object_append(of, ob);
    
    ob = e_widget_image_add_from_file(evas, "/home/simon/efl_src/tageslosung/images/module_icon1.png", 100, 100);
    e_widget_framelist_object_append(of, ob);
   

    e_widget_list_object_append(o, of, 1, 1, 0.5);
    
   e_widget_toolbook_page_append(otb, NULL, D_("Look"), o, 1, 0, 1, 0,
                                 0.5, 0.0);
   
 ////////////////////////////////////////////////////////////////////////    
     
   
    o  = e_widget_list_add(evas, 0, 0);
    of = e_widget_framelist_add(evas, D_("Online reading"), 0);  
       
    ob = e_widget_check_add(evas, D_("klick on the cross to open on bibelserver.com"),
        &(cfdata->openonline));
    e_widget_framelist_object_append(of, ob);

   
    e_widget_list_object_append(o, of, 1, 1, 0.5);   
    of = e_widget_framelist_add(evas, D_("Übersetungen"), 0);
 

        rg = e_widget_radio_group_new(&(cfdata->translation));        
        ob = e_widget_radio_add(evas, D_("Luther 2017"), 0, rg);
        e_widget_framelist_object_append(of, ob);
        
        ob = e_widget_radio_add(evas, D_("Luther 1984"), 1, rg);
        e_widget_framelist_object_append(of, ob);
 
        ob = e_widget_radio_add(evas, D_("Elberfelder Bibel"), 21, rg);
        e_widget_framelist_object_append(of, ob);
 
        ob = e_widget_radio_add(evas, D_("Hoffnung für alle"), 3, rg);
        e_widget_framelist_object_append(of, ob);
 
        ob = e_widget_radio_add(evas, D_("Schlachter 2000"), 4, rg);
        e_widget_framelist_object_append(of, ob);
 
        ob = e_widget_radio_add(evas, D_("Neue Genfer Übersetzung"), 5, rg);
        e_widget_framelist_object_append(of, ob);
 
        ob = e_widget_radio_add(evas, D_("Gute Nachricht Bibel"), 6, rg);
        e_widget_framelist_object_append(of, ob); 
        
        ob = e_widget_radio_add(evas, D_("Einheitsübersetzung"), 7, rg);
        e_widget_framelist_object_append(of, ob);
         
        ob = e_widget_radio_add(evas, D_("Neues Leben"), 8, rg);
        e_widget_framelist_object_append(of, ob);
         
        ob = e_widget_radio_add(evas, D_("Neue evangelistische Übersetzung"), 9, rg);
        e_widget_framelist_object_append(of, ob);
        
//         ob = e_widget_label_add(evas, "Auflistung der möglichen Übersetungen<br><br>"
//         "LUT Luther 2017 Deutsch	AT, NT<br>"
//         "LUT Luther 1984 Deutsch	AT, NT<br>"
//         "ELB Elberfelder Bibel Deutsch	AT, NT<br>"
//         "HFA Hoffnung für alle Deutsch	AT, NT<br>"
//         "SLT Schlachter 2000 Deutsch	AT, NT<br>"
//         "NGÜ Neue Genfer Übersetzung Deutsch	Psalmen, NT<br>"
//         "GNB Gute Nachricht Bibel Deutsch	AT, NT, Apokryphen<br>"
//         "EU Einheitsübersetzung Deutsch AT, NT, Apokryphen<br>"
//         "NLB Neues Leben. Die Bibel Deutsch	AT, NT<br>"
//         "NeÜ Neue evangelistische Übersetzung Deutsch	AT, NT");
//         e_widget_framelist_object_append(of, ob);
        

    e_widget_list_object_append(o, of, 1, 1, 0.5);   

       e_widget_toolbook_page_append(otb, NULL, D_("Online"), o, 1, 0, 1, 0,
                                 0.5, 0.0);
////////////////////////////////////////////////////////////////////////    
    o  = e_widget_list_add(evas, 0, 0);
        of = e_widget_framelist_add(evas, D_("Switching"), 0);
        
        ob = e_widget_label_add(evas, D_("Time of Switching of \"Losung\" and \"Lehrtext\":"));
    e_widget_framelist_object_append(of, ob);  
        
        ob = e_widget_check_add(evas, D_("Disable Interval"),
        &(cfdata->disable_interval));
    e_widget_on_change_hook_set(ob, _cb_show_interval_changed, cfdata);
    e_widget_framelist_object_append(of, ob);  //  

    e_widget_list_object_append(o, of, 1, 1, 0.5);   


    ob = e_widget_slider_add(evas, 1, 0, D_("%1.0f seconds"),
        5.0, 60.0, 1.0, 0, &(cfdata->poll_time), NULL, 150);
    cfdata->ui.disable = ob;
//
   e_widget_framelist_object_append(of, ob);

    e_widget_list_object_append(o, of, 1, 1, 0.5);
    
    _cb_show_interval_changed(cfdata, NULL);
     
   e_widget_toolbook_page_append(otb, NULL, D_("Switching"), o, 1, 0, 1, 0,
                                 0.5, 0.0);
////////////////////////////////////////////////////////////////////////    
   
    o  = e_widget_list_add(evas, 0, 0);
    
    of = e_widget_framelist_add(evas, D_("Usage"), 0);
 
    
        ob = e_widget_label_add(evas, "Mittler Maustaste auf das Widget um zwischen <br> \"Lehrtext\" und \"Losungstext\" zu wechseln.");
        e_widget_framelist_object_append(of, ob);
        
 
     e_widget_list_object_append(o, of, 1, 1, 0.5);
        e_widget_toolbook_page_append(otb, NULL, D_("Usage"), o, 1, 0, 1, 0,
                                 0.5, 0.0);
////////////////////////////////////////////////////////////////////////    

      o  = e_widget_list_add(evas, 0, 0);   
     of = e_widget_framelist_add(evas, D_("Copyright ©"), 0);
 
    
        ob = e_widget_label_add(evas, "Evangelische Brüder - Unität – Herrnhuter Brüdergemeinde<br>Weitere Informationen finden Sie auf <a href=\"www.herrnhuter.de\" target=new>www.herrnhuter.de<\a> ");
        e_widget_framelist_object_append(of, ob);
        
        ob = e_widget_button_add(evas, "www.losungen.de", NULL,
                           _losungen_de, NULL, NULL);
       e_widget_framelist_object_append(of, ob);
       
       
////////////////////7
       
       ob = e_widget_textblock_add(evas);
       e_widget_size_min_set(ob, 300, 150);
       e_widget_textblock_markup_set(ob, "ads");
       e_widget_framelist_object_append(of, ob);
// EINA_LIST_FOREACH(Losungs_List, l, list_data) snprintf("%s\n", (char*)list_data)
/////////////
       
       
     e_widget_list_object_append(o, of, 1, 1, 0.5);
       e_widget_toolbook_page_append(otb, NULL, D_("Copyright"), o, 1, 0, 1, 0,
                                 0.5, 0.0);
       
       
////////////////////////////////////////////////////////////////////////    

      o  = e_widget_list_add(evas, 0, 0);   
     of = e_widget_framelist_add(evas, D_("Suche ©"), 0);
 
        ob = e_widget_entry_add(evas, NULL, NULL, NULL, NULL);
        //e_widget_size_min_set(ob, 10, 1);
        //e_widget_entry_readonly_set(ob, 1);
        //ob = e_widget_label_add(evas, "Datum auswählen");
        e_widget_framelist_object_append(of, ob);
        
        ob = e_widget_button_add(evas, "suchen", NULL,
                           _losungen_de, NULL, NULL);
       e_widget_framelist_object_append(of, ob);
 /*      
        ob = e_widget_label_add(evas, inst->losungstext);
        e_widget_framelist_object_append(of, ob);
        
        ob = e_widget_label_add(evas, inst->losungstext);
        e_widget_framelist_object_append(of, ob);
     */  
     e_widget_list_object_append(o, of, 1, 1, 0.5);
       e_widget_toolbook_page_append(otb, NULL, D_("Suche"), o, 1, 0, 1, 0,
                                 0.5, 0.0);
       
       
////////////////////////////////////////////////////////////////////////    

       
   e_widget_toolbook_page_show(otb, 0);
   return otb;
    //return o;
}

static int _basic_apply_data(E_Config_Dialog *cfd,
E_Config_Dialog_Data *cfdata) {
    Config_Item *ci;
    char        *tmp;

    ci               = cfd->data;
    ci->poll_time    = cfdata->poll_time;

    ci->always_text  = cfdata->always_text;
    ci->show_popup = cfdata->show_popup;
    ci->theme = cfdata->theme;
    ci->openonline = cfdata->openonline;
    ci->translation = cfdata->translation;
    ci->disable_interval = cfdata->disable_interval;

    e_config_save_queue();
    _tageslosung_config_updated(ci);


    return 1;
}
