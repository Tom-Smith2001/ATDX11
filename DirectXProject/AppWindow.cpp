#include "AppWindow.h"
#include <Windows.h>
#include <vector>
#include <fstream>
#include "Matrix4x4.h"
#include "Vector3D.h"
#include "InputSystem.h"
#include "math.h"
#include <iostream>
#include <algorithm>



struct vertex
{
	Vector3D position;
	Vector3D colour;
	Vector3D color1;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
};


AppWindow::AppWindow()
{
}



void AppWindow::update()
{
	constant cc;
	cc.m_time = ::GetTickCount();

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;

	if (m_rot_x > 1.5)
	{
		std::cout << m_rot_x << std::endl;
		m_rot_x = 1.5;
	}
	if (m_rot_x < -1.5)
	{
		std::cout << m_rot_x << std::endl;
		m_rot_x = -1.5;
	}

	Matrix4x4 temp;

	m_delta_scale += m_delta_time / 0.55f;


	cc.m_world.setIdentity();

	Matrix4x4 world_cam;
	world_cam.setIdentity();


	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;


	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.03f);

	new_pos = new_pos + world_cam.getXDirection() * (m_strafe * 0.03f);

	new_pos = Vector3D(new_pos.m_x, 0, new_pos.m_z);

	world_cam.setTranslation(new_pos);

	world_cam = CheckCamCollisions(world_cam);

	m_world_cam = world_cam;

	world_cam.inverse();

	if (enemies == 0) 
	{
		if (level < 2) 
		{
			level += 1;
			BuildMap(level);
		}
		else 
		{
			level = 0;
			BuildMap(level);
		}
	}


	cc.m_view = world_cam;

	int width = this->getClientWindowRect().right - this->getClientWindowRect().left;

	int height = this->getClientWindowRect().bottom - this->getClientWindowRect().top;

	cc.m_proj.setPerspectiveFovLH(1.5f, (float(width) / float(height)), 0.0f, 100.0f);


	m_cb->update(GraphEng::get()->getImmediateDeviceContext(), &cc);
}

Matrix4x4 AppWindow::CheckCamCollisions(Matrix4x4 cam) 
{
	int i = 0;
	while (i < 100) 
	{
		if (cam.getTranslation().m_x <= cube_bounds[i].x_max && cam.getTranslation().m_x >= cube_bounds[i].x_min && cam.getTranslation().m_z <= cube_bounds[i].z_max && cam.getTranslation().m_z >= cube_bounds[i].z_min) 
		{
			std::cout << "Collided with cube, ";
			if (abs(cam.getTranslation().m_x - cube_bounds[i].x_max) < abs(cam.getTranslation().m_x - cube_bounds[i].x_min) && abs(cam.getTranslation().m_x - cube_bounds[i].x_max) < abs(cam.getTranslation().m_z - cube_bounds[i].z_max) && abs(cam.getTranslation().m_x - cube_bounds[i].x_max) < abs(cam.getTranslation().m_z - cube_bounds[i].z_min)) 
			{
				std::cout << "closest to x_max side" << std::endl;
				cam.setTranslation(Vector3D(cube_bounds[i].x_max + 0.02f, 0.0f, cam.getTranslation().m_z));
			}
			else if (abs(cam.getTranslation().m_x - cube_bounds[i].x_min) < abs(cam.getTranslation().m_x - cube_bounds[i].x_max) && abs(cam.getTranslation().m_x - cube_bounds[i].x_min) < abs(cam.getTranslation().m_z - cube_bounds[i].z_max) && abs(cam.getTranslation().m_x - cube_bounds[i].x_min) < abs(cam.getTranslation().m_z - cube_bounds[i].z_min))
			{
				std::cout << "closest to x_min side" << std::endl;
				cam.setTranslation(Vector3D(cube_bounds[i].x_min - 0.02f, 0.0f, cam.getTranslation().m_z));
			}
			else if (abs(cam.getTranslation().m_z - cube_bounds[i].z_max) < abs(cam.getTranslation().m_z - cube_bounds[i].z_min) && abs(cam.getTranslation().m_z - cube_bounds[i].z_max) < abs(cam.getTranslation().m_x - cube_bounds[i].x_max) && abs(cam.getTranslation().m_z - cube_bounds[i].z_max) < abs(cam.getTranslation().m_x - cube_bounds[i].x_min))
			{
				std::cout << "closest to z_max side" << std::endl;
				cam.setTranslation(Vector3D(cam.getTranslation().m_x, 0.0f, cube_bounds[i].z_max + 0.02f));
			}
			else if (abs(cam.getTranslation().m_z - cube_bounds[i].z_min) < abs(cam.getTranslation().m_z - cube_bounds[i].z_max) && abs(cam.getTranslation().m_z - cube_bounds[i].z_min) < abs(cam.getTranslation().m_x - cube_bounds[i].x_max) && abs(cam.getTranslation().m_z - cube_bounds[i].z_min) < abs(cam.getTranslation().m_x - cube_bounds[i].x_min))
			{
				std::cout << "closest to z_min side" << std::endl;
				cam.setTranslation(Vector3D(cam.getTranslation().m_x, 0.0f, cube_bounds[i].z_min - 0.02f));
			}
			
		}
		i++;
	}
	return cam;
}

bool AppWindow::CheckShot() 
{
	Vector3D start_pos = m_world_cam.getTranslation();
	float x = 0;
	int i = 0;
	while (x <= 10) 
	{
		Vector3D shot_pos = start_pos + (m_world_cam.getZDirection()) * x;
		std::cout << shot_pos.m_x;
		std::cout << ", ";
		std::cout << shot_pos.m_y;
		std::cout << ", ";
		std::cout << shot_pos.m_z << std::endl;
		i = 0;
		while (i < 100)
		{
			if (shot_pos.m_x <= cube_bounds[i].x_max && shot_pos.m_x >= cube_bounds[i].x_min && shot_pos.m_z <= cube_bounds[i].z_max && shot_pos.m_z >= cube_bounds[i].z_min)
			{
				if (cube_bounds[i].enemy && cube_bounds[i].alive) 
				{
					std::cout << "Shot hit Enemy!" << std::endl;
					cube_bounds[i].alive = false;
					UpdateMap(level, i);
					enemies -= 1;
					return true;
				}
				else 
				{
					std::cout << "Shot hit wall." << std::endl;
					return false;				
				}
				
			}
			i++;
		}
		x += 0.25f;
	}
	std::cout << "Shot did not hit anything" << std::endl;
	return false;
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	GraphEng::get()->init();
	m_swap_chain = GraphEng::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	UINT rc_width = rc.right - rc.left;
	UINT rc_height = rc.bottom - rc.top;

	m_swap_chain->init(this->m_hwnd, rc_width, rc_height);

	m_world_cam.setTranslation(Vector3D(0, 0, -2));

	
	BuildMap(level);

	constant cc;
	cc.m_time = 0;

	m_cb = GraphEng::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));


}

