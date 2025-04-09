#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <graphics.h>		
#include <conio.h>
#include <time.h>
#include <direct.h>
#include <io.h>
#include <sys/timeb.h>

#define IFKEYDOWN(VK_NAME) ((GetAsyncKeyState(VK_NAME) & 0x8000) ? 1 : 0)

#define PI 3.1415926
#define s_x 1000
#define s_y 850
#define kx s_x
#define ky s_y

#define rect_L 30  //y
#define rect_M 30  //x
#define rect_S 24  //z
#define rect_X rect_M
#define rect_Y rect_L
#define rect_Z rect_S
#define roll_step 0.04
#define o_x s_x/2
#define o_y s_y/2
#define o_z 0
#define map_x 20
#define map_y 20
#define uix 200
#define uiy s_y
#define midx o_x
#define midy o_y
#define c_s_x uix+s_x
#define c_s_y s_y

#define time_for_player_move_definemax 7.0

struct point_3d {
	int x;
	int y;
	int z;
};
struct rectangle_head {
	struct my_rectangle* first;
	struct my_rectangle* end;
	int base_x;
	int base_y;
	int base_z;
};
struct my_rectangle {
	struct my_rectangle* next;
	struct point_3d xyz;
};
struct roll_auto_struct {
	double roll_k_range;
	double acc_roll_k;
	int sign_acc_roll_k;
};
struct point_rectangle {
	struct point_3d f_l_d;
	struct point_3d f_l_u;
	struct point_3d f_r_d;
	struct point_3d f_r_u;
	struct point_3d b_l_d;
	struct point_3d b_l_u;
	struct point_3d b_r_d;
	struct point_3d b_r_u;
};
struct global_data {
	int score;
	int private_score;
	double time_for_print_me;
	int time_for_player_move;
	int fuckmex;
	int fuckmey;
	int x_pre;
	int y_pre;
	double roll_i = PI / 2;
	double roll_j = 0;
	double roll_k = PI / 9 * 13;
	int get_treasure;
	int get_treasure_pre;
	char name[99];

	int time_for_message;
	int message_type;
	char message[99];

	int roll_time;
	int roll_de;
};
struct score_data {
	char name[99];
	int score;
};
struct treasure_body {
	struct treasure_body* next;
	struct treasure_body* pre;
	int type;
	int x;
	int y;
	int height;
};
struct treasure_head {
	struct treasure_body* first;
	struct treasure_body* last;
	int if_exposed;
	//int sum;
};


