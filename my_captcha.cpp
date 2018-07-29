// my_captcha.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifndef cimg_debug
#define cimg_debug 1
#endif
#define cimg_use_png
#include "CImg.h"
#include "png.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

using namespace cimg_library;
using namespace std;


#undef min
#undef max

// Main procedure
//----------------
int main(int argc, char **argv) {

	// Read command line parameters
	//------------------------------
	cimg_usage("Simple captcha generator.");
	const char *file_o = cimg_option("-o", (const char*)0, "Output image file");
	const bool add_border = cimg_option("-b", true, "Add border to captcha image");
	const bool visu = cimg_option("-visu", true, "Enable visualization if no output file");
	
	cimg::srand();
	//объ€вление переменных дл€ генерации слова

	const char * ar[] = { "2","3","4","5","6","7","8","9","a","b","c","d","e","f","g","h","k","m","n","p","q","r","s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","K","L","M","N","P","Q","R","S","T","U","W","X","Y","Z" };
	int n = 6;
	string  s = "";
	char ch_str[80];
	for (int i = 0; i < n; i++) s += ar[rand() % (sizeof ar / sizeof(char *))];
	cout << s << endl;
	strcpy_s(ch_str, s.c_str());
	const char *const captcha_text = ch_str;


	// Create captcha image
	//----------------------

	// Write colored and distorted text
	CImg<unsigned char> captcha(256, 64, 1, 3, 0), color(3), back(3);
	char letter[2] = { 0 };
	//captcha.fill(64);
	for (unsigned int k = 0; k<6; ++k) {
		CImg<unsigned char> tmp;
		*letter = captcha_text[k];

		if (*letter) {
			unsigned char purple[] = { 255,0,255 };

			cimg_forX(color, i) color[i] = (unsigned char)(128 + (std::rand() % 127));
			cimg_forX(back, i) color[i] = (unsigned char)(64);

			tmp.draw_text((int)(2 + 8 * cimg::rand()),
				(int)(12 * cimg::rand()),
				letter, color.data(), 0, 1, std::rand() % 2 ? 38 : 57).resize(-100, -100, 3, 2);//предпоследн€€ цифра больше 2 - двуцветный шрифт;помен€ть последнюю цифру на 1 дл€ светло-голубого, 2- синий, 3 - черный
																								//делает буквы полупрозрачными
			const unsigned int dir = std::rand() % 4, wph = tmp.width() + tmp.height();
			cimg_forXYC(tmp, x, y, v) {
				const int val = dir == 0 ? x + y : (dir == 1 ? x + tmp.height() - y : (dir == 2 ? y + tmp.width() - x :
					tmp.width() - x + tmp.height() - y));
				tmp(x, y, v) = (unsigned char)std::max(0.0f, std::min(255.0f, 1.5f*tmp(x, y, v)*val / wph));
			}
			//if (std::rand() % 2) tmp = (tmp.get_dilate(3) -= tmp);//делает  буквы белыми внутри
			tmp.blur((float)cimg::rand()*0.8f).normalize(0, 255);//размвает буквы
																 //изгибает буквы
			const float sin_offset = (float)cimg::rand(-1, 1) * 3, sin_freq = (float)cimg::rand(-1, 1) / 7;
			cimg_forYC(captcha, y, v) captcha.get_shared_row(y, 0, v).shift((int)(4 * std::cos(y*sin_freq + sin_offset)));
			captcha.draw_image(6 + 40 * k, tmp);
		}
	}

	// Add geometric and random noise
	captcha.noise(40, 4);//создает хороший фон(4-Rician, 2 - Salt&Pepper, 3-Poisson(without sigma))
	captcha.noise(10, 2);//дл€ применени€ вместе с шумом 4; ѕри шуме 4 отключить выделение границ буквы
						 //captcha.noise(45, 2);//можно включить выделение границ букв
	captcha.noise(10, 3);
	CImg<unsigned char> copy = (+captcha).fill(0);
	for (unsigned int l = 0; l<3; ++l) {
		if (l) copy.blur(0.5f).normalize(0, 148);
		for (unsigned int k = 0; k<10; ++k) {
			cimg_forX(color, i) color[i] = (unsigned char)(128 + cimg::rand() * 127);
			if (cimg::rand()<0.5f) copy.draw_circle((int)(cimg::rand()*captcha.width()),
				(int)(cimg::rand()*captcha.height()),
				(int)(cimg::rand() * 30),
				color.data(), 20.0f, ~0U);
			else copy.draw_line((int)(cimg::rand()*captcha.width()),
				(int)(cimg::rand()*captcha.height()),
				(int)(cimg::rand()*captcha.width()),
				(int)(cimg::rand()*captcha.height()),
				color.data(), 20.0f);
		}
	}
	captcha |= copy;



	if (add_border)
		captcha.draw_rectangle(0, 0, captcha.width() - 1, captcha.height() - 1,
			CImg<unsigned char>::vector(255, 255, 255).data(), 1.0f, ~0U);
	captcha = (+captcha).fill(255) - captcha;

	// Write output image and captcha text
	//-------------------------------------
	std::printf("%s\n", captcha_text);
	if (file_o) captcha.save(file_o);
	else if (visu) {
		CImgDisplay disp(CImg<unsigned char>(512, 128, 1, 3, 180).draw_image(128, 32, captcha), captcha_text, 0);
		while (!disp.is_closed() && !disp.key()) { disp.wait(); if (disp.is_resized()) disp.resize(disp).wait(100); }
	}
	return 0;
}