void AppWindow::BuildMap(int num)
{
	std::string filename = "";
	if (num == 0) 
	{
		filename = "Lvl1.txt";
	}
	if (num == 1)
	{
		filename = "Lvl2.txt";
	}
	if (num == 2)
	{
		filename = "Lvl3.txt";
	}
	std::ifstream input_file(filename);
	if (!input_file.is_open()) {
		std::cout << "Could not open the file - '"
			<< filename << "'" << std::endl;
	}
	
	char lvl_data[100];
	char byte = 0;
	int x = 0;

	Vector3D cube_vectors[2400];
	int i = 0;
	while (input_file.get(byte) && i < 100)
	{
		
		if (byte == 'X')
		{
			int pos = i * 24;
			cube_vectors[pos] = Vector3D(-0.5f, -0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 1] = Vector3D(0, 0, 0);
			cube_vectors[pos + 2] = Vector3D(0.0f, 0.4f, 0.f);
			cube_vectors[pos + 3] = Vector3D(-0.5f, 0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 4] = Vector3D(0, 0, 0);
			cube_vectors[pos + 5] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 6] = Vector3D(0.5f, 0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 7] = Vector3D(0, 0, 0);
			cube_vectors[pos + 8] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 9] = Vector3D(0.5f, -0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 10] = Vector3D(0, 0, 0);
			cube_vectors[pos + 11] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 12] = Vector3D(0.5f, -0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 13] = Vector3D(0, 0, 0);
			cube_vectors[pos + 14] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 15] = Vector3D(0.5f, 0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 16] = Vector3D(0, 0, 0);
			cube_vectors[pos + 17] = Vector3D(0, 0.4f, 0.0f);
			cube_vectors[pos + 18] = Vector3D(-0.5f, 0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 19] = Vector3D(0, 0, 0);
			cube_vectors[pos + 20] = Vector3D(0, 0.4f, 0.0f);
			cube_vectors[pos + 21] = Vector3D(-0.5f, -0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 22] = Vector3D(0, 0, 0);
			cube_vectors[pos + 23] = Vector3D(0, 0.4f, 0);

			cube_bounds[i] = { static_cast<float>(-0.5f + 0.95 * (i % 10)), static_cast<float>(0.5f + 0.95 * (i % 10)), -0.5f + static_cast<float>(0.95 * floor(i / 10)), 0.5f + static_cast<float>(0.95 * floor(i / 10)), false, false };
			

			i++;
		
		}
		else if (byte == ' ')
		{
			int pos = i * 24;
			cube_vectors[pos] = Vector3D(0, 0, 0);
			cube_vectors[pos + 1] = Vector3D(0, 0, 0);
			cube_vectors[pos + 2] = Vector3D(0, 0, 0);
			cube_vectors[pos + 3] = Vector3D(0, 0, 0);
			cube_vectors[pos + 4] = Vector3D(0, 0, 0);
			cube_vectors[pos + 5] = Vector3D(0, 0, 0);
			cube_vectors[pos + 6] = Vector3D(0, 0, 0);
			cube_vectors[pos + 7] = Vector3D(0, 0, 0);
			cube_vectors[pos + 8] = Vector3D(0, 0, 0);
			cube_vectors[pos + 9] = Vector3D(0, 0, 0);
			cube_vectors[pos + 10] = Vector3D(0, 0, 0);
			cube_vectors[pos + 11] = Vector3D(0, 0, 0);
			cube_vectors[pos + 12] = Vector3D(0, 0, 0);
			cube_vectors[pos + 13] = Vector3D(0, 0, 0);
			cube_vectors[pos + 14] = Vector3D(0, 0, 0);
			cube_vectors[pos + 15] = Vector3D(0, 0, 0);
			cube_vectors[pos + 16] = Vector3D(0, 0, 0);
			cube_vectors[pos + 17] = Vector3D(0, 0, 0);
			cube_vectors[pos + 18] = Vector3D(0, 0, 0);
			cube_vectors[pos + 19] = Vector3D(0, 0, 0);
			cube_vectors[pos + 20] = Vector3D(0, 0, 0);
			cube_vectors[pos + 21] = Vector3D(0, 0, 0);
			cube_vectors[pos + 22] = Vector3D(0, 0, 0);
			cube_vectors[pos + 23] = Vector3D(0, 0, 0);

			cube_bounds[i] = { 0, 0, 0, 0, false};
			

			i++;
		}
		else if (byte == 'E')
		{
			int pos = i * 24;
			cube_vectors[pos] = Vector3D(-0.25f, -0.5f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 1] = Vector3D(0, 0, 0);
			cube_vectors[pos + 2] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 3] = Vector3D(-0.25f, 0.1f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 4] = Vector3D(0, 0, 0);
			cube_vectors[pos + 5] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 6] = Vector3D(0.25f, 0.1f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 7] = Vector3D(0, 0, 0);
			cube_vectors[pos + 8] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 9] = Vector3D(0.25f, -0.5f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 10] = Vector3D(0, 0, 0);
			cube_vectors[pos + 11] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 12] = Vector3D(0.25f, -0.5f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 13] = Vector3D(0, 0, 0);
			cube_vectors[pos + 14] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 15] = Vector3D(0.25f, 0.1f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 16] = Vector3D(0, 0, 0);
			cube_vectors[pos + 17] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 18] = Vector3D(-0.25f, 0.1f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 19] = Vector3D(0, 0, 0);
			cube_vectors[pos + 20] = Vector3D(0.4f, 0.0f, 0.0f);
			cube_vectors[pos + 21] = Vector3D(-0.25f, -0.5f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 22] = Vector3D(0, 0, 0);
			cube_vectors[pos + 23] = Vector3D(0.4f, 0.0f, 0);

			cube_bounds[i] = { static_cast<float>(-0.25f + 0.95 * (i % 10)), static_cast<float>(0.25f + 0.95 * (i % 10)), -0.25f + static_cast<float>(0.95 * floor(i / 10)), 0.25f + static_cast<float>(0.95 * floor(i / 10)), true, true };
			
			enemies++;

			i++;
		}
		else if (byte == 'P')
		{
			m_world_cam.setTranslation(Vector3D(static_cast<float>(0.95f * (i % 10)), 0.0f, static_cast<float>(0.95f * (floor(i / 10)))));
			cube_bounds[i] = { 0, 0, 0, 0, false};
			
			i++;
			
		}
		
		
	}
		
	vertex vertex_list[] = 
	{
		{cube_vectors[0], cube_vectors[1], cube_vectors[2]} ,
		{ cube_vectors[3], cube_vectors[4], cube_vectors[5] },
		{ cube_vectors[6], cube_vectors[7], cube_vectors[8] },
		{ cube_vectors[9], cube_vectors[10], cube_vectors[11] },
		{ cube_vectors[12], cube_vectors[13], cube_vectors[14] },
		{ cube_vectors[15], cube_vectors[16], cube_vectors[17] },
		{ cube_vectors[18], cube_vectors[19], cube_vectors[20] },
		{ cube_vectors[21], cube_vectors[22], cube_vectors[23] },
		{ cube_vectors[24], cube_vectors[25], cube_vectors[26] },
		{ cube_vectors[27], cube_vectors[28], cube_vectors[29] },
		{ cube_vectors[30], cube_vectors[31], cube_vectors[32] },
		{ cube_vectors[33], cube_vectors[34], cube_vectors[35] },
		{ cube_vectors[36], cube_vectors[37], cube_vectors[38] },
		{ cube_vectors[39], cube_vectors[40], cube_vectors[41] },
		{ cube_vectors[42], cube_vectors[43], cube_vectors[44] },
		{ cube_vectors[45], cube_vectors[46], cube_vectors[47] },
		{ cube_vectors[48], cube_vectors[49], cube_vectors[50] },
		{ cube_vectors[51], cube_vectors[52], cube_vectors[53] },
		{ cube_vectors[54], cube_vectors[55], cube_vectors[56] },
		{ cube_vectors[57], cube_vectors[58], cube_vectors[59] },
		{ cube_vectors[60], cube_vectors[61], cube_vectors[62] },
		{ cube_vectors[63], cube_vectors[64], cube_vectors[65] },
		{ cube_vectors[66], cube_vectors[67], cube_vectors[68] },
		{ cube_vectors[69], cube_vectors[70], cube_vectors[71] },
		{ cube_vectors[72], cube_vectors[73], cube_vectors[74] },
		{ cube_vectors[75], cube_vectors[76], cube_vectors[77] },
		{ cube_vectors[78], cube_vectors[79], cube_vectors[80] },
		{ cube_vectors[81], cube_vectors[82], cube_vectors[83] },
		{ cube_vectors[84], cube_vectors[85], cube_vectors[86] },
		{ cube_vectors[87], cube_vectors[88], cube_vectors[89] },
		{ cube_vectors[90], cube_vectors[91], cube_vectors[92] },
		{ cube_vectors[93], cube_vectors[94], cube_vectors[95] },
		{ cube_vectors[96], cube_vectors[97], cube_vectors[98] },
		{ cube_vectors[99], cube_vectors[100], cube_vectors[101] },
		{ cube_vectors[102], cube_vectors[103], cube_vectors[104] },
		{ cube_vectors[105], cube_vectors[106], cube_vectors[107] },
		{ cube_vectors[108], cube_vectors[109], cube_vectors[110] },
		{ cube_vectors[111], cube_vectors[112], cube_vectors[113] },
		{ cube_vectors[114], cube_vectors[115], cube_vectors[116] },
		{ cube_vectors[117], cube_vectors[118], cube_vectors[119] },
		{ cube_vectors[120], cube_vectors[121], cube_vectors[122] },
		{ cube_vectors[123], cube_vectors[124], cube_vectors[125] },
		{ cube_vectors[126], cube_vectors[127], cube_vectors[128] },
		{ cube_vectors[129], cube_vectors[130], cube_vectors[131] },
		{ cube_vectors[132], cube_vectors[133], cube_vectors[134] },
		{ cube_vectors[135], cube_vectors[136], cube_vectors[137] },
		{ cube_vectors[138], cube_vectors[139], cube_vectors[140] },
		{ cube_vectors[141], cube_vectors[142], cube_vectors[143] },
		{ cube_vectors[144], cube_vectors[145], cube_vectors[146] },
		{ cube_vectors[147], cube_vectors[148], cube_vectors[149] },
		{ cube_vectors[150], cube_vectors[151], cube_vectors[152] },
		{ cube_vectors[153], cube_vectors[154], cube_vectors[155] },
		{ cube_vectors[156], cube_vectors[157], cube_vectors[158] },
		{ cube_vectors[159], cube_vectors[160], cube_vectors[161] },
		{ cube_vectors[162], cube_vectors[163], cube_vectors[164] },
		{ cube_vectors[165], cube_vectors[166], cube_vectors[167] },
		{ cube_vectors[168], cube_vectors[169], cube_vectors[170] },
		{ cube_vectors[171], cube_vectors[172], cube_vectors[173] },
		{ cube_vectors[174], cube_vectors[175], cube_vectors[176] },
		{ cube_vectors[177], cube_vectors[178], cube_vectors[179] },
		{ cube_vectors[180], cube_vectors[181], cube_vectors[182] },
		{ cube_vectors[183], cube_vectors[184], cube_vectors[185] },
		{ cube_vectors[186], cube_vectors[187], cube_vectors[188] },
		{ cube_vectors[189], cube_vectors[190], cube_vectors[191] },
		{ cube_vectors[192], cube_vectors[193], cube_vectors[194] },
		{ cube_vectors[195], cube_vectors[196], cube_vectors[197] },
		{ cube_vectors[198], cube_vectors[199], cube_vectors[200] },
		{ cube_vectors[201], cube_vectors[202], cube_vectors[203] },
		{ cube_vectors[204], cube_vectors[205], cube_vectors[206] },
		{ cube_vectors[207], cube_vectors[208], cube_vectors[209] },
		{ cube_vectors[210], cube_vectors[211], cube_vectors[212] },
		{ cube_vectors[213], cube_vectors[214], cube_vectors[215] },
		{ cube_vectors[216], cube_vectors[217], cube_vectors[218] },
		{ cube_vectors[219], cube_vectors[220], cube_vectors[221] },
		{ cube_vectors[222], cube_vectors[223], cube_vectors[224] },
		{ cube_vectors[225], cube_vectors[226], cube_vectors[227] },
		{ cube_vectors[228], cube_vectors[229], cube_vectors[230] },
		{ cube_vectors[231], cube_vectors[232], cube_vectors[233] },
		{ cube_vectors[234], cube_vectors[235], cube_vectors[236] },
		{ cube_vectors[237], cube_vectors[238], cube_vectors[239] },
		{ cube_vectors[240], cube_vectors[241], cube_vectors[242] },
		{ cube_vectors[243], cube_vectors[244], cube_vectors[245] },
		{ cube_vectors[246], cube_vectors[247], cube_vectors[248] },
		{ cube_vectors[249], cube_vectors[250], cube_vectors[251] },
		{ cube_vectors[252], cube_vectors[253], cube_vectors[254] },
		{ cube_vectors[255], cube_vectors[256], cube_vectors[257] },
		{ cube_vectors[258], cube_vectors[259], cube_vectors[260] },
		{ cube_vectors[261], cube_vectors[262], cube_vectors[263] },
		{ cube_vectors[264], cube_vectors[265], cube_vectors[266] },
		{ cube_vectors[267], cube_vectors[268], cube_vectors[269] },
		{ cube_vectors[270], cube_vectors[271], cube_vectors[272] },
		{ cube_vectors[273], cube_vectors[274], cube_vectors[275] },
		{ cube_vectors[276], cube_vectors[277], cube_vectors[278] },
		{ cube_vectors[279], cube_vectors[280], cube_vectors[281] },
		{ cube_vectors[282], cube_vectors[283], cube_vectors[284] },
		{ cube_vectors[285], cube_vectors[286], cube_vectors[287] },
		{ cube_vectors[288], cube_vectors[289], cube_vectors[290] },
		{ cube_vectors[291], cube_vectors[292], cube_vectors[293] },
		{ cube_vectors[294], cube_vectors[295], cube_vectors[296] },
		{ cube_vectors[297], cube_vectors[298], cube_vectors[299] },
		{ cube_vectors[300], cube_vectors[301], cube_vectors[302] },
		{ cube_vectors[303], cube_vectors[304], cube_vectors[305] },
		{ cube_vectors[306], cube_vectors[307], cube_vectors[308] },
		{ cube_vectors[309], cube_vectors[310], cube_vectors[311] },
		{ cube_vectors[312], cube_vectors[313], cube_vectors[314] },
		{ cube_vectors[315], cube_vectors[316], cube_vectors[317] },
		{ cube_vectors[318], cube_vectors[319], cube_vectors[320] },
		{ cube_vectors[321], cube_vectors[322], cube_vectors[323] },
		{ cube_vectors[324], cube_vectors[325], cube_vectors[326] },
		{ cube_vectors[327], cube_vectors[328], cube_vectors[329] },
		{ cube_vectors[330], cube_vectors[331], cube_vectors[332] },
		{ cube_vectors[333], cube_vectors[334], cube_vectors[335] },
		{ cube_vectors[336], cube_vectors[337], cube_vectors[338] },
		{ cube_vectors[339], cube_vectors[340], cube_vectors[341] },
		{ cube_vectors[342], cube_vectors[343], cube_vectors[344] },
		{ cube_vectors[345], cube_vectors[346], cube_vectors[347] },
		{ cube_vectors[348], cube_vectors[349], cube_vectors[350] },
		{ cube_vectors[351], cube_vectors[352], cube_vectors[353] },
		{ cube_vectors[354], cube_vectors[355], cube_vectors[356] },
		{ cube_vectors[357], cube_vectors[358], cube_vectors[359] },
		{ cube_vectors[360], cube_vectors[361], cube_vectors[362] },
		{ cube_vectors[363], cube_vectors[364], cube_vectors[365] },
		{ cube_vectors[366], cube_vectors[367], cube_vectors[368] },
		{ cube_vectors[369], cube_vectors[370], cube_vectors[371] },
		{ cube_vectors[372], cube_vectors[373], cube_vectors[374] },
		{ cube_vectors[375], cube_vectors[376], cube_vectors[377] },
		{ cube_vectors[378], cube_vectors[379], cube_vectors[380] },
		{ cube_vectors[381], cube_vectors[382], cube_vectors[383] },
		{ cube_vectors[384], cube_vectors[385], cube_vectors[386] },
		{ cube_vectors[387], cube_vectors[388], cube_vectors[389] },
		{ cube_vectors[390], cube_vectors[391], cube_vectors[392] },
		{ cube_vectors[393], cube_vectors[394], cube_vectors[395] },
		{ cube_vectors[396], cube_vectors[397], cube_vectors[398] },
		{ cube_vectors[399], cube_vectors[400], cube_vectors[401] },
		{ cube_vectors[402], cube_vectors[403], cube_vectors[404] },
		{ cube_vectors[405], cube_vectors[406], cube_vectors[407] },
		{ cube_vectors[408], cube_vectors[409], cube_vectors[410] },
		{ cube_vectors[411], cube_vectors[412], cube_vectors[413] },
		{ cube_vectors[414], cube_vectors[415], cube_vectors[416] },
		{ cube_vectors[417], cube_vectors[418], cube_vectors[419] },
		{ cube_vectors[420], cube_vectors[421], cube_vectors[422] },
		{ cube_vectors[423], cube_vectors[424], cube_vectors[425] },
		{ cube_vectors[426], cube_vectors[427], cube_vectors[428] },
		{ cube_vectors[429], cube_vectors[430], cube_vectors[431] },
		{ cube_vectors[432], cube_vectors[433], cube_vectors[434] },
		{ cube_vectors[435], cube_vectors[436], cube_vectors[437] },
		{ cube_vectors[438], cube_vectors[439], cube_vectors[440] },
		{ cube_vectors[441], cube_vectors[442], cube_vectors[443] },
		{ cube_vectors[444], cube_vectors[445], cube_vectors[446] },
		{ cube_vectors[447], cube_vectors[448], cube_vectors[449] },
		{ cube_vectors[450], cube_vectors[451], cube_vectors[452] },
		{ cube_vectors[453], cube_vectors[454], cube_vectors[455] },
		{ cube_vectors[456], cube_vectors[457], cube_vectors[458] },
		{ cube_vectors[459], cube_vectors[460], cube_vectors[461] },
		{ cube_vectors[462], cube_vectors[463], cube_vectors[464] },
		{ cube_vectors[465], cube_vectors[466], cube_vectors[467] },
		{ cube_vectors[468], cube_vectors[469], cube_vectors[470] },
		{ cube_vectors[471], cube_vectors[472], cube_vectors[473] },
		{ cube_vectors[474], cube_vectors[475], cube_vectors[476] },
		{ cube_vectors[477], cube_vectors[478], cube_vectors[479] },
		{ cube_vectors[480], cube_vectors[481], cube_vectors[482] },
		{ cube_vectors[483], cube_vectors[484], cube_vectors[485] },
		{ cube_vectors[486], cube_vectors[487], cube_vectors[488] },
		{ cube_vectors[489], cube_vectors[490], cube_vectors[491] },
		{ cube_vectors[492], cube_vectors[493], cube_vectors[494] },
		{ cube_vectors[495], cube_vectors[496], cube_vectors[497] },
		{ cube_vectors[498], cube_vectors[499], cube_vectors[500] },
		{ cube_vectors[501], cube_vectors[502], cube_vectors[503] },
		{ cube_vectors[504], cube_vectors[505], cube_vectors[506] },
		{ cube_vectors[507], cube_vectors[508], cube_vectors[509] },
		{ cube_vectors[510], cube_vectors[511], cube_vectors[512] },
		{ cube_vectors[513], cube_vectors[514], cube_vectors[515] },
		{ cube_vectors[516], cube_vectors[517], cube_vectors[518] },
		{ cube_vectors[519], cube_vectors[520], cube_vectors[521] },
		{ cube_vectors[522], cube_vectors[523], cube_vectors[524] },
		{ cube_vectors[525], cube_vectors[526], cube_vectors[527] },
		{ cube_vectors[528], cube_vectors[529], cube_vectors[530] },
		{ cube_vectors[531], cube_vectors[532], cube_vectors[533] },
		{ cube_vectors[534], cube_vectors[535], cube_vectors[536] },
		{ cube_vectors[537], cube_vectors[538], cube_vectors[539] },
		{ cube_vectors[540], cube_vectors[541], cube_vectors[542] },
		{ cube_vectors[543], cube_vectors[544], cube_vectors[545] },
		{ cube_vectors[546], cube_vectors[547], cube_vectors[548] },
		{ cube_vectors[549], cube_vectors[550], cube_vectors[551] },
		{ cube_vectors[552], cube_vectors[553], cube_vectors[554] },
		{ cube_vectors[555], cube_vectors[556], cube_vectors[557] },
		{ cube_vectors[558], cube_vectors[559], cube_vectors[560] },
		{ cube_vectors[561], cube_vectors[562], cube_vectors[563] },
		{ cube_vectors[564], cube_vectors[565], cube_vectors[566] },
		{ cube_vectors[567], cube_vectors[568], cube_vectors[569] },
		{ cube_vectors[570], cube_vectors[571], cube_vectors[572] },
		{ cube_vectors[573], cube_vectors[574], cube_vectors[575] },
		{ cube_vectors[576], cube_vectors[577], cube_vectors[578] },
		{ cube_vectors[579], cube_vectors[580], cube_vectors[581] },
		{ cube_vectors[582], cube_vectors[583], cube_vectors[584] },
		{ cube_vectors[585], cube_vectors[586], cube_vectors[587] },
		{ cube_vectors[588], cube_vectors[589], cube_vectors[590] },
		{ cube_vectors[591], cube_vectors[592], cube_vectors[593] },
		{ cube_vectors[594], cube_vectors[595], cube_vectors[596] },
		{ cube_vectors[597], cube_vectors[598], cube_vectors[599] },
		{ cube_vectors[600], cube_vectors[601], cube_vectors[602] },
		{ cube_vectors[603], cube_vectors[604], cube_vectors[605] },
		{ cube_vectors[606], cube_vectors[607], cube_vectors[608] },
		{ cube_vectors[609], cube_vectors[610], cube_vectors[611] },
		{ cube_vectors[612], cube_vectors[613], cube_vectors[614] },
		{ cube_vectors[615], cube_vectors[616], cube_vectors[617] },
		{ cube_vectors[618], cube_vectors[619], cube_vectors[620] },
		{ cube_vectors[621], cube_vectors[622], cube_vectors[623] },
		{ cube_vectors[624], cube_vectors[625], cube_vectors[626] },
		{ cube_vectors[627], cube_vectors[628], cube_vectors[629] },
		{ cube_vectors[630], cube_vectors[631], cube_vectors[632] },
		{ cube_vectors[633], cube_vectors[634], cube_vectors[635] },
		{ cube_vectors[636], cube_vectors[637], cube_vectors[638] },
		{ cube_vectors[639], cube_vectors[640], cube_vectors[641] },
		{ cube_vectors[642], cube_vectors[643], cube_vectors[644] },
		{ cube_vectors[645], cube_vectors[646], cube_vectors[647] },
		{ cube_vectors[648], cube_vectors[649], cube_vectors[650] },
		{ cube_vectors[651], cube_vectors[652], cube_vectors[653] },
		{ cube_vectors[654], cube_vectors[655], cube_vectors[656] },
		{ cube_vectors[657], cube_vectors[658], cube_vectors[659] },
		{ cube_vectors[660], cube_vectors[661], cube_vectors[662] },
		{ cube_vectors[663], cube_vectors[664], cube_vectors[665] },
		{ cube_vectors[666], cube_vectors[667], cube_vectors[668] },
		{ cube_vectors[669], cube_vectors[670], cube_vectors[671] },
		{ cube_vectors[672], cube_vectors[673], cube_vectors[674] },
		{ cube_vectors[675], cube_vectors[676], cube_vectors[677] },
		{ cube_vectors[678], cube_vectors[679], cube_vectors[680] },
		{ cube_vectors[681], cube_vectors[682], cube_vectors[683] },
		{ cube_vectors[684], cube_vectors[685], cube_vectors[686] },
		{ cube_vectors[687], cube_vectors[688], cube_vectors[689] },
		{ cube_vectors[690], cube_vectors[691], cube_vectors[692] },
		{ cube_vectors[693], cube_vectors[694], cube_vectors[695] },
		{ cube_vectors[696], cube_vectors[697], cube_vectors[698] },
		{ cube_vectors[699], cube_vectors[700], cube_vectors[701] },
		{ cube_vectors[702], cube_vectors[703], cube_vectors[704] },
		{ cube_vectors[705], cube_vectors[706], cube_vectors[707] },
		{ cube_vectors[708], cube_vectors[709], cube_vectors[710] },
		{ cube_vectors[711], cube_vectors[712], cube_vectors[713] },
		{ cube_vectors[714], cube_vectors[715], cube_vectors[716] },
		{ cube_vectors[717], cube_vectors[718], cube_vectors[719] },
		{ cube_vectors[720], cube_vectors[721], cube_vectors[722] },
		{ cube_vectors[723], cube_vectors[724], cube_vectors[725] },
		{ cube_vectors[726], cube_vectors[727], cube_vectors[728] },
		{ cube_vectors[729], cube_vectors[730], cube_vectors[731] },
		{ cube_vectors[732], cube_vectors[733], cube_vectors[734] },
		{ cube_vectors[735], cube_vectors[736], cube_vectors[737] },
		{ cube_vectors[738], cube_vectors[739], cube_vectors[740] },
		{ cube_vectors[741], cube_vectors[742], cube_vectors[743] },
		{ cube_vectors[744], cube_vectors[745], cube_vectors[746] },
		{ cube_vectors[747], cube_vectors[748], cube_vectors[749] },
		{ cube_vectors[750], cube_vectors[751], cube_vectors[752] },
		{ cube_vectors[753], cube_vectors[754], cube_vectors[755] },
		{ cube_vectors[756], cube_vectors[757], cube_vectors[758] },
		{ cube_vectors[759], cube_vectors[760], cube_vectors[761] },
		{ cube_vectors[762], cube_vectors[763], cube_vectors[764] },
		{ cube_vectors[765], cube_vectors[766], cube_vectors[767] },
		{ cube_vectors[768], cube_vectors[769], cube_vectors[770] },
		{ cube_vectors[771], cube_vectors[772], cube_vectors[773] },
		{ cube_vectors[774], cube_vectors[775], cube_vectors[776] },
		{ cube_vectors[777], cube_vectors[778], cube_vectors[779] },
		{ cube_vectors[780], cube_vectors[781], cube_vectors[782] },
		{ cube_vectors[783], cube_vectors[784], cube_vectors[785] },
		{ cube_vectors[786], cube_vectors[787], cube_vectors[788] },
		{ cube_vectors[789], cube_vectors[790], cube_vectors[791] },
		{ cube_vectors[792], cube_vectors[793], cube_vectors[794] },
		{ cube_vectors[795], cube_vectors[796], cube_vectors[797] },
		{ cube_vectors[798], cube_vectors[799], cube_vectors[800] },
		{ cube_vectors[801], cube_vectors[802], cube_vectors[803] },
		{ cube_vectors[804], cube_vectors[805], cube_vectors[806] },
		{ cube_vectors[807], cube_vectors[808], cube_vectors[809] },
		{ cube_vectors[810], cube_vectors[811], cube_vectors[812] },
		{ cube_vectors[813], cube_vectors[814], cube_vectors[815] },
		{ cube_vectors[816], cube_vectors[817], cube_vectors[818] },
		{ cube_vectors[819], cube_vectors[820], cube_vectors[821] },
		{ cube_vectors[822], cube_vectors[823], cube_vectors[824] },
		{ cube_vectors[825], cube_vectors[826], cube_vectors[827] },
		{ cube_vectors[828], cube_vectors[829], cube_vectors[830] },
		{ cube_vectors[831], cube_vectors[832], cube_vectors[833] },
		{ cube_vectors[834], cube_vectors[835], cube_vectors[836] },
		{ cube_vectors[837], cube_vectors[838], cube_vectors[839] },
		{ cube_vectors[840], cube_vectors[841], cube_vectors[842] },
		{ cube_vectors[843], cube_vectors[844], cube_vectors[845] },
		{ cube_vectors[846], cube_vectors[847], cube_vectors[848] },
		{ cube_vectors[849], cube_vectors[850], cube_vectors[851] },
		{ cube_vectors[852], cube_vectors[853], cube_vectors[854] },
		{ cube_vectors[855], cube_vectors[856], cube_vectors[857] },
		{ cube_vectors[858], cube_vectors[859], cube_vectors[860] },
		{ cube_vectors[861], cube_vectors[862], cube_vectors[863] },
		{ cube_vectors[864], cube_vectors[865], cube_vectors[866] },
		{ cube_vectors[867], cube_vectors[868], cube_vectors[869] },
		{ cube_vectors[870], cube_vectors[871], cube_vectors[872] },
		{ cube_vectors[873], cube_vectors[874], cube_vectors[875] },
		{ cube_vectors[876], cube_vectors[877], cube_vectors[878] },
		{ cube_vectors[879], cube_vectors[880], cube_vectors[881] },
		{ cube_vectors[882], cube_vectors[883], cube_vectors[884] },
		{ cube_vectors[885], cube_vectors[886], cube_vectors[887] },
		{ cube_vectors[888], cube_vectors[889], cube_vectors[890] },
		{ cube_vectors[891], cube_vectors[892], cube_vectors[893] },
		{ cube_vectors[894], cube_vectors[895], cube_vectors[896] },
		{ cube_vectors[897], cube_vectors[898], cube_vectors[899] },
		{ cube_vectors[900], cube_vectors[901], cube_vectors[902] },
		{ cube_vectors[903], cube_vectors[904], cube_vectors[905] },
		{ cube_vectors[906], cube_vectors[907], cube_vectors[908] },
		{ cube_vectors[909], cube_vectors[910], cube_vectors[911] },
		{ cube_vectors[912], cube_vectors[913], cube_vectors[914] },
		{ cube_vectors[915], cube_vectors[916], cube_vectors[917] },
		{ cube_vectors[918], cube_vectors[919], cube_vectors[920] },
		{ cube_vectors[921], cube_vectors[922], cube_vectors[923] },
		{ cube_vectors[924], cube_vectors[925], cube_vectors[926] },
		{ cube_vectors[927], cube_vectors[928], cube_vectors[929] },
		{ cube_vectors[930], cube_vectors[931], cube_vectors[932] },
		{ cube_vectors[933], cube_vectors[934], cube_vectors[935] },
		{ cube_vectors[936], cube_vectors[937], cube_vectors[938] },
		{ cube_vectors[939], cube_vectors[940], cube_vectors[941] },
		{ cube_vectors[942], cube_vectors[943], cube_vectors[944] },
		{ cube_vectors[945], cube_vectors[946], cube_vectors[947] },
		{ cube_vectors[948], cube_vectors[949], cube_vectors[950] },
		{ cube_vectors[951], cube_vectors[952], cube_vectors[953] },
		{ cube_vectors[954], cube_vectors[955], cube_vectors[956] },
		{ cube_vectors[957], cube_vectors[958], cube_vectors[959] },
		{ cube_vectors[960], cube_vectors[961], cube_vectors[962] },
		{ cube_vectors[963], cube_vectors[964], cube_vectors[965] },
		{ cube_vectors[966], cube_vectors[967], cube_vectors[968] },
		{ cube_vectors[969], cube_vectors[970], cube_vectors[971] },
		{ cube_vectors[972], cube_vectors[973], cube_vectors[974] },
		{ cube_vectors[975], cube_vectors[976], cube_vectors[977] },
		{ cube_vectors[978], cube_vectors[979], cube_vectors[980] },
		{ cube_vectors[981], cube_vectors[982], cube_vectors[983] },
		{ cube_vectors[984], cube_vectors[985], cube_vectors[986] },
		{ cube_vectors[987], cube_vectors[988], cube_vectors[989] },
		{ cube_vectors[990], cube_vectors[991], cube_vectors[992] },
		{ cube_vectors[993], cube_vectors[994], cube_vectors[995] },
		{ cube_vectors[996], cube_vectors[997], cube_vectors[998] },
		{ cube_vectors[999], cube_vectors[1000], cube_vectors[1001] },
		{ cube_vectors[1002], cube_vectors[1003], cube_vectors[1004] },
		{ cube_vectors[1005], cube_vectors[1006], cube_vectors[1007] },
		{ cube_vectors[1008], cube_vectors[1009], cube_vectors[1010] },
		{ cube_vectors[1011], cube_vectors[1012], cube_vectors[1013] },
		{ cube_vectors[1014], cube_vectors[1015], cube_vectors[1016] },
		{ cube_vectors[1017], cube_vectors[1018], cube_vectors[1019] },
		{ cube_vectors[1020], cube_vectors[1021], cube_vectors[1022] },
		{ cube_vectors[1023], cube_vectors[1024], cube_vectors[1025] },
		{ cube_vectors[1026], cube_vectors[1027], cube_vectors[1028] },
		{ cube_vectors[1029], cube_vectors[1030], cube_vectors[1031] },
		{ cube_vectors[1032], cube_vectors[1033], cube_vectors[1034] },
		{ cube_vectors[1035], cube_vectors[1036], cube_vectors[1037] },
		{ cube_vectors[1038], cube_vectors[1039], cube_vectors[1040] },
		{ cube_vectors[1041], cube_vectors[1042], cube_vectors[1043] },
		{ cube_vectors[1044], cube_vectors[1045], cube_vectors[1046] },
		{ cube_vectors[1047], cube_vectors[1048], cube_vectors[1049] },
		{ cube_vectors[1050], cube_vectors[1051], cube_vectors[1052] },
		{ cube_vectors[1053], cube_vectors[1054], cube_vectors[1055] },
		{ cube_vectors[1056], cube_vectors[1057], cube_vectors[1058] },
		{ cube_vectors[1059], cube_vectors[1060], cube_vectors[1061] },
		{ cube_vectors[1062], cube_vectors[1063], cube_vectors[1064] },
		{ cube_vectors[1065], cube_vectors[1066], cube_vectors[1067] },
		{ cube_vectors[1068], cube_vectors[1069], cube_vectors[1070] },
		{ cube_vectors[1071], cube_vectors[1072], cube_vectors[1073] },
		{ cube_vectors[1074], cube_vectors[1075], cube_vectors[1076] },
		{ cube_vectors[1077], cube_vectors[1078], cube_vectors[1079] },
		{ cube_vectors[1080], cube_vectors[1081], cube_vectors[1082] },
		{ cube_vectors[1083], cube_vectors[1084], cube_vectors[1085] },
		{ cube_vectors[1086], cube_vectors[1087], cube_vectors[1088] },
		{ cube_vectors[1089], cube_vectors[1090], cube_vectors[1091] },
		{ cube_vectors[1092], cube_vectors[1093], cube_vectors[1094] },
		{ cube_vectors[1095], cube_vectors[1096], cube_vectors[1097] },
		{ cube_vectors[1098], cube_vectors[1099], cube_vectors[1100] },
		{ cube_vectors[1101], cube_vectors[1102], cube_vectors[1103] },
		{ cube_vectors[1104], cube_vectors[1105], cube_vectors[1106] },
		{ cube_vectors[1107], cube_vectors[1108], cube_vectors[1109] },
		{ cube_vectors[1110], cube_vectors[1111], cube_vectors[1112] },
		{ cube_vectors[1113], cube_vectors[1114], cube_vectors[1115] },
		{ cube_vectors[1116], cube_vectors[1117], cube_vectors[1118] },
		{ cube_vectors[1119], cube_vectors[1120], cube_vectors[1121] },
		{ cube_vectors[1122], cube_vectors[1123], cube_vectors[1124] },
		{ cube_vectors[1125], cube_vectors[1126], cube_vectors[1127] },
		{ cube_vectors[1128], cube_vectors[1129], cube_vectors[1130] },
		{ cube_vectors[1131], cube_vectors[1132], cube_vectors[1133] },
		{ cube_vectors[1134], cube_vectors[1135], cube_vectors[1136] },
		{ cube_vectors[1137], cube_vectors[1138], cube_vectors[1139] },
		{ cube_vectors[1140], cube_vectors[1141], cube_vectors[1142] },
		{ cube_vectors[1143], cube_vectors[1144], cube_vectors[1145] },
		{ cube_vectors[1146], cube_vectors[1147], cube_vectors[1148] },
		{ cube_vectors[1149], cube_vectors[1150], cube_vectors[1151] },
		{ cube_vectors[1152], cube_vectors[1153], cube_vectors[1154] },
		{ cube_vectors[1155], cube_vectors[1156], cube_vectors[1157] },
		{ cube_vectors[1158], cube_vectors[1159], cube_vectors[1160] },
		{ cube_vectors[1161], cube_vectors[1162], cube_vectors[1163] },
		{ cube_vectors[1164], cube_vectors[1165], cube_vectors[1166] },
		{ cube_vectors[1167], cube_vectors[1168], cube_vectors[1169] },
		{ cube_vectors[1170], cube_vectors[1171], cube_vectors[1172] },
		{ cube_vectors[1173], cube_vectors[1174], cube_vectors[1175] },
		{ cube_vectors[1176], cube_vectors[1177], cube_vectors[1178] },
		{ cube_vectors[1179], cube_vectors[1180], cube_vectors[1181] },
		{ cube_vectors[1182], cube_vectors[1183], cube_vectors[1184] },
		{ cube_vectors[1185], cube_vectors[1186], cube_vectors[1187] },
		{ cube_vectors[1188], cube_vectors[1189], cube_vectors[1190] },
		{ cube_vectors[1191], cube_vectors[1192], cube_vectors[1193] },
		{ cube_vectors[1194], cube_vectors[1195], cube_vectors[1196] },
		{ cube_vectors[1197], cube_vectors[1198], cube_vectors[1199] },
		{ cube_vectors[1200], cube_vectors[1201], cube_vectors[1202] },
		{ cube_vectors[1203], cube_vectors[1204], cube_vectors[1205] },
		{ cube_vectors[1206], cube_vectors[1207], cube_vectors[1208] },
		{ cube_vectors[1209], cube_vectors[1210], cube_vectors[1211] },
		{ cube_vectors[1212], cube_vectors[1213], cube_vectors[1214] },
		{ cube_vectors[1215], cube_vectors[1216], cube_vectors[1217] },
		{ cube_vectors[1218], cube_vectors[1219], cube_vectors[1220] },
		{ cube_vectors[1221], cube_vectors[1222], cube_vectors[1223] },
		{ cube_vectors[1224], cube_vectors[1225], cube_vectors[1226] },
		{ cube_vectors[1227], cube_vectors[1228], cube_vectors[1229] },
		{ cube_vectors[1230], cube_vectors[1231], cube_vectors[1232] },
		{ cube_vectors[1233], cube_vectors[1234], cube_vectors[1235] },
		{ cube_vectors[1236], cube_vectors[1237], cube_vectors[1238] },
		{ cube_vectors[1239], cube_vectors[1240], cube_vectors[1241] },
		{ cube_vectors[1242], cube_vectors[1243], cube_vectors[1244] },
		{ cube_vectors[1245], cube_vectors[1246], cube_vectors[1247] },
		{ cube_vectors[1248], cube_vectors[1249], cube_vectors[1250] },
		{ cube_vectors[1251], cube_vectors[1252], cube_vectors[1253] },
		{ cube_vectors[1254], cube_vectors[1255], cube_vectors[1256] },
		{ cube_vectors[1257], cube_vectors[1258], cube_vectors[1259] },
		{ cube_vectors[1260], cube_vectors[1261], cube_vectors[1262] },
		{ cube_vectors[1263], cube_vectors[1264], cube_vectors[1265] },
		{ cube_vectors[1266], cube_vectors[1267], cube_vectors[1268] },
		{ cube_vectors[1269], cube_vectors[1270], cube_vectors[1271] },
		{ cube_vectors[1272], cube_vectors[1273], cube_vectors[1274] },
		{ cube_vectors[1275], cube_vectors[1276], cube_vectors[1277] },
		{ cube_vectors[1278], cube_vectors[1279], cube_vectors[1280] },
		{ cube_vectors[1281], cube_vectors[1282], cube_vectors[1283] },
		{ cube_vectors[1284], cube_vectors[1285], cube_vectors[1286] },
		{ cube_vectors[1287], cube_vectors[1288], cube_vectors[1289] },
		{ cube_vectors[1290], cube_vectors[1291], cube_vectors[1292] },
		{ cube_vectors[1293], cube_vectors[1294], cube_vectors[1295] },
		{ cube_vectors[1296], cube_vectors[1297], cube_vectors[1298] },
		{ cube_vectors[1299], cube_vectors[1300], cube_vectors[1301] },
		{ cube_vectors[1302], cube_vectors[1303], cube_vectors[1304] },
		{ cube_vectors[1305], cube_vectors[1306], cube_vectors[1307] },
		{ cube_vectors[1308], cube_vectors[1309], cube_vectors[1310] },
		{ cube_vectors[1311], cube_vectors[1312], cube_vectors[1313] },
		{ cube_vectors[1314], cube_vectors[1315], cube_vectors[1316] },
		{ cube_vectors[1317], cube_vectors[1318], cube_vectors[1319] },
		{ cube_vectors[1320], cube_vectors[1321], cube_vectors[1322] },
		{ cube_vectors[1323], cube_vectors[1324], cube_vectors[1325] },
		{ cube_vectors[1326], cube_vectors[1327], cube_vectors[1328] },
		{ cube_vectors[1329], cube_vectors[1330], cube_vectors[1331] },
		{ cube_vectors[1332], cube_vectors[1333], cube_vectors[1334] },
		{ cube_vectors[1335], cube_vectors[1336], cube_vectors[1337] },
		{ cube_vectors[1338], cube_vectors[1339], cube_vectors[1340] },
		{ cube_vectors[1341], cube_vectors[1342], cube_vectors[1343] },
		{ cube_vectors[1344], cube_vectors[1345], cube_vectors[1346] },
		{ cube_vectors[1347], cube_vectors[1348], cube_vectors[1349] },
		{ cube_vectors[1350], cube_vectors[1351], cube_vectors[1352] },
		{ cube_vectors[1353], cube_vectors[1354], cube_vectors[1355] },
		{ cube_vectors[1356], cube_vectors[1357], cube_vectors[1358] },
		{ cube_vectors[1359], cube_vectors[1360], cube_vectors[1361] },
		{ cube_vectors[1362], cube_vectors[1363], cube_vectors[1364] },
		{ cube_vectors[1365], cube_vectors[1366], cube_vectors[1367] },
		{ cube_vectors[1368], cube_vectors[1369], cube_vectors[1370] },
		{ cube_vectors[1371], cube_vectors[1372], cube_vectors[1373] },
		{ cube_vectors[1374], cube_vectors[1375], cube_vectors[1376] },
		{ cube_vectors[1377], cube_vectors[1378], cube_vectors[1379] },
		{ cube_vectors[1380], cube_vectors[1381], cube_vectors[1382] },
		{ cube_vectors[1383], cube_vectors[1384], cube_vectors[1385] },
		{ cube_vectors[1386], cube_vectors[1387], cube_vectors[1388] },
		{ cube_vectors[1389], cube_vectors[1390], cube_vectors[1391] },
		{ cube_vectors[1392], cube_vectors[1393], cube_vectors[1394] },
		{ cube_vectors[1395], cube_vectors[1396], cube_vectors[1397] },
		{ cube_vectors[1398], cube_vectors[1399], cube_vectors[1400] },
		{ cube_vectors[1401], cube_vectors[1402], cube_vectors[1403] },
		{ cube_vectors[1404], cube_vectors[1405], cube_vectors[1406] },
		{ cube_vectors[1407], cube_vectors[1408], cube_vectors[1409] },
		{ cube_vectors[1410], cube_vectors[1411], cube_vectors[1412] },
		{ cube_vectors[1413], cube_vectors[1414], cube_vectors[1415] },
		{ cube_vectors[1416], cube_vectors[1417], cube_vectors[1418] },
		{ cube_vectors[1419], cube_vectors[1420], cube_vectors[1421] },
		{ cube_vectors[1422], cube_vectors[1423], cube_vectors[1424] },
		{ cube_vectors[1425], cube_vectors[1426], cube_vectors[1427] },
		{ cube_vectors[1428], cube_vectors[1429], cube_vectors[1430] },
		{ cube_vectors[1431], cube_vectors[1432], cube_vectors[1433] },
		{ cube_vectors[1434], cube_vectors[1435], cube_vectors[1436] },
		{ cube_vectors[1437], cube_vectors[1438], cube_vectors[1439] },
		{ cube_vectors[1440], cube_vectors[1441], cube_vectors[1442] },
		{ cube_vectors[1443], cube_vectors[1444], cube_vectors[1445] },
		{ cube_vectors[1446], cube_vectors[1447], cube_vectors[1448] },
		{ cube_vectors[1449], cube_vectors[1450], cube_vectors[1451] },
		{ cube_vectors[1452], cube_vectors[1453], cube_vectors[1454] },
		{ cube_vectors[1455], cube_vectors[1456], cube_vectors[1457] },
		{ cube_vectors[1458], cube_vectors[1459], cube_vectors[1460] },
		{ cube_vectors[1461], cube_vectors[1462], cube_vectors[1463] },
		{ cube_vectors[1464], cube_vectors[1465], cube_vectors[1466] },
		{ cube_vectors[1467], cube_vectors[1468], cube_vectors[1469] },
		{ cube_vectors[1470], cube_vectors[1471], cube_vectors[1472] },
		{ cube_vectors[1473], cube_vectors[1474], cube_vectors[1475] },
		{ cube_vectors[1476], cube_vectors[1477], cube_vectors[1478] },
		{ cube_vectors[1479], cube_vectors[1480], cube_vectors[1481] },
		{ cube_vectors[1482], cube_vectors[1483], cube_vectors[1484] },
		{ cube_vectors[1485], cube_vectors[1486], cube_vectors[1487] },
		{ cube_vectors[1488], cube_vectors[1489], cube_vectors[1490] },
		{ cube_vectors[1491], cube_vectors[1492], cube_vectors[1493] },
		{ cube_vectors[1494], cube_vectors[1495], cube_vectors[1496] },
		{ cube_vectors[1497], cube_vectors[1498], cube_vectors[1499] },
		{ cube_vectors[1500], cube_vectors[1501], cube_vectors[1502] },
		{ cube_vectors[1503], cube_vectors[1504], cube_vectors[1505] },
		{ cube_vectors[1506], cube_vectors[1507], cube_vectors[1508] },
		{ cube_vectors[1509], cube_vectors[1510], cube_vectors[1511] },
		{ cube_vectors[1512], cube_vectors[1513], cube_vectors[1514] },
		{ cube_vectors[1515], cube_vectors[1516], cube_vectors[1517] },
		{ cube_vectors[1518], cube_vectors[1519], cube_vectors[1520] },
		{ cube_vectors[1521], cube_vectors[1522], cube_vectors[1523] },
		{ cube_vectors[1524], cube_vectors[1525], cube_vectors[1526] },
		{ cube_vectors[1527], cube_vectors[1528], cube_vectors[1529] },
		{ cube_vectors[1530], cube_vectors[1531], cube_vectors[1532] },
		{ cube_vectors[1533], cube_vectors[1534], cube_vectors[1535] },
		{ cube_vectors[1536], cube_vectors[1537], cube_vectors[1538] },
		{ cube_vectors[1539], cube_vectors[1540], cube_vectors[1541] },
		{ cube_vectors[1542], cube_vectors[1543], cube_vectors[1544] },
		{ cube_vectors[1545], cube_vectors[1546], cube_vectors[1547] },
		{ cube_vectors[1548], cube_vectors[1549], cube_vectors[1550] },
		{ cube_vectors[1551], cube_vectors[1552], cube_vectors[1553] },
		{ cube_vectors[1554], cube_vectors[1555], cube_vectors[1556] },
		{ cube_vectors[1557], cube_vectors[1558], cube_vectors[1559] },
		{ cube_vectors[1560], cube_vectors[1561], cube_vectors[1562] },
		{ cube_vectors[1563], cube_vectors[1564], cube_vectors[1565] },
		{ cube_vectors[1566], cube_vectors[1567], cube_vectors[1568] },
		{ cube_vectors[1569], cube_vectors[1570], cube_vectors[1571] },
		{ cube_vectors[1572], cube_vectors[1573], cube_vectors[1574] },
		{ cube_vectors[1575], cube_vectors[1576], cube_vectors[1577] },
		{ cube_vectors[1578], cube_vectors[1579], cube_vectors[1580] },
		{ cube_vectors[1581], cube_vectors[1582], cube_vectors[1583] },
		{ cube_vectors[1584], cube_vectors[1585], cube_vectors[1586] },
		{ cube_vectors[1587], cube_vectors[1588], cube_vectors[1589] },
		{ cube_vectors[1590], cube_vectors[1591], cube_vectors[1592] },
		{ cube_vectors[1593], cube_vectors[1594], cube_vectors[1595] },
		{ cube_vectors[1596], cube_vectors[1597], cube_vectors[1598] },
		{ cube_vectors[1599], cube_vectors[1600], cube_vectors[1601] },
		{ cube_vectors[1602], cube_vectors[1603], cube_vectors[1604] },
		{ cube_vectors[1605], cube_vectors[1606], cube_vectors[1607] },
		{ cube_vectors[1608], cube_vectors[1609], cube_vectors[1610] },
		{ cube_vectors[1611], cube_vectors[1612], cube_vectors[1613] },
		{ cube_vectors[1614], cube_vectors[1615], cube_vectors[1616] },
		{ cube_vectors[1617], cube_vectors[1618], cube_vectors[1619] },
		{ cube_vectors[1620], cube_vectors[1621], cube_vectors[1622] },
		{ cube_vectors[1623], cube_vectors[1624], cube_vectors[1625] },
		{ cube_vectors[1626], cube_vectors[1627], cube_vectors[1628] },
		{ cube_vectors[1629], cube_vectors[1630], cube_vectors[1631] },
		{ cube_vectors[1632], cube_vectors[1633], cube_vectors[1634] },
		{ cube_vectors[1635], cube_vectors[1636], cube_vectors[1637] },
		{ cube_vectors[1638], cube_vectors[1639], cube_vectors[1640] },
		{ cube_vectors[1641], cube_vectors[1642], cube_vectors[1643] },
		{ cube_vectors[1644], cube_vectors[1645], cube_vectors[1646] },
		{ cube_vectors[1647], cube_vectors[1648], cube_vectors[1649] },
		{ cube_vectors[1650], cube_vectors[1651], cube_vectors[1652] },
		{ cube_vectors[1653], cube_vectors[1654], cube_vectors[1655] },
		{ cube_vectors[1656], cube_vectors[1657], cube_vectors[1658] },
		{ cube_vectors[1659], cube_vectors[1660], cube_vectors[1661] },
		{ cube_vectors[1662], cube_vectors[1663], cube_vectors[1664] },
		{ cube_vectors[1665], cube_vectors[1666], cube_vectors[1667] },
		{ cube_vectors[1668], cube_vectors[1669], cube_vectors[1670] },
		{ cube_vectors[1671], cube_vectors[1672], cube_vectors[1673] },
		{ cube_vectors[1674], cube_vectors[1675], cube_vectors[1676] },
		{ cube_vectors[1677], cube_vectors[1678], cube_vectors[1679] },
		{ cube_vectors[1680], cube_vectors[1681], cube_vectors[1682] },
		{ cube_vectors[1683], cube_vectors[1684], cube_vectors[1685] },
		{ cube_vectors[1686], cube_vectors[1687], cube_vectors[1688] },
		{ cube_vectors[1689], cube_vectors[1690], cube_vectors[1691] },
		{ cube_vectors[1692], cube_vectors[1693], cube_vectors[1694] },
		{ cube_vectors[1695], cube_vectors[1696], cube_vectors[1697] },
		{ cube_vectors[1698], cube_vectors[1699], cube_vectors[1700] },
		{ cube_vectors[1701], cube_vectors[1702], cube_vectors[1703] },
		{ cube_vectors[1704], cube_vectors[1705], cube_vectors[1706] },
		{ cube_vectors[1707], cube_vectors[1708], cube_vectors[1709] },
		{ cube_vectors[1710], cube_vectors[1711], cube_vectors[1712] },
		{ cube_vectors[1713], cube_vectors[1714], cube_vectors[1715] },
		{ cube_vectors[1716], cube_vectors[1717], cube_vectors[1718] },
		{ cube_vectors[1719], cube_vectors[1720], cube_vectors[1721] },
		{ cube_vectors[1722], cube_vectors[1723], cube_vectors[1724] },
		{ cube_vectors[1725], cube_vectors[1726], cube_vectors[1727] },
		{ cube_vectors[1728], cube_vectors[1729], cube_vectors[1730] },
		{ cube_vectors[1731], cube_vectors[1732], cube_vectors[1733] },
		{ cube_vectors[1734], cube_vectors[1735], cube_vectors[1736] },
		{ cube_vectors[1737], cube_vectors[1738], cube_vectors[1739] },
		{ cube_vectors[1740], cube_vectors[1741], cube_vectors[1742] },
		{ cube_vectors[1743], cube_vectors[1744], cube_vectors[1745] },
		{ cube_vectors[1746], cube_vectors[1747], cube_vectors[1748] },
		{ cube_vectors[1749], cube_vectors[1750], cube_vectors[1751] },
		{ cube_vectors[1752], cube_vectors[1753], cube_vectors[1754] },
		{ cube_vectors[1755], cube_vectors[1756], cube_vectors[1757] },
		{ cube_vectors[1758], cube_vectors[1759], cube_vectors[1760] },
		{ cube_vectors[1761], cube_vectors[1762], cube_vectors[1763] },
		{ cube_vectors[1764], cube_vectors[1765], cube_vectors[1766] },
		{ cube_vectors[1767], cube_vectors[1768], cube_vectors[1769] },
		{ cube_vectors[1770], cube_vectors[1771], cube_vectors[1772] },
		{ cube_vectors[1773], cube_vectors[1774], cube_vectors[1775] },
		{ cube_vectors[1776], cube_vectors[1777], cube_vectors[1778] },
		{ cube_vectors[1779], cube_vectors[1780], cube_vectors[1781] },
		{ cube_vectors[1782], cube_vectors[1783], cube_vectors[1784] },
		{ cube_vectors[1785], cube_vectors[1786], cube_vectors[1787] },
		{ cube_vectors[1788], cube_vectors[1789], cube_vectors[1790] },
		{ cube_vectors[1791], cube_vectors[1792], cube_vectors[1793] },
		{ cube_vectors[1794], cube_vectors[1795], cube_vectors[1796] },
		{ cube_vectors[1797], cube_vectors[1798], cube_vectors[1799] },
		{ cube_vectors[1800], cube_vectors[1801], cube_vectors[1802] },
		{ cube_vectors[1803], cube_vectors[1804], cube_vectors[1805] },
		{ cube_vectors[1806], cube_vectors[1807], cube_vectors[1808] },
		{ cube_vectors[1809], cube_vectors[1810], cube_vectors[1811] },
		{ cube_vectors[1812], cube_vectors[1813], cube_vectors[1814] },
		{ cube_vectors[1815], cube_vectors[1816], cube_vectors[1817] },
		{ cube_vectors[1818], cube_vectors[1819], cube_vectors[1820] },
		{ cube_vectors[1821], cube_vectors[1822], cube_vectors[1823] },
		{ cube_vectors[1824], cube_vectors[1825], cube_vectors[1826] },
		{ cube_vectors[1827], cube_vectors[1828], cube_vectors[1829] },
		{ cube_vectors[1830], cube_vectors[1831], cube_vectors[1832] },
		{ cube_vectors[1833], cube_vectors[1834], cube_vectors[1835] },
		{ cube_vectors[1836], cube_vectors[1837], cube_vectors[1838] },
		{ cube_vectors[1839], cube_vectors[1840], cube_vectors[1841] },
		{ cube_vectors[1842], cube_vectors[1843], cube_vectors[1844] },
		{ cube_vectors[1845], cube_vectors[1846], cube_vectors[1847] },
		{ cube_vectors[1848], cube_vectors[1849], cube_vectors[1850] },
		{ cube_vectors[1851], cube_vectors[1852], cube_vectors[1853] },
		{ cube_vectors[1854], cube_vectors[1855], cube_vectors[1856] },
		{ cube_vectors[1857], cube_vectors[1858], cube_vectors[1859] },
		{ cube_vectors[1860], cube_vectors[1861], cube_vectors[1862] },
		{ cube_vectors[1863], cube_vectors[1864], cube_vectors[1865] },
		{ cube_vectors[1866], cube_vectors[1867], cube_vectors[1868] },
		{ cube_vectors[1869], cube_vectors[1870], cube_vectors[1871] },
		{ cube_vectors[1872], cube_vectors[1873], cube_vectors[1874] },
		{ cube_vectors[1875], cube_vectors[1876], cube_vectors[1877] },
		{ cube_vectors[1878], cube_vectors[1879], cube_vectors[1880] },
		{ cube_vectors[1881], cube_vectors[1882], cube_vectors[1883] },
		{ cube_vectors[1884], cube_vectors[1885], cube_vectors[1886] },
		{ cube_vectors[1887], cube_vectors[1888], cube_vectors[1889] },
		{ cube_vectors[1890], cube_vectors[1891], cube_vectors[1892] },
		{ cube_vectors[1893], cube_vectors[1894], cube_vectors[1895] },
		{ cube_vectors[1896], cube_vectors[1897], cube_vectors[1898] },
		{ cube_vectors[1899], cube_vectors[1900], cube_vectors[1901] },
		{ cube_vectors[1902], cube_vectors[1903], cube_vectors[1904] },
		{ cube_vectors[1905], cube_vectors[1906], cube_vectors[1907] },
		{ cube_vectors[1908], cube_vectors[1909], cube_vectors[1910] },
		{ cube_vectors[1911], cube_vectors[1912], cube_vectors[1913] },
		{ cube_vectors[1914], cube_vectors[1915], cube_vectors[1916] },
		{ cube_vectors[1917], cube_vectors[1918], cube_vectors[1919] },
		{ cube_vectors[1920], cube_vectors[1921], cube_vectors[1922] },
		{ cube_vectors[1923], cube_vectors[1924], cube_vectors[1925] },
		{ cube_vectors[1926], cube_vectors[1927], cube_vectors[1928] },
		{ cube_vectors[1929], cube_vectors[1930], cube_vectors[1931] },
		{ cube_vectors[1932], cube_vectors[1933], cube_vectors[1934] },
		{ cube_vectors[1935], cube_vectors[1936], cube_vectors[1937] },
		{ cube_vectors[1938], cube_vectors[1939], cube_vectors[1940] },
		{ cube_vectors[1941], cube_vectors[1942], cube_vectors[1943] },
		{ cube_vectors[1944], cube_vectors[1945], cube_vectors[1946] },
		{ cube_vectors[1947], cube_vectors[1948], cube_vectors[1949] },
		{ cube_vectors[1950], cube_vectors[1951], cube_vectors[1952] },
		{ cube_vectors[1953], cube_vectors[1954], cube_vectors[1955] },
		{ cube_vectors[1956], cube_vectors[1957], cube_vectors[1958] },
		{ cube_vectors[1959], cube_vectors[1960], cube_vectors[1961] },
		{ cube_vectors[1962], cube_vectors[1963], cube_vectors[1964] },
		{ cube_vectors[1965], cube_vectors[1966], cube_vectors[1967] },
		{ cube_vectors[1968], cube_vectors[1969], cube_vectors[1970] },
		{ cube_vectors[1971], cube_vectors[1972], cube_vectors[1973] },
		{ cube_vectors[1974], cube_vectors[1975], cube_vectors[1976] },
		{ cube_vectors[1977], cube_vectors[1978], cube_vectors[1979] },
		{ cube_vectors[1980], cube_vectors[1981], cube_vectors[1982] },
		{ cube_vectors[1983], cube_vectors[1984], cube_vectors[1985] },
		{ cube_vectors[1986], cube_vectors[1987], cube_vectors[1988] },
		{ cube_vectors[1989], cube_vectors[1990], cube_vectors[1991] },
		{ cube_vectors[1992], cube_vectors[1993], cube_vectors[1994] },
		{ cube_vectors[1995], cube_vectors[1996], cube_vectors[1997] },
		{ cube_vectors[1998], cube_vectors[1999], cube_vectors[2000] },
		{ cube_vectors[2001], cube_vectors[2002], cube_vectors[2003] },
		{ cube_vectors[2004], cube_vectors[2005], cube_vectors[2006] },
		{ cube_vectors[2007], cube_vectors[2008], cube_vectors[2009] },
		{ cube_vectors[2010], cube_vectors[2011], cube_vectors[2012] },
		{ cube_vectors[2013], cube_vectors[2014], cube_vectors[2015] },
		{ cube_vectors[2016], cube_vectors[2017], cube_vectors[2018] },
		{ cube_vectors[2019], cube_vectors[2020], cube_vectors[2021] },
		{ cube_vectors[2022], cube_vectors[2023], cube_vectors[2024] },
		{ cube_vectors[2025], cube_vectors[2026], cube_vectors[2027] },
		{ cube_vectors[2028], cube_vectors[2029], cube_vectors[2030] },
		{ cube_vectors[2031], cube_vectors[2032], cube_vectors[2033] },
		{ cube_vectors[2034], cube_vectors[2035], cube_vectors[2036] },
		{ cube_vectors[2037], cube_vectors[2038], cube_vectors[2039] },
		{ cube_vectors[2040], cube_vectors[2041], cube_vectors[2042] },
		{ cube_vectors[2043], cube_vectors[2044], cube_vectors[2045] },
		{ cube_vectors[2046], cube_vectors[2047], cube_vectors[2048] },
		{ cube_vectors[2049], cube_vectors[2050], cube_vectors[2051] },
		{ cube_vectors[2052], cube_vectors[2053], cube_vectors[2054] },
		{ cube_vectors[2055], cube_vectors[2056], cube_vectors[2057] },
		{ cube_vectors[2058], cube_vectors[2059], cube_vectors[2060] },
		{ cube_vectors[2061], cube_vectors[2062], cube_vectors[2063] },
		{ cube_vectors[2064], cube_vectors[2065], cube_vectors[2066] },
		{ cube_vectors[2067], cube_vectors[2068], cube_vectors[2069] },
		{ cube_vectors[2070], cube_vectors[2071], cube_vectors[2072] },
		{ cube_vectors[2073], cube_vectors[2074], cube_vectors[2075] },
		{ cube_vectors[2076], cube_vectors[2077], cube_vectors[2078] },
		{ cube_vectors[2079], cube_vectors[2080], cube_vectors[2081] },
		{ cube_vectors[2082], cube_vectors[2083], cube_vectors[2084] },
		{ cube_vectors[2085], cube_vectors[2086], cube_vectors[2087] },
		{ cube_vectors[2088], cube_vectors[2089], cube_vectors[2090] },
		{ cube_vectors[2091], cube_vectors[2092], cube_vectors[2093] },
		{ cube_vectors[2094], cube_vectors[2095], cube_vectors[2096] },
		{ cube_vectors[2097], cube_vectors[2098], cube_vectors[2099] },
		{ cube_vectors[2100], cube_vectors[2101], cube_vectors[2102] },
		{ cube_vectors[2103], cube_vectors[2104], cube_vectors[2105] },
		{ cube_vectors[2106], cube_vectors[2107], cube_vectors[2108] },
		{ cube_vectors[2109], cube_vectors[2110], cube_vectors[2111] },
		{ cube_vectors[2112], cube_vectors[2113], cube_vectors[2114] },
		{ cube_vectors[2115], cube_vectors[2116], cube_vectors[2117] },
		{ cube_vectors[2118], cube_vectors[2119], cube_vectors[2120] },
		{ cube_vectors[2121], cube_vectors[2122], cube_vectors[2123] },
		{ cube_vectors[2124], cube_vectors[2125], cube_vectors[2126] },
		{ cube_vectors[2127], cube_vectors[2128], cube_vectors[2129] },
		{ cube_vectors[2130], cube_vectors[2131], cube_vectors[2132] },
		{ cube_vectors[2133], cube_vectors[2134], cube_vectors[2135] },
		{ cube_vectors[2136], cube_vectors[2137], cube_vectors[2138] },
		{ cube_vectors[2139], cube_vectors[2140], cube_vectors[2141] },
		{ cube_vectors[2142], cube_vectors[2143], cube_vectors[2144] },
		{ cube_vectors[2145], cube_vectors[2146], cube_vectors[2147] },
		{ cube_vectors[2148], cube_vectors[2149], cube_vectors[2150] },
		{ cube_vectors[2151], cube_vectors[2152], cube_vectors[2153] },
		{ cube_vectors[2154], cube_vectors[2155], cube_vectors[2156] },
		{ cube_vectors[2157], cube_vectors[2158], cube_vectors[2159] },
		{ cube_vectors[2160], cube_vectors[2161], cube_vectors[2162] },
		{ cube_vectors[2163], cube_vectors[2164], cube_vectors[2165] },
		{ cube_vectors[2166], cube_vectors[2167], cube_vectors[2168] },
		{ cube_vectors[2169], cube_vectors[2170], cube_vectors[2171] },
		{ cube_vectors[2172], cube_vectors[2173], cube_vectors[2174] },
		{ cube_vectors[2175], cube_vectors[2176], cube_vectors[2177] },
		{ cube_vectors[2178], cube_vectors[2179], cube_vectors[2180] },
		{ cube_vectors[2181], cube_vectors[2182], cube_vectors[2183] },
		{ cube_vectors[2184], cube_vectors[2185], cube_vectors[2186] },
		{ cube_vectors[2187], cube_vectors[2188], cube_vectors[2189] },
		{ cube_vectors[2190], cube_vectors[2191], cube_vectors[2192] },
		{ cube_vectors[2193], cube_vectors[2194], cube_vectors[2195] },
		{ cube_vectors[2196], cube_vectors[2197], cube_vectors[2198] },
		{ cube_vectors[2199], cube_vectors[2200], cube_vectors[2201] },
		{ cube_vectors[2202], cube_vectors[2203], cube_vectors[2204] },
		{ cube_vectors[2205], cube_vectors[2206], cube_vectors[2207] },
		{ cube_vectors[2208], cube_vectors[2209], cube_vectors[2210] },
		{ cube_vectors[2211], cube_vectors[2212], cube_vectors[2213] },
		{ cube_vectors[2214], cube_vectors[2215], cube_vectors[2216] },
		{ cube_vectors[2217], cube_vectors[2218], cube_vectors[2219] },
		{ cube_vectors[2220], cube_vectors[2221], cube_vectors[2222] },
		{ cube_vectors[2223], cube_vectors[2224], cube_vectors[2225] },
		{ cube_vectors[2226], cube_vectors[2227], cube_vectors[2228] },
		{ cube_vectors[2229], cube_vectors[2230], cube_vectors[2231] },
		{ cube_vectors[2232], cube_vectors[2233], cube_vectors[2234] },
		{ cube_vectors[2235], cube_vectors[2236], cube_vectors[2237] },
		{ cube_vectors[2238], cube_vectors[2239], cube_vectors[2240] },
		{ cube_vectors[2241], cube_vectors[2242], cube_vectors[2243] },
		{ cube_vectors[2244], cube_vectors[2245], cube_vectors[2246] },
		{ cube_vectors[2247], cube_vectors[2248], cube_vectors[2249] },
		{ cube_vectors[2250], cube_vectors[2251], cube_vectors[2252] },
		{ cube_vectors[2253], cube_vectors[2254], cube_vectors[2255] },
		{ cube_vectors[2256], cube_vectors[2257], cube_vectors[2258] },
		{ cube_vectors[2259], cube_vectors[2260], cube_vectors[2261] },
		{ cube_vectors[2262], cube_vectors[2263], cube_vectors[2264] },
		{ cube_vectors[2265], cube_vectors[2266], cube_vectors[2267] },
		{ cube_vectors[2268], cube_vectors[2269], cube_vectors[2270] },
		{ cube_vectors[2271], cube_vectors[2272], cube_vectors[2273] },
		{ cube_vectors[2274], cube_vectors[2275], cube_vectors[2276] },
		{ cube_vectors[2277], cube_vectors[2278], cube_vectors[2279] },
		{ cube_vectors[2280], cube_vectors[2281], cube_vectors[2282] },
		{ cube_vectors[2283], cube_vectors[2284], cube_vectors[2285] },
		{ cube_vectors[2286], cube_vectors[2287], cube_vectors[2288] },
		{ cube_vectors[2289], cube_vectors[2290], cube_vectors[2291] },
		{ cube_vectors[2292], cube_vectors[2293], cube_vectors[2294] },
		{ cube_vectors[2295], cube_vectors[2296], cube_vectors[2297] },
		{ cube_vectors[2298], cube_vectors[2299], cube_vectors[2300] },
		{ cube_vectors[2301], cube_vectors[2302], cube_vectors[2303] },
		{ cube_vectors[2304], cube_vectors[2305], cube_vectors[2306] },
		{ cube_vectors[2307], cube_vectors[2308], cube_vectors[2309] },
		{ cube_vectors[2310], cube_vectors[2311], cube_vectors[2312] },
		{ cube_vectors[2313], cube_vectors[2314], cube_vectors[2315] },
		{ cube_vectors[2316], cube_vectors[2317], cube_vectors[2318] },
		{ cube_vectors[2319], cube_vectors[2320], cube_vectors[2321] },
		{ cube_vectors[2322], cube_vectors[2323], cube_vectors[2324] },
		{ cube_vectors[2325], cube_vectors[2326], cube_vectors[2327] },
		{ cube_vectors[2328], cube_vectors[2329], cube_vectors[2330] },
		{ cube_vectors[2331], cube_vectors[2332], cube_vectors[2333] },
		{ cube_vectors[2334], cube_vectors[2335], cube_vectors[2336] },
		{ cube_vectors[2337], cube_vectors[2338], cube_vectors[2339] },
		{ cube_vectors[2340], cube_vectors[2341], cube_vectors[2342] },
		{ cube_vectors[2343], cube_vectors[2344], cube_vectors[2345] },
		{ cube_vectors[2346], cube_vectors[2347], cube_vectors[2348] },
		{ cube_vectors[2349], cube_vectors[2350], cube_vectors[2351] },
		{ cube_vectors[2352], cube_vectors[2353], cube_vectors[2354] },
		{ cube_vectors[2355], cube_vectors[2356], cube_vectors[2357] },
		{ cube_vectors[2358], cube_vectors[2359], cube_vectors[2360] },
		{ cube_vectors[2361], cube_vectors[2362], cube_vectors[2363] },
		{ cube_vectors[2364], cube_vectors[2365], cube_vectors[2366] },
		{ cube_vectors[2367], cube_vectors[2368], cube_vectors[2369] },
		{ cube_vectors[2370], cube_vectors[2371], cube_vectors[2372] },
		{ cube_vectors[2373], cube_vectors[2374], cube_vectors[2375] },
		{ cube_vectors[2376], cube_vectors[2377], cube_vectors[2378] },
		{ cube_vectors[2379], cube_vectors[2380], cube_vectors[2381] },
		{ cube_vectors[2382], cube_vectors[2383], cube_vectors[2384] },
		{ cube_vectors[2385], cube_vectors[2386], cube_vectors[2387] },
		{ cube_vectors[2388], cube_vectors[2389], cube_vectors[2390] },
		{ cube_vectors[2391], cube_vectors[2392], cube_vectors[2393] },
		{ cube_vectors[2394], cube_vectors[2395], cube_vectors[2396] },
		{ cube_vectors[2397], cube_vectors[2398], cube_vectors[2399] }
		

	};
	

	


	


	m_vb = GraphEng::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(vertex_list);

	unsigned int index_list[] =
	{
		
		0,1,2,2,3,0,4,5,6,6,7,4,1,6,5,5,2,1,7,0,3,3,4,7,3,2,5,5,4,3,7,6,1,1,0,7,
		8,9,10,10,11,8,12,13,14,14,15,12,9,14,13,13,10,9,15,8,11,11,12,15,11,10,13,13,12,11,15,14,9,9,8,15,
		16,17,18,18,19,16,20,21,22,22,23,20,17,22,21,21,18,17,23,16,19,19,20,23,19,18,21,21,20,19,23,22,17,17,16,23,
		24,25,26,26,27,24,28,29,30,30,31,28,25,30,29,29,26,25,31,24,27,27,28,31,27,26,29,29,28,27,31,30,25,25,24,31,
		32,33,34,34,35,32,36,37,38,38,39,36,33,38,37,37,34,33,39,32,35,35,36,39,35,34,37,37,36,35,39,38,33,33,32,39,
		40,41,42,42,43,40,44,45,46,46,47,44,41,46,45,45,42,41,47,40,43,43,44,47,43,42,45,45,44,43,47,46,41,41,40,47,
		48,49,50,50,51,48,52,53,54,54,55,52,49,54,53,53,50,49,55,48,51,51,52,55,51,50,53,53,52,51,55,54,49,49,48,55,
		56,57,58,58,59,56,60,61,62,62,63,60,57,62,61,61,58,57,63,56,59,59,60,63,59,58,61,61,60,59,63,62,57,57,56,63,
		64,65,66,66,67,64,68,69,70,70,71,68,65,70,69,69,66,65,71,64,67,67,68,71,67,66,69,69,68,67,71,70,65,65,64,71,
		72,73,74,74,75,72,76,77,78,78,79,76,73,78,77,77,74,73,79,72,75,75,76,79,75,74,77,77,76,75,79,78,73,73,72,79,
		80,81,82,82,83,80,84,85,86,86,87,84,81,86,85,85,82,81,87,80,83,83,84,87,83,82,85,85,84,83,87,86,81,81,80,87,
		88,89,90,90,91,88,92,93,94,94,95,92,89,94,93,93,90,89,95,88,91,91,92,95,91,90,93,93,92,91,95,94,89,89,88,95,
		96,97,98,98,99,96,100,101,102,102,103,100,97,102,101,101,98,97,103,96,99,99,100,103,99,98,101,101,100,99,103,102,97,97,96,103,
		104,105,106,106,107,104,108,109,110,110,111,108,105,110,109,109,106,105,111,104,107,107,108,111,107,106,109,109,108,107,111,110,105,105,104,111,
		112,113,114,114,115,112,116,117,118,118,119,116,113,118,117,117,114,113,119,112,115,115,116,119,115,114,117,117,116,115,119,118,113,113,112,119,
		120,121,122,122,123,120,124,125,126,126,127,124,121,126,125,125,122,121,127,120,123,123,124,127,123,122,125,125,124,123,127,126,121,121,120,127,
		128,129,130,130,131,128,132,133,134,134,135,132,129,134,133,133,130,129,135,128,131,131,132,135,131,130,133,133,132,131,135,134,129,129,128,135,
		136,137,138,138,139,136,140,141,142,142,143,140,137,142,141,141,138,137,143,136,139,139,140,143,139,138,141,141,140,139,143,142,137,137,136,143,
		144,145,146,146,147,144,148,149,150,150,151,148,145,150,149,149,146,145,151,144,147,147,148,151,147,146,149,149,148,147,151,150,145,145,144,151,
		152,153,154,154,155,152,156,157,158,158,159,156,153,158,157,157,154,153,159,152,155,155,156,159,155,154,157,157,156,155,159,158,153,153,152,159,
		160,161,162,162,163,160,164,165,166,166,167,164,161,166,165,165,162,161,167,160,163,163,164,167,163,162,165,165,164,163,167,166,161,161,160,167,
		168,169,170,170,171,168,172,173,174,174,175,172,169,174,173,173,170,169,175,168,171,171,172,175,171,170,173,173,172,171,175,174,169,169,168,175,
		176,177,178,178,179,176,180,181,182,182,183,180,177,182,181,181,178,177,183,176,179,179,180,183,179,178,181,181,180,179,183,182,177,177,176,183,
		184,185,186,186,187,184,188,189,190,190,191,188,185,190,189,189,186,185,191,184,187,187,188,191,187,186,189,189,188,187,191,190,185,185,184,191,
		192,193,194,194,195,192,196,197,198,198,199,196,193,198,197,197,194,193,199,192,195,195,196,199,195,194,197,197,196,195,199,198,193,193,192,199,
		200,201,202,202,203,200,204,205,206,206,207,204,201,206,205,205,202,201,207,200,203,203,204,207,203,202,205,205,204,203,207,206,201,201,200,207,
		208,209,210,210,211,208,212,213,214,214,215,212,209,214,213,213,210,209,215,208,211,211,212,215,211,210,213,213,212,211,215,214,209,209,208,215,
		216,217,218,218,219,216,220,221,222,222,223,220,217,222,221,221,218,217,223,216,219,219,220,223,219,218,221,221,220,219,223,222,217,217,216,223,
		224,225,226,226,227,224,228,229,230,230,231,228,225,230,229,229,226,225,231,224,227,227,228,231,227,226,229,229,228,227,231,230,225,225,224,231,
		232,233,234,234,235,232,236,237,238,238,239,236,233,238,237,237,234,233,239,232,235,235,236,239,235,234,237,237,236,235,239,238,233,233,232,239,
		240,241,242,242,243,240,244,245,246,246,247,244,241,246,245,245,242,241,247,240,243,243,244,247,243,242,245,245,244,243,247,246,241,241,240,247,
		248,249,250,250,251,248,252,253,254,254,255,252,249,254,253,253,250,249,255,248,251,251,252,255,251,250,253,253,252,251,255,254,249,249,248,255,
		256,257,258,258,259,256,260,261,262,262,263,260,257,262,261,261,258,257,263,256,259,259,260,263,259,258,261,261,260,259,263,262,257,257,256,263,
		264,265,266,266,267,264,268,269,270,270,271,268,265,270,269,269,266,265,271,264,267,267,268,271,267,266,269,269,268,267,271,270,265,265,264,271,
		272,273,274,274,275,272,276,277,278,278,279,276,273,278,277,277,274,273,279,272,275,275,276,279,275,274,277,277,276,275,279,278,273,273,272,279,
		280,281,282,282,283,280,284,285,286,286,287,284,281,286,285,285,282,281,287,280,283,283,284,287,283,282,285,285,284,283,287,286,281,281,280,287,
		288,289,290,290,291,288,292,293,294,294,295,292,289,294,293,293,290,289,295,288,291,291,292,295,291,290,293,293,292,291,295,294,289,289,288,295,
		296,297,298,298,299,296,300,301,302,302,303,300,297,302,301,301,298,297,303,296,299,299,300,303,299,298,301,301,300,299,303,302,297,297,296,303,
		304,305,306,306,307,304,308,309,310,310,311,308,305,310,309,309,306,305,311,304,307,307,308,311,307,306,309,309,308,307,311,310,305,305,304,311,
		312,313,314,314,315,312,316,317,318,318,319,316,313,318,317,317,314,313,319,312,315,315,316,319,315,314,317,317,316,315,319,318,313,313,312,319,
		320,321,322,322,323,320,324,325,326,326,327,324,321,326,325,325,322,321,327,320,323,323,324,327,323,322,325,325,324,323,327,326,321,321,320,327,
		328,329,330,330,331,328,332,333,334,334,335,332,329,334,333,333,330,329,335,328,331,331,332,335,331,330,333,333,332,331,335,334,329,329,328,335,
		336,337,338,338,339,336,340,341,342,342,343,340,337,342,341,341,338,337,343,336,339,339,340,343,339,338,341,341,340,339,343,342,337,337,336,343,
		344,345,346,346,347,344,348,349,350,350,351,348,345,350,349,349,346,345,351,344,347,347,348,351,347,346,349,349,348,347,351,350,345,345,344,351,
		352,353,354,354,355,352,356,357,358,358,359,356,353,358,357,357,354,353,359,352,355,355,356,359,355,354,357,357,356,355,359,358,353,353,352,359,
		360,361,362,362,363,360,364,365,366,366,367,364,361,366,365,365,362,361,367,360,363,363,364,367,363,362,365,365,364,363,367,366,361,361,360,367,
		368,369,370,370,371,368,372,373,374,374,375,372,369,374,373,373,370,369,375,368,371,371,372,375,371,370,373,373,372,371,375,374,369,369,368,375,
		376,377,378,378,379,376,380,381,382,382,383,380,377,382,381,381,378,377,383,376,379,379,380,383,379,378,381,381,380,379,383,382,377,377,376,383,
		384,385,386,386,387,384,388,389,390,390,391,388,385,390,389,389,386,385,391,384,387,387,388,391,387,386,389,389,388,387,391,390,385,385,384,391,
		392,393,394,394,395,392,396,397,398,398,399,396,393,398,397,397,394,393,399,392,395,395,396,399,395,394,397,397,396,395,399,398,393,393,392,399,
		400,401,402,402,403,400,404,405,406,406,407,404,401,406,405,405,402,401,407,400,403,403,404,407,403,402,405,405,404,403,407,406,401,401,400,407,
		408,409,410,410,411,408,412,413,414,414,415,412,409,414,413,413,410,409,415,408,411,411,412,415,411,410,413,413,412,411,415,414,409,409,408,415,
		416,417,418,418,419,416,420,421,422,422,423,420,417,422,421,421,418,417,423,416,419,419,420,423,419,418,421,421,420,419,423,422,417,417,416,423,
		424,425,426,426,427,424,428,429,430,430,431,428,425,430,429,429,426,425,431,424,427,427,428,431,427,426,429,429,428,427,431,430,425,425,424,431,
		432,433,434,434,435,432,436,437,438,438,439,436,433,438,437,437,434,433,439,432,435,435,436,439,435,434,437,437,436,435,439,438,433,433,432,439,
		440,441,442,442,443,440,444,445,446,446,447,444,441,446,445,445,442,441,447,440,443,443,444,447,443,442,445,445,444,443,447,446,441,441,440,447,
		448,449,450,450,451,448,452,453,454,454,455,452,449,454,453,453,450,449,455,448,451,451,452,455,451,450,453,453,452,451,455,454,449,449,448,455,
		456,457,458,458,459,456,460,461,462,462,463,460,457,462,461,461,458,457,463,456,459,459,460,463,459,458,461,461,460,459,463,462,457,457,456,463,
		464,465,466,466,467,464,468,469,470,470,471,468,465,470,469,469,466,465,471,464,467,467,468,471,467,466,469,469,468,467,471,470,465,465,464,471,
		472,473,474,474,475,472,476,477,478,478,479,476,473,478,477,477,474,473,479,472,475,475,476,479,475,474,477,477,476,475,479,478,473,473,472,479,
		480,481,482,482,483,480,484,485,486,486,487,484,481,486,485,485,482,481,487,480,483,483,484,487,483,482,485,485,484,483,487,486,481,481,480,487,
		488,489,490,490,491,488,492,493,494,494,495,492,489,494,493,493,490,489,495,488,491,491,492,495,491,490,493,493,492,491,495,494,489,489,488,495,
		496,497,498,498,499,496,500,501,502,502,503,500,497,502,501,501,498,497,503,496,499,499,500,503,499,498,501,501,500,499,503,502,497,497,496,503,
		504,505,506,506,507,504,508,509,510,510,511,508,505,510,509,509,506,505,511,504,507,507,508,511,507,506,509,509,508,507,511,510,505,505,504,511,
		512,513,514,514,515,512,516,517,518,518,519,516,513,518,517,517,514,513,519,512,515,515,516,519,515,514,517,517,516,515,519,518,513,513,512,519,
		520,521,522,522,523,520,524,525,526,526,527,524,521,526,525,525,522,521,527,520,523,523,524,527,523,522,525,525,524,523,527,526,521,521,520,527,
		528,529,530,530,531,528,532,533,534,534,535,532,529,534,533,533,530,529,535,528,531,531,532,535,531,530,533,533,532,531,535,534,529,529,528,535,
		536,537,538,538,539,536,540,541,542,542,543,540,537,542,541,541,538,537,543,536,539,539,540,543,539,538,541,541,540,539,543,542,537,537,536,543,
		544,545,546,546,547,544,548,549,550,550,551,548,545,550,549,549,546,545,551,544,547,547,548,551,547,546,549,549,548,547,551,550,545,545,544,551,
		552,553,554,554,555,552,556,557,558,558,559,556,553,558,557,557,554,553,559,552,555,555,556,559,555,554,557,557,556,555,559,558,553,553,552,559,
		560,561,562,562,563,560,564,565,566,566,567,564,561,566,565,565,562,561,567,560,563,563,564,567,563,562,565,565,564,563,567,566,561,561,560,567,
		568,569,570,570,571,568,572,573,574,574,575,572,569,574,573,573,570,569,575,568,571,571,572,575,571,570,573,573,572,571,575,574,569,569,568,575,
		576,577,578,578,579,576,580,581,582,582,583,580,577,582,581,581,578,577,583,576,579,579,580,583,579,578,581,581,580,579,583,582,577,577,576,583,
		584,585,586,586,587,584,588,589,590,590,591,588,585,590,589,589,586,585,591,584,587,587,588,591,587,586,589,589,588,587,591,590,585,585,584,591,
		592,593,594,594,595,592,596,597,598,598,599,596,593,598,597,597,594,593,599,592,595,595,596,599,595,594,597,597,596,595,599,598,593,593,592,599,
		600,601,602,602,603,600,604,605,606,606,607,604,601,606,605,605,602,601,607,600,603,603,604,607,603,602,605,605,604,603,607,606,601,601,600,607,
		608,609,610,610,611,608,612,613,614,614,615,612,609,614,613,613,610,609,615,608,611,611,612,615,611,610,613,613,612,611,615,614,609,609,608,615,
		616,617,618,618,619,616,620,621,622,622,623,620,617,622,621,621,618,617,623,616,619,619,620,623,619,618,621,621,620,619,623,622,617,617,616,623,
		624,625,626,626,627,624,628,629,630,630,631,628,625,630,629,629,626,625,631,624,627,627,628,631,627,626,629,629,628,627,631,630,625,625,624,631,
		632,633,634,634,635,632,636,637,638,638,639,636,633,638,637,637,634,633,639,632,635,635,636,639,635,634,637,637,636,635,639,638,633,633,632,639,
		640,641,642,642,643,640,644,645,646,646,647,644,641,646,645,645,642,641,647,640,643,643,644,647,643,642,645,645,644,643,647,646,641,641,640,647,
		648,649,650,650,651,648,652,653,654,654,655,652,649,654,653,653,650,649,655,648,651,651,652,655,651,650,653,653,652,651,655,654,649,649,648,655,
		656,657,658,658,659,656,660,661,662,662,663,660,657,662,661,661,658,657,663,656,659,659,660,663,659,658,661,661,660,659,663,662,657,657,656,663,
		664,665,666,666,667,664,668,669,670,670,671,668,665,670,669,669,666,665,671,664,667,667,668,671,667,666,669,669,668,667,671,670,665,665,664,671,
		672,673,674,674,675,672,676,677,678,678,679,676,673,678,677,677,674,673,679,672,675,675,676,679,675,674,677,677,676,675,679,678,673,673,672,679,
		680,681,682,682,683,680,684,685,686,686,687,684,681,686,685,685,682,681,687,680,683,683,684,687,683,682,685,685,684,683,687,686,681,681,680,687,
		688,689,690,690,691,688,692,693,694,694,695,692,689,694,693,693,690,689,695,688,691,691,692,695,691,690,693,693,692,691,695,694,689,689,688,695,
		696,697,698,698,699,696,700,701,702,702,703,700,697,702,701,701,698,697,703,696,699,699,700,703,699,698,701,701,700,699,703,702,697,697,696,703,
		704,705,706,706,707,704,708,709,710,710,711,708,705,710,709,709,706,705,711,704,707,707,708,711,707,706,709,709,708,707,711,710,705,705,704,711,
		712,713,714,714,715,712,716,717,718,718,719,716,713,718,717,717,714,713,719,712,715,715,716,719,715,714,717,717,716,715,719,718,713,713,712,719,
		720,721,722,722,723,720,724,725,726,726,727,724,721,726,725,725,722,721,727,720,723,723,724,727,723,722,725,725,724,723,727,726,721,721,720,727,
		728,729,730,730,731,728,732,733,734,734,735,732,729,734,733,733,730,729,735,728,731,731,732,735,731,730,733,733,732,731,735,734,729,729,728,735,
		736,737,738,738,739,736,740,741,742,742,743,740,737,742,741,741,738,737,743,736,739,739,740,743,739,738,741,741,740,739,743,742,737,737,736,743,
		744,745,746,746,747,744,748,749,750,750,751,748,745,750,749,749,746,745,751,744,747,747,748,751,747,746,749,749,748,747,751,750,745,745,744,751,
		752,753,754,754,755,752,756,757,758,758,759,756,753,758,757,757,754,753,759,752,755,755,756,759,755,754,757,757,756,755,759,758,753,753,752,759,
		760,761,762,762,763,760,764,765,766,766,767,764,761,766,765,765,762,761,767,760,763,763,764,767,763,762,765,765,764,763,767,766,761,761,760,767,
		768,769,770,770,771,768,772,773,774,774,775,772,769,774,773,773,770,769,775,768,771,771,772,775,771,770,773,773,772,771,775,774,769,769,768,775,
		776,777,778,778,779,776,780,781,782,782,783,780,777,782,781,781,778,777,783,776,779,779,780,783,779,778,781,781,780,779,783,782,777,777,776,783,
		784,785,786,786,787,784,788,789,790,790,791,788,785,790,789,789,786,785,791,784,787,787,788,791,787,786,789,789,788,787,791,790,785,785,784,791,
		792,793,794,794,795,792,796,797,798,798,799,796,793,798,797,797,794,793,799,792,795,795,796,799,795,794,797,797,796,795,799,798,793,793,792,799
	};


	m_ib = GraphEng::get()->createIndexBuffer();
	UINT size_index_list = ARRAYSIZE(index_list);

	m_ib->load(index_list, size_index_list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphEng::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphEng::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(vertex_list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphEng::get()->releaseCompiledShader();

	GraphEng::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphEng::get()->createPixelShader(shader_byte_code, size_shader);

	GraphEng::get()->releaseCompiledShader();
	std::cout << enemies << std::endl;
}


void AppWindow::UpdateMap(int num, int enemy_killed)
{
	std::string filename = "";
	if (num == 0)
	{
		std::cout << num << std::endl;
		filename = "Lvl1.txt";
	}
	if (num == 1)
	{
		std::cout << num << std::endl;
		filename = "Lvl2.txt";
	}
	if (num == 2)
	{
		std::cout << num << std::endl;
		filename = "Lvl3.txt";
	}
	std::ifstream input_file(filename);
	if (!input_file.is_open()) {
		std::cout << "Could not open the file - '"
			<< filename << "'" << std::endl;
	}

	char lvl_data[100];
	char byte = 0;
	int x = 0;
	

	Vector3D cube_vectors[2400];
	int i = 0;
	while (input_file.get(byte) && i < 100)
	{
		if (byte == 'X')
		{
			int pos = i * 24;
			cube_vectors[pos] = Vector3D(-0.5f, -0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 1] = Vector3D(0, 0, 0);
			cube_vectors[pos + 2] = Vector3D(0.0f, 0.4f, 0.f);
			cube_vectors[pos + 3] = Vector3D(-0.5f, 0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 4] = Vector3D(0, 0, 0);
			cube_vectors[pos + 5] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 6] = Vector3D(0.5f, 0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 7] = Vector3D(0, 0, 0);
			cube_vectors[pos + 8] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 9] = Vector3D(0.5f, -0.5f, -0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 10] = Vector3D(0, 0, 0);
			cube_vectors[pos + 11] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 12] = Vector3D(0.5f, -0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 13] = Vector3D(0, 0, 0);
			cube_vectors[pos + 14] = Vector3D(0.0f, 0.4f, 0);
			cube_vectors[pos + 15] = Vector3D(0.5f, 0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 16] = Vector3D(0, 0, 0);
			cube_vectors[pos + 17] = Vector3D(0, 0.4f, 0.0f);
			cube_vectors[pos + 18] = Vector3D(-0.5f, 0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 19] = Vector3D(0, 0, 0);
			cube_vectors[pos + 20] = Vector3D(0, 0.4f, 0.0f);
			cube_vectors[pos + 21] = Vector3D(-0.5f, -0.5f, 0.5f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
			cube_vectors[pos + 22] = Vector3D(0, 0, 0);
			cube_vectors[pos + 23] = Vector3D(0, 0.4f, 0);

			cube_bounds[i] = { static_cast<float>(-0.5f + 0.95 * (i % 10)), static_cast<float>(0.5f + 0.95 * (i % 10)), -0.5f + static_cast<float>(0.95 * floor(i / 10)), 0.5f + static_cast<float>(0.95 * floor(i / 10)), false, false };
			

			i++;

		}
		else if (byte == ' ')
		{
			int pos = i * 24;
			cube_vectors[pos] = Vector3D(0, 0, 0);
			cube_vectors[pos + 1] = Vector3D(0, 0, 0);
			cube_vectors[pos + 2] = Vector3D(0, 0, 0);
			cube_vectors[pos + 3] = Vector3D(0, 0, 0);
			cube_vectors[pos + 4] = Vector3D(0, 0, 0);
			cube_vectors[pos + 5] = Vector3D(0, 0, 0);
			cube_vectors[pos + 6] = Vector3D(0, 0, 0);
			cube_vectors[pos + 7] = Vector3D(0, 0, 0);
			cube_vectors[pos + 8] = Vector3D(0, 0, 0);
			cube_vectors[pos + 9] = Vector3D(0, 0, 0);
			cube_vectors[pos + 10] = Vector3D(0, 0, 0);
			cube_vectors[pos + 11] = Vector3D(0, 0, 0);
			cube_vectors[pos + 12] = Vector3D(0, 0, 0);
			cube_vectors[pos + 13] = Vector3D(0, 0, 0);
			cube_vectors[pos + 14] = Vector3D(0, 0, 0);
			cube_vectors[pos + 15] = Vector3D(0, 0, 0);
			cube_vectors[pos + 16] = Vector3D(0, 0, 0);
			cube_vectors[pos + 17] = Vector3D(0, 0, 0);
			cube_vectors[pos + 18] = Vector3D(0, 0, 0);
			cube_vectors[pos + 19] = Vector3D(0, 0, 0);
			cube_vectors[pos + 20] = Vector3D(0, 0, 0);
			cube_vectors[pos + 21] = Vector3D(0, 0, 0);
			cube_vectors[pos + 22] = Vector3D(0, 0, 0);
			cube_vectors[pos + 23] = Vector3D(0, 0, 0);

			cube_bounds[i] = { 0, 0, 0, 0, false };
			

			i++;
		}
		else if (byte == 'E')
		{
			if (cube_bounds[i].alive) 
			{
				int pos = i * 24;
				cube_vectors[pos] = Vector3D(-0.25f, -0.5f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 1] = Vector3D(0, 0, 0);
				cube_vectors[pos + 2] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 3] = Vector3D(-0.25f, 0.1f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 4] = Vector3D(0, 0, 0);
				cube_vectors[pos + 5] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 6] = Vector3D(0.25f, 0.1f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 7] = Vector3D(0, 0, 0);
				cube_vectors[pos + 8] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 9] = Vector3D(0.25f, -0.5f, -0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 10] = Vector3D(0, 0, 0);
				cube_vectors[pos + 11] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 12] = Vector3D(0.25f, -0.5f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 13] = Vector3D(0, 0, 0);
				cube_vectors[pos + 14] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 15] = Vector3D(0.25f, 0.1f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 16] = Vector3D(0, 0, 0);
				cube_vectors[pos + 17] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 18] = Vector3D(-0.25f, 0.1f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 19] = Vector3D(0, 0, 0);
				cube_vectors[pos + 20] = Vector3D(0.4f, 0.0f, 0.0f);
				cube_vectors[pos + 21] = Vector3D(-0.25f, -0.5f, 0.25f) + Vector3D(0.95 * (i % 10), 0.0f, 0.95 * floor(i / 10));
				cube_vectors[pos + 22] = Vector3D(0, 0, 0);
				cube_vectors[pos + 23] = Vector3D(0.4f, 0.0f, 0);

						
			}
			i++;
			
		}
		else if (byte == 'P')
		{
			cube_bounds[i] = { 0, 0, 0, 0, false };
			
			i++;

		}


	}
	std::cout << enemies << std::endl;

	int pos = enemy_killed * 24;
	cube_vectors[pos] = Vector3D(0, 0, 0);
	cube_vectors[pos + 1] = Vector3D(0, 0, 0);
	cube_vectors[pos + 2] = Vector3D(0, 0, 0);
	cube_vectors[pos + 3] = Vector3D(0, 0, 0);
	cube_vectors[pos + 4] = Vector3D(0, 0, 0);
	cube_vectors[pos + 5] = Vector3D(0, 0, 0);
	cube_vectors[pos + 6] = Vector3D(0, 0, 0);
	cube_vectors[pos + 7] = Vector3D(0, 0, 0);
	cube_vectors[pos + 8] = Vector3D(0, 0, 0);
	cube_vectors[pos + 9] = Vector3D(0, 0, 0);
	cube_vectors[pos + 10] = Vector3D(0, 0, 0);
	cube_vectors[pos + 11] = Vector3D(0, 0, 0);
	cube_vectors[pos + 12] = Vector3D(0, 0, 0);
	cube_vectors[pos + 13] = Vector3D(0, 0, 0);
	cube_vectors[pos + 14] = Vector3D(0, 0, 0);
	cube_vectors[pos + 15] = Vector3D(0, 0, 0);
	cube_vectors[pos + 16] = Vector3D(0, 0, 0);
	cube_vectors[pos + 17] = Vector3D(0, 0, 0);
	cube_vectors[pos + 18] = Vector3D(0, 0, 0);
	cube_vectors[pos + 19] = Vector3D(0, 0, 0);
	cube_vectors[pos + 20] = Vector3D(0, 0, 0);
	cube_vectors[pos + 21] = Vector3D(0, 0, 0);
	cube_vectors[pos + 22] = Vector3D(0, 0, 0);
	cube_vectors[pos + 23] = Vector3D(0, 0, 0);

	cube_bounds[enemy_killed] = { 0, 0, 0, 0, true, false };


	

	vertex vertex_list[] =
	{
		{cube_vectors[0], cube_vectors[1], cube_vectors[2]} ,
		{ cube_vectors[3], cube_vectors[4], cube_vectors[5] },
		{ cube_vectors[6], cube_vectors[7], cube_vectors[8] },
		{ cube_vectors[9], cube_vectors[10], cube_vectors[11] },
		{ cube_vectors[12], cube_vectors[13], cube_vectors[14] },
		{ cube_vectors[15], cube_vectors[16], cube_vectors[17] },
		{ cube_vectors[18], cube_vectors[19], cube_vectors[20] },
		{ cube_vectors[21], cube_vectors[22], cube_vectors[23] },
		{ cube_vectors[24], cube_vectors[25], cube_vectors[26] },
		{ cube_vectors[27], cube_vectors[28], cube_vectors[29] },
		{ cube_vectors[30], cube_vectors[31], cube_vectors[32] },
		{ cube_vectors[33], cube_vectors[34], cube_vectors[35] },
		{ cube_vectors[36], cube_vectors[37], cube_vectors[38] },
		{ cube_vectors[39], cube_vectors[40], cube_vectors[41] },
		{ cube_vectors[42], cube_vectors[43], cube_vectors[44] },
		{ cube_vectors[45], cube_vectors[46], cube_vectors[47] },
		{ cube_vectors[48], cube_vectors[49], cube_vectors[50] },
		{ cube_vectors[51], cube_vectors[52], cube_vectors[53] },
		{ cube_vectors[54], cube_vectors[55], cube_vectors[56] },
		{ cube_vectors[57], cube_vectors[58], cube_vectors[59] },
		{ cube_vectors[60], cube_vectors[61], cube_vectors[62] },
		{ cube_vectors[63], cube_vectors[64], cube_vectors[65] },
		{ cube_vectors[66], cube_vectors[67], cube_vectors[68] },
		{ cube_vectors[69], cube_vectors[70], cube_vectors[71] },
		{ cube_vectors[72], cube_vectors[73], cube_vectors[74] },
		{ cube_vectors[75], cube_vectors[76], cube_vectors[77] },
		{ cube_vectors[78], cube_vectors[79], cube_vectors[80] },
		{ cube_vectors[81], cube_vectors[82], cube_vectors[83] },
		{ cube_vectors[84], cube_vectors[85], cube_vectors[86] },
		{ cube_vectors[87], cube_vectors[88], cube_vectors[89] },
		{ cube_vectors[90], cube_vectors[91], cube_vectors[92] },
		{ cube_vectors[93], cube_vectors[94], cube_vectors[95] },
		{ cube_vectors[96], cube_vectors[97], cube_vectors[98] },
		{ cube_vectors[99], cube_vectors[100], cube_vectors[101] },
		{ cube_vectors[102], cube_vectors[103], cube_vectors[104] },
		{ cube_vectors[105], cube_vectors[106], cube_vectors[107] },
		{ cube_vectors[108], cube_vectors[109], cube_vectors[110] },
		{ cube_vectors[111], cube_vectors[112], cube_vectors[113] },
		{ cube_vectors[114], cube_vectors[115], cube_vectors[116] },
		{ cube_vectors[117], cube_vectors[118], cube_vectors[119] },
		{ cube_vectors[120], cube_vectors[121], cube_vectors[122] },
		{ cube_vectors[123], cube_vectors[124], cube_vectors[125] },
		{ cube_vectors[126], cube_vectors[127], cube_vectors[128] },
		{ cube_vectors[129], cube_vectors[130], cube_vectors[131] },
		{ cube_vectors[132], cube_vectors[133], cube_vectors[134] },
		{ cube_vectors[135], cube_vectors[136], cube_vectors[137] },
		{ cube_vectors[138], cube_vectors[139], cube_vectors[140] },
		{ cube_vectors[141], cube_vectors[142], cube_vectors[143] },
		{ cube_vectors[144], cube_vectors[145], cube_vectors[146] },
		{ cube_vectors[147], cube_vectors[148], cube_vectors[149] },
		{ cube_vectors[150], cube_vectors[151], cube_vectors[152] },
		{ cube_vectors[153], cube_vectors[154], cube_vectors[155] },
		{ cube_vectors[156], cube_vectors[157], cube_vectors[158] },
		{ cube_vectors[159], cube_vectors[160], cube_vectors[161] },
		{ cube_vectors[162], cube_vectors[163], cube_vectors[164] },
		{ cube_vectors[165], cube_vectors[166], cube_vectors[167] },
		{ cube_vectors[168], cube_vectors[169], cube_vectors[170] },
		{ cube_vectors[171], cube_vectors[172], cube_vectors[173] },
		{ cube_vectors[174], cube_vectors[175], cube_vectors[176] },
		{ cube_vectors[177], cube_vectors[178], cube_vectors[179] },
		{ cube_vectors[180], cube_vectors[181], cube_vectors[182] },
		{ cube_vectors[183], cube_vectors[184], cube_vectors[185] },
		{ cube_vectors[186], cube_vectors[187], cube_vectors[188] },
		{ cube_vectors[189], cube_vectors[190], cube_vectors[191] },
		{ cube_vectors[192], cube_vectors[193], cube_vectors[194] },
		{ cube_vectors[195], cube_vectors[196], cube_vectors[197] },
		{ cube_vectors[198], cube_vectors[199], cube_vectors[200] },
		{ cube_vectors[201], cube_vectors[202], cube_vectors[203] },
		{ cube_vectors[204], cube_vectors[205], cube_vectors[206] },
		{ cube_vectors[207], cube_vectors[208], cube_vectors[209] },
		{ cube_vectors[210], cube_vectors[211], cube_vectors[212] },
		{ cube_vectors[213], cube_vectors[214], cube_vectors[215] },
		{ cube_vectors[216], cube_vectors[217], cube_vectors[218] },
		{ cube_vectors[219], cube_vectors[220], cube_vectors[221] },
		{ cube_vectors[222], cube_vectors[223], cube_vectors[224] },
		{ cube_vectors[225], cube_vectors[226], cube_vectors[227] },
		{ cube_vectors[228], cube_vectors[229], cube_vectors[230] },
		{ cube_vectors[231], cube_vectors[232], cube_vectors[233] },
		{ cube_vectors[234], cube_vectors[235], cube_vectors[236] },
		{ cube_vectors[237], cube_vectors[238], cube_vectors[239] },
		{ cube_vectors[240], cube_vectors[241], cube_vectors[242] },
		{ cube_vectors[243], cube_vectors[244], cube_vectors[245] },
		{ cube_vectors[246], cube_vectors[247], cube_vectors[248] },
		{ cube_vectors[249], cube_vectors[250], cube_vectors[251] },
		{ cube_vectors[252], cube_vectors[253], cube_vectors[254] },
		{ cube_vectors[255], cube_vectors[256], cube_vectors[257] },
		{ cube_vectors[258], cube_vectors[259], cube_vectors[260] },
		{ cube_vectors[261], cube_vectors[262], cube_vectors[263] },
		{ cube_vectors[264], cube_vectors[265], cube_vectors[266] },
		{ cube_vectors[267], cube_vectors[268], cube_vectors[269] },
		{ cube_vectors[270], cube_vectors[271], cube_vectors[272] },
		{ cube_vectors[273], cube_vectors[274], cube_vectors[275] },
		{ cube_vectors[276], cube_vectors[277], cube_vectors[278] },
		{ cube_vectors[279], cube_vectors[280], cube_vectors[281] },
		{ cube_vectors[282], cube_vectors[283], cube_vectors[284] },
		{ cube_vectors[285], cube_vectors[286], cube_vectors[287] },
		{ cube_vectors[288], cube_vectors[289], cube_vectors[290] },
		{ cube_vectors[291], cube_vectors[292], cube_vectors[293] },
		{ cube_vectors[294], cube_vectors[295], cube_vectors[296] },
		{ cube_vectors[297], cube_vectors[298], cube_vectors[299] },
		{ cube_vectors[300], cube_vectors[301], cube_vectors[302] },
		{ cube_vectors[303], cube_vectors[304], cube_vectors[305] },
		{ cube_vectors[306], cube_vectors[307], cube_vectors[308] },
		{ cube_vectors[309], cube_vectors[310], cube_vectors[311] },
		{ cube_vectors[312], cube_vectors[313], cube_vectors[314] },
		{ cube_vectors[315], cube_vectors[316], cube_vectors[317] },
		{ cube_vectors[318], cube_vectors[319], cube_vectors[320] },
		{ cube_vectors[321], cube_vectors[322], cube_vectors[323] },
		{ cube_vectors[324], cube_vectors[325], cube_vectors[326] },
		{ cube_vectors[327], cube_vectors[328], cube_vectors[329] },
		{ cube_vectors[330], cube_vectors[331], cube_vectors[332] },
		{ cube_vectors[333], cube_vectors[334], cube_vectors[335] },
		{ cube_vectors[336], cube_vectors[337], cube_vectors[338] },
		{ cube_vectors[339], cube_vectors[340], cube_vectors[341] },
		{ cube_vectors[342], cube_vectors[343], cube_vectors[344] },
		{ cube_vectors[345], cube_vectors[346], cube_vectors[347] },
		{ cube_vectors[348], cube_vectors[349], cube_vectors[350] },
		{ cube_vectors[351], cube_vectors[352], cube_vectors[353] },
		{ cube_vectors[354], cube_vectors[355], cube_vectors[356] },
		{ cube_vectors[357], cube_vectors[358], cube_vectors[359] },
		{ cube_vectors[360], cube_vectors[361], cube_vectors[362] },
		{ cube_vectors[363], cube_vectors[364], cube_vectors[365] },
		{ cube_vectors[366], cube_vectors[367], cube_vectors[368] },
		{ cube_vectors[369], cube_vectors[370], cube_vectors[371] },
		{ cube_vectors[372], cube_vectors[373], cube_vectors[374] },
		{ cube_vectors[375], cube_vectors[376], cube_vectors[377] },
		{ cube_vectors[378], cube_vectors[379], cube_vectors[380] },
		{ cube_vectors[381], cube_vectors[382], cube_vectors[383] },
		{ cube_vectors[384], cube_vectors[385], cube_vectors[386] },
		{ cube_vectors[387], cube_vectors[388], cube_vectors[389] },
		{ cube_vectors[390], cube_vectors[391], cube_vectors[392] },
		{ cube_vectors[393], cube_vectors[394], cube_vectors[395] },
		{ cube_vectors[396], cube_vectors[397], cube_vectors[398] },
		{ cube_vectors[399], cube_vectors[400], cube_vectors[401] },
		{ cube_vectors[402], cube_vectors[403], cube_vectors[404] },
		{ cube_vectors[405], cube_vectors[406], cube_vectors[407] },
		{ cube_vectors[408], cube_vectors[409], cube_vectors[410] },
		{ cube_vectors[411], cube_vectors[412], cube_vectors[413] },
		{ cube_vectors[414], cube_vectors[415], cube_vectors[416] },
		{ cube_vectors[417], cube_vectors[418], cube_vectors[419] },
		{ cube_vectors[420], cube_vectors[421], cube_vectors[422] },
		{ cube_vectors[423], cube_vectors[424], cube_vectors[425] },
		{ cube_vectors[426], cube_vectors[427], cube_vectors[428] },
		{ cube_vectors[429], cube_vectors[430], cube_vectors[431] },
		{ cube_vectors[432], cube_vectors[433], cube_vectors[434] },
		{ cube_vectors[435], cube_vectors[436], cube_vectors[437] },
		{ cube_vectors[438], cube_vectors[439], cube_vectors[440] },
		{ cube_vectors[441], cube_vectors[442], cube_vectors[443] },
		{ cube_vectors[444], cube_vectors[445], cube_vectors[446] },
		{ cube_vectors[447], cube_vectors[448], cube_vectors[449] },
		{ cube_vectors[450], cube_vectors[451], cube_vectors[452] },
		{ cube_vectors[453], cube_vectors[454], cube_vectors[455] },
		{ cube_vectors[456], cube_vectors[457], cube_vectors[458] },
		{ cube_vectors[459], cube_vectors[460], cube_vectors[461] },
		{ cube_vectors[462], cube_vectors[463], cube_vectors[464] },
		{ cube_vectors[465], cube_vectors[466], cube_vectors[467] },
		{ cube_vectors[468], cube_vectors[469], cube_vectors[470] },
		{ cube_vectors[471], cube_vectors[472], cube_vectors[473] },
		{ cube_vectors[474], cube_vectors[475], cube_vectors[476] },
		{ cube_vectors[477], cube_vectors[478], cube_vectors[479] },
		{ cube_vectors[480], cube_vectors[481], cube_vectors[482] },
		{ cube_vectors[483], cube_vectors[484], cube_vectors[485] },
		{ cube_vectors[486], cube_vectors[487], cube_vectors[488] },
		{ cube_vectors[489], cube_vectors[490], cube_vectors[491] },
		{ cube_vectors[492], cube_vectors[493], cube_vectors[494] },
		{ cube_vectors[495], cube_vectors[496], cube_vectors[497] },
		{ cube_vectors[498], cube_vectors[499], cube_vectors[500] },
		{ cube_vectors[501], cube_vectors[502], cube_vectors[503] },
		{ cube_vectors[504], cube_vectors[505], cube_vectors[506] },
		{ cube_vectors[507], cube_vectors[508], cube_vectors[509] },
		{ cube_vectors[510], cube_vectors[511], cube_vectors[512] },
		{ cube_vectors[513], cube_vectors[514], cube_vectors[515] },
		{ cube_vectors[516], cube_vectors[517], cube_vectors[518] },
		{ cube_vectors[519], cube_vectors[520], cube_vectors[521] },
		{ cube_vectors[522], cube_vectors[523], cube_vectors[524] },
		{ cube_vectors[525], cube_vectors[526], cube_vectors[527] },
		{ cube_vectors[528], cube_vectors[529], cube_vectors[530] },
		{ cube_vectors[531], cube_vectors[532], cube_vectors[533] },
		{ cube_vectors[534], cube_vectors[535], cube_vectors[536] },
		{ cube_vectors[537], cube_vectors[538], cube_vectors[539] },
		{ cube_vectors[540], cube_vectors[541], cube_vectors[542] },
		{ cube_vectors[543], cube_vectors[544], cube_vectors[545] },
		{ cube_vectors[546], cube_vectors[547], cube_vectors[548] },
		{ cube_vectors[549], cube_vectors[550], cube_vectors[551] },
		{ cube_vectors[552], cube_vectors[553], cube_vectors[554] },
		{ cube_vectors[555], cube_vectors[556], cube_vectors[557] },
		{ cube_vectors[558], cube_vectors[559], cube_vectors[560] },
		{ cube_vectors[561], cube_vectors[562], cube_vectors[563] },
		{ cube_vectors[564], cube_vectors[565], cube_vectors[566] },
		{ cube_vectors[567], cube_vectors[568], cube_vectors[569] },
		{ cube_vectors[570], cube_vectors[571], cube_vectors[572] },
		{ cube_vectors[573], cube_vectors[574], cube_vectors[575] },
		{ cube_vectors[576], cube_vectors[577], cube_vectors[578] },
		{ cube_vectors[579], cube_vectors[580], cube_vectors[581] },
		{ cube_vectors[582], cube_vectors[583], cube_vectors[584] },
		{ cube_vectors[585], cube_vectors[586], cube_vectors[587] },
		{ cube_vectors[588], cube_vectors[589], cube_vectors[590] },
		{ cube_vectors[591], cube_vectors[592], cube_vectors[593] },
		{ cube_vectors[594], cube_vectors[595], cube_vectors[596] },
		{ cube_vectors[597], cube_vectors[598], cube_vectors[599] },
		{ cube_vectors[600], cube_vectors[601], cube_vectors[602] },
		{ cube_vectors[603], cube_vectors[604], cube_vectors[605] },
		{ cube_vectors[606], cube_vectors[607], cube_vectors[608] },
		{ cube_vectors[609], cube_vectors[610], cube_vectors[611] },
		{ cube_vectors[612], cube_vectors[613], cube_vectors[614] },
		{ cube_vectors[615], cube_vectors[616], cube_vectors[617] },
		{ cube_vectors[618], cube_vectors[619], cube_vectors[620] },
		{ cube_vectors[621], cube_vectors[622], cube_vectors[623] },
		{ cube_vectors[624], cube_vectors[625], cube_vectors[626] },
		{ cube_vectors[627], cube_vectors[628], cube_vectors[629] },
		{ cube_vectors[630], cube_vectors[631], cube_vectors[632] },
		{ cube_vectors[633], cube_vectors[634], cube_vectors[635] },
		{ cube_vectors[636], cube_vectors[637], cube_vectors[638] },
		{ cube_vectors[639], cube_vectors[640], cube_vectors[641] },
		{ cube_vectors[642], cube_vectors[643], cube_vectors[644] },
		{ cube_vectors[645], cube_vectors[646], cube_vectors[647] },
		{ cube_vectors[648], cube_vectors[649], cube_vectors[650] },
		{ cube_vectors[651], cube_vectors[652], cube_vectors[653] },
		{ cube_vectors[654], cube_vectors[655], cube_vectors[656] },
		{ cube_vectors[657], cube_vectors[658], cube_vectors[659] },
		{ cube_vectors[660], cube_vectors[661], cube_vectors[662] },
		{ cube_vectors[663], cube_vectors[664], cube_vectors[665] },
		{ cube_vectors[666], cube_vectors[667], cube_vectors[668] },
		{ cube_vectors[669], cube_vectors[670], cube_vectors[671] },
		{ cube_vectors[672], cube_vectors[673], cube_vectors[674] },
		{ cube_vectors[675], cube_vectors[676], cube_vectors[677] },
		{ cube_vectors[678], cube_vectors[679], cube_vectors[680] },
		{ cube_vectors[681], cube_vectors[682], cube_vectors[683] },
		{ cube_vectors[684], cube_vectors[685], cube_vectors[686] },
		{ cube_vectors[687], cube_vectors[688], cube_vectors[689] },
		{ cube_vectors[690], cube_vectors[691], cube_vectors[692] },
		{ cube_vectors[693], cube_vectors[694], cube_vectors[695] },
		{ cube_vectors[696], cube_vectors[697], cube_vectors[698] },
		{ cube_vectors[699], cube_vectors[700], cube_vectors[701] },
		{ cube_vectors[702], cube_vectors[703], cube_vectors[704] },
		{ cube_vectors[705], cube_vectors[706], cube_vectors[707] },
		{ cube_vectors[708], cube_vectors[709], cube_vectors[710] },
		{ cube_vectors[711], cube_vectors[712], cube_vectors[713] },
		{ cube_vectors[714], cube_vectors[715], cube_vectors[716] },
		{ cube_vectors[717], cube_vectors[718], cube_vectors[719] },
		{ cube_vectors[720], cube_vectors[721], cube_vectors[722] },
		{ cube_vectors[723], cube_vectors[724], cube_vectors[725] },
		{ cube_vectors[726], cube_vectors[727], cube_vectors[728] },
		{ cube_vectors[729], cube_vectors[730], cube_vectors[731] },
		{ cube_vectors[732], cube_vectors[733], cube_vectors[734] },
		{ cube_vectors[735], cube_vectors[736], cube_vectors[737] },
		{ cube_vectors[738], cube_vectors[739], cube_vectors[740] },
		{ cube_vectors[741], cube_vectors[742], cube_vectors[743] },
		{ cube_vectors[744], cube_vectors[745], cube_vectors[746] },
		{ cube_vectors[747], cube_vectors[748], cube_vectors[749] },
		{ cube_vectors[750], cube_vectors[751], cube_vectors[752] },
		{ cube_vectors[753], cube_vectors[754], cube_vectors[755] },
		{ cube_vectors[756], cube_vectors[757], cube_vectors[758] },
		{ cube_vectors[759], cube_vectors[760], cube_vectors[761] },
		{ cube_vectors[762], cube_vectors[763], cube_vectors[764] },
		{ cube_vectors[765], cube_vectors[766], cube_vectors[767] },
		{ cube_vectors[768], cube_vectors[769], cube_vectors[770] },
		{ cube_vectors[771], cube_vectors[772], cube_vectors[773] },
		{ cube_vectors[774], cube_vectors[775], cube_vectors[776] },
		{ cube_vectors[777], cube_vectors[778], cube_vectors[779] },
		{ cube_vectors[780], cube_vectors[781], cube_vectors[782] },
		{ cube_vectors[783], cube_vectors[784], cube_vectors[785] },
		{ cube_vectors[786], cube_vectors[787], cube_vectors[788] },
		{ cube_vectors[789], cube_vectors[790], cube_vectors[791] },
		{ cube_vectors[792], cube_vectors[793], cube_vectors[794] },
		{ cube_vectors[795], cube_vectors[796], cube_vectors[797] },
		{ cube_vectors[798], cube_vectors[799], cube_vectors[800] },
		{ cube_vectors[801], cube_vectors[802], cube_vectors[803] },
		{ cube_vectors[804], cube_vectors[805], cube_vectors[806] },
		{ cube_vectors[807], cube_vectors[808], cube_vectors[809] },
		{ cube_vectors[810], cube_vectors[811], cube_vectors[812] },
		{ cube_vectors[813], cube_vectors[814], cube_vectors[815] },
		{ cube_vectors[816], cube_vectors[817], cube_vectors[818] },
		{ cube_vectors[819], cube_vectors[820], cube_vectors[821] },
		{ cube_vectors[822], cube_vectors[823], cube_vectors[824] },
		{ cube_vectors[825], cube_vectors[826], cube_vectors[827] },
		{ cube_vectors[828], cube_vectors[829], cube_vectors[830] },
		{ cube_vectors[831], cube_vectors[832], cube_vectors[833] },
		{ cube_vectors[834], cube_vectors[835], cube_vectors[836] },
		{ cube_vectors[837], cube_vectors[838], cube_vectors[839] },
		{ cube_vectors[840], cube_vectors[841], cube_vectors[842] },
		{ cube_vectors[843], cube_vectors[844], cube_vectors[845] },
		{ cube_vectors[846], cube_vectors[847], cube_vectors[848] },
		{ cube_vectors[849], cube_vectors[850], cube_vectors[851] },
		{ cube_vectors[852], cube_vectors[853], cube_vectors[854] },
		{ cube_vectors[855], cube_vectors[856], cube_vectors[857] },
		{ cube_vectors[858], cube_vectors[859], cube_vectors[860] },
		{ cube_vectors[861], cube_vectors[862], cube_vectors[863] },
		{ cube_vectors[864], cube_vectors[865], cube_vectors[866] },
		{ cube_vectors[867], cube_vectors[868], cube_vectors[869] },
		{ cube_vectors[870], cube_vectors[871], cube_vectors[872] },
		{ cube_vectors[873], cube_vectors[874], cube_vectors[875] },
		{ cube_vectors[876], cube_vectors[877], cube_vectors[878] },
		{ cube_vectors[879], cube_vectors[880], cube_vectors[881] },
		{ cube_vectors[882], cube_vectors[883], cube_vectors[884] },
		{ cube_vectors[885], cube_vectors[886], cube_vectors[887] },
		{ cube_vectors[888], cube_vectors[889], cube_vectors[890] },
		{ cube_vectors[891], cube_vectors[892], cube_vectors[893] },
		{ cube_vectors[894], cube_vectors[895], cube_vectors[896] },
		{ cube_vectors[897], cube_vectors[898], cube_vectors[899] },
		{ cube_vectors[900], cube_vectors[901], cube_vectors[902] },
		{ cube_vectors[903], cube_vectors[904], cube_vectors[905] },
		{ cube_vectors[906], cube_vectors[907], cube_vectors[908] },
		{ cube_vectors[909], cube_vectors[910], cube_vectors[911] },
		{ cube_vectors[912], cube_vectors[913], cube_vectors[914] },
		{ cube_vectors[915], cube_vectors[916], cube_vectors[917] },
		{ cube_vectors[918], cube_vectors[919], cube_vectors[920] },
		{ cube_vectors[921], cube_vectors[922], cube_vectors[923] },
		{ cube_vectors[924], cube_vectors[925], cube_vectors[926] },
		{ cube_vectors[927], cube_vectors[928], cube_vectors[929] },
		{ cube_vectors[930], cube_vectors[931], cube_vectors[932] },
		{ cube_vectors[933], cube_vectors[934], cube_vectors[935] },
		{ cube_vectors[936], cube_vectors[937], cube_vectors[938] },
		{ cube_vectors[939], cube_vectors[940], cube_vectors[941] },
		{ cube_vectors[942], cube_vectors[943], cube_vectors[944] },
		{ cube_vectors[945], cube_vectors[946], cube_vectors[947] },
		{ cube_vectors[948], cube_vectors[949], cube_vectors[950] },
		{ cube_vectors[951], cube_vectors[952], cube_vectors[953] },
		{ cube_vectors[954], cube_vectors[955], cube_vectors[956] },
		{ cube_vectors[957], cube_vectors[958], cube_vectors[959] },
		{ cube_vectors[960], cube_vectors[961], cube_vectors[962] },
		{ cube_vectors[963], cube_vectors[964], cube_vectors[965] },
		{ cube_vectors[966], cube_vectors[967], cube_vectors[968] },
		{ cube_vectors[969], cube_vectors[970], cube_vectors[971] },
		{ cube_vectors[972], cube_vectors[973], cube_vectors[974] },
		{ cube_vectors[975], cube_vectors[976], cube_vectors[977] },
		{ cube_vectors[978], cube_vectors[979], cube_vectors[980] },
		{ cube_vectors[981], cube_vectors[982], cube_vectors[983] },
		{ cube_vectors[984], cube_vectors[985], cube_vectors[986] },
		{ cube_vectors[987], cube_vectors[988], cube_vectors[989] },
		{ cube_vectors[990], cube_vectors[991], cube_vectors[992] },
		{ cube_vectors[993], cube_vectors[994], cube_vectors[995] },
		{ cube_vectors[996], cube_vectors[997], cube_vectors[998] },
		{ cube_vectors[999], cube_vectors[1000], cube_vectors[1001] },
		{ cube_vectors[1002], cube_vectors[1003], cube_vectors[1004] },
		{ cube_vectors[1005], cube_vectors[1006], cube_vectors[1007] },
		{ cube_vectors[1008], cube_vectors[1009], cube_vectors[1010] },
		{ cube_vectors[1011], cube_vectors[1012], cube_vectors[1013] },
		{ cube_vectors[1014], cube_vectors[1015], cube_vectors[1016] },
		{ cube_vectors[1017], cube_vectors[1018], cube_vectors[1019] },
		{ cube_vectors[1020], cube_vectors[1021], cube_vectors[1022] },
		{ cube_vectors[1023], cube_vectors[1024], cube_vectors[1025] },
		{ cube_vectors[1026], cube_vectors[1027], cube_vectors[1028] },
		{ cube_vectors[1029], cube_vectors[1030], cube_vectors[1031] },
		{ cube_vectors[1032], cube_vectors[1033], cube_vectors[1034] },
		{ cube_vectors[1035], cube_vectors[1036], cube_vectors[1037] },
		{ cube_vectors[1038], cube_vectors[1039], cube_vectors[1040] },
		{ cube_vectors[1041], cube_vectors[1042], cube_vectors[1043] },
		{ cube_vectors[1044], cube_vectors[1045], cube_vectors[1046] },
		{ cube_vectors[1047], cube_vectors[1048], cube_vectors[1049] },
		{ cube_vectors[1050], cube_vectors[1051], cube_vectors[1052] },
		{ cube_vectors[1053], cube_vectors[1054], cube_vectors[1055] },
		{ cube_vectors[1056], cube_vectors[1057], cube_vectors[1058] },
		{ cube_vectors[1059], cube_vectors[1060], cube_vectors[1061] },
		{ cube_vectors[1062], cube_vectors[1063], cube_vectors[1064] },
		{ cube_vectors[1065], cube_vectors[1066], cube_vectors[1067] },
		{ cube_vectors[1068], cube_vectors[1069], cube_vectors[1070] },
		{ cube_vectors[1071], cube_vectors[1072], cube_vectors[1073] },
		{ cube_vectors[1074], cube_vectors[1075], cube_vectors[1076] },
		{ cube_vectors[1077], cube_vectors[1078], cube_vectors[1079] },
		{ cube_vectors[1080], cube_vectors[1081], cube_vectors[1082] },
		{ cube_vectors[1083], cube_vectors[1084], cube_vectors[1085] },
		{ cube_vectors[1086], cube_vectors[1087], cube_vectors[1088] },
		{ cube_vectors[1089], cube_vectors[1090], cube_vectors[1091] },
		{ cube_vectors[1092], cube_vectors[1093], cube_vectors[1094] },
		{ cube_vectors[1095], cube_vectors[1096], cube_vectors[1097] },
		{ cube_vectors[1098], cube_vectors[1099], cube_vectors[1100] },
		{ cube_vectors[1101], cube_vectors[1102], cube_vectors[1103] },
		{ cube_vectors[1104], cube_vectors[1105], cube_vectors[1106] },
		{ cube_vectors[1107], cube_vectors[1108], cube_vectors[1109] },
		{ cube_vectors[1110], cube_vectors[1111], cube_vectors[1112] },
		{ cube_vectors[1113], cube_vectors[1114], cube_vectors[1115] },
		{ cube_vectors[1116], cube_vectors[1117], cube_vectors[1118] },
		{ cube_vectors[1119], cube_vectors[1120], cube_vectors[1121] },
		{ cube_vectors[1122], cube_vectors[1123], cube_vectors[1124] },
		{ cube_vectors[1125], cube_vectors[1126], cube_vectors[1127] },
		{ cube_vectors[1128], cube_vectors[1129], cube_vectors[1130] },
		{ cube_vectors[1131], cube_vectors[1132], cube_vectors[1133] },
		{ cube_vectors[1134], cube_vectors[1135], cube_vectors[1136] },
		{ cube_vectors[1137], cube_vectors[1138], cube_vectors[1139] },
		{ cube_vectors[1140], cube_vectors[1141], cube_vectors[1142] },
		{ cube_vectors[1143], cube_vectors[1144], cube_vectors[1145] },
		{ cube_vectors[1146], cube_vectors[1147], cube_vectors[1148] },
		{ cube_vectors[1149], cube_vectors[1150], cube_vectors[1151] },
		{ cube_vectors[1152], cube_vectors[1153], cube_vectors[1154] },
		{ cube_vectors[1155], cube_vectors[1156], cube_vectors[1157] },
		{ cube_vectors[1158], cube_vectors[1159], cube_vectors[1160] },
		{ cube_vectors[1161], cube_vectors[1162], cube_vectors[1163] },
		{ cube_vectors[1164], cube_vectors[1165], cube_vectors[1166] },
		{ cube_vectors[1167], cube_vectors[1168], cube_vectors[1169] },
		{ cube_vectors[1170], cube_vectors[1171], cube_vectors[1172] },
		{ cube_vectors[1173], cube_vectors[1174], cube_vectors[1175] },
		{ cube_vectors[1176], cube_vectors[1177], cube_vectors[1178] },
		{ cube_vectors[1179], cube_vectors[1180], cube_vectors[1181] },
		{ cube_vectors[1182], cube_vectors[1183], cube_vectors[1184] },
		{ cube_vectors[1185], cube_vectors[1186], cube_vectors[1187] },
		{ cube_vectors[1188], cube_vectors[1189], cube_vectors[1190] },
		{ cube_vectors[1191], cube_vectors[1192], cube_vectors[1193] },
		{ cube_vectors[1194], cube_vectors[1195], cube_vectors[1196] },
		{ cube_vectors[1197], cube_vectors[1198], cube_vectors[1199] },
		{ cube_vectors[1200], cube_vectors[1201], cube_vectors[1202] },
		{ cube_vectors[1203], cube_vectors[1204], cube_vectors[1205] },
		{ cube_vectors[1206], cube_vectors[1207], cube_vectors[1208] },
		{ cube_vectors[1209], cube_vectors[1210], cube_vectors[1211] },
		{ cube_vectors[1212], cube_vectors[1213], cube_vectors[1214] },
		{ cube_vectors[1215], cube_vectors[1216], cube_vectors[1217] },
		{ cube_vectors[1218], cube_vectors[1219], cube_vectors[1220] },
		{ cube_vectors[1221], cube_vectors[1222], cube_vectors[1223] },
		{ cube_vectors[1224], cube_vectors[1225], cube_vectors[1226] },
		{ cube_vectors[1227], cube_vectors[1228], cube_vectors[1229] },
		{ cube_vectors[1230], cube_vectors[1231], cube_vectors[1232] },
		{ cube_vectors[1233], cube_vectors[1234], cube_vectors[1235] },
		{ cube_vectors[1236], cube_vectors[1237], cube_vectors[1238] },
		{ cube_vectors[1239], cube_vectors[1240], cube_vectors[1241] },
		{ cube_vectors[1242], cube_vectors[1243], cube_vectors[1244] },
		{ cube_vectors[1245], cube_vectors[1246], cube_vectors[1247] },
		{ cube_vectors[1248], cube_vectors[1249], cube_vectors[1250] },
		{ cube_vectors[1251], cube_vectors[1252], cube_vectors[1253] },
		{ cube_vectors[1254], cube_vectors[1255], cube_vectors[1256] },
		{ cube_vectors[1257], cube_vectors[1258], cube_vectors[1259] },
		{ cube_vectors[1260], cube_vectors[1261], cube_vectors[1262] },
		{ cube_vectors[1263], cube_vectors[1264], cube_vectors[1265] },
		{ cube_vectors[1266], cube_vectors[1267], cube_vectors[1268] },
		{ cube_vectors[1269], cube_vectors[1270], cube_vectors[1271] },
		{ cube_vectors[1272], cube_vectors[1273], cube_vectors[1274] },
		{ cube_vectors[1275], cube_vectors[1276], cube_vectors[1277] },
		{ cube_vectors[1278], cube_vectors[1279], cube_vectors[1280] },
		{ cube_vectors[1281], cube_vectors[1282], cube_vectors[1283] },
		{ cube_vectors[1284], cube_vectors[1285], cube_vectors[1286] },
		{ cube_vectors[1287], cube_vectors[1288], cube_vectors[1289] },
		{ cube_vectors[1290], cube_vectors[1291], cube_vectors[1292] },
		{ cube_vectors[1293], cube_vectors[1294], cube_vectors[1295] },
		{ cube_vectors[1296], cube_vectors[1297], cube_vectors[1298] },
		{ cube_vectors[1299], cube_vectors[1300], cube_vectors[1301] },
		{ cube_vectors[1302], cube_vectors[1303], cube_vectors[1304] },
		{ cube_vectors[1305], cube_vectors[1306], cube_vectors[1307] },
		{ cube_vectors[1308], cube_vectors[1309], cube_vectors[1310] },
		{ cube_vectors[1311], cube_vectors[1312], cube_vectors[1313] },
		{ cube_vectors[1314], cube_vectors[1315], cube_vectors[1316] },
		{ cube_vectors[1317], cube_vectors[1318], cube_vectors[1319] },
		{ cube_vectors[1320], cube_vectors[1321], cube_vectors[1322] },
		{ cube_vectors[1323], cube_vectors[1324], cube_vectors[1325] },
		{ cube_vectors[1326], cube_vectors[1327], cube_vectors[1328] },
		{ cube_vectors[1329], cube_vectors[1330], cube_vectors[1331] },
		{ cube_vectors[1332], cube_vectors[1333], cube_vectors[1334] },
		{ cube_vectors[1335], cube_vectors[1336], cube_vectors[1337] },
		{ cube_vectors[1338], cube_vectors[1339], cube_vectors[1340] },
		{ cube_vectors[1341], cube_vectors[1342], cube_vectors[1343] },
		{ cube_vectors[1344], cube_vectors[1345], cube_vectors[1346] },
		{ cube_vectors[1347], cube_vectors[1348], cube_vectors[1349] },
		{ cube_vectors[1350], cube_vectors[1351], cube_vectors[1352] },
		{ cube_vectors[1353], cube_vectors[1354], cube_vectors[1355] },
		{ cube_vectors[1356], cube_vectors[1357], cube_vectors[1358] },
		{ cube_vectors[1359], cube_vectors[1360], cube_vectors[1361] },
		{ cube_vectors[1362], cube_vectors[1363], cube_vectors[1364] },
		{ cube_vectors[1365], cube_vectors[1366], cube_vectors[1367] },
		{ cube_vectors[1368], cube_vectors[1369], cube_vectors[1370] },
		{ cube_vectors[1371], cube_vectors[1372], cube_vectors[1373] },
		{ cube_vectors[1374], cube_vectors[1375], cube_vectors[1376] },
		{ cube_vectors[1377], cube_vectors[1378], cube_vectors[1379] },
		{ cube_vectors[1380], cube_vectors[1381], cube_vectors[1382] },
		{ cube_vectors[1383], cube_vectors[1384], cube_vectors[1385] },
		{ cube_vectors[1386], cube_vectors[1387], cube_vectors[1388] },
		{ cube_vectors[1389], cube_vectors[1390], cube_vectors[1391] },
		{ cube_vectors[1392], cube_vectors[1393], cube_vectors[1394] },
		{ cube_vectors[1395], cube_vectors[1396], cube_vectors[1397] },
		{ cube_vectors[1398], cube_vectors[1399], cube_vectors[1400] },
		{ cube_vectors[1401], cube_vectors[1402], cube_vectors[1403] },
		{ cube_vectors[1404], cube_vectors[1405], cube_vectors[1406] },
		{ cube_vectors[1407], cube_vectors[1408], cube_vectors[1409] },
		{ cube_vectors[1410], cube_vectors[1411], cube_vectors[1412] },
		{ cube_vectors[1413], cube_vectors[1414], cube_vectors[1415] },
		{ cube_vectors[1416], cube_vectors[1417], cube_vectors[1418] },
		{ cube_vectors[1419], cube_vectors[1420], cube_vectors[1421] },
		{ cube_vectors[1422], cube_vectors[1423], cube_vectors[1424] },
		{ cube_vectors[1425], cube_vectors[1426], cube_vectors[1427] },
		{ cube_vectors[1428], cube_vectors[1429], cube_vectors[1430] },
		{ cube_vectors[1431], cube_vectors[1432], cube_vectors[1433] },
		{ cube_vectors[1434], cube_vectors[1435], cube_vectors[1436] },
		{ cube_vectors[1437], cube_vectors[1438], cube_vectors[1439] },
		{ cube_vectors[1440], cube_vectors[1441], cube_vectors[1442] },
		{ cube_vectors[1443], cube_vectors[1444], cube_vectors[1445] },
		{ cube_vectors[1446], cube_vectors[1447], cube_vectors[1448] },
		{ cube_vectors[1449], cube_vectors[1450], cube_vectors[1451] },
		{ cube_vectors[1452], cube_vectors[1453], cube_vectors[1454] },
		{ cube_vectors[1455], cube_vectors[1456], cube_vectors[1457] },
		{ cube_vectors[1458], cube_vectors[1459], cube_vectors[1460] },
		{ cube_vectors[1461], cube_vectors[1462], cube_vectors[1463] },
		{ cube_vectors[1464], cube_vectors[1465], cube_vectors[1466] },
		{ cube_vectors[1467], cube_vectors[1468], cube_vectors[1469] },
		{ cube_vectors[1470], cube_vectors[1471], cube_vectors[1472] },
		{ cube_vectors[1473], cube_vectors[1474], cube_vectors[1475] },
		{ cube_vectors[1476], cube_vectors[1477], cube_vectors[1478] },
		{ cube_vectors[1479], cube_vectors[1480], cube_vectors[1481] },
		{ cube_vectors[1482], cube_vectors[1483], cube_vectors[1484] },
		{ cube_vectors[1485], cube_vectors[1486], cube_vectors[1487] },
		{ cube_vectors[1488], cube_vectors[1489], cube_vectors[1490] },
		{ cube_vectors[1491], cube_vectors[1492], cube_vectors[1493] },
		{ cube_vectors[1494], cube_vectors[1495], cube_vectors[1496] },
		{ cube_vectors[1497], cube_vectors[1498], cube_vectors[1499] },
		{ cube_vectors[1500], cube_vectors[1501], cube_vectors[1502] },
		{ cube_vectors[1503], cube_vectors[1504], cube_vectors[1505] },
		{ cube_vectors[1506], cube_vectors[1507], cube_vectors[1508] },
		{ cube_vectors[1509], cube_vectors[1510], cube_vectors[1511] },
		{ cube_vectors[1512], cube_vectors[1513], cube_vectors[1514] },
		{ cube_vectors[1515], cube_vectors[1516], cube_vectors[1517] },
		{ cube_vectors[1518], cube_vectors[1519], cube_vectors[1520] },
		{ cube_vectors[1521], cube_vectors[1522], cube_vectors[1523] },
		{ cube_vectors[1524], cube_vectors[1525], cube_vectors[1526] },
		{ cube_vectors[1527], cube_vectors[1528], cube_vectors[1529] },
		{ cube_vectors[1530], cube_vectors[1531], cube_vectors[1532] },
		{ cube_vectors[1533], cube_vectors[1534], cube_vectors[1535] },
		{ cube_vectors[1536], cube_vectors[1537], cube_vectors[1538] },
		{ cube_vectors[1539], cube_vectors[1540], cube_vectors[1541] },
		{ cube_vectors[1542], cube_vectors[1543], cube_vectors[1544] },
		{ cube_vectors[1545], cube_vectors[1546], cube_vectors[1547] },
		{ cube_vectors[1548], cube_vectors[1549], cube_vectors[1550] },
		{ cube_vectors[1551], cube_vectors[1552], cube_vectors[1553] },
		{ cube_vectors[1554], cube_vectors[1555], cube_vectors[1556] },
		{ cube_vectors[1557], cube_vectors[1558], cube_vectors[1559] },
		{ cube_vectors[1560], cube_vectors[1561], cube_vectors[1562] },
		{ cube_vectors[1563], cube_vectors[1564], cube_vectors[1565] },
		{ cube_vectors[1566], cube_vectors[1567], cube_vectors[1568] },
		{ cube_vectors[1569], cube_vectors[1570], cube_vectors[1571] },
		{ cube_vectors[1572], cube_vectors[1573], cube_vectors[1574] },
		{ cube_vectors[1575], cube_vectors[1576], cube_vectors[1577] },
		{ cube_vectors[1578], cube_vectors[1579], cube_vectors[1580] },
		{ cube_vectors[1581], cube_vectors[1582], cube_vectors[1583] },
		{ cube_vectors[1584], cube_vectors[1585], cube_vectors[1586] },
		{ cube_vectors[1587], cube_vectors[1588], cube_vectors[1589] },
		{ cube_vectors[1590], cube_vectors[1591], cube_vectors[1592] },
		{ cube_vectors[1593], cube_vectors[1594], cube_vectors[1595] },
		{ cube_vectors[1596], cube_vectors[1597], cube_vectors[1598] },
		{ cube_vectors[1599], cube_vectors[1600], cube_vectors[1601] },
		{ cube_vectors[1602], cube_vectors[1603], cube_vectors[1604] },
		{ cube_vectors[1605], cube_vectors[1606], cube_vectors[1607] },
		{ cube_vectors[1608], cube_vectors[1609], cube_vectors[1610] },
		{ cube_vectors[1611], cube_vectors[1612], cube_vectors[1613] },
		{ cube_vectors[1614], cube_vectors[1615], cube_vectors[1616] },
		{ cube_vectors[1617], cube_vectors[1618], cube_vectors[1619] },
		{ cube_vectors[1620], cube_vectors[1621], cube_vectors[1622] },
		{ cube_vectors[1623], cube_vectors[1624], cube_vectors[1625] },
		{ cube_vectors[1626], cube_vectors[1627], cube_vectors[1628] },
		{ cube_vectors[1629], cube_vectors[1630], cube_vectors[1631] },
		{ cube_vectors[1632], cube_vectors[1633], cube_vectors[1634] },
		{ cube_vectors[1635], cube_vectors[1636], cube_vectors[1637] },
		{ cube_vectors[1638], cube_vectors[1639], cube_vectors[1640] },
		{ cube_vectors[1641], cube_vectors[1642], cube_vectors[1643] },
		{ cube_vectors[1644], cube_vectors[1645], cube_vectors[1646] },
		{ cube_vectors[1647], cube_vectors[1648], cube_vectors[1649] },
		{ cube_vectors[1650], cube_vectors[1651], cube_vectors[1652] },
		{ cube_vectors[1653], cube_vectors[1654], cube_vectors[1655] },
		{ cube_vectors[1656], cube_vectors[1657], cube_vectors[1658] },
		{ cube_vectors[1659], cube_vectors[1660], cube_vectors[1661] },
		{ cube_vectors[1662], cube_vectors[1663], cube_vectors[1664] },
		{ cube_vectors[1665], cube_vectors[1666], cube_vectors[1667] },
		{ cube_vectors[1668], cube_vectors[1669], cube_vectors[1670] },
		{ cube_vectors[1671], cube_vectors[1672], cube_vectors[1673] },
		{ cube_vectors[1674], cube_vectors[1675], cube_vectors[1676] },
		{ cube_vectors[1677], cube_vectors[1678], cube_vectors[1679] },
		{ cube_vectors[1680], cube_vectors[1681], cube_vectors[1682] },
		{ cube_vectors[1683], cube_vectors[1684], cube_vectors[1685] },
		{ cube_vectors[1686], cube_vectors[1687], cube_vectors[1688] },
		{ cube_vectors[1689], cube_vectors[1690], cube_vectors[1691] },
		{ cube_vectors[1692], cube_vectors[1693], cube_vectors[1694] },
		{ cube_vectors[1695], cube_vectors[1696], cube_vectors[1697] },
		{ cube_vectors[1698], cube_vectors[1699], cube_vectors[1700] },
		{ cube_vectors[1701], cube_vectors[1702], cube_vectors[1703] },
		{ cube_vectors[1704], cube_vectors[1705], cube_vectors[1706] },
		{ cube_vectors[1707], cube_vectors[1708], cube_vectors[1709] },
		{ cube_vectors[1710], cube_vectors[1711], cube_vectors[1712] },
		{ cube_vectors[1713], cube_vectors[1714], cube_vectors[1715] },
		{ cube_vectors[1716], cube_vectors[1717], cube_vectors[1718] },
		{ cube_vectors[1719], cube_vectors[1720], cube_vectors[1721] },
		{ cube_vectors[1722], cube_vectors[1723], cube_vectors[1724] },
		{ cube_vectors[1725], cube_vectors[1726], cube_vectors[1727] },
		{ cube_vectors[1728], cube_vectors[1729], cube_vectors[1730] },
		{ cube_vectors[1731], cube_vectors[1732], cube_vectors[1733] },
		{ cube_vectors[1734], cube_vectors[1735], cube_vectors[1736] },
		{ cube_vectors[1737], cube_vectors[1738], cube_vectors[1739] },
		{ cube_vectors[1740], cube_vectors[1741], cube_vectors[1742] },
		{ cube_vectors[1743], cube_vectors[1744], cube_vectors[1745] },
		{ cube_vectors[1746], cube_vectors[1747], cube_vectors[1748] },
		{ cube_vectors[1749], cube_vectors[1750], cube_vectors[1751] },
		{ cube_vectors[1752], cube_vectors[1753], cube_vectors[1754] },
		{ cube_vectors[1755], cube_vectors[1756], cube_vectors[1757] },
		{ cube_vectors[1758], cube_vectors[1759], cube_vectors[1760] },
		{ cube_vectors[1761], cube_vectors[1762], cube_vectors[1763] },
		{ cube_vectors[1764], cube_vectors[1765], cube_vectors[1766] },
		{ cube_vectors[1767], cube_vectors[1768], cube_vectors[1769] },
		{ cube_vectors[1770], cube_vectors[1771], cube_vectors[1772] },
		{ cube_vectors[1773], cube_vectors[1774], cube_vectors[1775] },
		{ cube_vectors[1776], cube_vectors[1777], cube_vectors[1778] },
		{ cube_vectors[1779], cube_vectors[1780], cube_vectors[1781] },
		{ cube_vectors[1782], cube_vectors[1783], cube_vectors[1784] },
		{ cube_vectors[1785], cube_vectors[1786], cube_vectors[1787] },
		{ cube_vectors[1788], cube_vectors[1789], cube_vectors[1790] },
		{ cube_vectors[1791], cube_vectors[1792], cube_vectors[1793] },
		{ cube_vectors[1794], cube_vectors[1795], cube_vectors[1796] },
		{ cube_vectors[1797], cube_vectors[1798], cube_vectors[1799] },
		{ cube_vectors[1800], cube_vectors[1801], cube_vectors[1802] },
		{ cube_vectors[1803], cube_vectors[1804], cube_vectors[1805] },
		{ cube_vectors[1806], cube_vectors[1807], cube_vectors[1808] },
		{ cube_vectors[1809], cube_vectors[1810], cube_vectors[1811] },
		{ cube_vectors[1812], cube_vectors[1813], cube_vectors[1814] },
		{ cube_vectors[1815], cube_vectors[1816], cube_vectors[1817] },
		{ cube_vectors[1818], cube_vectors[1819], cube_vectors[1820] },
		{ cube_vectors[1821], cube_vectors[1822], cube_vectors[1823] },
		{ cube_vectors[1824], cube_vectors[1825], cube_vectors[1826] },
		{ cube_vectors[1827], cube_vectors[1828], cube_vectors[1829] },
		{ cube_vectors[1830], cube_vectors[1831], cube_vectors[1832] },
		{ cube_vectors[1833], cube_vectors[1834], cube_vectors[1835] },
		{ cube_vectors[1836], cube_vectors[1837], cube_vectors[1838] },
		{ cube_vectors[1839], cube_vectors[1840], cube_vectors[1841] },
		{ cube_vectors[1842], cube_vectors[1843], cube_vectors[1844] },
		{ cube_vectors[1845], cube_vectors[1846], cube_vectors[1847] },
		{ cube_vectors[1848], cube_vectors[1849], cube_vectors[1850] },
		{ cube_vectors[1851], cube_vectors[1852], cube_vectors[1853] },
		{ cube_vectors[1854], cube_vectors[1855], cube_vectors[1856] },
		{ cube_vectors[1857], cube_vectors[1858], cube_vectors[1859] },
		{ cube_vectors[1860], cube_vectors[1861], cube_vectors[1862] },
		{ cube_vectors[1863], cube_vectors[1864], cube_vectors[1865] },
		{ cube_vectors[1866], cube_vectors[1867], cube_vectors[1868] },
		{ cube_vectors[1869], cube_vectors[1870], cube_vectors[1871] },
		{ cube_vectors[1872], cube_vectors[1873], cube_vectors[1874] },
		{ cube_vectors[1875], cube_vectors[1876], cube_vectors[1877] },
		{ cube_vectors[1878], cube_vectors[1879], cube_vectors[1880] },
		{ cube_vectors[1881], cube_vectors[1882], cube_vectors[1883] },
		{ cube_vectors[1884], cube_vectors[1885], cube_vectors[1886] },
		{ cube_vectors[1887], cube_vectors[1888], cube_vectors[1889] },
		{ cube_vectors[1890], cube_vectors[1891], cube_vectors[1892] },
		{ cube_vectors[1893], cube_vectors[1894], cube_vectors[1895] },
		{ cube_vectors[1896], cube_vectors[1897], cube_vectors[1898] },
		{ cube_vectors[1899], cube_vectors[1900], cube_vectors[1901] },
		{ cube_vectors[1902], cube_vectors[1903], cube_vectors[1904] },
		{ cube_vectors[1905], cube_vectors[1906], cube_vectors[1907] },
		{ cube_vectors[1908], cube_vectors[1909], cube_vectors[1910] },
		{ cube_vectors[1911], cube_vectors[1912], cube_vectors[1913] },
		{ cube_vectors[1914], cube_vectors[1915], cube_vectors[1916] },
		{ cube_vectors[1917], cube_vectors[1918], cube_vectors[1919] },
		{ cube_vectors[1920], cube_vectors[1921], cube_vectors[1922] },
		{ cube_vectors[1923], cube_vectors[1924], cube_vectors[1925] },
		{ cube_vectors[1926], cube_vectors[1927], cube_vectors[1928] },
		{ cube_vectors[1929], cube_vectors[1930], cube_vectors[1931] },
		{ cube_vectors[1932], cube_vectors[1933], cube_vectors[1934] },
		{ cube_vectors[1935], cube_vectors[1936], cube_vectors[1937] },
		{ cube_vectors[1938], cube_vectors[1939], cube_vectors[1940] },
		{ cube_vectors[1941], cube_vectors[1942], cube_vectors[1943] },
		{ cube_vectors[1944], cube_vectors[1945], cube_vectors[1946] },
		{ cube_vectors[1947], cube_vectors[1948], cube_vectors[1949] },
		{ cube_vectors[1950], cube_vectors[1951], cube_vectors[1952] },
		{ cube_vectors[1953], cube_vectors[1954], cube_vectors[1955] },
		{ cube_vectors[1956], cube_vectors[1957], cube_vectors[1958] },
		{ cube_vectors[1959], cube_vectors[1960], cube_vectors[1961] },
		{ cube_vectors[1962], cube_vectors[1963], cube_vectors[1964] },
		{ cube_vectors[1965], cube_vectors[1966], cube_vectors[1967] },
		{ cube_vectors[1968], cube_vectors[1969], cube_vectors[1970] },
		{ cube_vectors[1971], cube_vectors[1972], cube_vectors[1973] },
		{ cube_vectors[1974], cube_vectors[1975], cube_vectors[1976] },
		{ cube_vectors[1977], cube_vectors[1978], cube_vectors[1979] },
		{ cube_vectors[1980], cube_vectors[1981], cube_vectors[1982] },
		{ cube_vectors[1983], cube_vectors[1984], cube_vectors[1985] },
		{ cube_vectors[1986], cube_vectors[1987], cube_vectors[1988] },
		{ cube_vectors[1989], cube_vectors[1990], cube_vectors[1991] },
		{ cube_vectors[1992], cube_vectors[1993], cube_vectors[1994] },
		{ cube_vectors[1995], cube_vectors[1996], cube_vectors[1997] },
		{ cube_vectors[1998], cube_vectors[1999], cube_vectors[2000] },
		{ cube_vectors[2001], cube_vectors[2002], cube_vectors[2003] },
		{ cube_vectors[2004], cube_vectors[2005], cube_vectors[2006] },
		{ cube_vectors[2007], cube_vectors[2008], cube_vectors[2009] },
		{ cube_vectors[2010], cube_vectors[2011], cube_vectors[2012] },
		{ cube_vectors[2013], cube_vectors[2014], cube_vectors[2015] },
		{ cube_vectors[2016], cube_vectors[2017], cube_vectors[2018] },
		{ cube_vectors[2019], cube_vectors[2020], cube_vectors[2021] },
		{ cube_vectors[2022], cube_vectors[2023], cube_vectors[2024] },
		{ cube_vectors[2025], cube_vectors[2026], cube_vectors[2027] },
		{ cube_vectors[2028], cube_vectors[2029], cube_vectors[2030] },
		{ cube_vectors[2031], cube_vectors[2032], cube_vectors[2033] },
		{ cube_vectors[2034], cube_vectors[2035], cube_vectors[2036] },
		{ cube_vectors[2037], cube_vectors[2038], cube_vectors[2039] },
		{ cube_vectors[2040], cube_vectors[2041], cube_vectors[2042] },
		{ cube_vectors[2043], cube_vectors[2044], cube_vectors[2045] },
		{ cube_vectors[2046], cube_vectors[2047], cube_vectors[2048] },
		{ cube_vectors[2049], cube_vectors[2050], cube_vectors[2051] },
		{ cube_vectors[2052], cube_vectors[2053], cube_vectors[2054] },
		{ cube_vectors[2055], cube_vectors[2056], cube_vectors[2057] },
		{ cube_vectors[2058], cube_vectors[2059], cube_vectors[2060] },
		{ cube_vectors[2061], cube_vectors[2062], cube_vectors[2063] },
		{ cube_vectors[2064], cube_vectors[2065], cube_vectors[2066] },
		{ cube_vectors[2067], cube_vectors[2068], cube_vectors[2069] },
		{ cube_vectors[2070], cube_vectors[2071], cube_vectors[2072] },
		{ cube_vectors[2073], cube_vectors[2074], cube_vectors[2075] },
		{ cube_vectors[2076], cube_vectors[2077], cube_vectors[2078] },
		{ cube_vectors[2079], cube_vectors[2080], cube_vectors[2081] },
		{ cube_vectors[2082], cube_vectors[2083], cube_vectors[2084] },
		{ cube_vectors[2085], cube_vectors[2086], cube_vectors[2087] },
		{ cube_vectors[2088], cube_vectors[2089], cube_vectors[2090] },
		{ cube_vectors[2091], cube_vectors[2092], cube_vectors[2093] },
		{ cube_vectors[2094], cube_vectors[2095], cube_vectors[2096] },
		{ cube_vectors[2097], cube_vectors[2098], cube_vectors[2099] },
		{ cube_vectors[2100], cube_vectors[2101], cube_vectors[2102] },
		{ cube_vectors[2103], cube_vectors[2104], cube_vectors[2105] },
		{ cube_vectors[2106], cube_vectors[2107], cube_vectors[2108] },
		{ cube_vectors[2109], cube_vectors[2110], cube_vectors[2111] },
		{ cube_vectors[2112], cube_vectors[2113], cube_vectors[2114] },
		{ cube_vectors[2115], cube_vectors[2116], cube_vectors[2117] },
		{ cube_vectors[2118], cube_vectors[2119], cube_vectors[2120] },
		{ cube_vectors[2121], cube_vectors[2122], cube_vectors[2123] },
		{ cube_vectors[2124], cube_vectors[2125], cube_vectors[2126] },
		{ cube_vectors[2127], cube_vectors[2128], cube_vectors[2129] },
		{ cube_vectors[2130], cube_vectors[2131], cube_vectors[2132] },
		{ cube_vectors[2133], cube_vectors[2134], cube_vectors[2135] },
		{ cube_vectors[2136], cube_vectors[2137], cube_vectors[2138] },
		{ cube_vectors[2139], cube_vectors[2140], cube_vectors[2141] },
		{ cube_vectors[2142], cube_vectors[2143], cube_vectors[2144] },
		{ cube_vectors[2145], cube_vectors[2146], cube_vectors[2147] },
		{ cube_vectors[2148], cube_vectors[2149], cube_vectors[2150] },
		{ cube_vectors[2151], cube_vectors[2152], cube_vectors[2153] },
		{ cube_vectors[2154], cube_vectors[2155], cube_vectors[2156] },
		{ cube_vectors[2157], cube_vectors[2158], cube_vectors[2159] },
		{ cube_vectors[2160], cube_vectors[2161], cube_vectors[2162] },
		{ cube_vectors[2163], cube_vectors[2164], cube_vectors[2165] },
		{ cube_vectors[2166], cube_vectors[2167], cube_vectors[2168] },
		{ cube_vectors[2169], cube_vectors[2170], cube_vectors[2171] },
		{ cube_vectors[2172], cube_vectors[2173], cube_vectors[2174] },
		{ cube_vectors[2175], cube_vectors[2176], cube_vectors[2177] },
		{ cube_vectors[2178], cube_vectors[2179], cube_vectors[2180] },
		{ cube_vectors[2181], cube_vectors[2182], cube_vectors[2183] },
		{ cube_vectors[2184], cube_vectors[2185], cube_vectors[2186] },
		{ cube_vectors[2187], cube_vectors[2188], cube_vectors[2189] },
		{ cube_vectors[2190], cube_vectors[2191], cube_vectors[2192] },
		{ cube_vectors[2193], cube_vectors[2194], cube_vectors[2195] },
		{ cube_vectors[2196], cube_vectors[2197], cube_vectors[2198] },
		{ cube_vectors[2199], cube_vectors[2200], cube_vectors[2201] },
		{ cube_vectors[2202], cube_vectors[2203], cube_vectors[2204] },
		{ cube_vectors[2205], cube_vectors[2206], cube_vectors[2207] },
		{ cube_vectors[2208], cube_vectors[2209], cube_vectors[2210] },
		{ cube_vectors[2211], cube_vectors[2212], cube_vectors[2213] },
		{ cube_vectors[2214], cube_vectors[2215], cube_vectors[2216] },
		{ cube_vectors[2217], cube_vectors[2218], cube_vectors[2219] },
		{ cube_vectors[2220], cube_vectors[2221], cube_vectors[2222] },
		{ cube_vectors[2223], cube_vectors[2224], cube_vectors[2225] },
		{ cube_vectors[2226], cube_vectors[2227], cube_vectors[2228] },
		{ cube_vectors[2229], cube_vectors[2230], cube_vectors[2231] },
		{ cube_vectors[2232], cube_vectors[2233], cube_vectors[2234] },
		{ cube_vectors[2235], cube_vectors[2236], cube_vectors[2237] },
		{ cube_vectors[2238], cube_vectors[2239], cube_vectors[2240] },
		{ cube_vectors[2241], cube_vectors[2242], cube_vectors[2243] },
		{ cube_vectors[2244], cube_vectors[2245], cube_vectors[2246] },
		{ cube_vectors[2247], cube_vectors[2248], cube_vectors[2249] },
		{ cube_vectors[2250], cube_vectors[2251], cube_vectors[2252] },
		{ cube_vectors[2253], cube_vectors[2254], cube_vectors[2255] },
		{ cube_vectors[2256], cube_vectors[2257], cube_vectors[2258] },
		{ cube_vectors[2259], cube_vectors[2260], cube_vectors[2261] },
		{ cube_vectors[2262], cube_vectors[2263], cube_vectors[2264] },
		{ cube_vectors[2265], cube_vectors[2266], cube_vectors[2267] },
		{ cube_vectors[2268], cube_vectors[2269], cube_vectors[2270] },
		{ cube_vectors[2271], cube_vectors[2272], cube_vectors[2273] },
		{ cube_vectors[2274], cube_vectors[2275], cube_vectors[2276] },
		{ cube_vectors[2277], cube_vectors[2278], cube_vectors[2279] },
		{ cube_vectors[2280], cube_vectors[2281], cube_vectors[2282] },
		{ cube_vectors[2283], cube_vectors[2284], cube_vectors[2285] },
		{ cube_vectors[2286], cube_vectors[2287], cube_vectors[2288] },
		{ cube_vectors[2289], cube_vectors[2290], cube_vectors[2291] },
		{ cube_vectors[2292], cube_vectors[2293], cube_vectors[2294] },
		{ cube_vectors[2295], cube_vectors[2296], cube_vectors[2297] },
		{ cube_vectors[2298], cube_vectors[2299], cube_vectors[2300] },
		{ cube_vectors[2301], cube_vectors[2302], cube_vectors[2303] },
		{ cube_vectors[2304], cube_vectors[2305], cube_vectors[2306] },
		{ cube_vectors[2307], cube_vectors[2308], cube_vectors[2309] },
		{ cube_vectors[2310], cube_vectors[2311], cube_vectors[2312] },
		{ cube_vectors[2313], cube_vectors[2314], cube_vectors[2315] },
		{ cube_vectors[2316], cube_vectors[2317], cube_vectors[2318] },
		{ cube_vectors[2319], cube_vectors[2320], cube_vectors[2321] },
		{ cube_vectors[2322], cube_vectors[2323], cube_vectors[2324] },
		{ cube_vectors[2325], cube_vectors[2326], cube_vectors[2327] },
		{ cube_vectors[2328], cube_vectors[2329], cube_vectors[2330] },
		{ cube_vectors[2331], cube_vectors[2332], cube_vectors[2333] },
		{ cube_vectors[2334], cube_vectors[2335], cube_vectors[2336] },
		{ cube_vectors[2337], cube_vectors[2338], cube_vectors[2339] },
		{ cube_vectors[2340], cube_vectors[2341], cube_vectors[2342] },
		{ cube_vectors[2343], cube_vectors[2344], cube_vectors[2345] },
		{ cube_vectors[2346], cube_vectors[2347], cube_vectors[2348] },
		{ cube_vectors[2349], cube_vectors[2350], cube_vectors[2351] },
		{ cube_vectors[2352], cube_vectors[2353], cube_vectors[2354] },
		{ cube_vectors[2355], cube_vectors[2356], cube_vectors[2357] },
		{ cube_vectors[2358], cube_vectors[2359], cube_vectors[2360] },
		{ cube_vectors[2361], cube_vectors[2362], cube_vectors[2363] },
		{ cube_vectors[2364], cube_vectors[2365], cube_vectors[2366] },
		{ cube_vectors[2367], cube_vectors[2368], cube_vectors[2369] },
		{ cube_vectors[2370], cube_vectors[2371], cube_vectors[2372] },
		{ cube_vectors[2373], cube_vectors[2374], cube_vectors[2375] },
		{ cube_vectors[2376], cube_vectors[2377], cube_vectors[2378] },
		{ cube_vectors[2379], cube_vectors[2380], cube_vectors[2381] },
		{ cube_vectors[2382], cube_vectors[2383], cube_vectors[2384] },
		{ cube_vectors[2385], cube_vectors[2386], cube_vectors[2387] },
		{ cube_vectors[2388], cube_vectors[2389], cube_vectors[2390] },
		{ cube_vectors[2391], cube_vectors[2392], cube_vectors[2393] },
		{ cube_vectors[2394], cube_vectors[2395], cube_vectors[2396] },
		{ cube_vectors[2397], cube_vectors[2398], cube_vectors[2399] }

	};








	m_vb = GraphEng::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(vertex_list);


	unsigned int index_list[] =
	{
		
		0,1,2,2,3,0,4,5,6,6,7,4,1,6,5,5,2,1,7,0,3,3,4,7,3,2,5,5,4,3,7,6,1,1,0,7,
		8,9,10,10,11,8,12,13,14,14,15,12,9,14,13,13,10,9,15,8,11,11,12,15,11,10,13,13,12,11,15,14,9,9,8,15,
		16,17,18,18,19,16,20,21,22,22,23,20,17,22,21,21,18,17,23,16,19,19,20,23,19,18,21,21,20,19,23,22,17,17,16,23,
		24,25,26,26,27,24,28,29,30,30,31,28,25,30,29,29,26,25,31,24,27,27,28,31,27,26,29,29,28,27,31,30,25,25,24,31,
		32,33,34,34,35,32,36,37,38,38,39,36,33,38,37,37,34,33,39,32,35,35,36,39,35,34,37,37,36,35,39,38,33,33,32,39,
		40,41,42,42,43,40,44,45,46,46,47,44,41,46,45,45,42,41,47,40,43,43,44,47,43,42,45,45,44,43,47,46,41,41,40,47,
		48,49,50,50,51,48,52,53,54,54,55,52,49,54,53,53,50,49,55,48,51,51,52,55,51,50,53,53,52,51,55,54,49,49,48,55,
		56,57,58,58,59,56,60,61,62,62,63,60,57,62,61,61,58,57,63,56,59,59,60,63,59,58,61,61,60,59,63,62,57,57,56,63,
		64,65,66,66,67,64,68,69,70,70,71,68,65,70,69,69,66,65,71,64,67,67,68,71,67,66,69,69,68,67,71,70,65,65,64,71,
		72,73,74,74,75,72,76,77,78,78,79,76,73,78,77,77,74,73,79,72,75,75,76,79,75,74,77,77,76,75,79,78,73,73,72,79,
		80,81,82,82,83,80,84,85,86,86,87,84,81,86,85,85,82,81,87,80,83,83,84,87,83,82,85,85,84,83,87,86,81,81,80,87,
		88,89,90,90,91,88,92,93,94,94,95,92,89,94,93,93,90,89,95,88,91,91,92,95,91,90,93,93,92,91,95,94,89,89,88,95,
		96,97,98,98,99,96,100,101,102,102,103,100,97,102,101,101,98,97,103,96,99,99,100,103,99,98,101,101,100,99,103,102,97,97,96,103,
		104,105,106,106,107,104,108,109,110,110,111,108,105,110,109,109,106,105,111,104,107,107,108,111,107,106,109,109,108,107,111,110,105,105,104,111,
		112,113,114,114,115,112,116,117,118,118,119,116,113,118,117,117,114,113,119,112,115,115,116,119,115,114,117,117,116,115,119,118,113,113,112,119,
		120,121,122,122,123,120,124,125,126,126,127,124,121,126,125,125,122,121,127,120,123,123,124,127,123,122,125,125,124,123,127,126,121,121,120,127,
		128,129,130,130,131,128,132,133,134,134,135,132,129,134,133,133,130,129,135,128,131,131,132,135,131,130,133,133,132,131,135,134,129,129,128,135,
		136,137,138,138,139,136,140,141,142,142,143,140,137,142,141,141,138,137,143,136,139,139,140,143,139,138,141,141,140,139,143,142,137,137,136,143,
		144,145,146,146,147,144,148,149,150,150,151,148,145,150,149,149,146,145,151,144,147,147,148,151,147,146,149,149,148,147,151,150,145,145,144,151,
		152,153,154,154,155,152,156,157,158,158,159,156,153,158,157,157,154,153,159,152,155,155,156,159,155,154,157,157,156,155,159,158,153,153,152,159,
		160,161,162,162,163,160,164,165,166,166,167,164,161,166,165,165,162,161,167,160,163,163,164,167,163,162,165,165,164,163,167,166,161,161,160,167,
		168,169,170,170,171,168,172,173,174,174,175,172,169,174,173,173,170,169,175,168,171,171,172,175,171,170,173,173,172,171,175,174,169,169,168,175,
		176,177,178,178,179,176,180,181,182,182,183,180,177,182,181,181,178,177,183,176,179,179,180,183,179,178,181,181,180,179,183,182,177,177,176,183,
		184,185,186,186,187,184,188,189,190,190,191,188,185,190,189,189,186,185,191,184,187,187,188,191,187,186,189,189,188,187,191,190,185,185,184,191,
		192,193,194,194,195,192,196,197,198,198,199,196,193,198,197,197,194,193,199,192,195,195,196,199,195,194,197,197,196,195,199,198,193,193,192,199,
		200,201,202,202,203,200,204,205,206,206,207,204,201,206,205,205,202,201,207,200,203,203,204,207,203,202,205,205,204,203,207,206,201,201,200,207,
		208,209,210,210,211,208,212,213,214,214,215,212,209,214,213,213,210,209,215,208,211,211,212,215,211,210,213,213,212,211,215,214,209,209,208,215,
		216,217,218,218,219,216,220,221,222,222,223,220,217,222,221,221,218,217,223,216,219,219,220,223,219,218,221,221,220,219,223,222,217,217,216,223,
		224,225,226,226,227,224,228,229,230,230,231,228,225,230,229,229,226,225,231,224,227,227,228,231,227,226,229,229,228,227,231,230,225,225,224,231,
		232,233,234,234,235,232,236,237,238,238,239,236,233,238,237,237,234,233,239,232,235,235,236,239,235,234,237,237,236,235,239,238,233,233,232,239,
		240,241,242,242,243,240,244,245,246,246,247,244,241,246,245,245,242,241,247,240,243,243,244,247,243,242,245,245,244,243,247,246,241,241,240,247,
		248,249,250,250,251,248,252,253,254,254,255,252,249,254,253,253,250,249,255,248,251,251,252,255,251,250,253,253,252,251,255,254,249,249,248,255,
		256,257,258,258,259,256,260,261,262,262,263,260,257,262,261,261,258,257,263,256,259,259,260,263,259,258,261,261,260,259,263,262,257,257,256,263,
		264,265,266,266,267,264,268,269,270,270,271,268,265,270,269,269,266,265,271,264,267,267,268,271,267,266,269,269,268,267,271,270,265,265,264,271,
		272,273,274,274,275,272,276,277,278,278,279,276,273,278,277,277,274,273,279,272,275,275,276,279,275,274,277,277,276,275,279,278,273,273,272,279,
		280,281,282,282,283,280,284,285,286,286,287,284,281,286,285,285,282,281,287,280,283,283,284,287,283,282,285,285,284,283,287,286,281,281,280,287,
		288,289,290,290,291,288,292,293,294,294,295,292,289,294,293,293,290,289,295,288,291,291,292,295,291,290,293,293,292,291,295,294,289,289,288,295,
		296,297,298,298,299,296,300,301,302,302,303,300,297,302,301,301,298,297,303,296,299,299,300,303,299,298,301,301,300,299,303,302,297,297,296,303,
		304,305,306,306,307,304,308,309,310,310,311,308,305,310,309,309,306,305,311,304,307,307,308,311,307,306,309,309,308,307,311,310,305,305,304,311,
		312,313,314,314,315,312,316,317,318,318,319,316,313,318,317,317,314,313,319,312,315,315,316,319,315,314,317,317,316,315,319,318,313,313,312,319,
		320,321,322,322,323,320,324,325,326,326,327,324,321,326,325,325,322,321,327,320,323,323,324,327,323,322,325,325,324,323,327,326,321,321,320,327,
		328,329,330,330,331,328,332,333,334,334,335,332,329,334,333,333,330,329,335,328,331,331,332,335,331,330,333,333,332,331,335,334,329,329,328,335,
		336,337,338,338,339,336,340,341,342,342,343,340,337,342,341,341,338,337,343,336,339,339,340,343,339,338,341,341,340,339,343,342,337,337,336,343,
		344,345,346,346,347,344,348,349,350,350,351,348,345,350,349,349,346,345,351,344,347,347,348,351,347,346,349,349,348,347,351,350,345,345,344,351,
		352,353,354,354,355,352,356,357,358,358,359,356,353,358,357,357,354,353,359,352,355,355,356,359,355,354,357,357,356,355,359,358,353,353,352,359,
		360,361,362,362,363,360,364,365,366,366,367,364,361,366,365,365,362,361,367,360,363,363,364,367,363,362,365,365,364,363,367,366,361,361,360,367,
		368,369,370,370,371,368,372,373,374,374,375,372,369,374,373,373,370,369,375,368,371,371,372,375,371,370,373,373,372,371,375,374,369,369,368,375,
		376,377,378,378,379,376,380,381,382,382,383,380,377,382,381,381,378,377,383,376,379,379,380,383,379,378,381,381,380,379,383,382,377,377,376,383,
		384,385,386,386,387,384,388,389,390,390,391,388,385,390,389,389,386,385,391,384,387,387,388,391,387,386,389,389,388,387,391,390,385,385,384,391,
		392,393,394,394,395,392,396,397,398,398,399,396,393,398,397,397,394,393,399,392,395,395,396,399,395,394,397,397,396,395,399,398,393,393,392,399,
		400,401,402,402,403,400,404,405,406,406,407,404,401,406,405,405,402,401,407,400,403,403,404,407,403,402,405,405,404,403,407,406,401,401,400,407,
		408,409,410,410,411,408,412,413,414,414,415,412,409,414,413,413,410,409,415,408,411,411,412,415,411,410,413,413,412,411,415,414,409,409,408,415,
		416,417,418,418,419,416,420,421,422,422,423,420,417,422,421,421,418,417,423,416,419,419,420,423,419,418,421,421,420,419,423,422,417,417,416,423,
		424,425,426,426,427,424,428,429,430,430,431,428,425,430,429,429,426,425,431,424,427,427,428,431,427,426,429,429,428,427,431,430,425,425,424,431,
		432,433,434,434,435,432,436,437,438,438,439,436,433,438,437,437,434,433,439,432,435,435,436,439,435,434,437,437,436,435,439,438,433,433,432,439,
		440,441,442,442,443,440,444,445,446,446,447,444,441,446,445,445,442,441,447,440,443,443,444,447,443,442,445,445,444,443,447,446,441,441,440,447,
		448,449,450,450,451,448,452,453,454,454,455,452,449,454,453,453,450,449,455,448,451,451,452,455,451,450,453,453,452,451,455,454,449,449,448,455,
		456,457,458,458,459,456,460,461,462,462,463,460,457,462,461,461,458,457,463,456,459,459,460,463,459,458,461,461,460,459,463,462,457,457,456,463,
		464,465,466,466,467,464,468,469,470,470,471,468,465,470,469,469,466,465,471,464,467,467,468,471,467,466,469,469,468,467,471,470,465,465,464,471,
		472,473,474,474,475,472,476,477,478,478,479,476,473,478,477,477,474,473,479,472,475,475,476,479,475,474,477,477,476,475,479,478,473,473,472,479,
		480,481,482,482,483,480,484,485,486,486,487,484,481,486,485,485,482,481,487,480,483,483,484,487,483,482,485,485,484,483,487,486,481,481,480,487,
		488,489,490,490,491,488,492,493,494,494,495,492,489,494,493,493,490,489,495,488,491,491,492,495,491,490,493,493,492,491,495,494,489,489,488,495,
		496,497,498,498,499,496,500,501,502,502,503,500,497,502,501,501,498,497,503,496,499,499,500,503,499,498,501,501,500,499,503,502,497,497,496,503,
		504,505,506,506,507,504,508,509,510,510,511,508,505,510,509,509,506,505,511,504,507,507,508,511,507,506,509,509,508,507,511,510,505,505,504,511,
		512,513,514,514,515,512,516,517,518,518,519,516,513,518,517,517,514,513,519,512,515,515,516,519,515,514,517,517,516,515,519,518,513,513,512,519,
		520,521,522,522,523,520,524,525,526,526,527,524,521,526,525,525,522,521,527,520,523,523,524,527,523,522,525,525,524,523,527,526,521,521,520,527,
		528,529,530,530,531,528,532,533,534,534,535,532,529,534,533,533,530,529,535,528,531,531,532,535,531,530,533,533,532,531,535,534,529,529,528,535,
		536,537,538,538,539,536,540,541,542,542,543,540,537,542,541,541,538,537,543,536,539,539,540,543,539,538,541,541,540,539,543,542,537,537,536,543,
		544,545,546,546,547,544,548,549,550,550,551,548,545,550,549,549,546,545,551,544,547,547,548,551,547,546,549,549,548,547,551,550,545,545,544,551,
		552,553,554,554,555,552,556,557,558,558,559,556,553,558,557,557,554,553,559,552,555,555,556,559,555,554,557,557,556,555,559,558,553,553,552,559,
		560,561,562,562,563,560,564,565,566,566,567,564,561,566,565,565,562,561,567,560,563,563,564,567,563,562,565,565,564,563,567,566,561,561,560,567,
		568,569,570,570,571,568,572,573,574,574,575,572,569,574,573,573,570,569,575,568,571,571,572,575,571,570,573,573,572,571,575,574,569,569,568,575,
		576,577,578,578,579,576,580,581,582,582,583,580,577,582,581,581,578,577,583,576,579,579,580,583,579,578,581,581,580,579,583,582,577,577,576,583,
		584,585,586,586,587,584,588,589,590,590,591,588,585,590,589,589,586,585,591,584,587,587,588,591,587,586,589,589,588,587,591,590,585,585,584,591,
		592,593,594,594,595,592,596,597,598,598,599,596,593,598,597,597,594,593,599,592,595,595,596,599,595,594,597,597,596,595,599,598,593,593,592,599,
		600,601,602,602,603,600,604,605,606,606,607,604,601,606,605,605,602,601,607,600,603,603,604,607,603,602,605,605,604,603,607,606,601,601,600,607,
		608,609,610,610,611,608,612,613,614,614,615,612,609,614,613,613,610,609,615,608,611,611,612,615,611,610,613,613,612,611,615,614,609,609,608,615,
		616,617,618,618,619,616,620,621,622,622,623,620,617,622,621,621,618,617,623,616,619,619,620,623,619,618,621,621,620,619,623,622,617,617,616,623,
		624,625,626,626,627,624,628,629,630,630,631,628,625,630,629,629,626,625,631,624,627,627,628,631,627,626,629,629,628,627,631,630,625,625,624,631,
		632,633,634,634,635,632,636,637,638,638,639,636,633,638,637,637,634,633,639,632,635,635,636,639,635,634,637,637,636,635,639,638,633,633,632,639,
		640,641,642,642,643,640,644,645,646,646,647,644,641,646,645,645,642,641,647,640,643,643,644,647,643,642,645,645,644,643,647,646,641,641,640,647,
		648,649,650,650,651,648,652,653,654,654,655,652,649,654,653,653,650,649,655,648,651,651,652,655,651,650,653,653,652,651,655,654,649,649,648,655,
		656,657,658,658,659,656,660,661,662,662,663,660,657,662,661,661,658,657,663,656,659,659,660,663,659,658,661,661,660,659,663,662,657,657,656,663,
		664,665,666,666,667,664,668,669,670,670,671,668,665,670,669,669,666,665,671,664,667,667,668,671,667,666,669,669,668,667,671,670,665,665,664,671,
		672,673,674,674,675,672,676,677,678,678,679,676,673,678,677,677,674,673,679,672,675,675,676,679,675,674,677,677,676,675,679,678,673,673,672,679,
		680,681,682,682,683,680,684,685,686,686,687,684,681,686,685,685,682,681,687,680,683,683,684,687,683,682,685,685,684,683,687,686,681,681,680,687,
		688,689,690,690,691,688,692,693,694,694,695,692,689,694,693,693,690,689,695,688,691,691,692,695,691,690,693,693,692,691,695,694,689,689,688,695,
		696,697,698,698,699,696,700,701,702,702,703,700,697,702,701,701,698,697,703,696,699,699,700,703,699,698,701,701,700,699,703,702,697,697,696,703,
		704,705,706,706,707,704,708,709,710,710,711,708,705,710,709,709,706,705,711,704,707,707,708,711,707,706,709,709,708,707,711,710,705,705,704,711,
		712,713,714,714,715,712,716,717,718,718,719,716,713,718,717,717,714,713,719,712,715,715,716,719,715,714,717,717,716,715,719,718,713,713,712,719,
		720,721,722,722,723,720,724,725,726,726,727,724,721,726,725,725,722,721,727,720,723,723,724,727,723,722,725,725,724,723,727,726,721,721,720,727,
		728,729,730,730,731,728,732,733,734,734,735,732,729,734,733,733,730,729,735,728,731,731,732,735,731,730,733,733,732,731,735,734,729,729,728,735,
		736,737,738,738,739,736,740,741,742,742,743,740,737,742,741,741,738,737,743,736,739,739,740,743,739,738,741,741,740,739,743,742,737,737,736,743,
		744,745,746,746,747,744,748,749,750,750,751,748,745,750,749,749,746,745,751,744,747,747,748,751,747,746,749,749,748,747,751,750,745,745,744,751,
		752,753,754,754,755,752,756,757,758,758,759,756,753,758,757,757,754,753,759,752,755,755,756,759,755,754,757,757,756,755,759,758,753,753,752,759,
		760,761,762,762,763,760,764,765,766,766,767,764,761,766,765,765,762,761,767,760,763,763,764,767,763,762,765,765,764,763,767,766,761,761,760,767,
		768,769,770,770,771,768,772,773,774,774,775,772,769,774,773,773,770,769,775,768,771,771,772,775,771,770,773,773,772,771,775,774,769,769,768,775,
		776,777,778,778,779,776,780,781,782,782,783,780,777,782,781,781,778,777,783,776,779,779,780,783,779,778,781,781,780,779,783,782,777,777,776,783,
		784,785,786,786,787,784,788,789,790,790,791,788,785,790,789,789,786,785,791,784,787,787,788,791,787,786,789,789,788,787,791,790,785,785,784,791,
		792,793,794,794,795,792,796,797,798,798,799,796,793,798,797,797,794,793,799,792,795,795,796,799,795,794,797,797,796,795,799,798,793,793,792,799
	};


	m_ib = GraphEng::get()->createIndexBuffer();
	UINT size_index_list = ARRAYSIZE(index_list);

	m_ib->load(index_list, size_index_list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphEng::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphEng::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(vertex_list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphEng::get()->releaseCompiledShader();

	GraphEng::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphEng::get()->createPixelShader(shader_byte_code, size_shader);

	GraphEng::get()->releaseCompiledShader();
}



void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	GraphEng::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.2, 0.5, 1, 1);

	RECT rc = this->getClientWindowRect();
	GraphEng::get()->getImmediateDeviceContext()->setViewPortSize(rc.right - rc.left, rc.bottom - rc.top);
	

	update();

	GraphEng::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphEng::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	GraphEng::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphEng::get()->getImmediateDeviceContext()->setPixelShader(m_ps);


	GraphEng::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
	GraphEng::get()->getImmediateDeviceContext()->setIndexBuffer(m_ib);

	GraphEng::get()->getImmediateDeviceContext()->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);

	m_swap_chain->present(true);

	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_ib->release();
	m_cb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphEng::get()->release();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key) 
{
	if (key == 'W') 
	{
		m_forward = 1.0f;
	}
	if (key == 'S')
	{
		m_forward = -1.0f;
	}
	if (key == 'A')
	{
		m_strafe = -1.0f;
	}
	if (key == 'D')
	{
		m_strafe = 1.0f;
	}
	if (key == 'X') 
	{

		m_is_run = false;
	}
	
}

void AppWindow::onKeyUp(int key) 
{
	m_forward = 0.0f;
	m_strafe = 0.0f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int width = this->getClientWindowRect().right - this->getClientWindowRect().left;
	int height = this->getClientWindowRect().bottom - this->getClientWindowRect().top;

	m_rot_x += (mouse_pos.m_y - (height / 2.0f)) * m_delta_time * 0.1f;
	m_rot_y += (mouse_pos.m_x - (width / 2.0f)) * m_delta_time * 0.1f;
	

	InputSystem::get()->setCursorPosition(Point(width / 2.0f, height / 2.0f));

}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	if (!fired) 
	{
		fired = true;
		CheckShot();

	}
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	fired = false;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 2.0f;
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}
