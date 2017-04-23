#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

struct _Config {
    const char      *mod_dir;
    E_Config_Dialog *config_dialog;
    E_Menu          *menu;
    Eina_List       *instances, *items;
};

struct _Config_Item {
    const char *id;
    const char *device;
    double      poll_time;
    int         show_popup;
    int         always_text;
    int         theme;
    int         openonline;
    int         translation;
    int         disable_interval;
};

E_API extern E_Module_Api e_modapi;

E_API void *e_modapi_init    (E_Module *m);
E_API int   e_modapi_shutdown(E_Module *m);
E_API int   e_modapi_save    (E_Module *m);

void _tageslosung_config_updated(Config_Item *ci);
void _config_tageslosung_module(Config_Item *ci);
const char _losungen_de(void);

extern Config *tageslosung_config;

#endif
