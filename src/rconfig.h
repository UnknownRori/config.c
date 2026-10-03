/*
                             @@....--.......@@@                      
                          @@@.....-............@@                    
                       @...---.....................@                 
                    @........-##..-...............   @@              
                  @........++#++....--.......    ........@           
               @@.........++++++.........     ...........--@         
               @@.    ...++===++..   ..............--.------@        
                @.......+++==++..  ....--..........----------@       
                @--..---#++++++......--------------------+---  @  @  
               @--------##+++##---..-------##---##--+----++- ** ** @ 
             @@---------#######--------+-----###-----+----- ******* @
           @@%%---------#######------------++++-------+-----+ *** @  
         @##%-----------########+---------------+-----+...---+- @    
        @##%%------+----+########+----------....-+-----..----+---@   
      @#-##%#-----+----++--#######-+----....%%%.-+-----+++-------@   
      @##-###-----+----++-----####---+--%%%%....-+-----++++---+--@   
         @###@@--+++----%%%%%%%%%%...-...........-----+++++---+--@   
             @@+@@-+----++-   ++++..............-----++++++--+++--@  
               @+@+------++..  ===.............-----++++++---+++--@  
                @+++---------.................----+++++--+---++++-@  
                @++++-----------............---##+++++---+----++++-@ 
                @++++--+--+++++++.......---####--++@@--+++----@   @  
                 @++++----+++++++++++++####--..........@@@@+---@     
                   @++++--+@@  @@@......###.................@--@     
                      @++@@@@.........######............. ...@-@     
                       @@@..........#########.................@      
                        @..........###########................@      
                       @...........--.########.....-...........@     
                      @...........-..##########....-@-..........@    
                    @....--....--..-###########...--@ @..........@   
                   @......-==######%%############.-@   @@.........@  
                 @.......-=#######%%%#############-@  @..........@   
                @......--#########%%#############%-@ @.........-@    
               @......--.#%%%#####%%%###########%%@@..........-@     

Copyright (c) 2026 UnknownRori
Licensed under the PolyForm Noncommercial License 1.0.0
See LICENSE file in repository root for full terms.
 */

#pragma once

#ifndef RORI_CONFIG_H
#define RORI_CONFIG_H

#include "types.h"
#include <stddef.h>


#ifdef RORI_CONFIG_MALLOC
#error "Not supported yet dummy dumb dumb, I'm lazy"
#endif

#ifdef RORI_CONFIG_REALLOC
#error "Not supported yet dummy dumb dumb, I'm lazy"
#endif

#ifdef RORI_CONFIG_FREE
#error "Not supported yet dummy dumb dumb, I'm lazy"
#endif


typedef struct rori_config_t rori_config_t;

#ifndef RORI_SV_H
typedef struct rori_sv  {
    const char* ptr;
    usize len;
} rori_sv;
rori_sv rori_sv_from_cstr(const char* str);
int rori_sv_cmp_cstr(rori_sv sv, const char* str);
char* rori_sv_to_cstr(rori_sv sv);
#endif // RORI_SV_H

typedef struct rconfig_properties  {
    rori_sv section;
    rori_sv name;
    rori_sv value;
} rconfig_properties;

typedef struct rconfig_section_property_t {
    rori_sv name;
    rori_sv value;
} rconfig_section_property_t;

typedef struct rconfig_section_t {
    rori_sv name;

    rconfig_section_property_t* items;
    usize count;
    usize capacity;
} rconfig_section_t;

typedef struct rori_config_t {
    const char* buffer;

    rconfig_section_t* items;
    usize count;
    usize capacity;
} rori_config_t;

void rconfig_init_default(rori_config_t* self);
bool rconfig_parse_buffer(rori_config_t* self, const char* buffer);
bool rconfig_save_buffer(rori_config_t* self, char* buffer, size_t buffer_size, size_t* written_bytes);
void rconfig_unload(rori_config_t* self);

rconfig_section_t* rconfig_get_or_create_section(rori_config_t* self, const char* section_name);

bool rconfig_get_properties(rori_config_t* self, const char* section, const char* name, rconfig_properties* value);
bool rconfig_get_properties_bool(rori_config_t* self, const char* section, const char* name, bool* value);
bool rconfig_get_properties_i32(rori_config_t* self, const char* section, const char* name, i32* value);
bool rconfig_get_properties_cstr(rori_config_t* self, const char* section, const char* name, char* value, usize max_len);
bool rconfig_get_properties_sv(rori_config_t* self, const char* section, const char* name, rori_sv* value);

bool rconfig_set_properties(rori_config_t* self, const char* section, const char* name, const char* value);
bool rconfig_set_properties_i32(rori_config_t* self, const char* section, const char* name, i32 value); // NOTE: Possible memory leak
bool rconfig_set_properties_bool(rori_config_t* self, const char* section, const char* name, bool value);
bool rconfig_set_properties_cstr(rori_config_t* self, const char* section, const char* name, const char* value); // NOTE : Possible memory leak
bool rconfig_set_properties_sv(rori_config_t* self, const char* section, const char* name, rori_sv* value);


#ifdef RORI_RCONFIG_NO_PREFIX
#define config_init_default rconfig_init_default
#define config_parse_buffer rconfig_parse_buffer
#define config_save_buffer  rconfig_save_buffer
#define config_unload       rconfig_unload

#define config_get_or_create_section rconfig_get_or_create_section

#define config_get_properties      rconfig_get_properties
#define config_get_properties_bool rconfig_get_properties_bool     
#define config_get_properties_i32  rconfig_get_properties_i32      
#define config_get_properties_cstr rconfig_get_properties_cstr     
#define config_get_properties_sv   rconfig_get_properties_sv       
#define config_set_properties      rconfig_set_properties          
#define config_set_properties_i32  rconfig_set_properties_i32      
#define config_set_properties_bool rconfig_set_properties_bool     
#define config_set_properties_cstr rconfig_set_properties_cstr     
#define config_set_properties_sv   rconfig_set_properties_sv       
#endif

#endif // RORI_CONFIG_H
