#include <chrono>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <atlstr.h>
#include <fstream>
#define TEAM_NUMBER 3674
#define _WINNT_WIN32 0600
#include <shobjidl.h>
#include "GUI.h"

typedef std::chrono::high_resolution_clock Clock;

template< typename TContainer >
static bool EraseFromUnorderedByIndex(TContainer& inContainer, size_t inIndex)
{
	if (inIndex < inContainer.size())
	{
		if (inIndex != inContainer.size() - 1)
			inContainer[inIndex] = inContainer.back();
		inContainer.pop_back();
		return true;
	}
	return false;
}


struct vec2 {
	float x, y;
	vec2(float x, float y) : x(x), y(y) {}
	vec2() {}
};

vec2 operator + (vec2 a, vec2 b) {
	return vec2(a.x + b.x, a.y + b.y);
}

vec2 operator - (vec2 a, vec2 b) {
	return vec2(a.x - b.x, a.y - b.y);
}

vec2 operator * (float s, vec2 a) {
	return vec2(s * a.x, s * a.y);
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{

	if (uMsg == BFFM_INITIALIZED)
	{
		std::string tmp = (const char *)lpData;
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}

std::string BrowseFolder(std::string saved_path)
{
	TCHAR path[MAX_PATH];

	const char * path_param = saved_path.c_str();

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = ("Browse for folder...");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)path_param;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDList(pidl, path);

		//free memory used
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		return path;
	}

	return "";
}

std::string openFile(HWND hwnd)
{
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Path generator file\0*.pga*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a File";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
	bool done = false;
	if (GetOpenFileNameA(&ofn))
	{
		return filename;
	}
	else
	{
		switch (CommDlgExtendedError())
		{
			case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
			case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
			case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
			case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
			case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
			case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
			case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
			case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
			case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
			case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
			case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
			case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
			case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
			case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
			case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
			default: std::cout << "You cancelled.\n";
		}
		return "";
	}
}

