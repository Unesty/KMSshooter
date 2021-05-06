#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include "xdg-shell-client-protocol.h"
#include <linux/input.h>
#include <stdlib.h>
/* Shared memory support code */

static void
randname(char *buf)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	long r = ts.tv_nsec;
	for (int i = 0; i < 6; ++i) {
		buf[i] = 'A'+(r&15)+(r&16)*2;
		r >>= 5;
	}
}

static int
create_shm_file(void)
{
	int retries = 100;
	do {
		char name[] = "/wl_shm-XXXXXX";
		randname(name + sizeof(name) - 7);
		--retries;
		int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			shm_unlink(name);
			return fd;
		}
	} while (retries > 0 && errno == EEXIST);
	return -1;
}

static int
allocate_shm_file(size_t size)
{
	int fd = create_shm_file();
	if (fd < 0)
		return -1;
	int ret;
	do {
		ret = ftruncate(fd, size);
	} while (ret < 0 && errno == EINTR);
	if (ret < 0) {
		close(fd);
		return -1;
	}
	return fd;
}

/* Wayland code */
struct client_state {
	/* Globals */
	struct wl_display *wl_display;
	struct wl_registry *wl_registry;
	struct wl_shm *wl_shm;
	struct wl_compositor *wl_compositor;
	struct xdg_wm_base *xdg_wm_base;
	/* Objects */
	struct wl_surface *wl_surface;
	struct wl_callback *callback;
	struct xdg_surface *xdg_surface;
	struct xdg_toplevel *xdg_toplevel;
	/* Input */
	struct wl_seat *seat;
	struct wl_pointer *pointer;
	struct wl_keyboard *keyboard;
	struct wl_touch *touch;
	struct wl_cursor_theme *cursor_theme;
	struct wl_cursor *default_cursor;
	struct wl_surface *cursor_surface;
};

/* Input */

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
					 uint32_t serial, struct wl_surface *surface,
					 wl_fixed_t sx, wl_fixed_t sy)
{
	struct client_state *state = data;
	struct wl_buffer *buffer;
	struct wl_cursor *cursor = state->default_cursor;
	struct wl_cursor_image *image;
	/*
	if (state->fullscreen)
		wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
	else
	*/
	if (cursor) {
		image = state->default_cursor->images[0];
		buffer = wl_cursor_image_get_buffer(image);
		wl_pointer_set_cursor(pointer, serial,
							  state->cursor_surface,
							  image->width/4,
							  image->height/4);
		wl_surface_attach(state->cursor_surface, buffer, image->width, image->height);
		wl_surface_damage(state->cursor_surface, 0, 0, image->width, image->height);
		wl_surface_commit(state->cursor_surface);
	}
	wl_pointer_set_cursor(pointer, serial, state->cursor_surface, 8, 8);
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
					 uint32_t serial, struct wl_surface *surface)
{
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
					  uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
					  uint32_t serial, uint32_t time, uint32_t button,
					  uint32_t button_state)
{
	struct client_state *state = data;

	/*
	if (button == BTN_LEFT && button_state == WL_POINTER_BUTTON_STATE_PRESSED)
		wl_shell_surface_move(state->window->shell_surface,
				      state->seat, serial);

				      */
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
					uint32_t time, uint32_t axis, wl_fixed_t value)
{
}

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static void
touch_handle_down(void *data, struct wl_touch *wl_touch,
				  uint32_t serial, uint32_t time, struct wl_surface *surface,
				  int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
	struct client_state *state = data;

	if (state->xdg_wm_base)
		return;

	xdg_toplevel_move(state->xdg_toplevel, state->seat, serial);
}

static void
touch_handle_up(void *data, struct wl_touch *wl_touch,
				uint32_t serial, uint32_t time, int32_t id)
{
}

static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
					uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
}

static void
touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
}

static void
touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
}

static const struct wl_touch_listener touch_listener = {
	touch_handle_down,
	touch_handle_up,
	touch_handle_motion,
	touch_handle_frame,
	touch_handle_cancel,
};

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
					   uint32_t format, int fd, uint32_t size)
{
	/* Just so we don’t leak the keymap fd */
	//close(fd);
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
					  uint32_t serial, struct wl_surface *surface,
					  struct wl_array *keys)
{
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
					  uint32_t serial, struct wl_surface *surface)
{
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
					uint32_t serial, uint32_t time, uint32_t key,
					uint32_t key_state)
{
	struct client_state *state = data;
	if (key == KEY_F11 && key_state) {
		//toggle_fullscreen(d->window, d->window->fullscreen ^ 1);
		xdg_toplevel_set_fullscreen(state->xdg_toplevel, NULL);
		//xdg_toplevel_unset_fullscreen(state->xdg_toplevel);
	} else if (key == KEY_ESC && key_state) {
		if (state->xdg_toplevel)
			xdg_toplevel_destroy(state->xdg_toplevel);
		if (state->xdg_surface)
			xdg_surface_destroy(state->xdg_surface);
		wl_surface_destroy(state->wl_surface);

		if (state->callback) {
			wl_callback_destroy(state->callback);
			//xdg_toplevel_set_fullscreen(state->xdg_toplevel, NULL);
			//running = 0;
		}
		exit(0);
	}
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
						  uint32_t serial, uint32_t mods_depressed,
						  uint32_t mods_latched, uint32_t mods_locked,
						  uint32_t group)
{
}

