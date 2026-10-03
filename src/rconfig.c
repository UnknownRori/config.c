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


#include "rconfig.h"

#include <ctype.h>
#include <rstb_common.h>
#include <rstb_da.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifndef RORI_SV_H
rori_sv rori_sv_from_cstr(const char* str)
{
    return (rori_sv) {
        .ptr = str,
        .len = strlen(str),
    };
}

rori_sv rori_sv_chop_by_delim  (rori_sv* sv, char delim)
{
    for (size_t i = 0; i < sv->len; i++) {
        if (sv->ptr[i] == delim) {
            rori_sv temp = (rori_sv) {
                .ptr = sv->ptr,
                .len = i,
            };
            sv->ptr  += i + 1;
            sv->len -= i + 1;
            return temp;
        }
    }
    return *sv;
}

int rori_sv_cmp_cstr(rori_sv sv, const char* str)
{
    if (str == NULL) return 1;
    size_t str_len = strlen(str);
    size_t min_len = sv.len < str_len ? sv.len : str_len;
    
    int res = strncmp(sv.ptr, str, min_len);
    if (res != 0) return res;

    if (sv.len < str_len) return -1;
    if (sv.len > str_len) return 1;
    return 0;
}

char* rori_sv_to_cstr(rori_sv sv)
{
    char* buffer = malloc(sv.len + 1);
    if (!buffer) return NULL;
    strncpy(buffer, sv.ptr, sv.len);
    buffer[sv.len + 1] = '\0';
    return buffer;
}
#endif // RORI_SV_H

#define DEFAULT_ALLOC_PROPERTIES_ITEM 16

const static char* true_str = "true";
const static char* false_str = "false";

static bool expect(rori_sv* sv, char c);
static rori_sv skip(rori_sv* sv, size_t count);
static rori_sv skip_until(rori_sv* right, char ch);
static bool parse(rori_config_t* config);
static void parse_skip_comment(rori_sv* buffer);
static bool parse_section(rori_config_t* config, rori_sv* buffer);
static bool parse_section_properties(rori_config_t* config, rori_sv* buffer);

usize rconfig_default_size()
{
    return sizeof(rori_config_t);
}

void rconfig_init_default(rori_config_t* self)
{
    memset(self, 0, sizeof(*self));
    rstb_da_reserve(self, 10);
}

bool rconfig_parse_buffer(rori_config_t* self, const char* buffer)
{
    RORI_ASSERT(self != NULL && "skill issue");
    rconfig_init_default(self);
    self->buffer = buffer;
    return parse(self);
}

bool rconfig_save_buffer(rori_config_t* self, char* buffer, size_t buffer_size, size_t* written_bytes)
{
    RORI_ASSERT(self != NULL && "skill issue");
    RORI_ASSERT(buffer != NULL && "skill issue");
    RORI_ASSERT(written_bytes != NULL && "skill issue");

#define SAFE_WRITE(data, len) \
    do { \
        if (pos + (len) > buffer_size) return false; \
        memcpy(buffer + pos, (data), (len)); \
        pos += (len); \
    } while (0)

#define SAFE_WRITE_CHAR(c) \
    do { \
        if (pos + 1 > buffer_size) return false; \
        buffer[pos++] = (c); \
    } while (0)

#define SAFE_WRITE_STR(sv) \
    SAFE_WRITE((sv).ptr, (sv).len)

    size_t pos = 0;

    for (usize i = 0; i < self->count; i++) {
        rconfig_section_t* section = &self->items[i];

        SAFE_WRITE_CHAR('[');
        SAFE_WRITE_STR(section->name);
        SAFE_WRITE_CHAR(']');
        SAFE_WRITE_CHAR('\n');

        for (usize j = 0; j < section->count; j++) {
            rconfig_section_property_t* prop = &section->items[j];

            SAFE_WRITE_STR(prop->name);
            SAFE_WRITE_CHAR('=');
            SAFE_WRITE_STR(prop->value);
            SAFE_WRITE_CHAR('\n');
        }

        if (i < self->count - 1) {
            SAFE_WRITE_CHAR('\n');
        }
    }

    *written_bytes = pos;

#undef SAFE_WRITE
#undef SAFE_WRITE_CHAR
#undef SAFE_WRITE_STR

    return true;
}

void rconfig_unload(rori_config_t* self)
{
    RORI_ASSERT(self != NULL && "skill issue");
    rstb_da_foreach(rconfig_section_t, section, self) {
        rstb_da_free(section);
    }
    rstb_da_free(self);
}

static rconfig_section_t* find_section(rori_config_t* self, const char* name)
{
    rstb_da_foreach(rconfig_section_t, section, self) {
        if (rori_sv_cmp_cstr(section->name, name) == 0) {
            return section;
        }
    }
    return NULL;
}

