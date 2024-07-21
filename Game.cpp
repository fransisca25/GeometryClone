#include "Game.h"
#include <iostream>
#include <fstream>
#include <cmath>


const double PI = 3.1415926535897932384626433832795028841971693993751058209;
const int COOLDOWNTIME = 120;
const int MAX_SPECIAL_WEAPON = 10;


Game::Game(const std::string& config)
{
	init(config);
}


void Game::init(const std::string& path)
{
	// read in config file 
	// use the premade PlayerConfig, EnemyConfig, BulletConfig variables
	std::string type, fFile;
	float wWidth, wHeight;
	int fps{}, mode, fSize, r, g, b;

	std::ifstream fin(path);

	// Handle error config file
	if (!fin)
	{
		std::cerr << "Cannot open file!\n";
		exit(-1);
	}

	while (fin >> type)
	{
		// When the type is window
		if (type == "Window")
		{
			fin >> wWidth >> wHeight >> fps >> mode;

			// Fullscreen mode
			if (mode == 0)
			{
				m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2");
			}
			else if (mode == 1)
			{
				m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2", sf::Style::Fullscreen);
			}

			m_window.setFramerateLimit(fps);
		}

		// When the type is Font
		if (type == "Font")
		{
			fin >> fFile >> fSize >> r >> g >> b;

			// Handle error font file
			if (!m_font.loadFromFile(fFile))
			{
				std::cerr << "Error load font file!\n";
			}

			m_text.setFont(m_font);
			m_text.setString("Score: " + std::to_string(m_score));
			m_text.setCharacterSize(fSize);
			m_text.setFillColor(sf::Color(r, g, b));
		}

		// When the type is Player
		if (type == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}

		// When the type is Enemy
		if (type == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}

		// When the type is Bullet
		if (type == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}
	spawnPlayer();
}


void Game::run()
{
	while (m_running)
	{
		m_entities.update();
		sUserInput();

		if (!m_paused)
		{
			sEnemySpawner();
			sUserInput();
			sMovement();
			sCollision();
			sLifespan();
		}

		sRender();
		m_currentFrame++;
	}
}


void Game::setPaused(bool paused)
{
	m_paused = paused;
}


void Game::spawnPlayer()
{
	auto player = m_entities.addEntity("player");

	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	player->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0, 0), 0.0f);
	player->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
	player->cInput = std::make_shared<CInput>();
	player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	m_player = player;
}


void Game::spawnEnemy()
{
	auto enemy = m_entities.addEntity("enemy");

	// Attempt to generate random float number (originally rand() does not support floating numbers)
	float eSpeed = m_enemyConfig.SMIN + float(rand()) / float(RAND_MAX) * (m_enemyConfig.SMAX - m_enemyConfig.SMIN);
	float eAngle = float(rand()) / float(RAND_MAX) * 2.0f * PI;

	float emx = rand() % m_window.getSize().x;
	float emy = rand() % m_window.getSize().y;
	
	Vec2 eDirection = Vec2(std::cos(eAngle), std::sin(eAngle));
	Vec2 eVelocity = eDirection * eSpeed;

	int enemyVertices = m_enemyConfig.VMIN + rand() % m_enemyConfig.VMAX;
	int enemyColorR = 0 + rand() % 256;
	int enemyColorG = 0 + rand() % 256;
	int enemyColorB = 0 + rand() % 256;

	enemy->cTransform = std::make_shared<CTransform>(Vec2(emx, emy), eVelocity, 0.0f);
	enemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR, enemyVertices, sf::Color(enemyColorR, enemyColorG, enemyColorB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
	enemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	enemy->cScore = std::make_shared<CScore>(enemyVertices * 100);

	// record when the most recent enemy was spawned
	m_lastEntitySpawnTime = m_currentFrame;
}


void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	int numVertices = e->cShape->circle.getPointCount();
	int shapeSize = e->cShape->circle.getRadius() / 2;
	sf::Color fillColor = e->cShape->circle.getFillColor();
	sf::Color outColor = e->cShape->circle.getOutlineColor();
	int smallThickness = e->cShape->circle.getOutlineThickness();
	float smallSpeed = e->cTransform->velocity.length();

	for (int i = 0; i <= numVertices; i++)
	{
		float smallAngle = (2 * PI / numVertices) * i;
		Vec2 smallDirection = Vec2(std::cos(smallAngle), std::sin(smallAngle));
		Vec2 smallVelocity = smallDirection * smallSpeed;

		auto smallEnemy = m_entities.addEntity("small enemy");

		smallEnemy->cTransform = std::make_shared<CTransform>(e->cTransform->pos, smallVelocity, 0.0f);
		smallEnemy->cShape = std::make_shared<CShape>(shapeSize, numVertices, fillColor, outColor, smallThickness);
		smallEnemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR / 2);
		smallEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
		smallEnemy->cScore = std::make_shared<CScore>(e->cScore->score * 2);	
	}
}


