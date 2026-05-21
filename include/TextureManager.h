#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
class TextureManager {
private:
	std::unordered_map<std::string, sf::Texture>textures;
public:
	void load(const std::string& name, const std::string& filepath) {
		sf::Texture tex;
		if (tex.loadFromFile(filepath)) {
			textures[name] = tex;
		}
		else {
			std::cout << "Error in texture loading" << filepath << " " << name;
		}
	}
	const sf::Texture& get(const std::string& name)const {
		return textures.at(name);
	}
	
};
