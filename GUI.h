#pragma once

#include <SFML/Graphics.hpp>

namespace GUI
{
	enum ButtonState
	{
		Normal,
		Hover,
		Clicked
	};

	class Button
	{
		sf::Texture tex[3];
		sf::IntRect rect[3];
		sf::Vector2f pos;
		ButtonState state;
		sf::Sprite* normal;
		sf::Sprite* hover;
		sf::Sprite* clicked;
		sf::Sprite* current;
	public:
		Button() {};
		Button(std::string, std::string, std::string, sf::Vector2f);
		ButtonState Update(sf::RenderWindow*);
		void Draw(sf::RenderWindow*);
		void ChangePos(sf::Vector2f);
		~Button();
	};

	class ToggleButton
	{
		sf::Texture tex[2];
		sf::IntRect rect[2];
		sf::Vector2f pos;
		ButtonState state;
		sf::Sprite* normal;
		sf::Sprite* clicked;
		sf::Sprite* current;
		bool toggled = false;
	public:
		ToggleButton() {};
		ToggleButton(std::string, std::string, sf::Vector2f);
		ButtonState Update(sf::RenderWindow*);
		void Draw(sf::RenderWindow*);
		void ChangePos(sf::Vector2f);
		void SetState(GUI::ButtonState state);
		~ToggleButton();
	};

	class Scroll
	{
		sf::Texture tex;
		sf::IntRect rect;
		sf::Vector2f pos;
		int start_y;
		int size = 100;
		sf::Sprite* current;
		bool pressed = false;
		
	public:
		Scroll() {};
		Scroll(std::string, sf::Vector2f, int);
		int Update(sf::RenderWindow*);
		void Draw(sf::RenderWindow*);
		~Scroll();
	};

	class NumberBox
	{
		std::string s;
		sf::Texture tex;
		sf::IntRect rect;
		sf::Vector2f pos;
		ButtonState state;
		sf::Sprite* current;
		std::string text;
		bool pressed[10] = { false };
		bool bpressed = false;
		sf::Text text_;
		sf::Font font;
		int size;
	public:
		NumberBox() {};
		NumberBox(std::string, sf::Vector2f, std::string, int);
		std::string Update(sf::RenderWindow*);
		void Draw(sf::RenderWindow*);
		~NumberBox();
	};

}

