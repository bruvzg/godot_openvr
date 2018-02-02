/**
  simple blit shader
**/

#ifndef BLIT_SHADER_H
#define BLIT_SHADER_H

#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


void blit_render(GLuint p_texture_id);
void blit_shader_init();
void blit_shader_cleanup();

#ifdef __cplusplus
}
#endif

#endif /* BLIT_SHADER_H */