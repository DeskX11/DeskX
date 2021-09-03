
#include "../../include/Client.h"

int processIsTranslated() {
   int ret = 0;
   size_t size = sizeof(ret);
   if (sysctlbyname("sysctl.proc_translated", &ret, &size, NULL, 0) == -1) 
   {
      if (errno == ENOENT)
         return 0;
      return -1;
   }
   return ret;
}

int main(int argc, char *argv[]) {
	Global::args = Tools::ArgsRead(argc, argv);
	byte request[AUTH_SIZE];
	
	// disable work on apple silicon
	if (processIsTranslated() == 1) {
		return 1;
	}

	if (Global::args.ip.empty() || Global::args.port < 1) {
		std::cout << man_text << "\n";
		return 1;
	}

	Global::net = new TCP(Global::args.port, Global::args.ip);
	assert(Global::net);

	switch (*Global::args.cmd.c_str()) {
	case 'r':
		/**
		 *	Remote control mode
		 */
		Actions::StartStreaming(request);
		break;

	case 'e':
		/**
		 *	Completion of work
		 */
		*request = 2;
		Global::net->Send(request, AUTH_SIZE);

	default: break;
	}

	delete Global::net;
	return 0;
}
