DPI_MODELS += lcd_ili9341

lcd_ili9341_SRCS = lcd/ili9341.cpp

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

ifneq '$(SDL_CFLAGS)$(SDL_LDFLAGS)' ''
lcd_ili9341_CFLAGS += $(SDL_CFLAGS) -D__USE_SDL__
lcd_ili9341_LDFLAGS += $(SDL_LDFLAGS)
endif
