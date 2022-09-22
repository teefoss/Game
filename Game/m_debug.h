//
//  m_debug.h
//  Game
//
//  Created by Thomas Foster on 9/22/22.
//

#ifndef m_debug_h
#define m_debug_h

#include "w_world.h"

void DisplayDebugInfo(world_t * world);

extern bool show_geometry;
extern bool show_world;
extern bool show_debug_info;

extern int frame;
extern int frame_ms;
extern int render_ms;
extern int update_ms;
extern float debug_dt;
extern int debug_hours;
extern int debug_minutes;
extern vec2_t mouse_tile;

#endif /* m_debug_h */