void point_3d_roll(struct point_3d& p, struct global_data& g, double add_roll_k = 0) {
	struct point_3d pp;
	double trk = g.roll_k + add_roll_k;
	pp.x = cos(trk) * p.x - sin(trk) * p.y;
	pp.y = sin(trk) * p.x + cos(trk) * p.y;
	pp.z = p.z;

	p = pp;

	pp.x = cos(g.roll_j) * p.x + sin(g.roll_j) * p.z;
	pp.y = p.y;
	pp.z = -sin(g.roll_j) * p.x + cos(g.roll_j) * p.z;

	p = pp;

	pp.x = p.x;
	pp.y = cos(g.roll_i) * p.y - sin(g.roll_i) * p.z;
	pp.z = sin(g.roll_i) * p.y + cos(g.roll_i) * p.z;

	p = pp;


}
void print_flower(struct point_3d point_base, struct global_data& g, struct point_rectangle p_r) {
	setlinecolor(RED);
	int basex = point_base.x, basey = point_base.y, basez = point_base.z;
	struct point_3d root_bottom = { rect_X / 2,rect_Y / 2,0 };
	struct point_3d root_top = { rect_X / 2,rect_Y / 2,rect_Z + 8 };
	//struct point_3d root_top = { rect_X / 2 +  ,rect_Y / 2,rect_Z };
	point_3d_roll(root_bottom, g);
	point_3d_roll(root_top, g);
	POINT pts_root[] = {
				{basex + root_bottom.x - 3,basey + root_bottom.y},
				{basex + root_bottom.x + 3,basey + root_bottom.y},
				{basex + root_top.x,basey + root_top.y}
	};
	setfillcolor(GREEN);
	fillpolygon(pts_root, 3);

	

	struct point_3d mid = { rect_X / 2,rect_X / 2,0 };
	point_3d_roll(mid, g);
	setfillcolor(RED);
	double tr = 0;
	int i = 0;
	for (tr=0,i=0;i<6;i++,tr+=PI/3) {
		
		struct point_3d pie_a = {  - 10,4,rect_Z + 4 };
		struct point_3d pie_mid = { -13 ,0,rect_Z + 4 };
		struct point_3d pie_b = { - 10,-4,rect_Z + 4 };
		struct point_3d top = { 0,0,rect_Z + 4 };
		point_3d_roll(pie_a, g, tr);
		point_3d_roll(pie_mid, g, tr);
		point_3d_roll(pie_b, g, tr);
		point_3d_roll(top, g);
		POINT pts_leaves[] = {
					{basex + mid.x + pie_a.x,basey + mid.y + pie_a.y},
					{basex + mid.x + pie_mid.x,basey + mid.y + pie_mid.y},
					{basex + mid.x + pie_b.x,basey + mid.y + pie_b.y},
					{basex + mid.x + top.x,basey + mid.y + top.y}
		};
		solidpolygon(pts_leaves, 4);
	}


	struct point_3d top_b = { rect_X / 2,rect_Y / 2,rect_Z + 5 };
	point_3d_roll(top_b, g);
	setfillcolor(RGB(230, 230, 10));
	fillcircle(basex + top_b.x, basey + top_b.y, 4);
}
void print_a_rectangle(struct point_3d point_base, struct point_rectangle p_r, struct global_data& g) {

	setlinestyle(PS_SOLID);
	
	int basex = point_base.x, basey = point_base.y, basez = point_base.z;

	if (g.roll_k<PI * 2 && g.roll_k>PI) {
		double aa = PI * 2;
		double bb = PI;
		double temp_roll = g.roll_k ;
		int cointa = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa+bb)/2 - aa)*((aa + bb) / 2 - bb)));
		int cointb = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointc = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_front[] = {
			{basex + p_r.f_l_d.x,basey + p_r.f_l_d.y},
			{basex + p_r.f_l_u.x,basey + p_r.f_l_u.y},
			{basex + p_r.f_r_u.x,basey + p_r.f_r_u.y},
			{basex + p_r.f_r_d.x,basey + p_r.f_r_d.y},
		};
		solidpolygon(pts_front, 4);
	}
	if (g.roll_k<3 * PI / 2 && g.roll_k>PI / 2) {
		double aa = 3 * PI / 2;
		double bb = PI / 2;
		double temp_roll = g.roll_k;
		int cointa = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointb = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointc = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_left[] = {
			{basex + p_r.f_l_d.x,basey + p_r.f_l_d.y},
			{basex + p_r.f_l_u.x,basey + p_r.f_l_u.y},
			{basex + p_r.b_l_u.x,basey + p_r.b_l_u.y},
			{basex + p_r.b_l_d.x,basey + p_r.b_l_d.y},
		};
		solidpolygon(pts_left, 4);
	}
	if ((g.roll_k < PI / 2 && g.roll_k > 0) || (g.roll_k<PI * 2 && g.roll_k>PI * 2 - PI / 2)) {
		double aa = PI * 2 - PI / 2;
		double bb = PI * 2 + PI / 2;
		double temp_roll = g.roll_k;
		if (temp_roll < PI * 2 - PI / 2) {
			temp_roll += PI * 2;
		}
		int cointa = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointb = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointc = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_right[] = {
			{basex + p_r.f_r_d.x,basey + p_r.f_r_d.y},
			{basex + p_r.f_r_u.x,basey + p_r.f_r_u.y},
			{basex + p_r.b_r_u.x,basey + p_r.b_r_u.y},
			{basex + p_r.b_r_d.x,basey + p_r.b_r_d.y},
		};
		solidpolygon(pts_right, 4);
	}
	if (g.roll_k < PI && g.roll_k > 0) {
		double aa = PI;
		double bb = 0;
		double temp_roll = g.roll_k;
		int cointa = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointb = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		int cointc = 200 * ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_back[] = {
			{basex + p_r.b_l_d.x,basey + p_r.b_l_d.y},
			{basex + p_r.b_l_u.x,basey + p_r.b_l_u.y},
			{basex + p_r.b_r_u.x,basey + p_r.b_r_u.y},
			{basex + p_r.b_r_d.x,basey + p_r.b_r_d.y},
		};
		solidpolygon(pts_back, 4);
	}
	{
		double temp_roll = g.roll_k;
		int cointa = 150 + int(20 * sin(temp_roll));
		int cointb = 150 + int(20 * sin(temp_roll + 2 * PI / 3));
		int cointc = 150 + int(20 * sin(temp_roll + 4 * PI / 3));
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_top[] = {
			{basex + p_r.f_l_u.x,basey + p_r.f_l_u.y},
			{basex + p_r.f_r_u.x,basey + p_r.f_r_u.y},
			{basex + p_r.b_r_u.x,basey + p_r.b_r_u.y},
			{basex + p_r.b_l_u.x,basey + p_r.b_l_u.y},
		};
		solidpolygon(pts_top, 4);
	}
}
void print_a_rectangle_t(struct point_3d point_base, struct point_rectangle p_r, struct global_data& g, struct treasure_head& head, struct treasure_body* p) {
	setlinestyle(PS_SOLID);
	print_flower(point_base, g, p_r);
	if (head.if_exposed == 1) {
		int basex = point_base.x, basey = point_base.y, basez = point_base.z;
		int height = 60 + 10 * sin(g.time_for_print_me);
		struct point_3d mid = { rect_X / 2,rect_X / 2,height };
		point_3d_roll(mid, g);
		char nametpye[4];
		setbkmode(TRANSPARENT);
		if (p->type == 1) {
			strcpy_s(nametpye, "宝");
			setfillcolor(BLUE);
			settextcolor(YELLOW);
			setlinecolor(YELLOW);
		}
		else if (p->type == 0) {
			strcpy_s(nametpye, "陷");
			setfillcolor(WHITE);
			settextcolor(RED);
			setlinecolor(RED);
		}
		fillcircle(basex + mid.x, basey + mid.y, 12);
		outtextxy(basex + mid.x - 7, basey + mid.y - 7, nametpye);
		setbkmode(OPAQUE);
	}
}
void auto_roll(struct roll_auto_struct& r_a_s,struct global_data& g) {
	if (g.roll_time) {
		if (fabs(r_a_s.acc_roll_k) >= r_a_s.roll_k_range) {
			r_a_s.roll_k_range = 100;
			r_a_s.sign_acc_roll_k *= -1;
		}
		if (fabs(r_a_s.acc_roll_k) <= 0.01 && r_a_s.roll_k_range == 100) {
			r_a_s.roll_k_range = fabs(0.05 + rand() % 20 * 0.001);
		}
		if (fabs(r_a_s.acc_roll_k) >= 0.01) {
			g.roll_de = 1;
		}
		if (fabs(r_a_s.acc_roll_k) <= 0.001 && g.roll_de == 1) {
			g.roll_de = 0;
			g.roll_time--;
		}
		r_a_s.acc_roll_k += r_a_s.sign_acc_roll_k * 0.0003;
		g.roll_k += r_a_s.acc_roll_k;
	}
}
void player_keyboard(int map[map_x][map_y],struct global_data& g) {
	if (IFKEYDOWN('B')) {
		g.fuckmex = map_x - 1;
		g.fuckmey = map_y - 2;
	}
	if (IFKEYDOWN('Q')) {
		g.roll_k -= roll_step;
	}
	if (IFKEYDOWN('E')) {
		g.roll_k += roll_step;
	}
	if (IFKEYDOWN('A') || IFKEYDOWN('D') || IFKEYDOWN('W') || IFKEYDOWN('S')) {
		if (g.time_for_player_move == 0) {
			int move_me_x = 0;
			int move_me_y = 0;
			if (IFKEYDOWN('S')) {
				move_me_x++;
			}
			if (IFKEYDOWN('W')) {
				move_me_x--;
			}
			if (IFKEYDOWN('A')) {
				move_me_y++;
			}
			if (IFKEYDOWN('D')) {
				move_me_y--;
			}
			if (g.roll_k > PI / 4 && g.roll_k < 3 * PI / 4) {

			}
			else if (g.roll_k > 3 * PI / 4 && g.roll_k < 5 * PI / 4) {
				int temp = move_me_y;
				move_me_y = -move_me_x;
				move_me_x = temp;
			}
			else if (g.roll_k > 5 * PI / 4 && g.roll_k < 7 * PI / 4) {
				move_me_y = -move_me_y;
				move_me_x = -move_me_x;
			}
			else if (g.roll_k > 7 * PI / 4 || g.roll_k < PI / 4) {
				int temp = move_me_y;
				move_me_y = move_me_x;
				move_me_x = -temp;
			}
			{
				int move_dest_x = g.fuckmex + move_me_x;
				int move_dest_y = g.fuckmey + move_me_y;
				int if_move = 0;
				if (move_dest_x >= 0 && move_dest_x < map_x && map[move_dest_x][g.fuckmey] == 0) {
					g.x_pre = g.fuckmex;
					g.y_pre = g.fuckmey;
					g.fuckmex = move_dest_x;
					g.time_for_player_move = time_for_player_move_definemax;
					if (move_me_x != 0) if_move = 1;
				}
				if (move_dest_y >= 0 && move_dest_y < map_y && map[g.fuckmex][move_dest_y] == 0 && if_move == 0) {
					g.x_pre = g.fuckmex;
					g.y_pre = g.fuckmey;
					g.fuckmey = move_dest_y;
					g.time_for_player_move = time_for_player_move_definemax;
				}
			}
		}
	}
}
void print_me(struct point_3d point_base, struct global_data& g) {
	if (g.time_for_player_move != 0) g.time_for_player_move--;
	int t_x = rect_M * (g.time_for_player_move / time_for_player_move_definemax) * (g.x_pre - g.fuckmex); //temp_player_move_x
	int t_y = rect_L * (g.time_for_player_move / time_for_player_move_definemax) * (g.y_pre - g.fuckmey); //temp_player_move_y
	int height = 60;
	struct point_3d f_l_d = { 3 + t_x,3 + t_y,-0 };
	struct point_3d f_l_u = { 11 + t_x,11 + t_y,rect_S + height };
	struct point_3d f_r_d = { 3 + t_x,rect_L -3 + t_y,-0 };
	struct point_3d f_r_u = { 11 + t_x,rect_L -11 + t_y,rect_S + height };
	struct point_3d b_l_d = { rect_M - 3 + t_x ,3 + t_y,-0 };
	struct point_3d b_l_u = { rect_M - 11 + t_x ,11 + t_y,rect_S + height };
	struct point_3d b_r_d = { rect_M - 3 + t_x ,rect_L - 3 + t_y,-0 };
	struct point_3d b_r_u = { rect_M - 11 + t_x ,rect_L - 11 + t_y,rect_S + height };
	point_3d_roll(f_l_d,g);
	point_3d_roll(f_l_u,g);
	point_3d_roll(f_r_d,g);
	point_3d_roll(f_r_u,g);
	point_3d_roll(b_l_d,g);
	point_3d_roll(b_l_u,g);
	point_3d_roll(b_r_d,g);
	point_3d_roll(b_r_u,g);

	setlinestyle(PS_SOLID);
	int basex = point_base.x, basey = point_base.y, basez = point_base.z;
	int cr = 200;
	int cg = 190;
	int cb = 0;
	if (g.get_treasure == 1) {
		cr = (13.0) + (200.0 - 13.0) * ((g.time_for_player_move / time_for_player_move_definemax));
		cg = (31.0) + (190.0 - 31.0) * ((g.time_for_player_move / time_for_player_move_definemax));
		cb = (193.0) - (193.0 - 0.0) * ((g.time_for_player_move / time_for_player_move_definemax));
		g.get_treasure_pre = 1;
	}
	if (g.get_treasure == 0 && g.get_treasure_pre == 1) {
		cr = (200.0) - (200.0 - 13.0) * ((g.time_for_player_move / time_for_player_move_definemax));
		cg = (190.0) - (190.0 - 31.0) * ((g.time_for_player_move / time_for_player_move_definemax));
		cb = (0.0) + (193.0 - 0.0) * ((g.time_for_player_move / time_for_player_move_definemax));
		if (cb < 0.01) g.get_treasure_pre = 0;
	}

	if ((g.roll_k<PI * 2 && g.roll_k>PI) && (g.roll_k<3 * PI / 2 && g.roll_k>PI / 2)) {
		{
			//D
			double aa = PI;
			double bb = 0;
			double temp_roll = g.roll_k;
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_back[] = {
				{basex + b_l_d.x,basey + b_l_d.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_back, 4);
		}
		{
			//C
			double aa = PI * 2 - PI / 2;
			double bb = PI * 2 + PI / 2;
			double temp_roll = g.roll_k;
			if (temp_roll < PI * 2 - PI / 2) {
				temp_roll += PI * 2;
			}
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_right[] = {
				{basex + f_r_d.x,basey + f_r_d.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_right, 4);
		}
		{
			//A
			double aa = PI * 2;
			double bb = PI;
			double temp_roll = g.roll_k;
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_front[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + f_r_d.x,basey + f_r_d.y},
			};
			solidpolygon(pts_front, 4);
		}
		{
			//B
			double aa = 3 * PI / 2;
			double bb = PI / 2;
			double temp_roll = g.roll_k;
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_left[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_l_d.x,basey + b_l_d.y},
			};
			solidpolygon(pts_left, 4);
		}
	}
	else if ((g.roll_k<3 * PI / 2 && g.roll_k>PI / 2) && (g.roll_k < PI / 2 && g.roll_k > 0) || (g.roll_k<PI * 2 && g.roll_k>PI * 2 - PI / 2)) {
		{
			//D
			double aa = PI;
			double bb = 0;
			double temp_roll = g.roll_k;
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_back[] = {
				{basex + b_l_d.x,basey + b_l_d.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_back, 4);
		}
		{
			//B
			double aa = 3 * PI / 2;
			double bb = PI / 2;
			double temp_roll = g.roll_k;
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_left[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_l_d.x,basey + b_l_d.y},
			};
			solidpolygon(pts_left, 4);
		}
		{
			//A
			double aa = PI * 2;
			double bb = PI;
			double temp_roll = g.roll_k;
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_front[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + f_r_d.x,basey + f_r_d.y},
			};
			solidpolygon(pts_front, 4);
		}
		{
			//C
			double aa = PI * 2 - PI / 2;
			double bb = PI * 2 + PI / 2;
			double temp_roll = g.roll_k;
			if (temp_roll < PI * 2 - PI / 2) {
				temp_roll += PI * 2;
			}
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_right[] = {
				{basex + f_r_d.x,basey + f_r_d.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_right, 4);
		}
	}
	else if (((g.roll_k < PI / 2 && g.roll_k > 0) || (g.roll_k<PI * 2 && g.roll_k>PI * 2 - PI / 2)) && (g.roll_k < PI && g.roll_k > 0)) {
		{
			//A
			double aa = PI * 2;
			double bb = PI;
			double temp_roll = g.roll_k;
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_front[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + f_r_d.x,basey + f_r_d.y},
			};
			solidpolygon(pts_front, 4);
		}
		{
			//B
			double aa = 3 * PI / 2;
			double bb = PI / 2;
			double temp_roll = g.roll_k;
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_left[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_l_d.x,basey + b_l_d.y},
			};
			solidpolygon(pts_left, 4);
		}
		{
			//C
			double aa = PI * 2 - PI / 2;
			double bb = PI * 2 + PI / 2;
			double temp_roll = g.roll_k;
			if (temp_roll < PI * 2 - PI / 2) {
				temp_roll += PI * 2;
			}
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_right[] = {
				{basex + f_r_d.x,basey + f_r_d.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_right, 4);
		}
		{
			//D
			double aa = PI;
			double bb = 0;
			double temp_roll = g.roll_k;
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_back[] = {
				{basex + b_l_d.x,basey + b_l_d.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_back, 4);
		}
	}
	else {
		{
			//C
			double aa = PI * 2 - PI / 2;
			double bb = PI * 2 + PI / 2;
			double temp_roll = g.roll_k;
			if (temp_roll < PI * 2 - PI / 2) {
				temp_roll += PI * 2;
			}
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_right[] = {
				{basex + f_r_d.x,basey + f_r_d.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_right, 4);
		}
		{
			//A
			double aa = PI * 2;
			double bb = PI;
			double temp_roll = g.roll_k;
			int cointa = cr;
			int cointb = cg;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_front[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + f_r_u.x,basey + f_r_u.y},
				{basex + f_r_d.x,basey + f_r_d.y},
			};
			solidpolygon(pts_front, 4);
		}
		{
			//B
			double aa = 3 * PI / 2;
			double bb = PI / 2;
			double temp_roll = g.roll_k;
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_left[] = {
				{basex + f_l_d.x,basey + f_l_d.y},
				{basex + f_l_u.x,basey + f_l_u.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_l_d.x,basey + b_l_d.y},
			};
			solidpolygon(pts_left, 4);
		}
		{
			//D
			double aa = PI;
			double bb = 0;
			double temp_roll = g.roll_k;
			double color_change = ((temp_roll - aa) * (temp_roll - bb) / (((aa + bb) / 2 - aa) * ((aa + bb) / 2 - bb)));
			int cointa = cr + 30 * color_change * color_change;
			int cointb = cg + 30 * color_change * color_change;
			int cointc = cb;
			COLORREF co = RGB(cointa, cointb, cointc);
			setfillcolor(co);
			setlinecolor(co);
			POINT pts_back[] = {
				{basex + b_l_d.x,basey + b_l_d.y},
				{basex + b_l_u.x,basey + b_l_u.y},
				{basex + b_r_u.x,basey + b_r_u.y},
				{basex + b_r_d.x,basey + b_r_d.y},
			};
			solidpolygon(pts_back, 4);
		}
	}

	{
		double temp_roll = g.roll_k;
		int cointa = 215 * 0.8;
		int cointb = 200 * 0.8;
		int cointc = 10 * 0.8;
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_top[] = {
			{basex + f_l_u.x,basey + f_l_u.y},
			{basex + f_r_u.x,basey + f_r_u.y},
			{basex + b_r_u.x,basey + b_r_u.y},
			{basex + b_l_u.x,basey + b_l_u.y},
		};
		solidpolygon(pts_top, 4);
	}
	
	
	g.time_for_print_me += 0.06;
	if (g.time_for_print_me > PI) g.time_for_print_me = -g.time_for_print_me;
	
	for (int i=5;i<20;i+=3) {
		int move_high = 40 + 10 * sin(g.time_for_print_me+i*PI/12);
		struct point_3d f_l_uu = { i + t_x,i + t_y,rect_S + height + move_high };
		struct point_3d f_r_uu = { i + t_x,rect_L - i + t_y,rect_S + height + move_high };
		struct point_3d b_l_uu = { rect_M - i + t_x ,i + t_y,rect_S + height + move_high };
		struct point_3d b_r_uu = { rect_M - i + t_x ,rect_L - i + t_y,rect_S + height + move_high };
		point_3d_roll(f_l_uu,g);
		point_3d_roll(f_r_uu,g);
		point_3d_roll(b_l_uu,g);
		point_3d_roll(b_r_uu,g);
		double temp_roll = g.roll_k;
		int cointa = 215 * sin(g.time_for_print_me + i * PI / 12);
		int cointb = 215 * sin(g.time_for_print_me + i * PI / 10);
		int cointc = 215 * sin(g.time_for_print_me + i * PI / 8);
		COLORREF co = RGB(cointa, cointb, cointc);
		setfillcolor(co);
		setlinecolor(co);
		POINT pts_top[] = {
			{basex + f_l_uu.x,basey + f_l_uu.y},
			{basex + f_r_uu.x,basey + f_r_uu.y},
			{basex + b_r_uu.x,basey + b_r_uu.y},
			{basex + b_l_uu.x,basey + b_l_uu.y},
		};
		polygon(pts_top, 4);
	}
}
void p_r_roll(struct global_data& g,struct point_rectangle& p_r, int x = 0, int y = 0, int z = 0) {
	int rect_MT = rect_M + x;
	int rect_LT = rect_L + y;
	int rect_ST = rect_S + z;
	p_r.f_l_d = { -0,-0,-0 };
	p_r.f_l_d = { -0,-0,-0 };
	p_r.f_l_u = { -0,-0,rect_ST + 0 };
	p_r.f_r_d = { -0,rect_LT + 0,-0 };
	p_r.f_r_u = { -0,rect_LT + 0,rect_ST + 0 };
	p_r.b_l_d = { rect_MT + 0,-0,-0 };
	p_r.b_l_u = { rect_MT + 0,-0,rect_ST + 0 };
	p_r.b_r_d = { rect_MT + 0,rect_LT + 0,-0 };
	p_r.b_r_u = { rect_MT + 0,rect_LT + 0,rect_ST + 0 };
	point_3d_roll(p_r.f_l_d, g);
	point_3d_roll(p_r.f_l_u, g);
	point_3d_roll(p_r.f_r_d, g);
	point_3d_roll(p_r.f_r_u, g);
	point_3d_roll(p_r.b_l_d, g);
	point_3d_roll(p_r.b_l_u, g);
	point_3d_roll(p_r.b_r_d, g);
	point_3d_roll(p_r.b_r_u, g);
}
void print_temp_int(int if_exist[map_x][map_y],int i,int j, struct global_data& g, struct treasure_head& head) {
	g.get_treasure = 0;
	struct treasure_body* p = head.first;
	while (p != NULL) {
		if (p->x == i && p->y == j) {
			struct point_rectangle p_r;
			int dx = 6;
			int dy = 6;
			int dz = 14;
			p_r_roll(g, p_r, -dx, -dy, -dz);
			struct point_3d point_temp;
			point_temp.x = (i - map_x / 2) * rect_X + dx / 2;
			point_temp.y = (j - map_y / 2) * rect_Y + dx / 2;
			point_temp.z = 0;
			point_3d_roll(point_temp, g);
			point_temp.x += o_x;
			point_temp.y += o_y;
			point_temp.z += o_z;
			print_a_rectangle_t(point_temp, p_r, g, head, p);
			g.get_treasure = 1;
			//g.get_treasure_pre = 1;
			break;
		}
		p = p->next;
	}
	if (if_exist[i][j] == 1 || i == g.fuckmex && j == g.fuckmey) {
		struct point_rectangle p_r;
		p_r_roll(g, p_r, 0, 0, 0);
		struct point_3d point_temp;
		point_temp.x = (i - map_x / 2) * rect_X;
		point_temp.y = (j - map_y / 2) * rect_Y;
		point_temp.z = 0;
		point_3d_roll(point_temp, g);
		point_temp.x += o_x;
		point_temp.y += o_y; 
		point_temp.z += o_z;
		if (i == g.fuckmex && j == g.fuckmey) {
			print_me(point_temp, g);
		}
		else {
			print_a_rectangle(point_temp, p_r, g);
		}
	}
}
void print_map(int if_exist[map_x][map_y], struct global_data& g, struct treasure_head& head) {
	{
		struct point_3d bottom_a = { -rect_M * map_x / 2 + 1,-rect_L * map_y / 2 + 1,0 };
		struct point_3d bottom_b = { rect_M * map_x / 2 - 1,-rect_L * map_y / 2 + 1,0 };
		struct point_3d bottom_c = { rect_M * map_x / 2 - 1,rect_L * map_y / 2 - 1,0 };
		struct point_3d bottom_d = { -rect_M * map_x / 2 + 1,rect_L * map_y / 2 - 1,0 };
		point_3d_roll(bottom_a, g);
		point_3d_roll(bottom_b, g);
		point_3d_roll(bottom_c, g);
		point_3d_roll(bottom_d, g);
		setfillcolor(RGB(250, 250, 250));
		setlinecolor(RGB(250, 250, 250));
		POINT pts_top[] = {
			{o_x + bottom_a.x ,o_y + bottom_a.y },
			{o_x + bottom_b.x ,o_y + bottom_b.y },
			{o_x + bottom_c.x ,o_y + bottom_c.y },
			{o_x + bottom_d.x ,o_y + bottom_d.y },
		};
		solidpolygon(pts_top, 4);
	}
	{
		struct point_3d bottom_a = { -rect_M * map_x / 2 + 1,-rect_L * map_y / 2 + 1,0 };
		struct point_3d bottom_b = { -rect_M * map_x / 2 + rect_M + 1,-rect_L * map_y / 2 + 1,0 };
		struct point_3d bottom_d = { -rect_M * map_x / 2 + 1,-rect_L * map_y / 2 + rect_L + 1,0 };
		struct point_3d bottom_c = { -rect_M * map_x / 2 + rect_M + 1,-rect_L * map_y / 2 + rect_L + 1,0 };
		point_3d_roll(bottom_a, g);
		point_3d_roll(bottom_b, g);
		point_3d_roll(bottom_c, g);
		point_3d_roll(bottom_d, g);
		setfillcolor(RGB(0, 50, 250));
		setlinecolor(RGB(0, 50, 250));
		POINT pts_top[] = {
			{o_x + bottom_a.x ,o_y + bottom_a.y },
			{o_x + bottom_b.x ,o_y + bottom_b.y },
			{o_x + bottom_c.x ,o_y + bottom_c.y },
			{o_x + bottom_d.x ,o_y + bottom_d.y },
		};
		solidpolygon(pts_top, 4);
	}
	{
		struct point_3d bottom_a = { rect_M * map_x / 2 + 1,rect_L * map_y / 2 + 1,0 };
		struct point_3d bottom_b = { rect_M * map_x / 2 - rect_M + 1,rect_L * map_y / 2 + 1,0 };
		struct point_3d bottom_d = { rect_M * map_x / 2 + 1,rect_L * map_y / 2 - rect_L + 1,0 };
		struct point_3d bottom_c = { rect_M * map_x / 2 - rect_M + 1,rect_L * map_y / 2 - rect_L + 1,0 };
		point_3d_roll(bottom_a, g);
		point_3d_roll(bottom_b, g);
		point_3d_roll(bottom_c, g);
		point_3d_roll(bottom_d, g);
		setfillcolor(RGB(250, 50, 0));
		setlinecolor(RGB(250, 50, 0));
		POINT pts_top[] = {
			{o_x + bottom_a.x ,o_y + bottom_a.y },
			{o_x + bottom_b.x ,o_y + bottom_b.y },
			{o_x + bottom_c.x ,o_y + bottom_c.y },
			{o_x + bottom_d.x ,o_y + bottom_d.y },
		};
		solidpolygon(pts_top, 4);
	}
	if (g.roll_k > PI / 2 && g.roll_k < PI) {
		for (int i = 0;i < map_x;i++) {
			for (int j = map_y - 1;j >= 0;j--) {
				print_temp_int(if_exist, i, j, g, head);
			}
		}
	}
	else if (g.roll_k > PI && g.roll_k < PI + PI / 2) {
		for (int i = map_x - 1;i >= 0;i--) {
			for (int j = map_y - 1;j >= 0;j--) {
				print_temp_int(if_exist, i, j, g, head);
			}
		}
	}
	else if (g.roll_k > PI + PI / 2 && g.roll_k < PI * 2) {
		for (int i = map_x - 1;i >= 0;i--) {
			for (int j = 0;j < map_y;j++) {
				print_temp_int(if_exist, i, j, g, head);
			}
		}
	}
	else {
		for (int i = 0;i < map_x;i++) {
			for (int j = 0;j < map_y;j++) {
				print_temp_int(if_exist, i, j, g, head);
			}
		}
	}
}
void roll_angle_ranging(struct global_data& g) {
	{
		if (g.roll_i > PI * 2) {
			g.roll_i -= PI * 2;
		}
		else if (g.roll_i < 0) {
			g.roll_i += PI * 2;
		}
		else if (g.roll_j > PI * 2) {
			g.roll_j -= PI * 2;
		}
		else if (g.roll_j < 0) {
			g.roll_j += PI * 2;
		}
		else if (g.roll_k > PI * 2) {
			g.roll_k -= PI * 2;
		}
		else if (g.roll_k < 0) {
			g.roll_k += PI * 2;
		}
	}
}
int check_map_available(int map[map_x][map_y], int map_right_way[map_x][map_y]) {
	for (int i = 0;i < map_x;i++) {
		for (int j = 0;j < map_y;j++) {
			map_right_way[i][j] = map[i][j];
		}
	}
	map_right_way[0][0] = -1;
	int now_step = -1, if_this_time_moved = 0;
	for (;;) {
		for (int i = 0;i < map_x;i++) {
			for (int j = 0;j < map_y;j++) {
				if (map_right_way[i][j] == now_step) {
					if (i > 0) {
						if (map_right_way[i - 1][j] == 0) {
							map_right_way[i - 1][j] = now_step - 1;
						}
					}
					if (i < map_x - 1) {
						if (map_right_way[i + 1][j] == 0) {
							map_right_way[i + 1][j] = now_step - 1;
						}
					}
					if (j > 0) {
						if (map_right_way[i][j - 1] == 0) {
							map_right_way[i][j - 1] = now_step - 1;
						}
					}
					if (j < map_y - 1) {
						if (map_right_way[i][j + 1] == 0) {
							map_right_way[i][j + 1] = now_step - 1;
						}
					}
					if_this_time_moved = 1;
				}
			}
		}
		//system("cls");
		//for (int j = 0;j < map_y;j++) {
		//	for (int i = 0;i < map_x;i++) {
		//		printf_s("%2d", map_right_way[i][j]);
		//	}
		//	printf_s("\n");
		//}
		//_getch();
		if (map_right_way[map_x - 1][map_y - 1] < 0) {
			if (now_step <= -55) {
				return 1;
			}
			else return -2;
		}
		if (if_this_time_moved) {
			if_this_time_moved = 0;
			now_step--;
		}
		else return -1;
	}
}
void create_map(int map[map_x][map_y]) {
	int map_right_way[map_x][map_y];
	for (;;) {
		for (int i = 0;i < map_x;i++) {
			for (int j = 0;j < map_y;j++) {
				map[i][j] = 0;
			}
		}
		for (int i = 0;i < map_x;i++) {
			for (int j = 0;j < map_y;j++) {
				if (rand() % 15 <= 4)
					map[i][j] = 1;
			}
		}
		map[0][0] = map[map_x-1][map_y-1] = 0;
		int if_map_available_returned = check_map_available(map, map_right_way);
		if (if_map_available_returned == 1) {
			break;
		}
		//else printf_s("%3d", if_map_available_returned);
	}
	//map_dig(map, map_right_way);
}
int check_win(int map[map_x][map_y], struct global_data& g) {
	if (g.fuckmex == map_x - 1 && g.fuckmey == map_y - 1 && g.time_for_player_move == 0) {
		return 1;
	}
	return -1;
}
void print_win(struct global_data& g) {
	settextcolor(WHITE);
	Sleep(200);
	//cleardevice();
	int fill_step = 3;
	setfillcolor(WHITE);
	for (int i = 0;i <= s_y / fill_step;i++) {
		if (i % 9 == 1) Sleep(1);
		solidrectangle(0, i * fill_step, s_x + uix, (i + 1) * fill_step);
		FlushBatchDraw();
	}
	setfillcolor(BLACK);
	for (int i = 0;i <= s_y / fill_step;i++) {
		if (i % 9 == 1) Sleep(1);
		solidrectangle(0, i * fill_step, s_x + uix, (i + 1) * fill_step);
		FlushBatchDraw();
	}
	FlushBatchDraw();
	char a[99];
	sprintf_s(a, "游戏结束！");
	outtextxy(s_x / 2 - 60 + uix / 2, s_y / 2, a);
	sprintf_s(a, "%s，你获得%d分！",g.name,g.score);
	outtextxy(s_x / 2 - 100 + uix / 2, s_y / 2+20, a);
	FlushBatchDraw();
	Sleep(1500);
	if (_kbhit()) {
		_getch();
	}
	_getch();
	FlushBatchDraw();
}
int check_cursor_in_range(int left, int top, int right, int bottom, struct MOUSEMSG& mouseA) {
	if (mouseA.x > left && mouseA.x < right || mouseA.x<left && mouseA.x>right) {
		if (mouseA.y<top && mouseA.y>bottom || mouseA.y > top && mouseA.y < bottom) {
			return 1;
		}
	}
	return -1;
}
void print_bar_menu(int movementy, char texti[], int if_lbutton, MOUSEMSG& mouseA) {
	if (MouseHit()) mouseA = GetMouseMsg();
	setbkmode(TRANSPARENT);
	int midyi = midy - 70;
	if (check_cursor_in_range(midx + uix / 2 - 274, midyi - 54 + movementy, midx + uix / 2 + 286, midyi + 24 + movementy, mouseA) == 1) {
		if (mouseA.mkLButton == 1) {
			setfillcolor(RGB(33,135,135));
		}
		else {
			setfillcolor(YELLOW);
		}
		setlinecolor(WHITE);
		fillroundrect(midx + uix / 2 - 290, midyi - 70 + movementy, midx + uix / 2 + 296 - 3, midyi + 34 + movementy - 2,3,3);
		setfillcolor(BLACK);
		fillroundrect(midx + uix / 2 - 286, midyi - 66 + movementy, midx + uix / 2 + 292 - 3, midyi + 30 + movementy - 2,3,3);
	}
	setfillcolor(RGB(40, 70, 70));
	solidroundrect(midx + uix / 2 - 274, midyi - 54 + movementy, midx + uix / 2 + 286, midyi + 24 + movementy, 5, 5);
	setfillcolor(RGB(50, 90, 90));
	setlinecolor(RGB(70, 160, 160));
	fillroundrect(midx + uix / 2 - 277, midyi - 57 + movementy, midx + uix / 2 + 283, midyi + 22 + movementy, 5, 5);
	setfillcolor(RGB(70, 160, 160));
	setlinecolor(WHITE);
	if (if_lbutton) {
		fillroundrect(midx + uix / 2 - 280, midyi - 60 + movementy, midx + uix / 2 + 280, midyi + 20 + movementy, 5, 5);
	}
	settextcolor(BLACK);
	outtextxy(midx + 7 - ((strlen(texti) - 8) / 4) * 48, midyi - 43 + 3 + movementy, texti);
	if (if_lbutton) settextcolor(WHITE);
	else settextcolor(RGB(200, 200, 200));
	outtextxy(midx + 4 - ((strlen(texti) - 8) / 4) * 48, midyi - 43 + movementy, texti);
	settextcolor(WHITE);
	setbkmode(OPAQUE);
}
int start_menu_return(struct MOUSEMSG mouseA) {
	//mouseA = GetMouseMsg();
	int i;
	int movementy = 130;
	char texti[4][99] = { "开始游戏","查看排名" ,"加载游戏" ,"退出游戏" };
	for (;;) {
		Sleep(1);
		if (MouseHit()) {
			Sleep(1);
			//mouseA = GetMouseMsg();
			if (MouseHit())
				mouseA = GetMouseMsg();
			int temp_check = 0;

			setbkmode(TRANSPARENT);
			LOGFONT point, before;
			gettextstyle(&point);
			before = point;
			point.lfHeight = 90;
			strcpy_s(point.lfFaceName, "黑体");
			point.lfQuality = ANTIALIASED_QUALITY;
			settextstyle(&point);
			char text_title[99] = "走迷宫";
			outtextxy(midx - strlen(text_title) * 11.5 + 30, 75, text_title);
			point.lfHeight = 48;
			settextstyle(&point);

			for (i = 0;i < 4;i++) {
				print_bar_menu(movementy * i, texti[i], 1, mouseA);
			}
			FlushBatchDraw();
			for (temp_check = 0;temp_check <= 3;temp_check++) {
				if (check_cursor_in_range(midx + uix / 2 - 280, midy - 70 - 60 + temp_check * movementy, midx + uix / 2 + 280, midy - 70 + 20 + temp_check * movementy, mouseA) == 1) {
					if (mouseA.mkLButton == 1) {
						clearrectangle(midx + uix / 2 - 280, midy - 70 - 60 + temp_check * movementy, midx + uix / 2 + 280, midy - 70 + 20 + temp_check * movementy);
						print_bar_menu(movementy * temp_check, texti[temp_check], 0, mouseA);
						FlushBatchDraw();
						for (;;) {
							Sleep(1);
							if (!mouseA.mkLButton) {
								settextstyle(&before);
								return temp_check;
							}
							if (MouseHit()) mouseA = GetMouseMsg();
						}
					}
				}
			}
			cleardevice();
		}
	}
}
int if_save() {
	FILE* f;
	HWND wind = GetHWnd();
	int tempi = fopen_s(&f, "data\\save", "r");
	if (tempi != 0) {
		MessageBox(wind, "没有任何数据", "错误", MB_OK | MB_ICONWARNING);
		return 1;
	}
	int tempiii = fgetc(f);
	if (tempiii == EOF) {
		MessageBox(wind, "没有任何数据", "错误", MB_OK | MB_ICONWARNING);
		fclose(f);
		return 1;
	}
	fclose(f);
	return 0;
}
void data_create() {
	int i = _access("data", 0);
	if (i == -1) {
		_mkdir("data");
	}
	FILE* f;
	i = fopen_s(&f, "data\\score", "r");
	if (i != 0) {
		fopen_s(&f, "data\\score", "w");
	}
	fclose(f);
	i = fopen_s(&f, "data\\save", "r");
	if (i != 0) {
		fopen_s(&f, "data\\save", "w");
	}
	fclose(f);
}
int print_ui(struct global_data& g, struct MOUSEMSG& mouseA) {
	setlinecolor(WHITE);
	settextcolor(WHITE);
	fillrectangle(kx, 0, kx, ky);

	{char out[99];
	sprintf_s(out, "    当前分数：%d   ", g.score);
	outtextxy(kx + 30, midy - 40, out);}

	{char out[99];
	sprintf_s(out, "   WSAD移动");
	outtextxy(kx + 50, midy - 100, out);}

	{char out[99];
	sprintf_s(out, "QE可以转动角度");
	outtextxy(kx + 45, midy - 80, out);}

	if (g.roll_time) {
		char out[99];
		sprintf_s(out, "       眩晕！ ");
		outtextxy(kx + 50, midy - 20, out);
	}
	if (g.message_type) {
		g.time_for_message = 200;
		if (g.message_type == 1) {
			strcpy_s(g.message, "找到宝箱！获得1000分！");
		}
		else if (g.message_type == 2) {
			strcpy_s(g.message, "      陷阱！减250分！");
		}
		else if (g.message_type == 3) {
			strcpy_s(g.message, "           保存完毕！");
		}
		else if (g.message_type == 4) {
			strcpy_s(g.message, "           添加完毕！");
		}
		else if (g.message_type == 5) {
			strcpy_s(g.message, "           清除完毕！");
		}
		g.message_type = 0;
	}
	if (g.time_for_message) {
		if (g.time_for_message % 30 >= 15) {
			settextcolor(BLACK);
		}
		g.time_for_message--;
		outtextxy(kx + 20, 100 - 20, g.message);
	}
	setfillcolor(RGB(80, 80, 80));
	setlinecolor(WHITE);

	setbkmode(TRANSPARENT);
	LOGFONT point, before;

	gettextstyle(&point);
	before = point;
	point.lfHeight = 32;
	point.lfQuality = ANTIALIASED_QUALITY;
	strcpy_s(point.lfFaceName, "黑体");
	settextstyle(&point);

	int temp_ji = 80;
	int i = 0;
	char temp_ui_char[5][99] = { "储存存档","添加花朵","清空花朵","回到菜单","切换显示" };

	if (MouseHit()) mouseA = GetMouseMsg();
	int temp_x = mouseA.x, temp_y = mouseA.y;
	int iftouch = -1;

	for (i = 0;i < 5;i++) {
		if (temp_x >= kx + 20 && temp_x <= kx + uix - 20 && temp_y >= ky - 80 - temp_ji * i && temp_y <= ky - 20 - temp_ji * i) {
			if (mouseA.mkLButton) {
				iftouch = i;
				//Sleep(1000);
			}
		}
		if (check_cursor_in_range(kx + 20, ky - 80 - temp_ji * i, kx + uix - 20, ky - 20 - temp_ji * i, mouseA) == 1) {
			setlinecolor(YELLOW);
			rectangle(kx + 20 - 5, ky - 80 - temp_ji * i - 5, kx + uix - 20 + 5, ky - 20 - temp_ji * i + 5);
			setlinecolor(WHITE);
		}
		if (check_cursor_in_range(kx + 20, ky - 80 - temp_ji * i, kx + uix - 20, ky - 20 - temp_ji * i, mouseA) == 1 && mouseA.mkLButton == 1) {
			setfillcolor(RGB(50, 50, 50));
		}
		else {
			setfillcolor(RGB(80, 80, 80));
		}
		fillrectangle(kx + 20, ky - 80 - temp_ji * i, kx + uix - 20, ky - 20 - temp_ji * i);
		settextcolor(BLACK);
		outtextxy(kx + 20 + 15 + 3, ky - 80 + 15 - temp_ji * i + 3, temp_ui_char[i]);
		settextcolor(WHITE);
		outtextxy(kx + 20 + 15, ky - 80 + 15 - temp_ji * i, temp_ui_char[i]);
	}
	settextstyle(&before);
	setbkmode(OPAQUE);
	settextcolor(WHITE);
	mouseA.mkLButton = 0;
	if (iftouch != -1) {
		return iftouch;
	}
	return -1;
}
void player_score(struct global_data& g) {
	g.private_score++;
	if (g.private_score == 5) {
		g.score--;
		g.private_score = 0;
	}
}
void init_game(struct roll_auto_struct& r_a_s, struct global_data& g, struct treasure_head& head) {
	r_a_s.roll_k_range = 0.05;
	r_a_s.acc_roll_k = 0;
	r_a_s.sign_acc_roll_k = 1;
	g.roll_i = PI / 2;
	g.roll_j = 0;
	g.roll_k = PI / 9 * 13;

	g.fuckmex = g.fuckmey = g.x_pre = g.y_pre = 0;
	g.time_for_print_me = 0;
	g.time_for_player_move = 0;
	g.score = 2000;
	g.private_score = 0;
	g.get_treasure = 0;
	g.get_treasure_pre = 0;
	g.time_for_message = 0;
	g.message_type = 0;
	strcpy_s(g.name, "");
	g.roll_time = 0;
	g.roll_de = 0;

	head.first = NULL;
	head.last = NULL;
	head.if_exposed = 0;
}

