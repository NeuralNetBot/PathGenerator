#include "GUI.h"

#include <iostream>

namespace GUI
{

	Button::Button(std::string normal_dir, std::string hover_dir, std::string clicked_dir, sf::Vector2f pos)
	{
		this->pos = pos;
		tex[0].loadFromFile(normal_dir);
		rect[0] = sf::IntRect(0, 0, tex[0].getSize().x, tex[0].getSize().y);
		normal = new sf::Sprite(tex[0], rect[0]);
		normal->setPosition(pos);

		tex[1].loadFromFile(hover_dir);
		rect[1] = sf::IntRect(0, 0, tex[1].getSize().x, tex[1].getSize().y);
		hover = new sf::Sprite(tex[1], rect[1]);
		hover->setPosition(pos);

		tex[2].loadFromFile(clicked_dir);
		rect[2] = sf::IntRect(0, 0, tex[2].getSize().x, tex[2].getSize().y);
		clicked = new sf::Sprite(tex[2], rect[2]);
		clicked->setPosition(pos);

		current = normal;
		state = ButtonState::Normal;
	}

	ButtonState Button::Update(sf::RenderWindow* window)
	{
		if (current->getTextureRect().contains(sf::Mouse::getPosition(*window) - sf::Vector2i(current->getPosition())))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				current = clicked;
				state = ButtonState::Clicked;
			}
			else
			{
				current = hover;
				state = ButtonState::Hover;
			}
		}
		else
		{
			current = normal;
			state = ButtonState::Normal;
		}
		return state;
	}

	void Button::Draw(sf::RenderWindow* window)
	{
		current->setPosition(pos);
		window->draw(*current);
	}

	void Button::ChangePos(sf::Vector2f pos)
	{
		this->pos = pos;
	}

	Button::~Button()
	{
	}


	ToggleButton::ToggleButton(std::string normal_dir, std::string clicked_dir, sf::Vector2f pos)
	{
		this->pos = pos;
		tex[0].loadFromFile(normal_dir);
		rect[0] = sf::IntRect(0, 0, tex[0].getSize().x, tex[0].getSize().y);
		normal = new sf::Sprite(tex[0], rect[0]);
		normal->setPosition(pos);

		tex[1].loadFromFile(clicked_dir);
		rect[1] = sf::IntRect(0, 0, tex[1].getSize().x, tex[1].getSize().y);
		clicked = new sf::Sprite(tex[1], rect[1]);
		clicked->setPosition(pos);

		current = normal;
		state = ButtonState::Normal;
	}

	ButtonState ToggleButton::Update(sf::RenderWindow* window)
	{


		if (current->getTextureRect().contains(sf::Mouse::getPosition(*window) - sf::Vector2i(current->getPosition())))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				if (!toggled)
				{
					if (state == ButtonState::Clicked)
					{
						state = ButtonState::Normal;
						current = normal;
					}
					else if (state == ButtonState::Normal)
					{
						state = ButtonState::Clicked;
						current = clicked;
					}
				}
				toggled = true;
			}
			else
			{
				toggled = false;
			}
		}
		return state;
	}

	void ToggleButton::Draw(sf::RenderWindow* window)
	{
		window->draw(*current);
	}

	void ToggleButton::ChangePos(sf::Vector2f pos)
	{
		this->pos = pos;
		current->setPosition(pos);
		normal->setPosition(pos);
		clicked->setPosition(pos);
	}

	void ToggleButton::SetState(GUI::ButtonState state)
	{
		if (state = GUI::ButtonState::Clicked)
		{
			this->state = GUI::ButtonState::Clicked;
			current = clicked;
		}
		else
		{
			this->state = GUI::ButtonState::Normal;
			current = normal;
		}
	}

	ToggleButton::~ToggleButton()
	{
	}

	Scroll::Scroll(std::string dir, sf::Vector2f pos, int size)
	{
		this->size = size;
		this->pos = pos;
		start_y = pos.y;
		tex.loadFromFile(dir);
		rect = sf::IntRect(0, 0, tex.getSize().x, tex.getSize().y);
		current = new sf::Sprite(tex, rect);
		current->setPosition(pos);
	}

	int Scroll::Update(sf::RenderWindow* window)
	{
		if (!pressed)
		{
			if (current->getTextureRect().contains(sf::Mouse::getPosition(*window) - sf::Vector2i(current->getPosition())))
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					pressed = true;
				}
			}
		}
		else
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				pos = sf::Vector2f(pos.x, sf::Mouse::getPosition(*window).y - ((int)tex.getSize().y / 2));
			}
			else
			{
				pressed = false;
			}
		}
		if (pos.y <= start_y)
		{
			pos.y = start_y;
		}
		if (pos.y >= start_y + size)
		{
			pos.y = start_y + size;
		}
		return pos.y - start_y;
	}

	void Scroll::Draw(sf::RenderWindow* window)
	{
		current->setPosition(pos);
		window->draw(*current);
	}

	Scroll::~Scroll()
	{
	}

}