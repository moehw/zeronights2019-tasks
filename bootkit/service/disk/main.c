#define WIGTH  1024
#define HEIGHT  768

int  hitkey();
int  scancode();

void string(int x, int y, int color, int size, char* text);
void double_glyph(int x, int y, int back_color, int middle_color, int front_color, int size, char symbol);
void glyph(int x, int y, int color, int size, char symbol);

void point(int x, int y, int color);
void point_size(int x, int y, int color, int size);
void full_screen(int color);
void background_gradient(int resolution_x, int resolution_y, int color1, int color2);

void square(int x, int y, int widht, int height, int color);

void string_ex(int x, int y, int back_color, int middle_color, int front_color, int size, char* text);

static char *lfb;

int _start(char* loader_lfb)
{
	int start = 100;
	lfb = loader_lfb;
	int i, j;

	char stick = '_';
	char h_letter = 'h';

	unsigned int y_pos = 200;
	int size = 6;

	background_gradient(WIGTH, HEIGHT, 0x62a7df, 0x0b861a5);
	
	// "ZN2019{8o07k17_1s_jus7_4_pr0gr4m}"
	char* o0 = "o0";

	const char* hint = "Hey, dood, this is intentionally bad code, not for reverse, just run it :)";

	const char* flag_place1 = "ZN2019{";
	char* flag_place2 = "}";

    int back_color   = 0x04fb8eb;
    int middle_color = 0x0bd5ea3;
    int front_color  = 0x0FFFFFF;

	double_glyph(start+100+8*size*7, y_pos, back_color, middle_color, front_color, size, h_letter ^ stick); // 7
	double_glyph(start+100+8*size  , y_pos, back_color, middle_color, front_color, size, '8'); // 8
	double_glyph(start+100+8*size*5, y_pos, back_color, middle_color, front_color, size, h_letter + 3); // k
	double_glyph(start+100+8*size*6, y_pos, back_color, middle_color, front_color, size, 'i'); // i
	string_ex(start+100+8*size*2+10, y_pos, back_color, middle_color, front_color, size-1, o0); // oO

	y_pos += size*10+5*size;

	double_glyph(start+ 100+8*size*8, y_pos-size*10+5*size, back_color, middle_color, front_color, size, stick); // _

	double_glyph(start+100+8*size*4, 200, back_color, middle_color, front_color, size, h_letter ^ stick); // 7

	double_glyph(start+200+8*size*3+20, y_pos+20, back_color, middle_color, front_color, 4, stick); // _
	double_glyph(start+200+8*size*2, y_pos, back_color, middle_color, front_color, size+1, 's'); // s
	double_glyph(start+200+8*size, y_pos, back_color, middle_color, front_color, size, '1'); // 1
	char* part_jus = "jus";
	string_ex(start+200+8*size*4+30, y_pos, back_color, middle_color, front_color, size-1, part_jus); // jus
	double_glyph(start+200+8*size*8, y_pos, back_color, middle_color, front_color, size, stick); // _
	double_glyph(start+200+8*size*7, y_pos, back_color, middle_color, front_color, size, h_letter ^ stick); // 7

	y_pos += size*10+5*size;

	string_ex(start+300+8*size, y_pos+10, back_color, middle_color, front_color, size-1, "4_pr0gr4m"); // 4_pr0gr4m

	string_ex(50, 30, back_color, middle_color, front_color, size, flag_place1);
	string_ex(900, 600, back_color, middle_color, front_color, size, flag_place2);

	while(1) {
	//	if ((hitkey() & 1) == 1)
	//		switch(scancode()) {
	//			case 1: break;
	//		}
	}
}


int hitkey()
{
	asm("xor eax, eax");
	asm("in al, 0x64");
}

int scancode()
{
	asm("xor eax, eax");
	asm("in al, 0x60");
}

void point_size(int x, int y, int color, int size)
{
	square(x, y, size, size, color);
}

void point(int x, int y, int color)
{
	char* local_lfb = lfb;

	int pos = ((y * 1024) + x) * 3;
	local_lfb += pos;
	
	char save_4_byte = *(local_lfb + 3); // save 4th byte
	
	*(int*)local_lfb = *(int*)&color; // change 4 bytes
	
	local_lfb += 3;
	*local_lfb = save_4_byte; // restore 4th byte
}

void double_glyph(int x, int y, int back_color, int middle_color, int front_color, int size, char symbol)
{
	for (int i = 0; i < size; i++) {
		glyph(x + i, y, back_color, size, symbol);
		glyph(x - i, y, middle_color, size, symbol);
	}

	glyph(x, y, front_color, size, symbol);
}

void glyph(int x, int y, int color, int size, char symbol)
{
	char* glyph_table = 0x0A000;

	glyph_table += symbol * 0x10; // 0x10 - size of one glyph, come to the needed one
	
	int temp_x = x;
	for (int i = 0; i < 0x0F; i++) {

		symbol = *glyph_table;
		for (int j = 0; j < 8; j++) {

			if ((symbol >> (7 - j)) & 1) {
				for (int m = 0; m < size; m++)
					for (int n = 0; n < size; n++)
						point(x + n, y + m, color);
			}

			x += size;
		}
		glyph_table++;
		y += size;
		x = temp_x;
	}
}

void full_screen(int color)
{
	for (int i = 0; i < WIGTH * HEIGHT; i++)
		point(i, 0, color);
}

void square(int x, int y, int widht, int height, int color)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < widht; j++)
			point(x + j, y + i, color);
}

void string_ex(int x, int y, int back_color, int middle_color, int front_color, int size, char* text)
{
	int i = 0;
	while (*text) {
		double_glyph(x + i, y, back_color, middle_color, front_color, size, *text);
		i += 8 * size;
		text++;
	}
}

void string(int x, int y, int color, int size, char* text)
{
	int i = 0;
	while (*text) {
		glyph(x + i, y, color, size, *text);
		i += 8 * size;
		text++;
	}
}

void background_gradient(int resolution_x, int resolution_y, int color1, int color2)
{	
	int x=0, y=0;

	unsigned char R1 = ((color1 >> 16) & 0x0FF);
	unsigned char G1 = ((color1 >> 8) & 0x0FF);
	unsigned char B1 = (color1 & 0x0FF);

	unsigned char R2 = ((color2 >> 16) & 0x0FF);
	unsigned char G2 = ((color2 >> 8) & 0x0FF);
	unsigned char B2 = (color2 & 0x0FF);

	char inv_R = R1>R2 ? -1 : 1;
	char inv_G = G1>G2 ? -1 : 1;
	char inv_B = B1>B2 ? -1 : 1;


	int dif_R_y = R1 != R2 ? (resolution_y / (((R1 - R2)*inv_R))) : 1;
	int dif_G_y = G1 != G2 ? (resolution_y / (((G1 - G2)*inv_G))) : 1;
	int dif_B_y = B1 != B2 ? (resolution_y / (((B1 - B2)*inv_B))) : 1;

	while (y != resolution_y) {
		if (!(y % dif_R_y)) color1 += 0x010000 * inv_R;
		if (!(y % dif_G_y)) color1 += 0x0100 * inv_G;
		if (!(y % dif_B_y)) color1 += 0x01 * inv_B;

		while (x != resolution_x) {
			point(x, y, color1);
			x++;
		}

		x = 0;
		y++;
	}
}