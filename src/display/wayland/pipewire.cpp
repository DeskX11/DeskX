
#include <display/wayland/pipewire.hpp>

namespace display {
namespace {
namespace res {

constexpr auto min = SPA_RECTANGLE(SCR_X_MIN, SCR_Y_MIN);
constexpr auto max = SPA_RECTANGLE(SCR_X_MAX, SCR_Y_MAX);

}

namespace fps {

constexpr auto min = SPA_FRACTION( 0, 1);
constexpr auto max = SPA_FRACTION(80, 1);

}

void
screen(void *data) {
	pipewire::cbdata *ptr = reinterpret_cast<pipewire::cbdata *>(data);
	pw_buffer *b;

	RET_IF(ptr->error);
	if ((b = ::pw_stream_dequeue_buffer(ptr->stream)) == nullptr) {
		INFO(WARN"Pipewire out of buffer");
		return;
	}

	spa_buffer *buff = b->buffer;
	RET_IF(!buff->datas[0].data);

	ptr->start = true;
	ptr->pixs  = buff->datas[0].data;
	::pw_stream_queue_buffer(ptr->stream, b);
}

void
info(void *data, uint32_t id, const spa_pod *param) {
	pipewire::cbdata *ptr = reinterpret_cast<pipewire::cbdata *>(data);
	spa_video_info format;

	RET_IF(ptr->error);
	if (param == nullptr || id != SPA_PARAM_Format) {
		INFO(WARN"Screen's info call failed");
		return;
	}
	if (::spa_format_parse(param, &format.media_type,
						   &format.media_subtype) < 0) {
		INFO(WARN"Can't get screen format info");
		ptr->error = true;
		return;
	}
	if (::spa_format_video_raw_parse(param, &format.info.raw) < 0) {
		INFO(WARN"Can't get parse screen format info");
		ptr->error = true;
		return;
	}

	ptr->scr.width  = format.info.raw.size.width;
	ptr->scr.height = format.info.raw.size.height;
	switch (format.info.raw.format) {
	case SPA_VIDEO_FORMAT_RGB:	ptr->scr.format = RGB;
								ptr->scr.shift  = 3;
								return;
	case SPA_VIDEO_FORMAT_BGRx:
	case SPA_VIDEO_FORMAT_BGRA:
	case SPA_VIDEO_FORMAT_RGBx:
	case SPA_VIDEO_FORMAT_RGBA:	ptr->scr.format = RGBA;
								ptr->scr.shift  = 4;
								return;
	default:					return;
	}
}

void
state(void *data, pw_stream_state old, pw_stream_state state, const char *error) {
	pipewire::cbdata *ptr = reinterpret_cast<pipewire::cbdata *>(data);
	RET_IF(!ptr->error && !error);
	INFO(WARN"Pipewire status: " + std::string(pw_stream_state_as_string(state)) +
			 ", details: " + (error ? std::string(error) : std::string()));
	ptr->error = true;
}

constexpr int sflags = PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS;
const pw_stream_events cbs = {
	PW_VERSION_STREAM_EVENTS,
	.state_changed = state, .param_changed = info, .process = screen,
};

}

pipewire::pipewire(const int &fd) {
	RET_IF(fd < 0);

	pw_init(0, nullptr);
	loop = ::pw_main_loop_new(nullptr);
	DIE(!loop);

	auto props = ::pw_properties_new(PW_KEY_CORE_DAEMON, nullptr, nullptr);
	context = ::pw_context_new(::pw_main_loop_get_loop(loop), props, 0);
	DIE(!context);
	core = ::pw_context_connect_fd(context, fd, nullptr, 0);
	DIE(!core);
}

pipewire::~pipewire(void) {
	RET_IF(!loop);
	if (stream) {
		::pw_stream_disconnect(stream);
		::pw_stream_destroy(stream);
		::pw_core_disconnect(core);
	}
	::pw_context_destroy(context);
	::pw_main_loop_quit(loop);
	::pw_main_loop_destroy(loop);
	::pw_deinit();
}

pipewire::screen
pipewire::connect(void) {
	RET_IF(!loop, data.scr);

	byte buffer[4096];
	auto props = ::pw_properties_new(PW_KEY_MEDIA_TYPE, "Video",
									 PW_KEY_MEDIA_CATEGORY, "Capture",
									 PW_KEY_MEDIA_ROLE, "Screen", nullptr);
	stream = ::pw_stream_new(core, "DeskX Stream", props);
	DIE(!stream);
	void *ptr = reinterpret_cast<void *>(&data);
	data.stream = stream;

	::pw_stream_add_listener(stream, &listener, &cbs, ptr);
	auto b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	const spa_pod *params = reinterpret_cast<spa_pod *>(spa_pod_builder_add_object(&b,
		SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
		SPA_FORMAT_mediaType,		SPA_POD_Id(SPA_MEDIA_TYPE_video),
		SPA_FORMAT_mediaSubtype,	SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
		SPA_FORMAT_VIDEO_format,	SPA_POD_CHOICE_ENUM_Id(6,
										SPA_VIDEO_FORMAT_RGB,
										SPA_VIDEO_FORMAT_RGB,
										SPA_VIDEO_FORMAT_RGBA,
										SPA_VIDEO_FORMAT_RGBx,
										SPA_VIDEO_FORMAT_BGRx,
										SPA_VIDEO_FORMAT_BGRA),
		SPA_FORMAT_VIDEO_size,		SPA_POD_CHOICE_RANGE_Rectangle(
										&res::min,
										&res::min,
										&res::max),
		SPA_FORMAT_VIDEO_framerate,	SPA_POD_CHOICE_RANGE_Fraction(
										&fps::min,
										&fps::min,
										&fps::max)
		));
	::pw_stream_connect(stream, PW_DIRECTION_INPUT, PW_ID_ANY,
						static_cast<pw_stream_flags>(sflags), &params, 1);
	pw_loop_enter(::pw_main_loop_get_loop(loop));

	while (!data.start) {
		RET_IF(data.error, data.scr);
		pw_loop_iterate(::pw_main_loop_get_loop(loop), -1);
	}

	return data.scr;
}

void *
pipewire::next(void) {
	pw_loop_iterate(::pw_main_loop_get_loop(loop), -1);
	return data.pixs;
}

}