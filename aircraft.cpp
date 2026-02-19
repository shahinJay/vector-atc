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

	this->airspace = &airspace;
	

	if (!this->font.openFromFile(this->fontpath)) {
		std::cout << "ERROR loading font" << std::endl;
	}

}

void Aircraft::selfDestruct() {
	delete this;
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

float Aircraft::distance(sf::Vector2f vec1, sf::Vector2f vec2) {
	return sqrt((vec2.x - vec1.x) * (vec2.x - vec1.x) + (vec2.y - vec1.y) * (vec2.y - vec1.y));
}

int Aircraft::vec_to_angle(sf::Vector2f vec1, sf::Vector2f vec2) {
	float dx = vec2.x - vec1.x;
	float dy = vec2.y - vec1.y;

	float rad = std::atan2(dy, dx);
	float degrees = rad * this->rad_to_deg;

	if (degrees < 0)
		degrees += 360;

	int angle_to = static_cast<int>(std::round(degrees)) % 360;

	return angle_to; 
}


//---------------------------------------------------------------------------------------------
void Aircraft::assign_callsign(int ID) {
	this->callsign = "ABC" + std::to_string(ID);
}

//TAKEOFF / LANDING SEQUENCES -----------------------------------------------------------------

void Aircraft::takeoff() {

}

void Aircraft::land() {
	this->groundspeed = this->landing_speed;

	if (distance(this->target_faf.rw_position, this->position) < 10) {
		this->LANDED = true;
	}
}

void Aircraft::check_landing_conditions(Airspace::final_approach_fix faf) {
	float heading_diff = std::fmod(faf.heading - this->heading + 540.0f, 360.0f) - 180.0f;
	
	if ((distance(this->position, faf.position) < ils_dist_threshold))
	{
		if (std::fabs(heading_diff) <= ils_angle_threshold && this->groundspeed <= this->approach_speed) {
			this->position = this->target_faf.position;
			this->heading = this->target_faf.heading;
			this->LAND = true;
			this->states_to_change = { false, false, false, false, false };
		}
		else {
			std::cout << "CANT LAND, going around..." << std::endl;
		}
	}
}

//CONTROLS ------------------------------------------------------------------------------------
void Aircraft::change_heading() {
    float diff = std::fmod(this->target_heading - this->heading + 540.0f, 360.0f) - 180.0f;

    if (std::fabs(diff) <= this->turn_rate || std::fabs(diff) < 0.01f) {
        this->heading = std::fmod(this->target_heading + 360.0f, 360.0f);
        if (this->heading < 0.0f) this->heading += 360.0f;

        this->owned = false;
        this->change_required = false;
        return;
    }

    if (diff > 0.0f) {
        this->heading += this->turn_rate; // turn right (increasing heading)
    } else {
        this->heading -= this->turn_rate; // turn left (decreasing heading)
    }

    this->heading = std::fmod(this->heading + 360.0f, 360.0f);
    if (this->heading < 0.0f) this->heading += 360.0f;
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

//routing .................................................................
void Aircraft::direct_to_runway() {													// RUNWAY ROUTING
	bool found = false;

	sf::Vector2f faf_pos;
	Airspace::final_approach_fix faf;

	for (int i = 0; i < this->airspace->final_approach_fixes.size(); i++) {
		if (this->airspace->final_approach_fixes[i].rw_no == this->target_runway) {
			faf = this->airspace->final_approach_fixes[i];

			faf_pos = this->airspace->final_approach_fixes[i].position;
			found = true;
			break;
		}
	}
	if (!found)
		return;

	this->target_faf = faf;

	int locked_heading = vec_to_angle(faf_pos, this->position) - 180;

	this->target_heading = locked_heading;
	this->states_to_change[0] = true;
}

void Aircraft::direct_to_waypoint() {												// WAYPOINT ROUTING
	bool found = false;

	sf::Vector2f wp_pos;

	for (int i = 0; i < this->airspace->waypoints_array.size(); i++) {
		if (this->airspace->waypoints_array[i].wp_name == this->target_waypoint) {
			wp_pos = this->airspace->waypoints_array[i].position;
			found = true;
			break;
		}
	}
	if (!found)
		return;

	int locked_heading = vec_to_angle(wp_pos, this->position) - 180;

	this->target_heading = locked_heading;
	this->states_to_change[0] = true;
}
//..........................................................................

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

	//INSTRUCTION PROCESSING ...................................................................
	std::cout << this->callsign;
	std::cout << " ";
	for (int i = 1; i < parsed_command.size(); i++) {
		std::cout << parsed_command[i] << std::endl;
		switch (parsed_command[i][0]) {
			case 'H':	//HEADING
				this->states_to_change[0] = true;
				this->target_heading = (std::stoi(parsed_command[i].substr(1))-90);
				break;
			case 'S':	//SPEED
				this->states_to_change[1] = true;
				this->target_speed = std::stof(parsed_command[i].substr(1));
				break;
			case 'A':	//ALTITUDE
				this->states_to_change[2] = true;
				this->target_altitude = std::stof(parsed_command[i].substr(1));
				break;
			case 'D':	//WAYPOINT
				this->states_to_change[3] = true;
				this->target_waypoint = parsed_command[i].substr(1);
				break;
			case 'L':	//LANDING/ILS
				this->states_to_change[4] = true;
				this->target_runway = parsed_command[i].substr(1);
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
		
		if(this->states_to_change[1])
			change_speed();
		if(this->states_to_change[2])
			change_altitude();

		if (this->states_to_change[0])
			change_heading();

		if (this->states_to_change[3])
			direct_to_waypoint();

		this->velocity = heading_to_vector();
	}
	else if (collision_course)
		this->target_color = this->warning_color;
	else {
		this->states_to_change[0] = false;
		this->states_to_change[1] = false;
		this->states_to_change[2] = false;

		this->target_color = this->unowned_color;
	}

	//TAKEOFF / LANDING 
	if (this->states_to_change[4]) {
		direct_to_runway();
		check_landing_conditions(this->target_faf);
	}

	if (LAND) {
		this->target_color = landing_color;
		land();
		if (this->LANDED) return;
	}

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