void save_game(struct global_data& g, struct treasure_head& head,int map[map_x][map_y]) {
	FILE* f;
	data_create();
	int ifopened = fopen_s(&f, "data\\save", "w");
	if (f) {
		fprintf_s(f, "player:\n");
		fprintf_s(f, "\tscore=%d\n", g.score);
		fprintf_s(f, "\tprivate_score=%d\n", g.private_score);
		fprintf_s(f, "\ttime_for_print_me=%lf\n", g.time_for_print_me);
		fprintf_s(f, "\ttime_for_player_move=%d\n", g.time_for_player_move);
		fprintf_s(f, "\tfuckmex=%d\n", g.fuckmex);
		fprintf_s(f, "\tfuckmey=%d\n", g.fuckmey);
		fprintf_s(f, "\tx_pre=%d\n", g.x_pre);
		fprintf_s(f, "\ty_pre=%d\n", g.y_pre);
		fprintf_s(f, "\troll_i=%lf\n", g.roll_i);
		fprintf_s(f, "\troll_j=%lf\n", g.roll_j);
		fprintf_s(f, "\troll_k=%lf\n", g.roll_k);
		fprintf_s(f, "\tget_treasure=%d\n", g.get_treasure);
		fprintf_s(f, "\tget_treasure_pre=%d\n", g.get_treasure_pre);
		fprintf_s(f, "\tname=%s\n", g.name);
		fprintf_s(f, "\troll_time=%d\n", g.roll_time);
		fprintf_s(f, "\troll_de=%d\n", g.roll_de);

		fprintf_s(f, "treasure:\n");
		{
			struct treasure_body* p = head.first;
			int sum = 0;
			while (p != NULL) {
				sum++;
				p = p->next;
			}
			fprintf_s(f, "\tif_exposed=%d\n", head.if_exposed);
			fprintf_s(f, "\tsum=%d\n", sum);

			p = head.first;
			while (p != NULL) {
				fprintf_s(f, "\t{\n");
				fprintf_s(f, "\t\tx=%d\n", p->x);
				fprintf_s(f, "\t\ty=%d\n", p->y);
				fprintf_s(f, "\t\ttype=%d\n", p->type);
				fprintf_s(f, "\t\theight=%d\n", p->height);
				fprintf_s(f, "\t}\n");
				p = p->next;
			}
		}
		fprintf_s(f, "map:\n");
		int i = 0;
		int j = 0;
		for (i = 0;i < map_x; i++) {
			fprintf_s(f,"\t");
			for (j = 0;j < map_y;j++) {
				fprintf_s(f,"%d", map[i][j]);
				if (j < map_y - 1) {
					fprintf_s(f, " ");
				}
			}
			if (i < map_x - 1) {
				fprintf_s(f,"\n");
			}
		}
		fclose(f);
	}
	if (f) fclose(f);
}
int load_game(struct global_data& g, struct treasure_head& head, int map[map_x][map_y]) {
	FILE* f;
	HWND wind = GetHWnd();
	int tempi = fopen_s(&f, "data\\save", "r");
	if (tempi != 0) {
		MessageBox(wind, "没有任何数据", "错误", MB_OK | MB_ICONWARNING);
		return 1;
	}
	else {
		FILE* ftemp = f;
		int tempiii = fgetc(ftemp);
		if (tempiii == EOF) {
			MessageBox(wind, "没有任何数据", "错误", MB_OK | MB_ICONWARNING);
			fclose(f);
			fclose(ftemp);
			return 1;
		}
		fclose(ftemp);
	}

	if (f) fclose(f);
	data_create();
	int ifopened = fopen_s(&f, "data\\save", "r");
	if (f) {
		fscanf_s(f, "player:\n");
		fscanf_s(f, "\tscore=%d\n", &g.score);
		fscanf_s(f, "\tprivate_score=%d\n", &g.private_score);
		fscanf_s(f, "\ttime_for_print_me=%lf\n", &g.time_for_print_me);
		fscanf_s(f, "\ttime_for_player_move=%d\n", &g.time_for_player_move);
		fscanf_s(f, "\tfuckmex=%d\n", &g.fuckmex);
		fscanf_s(f, "\tfuckmey=%d\n", &g.fuckmey);
		fscanf_s(f, "\tx_pre=%d\n", &g.x_pre);
		fscanf_s(f, "\ty_pre=%d\n", &g.y_pre);
		fscanf_s(f, "\troll_i=%lf\n", &g.roll_i);
		fscanf_s(f, "\troll_j=%lf\n", &g.roll_j);
		fscanf_s(f, "\troll_k=%lf\n", &g.roll_k);
		fscanf_s(f, "\tget_treasure=%d\n", &g.get_treasure);
		fscanf_s(f, "\tget_treasure_pre=%d\n", &g.get_treasure_pre);
		fscanf_s(f, "\tname=%s\n", g.name, 99);
		fscanf_s(f, "\troll_time=%d\n", &g.roll_time);
		fscanf_s(f, "\troll_de=%d\n", &g.roll_de);


		fscanf_s(f, "treasure:\n");
		{
			fscanf_s(f, "\tif_exposed=%d\n", &head.if_exposed);
			int sum;
			fscanf_s(f, "\tsum=%d\n", &sum);
			int i = 0;
			for (i = 0;i < sum;i++) {
				struct treasure_body* p;
				p = (struct treasure_body*)malloc(sizeof(struct treasure_body));
				fscanf_s(f, "\t{\n");
				fscanf_s(f, "\t\tx=%d\n", &p->x);
				fscanf_s(f, "\t\ty=%d\n", &p->y);
				fscanf_s(f, "\t\ttype=%d\n", &p->type);
				fscanf_s(f, "\t\theight=%d\n", &p->height);
				fscanf_s(f, "\t}\n");
				p->next = NULL;
				if (head.first == NULL) {
					head.first = p;
					head.last = p;
					p->pre = NULL;
				}
				else {
					p->pre = head.last;
					head.last->next = p;
					head.last = p;
				}
			}
		} {
			fscanf_s(f, "map:\n");
			int i, j;
			for (i = 0;i < map_x;i++) {
				for (j = 0;j < map_y;j++) {
					fscanf_s(f,"%d", &map[i][j]);
				}
			}
		}
		fclose(f);
	}
}
int save_score(struct global_data& g) {
	FILE* f;
	fopen_s(&f, "data\\score", "r+");
	int suml = 0;
	for (;;) {
		if (fgetc(f) == EOF) break;
		suml++;
	}
	if (suml == 0) fprintf_s(f, "%s %d", g.name, g.score);
	else fprintf_s(f, "\n%s %d", g.name, g.score);
	fclose(f);
	return 1;
}
void high_score() {
	FILE* f;
	HWND wind = GetHWnd();
	int ifex = fopen_s(&f, "data\\score", "r");
	if (ifex != 0) {
		MessageBox(wind, "没有任何数据啊", "错误", MB_OK | MB_ICONWARNING);
	}
	else {
		FILE* ftemp = f;
		int tempiii = fgetc(ftemp);
		if (tempiii == EOF) {
			MessageBox(wind, "没有任何数据", "错误", MB_OK | MB_ICONWARNING);
			fclose(f);
			fclose(ftemp);
			return;
		}
		fclose(ftemp);
		fclose(f);
		fopen_s(&f, "data\\score", "r");
		int line = 0, i = 0;
		for (;;) {
			int a;
			a = fgetc(f);
			if (a == EOF) {
				line++;
				break;
			}
			if (a == '\n') {
				line++;
			}
		}
		fclose(f);
		if (line == 0) {
			MessageBox(wind, "没有任何数据啊", "错误", MB_OK | MB_ICONWARNING);
		}
		else {
			struct score_data* score_datap = (struct score_data*)malloc(sizeof(score_data) * line);

			fopen_s(&f, "data\\score", "r");
			for (i = 0;i < line;i++) {
				char iasdiasd;
				fscanf_s(f, "%s%d", score_datap[i].name, 99, &score_datap[i].score);
			}
			fclose(f);
			int ifend = 1;
			for (;ifend != 0;) {
				ifend = 0;
				for (i = 0;i < line - 1; i++) {
					if (score_datap[i].score < score_datap[i + 1].score) {
						struct score_data temp = score_datap[i];
						score_datap[i] = score_datap[i + 1];
						score_datap[i + 1] = temp;
						ifend++;
					}
				}
			}
			char outtemp[2000];
			sprintf_s(outtemp, "分数为");
			for (i = 0;i < line && i < 10;i++) {
				char cattemp[99];
				sprintf_s(cattemp, "\n%s：%d", score_datap[i].name, score_datap[i].score);
				strcat_s(outtemp, cattemp);
			}
			fopen_s(&f, "data\\score", "w");
			for (i = 0;i < line && i < 10;i++) {
				if (i == 0) fprintf_s(f, "%s %d", score_datap[i].name, score_datap[i].score);
				else fprintf_s(f, "\n%s %d", score_datap[i].name, score_datap[i].score);
			}
			fclose(f);
			free(score_datap);
			MessageBox(wind, outtemp, "高分榜", MB_OK);
		}
	}
}