bool rconfig_get_properties(rori_config_t* self, const char* section_name, const char* name, rconfig_properties* value)
{
    RORI_ASSERT(self != NULL && "skill issue");
    RORI_ASSERT(section_name != NULL && "skill issue");
    RORI_ASSERT(name != NULL && "skill issue");
    
    rconfig_section_t* sect = find_section(self, section_name);
    if (sect == NULL) return false;

    rstb_da_foreach(rconfig_section_property_t, property, sect) {
        if (rori_sv_cmp_cstr(property->name, name) == 0) {
            if (value != NULL) {
                *value = (rconfig_properties) {
                    .section = sect->name,
                    .name = property->name,
                    .value = property->value,
                };
            }
            return true;
        }
    }
    return false;
}

bool rconfig_get_properties_bool(rori_config_t* self, const char* section, const char* name, bool* value)
{
    RORI_ASSERT(self != NULL && "skill issue");
    RORI_ASSERT(section != NULL && "skill issue");
    RORI_ASSERT(name != NULL && "skill issue");
    
    rconfig_properties property;
    if (!rconfig_get_properties(self, section, name, &property)) return false;
    
    if (property.value.len == 0) return false;

    if (property.value.ptr[0] == '1') {
        if (value) *value = true;
        return true;
    }
    if (property.value.ptr[0] == '0') {
        if (value) *value = false;
        return true;
    }
    if (rori_sv_cmp_cstr(property.value, "true") == 0) {
        if (value) *value = true;
        return true;
    }
    if (rori_sv_cmp_cstr(property.value, "false") == 0) {
        if (value) *value = false;
        return true;
    }
    return false;
}

bool rconfig_get_properties_i32(rori_config_t* self, const char* section, const char* name, i32* value)
{
    RORI_ASSERT(self != NULL && "skill issue");
    rconfig_properties property;
    if (!rconfig_get_properties(self, section, name, &property)) return false;
    
    if (property.value.len == 0) return false;

    char tmp[64];
    size_t copy_len = property.value.len < sizeof(tmp) - 1 ? property.value.len : sizeof(tmp) - 1;
    memcpy(tmp, property.value.ptr, copy_len);
    tmp[copy_len] = '\0';
    
    char* endptr = NULL;
    long val = strtol(tmp, &endptr, 10);
    if (endptr == tmp) return false;

    if (value) *value = (i32)val;
    return true;
}

bool rconfig_get_properties_cstr(rori_config_t* self, const char* section, const char* name, char* value, usize max_len)
{
    RORI_ASSERT(self != NULL && "skill issue");
    if (value == NULL || max_len == 0) return false;

    rconfig_properties property;
    if (!rconfig_get_properties(self, section, name, &property)) return false;

    rori_sv sv = property.value;
    if (sv.len >= 2 && sv.ptr[0] == '"' && sv.ptr[sv.len - 1] == '"') {
        sv.ptr++;
        sv.len -= 2;
    }

    size_t copy_len = sv.len < max_len - 1 ? sv.len : max_len - 1;
    memcpy(value, sv.ptr, copy_len);
    value[copy_len] = '\0';

    return true;
}

bool rconfig_get_properties_sv(rori_config_t* self, const char* section, const char* name, rori_sv* value)
{
    RORI_ASSERT(self != NULL && "skill issue");
    rconfig_properties property;
    if (!rconfig_get_properties(self, section, name, &property)) return false;
    if (value) *value = property.value;
    return true;
}

rconfig_section_t* rconfig_get_or_create_section(rori_config_t* self, const char* section_name)
{
    rconfig_section_t* sect = find_section(self, section_name);
    if (sect != NULL) return sect;

    rconfig_section_t new_section = (rconfig_section_t) {
        .name = rori_sv_from_cstr(section_name),
        .count = 0,
        .items = NULL,
        .capacity = 0,
    };
    rstb_da_reserve(&new_section, DEFAULT_ALLOC_PROPERTIES_ITEM);
    rstb_da_append(self, new_section);
    return &rstb_da_last(self);
}

bool rconfig_set_properties(rori_config_t* self, const char* section_name, const char* name, const char* value)
{
    RORI_ASSERT(self != NULL && "skill issue");
    RORI_ASSERT(section_name != NULL && "skill issue");
    RORI_ASSERT(name != NULL && "skill issue");

    rconfig_section_t* sect = rconfig_get_or_create_section(self, section_name);

    rstb_da_foreach(rconfig_section_property_t, prop, sect) {
        if (rori_sv_cmp_cstr(prop->name, name) == 0) {
            prop->value = rori_sv_from_cstr(value ? value : "");
            return true;
        }
    }

    rconfig_section_property_t new_prop = (rconfig_section_property_t) {
        .name = rori_sv_from_cstr(name),
        .value = rori_sv_from_cstr(value ? value : ""),
    };
    rstb_da_append(sect, new_prop);
    return true;
}

bool rconfig_set_properties_i32(rori_config_t* self, const char* section, const char* name, i32 value)
{
    char* buf = malloc(32); // TODO  Fix this memleak later
    if (buf == NULL) return false;
    snprintf(buf, 32, "%d", value);
    return rconfig_set_properties(self, section, name, buf);
}

