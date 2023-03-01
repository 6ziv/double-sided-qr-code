#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
EM_JS(void, log_critical_impl, (const char* str), {
	console.log(str);
	alert(UTF8ToString(str));
});
EM_JS(void, log_progress_ver_lv_impl, (int ver, int lv), {
	const levels = ['L','M','Q','H'];
	document.getElementById("ver_lv_progress").value = 4 * ver + lv;
	document.getElementById("ver_lv_label").textContent = 'Trying Version ' + ver + ',Level ' + levels[lv];
});
EM_JS(void, log_progress_equations_impl, (int equations, int total), {
	document.getElementById("equations_progress").value = equations;
	document.getElementById("equations_progress").max = total;
	document.getElementById("equations_label").textContent = "" + equations + '/' + total;
});
void log_critical(const char* msg) {
	log_critical_impl(msg);
	emscripten_sleep(0);
}

void log_progress_ver_lv(int ver, int lv) {
	log_progress_ver_lv_impl(ver,lv);
	emscripten_sleep(0);
}

void log_progress_equations(int equations, int total) {
	log_progress_equations_impl(equations, total);
	emscripten_sleep(0);
}
#else
#include <iostream>
#include <boost/format.hpp>
void log_critical(const char* msg) {
	std::cerr << msg << std::endl;
}

void log_progress_ver_lv(int ver, int lv) {
	const char levels[4] = { 'L','M','Q','H' };
	std::cout << boost::format("Trying Version %1%, Level %2%") % ver % levels[lv] << std::endl;
}

void log_progress_equations(int equations, int total) {
	std::cout << boost::format("\rSolving equation %1% / %2%") % equations % total << std::flush;
}
#endif