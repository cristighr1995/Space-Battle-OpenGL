#include "Laborator3.h"

#include <vector>
#include <iostream>
#include <string>>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"

using namespace std;

typedef struct Bullet {
	Mesh* mBullet;
	float angularStep;
	float tx, ty;
	string name;
} Bullet;

typedef struct Enemy {
	Mesh* mEnemyShip;
	float angularStep;
	float tx, ty;
	string name;
	int type; // for scaling
	float speed;
	int remaining_lives;
	float scaleX, scaleY;
} Enemy;

typedef struct LifeStick {
	float tx, ty;
	Mesh* mStick;
	string name;
} LifeStick;

vector<Bullet> bulletVector;
vector<Enemy> enemyVector;
vector<LifeStick> lifeVector;
float enemy_ship_type;
int pieces_for_game_over_anim;
int pieces_for_shrink_anim;
float starting_color = 0.0f;
bool interval_changed = false;

Laborator3::Laborator3()
{
}

Laborator3::~Laborator3()
{
}

void Laborator3::Init()
{
	resolution = window->GetResolution();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	cameraInput->SetActive(false);
	srand(time(NULL));

	corner = glm::vec3(0, 0, 0);
	
	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;
	// initialize angularStep
	angularStep = 0;

	// start my ship in the middle of the screen
	starting_x = resolution.x / 2;
	starting_y = resolution.y / 2;
	// initialize tx and ty (the translation steps)
	translateX = starting_x;
	translateY = starting_y;

	// set speed for my ship movement
	my_ship_moving_speed = 3.5f;
	// set speed for the bullet
	bullet_speed = 6.0f;

	// init radius for enemy distance
	circle_distance_radius = resolution.x / 2 - 50;

	Mesh* ship = Object2D::CreateShip("my_ship", corner, ship_length_side, glm::vec3(0, 1, 1), true); // cyan color
	AddMeshToList(ship);
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(starting_x, starting_y);
	RenderMesh2D(meshes["my_ship"], shaders["VertexColor"], modelMatrix);

	float stick_width = 50;
	float stick_height = 7;
	float start_stick_x = resolution.x - player_nr_lives * stick_width / 2 - 50;
	float start_stick_y = resolution.y - 50;

	// initially player has 3 lives
	for (int i = 0; i < player_nr_lives; ++i) {
		LifeStick stick;
		stick.tx = start_stick_x;
		stick.ty = start_stick_y;
		start_stick_x += stick_width / 2;

		std::string name;
		name.append("stick_");
		name.append(to_string(i));
		stick.name = name;

		stick.mStick = Object2D::CreateBullet(name, corner, stick_width, stick_height, glm::vec3(1, 1, 0), true);
		AddMeshToList(stick.mStick);

		lifeVector.push_back(stick);
	}	

	interval_of_generation += interval_subtract_coef;
}