void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	auto bullet = m_entities.addEntity("bullet");

	Vec2 direction = target - entity->cTransform->pos;
	direction = direction.normalize();
	Vec2 velocity = direction * m_bulletConfig.S;

	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);
	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}


void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	int sVertices = entity->cShape->circle.getPointCount();
	int sSize = entity->cShape->circle.getRadius() / 2;
	sf::Color fColor = entity->cShape->circle.getFillColor();
	sf::Color oColor = entity->cShape->circle.getOutlineColor();
	int sThickness = entity->cShape->circle.getOutlineThickness();
	float sSpeed = entity->cTransform->velocity.length();

	// Check the number of existing special weapon 
	if (m_entities.getEntities("special weapon").size() >= MAX_SPECIAL_WEAPON) 
	{
		return; 
	}
	
	if (sSpeed == 0)
	{
		sSpeed = m_playerConfig.V;
	}

	for (int i = 0; i < sVertices; i++)
	{
		float pAngle = (2 * PI / sVertices) * i;
		Vec2 pDirection = Vec2(std::cos(pAngle), std::sin(pAngle));
		Vec2 pVelocity = pDirection * sSpeed;

		auto special = m_entities.addEntity("special weapon");

		special->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, pVelocity, 0.0f);
		special->cShape = std::make_shared<CShape>(sSize, sVertices, fColor, oColor, sThickness);
		special->cCollision = std::make_shared<CCollision>(m_playerConfig.CR / 2);
		special->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
	}
}



void Game::sMovement()
{
	m_player->cTransform->velocity = { 0, 0 };

	// Implement player movement
	if (m_player->cInput->up)
	{
		m_player->cTransform->velocity.y = -m_playerConfig.V;
	} 
	else if (m_player->cInput->down)
	{
		m_player->cTransform->velocity.y = m_playerConfig.V;
	}
	else if (m_player->cInput->left)
	{
		m_player->cTransform->velocity.x = -m_playerConfig.V;
	}
	else if (m_player->cInput->right)
	{
		m_player->cTransform->velocity.x = m_playerConfig.V;
	}
	else
	{
		m_player->cTransform->velocity = { 0, 0 };
	}

	// Entities movement update
	for (auto& e : m_entities.getEntities())
	{
		e->cTransform->pos += e->cTransform->velocity;
	}
}


void Game::sLifespan()
{
	for (auto& e : m_entities.getEntities())
	{
		// Entities with no lifespan
		if (!e->cLifespan)
		{
			continue;
		}

		// Scale alpha channel when the entities have lifespan and is active
		if (e->cLifespan && e->isActive())
		{
			e->cLifespan->remaining--;

			int alpha = (e->cLifespan->remaining * 255) / e->cLifespan->total;

			std::cout << "Entity tag: " << e->tag() << " Remaining lifespan: " << e->cLifespan->remaining << " Alpha: " << alpha << std::endl;

			// fading shapes fill color
			sf::Color efill = e->cShape->circle.getFillColor();
			efill.a = alpha;
			e->cShape->circle.setFillColor(efill);

			// fading shapes outline
			sf::Color eOutline = e->cShape->circle.getOutlineColor();
			eOutline.a = alpha;
			e->cShape->circle.setOutlineColor(eOutline);
		}

		// When the entities have lifespan and the time is up
		if (e->cLifespan && e->cLifespan->remaining <= 0)
		{
			e->destroy();
		}
	}
}


