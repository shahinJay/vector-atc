#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

#include "airspace.h"
#include "aircraft.h"

Aircraft::Aircraft(Airspace& airspace, int ID, sf::Vector2f position, float heading, float groundspeed, float altitude) {
	assign_callsign(ID);
	std::cout << this->callsign << std::endl;
	this->position = position;

	this->heading = heading;
	this->groundspeed = groundspeed;
	this->altitude = altitude;

	this->target_heading = this->heading;
	this->target_speed = this->groundspeed;
	this->target_altitude = this->altitude;

	this->velocity = heading_to_vector();
	

	if (!this->font.openFromFile(this->fontpath)) {
		std::cout << "ERROR loading font" << std::endl;
	}

}

//MATH----------------------------------------------------------------------------------------
sf::Vector2f Aircraft::heading_to_vector() {
	return sf::Vector2f(cos(this->heading * this->deg_to_rad) * this->groundspeed, sin(this->heading * this->deg_to_rad)*this->groundspeed);
}

float Aircraft::random_range(float lower, float upper){
	 static std::random_device rd;
	 static std::mt19937 gen(rd());
	 std::uniform_real_distribution<float> dist(lower, upper);

	 return dist(gen);
}
//---------------------------------------------------------------------------------------------
void Aircraft::assign_callsign(int ID) {
	this->callsign = "ABC" + std::to_string(ID);

}
//CONTROLS ------------------------------------------------------------------------------------
void Aircraft::change_heading() {
	int angle_diff = static_cast<int>(this->target_heading - this->heading)%360;
	if (angle_diff > 0) {
		this->heading += this->turn_rate; //TURN RIGHT
	}
	else if (angle_diff < 0) {
		this->heading -= this->turn_rate; //TURN LEFT
	}
	else {
		this->owned = false;
		this->change_required = false;
		return;
	}
}

void Aircraft::change_speed() {
	if (this->target_speed > this->groundspeed){
		this->groundspeed += accel_rate;
		}
	else if (this->target_speed < this->groundspeed) {
		this->groundspeed -= accel_rate;
	}
	else {
		this->owned = false;
		this->change_required = false;
		return;
	}
}

void Aircraft::change_altitude() {
	if (this->target_altitude > this->altitude) {
		this->altitude += climb_rate;
	}
	else if (this->target_altitude < this->altitude) {
		this->altitude -= climb_rate;
	}
	else {
		this->owned = false;
		this->change_required = false;
		return;
	}
}
//----------------------------------------------------------------------------------------------

//COMMAND --------------------------------------------------------------------------------------

void Aircraft::parse_command(std::string command, std::vector<std::string>& parsed_command) {
	std::stringstream ss(command);
	std::string word;

	while (ss >> word) {
		parsed_command.push_back(word);
	}
}

void Aircraft::listen(std::string command) {
	
	std::vector<std::string> parsed_command;

	parse_command(command, parsed_command);

	if (parsed_command[0] == this->callsign) {
		this->owned = true;
		this->change_required = true;
	}
	else {
		this->owned = false;
		return;
	}

	//INSTRUCTION PROCESSING -------------------------------------------------------------------
	std::cout << this->callsign;
	std::cout << " ";
	for (int i = 1; i < parsed_command.size(); i++) {
		std::cout << parsed_command[i] << std::endl;
		switch (parsed_command[i][0]) {
			case 'H':
				this->state_to_change = 0;
				this->target_heading = (std::stoi(parsed_command[i].substr(1))-90);
				break;
			case 'S':
				this->state_to_change = 1;
				this->target_speed = std::stof(parsed_command[i].substr(1));
				break;
			case 'A':
				this->state_to_change = 2;
				this->target_altitude = std::stof(parsed_command[i].substr(1));
				break;
			default:
				break;
		}
	}
}
//------------------------------------------------------------------------------------------------
//UPDATES-----------------------------------------------------------------------------------------

void Aircraft::update_label(sf::RenderWindow& window) {

	sf::Text callsign(font);
	callsign.setString(this->callsign);
	callsign.setCharacterSize(8);
	callsign.setFillColor(target_color);
	callsign.setPosition(this->label_point);

	window.draw(callsign);

	sf::Text heading(font);
	heading.setString(std::to_string((int)std::round(this->heading + 90) % 360));
	heading.setCharacterSize(8);
	heading.setFillColor(target_color);
	sf::Vector2f heading_pos = sf::Vector2f(this->label_point.x, this->label_point.y + 10);
	heading.setPosition(heading_pos);

	window.draw(heading);

	sf::Text altitude(font);
	altitude.setString(std::to_string((int)std::round(this->altitude)));
	altitude.setCharacterSize(8);
	altitude.setFillColor(target_color);
	sf::Vector2f altitude_pos = sf::Vector2f(this->label_point.x + 15, heading_pos.y);
	altitude.setPosition(altitude_pos);

	window.draw(altitude);
	
}

void Aircraft::draw(sf::RenderWindow& window) {

	sf::VertexArray points(sf::PrimitiveType::LineStrip);
	sf::Vector2f curr;


	//UPDATE STATES------------------------------------------------------------------------------------
	if (this->owned || this->change_required) {
		this->target_color = this->owned_color;

		switch (this->state_to_change) {
		case 0:
			change_heading();
			break;
		case 1:
			change_speed();
			break;
		case 2:
			change_altitude();
			break;
		default:
			break;
		}
		this->velocity = heading_to_vector();
	}
	else if (collision_course)
		this->target_color = this->warning_color;
	else
		this->target_color = this->unowned_color;

	//DRAWING THE TARGET------------------------------------------------------------------------------
	for (int i = 0; i < 4; i++) {
		curr.x = this->position.x + this->rect_size * cos((i * 90 * deg_to_rad) + (45 * deg_to_rad));
		curr.y = this->position.y + this->rect_size * sin((i * 90 * deg_to_rad) + (45 * deg_to_rad));

		points.append(sf::Vertex(curr, this->target_color));
	}

	curr.x = this->position.x + this->rect_size * cos(45 * deg_to_rad);
	curr.y = this->position.y + this->rect_size * sin(45 * deg_to_rad);

	points.append(sf::Vertex(curr, this->target_color));

	curr.x = this->position.x + this->rect_size * cos(315 * deg_to_rad);
	curr.y = this->position.y + this->rect_size * sin(315 * deg_to_rad);

	points.append(sf::Vertex(curr, this->target_color));
	
	curr.x = this->position.x + this->rect_size * 5 * cos(315 * deg_to_rad);
	curr.y = this->position.y + this->rect_size * 5 * sin(315 * deg_to_rad);

	this->label_point = curr;

	points.append(sf::Vertex(curr, this->target_color));

	window.draw(points);

	update_label(window);
	//-------------------------------------------------------------------------------------------------

}