void Laborator3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator3::Update(float deltaTimeSeconds)
{	
	// calculate in how many pieces to divide for last animation
	pieces_for_game_over_anim = (int)(2.0f / deltaTimeSeconds);
	pieces_for_shrink_anim = (int)(0.25f / deltaTimeSeconds);

	// shrink the interval every a specific number of enemies which is set to 20 now
	if (player_score % nr_enemy_change_interval == 0 && !interval_changed) 
		if (interval_of_generation > MIN_INTERVAL_SECONDS) {
			interval_of_generation -= interval_subtract_coef;
			interval_changed = true;
		}
	// after changed interval, wait one enemy to kill after change variable
	if(player_score % nr_enemy_change_interval == 1) interval_changed = false;

	if (player_nr_lives == 0 && starting_color <= 1.0f) {
		// draw background color RED
		glClearColor(starting_color, 0.0f, 0.0f, 1.0f);
		starting_color += 1.0f / pieces_for_game_over_anim;
		glClear(GL_COLOR_BUFFER_BIT);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		modelMatrix *= Transform2D::Rotate(angularStep);
		RenderMesh2D(meshes["my_ship"], shaders["VertexColor"], modelMatrix);

		for (int i = 0; i < enemyVector.size(); ++i) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(enemyVector[i].tx, enemyVector[i].ty);
			modelMatrix *= Transform2D::Rotate(enemyVector[i].angularStep);
			RenderMesh2D(meshes[enemyVector[i].name], shaders["VertexColor"], modelMatrix);
		}
	}
	else if (player_nr_lives == 0) { // normally game should quit here
		// until player hits exit, the background color remains purely RED
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		modelMatrix *= Transform2D::Rotate(angularStep);
		RenderMesh2D(meshes["my_ship"], shaders["VertexColor"], modelMatrix);

		for (int i = 0; i < enemyVector.size(); ++i) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(enemyVector[i].tx, enemyVector[i].ty);
			modelMatrix *= Transform2D::Rotate(enemyVector[i].angularStep);
			RenderMesh2D(meshes[enemyVector[i].name], shaders["VertexColor"], modelMatrix);
		}
	}
	else {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		modelMatrix *= Transform2D::Rotate(angularStep);
		RenderMesh2D(meshes["my_ship"], shaders["VertexColor"], modelMatrix);

		// Render score sticks
		for (int i = 0; i < player_nr_lives; ++i) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(lifeVector[i].tx, lifeVector[i].ty);
			modelMatrix *= Transform2D::Rotate(M_PI / 2);
			RenderMesh2D(meshes[lifeVector[i].name], shaders["VertexColor"], modelMatrix);
		}

		// Render every bullet
		for (int i = 0; i < bulletVector.size(); ++i) {
			float tx = bulletVector[i].tx;
			float ty = bulletVector[i].ty;
			float as = bulletVector[i].angularStep;
			float tx_multiplyer = bullet_speed * cos(as);
			float ty_multiplyer = bullet_speed * sin(as);
			// knowing x1 and angle => x2 = x1 + L * cos(angle)
			// knowing y1 and angle => y2 = y1 + L * sin(angle)

			// modify tx, ty knowing the cadran
			if (as < M_PI / 2) { tx += tx_multiplyer; ty += ty_multiplyer; }
			if (as > M_PI / 2 && as < M_PI) { tx -= -tx_multiplyer; ty += ty_multiplyer; }
			if (as > M_PI && as < 3 * M_PI / 2) { tx -= -tx_multiplyer; ty -= -ty_multiplyer; }
			if (as > 3 * M_PI / 2 && as < 2 * M_PI) { tx += tx_multiplyer; ty -= -ty_multiplyer; }

			bulletVector[i].tx = tx;
			bulletVector[i].ty = ty;

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(tx, ty);
			modelMatrix *= Transform2D::Rotate(as);
			RenderMesh2D(meshes[bulletVector[i].name], shaders["VertexColor"], modelMatrix);
		}

		// erase bullets outside the frame
		vector<Bullet>::iterator it = bulletVector.begin();
		for (; it != bulletVector.end(); ) {
			if ((*it).tx < 0 || (*it).tx > resolution.x || (*it).ty < 0 || (*it).ty > resolution.y)
				it = bulletVector.erase(it);
			else
				++it;
		}

		// generate every interval an enemy
		int interval = (int)(Engine::GetElapsedTime() * 1000) % interval_of_generation;
		int error_eps = (int)(deltaTimeSeconds * 1000);
		if (interval <= error_eps) {
			how_many_enemies_added++;
			Enemy new_enemy;
			int min = (int)(translateX - circle_distance_radius);
			int max = (int)(translateX + circle_distance_radius);
			float ex = rand() % (max - min + 1) + min;
			float ey = rand() % resolution.y; // in case of uninitialized variable

			int bound = rand() % 2; // used to choose upper half or lower half of scren 
									// using sqrt( (ex - tx)^2 + (ey - ty)^2 ) = circle_radius 
									// generate random just ex and find ey for putting the enemy on the radius
			float radical = sqrt(pow(circle_distance_radius, 2) - pow((ex - translateX), 2));
			if (bound == 0) ey = translateY + radical;
			else ey = translateY - radical;

			new_enemy.tx = ex;
			new_enemy.ty = ey;

			std::string name;
			name.append("enemy_ship_");
			name.append(to_string(how_many_enemies_added));
			new_enemy.name = name;

			new_enemy.speed = rand() % enemy_ship_max_speed + 1;

			new_enemy.angularStep = GetEnemyAngularStep(ex, ey, translateX, translateY);

			new_enemy.scaleX = 1;
			new_enemy.scaleY = 1;

			enemy_ship_type = rand() % 2;
			new_enemy.type = enemy_ship_type;

			if (enemy_ship_type == BLUE_TYPE_SHIP) { // blue
				enemy_ship_color = glm::vec3(0, 0, 1);
				new_enemy.remaining_lives = 1;
			}
			else { // yellow
				enemy_ship_color = glm::vec3(1, 1, 0);
				new_enemy.remaining_lives = 2;
			}

			new_enemy.mEnemyShip = Object2D::CreateShip(name, corner, ship_length_side, enemy_ship_color, true);
			AddMeshToList(new_enemy.mEnemyShip);

			enemyVector.push_back(new_enemy);
		}

		// determine movement for the enemy ship
		for (int i = 0; i < enemyVector.size(); ++i) {
			float tx = enemyVector[i].tx;
			float ty = enemyVector[i].ty;
			float as = GetEnemyAngularStep(tx, ty, translateX, translateY);
			float tx_multiplyer = enemyVector[i].speed * cos(as);
			float ty_multiplyer = enemyVector[i].speed * sin(as);

			if (as < M_PI / 2) { tx += tx_multiplyer; ty += ty_multiplyer; }
			if (as > M_PI / 2 && as < M_PI) { tx -= -tx_multiplyer; ty += ty_multiplyer; }
			if (as > M_PI && as < 3 * M_PI / 2) { tx -= -tx_multiplyer; ty -= -ty_multiplyer; }
			if (as > 3 * M_PI / 2 && as < 2 * M_PI) { tx += tx_multiplyer; ty -= -ty_multiplyer; }

			enemyVector[i].tx = tx;
			enemyVector[i].ty = ty;
			enemyVector[i].angularStep = as;

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(tx, ty);
			modelMatrix *= Transform2D::Rotate(as);
			modelMatrix *= Transform2D::Scale(enemyVector[i].scaleX, enemyVector[i].scaleY);
			RenderMesh2D(meshes[enemyVector[i].name], shaders["VertexColor"], modelMatrix);
		}

		vector<int> indexes_to_erase_enemies;
		vector<int> indexes_to_erase_bullets;
		vector<Enemy> enemies_shrinked;

		// update in second frame the animation for shrinking
		for(int i = 0; i < enemyVector.size(); ++i)
			if (enemyVector[i].scaleX <= 0.99f && enemyVector[i].scaleX >= 0.5f) {
				enemyVector[i].scaleX -= 0.5f / pieces_for_shrink_anim;
				enemyVector[i].scaleY -= 0.5f / pieces_for_shrink_anim;
			}

		// check collision bullet -> enemy but also enemy -> my_ship
		for (int i = 0; i < enemyVector.size(); ++i) {
			float ex, ey, bx, by, distance;
			// check first collision with my_ship
			ex = enemyVector[i].tx;
			ey = enemyVector[i].ty;
			distance = sqrt(pow((ex - translateX), 2) + pow((ey - translateY), 2));
			if (distance <= ship_length_side) { // means one enemy has hit my ship
				player_nr_lives--;
				player_score++;
				indexes_to_erase_enemies.push_back(i); // add enemy to erase
				cout << "Score: " << player_score << endl;
				continue;
			}

			for (int j = 0; j < bulletVector.size(); ++j) {
				// check distance between bullet and center of enemy 
				// use a circle radius that is equal with the ship_length_side
				bx = bulletVector[j].tx;
				by = bulletVector[j].ty;

				distance = sqrt(pow((ex - bx), 2) + pow((ey - by), 2));
				if (distance <= ship_length_side) { // means bullet has hit the enemy
					player_score++;
					enemyVector[i].remaining_lives--;

					if (enemyVector[i].remaining_lives == 1) { // is yellow so need to shrink the ship
															   // create new enemy with color red and with exactly the same characteristics as old one
						Enemy enemy;
						enemy.angularStep = enemyVector[i].angularStep;
						enemy.name = enemyVector[i].name;
						enemy.mEnemyShip = Object2D::CreateShip(enemy.name, corner, ship_length_side, glm::vec3(1, 0, 0), true); // red color
						AddMeshToList(enemy.mEnemyShip);
						enemy.remaining_lives = enemyVector[i].remaining_lives;
						enemy.scaleX = 0.99f; // shrink the ship
						enemy.scaleY = 0.99f;
						enemy.speed = enemyVector[i].speed * 2; // double the speed
						enemy.tx = enemyVector[i].tx;
						enemy.ty = enemyVector[i].ty;
						enemy.type = enemyVector[i].type;
						enemies_shrinked.push_back(enemy); // remember to add these enemies back to enemyVector

						player_score--;
					}

					indexes_to_erase_enemies.push_back(i); // add enemy to erase
					indexes_to_erase_bullets.push_back(j); // add bullet to erase
					cout << "Score: " << player_score << endl;
					break;
				}
			}
		}

		for (int i = indexes_to_erase_enemies.size() - 1; i >= 0; --i)
			enemyVector.erase(enemyVector.begin() + indexes_to_erase_enemies[i]);
		for (int i = indexes_to_erase_bullets.size() - 1; i >= 0; --i)
			bulletVector.erase(bulletVector.begin() + indexes_to_erase_bullets[i]);
		for (int i = 0; i < enemies_shrinked.size(); ++i)
			enemyVector.push_back(enemies_shrinked[i]);
	}
}