void Game::sCollision()
{
	// Check collision between wall and entities
	// right wall
	if (m_player->cTransform->pos.x > m_window.getSize().x - m_player->cCollision->radius - m_playerConfig.OT)
	{
		m_player->cTransform->pos = { m_window.getSize().x - m_player->cCollision->radius - m_playerConfig.OT, m_player->cTransform->pos.y };
		m_playerConfig.S = -m_playerConfig.S;
	}
	// left wall
	if (m_player->cTransform->pos.x < m_player->cCollision->radius + m_playerConfig.OT)
	{
		m_player->cTransform->pos = { m_player->cCollision->radius + m_playerConfig.OT, m_player->cTransform->pos.y };
		m_playerConfig.S = -m_playerConfig.S;
	}
	// top wall
	if (m_player->cTransform->pos.y < m_player->cCollision->radius + m_playerConfig.OT)
	{
		m_player->cTransform->pos = { m_player->cTransform->pos.x, m_player->cCollision->radius + m_playerConfig.OT };
		m_playerConfig.S = -m_playerConfig.S;
	}
	// bottom wall
	if (m_player->cTransform->pos.y > m_window.getSize().y - m_player->cCollision->radius - m_playerConfig.OT)
	{
		m_player->cTransform->pos = { m_player->cTransform->pos.x, m_window.getSize().y - m_player->cCollision->radius - m_playerConfig.OT };
		m_playerConfig.S = -m_playerConfig.S;
	}

	// Check collision between enemy and wall
	for (auto& enemy : m_entities.getEntities("enemy"))
	{
		// right wall
		if (enemy->cTransform->pos.x > m_window.getSize().x - enemy->cCollision->radius - m_enemyConfig.OT)
		{
			enemy->cTransform->pos = { m_window.getSize().x - enemy->cCollision->radius - m_enemyConfig.OT, enemy->cTransform->pos.y };
			enemy->cTransform->velocity.x = -enemy->cTransform->velocity.x;
			enemy->cTransform->velocity.y = -enemy->cTransform->velocity.y;
		}
		// left wall
		if (enemy->cTransform->pos.x < enemy->cCollision->radius + m_enemyConfig.OT)
		{
			enemy->cTransform->pos = { enemy->cCollision->radius + m_enemyConfig.OT, enemy->cTransform->pos.y };
			enemy->cTransform->velocity.x = -enemy->cTransform->velocity.x;
			enemy->cTransform->velocity.y = -enemy->cTransform->velocity.y;
		}
		// top wall
		if (enemy->cTransform->pos.y < enemy->cCollision->radius + m_enemyConfig.OT)
		{
			enemy->cTransform->pos = { enemy->cTransform->pos.x, enemy->cCollision->radius + m_enemyConfig.OT };
			enemy->cTransform->velocity.x = -enemy->cTransform->velocity.x;
			enemy->cTransform->velocity.y = -enemy->cTransform->velocity.y;
		}
		// bottom wall
		if (enemy->cTransform->pos.y > m_window.getSize().y - enemy->cCollision->radius - m_enemyConfig.OT)
		{
			enemy->cTransform->pos = { enemy->cTransform->pos.x, m_window.getSize().y - enemy->cCollision->radius - m_enemyConfig.OT };
			enemy->cTransform->velocity.x = -enemy->cTransform->velocity.x;
			enemy->cTransform->velocity.y = -enemy->cTransform->velocity.y;
		}
	}

	// Check collision between player and enemy	
	for (auto& e : m_entities.getEntities("enemy"))
	{
		float eDistance = m_player->cTransform->pos.dist(e->cTransform->pos);

		if (eDistance <= (m_playerConfig.CR + m_enemyConfig.CR + m_playerConfig.OT + m_enemyConfig.OT))
		{
			spawnSmallEnemies(e);
			e->destroy();
			m_player->destroy();
			spawnPlayer();
			break;
		}

		// Check collision between bullet and enemy
		for (auto& b : m_entities.getEntities("bullet"))
		{
			float bDistance = e->cTransform->pos.dist(b->cTransform->pos);

			if (bDistance <= (m_bulletConfig.CR + m_enemyConfig.CR + m_bulletConfig.OT + m_enemyConfig.OT))
			{
				m_score += e->cScore->score;
				m_text.setString("Score: " + std::to_string(m_score));
				spawnSmallEnemies(e);
				e->destroy();
				b->destroy();
				break;
			}
		}

		// Check collision between special weapon and enemy
		for (auto& s : m_entities.getEntities("special weapon"))
		{
			float sDistance = e->cTransform->pos.dist(s->cTransform->pos);

			if (sDistance <= ((m_playerConfig.CR / 2) + m_enemyConfig.CR + m_bulletConfig.OT + m_enemyConfig.OT))
			{
				m_score += e->cScore->score;
				m_text.setString("Score: " + std::to_string(m_score));
				spawnSmallEnemies(e);
				e->destroy();
				s->destroy();
				break;
			}
		}
	}

	// Check Collision between small enemy and player
	for (auto& s : m_entities.getEntities("small enemy"))
	{
		float sDistance = m_player->cTransform->pos.dist(s->cTransform->pos);

		if (sDistance <= (m_playerConfig.CR / 2 + m_enemyConfig.CR / 2 + m_playerConfig.OT + m_enemyConfig.OT))
		{
			s->destroy();
			m_player->destroy();
			spawnPlayer();
			break;
		}

		// Check collision between bullet and enemy
		for (auto& b : m_entities.getEntities("bullet"))
		{
			float bsDistance = s->cTransform->pos.dist(b->cTransform->pos);

			if (bsDistance <= (m_bulletConfig.CR / 2 + m_enemyConfig.CR / 2 + m_bulletConfig.OT + m_enemyConfig.OT))
			{
				m_score += s->cScore->score;
				m_text.setString("Score: " + std::to_string(m_score));
				s->destroy();
				b->destroy();
				break;
			}
		}

		// Check collision between special weapon and enemy
		for (auto& special : m_entities.getEntities("special weapon"))
		{
			float specialDistance = s->cTransform->pos.dist(special->cTransform->pos);

			if (specialDistance <= (m_playerConfig.CR / 2 + m_enemyConfig.CR / 2 + m_bulletConfig.OT + m_enemyConfig.OT))
			{
				m_score += s->cScore->score;
				m_text.setString("Score: " + std::to_string(m_score));
				s->destroy();
				special->destroy();
				break;
			}
		}
	}

	// Check collision between enemy and enemy
	auto enemies = m_entities.getEntities("enemy");

	for (int i = 0; i < enemies.size(); i++)
	{
		for (int j = i + 1; j < enemies.size(); j++)
		{
			float eeDistance = enemies[i]->cTransform->pos.dist(enemies[j]->cTransform->pos);

			if (eeDistance <= ((m_enemyConfig.CR * 2) + (m_enemyConfig.OT * 2)))
			{
				Vec2 normCollision = (enemies[i]->cTransform->pos - enemies[j]->cTransform->pos).normalize();
				Vec2 temp = enemies[i]->cTransform->velocity;
				enemies[i]->cTransform->velocity = enemies[j]->cTransform->velocity;
				enemies[j]->cTransform->velocity = temp;

				float overlap = ((m_enemyConfig.CR * 2) + (m_enemyConfig.OT * 2)) - eeDistance;
				Vec2 separate = normCollision * (overlap / 2.0f);
				enemies[i]->cTransform->pos += separate;
				enemies[j]->cTransform->pos -= separate;
			}
		}
	}
}


