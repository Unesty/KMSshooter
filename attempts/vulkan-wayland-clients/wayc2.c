#include "wayland.c"

int main(int argc, char *argv[])
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

	state.wl_surface = wl_compositor_create_surface(state.wl_compositor);
	state.xdg_surface = xdg_wm_base_get_xdg_surface(state.xdg_wm_base, state.wl_surface);
	xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);
	state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);
	xdg_toplevel_set_title(state.xdg_toplevel, "example client");
	wl_surface_commit(state.wl_surface);

	while (wl_display_dispatch(state.wl_display)) {
		/* This space deliberately left blank */
	}

	return 0;
}