float Laborator3::GetEnemyAngularStep(float ex, float ey, float tx, float ty) 
{
	float angle = 0;

	// for upper bound of y we add M_PI to change direction
	if (ex > tx && ey > ty)
		angle = M_PI + atan((ey - ty) / (ex - tx));

	if (ex < tx && ey > ty)
		angle = 2 * M_PI - atan((ey - ty) / (tx - ex));

	// for lower bound of y we subtract M_PI to change direction
	if (ex < tx && ey < ty)
		angle = atan((ty - ey) / (tx - ex));

	if (ex > tx && ey < ty)
		angle = -M_PI + 3 * M_PI / 2 + atan((ex - tx) / (ty - ey));

	return angle;
}

void Laborator3::FrameEnd()
{

}

void Laborator3::OnInputUpdate(float deltaTime, int mods)
{
	if (player_nr_lives != 0) {
		if (window->KeyHold(GLFW_KEY_W)) translateY += my_ship_moving_speed;
		if (window->KeyHold(GLFW_KEY_S)) translateY -= my_ship_moving_speed;

		if (window->KeyHold(GLFW_KEY_A)) translateX -= my_ship_moving_speed;
		if (window->KeyHold(GLFW_KEY_D)) translateX += my_ship_moving_speed;
	}
}

void Laborator3::OnKeyPress(int key, int mods)
{
	// add key press event
};

