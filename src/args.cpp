
#include <stdlib.h>
#include <string.h>
#include <macro.hpp>
#include <args.hpp>

args::args(const int argc, char **argv) {
	RET_IF(argc < 2);
	const std::string arg(argv[1]);
	type_ = arg == "server" ? SERVER : (arg == "client" ? CLIENT : UNKNOWN);

	std::pair<std::string, std::string> tmp;
	char *key, *val;

	for (int i = 2; i < argc; i++) {
		NEXT_IF(::strlen(argv[i]) < 4 || ::strncmp("--", argv[i], 2) != 0);

		key = argv[i] + 2;
		val = ::strchr(key, '=');
		NEXT_IF(!val);

		map_.insert(std::make_pair(std::string(key, val - key),
								   std::string(val + 1)));
	}
}

void
args::operator=(const args &arg) {
	map_ = arg.map_;
	type_ = arg.type_;
}

std::string
args::operator[](const std::string &key) const {
	auto it = map_.find(key);
	return it == map_.end() ? std::string() : it->second;
}

bool
args::ok(void) const {
	return type_ != UNKNOWN && !map_.empty();
}

args::type
args::mode(void) const {
	return type_;
}

void
args::print(void) const {
	for (const auto &[key, value] : map_) {
		std::cout << key << ": " << value << "\n";
	}
}

int
args::num(const std::string &key) const {
	auto it = map_.find(key);
	return it == map_.end() ? -1 : ::atoi(it->second.c_str());
}
