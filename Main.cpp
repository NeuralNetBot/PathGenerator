#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "GUI.h"

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

std::string openFile(HWND hwnd)
{
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Any File\0*.*\0";
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
		// All this stuff below is to tell you exactly how you messed up above. 
		// Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
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

vec2 getBezierPoint(std::vector<vec2> points, int numPoints, float t) {
	
	vec2* points_ = points.data();

	vec2* tmp = new vec2[numPoints];
	memcpy(tmp, points_, numPoints * sizeof(vec2));
	int i = numPoints - 1;
	while (i > 0) {
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

	std::vector<std::vector<vec2>> paths;
	{
		std::vector<vec2> points;
		points.push_back(vec2(1, 1));
		points.push_back(vec2(100, 100));
		paths.push_back(points);
	}
	int point_count = 4;

	sf::Image field;
	field.loadFromFile("data/2019-field.jpg");

	sf::RenderWindow window(sf::VideoMode(field.getSize().x/2 + 300, field.getSize().y/2 + 200), "profiler", sf::Style::Close);
	window.setPosition(sf::Vector2i(0, 0));
	HWND hwnd = window.getSystemHandle();

	GUI::Button open_field = GUI::Button("data/load_field.png", "data/load_field_hover.png", "data/load_field.png", sf::Vector2f(0, field.getSize().y / 2));
	GUI::Button add_point = GUI::Button("data/point/add_point.png", "data/point/add_point_hover.png", "data/point/add_point.png", sf::Vector2f(0, field.getSize().y / 2 + 50));
	GUI::Button delete_point = GUI::Button("data/point/delete_point.png", "data/point/delete_point_hover.png", "data/point/delete_point.png", sf::Vector2f(0, field.getSize().y / 2 + 100));
	GUI::Button add_path = GUI::Button("data/path/add_path.png", "data/path/add_path_hover.png", "data/path/add_path.png", sf::Vector2f(165, field.getSize().y / 2 + 50));
	GUI::Button delete_path = GUI::Button("data/path/delete_path.png", "data/path/delete_path_hover.png", "data/path/delete_path.png", sf::Vector2f(165, field.getSize().y / 2 + 100));
	GUI::ToggleButton sh_points = GUI::ToggleButton("data/point/show_points.png", "data/point/hide_points.png", sf::Vector2f(165, field.getSize().y / 2));
	GUI::Scroll scroll = GUI::Scroll("data/scroll.png", sf::Vector2f(field.getSize().x / 2 + 280, 1), field.getSize().y / 2 + 150);
	GUI::ToggleButton button = GUI::ToggleButton("data/button.png", "data/button_hover.png", sf::Vector2f(0,0));
	std::vector<GUI::ToggleButton> buttons;
	std::vector<bool> buttons_once;
	std::vector<bool> targets;

	targets.push_back(false);
	targets.push_back(false);

	buttons.push_back(button);
	buttons.push_back(button);
	buttons_once.push_back(false);
	buttons_once.push_back(false);

	bool add_point_once = false;
	bool delete_point_once = false;
	bool add_path_once = false;
	bool delete_path_once = false;

	sf::Image point_img;
	point_img.loadFromFile("data/point.png");
	sf::Texture point_tex;
	point_tex = sf::Texture();
	point_tex.loadFromImage(point_img);
	sf::Sprite point_sprite = sf::Sprite();
	point_sprite.setTexture(point_tex);
	point_sprite.setPosition(sf::Vector2f(0.f, 0.f));


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Image image;
		image.create(field.getSize().x, field.getSize().y, sf::Color(255, 255, 255, 0));
		int speed = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			speed = 5;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			for (int i = 0; i < targets.size(); i++)
			{
				if (targets.at(i))
				{
					paths.at(0).at(i).y -= speed;
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			for (int i = 0; i < targets.size(); i++)
			{
				if (targets.at(i))
				{
					paths.at(0).at(i).y += speed;
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			for (int i = 0; i < targets.size(); i++)
			{
				if (targets.at(i))
				{
					paths.at(0).at(i).x -= speed;
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			for (int i = 0; i < targets.size(); i++)
			{
				if (targets.at(i))
				{
					paths.at(0).at(i).x += speed;
				}
			}
		}


		//std::cout << "start" << std::endl;
		std::vector<vec2> lpoints;
		point_count = paths.at(0).size();
		for (double i = 0; i < 1; i += .01)
		{
			vec2 p = getBezierPoint(paths.at(0), point_count, i);
			//std::cout << p.x << " : " << p.y  << " : "<<i/.01<< std::endl;
			lpoints.push_back(p);
			//lines[i / .01] = sf::Vector2f(p.x, p.y);
			//image.setPixel(p.x, p.y, sf::Color(0, 255, 0));
		}
		sf::VertexArray lines(sf::LinesStrip, lpoints.size());
		for (int i = 0; i < lpoints.size(); i++)
		{
			lines[i].position = sf::Vector2f(lpoints.at(i).x, lpoints.at(i).y);
			lines[i].color = sf::Color(0, 0, 0, 255);
		}
		sf::Texture tex;
		tex = sf::Texture();
		tex.loadFromImage(image);
		sf::Sprite sprite = sf::Sprite();
		sprite.setTexture(tex);
		sprite.setPosition(sf::Vector2f(0.f, 0.f));
		sprite.scale(sf::Vector2f(.5, .5));


		sf::Texture field_tex;
		field_tex = sf::Texture();
		field_tex.loadFromImage(field);
		sf::Sprite field_sprite = sf::Sprite();
		field_sprite.setTexture(field_tex);
		field_sprite.setPosition(sf::Vector2f(0.f, 0.f));
		field_sprite.scale(sf::Vector2f(.5, .5));

		window.clear(sf::Color(0,0,0));

		window.draw(field_sprite);
		window.draw(lines);

		GUI::ButtonState open_field_state = open_field.Update(&window);
		GUI::ButtonState add_point_state = add_point.Update(&window);
		GUI::ButtonState delete_point_state = delete_point.Update(&window);
		GUI::ButtonState add_path_state = add_path.Update(&window);
		GUI::ButtonState delete_path_state = delete_path.Update(&window);
		GUI::ButtonState sh_points_state = sh_points.Update(&window);
		int scroll_state = scroll.Update(&window);//478

		for (int i = 0; i < paths.at(0).size(); i++)
		{
			buttons.at(i).ChangePos(sf::Vector2f(field.getSize().x / 2 + 134, field.getSize().y / 2 + 150 - (scroll_state + (i * 50))));
			
			if (buttons.at(i).Update(&window) == GUI::ButtonState::Clicked)
			{
				if (!buttons_once.at(i))
				{
					targets.at(i) = true;
				}
				buttons_once.at(i) = true;
			}
			else
			{
				targets.at(i) = false;
				buttons_once.at(i) = false;
			}
			buttons.at(i).Draw(&window);
		}

		open_field.Draw(&window);
		add_point.Draw(&window);
		delete_point.Draw(&window);
		add_path.Draw(&window);
		delete_path.Draw(&window);
		sh_points.Draw(&window);
		scroll.Draw(&window);


		if (add_point_state == GUI::ButtonState::Clicked)
		{
			if (!add_point_once)
			{ 
				paths.at(0).push_back(vec2(0, 0));
				buttons.push_back(button);
				buttons_once.push_back(false);
				targets.push_back(false);
			}
			add_point_once = true;
		}
		else
		{
			add_point_once = false;
		}

		if (sh_points_state == GUI::ButtonState::Clicked)
		{
			for (int i = 0; i < paths.at(0).size(); i++)
			{
				point_sprite.setPosition(sf::Vector2f(paths.at(0).at(i).x - point_sprite.getTexture()->getSize().x, paths.at(0).at(i).y - point_sprite.getTexture()->getSize().y /2));
				window.draw(point_sprite);
			}
		}
		//window.draw(sprite);
		window.display();
	}
	std::cin.get();
	return 0;
}