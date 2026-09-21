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

typedef struct rori_config_t rori_config_t;

#ifndef RORI_SV
typedef struct rori_sv  {
    const char* ptr;
    usize len;
} rori_sv;
rori_sv rori_sv_from_cstr(const char* str);
int rori_sv_cmp_cstr(rori_sv sv, const char* str);
char* rori_sv_to_cstr(rori_sv sv);
#endif

typedef struct rori_config_properties  {
    rori_sv section;
    rori_sv name;
    rori_sv value;
} rori_config_properties;

typedef struct section_property_t {
    rori_sv name;
    rori_sv value;
} section_property_t;

typedef struct section_t {
    rori_sv name;

    section_property_t* items;
    usize count;
    usize capacity;
} section_t;

typedef struct rori_config_t {
    const char* buffer;

    section_t* items;
    usize count;
    usize capacity;
} rori_config_t;

void config_init_default(rori_config_t* self);
bool config_parse_buffer(rori_config_t* self, const char* buffer);
bool config_save_buffer(rori_config_t* self, char* buffer, size_t buffer_size, size_t* written_bytes);
void config_unload(rori_config_t* self);

section_t* config_get_or_create_section(rori_config_t* self, const char* section_name);

bool config_get_properties(rori_config_t* self, const char* section, const char* name, rori_config_properties* value);
bool config_get_properties_bool(rori_config_t* self, const char* section, const char* name, bool* value);
bool config_get_properties_i32(rori_config_t* self, const char* section, const char* name, i32* value);
bool config_get_properties_cstr(rori_config_t* self, const char* section, const char* name, char* value, usize max_len);
bool config_get_properties_sv(rori_config_t* self, const char* section, const char* name, rori_sv* value);

bool config_set_properties(rori_config_t* self, const char* section, const char* name, const char* value);
bool config_set_properties_i32(rori_config_t* self, const char* section, const char* name, i32 value); // NOTE: Possible memory leak
bool config_set_properties_bool(rori_config_t* self, const char* section, const char* name, bool value);
bool config_set_properties_cstr(rori_config_t* self, const char* section, const char* name, const char* value); // NOTE : Possible memory leak
bool config_set_properties_sv(rori_config_t* self, const char* section, const char* name, rori_sv* value);

#endif // RORI_CONFIG_H
