
#include "../../include/Server.h"

void Processing(void) {
	byte req[AUTH_SIZE];
	RET_IF_VOID(!Global::net->Recv(req, AUTH_SIZE));
	Global::args.ip = Global::net->GetIp();
	/**
	 *	Protocols for screen and for events
	 */
	bool screen = req[2] == 1;
	bool events = req[3] == 1;
	bool sdlevs = req[5] == 1;
	Global::args.dvert = req[4] == 0;

	switch (*req) {
	case 0:
		/**
		 *	Remote control mode
		 */
		Actions::StartStream(req[1], screen, events, sdlevs);
		return;

	default:
		/**
		 *	Completion of work
		 */
		Global::net->Exit();
	}
}

void PaletteGenerating(void) {
	std::ofstream fd(Global::args.path, std::ios::binary | std::ios::trunc);
	ERROR(!fd.is_open(), "Can't open file " + Global::args.path);

	byte buff[TABLE_SIZE], size;

	std::cout << "Info: It may take a couple of minutes to generate "
			  << "the palette, please wait.\n";

	Global::x11->Start(0,  false);
	Global::x11->MakeLinksTable();
	size = (byte)Global::x11->PackLinks(buff);

	fd.write(reinterpret_cast<char *>(&size), 1);
	fd.write(reinterpret_cast<char *>(buff ), size * U32S);
	fd.close();

	std::cout << "Info: Palette generated successfully.\n";

	if (Global::args.cmd == "palette") {
		delete Global::x11;
		exit(0);
	}
}

void ReadPalette(void) {
	std::ifstream fd(Global::args.path, std::ios::binary);
	
	if (!fd.is_open()) {
		std::cout << "Info: The color palette was ignored.\n";
		return;
	}

	byte buff[TABLE_SIZE], size;
	uint32_t reallen;

	fd.read(reinterpret_cast<char *>(&size), 1);
	if ((reallen = size * U32S) == 0) {
		std::cout << "Info: Palette file is empty. Ignore it.\n";
		fd.close();
		return;
	}

	fd.read(reinterpret_cast<char *>(buff), reallen);

	if (fd.gcount() != reallen) {
		std::cout << "Info: The palette file is broken. File"
				  << " ignored.\n";
		fd.close();
		return;
	}

	Global::x11->SetLinks(buff, size);
	fd.close();
}

int main(int argc, char *argv[]) {
	Global::args = Tools::ArgsRead(argc, argv);

	Tools::SetDisplay(Global::args.display);
	Tools::SetXAuth(Global::args.xauth);

	assert(Global::x11 = new X11);
	/**
	 *	Generating and reading a palette. If the argument cmd is
	 *	all then the palette is created with the start of the
	 *	control mode immediately.
	 */
	if (Global::args.cmd != "rat") {
		PaletteGenerating();
	}
	if (Global::args.cmd != "all") {
		ReadPalette();
	}

	if (Global::args.port < 1) {
		std::cout << man_text << "\n";
		return 1;
	}

	Global::net = new TCP(Global::args.port);
	assert(Global::net);
	/**
	 *	Cycle for processing connections to the server. Only 1
	 *	client connects!
	 */
	while (Global::net->Alive()) {
		NEXT_IF(!Global::net->Accept());
		Processing();

		Global::net->CloseConnection();
	}

	// It's over Anakin.
	delete Global::x11;
	delete Global::net;
	return 0;
}