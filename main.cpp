#include "reed_solomon_helper.hpp"
#include "build_qr_code.hpp"
#ifdef __EMSCRIPTEN__
const char* target = "qr_code_div";
extern "C" {
void do_search(const char* str1, const char* str2) {
	if (search_answer(str1, str2, target)) {
		emscripten_run_script("Module.on_finished(true)");
	}
	else {
		emscripten_run_script("Module.on_finished(false)");
	}
}
}
#else
#define RUN_DEMO 1
const char* target = "F:\\result.jpg";
#endif

int main()
{
	//Take Version 7-L for example;
	RS::RSinit();
#ifdef RUN_DEMO
	search_answer("https://www.google.com", "https://www.bing.com", target);
	return 0;
#endif
}