int treasure_add(struct treasure_head& head, int map[map_x][map_y]) {
	struct treasure_body* p;
	p = (struct treasure_body*)malloc(sizeof(struct treasure_body));
	if (p != NULL) {
		p->height = 0;
		p->type = rand() % 2;
		int tempx, tempy;
		while (1) {
			struct treasure_head ht = head;
			struct treasure_body* pt = head.first;
			tempx = rand() % map_x;
			tempy = rand() % map_y;
			int no_overlap = 1;
			while (pt != NULL) {
				if (pt->x == tempx && pt->y == tempy) no_overlap = 0;
				pt = pt->next;
			}
			if (map[tempx][tempy] == 0 && no_overlap == 1) break;
		}
		p->x = tempx;
		p->y = tempy;
		p->next = NULL;

		if (head.first == NULL) {
			head.first = p;
			head.last = p;
			p->pre = NULL;
		}
		else {
			p->pre = head.last;
			head.last->next = p;
			head.last = p;
		}
		return 0;
	}
	else return -1;
}
void treasure_remove(struct treasure_head& head, struct treasure_body* p) {
	struct treasure_body* pt;
	if (head.last == p) {
		head.last = p->pre;
	}
	if (p->pre == NULL) {
		head.first = p->next;
		if (p->next != NULL) {
			p->next->pre = NULL;
		}
		free(p);
	}
	else {
		p->pre->next = p->next;
		if (p->next != NULL)
			p->next->pre = p->pre;
		free(p);
	}
	struct treasure_body* pf = head.first;
	//printf_s("\n");
	while (pf != NULL) {
		//printf_s("\n%d %d %p", pf->x, pf->y, pf->pre);
		pf = pf->next;
	}
}
void treasure_check_get(struct global_data& g, struct treasure_head& head) {
	struct treasure_body* p = head.first;
	while (p != NULL) {
		if (g.fuckmex == p->x && g.fuckmey == p->y) {
			if (p->type == 1) {
				g.message_type = 1;
				g.score += 1000;
			}
			else if (p->type == 0) {
				g.message_type = 2;
				g.score -= 250;
				g.roll_time++;
			}
			treasure_remove(head,p);
			return;
		}
		p = p->next;
	}
}
void treasure_all_free(struct treasure_head& head) {
	struct treasure_body* p = head.first;
	struct treasure_body* pt = p;
	while (p != NULL) {
		p = p->next;
		free(pt);
		pt = p;
	}
	head.first = head.last = NULL;
}

