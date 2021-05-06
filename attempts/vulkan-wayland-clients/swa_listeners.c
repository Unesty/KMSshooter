#include "globals.h"
#include "vulkan/swapchain.h"

void window_resize(struct swa_window* win, unsigned w, unsigned h)
{

	//dlg_info("resizing to %d %d", w, h);

	if(!state.swapchain) {
		//TODO
		/*
		if(!init_swapchain(state, w, h)) {
			dlg_error("Failed to init swapchain");
			return;
		}
		*/
		if (vulkan_swapchain(state.physicalDevice, state.device, state.surface, &state.swapchain, w, h)) {
		//	printf("not resizing");
			return;
		}

		//vulkan_graphics_pipeline(state.device, &state.graphics_pipeline, &state.renderPass, &state.graphics_pipelineLayout);

		vulkan_framebuffers(state.device, state.swapchain, state.renderPass, &state.n_bufs,
							&state.framebuffers, state.w, state.h);
		//create_buffer(device, physicalDevice, 2*M*sizeof(float), &vertexBuffer, &vertexBufferMemory);

		//vulkan_graphics_commands(state.device, state.graphics_pipeline, state.compute_pipeline, state.renderPass, state.n_bufs,
		//			 state.framebuffers, state.vertexBuffer, &state.commandBuffers, &state.commandPool, &state.compute_pipelineLayout, &state.compute_descriptorSet);
}

void window_close(struct swa_window* win)
{
	//struct state* state = swa_window_get_userdata(win);
	//TODO
	state.run = false;
}

void window_key(struct swa_window* win, const struct swa_key_event* ev)
{
	//struct state* state = swa_window_get_userdata(win);

	if(ev->pressed && ev->keycode == swa_key_escape) {
		dlg_info("Escape pressed, exiting");
		state.run = false;
	}
}

void window_focus(struct swa_window* win, bool gained)
{
	dlg_info("focus %s", gained ? "gained" : "lost");
}

void mouse_move(struct swa_window* win,
					   const struct swa_mouse_move_event* ev)
{
	dlg_info("mouse moved to (%d, %d)", ev->x, ev->y);
}

void mouse_cross(struct swa_window* win,
						const struct swa_mouse_cross_event* ev)
{
	dlg_info("mouse %s at (%d, %d)", ev->entered ? "entered" : "left",
			 ev->x, ev->y);
}

static void mouse_button(struct swa_window* win,
						 const struct swa_mouse_button_event* ev)
{
	dlg_info("mouse button: button = %d %s, pos = (%d, %d)",
			 ev->button, ev->pressed ? "pressed" : "released",
			 ev->x, ev->y);
}

void mouse_wheel(struct swa_window* win,
						float dx, float dy)
{
	dlg_info("mouse wheel: (%f, %f)", dx, dy);
}

void touch_begin(struct swa_window* win,
						const struct swa_touch_event* ev)
{
	dlg_info("touch begin: id = %d, pos = (%d, %d)",
			 ev->id, ev->x, ev->y);

	//showing_keyboard = !showing_keyboard;
	//show_keyboard((struct swa_display*) swa_window_get_userdata(win),
	//	showing_keyboard);
}

void touch_update(struct swa_window* win,
						 const struct swa_touch_event* ev)
{
	dlg_info("touch update: id = %d, pos = (%d, %d)",
			 ev->id, ev->x, ev->y);
}

void touch_end(struct swa_window* win, unsigned id)
{
	dlg_info("touch end: id = %d", id);
}

void touch_cancel(struct swa_window* win)
{
	dlg_info("touch cancel");
}

void window_close(struct swa_window* win)
{
	//struct state* state = swa_window_get_userdata(win);
	//TODO
	state.run = false;
}

void window_key(struct swa_window* win, const struct swa_key_event* ev)
{
	//struct state* state = swa_window_get_userdata(win);

	if(ev->pressed && ev->keycode == swa_key_escape) {
		dlg_info("Escape pressed, exiting");
		state.run = false;
	}
}

void mouse_move(struct swa_window* win,
					   const struct swa_mouse_move_event* ev)
{
	dlg_info("mouse moved to (%d, %d)", ev->x, ev->y);
}

void mouse_cross(struct swa_window* win,
						const struct swa_mouse_cross_event* ev)
{
	dlg_info("mouse %s at (%d, %d)", ev->entered ? "entered" : "left",
			 ev->x, ev->y);
}

void mouse_button(struct swa_window* win,
						 const struct swa_mouse_button_event* ev)
{
	dlg_info("mouse button: button = %d %s, pos = (%d, %d)",
			 ev->button, ev->pressed ? "pressed" : "released",
			 ev->x, ev->y);
}