void Laborator3::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (player_nr_lives != 0) {
		int actualMouseY = window->GetResolution().y - mouseY;

		if (mouseX > translateX && actualMouseY > translateY)
			angularStep = atan((actualMouseY - translateY) / (mouseX - translateX));

		if (mouseX < translateX && actualMouseY > translateY)
			angularStep = M_PI - atan((actualMouseY - translateY) / (translateX - mouseX));

		if (mouseX < translateX && actualMouseY < translateY)
			angularStep = M_PI + atan((translateY - actualMouseY) / (translateX - mouseX));

		if (mouseX > translateX && actualMouseY < translateY)
			angularStep = 3 * M_PI / 2 + atan((mouseX - translateX) / (translateY - actualMouseY));
	}
};

void Laborator3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
		Bullet new_bullet;
		// bullet receives the ship position when pressed left click
		new_bullet.tx = translateX;
		new_bullet.ty = translateY;
		new_bullet.angularStep = angularStep;
		std::string name;
		name.append("bullet_");
		name.append(to_string(bulletVector.size()));
		new_bullet.name = name;

		new_bullet.mBullet = Object2D::CreateBullet(name, corner, bullet_width, bullet_heigth, glm::vec3(1, 1, 1), true);
		AddMeshToList(new_bullet.mBullet);

		bulletVector.push_back(new_bullet);
	}
};

void Laborator3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}