void normal_start(struct global_data& g, struct treasure_head& head,int map[map_x][map_y]) {
	int sum_flower;
	InputBox(g.name, 99, "请输入玩家名称");
	if (strcmp(g.name, "") == 0) {
		strcpy_s(g.name, "anonymity");
	}
	while (1) {
		char temp[99];
		InputBox(temp, 99, "请输入花朵数量\n1到10的整数", "输入花朵数量", "5", 0, 0, "ture");
		sum_flower = atoi(temp);
		if (sum_flower >= 1 && sum_flower <= 10) {
			break;
		}
		else {
			HWND wind = GetHWnd();
			MessageBox(wind, "请输入1到10的整数", "错误", MB_OK | MB_ICONWARNING);
		}
	}
	for (int i = 0;i < sum_flower;i++) {
		treasure_add(head, map);
	}
}

int main() {
	srand((unsigned)time(NULL));
	initgraph(c_s_x, c_s_y);

	LOGFONT before;
	gettextstyle(&before);
	BeginBatchDraw();
	data_create();
	struct roll_auto_struct r_a_s;
	struct global_data g;
	struct treasure_head head;

	init_game(r_a_s, g,head);

	while (1) {
		MOUSEMSG mouseA = GetMouseMsg();
		int map[map_x][map_y];
		init_game(r_a_s, g, head);
		treasure_all_free(head);

		create_map(map);
		for (;;) {
			Sleep(1);
			int tempii = start_menu_return(mouseA);
			if (tempii == 0) {
				normal_start(g,head,map);
				break;
			}
			if (tempii == 1) high_score();
			if (tempii == 2) {
				if (if_save() == 0) {
					load_game(g, head, map);
					break;
				}
				tempii = 0;
			}
			if (tempii == 3) {
				exit(0);
			}
		}
		settextstyle(&before);
		print_map(map,g,head);
		FlushBatchDraw();
		while (1) {
			if (MouseHit()) mouseA = GetMouseMsg();
			if (g.roll_i > PI / 6 + 0.009) g.roll_i -= 0.03 * (g.roll_i - PI / 6 + 0.005);
			Sleep(5);
			player_keyboard(map, g);
			auto_roll(r_a_s,g);
			roll_angle_ranging(g);
			cleardevice();
			int ui_return = print_ui(g, mouseA);
			if (ui_return == 0) {
				save_game(g, head, map);
				g.message_type = 3;
			}
			if (ui_return == 1) {
				treasure_add(head, map);
				g.message_type = 4;
			}
			if (ui_return == 2) {
				treasure_all_free(head);
				g.message_type = 5;
			}
			if (ui_return == 3) {
				g.score = 0;
				break;
			}
			if (ui_return == 4) {
				head.if_exposed = head.if_exposed ? 0 : 1;
			}
			treasure_check_get(g, head);
			print_map(map, g, head);
			FlushBatchDraw();
			int if_win = check_win(map, g);
			if (if_win == 1) {
				mouseA.mkLButton = 0;
				break;
			}
			player_score(g);
		}
		save_score(g);
		print_win(g);
	}
	EndBatchDraw();
	Sleep(1000);
	cleardevice();
	return 0;
}