vec2 getBezierPoint(std::vector<vec2> points, int numPoints, float t)
{
	vec2* points_ = points.data();

	vec2* tmp = new vec2[numPoints];
	memcpy(tmp, points_, numPoints * sizeof(vec2));
	int i = numPoints - 1;
	while (i > 0)
	{
		for (int k = 0; k < i; k++)
			tmp[k] = tmp[k] + t * (tmp[k + 1] - tmp[k]);
		i--;
	}
	vec2 answer = tmp[0];
	delete[] tmp;
	return answer;
}
int main()
{
	std::vector<vec2> saved_points_l;
	std::vector<vec2> saved_points_c;
	std::vector<vec2> saved_points_r;

	std::vector<std::vector<vec2>> paths;
	{
		std::vector<vec2> points;
		points.push_back(vec2(1, 1));
		points.push_back(vec2(100, 100));
		paths.push_back(points);
	}

	sf::Image field;
	if (!field.loadFromFile("data/field.png"))
	{
		std::cout << "ERROR: unable to load data/field.png" << std::endl;
	}

	sf::RenderWindow window(sf::VideoMode(field.getSize().x + 300, field.getSize().y + 200), "3674-profiler-E5A", sf::Style::Close);
	window.setFramerateLimit(1 / 60);
	window.setPosition(sf::Vector2i(0, 0));
	sf::Image icon;
	if(!icon.loadFromFile("data/icon.png"))
	{
		std::cout << "ERROR: unable to load data/icon.png" << std::endl;
	}
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	HWND hwnd = window.getSystemHandle();
	sf::Font font;
	if (!font.loadFromFile("data/consola.ttf"))
	{
		std::cout << "ERROR: unable to load font data/consola.ttf" << std::endl;
	}

	sf::Text text;
	text.setFont(font);
	text.setString("0");
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::Black);
	text.setStyle(sf::Text::Regular);

	GUI::Button open_field = GUI::Button("data/load_field.png", "data/load_field_hover.png", "data/load_field.png", sf::Vector2f(0, field.getSize().y));
	GUI::Button add_point = GUI::Button("data/point/add_point.png", "data/point/add_point_hover.png", "data/point/add_point.png", sf::Vector2f(0, field.getSize().y + 50));
	GUI::Button delete_point = GUI::Button("data/point/delete_point.png", "data/point/delete_point_hover.png", "data/point/delete_point.png", sf::Vector2f(0, field.getSize().y + 100));
	GUI::Button add_path = GUI::Button("data/path/add_path.png", "data/path/add_path_hover.png", "data/path/add_path.png", sf::Vector2f(165, field.getSize().y + 50));
	GUI::Button delete_path = GUI::Button("data/path/delete_path.png", "data/path/delete_path_hover.png", "data/path/delete_path.png", sf::Vector2f(165, field.getSize().y + 100));
	GUI::ToggleButton sh_points = GUI::ToggleButton("data/point/show_points.png", "data/point/hide_points.png", sf::Vector2f(165, field.getSize().y));
	GUI::Scroll scroll = GUI::Scroll("data/scroll.png", sf::Vector2f(field.getSize().x + 280, 1), field.getSize().y + 150);
	GUI::ToggleButton button = GUI::ToggleButton("data/button.png", "data/button_hover.png", sf::Vector2f(0, 0));
	GUI::Button trace_path = GUI::Button("data/path/trace_path.png", "data/path/trace_path_hover.png", "data/path/trace_path.png", sf::Vector2f(165, field.getSize().y + 150));
	GUI::ToggleButton path_button = GUI::ToggleButton("data/button.png", "data/button_hover.png", sf::Vector2f(0, 0));
	GUI::NumberBox width_box = GUI::NumberBox("data/num_box.png", sf::Vector2f(334, field.getSize().y), "W: ", 7);
	GUI::NumberBox length_box = GUI::NumberBox("data/num_box.png", sf::Vector2f(334, field.getSize().y + 50), "L: ", 7);
	GUI::Button load = GUI::Button("data/load.png", "data/load_hover.png", "data/load.png", sf::Vector2f(334 + 145, field.getSize().y));
	GUI::Button save = GUI::Button("data/save.png", "data/save_hover.png", "data/save.png", sf::Vector2f(334 + 145, field.getSize().y + 50));
	GUI::ToggleButton lock = GUI::ToggleButton("data/lock.png", "data/lock_hover.png", sf::Vector2f(0, 0));

	std::vector<GUI::ToggleButton> lock_buttons;
	lock_buttons.push_back(lock);

	std::vector<bool> target;
	std::vector<bool> button_once;
	std::vector<GUI::ToggleButton> buttons_;
	std::vector<GUI::ToggleButton> path_buttons;
	std::vector<bool> path_button_once;
	std::vector<std::vector<GUI::ToggleButton>> buttons;
	std::vector<std::vector<bool>> buttons_once;
	std::vector<std::vector<bool>> targets;
	std::vector<bool> path_targeted;

	path_targeted.push_back(false);
	target.push_back(false);
	target.push_back(false);
	button_once.push_back(false);
	button_once.push_back(false);
	buttons_.push_back(button);
	buttons_.push_back(button);
	path_buttons.push_back(button);

	buttons.push_back(buttons_);
	buttons.push_back(buttons_);
	buttons_once.push_back(button_once);
	buttons_once.push_back(button_once);
	targets.push_back(target);
	targets.push_back(target);

	bool add_point_once = false;
	bool delete_point_once = false;
	bool add_path_once = false;
	bool delete_path_once = false;

	sf::Image point_img;
	if(!point_img.loadFromFile("data/point.png"))
	{
		std::cout << "ERROR: unable to load data/point.png" << std::endl;
	}
	sf::Texture point_tex;
	point_tex = sf::Texture();
	point_tex.loadFromImage(point_img);
	sf::Sprite point_sprite = sf::Sprite();
	point_sprite.setTexture(point_tex);
	point_sprite.setPosition(sf::Vector2f(0.f, 0.f));

	sf::Image comp_img;
	if (!comp_img.loadFromFile("data/comp.png"))
	{
		std::cout << "ERROR: unable to load data/comp.png" << std::endl;
	}
	sf::Texture comp_tex;
	comp_tex = sf::Texture();
	comp_tex.loadFromImage(comp_img);
	sf::Sprite comp_sprite = sf::Sprite();
	comp_sprite.setTexture(comp_tex);
	comp_sprite.scale(sf::Vector2f(.89, .89));
	comp_sprite.setPosition(sf::Vector2f(710, field.getSize().y + 3));

	double rwidth = 10;
	while (window.isOpen())
	{
		auto t1 = Clock::now();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		double speed = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			speed = 5;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		{
			speed = .1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			for (int p = 0; p < paths.size(); p++)
			{
				for (int i = 0; i < targets.at(p).size(); i++)
				{
					if (targets.at(p).at(i))
					{
						paths.at(p).at(i).y -= speed;
					}
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			for (int p = 0; p < paths.size(); p++)
			{
				for (int i = 0; i < targets.at(p).size(); i++)
				{
					if (targets.at(p).at(i))
					{
						paths.at(p).at(i).y += speed;
					}
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			for (int p = 0; p < paths.size(); p++)
			{
				for (int i = 0; i < targets.at(p).size(); i++)
				{
					if (targets.at(p).at(i))
					{
						paths.at(p).at(i).x -= speed;
					}
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			for (int p = 0; p < paths.size(); p++)
			{
				for (int i = 0; i < targets.at(p).size(); i++)
				{
					if (targets.at(p).at(i))
					{
						paths.at(p).at(i).x += speed;
					}
				}
			}
		}

		sf::Texture field_tex;
		field_tex = sf::Texture();
		field_tex.loadFromImage(field);
		sf::Sprite field_sprite = sf::Sprite();
		field_sprite.setTexture(field_tex);
		field_sprite.setPosition(sf::Vector2f(0.f, 0.f));

		window.clear(sf::Color(0,0,0));

		window.draw(field_sprite);
		saved_points_l.clear();
		saved_points_c.clear();
		saved_points_r.clear();

		for (int p = 0; p < paths.size(); p++)
		{
			std::vector<vec2> lpoints;
			
			for (double i = 0; i < 1.01; i += .01)
			{
				vec2 pn = getBezierPoint(paths.at(p), paths.at(p).size(), i);
				lpoints.push_back(pn);
			}
			sf::VertexArray path(sf::LinesStrip, lpoints.size());
			sf::VertexArray leftpath(sf::LinesStrip, lpoints.size());
			sf::VertexArray rightpath(sf::LinesStrip, lpoints.size());
			for (int i = 0; i < lpoints.size(); i++)
			{
				path[i].position = sf::Vector2f(lpoints.at(i).x, lpoints.at(i).y);
				path[i].color = sf::Color(0, 0, 0, 255);
				sf::Vector2f temp_left;
				sf::Vector2f temp_right;
				if (i < lpoints.size() - 1)
				{
					if ((lpoints.at(i).x - lpoints.at(i + 1).x) < 0)
					{
						temp_left = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
						temp_right = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
					}
					else
					{
						temp_left = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
						temp_right = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i + 1).y) / (lpoints.at(i).x - lpoints.at(i + 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
					}
				}
				else
				{
					if ((lpoints.at(i).x - lpoints.at(i - 1).x) > 0)
					{
						temp_left = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
						temp_right = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
					}
					else
					{
						temp_left = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) + (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
						temp_right = sf::Vector2f(
							(cos(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).x,
							(sin(atan((lpoints.at(i).y - lpoints.at(i - 1).y) / (lpoints.at(i).x - lpoints.at(i - 1).x)) - (3.14159265359 / 2)) * rwidth) + lpoints.at(i).y);
					}
				}
				leftpath[i].position = temp_left;
				rightpath[i].position = temp_right;
				leftpath[i].color = sf::Color(255, 0, 0, 255);
				rightpath[i].color = sf::Color(0, 0, 255, 255);
				saved_points_l.push_back(vec2(temp_left.x, temp_left.y));
				saved_points_c.push_back(vec2(lpoints[i].x, lpoints[i].y));
				saved_points_r.push_back(vec2(temp_right.x, temp_right.y));
			}
			saved_points_l.push_back(vec2(TEAM_NUMBER, TEAM_NUMBER));
			saved_points_c.push_back(vec2(TEAM_NUMBER, TEAM_NUMBER));
			saved_points_r.push_back(vec2(TEAM_NUMBER, TEAM_NUMBER));
			window.draw(path);
			window.draw(leftpath);
			window.draw(rightpath);
		}

		GUI::ButtonState open_field_state = open_field.Update(&window);
		GUI::ButtonState add_point_state = add_point.Update(&window);
		GUI::ButtonState delete_point_state = delete_point.Update(&window);
		GUI::ButtonState add_path_state = add_path.Update(&window);
		GUI::ButtonState delete_path_state = delete_path.Update(&window);
		GUI::ButtonState sh_points_state = sh_points.Update(&window);
		int scroll_state = scroll.Update(&window);//478
		GUI::ButtonState trace_path_state = trace_path.Update(&window);
		std::string width_box_state = width_box.Update(&window);
		std::string length_box_state = length_box.Update(&window);
		GUI::ButtonState save_state = save.Update(&window);
		GUI::ButtonState load_state = load.Update(&window);

		if (save_state == GUI::ButtonState::Clicked)
		{
			OPENFILENAME ofn;
			char filename[100] = "untitled";
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = filename;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrFilter = "Text\0*.txt*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			GetSaveFileName(&ofn);
			std::cout << filename << std::endl;


			std::ofstream userfile;
			userfile.open(filename + std::string(".txt"));
			for (int j = 0; j < saved_points_c.size(); j++)
			{
				userfile << saved_points_l.at(j).x << ", " << saved_points_l.at(j).y << ", " << saved_points_c.at(j).x
					<< ", " << saved_points_c.at(j).y << ", " << saved_points_r.at(j).x << ", " << saved_points_r.at(j).y << std::endl;
			}
			userfile.close();

			std::ofstream profilefile;
			ofn.lpstrFilter = "Path generator file\0*.pga*\0";
			GetSaveFileName(&ofn);
			profilefile.open(filename + std::string(".pga"));
			for (int j = 0; j < paths.size(); j++)
			{
				for (int h = 0; h < paths.at(j).size(); h++)
				{
					profilefile << paths.at(j).at(h).x << " " << paths.at(j).at(h).y << std::endl;
				}
				if (j != paths.size() - 1)
				{
					profilefile << TEAM_NUMBER << " " << TEAM_NUMBER << std::endl;
				}
			}
			profilefile.close();
		}
		if (load_state == GUI::ButtonState::Clicked)
		{
			std::string filename = openFile(hwnd);
			std::ifstream infile(filename);
			if (filename != "")
			{
				paths.clear();
				path_buttons.clear();
				lock_buttons.clear();
				path_targeted.clear();
				buttons.clear();
				buttons_once.clear();
				
				std::vector<vec2> points;
				paths.push_back(points);
				buttons.push_back(buttons_);
				buttons_once.push_back(button_once);
				targets.push_back(target);
				path_buttons.push_back(button);
				lock_buttons.push_back(lock);
				path_targeted.push_back(false);
				int index = 0;
				double a, b;
				while (infile >> a >> b)
				{
					std::cout << a << " : " << b << std::endl;
					if (a == TEAM_NUMBER && b == TEAM_NUMBER)
					{
						paths.push_back(points);
						buttons.push_back(buttons_);
						buttons_once.push_back(button_once);
						targets.push_back(target);
						path_buttons.push_back(button);
						lock_buttons.push_back(lock);
						path_targeted.push_back(false);
						index++;
					}
					else
					{
						paths.at(index).push_back(vec2(a, b));
						buttons.at(index).push_back(button);
						buttons_once.at(index).push_back(false);
						targets.at(index).push_back(false);
					}
				}
			}
		}

		rwidth = (double)atoi(width_box_state.c_str()) / 2;
		int button_pos = 0;
		for (int p = 0; p < paths.size(); p++)
		{
			int paths_size = paths.size() - 1;
			for (int j = 0; j < paths.size(); j++)
			{
				paths_size += paths.at(j).size();
			}
			path_buttons.at(p).ChangePos(sf::Vector2f(field.getSize().x + (134 / 2), ((button_pos) * 50) - (scroll_state * paths_size / 10)));

			if (path_buttons.at(p).Update(&window) == GUI::ButtonState::Clicked)
			{
				path_targeted.at(p) = true;
			}
			else
			{
				path_targeted.at(p) = false;
			}
			path_buttons.at(p).Draw(&window);

			lock_buttons.at(p).ChangePos(sf::Vector2f(field.getSize().x + (134 / 2) - 62, ((button_pos) * 50) - (scroll_state * paths_size / 10)));
			lock_buttons.at(p).Draw(&window);

			text.setPosition(sf::Vector2f(field.getSize().x + (134 / 2)+3, ((button_pos) * 50) - (scroll_state * paths_size / 10)));
			text.setString(std::to_string(p));
			if (p == 0)
			{
				lock_buttons.at(p).Update(&window);
			}
			else
			{
				paths.at(p).at(0) = paths.at(p - 1).at(paths.at(p - 1).size() - 1);
				if (lock_buttons.at(p).Update(&window) == GUI::ButtonState::Normal)
				{
					paths.at(p).at(1) = paths.at(p - 1).at(paths.at(p - 1).size() - 2);
				}
			}
			window.draw(text);
			button_pos++;

			for (int i = 0; i < paths.at(p).size(); i++)
			{
				buttons.at(p).at(i).ChangePos(sf::Vector2f(field.getSize().x + 134, ((button_pos) * 50) - (scroll_state *paths_size / 10)));
				text.setPosition(sf::Vector2f(field.getSize().x + 134 + 3, ((button_pos) * 50) - (scroll_state * paths_size / 10)));
				button_pos++;
				if (i == 0)
				{
					text.setString(std::to_string(i) + " start");
				}
				else if (i == paths.at(p).size() - 1)
				{
					text.setString(std::to_string(i) + " end");
				}
				else
				{
					text.setString(std::to_string(i));
				}
				if (buttons.at(p).at(i).Update(&window) == GUI::ButtonState::Clicked)
				{
					if (delete_point_state == GUI::ButtonState::Clicked)
					{
						EraseFromUnorderedByIndex(paths.at(p), i);
						EraseFromUnorderedByIndex(buttons.at(p), i);
						EraseFromUnorderedByIndex(buttons_once.at(p), i);
						EraseFromUnorderedByIndex(targets.at(p), i);
						continue;
					}
					if (!buttons_once.at(p).at(i))
					{
						targets.at(p).at(i) = true;
					}
					buttons_once.at(p).at(i) = true;
				}
				else
				{
					targets.at(p).at(i) = false;
					buttons_once.at(p).at(i) = false;
				}

				buttons.at(p).at(i).Draw(&window);
 				window.draw(text);
			}
		}
		if (add_point_state == GUI::ButtonState::Clicked)
		{
			if (!add_point_once)
			{ 
				for (int p = 0; p < paths.size(); p++) 
				{
					if (path_targeted.at(p))
					{
						paths.at(p).push_back(vec2(0, 0));
						buttons.at(p).push_back(button);
						buttons_once.at(p).push_back(false);
						targets.at(p).push_back(false);
					}
				}
			}
			add_point_once = true;
		}
		else
		{
			add_point_once = false;
		}
		if (add_path_state == GUI::ButtonState::Clicked)
		{
			if (!add_path_once)
			{
				std::vector<vec2> points;
				points.push_back(vec2(1, 1));
				points.push_back(vec2(100, 100));

				paths.push_back(points);
				buttons.push_back(buttons_);
				buttons_once.push_back(button_once);
				targets.push_back(target);
				targets.push_back(target);
				path_buttons.push_back(button);
				lock_buttons.push_back(lock);
				path_targeted.push_back(false);
			}
			add_path_once = true;
		}
		else
		{
			add_path_once = false;
		}
		if (delete_path_state == GUI::ButtonState::Clicked)
		{
			if (!delete_path_once)
			{
				for (int p = 0; p < paths.size(); p++)
				{
					if (path_buttons.at(p).Update(&window) == GUI::ButtonState::Clicked)
					{
						EraseFromUnorderedByIndex(paths, p);
						EraseFromUnorderedByIndex(path_buttons, p);
						EraseFromUnorderedByIndex(lock_buttons, p);
						EraseFromUnorderedByIndex(path_targeted, p);
						EraseFromUnorderedByIndex(buttons, p);
						EraseFromUnorderedByIndex(buttons_once, p);
					}
				}
			}
			delete_path_once = true;
		}
		else
		{
			delete_path_once = false;
		}
		if (sh_points_state == GUI::ButtonState::Clicked)
		{
			for (int p = 0; p < paths.size(); p++)
			{
				for (int i = 0; i < paths.at(p).size(); i++)
				{
					point_sprite.setPosition(sf::Vector2f(paths.at(p).at(i).x - point_sprite.getTexture()->getSize().x / 2, paths.at(p).at(i).y - point_sprite.getTexture()->getSize().y / 2));
					window.draw(point_sprite);
				}
			}
		}
		open_field.Draw(&window);
		add_point.Draw(&window);
		delete_point.Draw(&window);
		add_path.Draw(&window);
		delete_path.Draw(&window);
		sh_points.Draw(&window);
		scroll.Draw(&window);
		trace_path.Draw(&window);
		width_box.Draw(&window);
		length_box.Draw(&window);
		save.Draw(&window);
		load.Draw(&window);
		text.setFillColor(sf::Color::Green);

		text.setPosition(sf::Vector2f(field.getSize().x - 300, field.getSize().y + 110));
		text.setString("Paths: " + std::to_string(paths.size()));
		window.draw(text);

		text.setPosition(sf::Vector2f(field.getSize().x - 300, field.getSize().y + 140));
		int pc = 0;
		for (int p = 0; p < paths.size(); p++)
		{
			pc += paths.at(p).size();
		}
		text.setString("Points: " + std::to_string(pc));
		window.draw(text);
		window.draw(comp_sprite);

		text.setPosition(sf::Vector2f(field.getSize().x - 300, field.getSize().y + 170));
		auto t2 = Clock::now();
		text.setString(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) + " (ms) Frame Time");
		window.draw(text);
		text.setFillColor(sf::Color::Black);
		//window.draw(sprite);
		window.display();
	}
	return 0;
}