void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEntitySpawnTime >= m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}


void Game::sRender()
{
	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		if (e->cShape)
		{
			// Set the position of the shape based on the entity position
			e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

			// Increment the angle by a fixed amount each frame
			e->cTransform->angle += 5.0f;
			e->cShape->circle.setRotation(e->cTransform->angle);

			// Draw the entity
			m_window.draw(e->cShape->circle);
		}
	}

	// Cooldown special weapon
	if (m_cooldown > 0) {
		m_cooldown--;
	}

	m_window.draw(m_text);
	m_window.display();
}


void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// this event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// this event triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W Key Pressed\n";
				m_player->cInput->up = true;
				break;

			case sf::Keyboard::A:
				std::cout << "A Key Pressed\n";
				m_player->cInput->left = true;
				break;

			case sf::Keyboard::D:
				std::cout << "D Key Pressed\n";
				m_player->cInput->right = true;
				break;

			case sf::Keyboard::S:
				std::cout << "S Key Pressed\n";
				m_player->cInput->down = true;
				break;

			case sf::Keyboard::P:
				std::cout << "PAUSE\n";
				setPaused(!m_paused);
				break;

			default: break;
			}
		}

		// this event triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W Key Released\n";
				m_player->cInput->up = false;
				break;

			case sf::Keyboard::A:
				std::cout << "A Key Released\n";
				m_player->cInput->left = false;
				break;

			case sf::Keyboard::D:
				std::cout << "D Key Released\n";
				m_player->cInput->right = false;
				break;

			case sf::Keyboard::S:
				std::cout << "S Key Released\n";
				m_player->cInput->down = false;
				break;

			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
				spawnSpecialWeapon(m_player);
				m_cooldown = COOLDOWNTIME;
			}	
		}
	}
}