bool rconfig_set_properties_bool(rori_config_t* self, const char* section, const char* name, bool value)
{
    return rconfig_set_properties(self, section, name, value ? true_str : false_str);
}

bool rconfig_set_properties_cstr(rori_config_t* self, const char* section, const char* name, const char* value)
{
    return rconfig_set_properties(self, section, name, strdup(value));
}

bool rconfig_set_properties_sv(rori_config_t* self, const char* section, const char* name, rori_sv* value)
{
    RORI_ASSERT(self != NULL && "skill issue");
    RORI_ASSERT(section != NULL && "skill issue");
    RORI_ASSERT(name != NULL && "skill issue");

    rconfig_section_t* sect = rconfig_get_or_create_section(self, section);
    rori_sv val_sv = value ? *value : (rori_sv){0};

    rstb_da_foreach(rconfig_section_property_t, prop, sect) {
        if (rori_sv_cmp_cstr(prop->name, name) == 0) {
            prop->value = val_sv;
            return true;
        }
    }

    rconfig_section_property_t new_prop = (rconfig_section_property_t) {
        .name = rori_sv_from_cstr(name),
        .value = val_sv,
    };
    rstb_da_append(sect, new_prop);
    return true;
}

static bool expect(rori_sv* sv, char c)
{
    if (sv->len <= 0) return false;
    if (sv->ptr[0] == c) {
        skip(sv, 1);
        return true;
    }
    return false;
}

static void skip_whitespace(rori_sv* sv)
{
    RORI_ASSERT(sv != NULL && "are you being silly or dummy dumb dumb?");
    while (sv->len > 0 && isspace((unsigned char)*sv->ptr)) {
        sv->ptr++;
        sv->len--;
    }
}

static void parse_skip_comment(rori_sv* buffer)
{
    if (expect(buffer, ';') || expect(buffer, '#')) {
        skip_until(buffer, '\n');
        if (buffer->len > 0 && buffer->ptr[0] == '\n') {
            skip(buffer, 1);
        }
    }
}

static rori_sv skip(rori_sv* sv, size_t count)
{
    RORI_ASSERT(sv != NULL && "are you being silly or dummy dumb dumb?");
    if (sv->len < count) {
        return (rori_sv) {0};
    }

    rori_sv result = (rori_sv) {
        .ptr = sv->ptr,
        .len = count,
    };
    sv->ptr += count;
    sv->len -= count;
    return result;
}

static rori_sv skip_until(rori_sv* right, char ch)
{
    RORI_ASSERT(right != NULL && "are you being silly or dummy dumb dumb?");
    rori_sv left = *right;
    left.len = 0;
    while (right->len > 0 && *right->ptr != ch) {
        left.len += 1;
        right->ptr++;
        right->len--;
    }
    return left;
}

static bool parse(rori_config_t* config)
{
    rori_sv view = rori_sv_from_cstr(config->buffer);
    while (view.len > 0) {
        skip_whitespace(&view);
        if (view.len == 0) break;

        parse_skip_comment(&view);

        if (parse_section(config, &view)) {
            while (parse_section_properties(config, &view)) {}
        } else {
            view.ptr++;
            view.len--;
        }
    }
    return true;
}


static bool parse_section(rori_config_t* config, rori_sv* buffer)
{
    RORI_ASSERT(config != NULL && "are you being silly or dummy dumb dumb?");
    if (!expect(buffer, '[')) return false;
    rori_sv name = skip_until(buffer, ']');
    if (name.ptr == buffer->ptr) return false;
    skip(buffer, 1);
    rconfig_section_t section = (rconfig_section_t) {
        .name = name,
        .count = 0,
        .items = NULL,
        .capacity = 0,
    };
    rstb_da_reserve(&section, DEFAULT_ALLOC_PROPERTIES_ITEM);
    rstb_da_append(config, section);
    return true;
}
static bool parse_section_properties(rori_config_t* config, rori_sv* buffer)
{
    RORI_ASSERT(config != NULL && "are you being silly or dummy dumb dumb?");
    
    skip_whitespace(buffer);
    parse_skip_comment(buffer);
    if (buffer->len == 0 || buffer->ptr[0] == '[') return false;
    if (config->count == 0) return false;

    rconfig_section_t* section = &rstb_da_last(config);
    rori_sv name = skip_until(buffer, '=');
    if (buffer->len == 0 || buffer->ptr[0] != '=') return false;
    skip(buffer, 1);
    
    rori_sv value = skip_until(buffer, '\n');
    skip_whitespace(&value);
    value = rori_sv_chop_by_delim(&value, ';');
    value = rori_sv_chop_by_delim(&value, '#');
    if (buffer->len > 0 && buffer->ptr[0] == '\n') {
        skip(buffer, 1);
    }

    rconfig_section_property_t property = (rconfig_section_property_t) {
        .name = name,
        .value = value,
    };
    rstb_da_append(section, property);
    return true;
}