static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
						 enum wl_seat_capability caps)
{
	struct client_state *state = data;

	if ((caps & WL_SEAT_CAPABILITY_POINTER) && !state->pointer) {
		state->pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(state->pointer, &pointer_listener, state);
	} else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && state->pointer) {
		wl_pointer_destroy(state->pointer);
		state->pointer = NULL;
	}

	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !state->keyboard) {
		state->keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(state->keyboard, &keyboard_listener, state);
	} else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && state->keyboard) {
		wl_keyboard_destroy(state->keyboard);
		state->keyboard = NULL;
	}

	if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !state->touch) {
		state->touch = wl_seat_get_touch(seat);
		wl_touch_set_user_data(state->touch, state);
		wl_touch_add_listener(state->touch, &touch_listener, state);
	} else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && state->touch) {
		wl_touch_destroy(state->touch);
		state->touch = NULL;
	}
}

static const struct wl_seat_listener seat_listener = {
	seat_handle_capabilities,
};


static void
wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
	/* Sent by the compositor when it's no longer using this buffer */
	wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
	.release = wl_buffer_release,
};

static struct wl_buffer *
draw_frame(struct client_state *state)
{
	const int width = 1366, height = 480;
	int stride = width * 4;
	int size = stride * height;

	int fd = allocate_shm_file(size);
	if (fd == -1) {
		return NULL;
	}

	uint32_t *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		close(fd);
		return NULL;
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(state->wl_shm, fd, size);
	struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	/* Draw checkerboxed background */
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if ((x + y / 8 * 8) % 16 < 8)
				data[y * width + x] = 0xFF666666;
			else
				data[y * width + x] = 0xFFEEEEEE;
		}
	}

	munmap(data, size);
	wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
	return buffer;
}


static void
xdg_surface_configure(void *data,
					  struct xdg_surface *xdg_surface, uint32_t serial)
{
	struct client_state *state = data;
	xdg_surface_ack_configure(xdg_surface, serial);

	struct wl_buffer *buffer = draw_frame(state);
	wl_surface_attach(state->wl_surface, buffer, 0, 0);
	wl_surface_commit(state->wl_surface);
}

const static struct xdg_surface_listener xdg_surface_listener = {
	.configure = xdg_surface_configure,
};

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
	xdg_wm_base_pong(xdg_wm_base, serial);
}

const static struct xdg_wm_base_listener xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

static void
registry_global(void *data, struct wl_registry *wl_registry,
				uint32_t name, const char *interface, uint32_t version)
{
	struct client_state *state = data;
	if (strcmp(interface, wl_shm_interface.name) == 0) {
		state->wl_shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
		state->cursor_theme = wl_cursor_theme_load(NULL, 32, state->wl_shm);
		if (!state->cursor_theme) {
			fprintf(stderr, "unable to load default theme\n");
			return;
		}
		state->default_cursor =
			wl_cursor_theme_get_cursor(state->cursor_theme, "left_ptr");
		if (!state->default_cursor) {
			fprintf(stderr, "unable to load default left pointer\n");
			// TODO: abort ?
		}
		state->cursor_surface = wl_compositor_create_surface(state->wl_compositor);
	} else if (strcmp(interface, wl_compositor_interface.name) == 0) {
		state->wl_compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
	} else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		state->xdg_wm_base = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener, state);
	} else if (strcmp(interface, "wl_seat") == 0) {
		state->seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 1);
		wl_seat_add_listener(state->seat, &seat_listener, state);
	}
}

static void
registry_global_remove(void *data,
					   struct wl_registry *wl_registry, uint32_t name)
{
	/* This space deliberately left blank */
}

const static struct wl_registry_listener wl_registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

int main(void)
{
	struct client_state state = { 0 };
	state.wl_display = wl_display_connect(NULL);
	if(state.wl_display==NULL) {
		write(2,"failed to connect to a wayland server\n",38);
		return 0;
	}
	state.wl_registry = wl_display_get_registry(state.wl_display);
	wl_registry_add_listener(state.wl_registry, &wl_registry_listener, &state);
	wl_display_roundtrip(state.wl_display);

	//state.wl_surface = wl_compositor_create_surface(state.wl_compositor);
	//state.xdg_surface = xdg_wm_base_get_xdg_surface(state.xdg_wm_base, state.wl_surface);
	//xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);
	//state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);
	//xdg_toplevel_set_title(state.xdg_toplevel, "example client");
	//wl_surface_commit(state.wl_surface);

	while (wl_display_dispatch(state.wl_display)) {
		/* This space deliberately left blank */
	}

	return 0;
}